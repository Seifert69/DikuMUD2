/* *********************************************************************** *
 * File   : updown.cc                                 Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert                                                        *
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
#include "log.h"
#include "system.h"
#include "str_parse.h"
#include "updown.h"

#ifdef LINUX
#include "arg.h"
#endif

struct bbs_type bbs;

int isfilename(char *fname);

void UpdateBBS(void)
{
   FILE *f;
   char buf[256];
   static time_t last = 0, now;

   now = time(0);

   if (now - last < 15) // Every 15 seconds...
     return;

   last = now;

   sprintf(buf, "%sbbs/%d.%d", arg.pHomeDir, arg.nPort, atoi(arg.pExt+1));

   f = fopen(buf, "rb");

   if (f == NULL)
   {
      bbs.bUpload   = TRUE;
      bbs.bDownload = FALSE;
   }
   else
   {

      fscanf(f, "%d %d %s %d %d",
	    &bbs.bOverdue,
	    &bbs.nTotalCredit,
	    bbs.name,
	    &bbs.nLevel,
	    &bbs.nAccess);

      fclose(f);

      bbs.bDownload = (bbs.bOverdue == FALSE) && (bbs.nTotalCredit > 0);
      bbs.bUpload   = 1;
   }
}

ubit32 fsize(FILE *f)
{
   ubit32 oldpos, size;

   oldpos = ftell(f);

   if (fseek(f, 0L, SEEK_END))    /* Seek to end of file */
      assert(FALSE);

   size = ftell(f);

   if (fseek(f, oldpos, SEEK_SET))    /* Seek to end of file */
      assert(FALSE);

   return size;
}

/* ========================================================================= */
/*                               UPLOAD                                      */
/* ========================================================================= */


cUpload::cUpload(ubit8 nChn, char *fname) : cChannel(nChn)
{
   if (isfilename(fname))
     pFileName = str_dup(fname);
   else
     pFileName = NULL;

   f       = NULL;
   nLength = 0;
   nPos    = 0;
   nCrc    = 0;
   nState  = 0;
}


cUpload::~cUpload()
{
   if (f)
     fclose(f);

   if (pFileName)
     free(pFileName);
}


const char * cUpload::Status(void)
{
   return "Upload session.\n\r";
}


void cUpload::Receive(ubit8 *data, ubit32 len)
{
}


