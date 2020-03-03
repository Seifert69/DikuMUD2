/* *********************************************************************** *
 * File   : packet.cc                                 Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert                                                        *
 *                                                                         *
 * Purpose: Packet Layer                                                   *
 * Bugs   : None Known                                                     *
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

#include "essential.h"
#include "slide.h"
#include "packet.h"
#include "log.h"

//
//
// The special FRD and ESC characters.
//
#define SLW_FRD     (0xFE)   // Should be set so as to be least frequently used
#define SLW_ESC     (0xFD)   // Ditto.


//
// These can be any character but FRD.
//
#define SLW_ESC_FRD (0xFC)   // Result: SLW_FRD
#define SLW_ESC_ESC (0xFB)   // Result: SLW_ESC


cPacketLayer::cPacketLayer(void)
{
   pData = NULL;
   Activate();
}


cPacketLayer::~cPacketLayer(void)
{
   if (pData)
   {
      free(pData);
      pData = NULL;
   }
}


void cPacketLayer::Activate(void)
{
   bActive   = TRUE;
   bHadError = FALSE;
   bEscaped  = FALSE;

   nLength   = 0;
   nCrc      = 0;
   pData     = (ubit8 *) malloc(PCKT_DATA_LEN+PCKT_HEADER_LEN+1);
}


void cPacketLayer::Deactivate(void)
{
   bActive = FALSE;
}


// Used to update the CRC on receive and transmit. Convert to assembler
// for various processors for possible performance gain.
//
ubit16 cPacketLayer::UpdateCRC(register ubit8 c, register ubit16 crc)
{
/*

  The UpdateCRC is as follows in 8088 assembler:

  AL = Char to update with (8  bit)
  BX = CRC value to update (16 bit) (BH is MSB, BL is LSB of CRC).

  At the start of a packet, CRC should be set to 0.
  Always at the end of a packet before transmitting the CRC, do:

     CRC = UpdateCRC(0, UpdateCRC(0, CRC))

  Then transmit the CRC in MSB, LSB order.

  When receiving: The two CRC value characters should be calculated
  as part of the package. Then CRC will be zero if no error occurred.

         MOV CX, 8      ; The generator is X^16+X^12+X^5+1
LOOP1:                  ; as recommended by CCITT. An alternate
         ROL AL, 1      ; generator often used in synchronous
         RCL BX, 1      ; communication is X^16+X^15+X^2+1
         JNB SKIP       ; This may be used by substituting XOR 8005H
         XOR BX, 8005H  ; for XOR 1021H in the adjacent code.
SKIP:
         LOOP LOOP1

*/
   {
      register int i;
      register ubit16 rcl_cf;

      for (i=0; i < 8; i++)
      {
	 c = (ubit8) ((c << 1) | ((c & 128) >> 7));  // ROL c, 1 (CF == c & 1)
	 rcl_cf = crc & 0x8000;
	 crc = (crc << 1) | (c & 1);       // RCL crc, 1 (CF is rcl_cf)
	 if (rcl_cf)                       // JNB loop
	   crc ^= (ubit16) 0x8005;                  // XOR crc, 8005H
      }
   }

   return crc;
}
      


void cPacketLayer::TransmitStuffCharacter(register ubit8 c)
{
   if (c == SLW_ESC)
   {
      Send(SLW_ESC);
      Send(SLW_ESC_ESC);
   }
   else if (c == SLW_FRD)
   {
      Send(SLW_ESC);
      Send(SLW_ESC_FRD);
   }
   else
     Send(c);
}


/* ======================================================================= */
/*                              P U B L I C                                */
/* ======================================================================= */

//
// Send between 1..PCKT_MAX_DATA bytes of data
//
int cPacketLayer::TransmitFrame(const ubit8 *data, ubit32 len)
{
   if (!bActive)  // Bypass packet layer?
     return Send(data, len);

   ubit32 i;
   ubit16 crc = 0;

   assert(len > 0 && len <= PCKT_DATA_LEN);

   PcktError.nTxPackets++;

   for (i=0; i < len; i++)
   {
      TransmitStuffCharacter(data[i]);
      crc = UpdateCRC(data[i], crc);
   }

   len--; // We can have one more byte of data by subtracting one.
   TransmitStuffCharacter((ubit8) len);
   crc = UpdateCRC((ubit8) len, crc);

   crc = UpdateCRC((ubit8) 0, UpdateCRC((ubit8) 0, crc));

   TransmitStuffCharacter((ubit8) ((crc>>8) & 0xFF));  /* MSB of CRC */
   TransmitStuffCharacter((ubit8) (crc      & 0xFF));  /* LSB of CRC */

   return Send(SLW_FRD);  /* Frame Separator */
}



