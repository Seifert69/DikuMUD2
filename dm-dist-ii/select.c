/* *********************************************************************** *
 * File   : select.cc                                 Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert                                                        *
 *                                                                         *
 * Purpose: Select hooks & buffer handling for sockets                     *
 * Bugs   : Unknown.                                                       *
 * Status : Unpublished.                                                   *
 *                                                                         *
 * Copyright (C) Valhalla (This work is unpublished).                      *
 *                                                                         *
 * This work is a property of:                                             *
 *                                                                         *
 *        Valhalla I/S                                                     *
 *        Noerre Soegade 37A, 4th floor                                    *
 *        1370 Copenhagen K.                                               *
 *        Denmark                                                          *
 *                                                                         *
 * This is an unpublished work containing Valhalla confidential and        *
 * proprietary information. Disclosure, use or reproduction without        *
 * authorization of Valhalla is prohobited.                                *
 * *********************************************************************** */

#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

#include "select.h"
#include "utility.h"

cCaptainHook CaptainHook;

/* ------------------------------------------------------------------- */
/*                                HOOK                                 */
/* ------------------------------------------------------------------- */

cHook::cHook(void)
{
   fd = -1;
}


cHook::~cHook(void)
{
   Unhook();
}

int cHook::tfd(void)
{ 
   return fd;
}

int cHook::IsHooked(void)
{
   return fd != -1;
}


void cHook::Unhook(void)
{
   if (IsHooked())
     CaptainHook.Unhook(this);

   fd = -1;
}


void cHook::PushWrite(void)
{
   int sofar, len;
   int thisround;
   ubit8 buf[1460];

   if (!IsHooked())
     return;

   while (!qTX.IsEmpty())
   {
      len = MIN(sizeof(buf), qTX.Bytes());

      qTX.CutCopy(buf, len);

      sofar = 0;

      for (;;)
      {
	 thisround = write(fd, buf + sofar, len - sofar);

	 if (thisround == 0)
	 {
	    slog(LOG_ALL, 0, "PushWrite (%d): Write to socket EOF", fd);
	    Unhook();
	    return;
	 }
	 else if (thisround < 0)
	 {
	    if (errno == EWOULDBLOCK) /* I'd rather this didn't happen */
	    {
	       qTX.Prepend(new cQueueElem(buf+sofar, len-sofar));
	       return;
	    }
	    
	    slog(LOG_ALL, 0, "PushWrite (%d): Write to socket, error %d",
		 fd, errno);
	    Unhook();
	    return;
	 }

	 if (thisround < len - sofar)
	 {
	    sofar += thisround;
	    qTX.Prepend(new cQueueElem(buf+sofar, len-sofar));
	    return;
	 }

	 sofar += thisround;

	 if (sofar >= len)
	   break;
      }
   }
}

void cHook::Write(ubit8 *pData, ubit32 nLen, int bCopy)
{
   if (nLen <= 0)
     return;

   if (!IsHooked())
   {
      if (!bCopy)
	free(pData);
      return;
   }

   qTX.Append(new cQueueElem(pData, nLen, bCopy));

   PushWrite();
}


/* ------------------------------------------------------------------- */
/*                            CAPTAIN HOOK                             */
/* ------------------------------------------------------------------- */

cCaptainHook::cCaptainHook(void)
{
   signal(SIGPIPE, SIG_IGN);  // Or else pipe fucks the whole thing up...

   for (int i=0; i < 256; i++)
     pfHook[i] = NULL;
   
   nTop = 0;
   nMax = 0;
}


cCaptainHook::~cCaptainHook(void)
{
   Close();
}


void cCaptainHook::Close(void)
{
   for (int i=0; i < 256; i++)
     if (pfHook[i])
       pfHook[i]->Unhook();
   
   nTop = 0;
   nMax = 0;
}

