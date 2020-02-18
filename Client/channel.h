/* *********************************************************************** *
 * File   : channel.h                                 Part of Valhalla MUD *
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

#ifndef CHANNELS_H
#define CHANNELS_H

#include <assert.h>
#include "essential.h"
#include "slide.h"

extern cSlw *Slw;

// Reserved channels.
//
#define SERVICE_SYSTEM      1
#define SERVICE_TEXT        2
#define SERVICE_UPLOAD      3
#define SERVICE_DOWNLOAD    4


#define CHN_MAX_DATA (SLW_MAX_DATA - 1)

// This class defines the basic elements that are in a single Channel.
// Namely a send, a receive, and the number of the channel.
//
class cChannel
{
  public:
   cChannel(ubit8 n);
   virtual ~cChannel();

   ubit8 ChannelNo(void) { return nChannel; }
   sbit32 Send(ubit8 *data, ubit32 len);
   virtual void Receive(ubit8 *data, ubit32 len) = NULL;
   virtual void Thread(void);
   virtual const char *Status(void);

  private:
   ubit8 nChannel;
};


// This is the class that maintains the channels. It consists of 256
// base channels (above).
//
class cChannelManager
{
   friend class cChannel;

  public:
   cChannelManager(ubit8 bArg);
   cChannel *Set(ubit8 nService, ubit8 nChn, char *pData = NULL);

   cChannel *Alloc(ubit8 nService, char *pData = NULL);

   void Close(void);
   void Free(ubit8 nChannel);

   sbit32 MngrTX(ubit8 *data, ubit32 len, ubit8 nChn);
   void MngrRX(ubit8 *data, ubit32 len);
   void MngrRX(ubit8 *data, ubit32 len, ubit8 nChannel); // For direct connect
   void WriteQueue(void);

   void Threads(void);

   cChannel *ChannelPtr(ubit8 nChn);
   
  private:
   ubit8 bServer;  // TRUE if server (host), FALSE if client (caller)
   class cChannel *BaseChannel[256];
   cQueue qTx;
};

extern cChannelManager ChannelManager;

#endif