void cUpload::Thread(void)
{
   ubit8 *cp;
   ubit8 Buf[SLW_MAX_DATA + 2];
   static ubit32 nCalls;
   sbit32 n;
   FILE *tmpf;

   nCalls++;

   UpdateBBS();

   if (!bbs.bDownload)
   {
      Buf[0] = SYS_LOGINFO;
      strcpy((char *) &Buf[1], "No download permission.");
      System->Send((ubit8 *) Buf, strlen((char *) Buf) + 1);
      System->RemoteClose(ChannelNo());
      delete this;
      return;
   }

   switch (nState)
   {
     case 0:
      if (pFileName == NULL)
      {
	 Buf[0] = SYS_LOGINFO;
	 strcpy((char *) &Buf[1], "No such file to upload.");
	 System->Send(Buf, strlen((char *) Buf) + 1);
	 System->RemoteClose(ChannelNo());
	 delete this;
	 return;
      }

      f = fopen(pFileName, "rb");

      if (f == NULL)
      {
	 Buf[0] = SYS_LOGINFO;
	 strcpy((char *) &Buf[1], "No such file.");
	 System->Send(Buf, strlen((char *) Buf) + 1);
	 System->RemoteClose(ChannelNo());
	 delete this;
	 return;
      }

      nLength = fsize(f);

      nState++;
      break;

     case 1:

      // Send info to the download session in the following format:
      // <filename> <0> <len 1..4> <0> <crc 1> <crc 2>
      
      cp = Buf;
      strcpy((char *) cp, pFileName);
      TAIL(cp);
      cp++;     // Skip the trailing zero.

      // This is done the hard way, because of Big / Small Endian

      cp[0] = (ubit8) ((nLength >> 24) & 0xFF);
      cp[1] = (ubit8) ((nLength >> 16) & 0xFF);
      cp[2] = (ubit8) ((nLength >>  8) & 0xFF);
      cp[3] = (ubit8)  (nLength        & 0xFF);

      pLogFile->Log("Uploading %s.", pFileName);

      cp[4] = (ubit8) ((nCrc >>  8) & 0xFF);
      cp[5] = (ubit8)  (nCrc & 0xFF);
      assert(&cp[6] - Buf < (int) sizeof(Buf));

      Send(Buf, &cp[6] - Buf);
      nState++;
      break;

     case 2:
      // Send all the fragments in time...

      if (Slw->nBuffered() <= System->PriorityQueue())
      {
	 n = fread((char *) Buf, sizeof(ubit8), System->PrioritySize(), f);

	 if (n == -1)
	 {
	    pLogFile->Log("Error %d reading from file %s.", errno, pFileName);
	    System->RemoteClose(ChannelNo());
	    delete this;
	    return;
	 }

	 if (n > 0)
	   Send(Buf, n);
	 else if (feof(f))
	   nState++;
      }
      break;

     case 3:
      // Upload is finished...
      cp = Buf;

      *cp++ = 0xFF;
      *cp++ = 0xFF;
      *cp++ = 0xFF;
      *cp++ = 0xFF;

      Send(Buf, cp - Buf);
      nState++;

      sprintf((char *) Buf, ".%s", pFileName);

      tmpf = fopen((char *) Buf, "r+b");
      if (tmpf)
      {
	 fgets((char *) Buf, sizeof(Buf)-1, tmpf);
	 cp = (ubit8 *) strchr((char *) Buf, '[');
         if (cp && isdigit(cp[1]) && isdigit(cp[2]) &&
	     isdigit(cp[3]) && isdigit(cp[4]))
         {
	    if (fseek(tmpf, cp - Buf + 1, SEEK_SET) == 0)
	    {
	       n = (cp[4]-'0') + 10*(cp[3]-'0') +
		 100*(cp[2]-'0') + 1000*(cp[1]-'0');
	       n++;
	       fprintf(tmpf, "%04d", n);
	    }
	    else
	      pLogFile->Log("Seek error in %s, cannot update download count",
			    pFileName);
         }
	 else
	   pLogFile->Log("Digit error in %s, cannot update download count",
			 pFileName);

	 fclose(tmpf);
      }


      break;

     case 4:
      System->RemoteClose(ChannelNo());
      delete this;
      break;

     default:
      assert(FALSE);
   }
}


/* ========================================================================= */
/*                               Download                                    */
/* ========================================================================= */


cDownload::cDownload(ubit8 nChn, char *fname) : cChannel(nChn)
{
   nExpectedLength = 0;
   nExpectedCrc    = 0;

   nState      = 0;
   nLength     = 0;
   nCrc        = 0;
   f           = NULL;
   fshadow     = NULL;
   pFileName   = NULL;
   pShadowName = NULL;
   pBaseName   = NULL;
   bComplete   = FALSE;
}


cDownload::~cDownload()
{
   if (f)
     fclose(f);

   if (fshadow)
     fclose(fshadow);

   if (!bComplete)
   {
      pLogFile->Log("Did not receive whole %s", pFileName);
      remove(pFileName);
      remove(pShadowName);
   }

   if (pFileName)
     free(pFileName);

   if (pShadowName)
     free(pShadowName);

   if (pBaseName)
     free(pBaseName);
}


const char * cDownload::Status(void)
{
   return "Download session.\n\r";
}


