/* *********************************************************************** *
 * File   : packet.h                                  Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert                                                        *
 *                                                                         *
 * Purpose: Packet Layer Headers                                           *
 * Bugs   : None Known.                                                    *
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

#ifndef PCKT_H
#define PCKT_H

#include <stdio.h>

#include "essential.h"
#include "serial.h"

class cPcktErrors
{
  public:
   void Reset(void)
   {
      nTxPackets = 0;
      nRxPackets = 0;
      nLengthError = 0;
      nCrcError = 0;
      nEscError = 0;
   }

   cPcktErrors() { Reset(); }

   void Status(char *Buf)
   {
      sprintf(Buf, "\nPacket Layer:\n"
	      "        TX: %5ld   RX:    %5ld\n"
              "Errors: CRC %5ld   Length %5ld   Garbled %5ld\n",

              (signed long) nTxPackets, (signed long) nRxPackets,
              (signed long) nCrcError,  (signed long) nLengthError,
	      (signed long) nEscError);
   }
 
   ubit32 nTxPackets;
   ubit32 nRxPackets;

   ubit32 nLengthError;
   ubit32 nCrcError;
   ubit32 nEscError;
};


class cPacketLayer : public cSerial
{
  public:
   cPacketLayer(void);
   ~cPacketLayer(void);

   void Activate(void);
   void Deactivate(void);

   // Called from the sliding layer, when a frame is to be transmitted.
   // Operation is blocking, not released until frame is placed in the
   // "hardware" (or system) buffers for transmission.
   //
   int TransmitFrame(const ubit8 *data, ubit32 len);

   // Called when error occurs in this layer...
   //
   virtual void EventPacketError(void) {}

   // Called when packet is ready for delivery...
   //
   virtual void EventFrameArrival(ubit8 *pTmpData, ubit32 nTmpLen) = 0;


   // Called from the physical layer in one of two ways.
   //   if bPcktRXBlocked is TRUE
   //      Called with a garbage character to try to make the layer
   //      deliver its packet.
   //   if bPcktRXBlocked is FALSE
   //      Called with a data character read from the incoming data stream.
   //
   inline void ReceiveCharacter(ubit8 c);

   // Virtual From NetInterface
   void Receive(ubit8 *pData, ubit32 nLen);

   cPcktErrors PcktError;

  private:
   inline ubit16 UpdateCRC(register ubit8 c, register ubit16 crc);
   inline void TransmitStuffCharacter(register ubit8 c);
   void   FrameError(void);
   inline int FrameCheck(void);
   void   DeliverPacket(void);

   // It's nasty that these are "global", but I dont want one big messy
   // function, neither do I want to pass millions of arguments and get
   // huge overheads in processing.
   //

   ubit8    bActive;
   ubit8    bHadError;
   ubit8    bEscaped;

   ubit32   nLength;
   ubit16   nCrc;
   ubit8    *pData;
};


// The maximum amount of data in a packet (excl. header).
//
#define PCKT_DATA_LEN     (256)

// The maximum amount of header data in a packet.
//
#define PCKT_HEADER_LEN   (3)

#endif
