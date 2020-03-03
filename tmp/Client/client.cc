/* *********************************************************************** *
 * File   : client.c                                  Part of Valhalla MUD *
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

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>

#include "essential.h"
#include "select.h"
#include "channel.h"
#include "slide.h"
#include "log.h"
#include "arg.h"
#include "client.h"
#include "system.h"


cSlw *Slw = NULL;

int bDirectConnect = TRUE;

cLog *pActivityLog = NULL;


void log_connect(int on)
{
   if (on)
     pActivityLog->Log("\nConnected %d - ", (int) time(0));
   else
     pActivityLog->Log("%d disconnected.", (int) time(0));
}



void incoming(ubit8 *data, ubit32 len)
{
   ChannelManager.MngrRX(data, len);
}



void direct_connect(ubit8 *data, ubit32 len)
{
   ChannelManager.MngrRX(data, len, 0);
}



static void login_show(void)
{
   static const char *str = "\r\n\r\n[MS-V/C@95ID]\rPress ESC to enter.";

   Slw->Transmit((ubit8 *) str, strlen(str));
}

/* Return 0 if no progress                */
/* Return 1 if direct connection detected */
/* Return 2 if client connection detected */

static int login_compare(char c)
{
   static const char *match = "[MS-V/C@RESP]";
   static int state  = 0;
   static int bytes  = 0;

   if (c == 27)
   {
      state = 0;
      bytes = 0;
      return 1;  // Direct connection detected!
   }

   bytes++;

   for (;;)
   {
      if (c == match[state])
      {
	 state++;

         if (match[state] == 0)
         {
	    bytes = 0;
	    state = 0;
	    return 2;   // Client connection
         }

	 break;
      }

      if (state)
	state = 0;  // Possibly it matches the first char, try again.
      else
	break;
   }


   if (bytes - state > 3)
   {
      bytes = 0;
      login_show();
   }

   return 0;
}


// Called from Slw with the data to be transmitted.
//
void login(ubit8 *data, ubit32 len)
{
   for (ubit32 i = 0; i < len; i++)
   {
      switch (login_compare(data[i]))
      {
	case 0:
	 break;

	case 1:
	 Slw->SetArrivalFunction(direct_connect);
	 bDirectConnect = TRUE;
	 direct_connect((ubit8 *) "?", 1);  /* Make the menu appear */
         pLogFile->Log("Direct Connection");
	 return;

	case 2:
	 Slw->SetArrivalFunction(incoming);
	 Slw->Activate();
	 bDirectConnect = FALSE;
         pLogFile->Log("SLW Connection");
	 // Warning, requires channel 0 as interactive text!
         // Make the menu appear
	 ChannelManager.ChannelPtr(0)->Receive((ubit8 *) "?", 1);
	 return;
      }
   }
}


void Control(void)
{
   int n;
   struct timeval tmv;

   pLogFile->Log("Entering control loop for new session.");

   if (chdir(arg.pFtpDir) != 0)
   {
      pLogFile->Log("Unable to change ftp directory to '%s', error %d.\n",
		    arg.pFtpDir, errno);
      exit(1);
   }

   bDirectConnect = TRUE;

   Slw->SetArrivalFunction(login);

   // Done in serial... CaptainHook.Hook(Slw->fd(), &SlwHook);

   ChannelManager.Set(SERVICE_TEXT,   0);
   ChannelManager.Set(SERVICE_SYSTEM, 1);
   ChannelManager.Set(SERVICE_TEXT,   2);
   ChannelManager.Set(SERVICE_TEXT,   3);
   ChannelManager.Set(SERVICE_TEXT,   4);

   login_show();

   tmv.tv_sec = 0;
   tmv.tv_usec = System->PriorityDelay();

   while (Slw->IsHooked())
   {
      n = CaptainHook.Wait(&tmv);

      if (n == -1)
      {
	 pLogFile->Log("N==%d, errno == %d\n", n, errno);
	 break;
      }
      else if (n == 0)	 // Control loop timeout
      {
	 tmv.tv_sec = 0;
	 tmv.tv_usec = System->PriorityDelay();

	 ChannelManager.Threads();
      }
      else
      {
	 // Descriptors were processed along the way...
	 if ((tmv.tv_sec == 0) && (tmv.tv_usec == 0))
	 {
	    tmv.tv_sec = 0;
	    tmv.tv_usec = System->PriorityDelay();

	    ChannelManager.Threads();
	 }
      }
   }

   pLogFile->Log("Session terminated normally.");

   Slw->Deactivate();

   ChannelManager.Close();

   Slw->Unhook();
}


// Returns FALSE if the session fails.
//
void SessionStart(void)
{
   int error_count = 0, n;

   Slw = new cSlw(arg.pLogName); // Log file name here.
   pActivityLog = new cLog(arg.pActivityName);

   while (error_count++ < 10)
   {
      if (Slw->Open(arg.pDevice, arg.nBaud) == -1)
      {
	 pLogFile->Log("Can't open Serial device.\n");
	 sleep(1);
	 continue;
      }

      Slw->Deactivate();

      if (arg.bModem)
      {
	 if (Slw->ModemInit(arg.ppInit) == -1)
	 {
	    pLogFile->Log("Can't init modem.\n");
	    if (Slw->Close() == -1)
	    {
	       pLogFile->Log("Can't close Slw!\n");
	       exit(1);
	    }
	    sleep(5);
	    continue;
	 }

	 error_count = 0;

	 if (Slw->WaitOnline() == -1)
	 {
	    pLogFile->Log("Wait online problems.\n");
	    if (Slw->Close() == -1)
	    {
	       pLogFile->Log("Can't close Slw!\n");
	       exit(1);
	    }
	    sleep(5);
	    continue;
	 }

	 /* The comms program Terminate! needs a little break here */
	 usleep(1000000/2);
      }

      if (Slw->Flush() != -1)
      {      
	 log_connect(TRUE);

	 Control();

	 log_connect(FALSE);
      }

      if (Slw->Close() == -1)
      {
	 pLogFile->Log("Close error.");
	 exit(1);
      }

      error_count = 0;
   }

   char buf[160];
   pLogFile->Log("Giving up on opening serial device.\n");
   sprintf(buf, "echo Error on %s %c%c%c%c%c. > /dev/console",
	   arg.pDevice, 7, 7, 7, 7, 7);
   pLogFile->Log("Calling system to alert operator\n");
   system(buf);
   pLogFile->Log("Ending\n");
   delete pActivityLog;
   exit(1);
}


int main(int argc, char *argv[])
{
   if ( !ParseArg(argc, argv) )
     exit(8);

   SessionStart();

   return 0;
}
