/* *********************************************************************** *
 * File   : channel.cc                                Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : Unknown.                                                       *
 *                                                                         *
 * Purpose: Unknown.                                                       *
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

#include <assert.h>

#ifdef LINUX
#include <unistd.h>
#endif

#include "channel.h"
#include "inter.h"
#include "system.h"
#include "updown.h"

#include "slide.h"
#include "log.h"
#include "arg.h"
#include "client.h"

#ifdef LINUX
#include "select.h"
#else
#include "timer.h"
#endif

// We need to discriminate between Server & Client mode!
// The server will ask "Press ESC to enter", and the Client will
// attempt to send the "CLIENT" login sequence
//

#ifdef LINUX
cChannelManager ChannelManager(TRUE);
#else
cChannelManager ChannelManager(FALSE); // DOS
#endif

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*                             CHANNEL CLASS                                */
/*                                                                          */
/* ------------------------------------------------------------------------ */

cChannel::cChannel(ubit8 n)
{
   nChannel = n;
}


cChannel::~cChannel()
{
   ChannelManager.Free(nChannel);
}


sbit32 cChannel::Send(ubit8 *data, ubit32 len)
{
   return ChannelManager.MngrTX(data, len, nChannel);
}


void cChannel::Thread(void)
{
}


const char * cChannel::Status(void)
{
   return "No Status Available for this Channel.\n\r";
}


/* ------------------------------------------------------------------------ */
/*                                                                          */
/*                        CHANNEL MANAGER CLASS                             */
/*                                                                          */
/* ------------------------------------------------------------------------ */


cChannelManager::cChannelManager(ubit8 bArg)
{
   for (int i=0; i < 256; i++)
     BaseChannel[i] = NULL;

   bServer = bArg;
}


void cChannelManager::WriteQueue(void)
{
   cQueueElem *qE;
   static int blocked = FALSE;

   if (blocked)
     return;

   blocked = TRUE;

   while (!qTx.IsEmpty())
   {
      qE = (cQueueElem *) qTx.PeekHead();

      if (Slw->Transmit(qE->Data(), qE->Bytes()) == 0)
      {
	 qE = qTx.GetHead();
	 delete qE;
      }
      else
	break;
   }

   blocked = FALSE;
}


sbit32 cChannelManager::MngrTX(ubit8 *data, ubit32 len, ubit8 nChannel)
{
   int n = 0;
   static int blocked = 0;

   assert(len > 0);

   if (bDirectConnect)
   {
      blocked++;

      assert(blocked == 1);

      if (Slw->Transmit((ubit8 *) "", 1) == -1)
      {
	 Slw->Unhook();
	 blocked--;
	 return -1;
      }

      if (Slw->Transmit(data, len) == -1)
      {
	 Slw->Unhook();
	 blocked--;
	 return -1;
      }

      n = len;

#ifdef LINUX      
      if (data[len-1] == '+')  // Avoid the +++ sequence to mess up modems
      {
         if (Slw->Transmit((ubit8 *) "\b+", 2) == -1)
         {
            Slw->Unhook();
            n = -1;
         }
      }
#endif
      blocked--;
   }
   else
   {
      ubit8 aData[SLW_MAX_DATA];
      int tlen;

      aData[0] = nChannel;

      n = len;

      do
      {
	 if (len > SLW_MAX_DATA-1)
	   tlen = SLW_MAX_DATA-1;
	 else
	   tlen = len;

	 memcpy(&aData[1], data, tlen);
	    
	 len  -= tlen;
	 data += tlen;
	    
	 cQueueElem *qE = new cQueueElem(aData, tlen + 1);
	 qTx.Append(qE);
      }
      while(len > 0);

      WriteQueue();
   }

   return n;
}



void cChannelManager::MngrRX(ubit8 *data, ubit32 len)
{
   assert(len > 1);

   if (BaseChannel[data[0]] == NULL)
   {
      pLogFile->Log("Received data for non-existing channel %d.\n", data[0]);
      return;
   }

   BaseChannel[data[0]]->Receive(&data[1], len - 1);
}


void cChannelManager::MngrRX(ubit8 *data, ubit32 len, ubit8 nChannel)
{
   assert(len >= 1);

   if (BaseChannel[nChannel] == NULL)
   {
      pLogFile->Log("Received data for non-existing channel %d.\n", data[0]);
      return;
   }

   BaseChannel[nChannel]->Receive(data, len);
}

void cChannelManager::Threads(void)
{
   WriteQueue();

   for (int i=0; i < 256; i++)
     if (BaseChannel[i] != NULL)
       BaseChannel[i]->Thread();
}




cChannel *cChannelManager::Alloc(ubit8 nService, char *pData)
{
   ubit8 i, min;
   int max;

   if (bServer)
   {
      min = 0;
      max = 128;
   }
   else
   {
      min = 128;
      max = 256;
   }

   for (i = min; i < max; i++)
     if (BaseChannel[i] == NULL)
       break;

   if (i >= max)
     return NULL;

   return Set(nService, i, pData);
}


cChannel *cChannelManager::Set(ubit8 nService, ubit8 nChn, char *pData)
{
   assert(BaseChannel[nChn] == NULL);

   switch (nService)
   {
     case SERVICE_TEXT:
      BaseChannel[nChn] = new cText(nChn);
      break;

     case SERVICE_SYSTEM:
      BaseChannel[nChn] = new cSystem(nChn);
      break;

     case SERVICE_UPLOAD:
      BaseChannel[nChn] = new cUpload(nChn, pData);
      break;
      
     case SERVICE_DOWNLOAD:
      BaseChannel[nChn] = new cDownload(nChn, pData);
      break;
      

     default:
      abort();
   }

   return BaseChannel[nChn];
}


void cChannelManager::Free(ubit8 nChannel)
{
   assert(BaseChannel[nChannel] != NULL);

   BaseChannel[nChannel] = NULL;
}


cChannel * cChannelManager::ChannelPtr(ubit8 nChn)
{
   return BaseChannel[nChn];
}


void cChannelManager::Close(void)
{
   for (int i=0; i < 256; i++)
     if (BaseChannel[i])
     {
        delete BaseChannel[i];
        BaseChannel[i] = NULL;
     }
}

