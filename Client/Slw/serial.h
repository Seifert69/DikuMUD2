/* *********************************************************************** *
 * File   : serial.h                                  Part of Valhalla MUD *
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

#ifndef _MS_SERIAL_H
#define _MS_SERIAL_H

#include <termios.h>

#include "interfc.h"
#include "select.h"

class cSerial : public cNetInterface, public cHook
{
  public:
   cSerial();
   cSerial(char *pDev);
   virtual ~cSerial();
   
   int  Open(char *cpDev, int nBaud = 38400);
   int  Close(void);

   int  Online(void);
   int  Hangup(void);
   int  ModemInit(char **init_string[]);

   int  Send(ubit8 data);
   int  Send(const ubit8 *pData, ubit32 nLen);

   int  SendString(char *pStr);
   virtual void Receive(ubit8 *pChunk, ubit32 nSize) = 0;

   void Poll(void);
   int WaitOnline(void);  // Wait for modem to become active
   int Flush(void);

  private:
   int ReadBlock(ubit8 *pChunk, ubit32 nSize);
   int DisplayStatus(void);
   int Status(void);
   int Break(void);
   int SetRaw(void);
   int SetNormal(void);
   int SetMode(int baud, char par, char bits);
   int WaitFor(char *pStr, int nTimeOutSecs);
   int Command(char *txStr);

   struct termios org_tty;
   struct termios raw_tty;
   int ori_flags;
   int raw_flags;
};

#endif
