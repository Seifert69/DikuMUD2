/* *********************************************************************** *
 * File   : inter.h                                   Part of Valhalla MUD *
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

#ifndef INTERACTIVE_H
#define INTERACTIVE_H

#include <string.h>
#include <netinet/in.h>

#include "essential.h"
#include "channel.h"
#include "select.h"


class cBaseInteractive;
class cMud;
class cMainMenu;
class cZload;

class cText : public cChannel
{
  public:
   cText(ubit8 nChn);
   virtual ~cText(void);

   void Receive(ubit8 *data, ubit32 len);
   int SendString(char *pString);

   int Change(cBaseInteractive *pcNew);

   cBaseInteractive *pcCurrent;

   cMud      *pcMud;
   cMainMenu *pcMainMenu;
   cZload    *pcZload;

};


// Interactive classes must inherit the Channel Base, such that each will
// fit nicely in a channel. Also, activate and stop are made virtual for
// ease of use.
//
class cBaseInteractive
{
  public:
   cBaseInteractive(cText *pcNew) { pcInteractive = pcNew; }

   virtual void Receive(ubit8 *data, ubit32 len) = 0;

   int SendString(char *str) { return Send((ubit8 *) str, strlen(str)); }

   int Send(ubit8 *data, int len) { return pcInteractive->Send(data, len); }


   virtual int Activate(void) { return 0; };
   virtual int End(void) { return 0; };

   cText *pcInteractive;
};


class cMud : public cBaseInteractive, public cHook
{
  public:
   cMud(cText *pcNew) : cBaseInteractive(pcNew) { };
   virtual ~cMud(void);

   void Input(int nFlags);
   void Receive(ubit8 *data, ubit32 nLen);

   int Activate(void);
   int End(void);

  private:
   struct sockaddr_in server_addr;
};

class cMainMenu : public cBaseInteractive
{
  public:
   cMainMenu(cText *pcNew) : cBaseInteractive(pcNew) {};

   void Receive(ubit8 *data, ubit32 len);
   int Activate(void);

  protected:
   int Menu(char c);
};

class cZload : public cBaseInteractive
{
  public:
   cZload(cText *pcNew) : cBaseInteractive(pcNew) {};

   void Receive(ubit8 *data, ubit32 len);
   int Activate(void);

  protected:
   int Menu(char c);
};

#endif
