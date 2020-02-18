/* *********************************************************************** *
 * File   : system.cc                                 Part of Valhalla MUD *
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

#include <ctype.h>

#include "essential.h"
#include "inter.h"
#include "serial.h"
#include "system.h"
#include "str_parse.h"

#ifdef LINUX
#include <unistd.h>
#include "arg.h"
#endif


#define STATE_NORMAL  (1)
#define STATE_GETINFO (2)

class cSystem *System = NULL;

#define SYS_CLOSE    (2)

/* ========================================================================= */
/*                               SYSTEM                                      */
/* ========================================================================= */


cSystem::cSystem(ubit8 nChn) : cChannel(nChn)
{
   assert(System == NULL);
   
   StatusBuf[0] = 0;
   nState = STATE_NORMAL;
   System = this;
   nPriorityQueue = 1; // Lowest transfer
   nPriorityDelay = 1; // Lowest transfer
   nPrioritySize  = 5; // Largest size

   SetPriority();
}

cSystem::~cSystem()
{
   System = NULL;
}

void cSystem::SystemGatherInfo(ubit8 *data, int len)
{
   int n = strlen(StatusBuf);

   assert(n + len + 1 < (int) sizeof(StatusBuf));

   memcpy(&StatusBuf[n], data, len);

   StatusBuf[n + len] = 0;

   if (data[len-1] == 0)
     nState = STATE_NORMAL;
}


const char * cSystem::Status(void)
{
   if (nState == STATE_GETINFO)
     return "Still collecting data from remote host.\n\r";
   else
     return StatusBuf;
}


void cSystem::SetPriority(void)
{
   switch (nPriorityQueue)
   {
     case 1:
      nQueue = 1;
      break;

     case 2:
      nQueue = 2;
      break;

     case 3:
      nQueue = 4;
      break;

     case 4:
      nQueue = 8;
      break;

     case 5:
      nQueue = (NO_BUFS+1);
      break;
   }

   switch (nPriorityDelay)
   {
     case 1:
      nDelay = 999999;
      break;
     case 2:
      nDelay = 600000;
      break;
     case 3:
      nDelay = 200000;
      break;
     case 4:
      nDelay = 100000;
      break;
     case 5:
      nDelay = 50000;  // 1 / 20th second
      break;
   }


   switch (nPrioritySize)
   {
     case 1:
      nSize  = 32;
      break;
     case 2:
      nSize  = 64;
      break;
     case 3:
      nSize  = 128;
      break;
     case 4:
      nSize  = 192;
      break;
     case 5:
      nSize  = (SLW_MAX_DATA - 1);
      break;
   }
}

void strip_trailing_blanks(char *str)
{
   if (!*str)			/* empty string: return at once      */
     return;

   for ( ; *str; ++str)		/* wind to end of string             */
     ;

   if (!isspace(*--str))	/* Not a spaceterminated string      */
     return;			/* This is mainly for `inter-code' strings */

   while (isspace(*--str))	/* rewind to last nonspace character */
     ;

   *++str = '\0';		/* step ahead and end string         */
}


void cSystem::SendDir(void)
{
   char Buf[SLW_MAX_DATA + 10], TmpBuf[SLW_MAX_DATA + 10];
   FILE *f;

   Buf[0] = SYS_FILEINFO;
   Buf[1] = 0;

   f = fopen(DIR_FILE, "rb");
   if (f == NULL)
   {
      strcpy(&Buf[1], "No such directory.");
      Send((ubit8 *) Buf, strlen(Buf) + 1);
      return;
   }

   for (;;)
   {
      if (fgets(TmpBuf, SLW_MAX_DATA-2, f) == NULL)
	break;

      strip_trailing_blanks(TmpBuf);
      strcpy(&Buf[1], TmpBuf);
      Send((ubit8 *) Buf, strlen(Buf) + 1);
   }

   fclose(f);
}

void cSystem::CD(char *data)
{
   char Buf[256];
   char *c;
   int n;

   Buf[0] = SYS_LOGINFO;

   data = skip_blanks(data);

   if (*data == '/')
   {
      while (*data == '/')
	data++;
      chdir(arg.pFtpDir); // Root directory
   }

   n = chdir(data);
   if (n != 0)
   {
      strcpy(&Buf[1], "No such directory.");
      Send((ubit8 *) Buf, strlen(Buf) + 1);
      return;
   }

   c = getcwd(NULL, 0);

   if (c == NULL)
   {
      sprintf(&Buf[1], "getcwd error %d", errno);
      Send((ubit8 *) Buf, strlen(Buf) + 1);
      return;
   }

   if (strncmp(c, arg.pFtpDir, strlen(arg.pFtpDir)) != 0)
   {
      /* Not within the specified limits */
      chdir(arg.pFtpDir);
      strcpy(&Buf[1], "CD: /");
      Send((ubit8 *) Buf, strlen(Buf) + 1);
      free(c);
      return;
   }

   sprintf(&Buf[1], "CD: %s", data);
   Send((ubit8 *) Buf, strlen(Buf) + 1);
   free(c);
}


