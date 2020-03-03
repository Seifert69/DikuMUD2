/* *********************************************************************** *
 * File   : inter.cc                                  Part of Valhalla MUD *
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

#include <arpa/telnet.h>

#include "essential.h"
#include "inter.h"
#include "select.h"
#include "serial.h"
#include "network.h"
#include "arg.h"

#include "channel.h"

#define MENU \
"\n\r"\
"\r\n[1] Play Valhalla\n\r"\
"[!] Goodbye\n\r\n"\
"    Select: "

//"[2] Download documentation\n\r"\


#define DOWNLOAD_MENU \
"\n\r\n[1] Download the Valhalla Client (Z Modem)\n\r"\
"[2] Download the Valhalla Client (X Modem)\n\r"\
"[R] Return to Main Menu\n\r\n"\
"    Select: " 



/* ========================================================================= */
/*                             INTERACTIVE                                   */
/* ========================================================================= */

cText::cText(ubit8 nChn) : cChannel(nChn)
{
   pcMud      = new cMud(this);
   pcZload    = new cZload(this);
   pcMainMenu = new cMainMenu(this);

   pcCurrent  = pcMainMenu;
}


cText::~cText(void)
{
   delete pcMud;
   delete pcZload;
   delete pcMainMenu;
}


void cText::Receive(ubit8 *data, ubit32 len)
{
   pcCurrent->Receive(data, len);
}

int cText::SendString(char *pString)
{
   return Send((ubit8 *) pString, strlen(pString));
}


int cText::Change(cBaseInteractive *pInter)
{
   pcCurrent->End();

   pcCurrent = pInter;

   return pcCurrent->Activate();
}


/* ========================================================================= */
/*                              DOWNLOAD                                     */
/* ========================================================================= */

void cZload::Receive(ubit8 *data, ubit32 len)
{
   return;
}

// Return 0 if ok, -1 if error.
//
int cZload::Activate(void)
{
   return 0;
}

int cZload::Menu(char c)
{
   return 0;
}

/* ========================================================================= */
/*                             MAIN MENU                                     */
/* ========================================================================= */

// Return 0 if ok to proceed, -1 if error, 1 if no proceeding.
//
//
int cMainMenu::Menu(char ch)
{
   switch (ch)
   {
     case '!':
      if (SendString("\n\rHope you enjoyed yourself, "
		     "come back soon...") == -1)
	return -1;

      Slw->Hangup();
      return -1;
      
      //case '2':
      // pcInteractive->Change(pcInteractive->pcZload);
      // return SendString("\n\rThis option is currently disabeled.\n\r");

     case '1':
      pcInteractive->Change(pcInteractive->pcMud);
      return 1;

     case ' ':
     case '?':
     case 'h':
     case 'H':
     case 'm':
     case 'M':
      if (Send((ubit8 *) &ch, 1) == -1)
	return -1;

     case  3: /* CTRL C */
     case 13:
     case 10:
     case 24:  /* CTRL X  */
      if (SendString(arg.pHeader) == -1)
	return -1;

      if (SendString(MENU) == -1)
	return -1;

      return 1;
   }

   return 0;
}


void cMainMenu::Receive(ubit8 *data, ubit32 len)
{
   int n;

   for (ubit32 i = 0; i < len; i++)
   {
      n = Menu(data[i]);

      if (n != 0)
	break;
   }

   return;
}



// Return 0 if ok, -1 if error.
//
int cMainMenu::Activate(void)
{
   return Menu(03) == -1 ? -1 : 0;
}


/* ========================================================================= */
/*                                   MUD                                     */
/* ========================================================================= */

cMud::~cMud(void)
{
   End();
}


// Return 0 if ok, -1 if error.
//
int cMud::Activate(void)
{
   char buf[128];
   ubit8 line = 255;
   int fdMud = -1;

   if (SendString("\n\r\nConnecting to Valhalla...") == -1)
     return -1;

   fdMud = OpenNetwork(arg.pcAddress, arg.nPort, &server_addr);

   if (fdMud == -1)
   {
      if (SendString("\r\n\nValhalla is unavailable at this "
		     "moment. Try again within a minute.") == -1)
	return -1;

      pcInteractive->Change(pcInteractive->pcMainMenu);

      return 0;
   }

   CaptainHook.Hook(fdMud, this);


   line = atoi(arg.pExt+1);

   sprintf(buf, "%c%c%c%c%c%c%c",
	   IAC, WILL, TELOPT_ECHO, line, IAC, WONT, TELOPT_ECHO);

   if (TransmitChunk(this->tfd(), (char *) buf, 7) == -1)
   {
      DEBUG("Lost connection right after sending .\n");
      pcInteractive->Change(pcInteractive->pcMainMenu);
   }


   return 0;
}



int cMud::End(void)
{
   // DEBUG("Mud connection closed.\n");

   Unhook();

   return 0;
}


// Just forward the data from the user to the MUD...
//
void cMud::Receive(ubit8 *data, ubit32 len)
{
   if (TransmitChunk(this->tfd(), (char *) data, len) == -1)
     pcInteractive->Change(pcInteractive->pcMainMenu);
}


void cMud::Input(int nFlags)
{
   int n;
   char NetworkBuf[2048];

   if (IS_SET(nFlags, SELECT_EXCEPT))
   {
      // LOG("MUD EXCEPTION\n");
      pcInteractive->Change(pcInteractive->pcMainMenu);
      return;
   }

   if (IS_SET(nFlags, SELECT_READ))
   {
      n = ReceiveChunk(this->tfd(), NetworkBuf, sizeof(NetworkBuf)-1);

      if (n > 0)
      {
	 if (Send((ubit8 *) NetworkBuf, n) == -1)
	 {
	    DEBUG("Error writing serial data from MUD.\n");
	    pcInteractive->Change(pcInteractive->pcMainMenu);
	    return;
	 }
      }
      else if (n == -1)
      {
	 // LOG("Error reading from MUD socket (%d).\n", errno);
	 pcInteractive->Change(pcInteractive->pcMainMenu);
	 return;
      }
      else /* n == 0 */
      {
	 /* DEBUG("DIKUMUD OTHER: %d\n", n); */
	 pcInteractive->Change(pcInteractive->pcMainMenu);
	 return;
      }
   }
}