void cDownload::Receive(ubit8 *data, ubit32 len)
{
   char Buf[256];
   ubit8 *cp = data;
   sbit32 n;
   time_t tnow;

   switch (nState)
   {
     case 0:

      if (!isfilename((char *) cp))
      {
	 char Buf[SLW_MAX_DATA + 2];

	 Buf[0] = SYS_LOGINFO;
	 strcpy(&Buf[1], "Illegal name.");
	 System->Send((ubit8 *) Buf, strlen((char *) Buf) + 1);
	 System->RemoteClose(ChannelNo());
	 delete this;
	 return;
      }
      
      pBaseName = str_dup((char *) cp);

      sprintf(Buf, "%s/upload/%s", arg.pFtpDir, cp);
      pFileName = str_dup(Buf);

      sprintf(Buf, "%s/upload/.%s", arg.pFtpDir, cp);
      pShadowName = str_dup(Buf);

      TAIL(cp);
      cp++; // Skip trailing zero

      nExpectedLength |= cp[0] << 24L;
      nExpectedLength |= cp[1] << 16L;
      nExpectedLength |= cp[2] <<  8L;
      nExpectedLength |= cp[3];

      nExpectedCrc    |= cp[4] <<  8;
      nExpectedCrc    |= cp[5];
      nState++;

      assert((ubit32) (&cp[6] - data) <= len);

      f = fopen(pFileName, "rb");

      if (f)
      {
	 char Buf[SLW_MAX_DATA + 2];

	 Buf[0] = SYS_LOGINFO;
	 strcpy(&Buf[1], "File already exists.");
	 System->Send((ubit8 *) Buf, strlen(Buf) + 1);
	 System->RemoteClose(ChannelNo());
	 delete this;
	 return;
      }

      fshadow = fopen(pShadowName, "rb");

      if (fshadow)
      {
	 char Buf[SLW_MAX_DATA + 2];

	 Buf[0] = SYS_LOGINFO;
	 strcpy(&Buf[1], "Shadow file already exists.");
	 System->Send((ubit8 *) Buf, strlen(Buf) + 1);
	 System->RemoteClose(ChannelNo());
	 delete this;
	 return;
      }

      f = fopen(pFileName, "wb");

      if (f == NULL)
      {
	 pLogFile->Log("Unable to create: %s", pFileName);
	 System->RemoteClose(ChannelNo());
	 delete this;
	 return;
      }

      fshadow = fopen(pShadowName, "wb");

      if (fshadow == NULL)
      {
	 pLogFile->Log("Unable to create shadow.");
	 System->RemoteClose(ChannelNo());
	 delete this;
	 return;
      }
      break;

     case 1:
      // Read the data.

      n = fwrite(data, sizeof(ubit8), len, f);

      if ((ubit32) n != len)
      {
	 pLogFile->Log("Error %d: Unable to write %d bytes in %s.\n",
	       n, len, pFileName);
	 System->RemoteClose(ChannelNo());
	 delete this;
	 return;
      }

      nLength += len;

      if (nLength >= nExpectedLength)
      {
	 if (nLength > nExpectedLength)
	 {
	    pLogFile->Log("Error: File too long (%d bytes read, %d expected) in %s.\n",
		  nLength, nExpectedLength, pFileName);
	    System->RemoteClose(ChannelNo());
	    delete this;
	    return;	    
	 }

	 nState++;
      }
      break;

     case 2:
      // It should now be 0xFFFFFFFF received...
      // With the short description appended just after...
      if (data[0] != 0xFF || data[1] != 0xFF ||
	  data[2] != 0xFF || data[3] != 0xFF)
      {
	 pLogFile->Log("EOF marker not received properly\n");
	 delete this;
	 return;
      }
      
      fprintf(fshadow, "%s,%d,%s\n", pBaseName, nExpectedLength, &data[4]);

      tnow = time(0);
      strftime(Buf, 40, "%b %d, %Y", localtime(&tnow));

      fprintf(fshadow, "Downloaded [0000] times.\n");
      fprintf(fshadow, "Uploaded %s by %s.\n", Buf, "Anonymous");

      nState++;
      break;

     case 3:
      if (data[0] == 0xFF)
      {
	 bComplete = TRUE;
	 nState++;
      }
      else
	fprintf(fshadow, "%s", data);
      break;

     case 4:
      delete this;
      break;

     default:
      assert(FALSE);
   }
}