/* ======================================================================= */

void cPacketLayer::FrameError(void)
{
   if (!bHadError)
   {
      EventPacketError();
      bHadError = TRUE;
   }

   bEscaped = FALSE;
   nLength  = 0;
   nCrc     = 0;
}


int cPacketLayer::FrameCheck(void)
{
   if (nLength > PCKT_DATA_LEN+PCKT_HEADER_LEN) // Too long, read next frame
   {
      pLogFile->Debug("PCKT: RX too long.\n");

      PcktError.nLengthError++;

      FrameError();

      return FALSE;
   }

   if (nLength < 1+PCKT_HEADER_LEN) // We must have header and 1 data!
   {
      pLogFile->Debug("PCKT: RX header & data too small\n");

      PcktError.nLengthError++;

      FrameError();

      return FALSE;
   }

   if (1 + (ubit32) pData[nLength-3] != nLength - 3)
   {
      pLogFile->Debug("PCKT: RX length mismatch (headr %d versus %d read)\n", pData[nLength-3] + 1, nLength-3);


      PcktError.nLengthError++;

      FrameError();

      return FALSE;
   }

   if (nCrc != 0)
   {
      pLogFile->Debug("PCKT: RX CRC mismatch %d\n", nCrc);

      PcktError.nCrcError++;

      FrameError();

      return FALSE;
   }

   // We got a frame successfully!
   //
   // pLogFile->Debug("PCKT: Got frame (%d bytes)\n", nLength - 3);
   bHadError = FALSE;
   return TRUE;
}
	 

void cPacketLayer::DeliverPacket(void)
{
   ubit8 *pTmpData = pData;
   ubit32 nTmpLen = nLength - 3;

   PcktError.nRxPackets++;

   nLength  = 0;
   nCrc     = 0;
   bEscaped = FALSE;
   pData    = (ubit8 *) malloc(PCKT_DATA_LEN+PCKT_HEADER_LEN+1);

   EventFrameArrival(pTmpData, nTmpLen);
}

//
// -1 on failure
//  0 on success
//
// If more than one damaged frame arrives, only one error is reported
// in order to not let a single burst of errors make a wreck of error
// reports.
//

void cPacketLayer::ReceiveCharacter(ubit8 c)
{
   if (c == SLW_FRD)
   {
      if (!FrameCheck())
	return;

      // Pass on packet to sliding windows layer
      // Get ready for receiving the next packet.
      //

      DeliverPacket();

      return;
   }
   else if (bEscaped)
   {
      bEscaped = FALSE;

      if (c == SLW_ESC_ESC)
	c = SLW_ESC;
      else if (c == SLW_ESC_FRD)
	c = SLW_FRD;
      else
      {
	 // Wrong character escaped, frame garbeled, skip until SLW_FRD

	 pLogFile->Debug("PCKT: RX Illegal escape sequence\n");

	 PcktError.nEscError++;

	 nLength = PCKT_DATA_LEN + PCKT_HEADER_LEN + 1;
	 return;
      }
   }
   else if (c == SLW_ESC)
   {
      bEscaped = TRUE;
      return;
   }


   if (nLength >= PCKT_DATA_LEN+PCKT_HEADER_LEN) // Error, skip
   {
      nLength++;
      return;
   }

   pData[nLength++] = c;
   nCrc = UpdateCRC(c, nCrc);
}


void cPacketLayer::Receive(ubit8 *pData, ubit32 nLen)
{
   if (!bActive)  // Bypass packet layer?
   {
      EventFrameArrival(pData, nLen);
      return;
   }

   for (ubit32 i = 0; i < nLen; i++)
     ReceiveCharacter(pData[i]);
}