void cSystem::SystemNormal(ubit8 *data, int len)
{
   char Buf[1024];
   int n;
   FILE *f;

   switch (data[0])
   {
     case SYS_PRIORITY:
      assert(data[1] >=1 && data[1] <= 5);
      nPriorityQueue = data[1];
      assert(data[2] >=1 && data[2] <= 5);
      nPriorityDelay = data[2];
      assert(data[3] >=1 && data[3] <= 5);
      nPrioritySize  = data[3];
      
      SetPriority();

      Buf[0] = SYS_LOGINFO;
      sprintf(&Buf[1], "Priority: Delay %d usec, Queue %d, Size %d.",
	      nDelay, nQueue, nSize);
      Send((ubit8 *) Buf, strlen(Buf) + 1);
      break;

     case SYS_CLOSE:
      if (ChannelManager.ChannelPtr(data[1]) != NULL)
	delete ChannelManager.ChannelPtr(data[1]);
      break;

     case SYS_UPLOAD:
      ChannelManager.Set(SERVICE_UPLOAD, data[1], (char *) &data[2]);
      break;

     case SYS_DOWNLOAD:
      ChannelManager.Set(SERVICE_DOWNLOAD, data[1], (char *) &data[2]);
      break;

     case SYS_DIRECTORY:
      SendDir();
      break;

     case SYS_LONGINFO:
      if (!isfilename((char *) &data[1]))
      {
	 Buf[0] = SYS_LOGINFO;
	 sprintf(&Buf[1], "Illegal name.");
	 Send((ubit8 *) Buf, strlen(Buf) + 1);
	 return;
      }

      sprintf(Buf, ".%s", &data[1]);

      f = fopen(Buf, "rb");

      if (f)
      {
	 while (!feof(f))
	 {
	    if (fgets(&Buf[1], SLW_MAX_DATA-5, f))
	    {
	       assert(strlen(&Buf[1]) < SLW_MAX_DATA - 5);
	       strip_trailing_blanks(&Buf[1]);
	       if (!str_is_empty(&Buf[1]))
	       {
		  Buf[0] = SYS_LOGINFO;
		  Send((ubit8 *) Buf, strlen(Buf) + 1);
	       }
	    }
	    else
	      break;
	 }
	 fclose(f);
      }
      else
      {
	 sprintf(&Buf[1], "Error reading information.");
	 Buf[0] = SYS_LOGINFO;
	 Send((ubit8 *) Buf, strlen(Buf) + 1);
      }
      break;

     case SYS_CD:
      Buf[0] = SYS_FILEINFO;
      CD((char *) &data[1]);
      break;

     case SYS_STATUS:

      // Build a info packet and return it.
      //
      Buf[0] = SYS_INFO;

      Slw->SlwError.Status(&Buf[1]);
      Slw->PcktError.Status(&Buf[strlen(Buf)]);
      assert(strlen(Buf) < sizeof(Buf));

      Send((ubit8 *) Buf, strlen(Buf) + 1);

      break;

     case SYS_RTT:
      Send(data, len);
      break;

     case SYS_INFO:
      StatusBuf[0] = 0;

      nState = STATE_GETINFO;
      SystemGatherInfo(&data[1], len - 1);
      break;

     default:
      abort();
   }
}



void cSystem::Receive(ubit8 *data, ubit32 len)
{
   switch (nState)
   {
     case STATE_NORMAL:
      SystemNormal(data, len);
      break;

     case STATE_GETINFO:
      SystemGatherInfo(data, len);
      break;

     default:
      abort();
   }

}


void cSystem::RemoteOpen(int nService, ubit8 nChn, char *pStr)
{
   char Buf[1024];

   switch (nService)
   {
     case SYS_STATUS:
      Buf[0] = (ubit8) nService;
      Send((ubit8 *) Buf, 1);
      break;

     case SYS_UPLOAD:
     case SYS_DOWNLOAD:
      Buf[0] = (ubit8) nService;
      Buf[1] = nChn;
      strcpy(&Buf[2], pStr);
      Send((ubit8 *) Buf, 2 + strlen(&Buf[2]) + 1);
      break;

     default:
      abort();
   }
}



void cSystem::RemoteClose(ubit8 nChn)
{
   char Buf[5];

   Buf[0] = (ubit8) SYS_CLOSE;
   Buf[1] = nChn;
   Send((ubit8 *) Buf, 2);
}
