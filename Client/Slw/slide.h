/* *********************************************************************** *
 * File   : slide.h                                   Part of Valhalla MUD *
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

#ifndef SLW_H
#define SLW_H

#include "packet.h"
#include "mytime.h"
#include "queue.h"
#include "timeout.h"

#define SLW_HEADER   (2)
#define SLW_MAX_DATA (PCKT_DATA_LEN - SLW_HEADER)

class cSlwErrors
{
  public:
   int AnyActivity(void) { return (nTxPackets > 0) || (nRxPackets > 0); }

   void Reset(void)
   {
      nTxPackets = 0;
      nRxPackets = 0;
      nCrcError = 0;
      nAckPackets = 0;
      nNakPackets = 0;
   }
   
   cSlwErrors() { Reset(); }


   void Status(char *Buf)
   {
      sprintf(Buf, "\nSliding Windows Layer\n"
	      "        TX: %5ld   RX: %5ld   Ack: %5ld   Nak: %5ld\n"
              "Errors: Packet Layer %5ld\n",
              (signed long) nTxPackets,   (signed long) nRxPackets,
	      (signed long) nAckPackets,  (signed long) nNakPackets,
              (signed long) nCrcError);
   }
 
   ubit32 nRxPackets;
   ubit32 nTxPackets;

   ubit32 nCrcError;

   ubit32 nAckPackets;
   ubit32 nNakPackets;
};


typedef void (*pfSlwReceive)(ubit8 *pData, ubit32 len);

struct rx_frame_type
{
   ubit8 bArrived;  // True if the frame has already arrived.
   ubit8 nNaks;     // Number of Naks that has been TX'ed for the frame.
   ubit8 nLen;
   ubit8 *pData;    // The data received is pre-allocated
};

struct tx_frame_type
{
   int nSeq;        // Sanity checking for the timer.

   ubit8 nLen;
   ubit8 aData[SLW_HEADER + SLW_MAX_DATA];
};


// Constant basic definitions.
//
#define MAX_SEQ   (63)               // 0..63 are the sequence numbers
#define NO_BUFS   ((MAX_SEQ+1) / 2)  // We only need half the amount of buffers
#define MAX_BUF   (NO_BUFS - 1)

class cSlw : private cSlwTimeout, public cPacketLayer
{
  public:
   cSlw(char *logfile = NULL);
   ~cSlw(void);

   int  Open(char *pDevice, int nBaud = 38400, pfSlwReceive pfRcv = NULL);

   void SetArrivalFunction(pfSlwReceive pfNLE);

   int isActive(void) { return bActive; }
   void Activate(void);
   void Deactivate(void);
   void Input(int nFlags);
   
   void Poll(void) { cSerial::Poll(); }

   // Call when you have data to transmit. Returns -1 if there is no more
   // room (i.e. ENOBUFS), -2 on error, 0 on success.
   //
   int  Transmit(const ubit8 *pData, ubit32 nLen);

   cSlwErrors SlwError;

   // ---------------- PACKEY LAYER INTERFACE ------------------
   // Called from the packet layer when a damage frame arrives.
   // Only called between successful reception of a packet.
   //
   void EventPacketError(void);
   sbit32 nBuffered(void);

  protected:

   // ------------------ TIMEOUT INTERFACE --------------------
   // Called from the cSlwTimeout layer when a timeout occurs.
   // These are inherited as virtuals.
   //
   void EventRetransmitTimeout(void);
   void EventPiggybackTimeout(void);

   // Called when a packet arrives free of errors.
   // Inherited from the packet layer
   //
   void EventFrameArrival(ubit8 *data, ubit32 len);
   

  private:
   void Reset(void);
   void SendACK(void);
   void SendNAK(int nSeq);
   void SendNaks(void);
   void SendData(int nFrameNo);
   void ArrivedUpdateAck(ubit8 ack);
   void ArrivedData(ubit8 *data, int len);
   void ArrivedAck(ubit8 seq, int len);
   void ArrivedNak(ubit8 seq, int len);
   
   struct rx_frame_type aRxBuf[NO_BUFS];
   struct tx_frame_type aTxBuf[NO_BUFS];
   cQueue SlwFifo;
   pfSlwReceive pfNetworkLayerEvent;

   int   nTxLowWin;       // Low edge of senders window
   int   nTxHighWin;      // Upper edge of senders window + 1
   sbit32 nNoBuffered;     // How many output buffers currently used?
   
   int   nRxLowWin;       // Low edge of receivers window
   int   nRxHighWin;      // Upper edge of receivers window + 1
   int   nRxTop;          // Sequence of highest window frame
   int   bActive;         // TRUE when the sliding windows is active
};


#endif