void cCaptainHook::Hook(int nHandle, cHook *hook)
{
   static int newid = 0;

   assert(pfHook[nHandle] == NULL);
   assert(hook->fd == -1);

   pfHook[nHandle]   = hook;
   hook->fd          = nHandle;
   hook->id          = newid++;

   hook->qTX.Flush();
   hook->qRX.Flush();

   nIdx[nTop]        = nHandle;

   nTop++;

   if (nHandle > nMax)
     nMax = nHandle;

   // DEBUG("HOOKED: Fd %d, Flags %d, Max %d\n", nHandle, nFlag, nMax);
}


void cCaptainHook::Unhook(cHook *hook)
{
   int i;
   int nHandle = hook->fd;

   assert(pfHook[nHandle] == hook);

   i = close(nHandle);
   if (i < 0)
     slog(LOG_ALL, 0, "Captain Hook: Close error %d.", errno);

   for (i = 0; i < nTop; i++)
   {
      if (nIdx[i] == nHandle)
      {
	 nIdx[i]  = nIdx[nTop-1];
	 nTop--;
	 break;
      }
   }

   pfHook[nHandle]->id = -1;
   pfHook[nHandle]     = NULL;

   nMax = 0;
   for (i = 0; i < nTop; i++)
   {
      if (nIdx[i] > nMax)
	nMax = nIdx[i];
   }
}

// If an unhook is performed during the wait, the possible select may be
// postponed until the next call to wait().
//
int cCaptainHook::Wait(struct timeval *timeout)
{
   int n, lmax;

   /* The following two are used in Wait() because the Input & Write
      can cause any descriptor to become unhooked. It is then the job
      of the Unhook to update this table of descriptors accordingly */

   int nTable[256], nId[256];
   int nTableTop;
   
   memcpy(nTable, nIdx, sizeof(int) * nTop);
   nTableTop = nTop;

   FD_ZERO(&read_set);
   FD_ZERO(&write_set);
   
   for (int i = 0; i < nTableTop; i++)
   {
      nId[i] = pfHook[nTable[i]]->id;

      assert(nId[i] != -1);

      FD_SET(nTable[i], &read_set);

      if (!pfHook[nTable[i]]->qTX.IsEmpty())
	FD_SET(nTable[i], &write_set);
   }

   n = select(nMax + 1, &read_set, &write_set, NULL, timeout);

   if (n == -1)
   {
      // Do not set to zero, it means that a timeout occurred.
      //
      if (errno == EAGAIN)
	n = 1;
      else if (errno == EINTR)
      {
	 //slog(LOG_ALL, 0, "CaptainHook: Select Interrupted.\n");
	 n = 1;
      }
      else
	slog(LOG_ALL, 0, "CaptainHook: Select error %d.\n", errno);
   }
   else if (n > 0)
   {
      int nFlag, tmpfd, i;

      /* We need to do this the hard way, because nTable[] can be
	 changed radically by any sequence of read or write */

      for (i=0; i < nTableTop; i++)
      {
	 nFlag = 0;
	 tmpfd = nTable[i];

	 if (FD_ISSET(tmpfd, &read_set))
	   SET_BIT(nFlag, SELECT_READ);
	    
	 if (FD_ISSET(tmpfd, &write_set))
	   SET_BIT(nFlag, SELECT_WRITE);
	 
	 cHook *pfTmpHook = pfHook[tmpfd];

	 /* It could have been unhooked by any previous sequence of
	    Input() or Write() sequences */

	 if (nFlag && pfTmpHook)
	 {
	    int nTmpid = pfTmpHook->id;

	    if ((pfTmpHook == pfHook[tmpfd]) &&	(pfTmpHook->id == nId[i]))
	    {
	       if (nFlag & (SELECT_READ | SELECT_EXCEPT))
		 pfTmpHook->Input(nFlag);
	    }

	    if ((pfTmpHook == pfHook[tmpfd]) && (pfTmpHook->id == nId[i]))
	    {
	       if (nFlag & SELECT_WRITE)
		 pfTmpHook->PushWrite();
	    }
	 }
      }
   }

   return n;
}



