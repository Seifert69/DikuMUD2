/* *********************************************************************** *
 * File   : slide.cc                                  Part of Valhalla MUD *
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

#include <assert.h>
#include <string.h>

#include "slide.h"
#include "packet.h"
#include "timeout.h"
#include "log.h"

//
// For discriminating when ACK & NAK & Sequence numbers are used.
//
// The header is two bytes, and is either of:
//
//   [     ] [     ]
//     SEQ     ACK
//     NAK     ACK


#define SLW_HDRUSED   (0x80)
#define SLW_HDRMASK   (0x7F)


void cSlw::Input(int nFlags)
{
   int n;

   if (!IsHooked())
   {
      pLogFile->Log("No longer on-line.");
      Unhook();
      return;
   }

   if (IS_SET(nFlags, SELECT_EXCEPT))
   {
      pLogFile->Log("Exception on serial descriptor.");
      Unhook();
      return;
   }

   if (IS_SET(nFlags, SELECT_READ))
   {
      Poll();
      
      if (!IsHooked())
      {
	 pLogFile->Log("Control: Poll -1\n");
	 Unhook();
	 return;
      }
   }
}


void cSlw::Activate(void)
{
   bActive = TRUE;

   // Also activate the packet layer
   Reset();

   cPacketLayer::Activate();
}


void cSlw::Deactivate(void)
{
   bActive = FALSE;

   Reset();

   cPacketLayer::Deactivate();
}


cSlw::cSlw(char *logfile)
{
   if (logfile)
     pLogFile = new cLog(logfile);
   else
     pLogFile = new cLog;

   Activate();
}


cSlw::~cSlw(void)
{
   delete pLogFile;
}


int cSlw::Open(char *pDevice, int nBaud, pfSlwReceive pfNLE)
{
   SetArrivalFunction(pfNLE);

   return cPacketLayer::Open(pDevice, nBaud);
}


// Called if we have to switch this on / off (e.g. between sessions).
//
void cSlw::Reset(void)
{
   TimeoutClear();

   SlwFifo.Flush();

   SlwError.Reset();
   PcktError.Reset();

   nTxLowWin        = 0;       // Low edge of senders window
   nTxHighWin       = 0;       // Upper edge of senders window + 1
   nNoBuffered      = 0;       // How many output buffers currently used?

   nRxLowWin        = 0;       // Low edge of receivers window
   nRxHighWin       = NO_BUFS; // Upper edge of receivers window + 1
   nRxTop           = 0;       // Sequence of highest window frame

   for (int i=0; i < NO_BUFS; i++)
   {
      aRxBuf[i].bArrived = FALSE;
      aRxBuf[i].nNaks    = 0;
      aRxBuf[i].nLen     = 0;
      aRxBuf[i].pData    = NULL;
      
      aTxBuf[i].nSeq     = 0;
      aTxBuf[i].nLen     = 0;
   }
}

// Does the received frame fall within the current window?
//
static inline int between(int lower, int received, int upper)
{
   if (((lower <= received) && (received < upper)) ||
       ((upper < lower) && ((lower <= received) || (received < upper))))
     return TRUE;
   else
     return FALSE;
}


// ACKs the packet just previously to the lower edge (nRxLowWin-1).
//
// The ACK message has the format:
//  [ N/A ] [ ACK ]
//
void cSlw::SendACK(void)
{
   ubit8 header[2];

   //pLogFile->Debug("TX: ACK frame %d.\n", (nRxLowWin + MAX_SEQ) % (MAX_SEQ + 1));

   SlwError.nAckPackets++;

   header[0] = 0;
   header[1] = (ubit8) (SLW_HDRUSED | ((nRxLowWin + MAX_SEQ) % (MAX_SEQ + 1)));

   UnregisterPiggyTimer();

   TransmitFrame(header, (ubit32) 2);
}


// NAK the packet nSeq (0..MAX_SEQ). Remember the buffers are only in
// the range [0..NO_BUFS]!
//
// The NAK message has the format:
//  [ NAK ] [ N/A ]
//
void cSlw::SendNAK(int nSeq)
{
   ubit8 header[2];

   assert(!aRxBuf[nSeq % NO_BUFS].bArrived);

   // Important! Settingthe number of NAK transmits too low will cause
   // a too frequent reliance on retransmit timeouts. Setting it too high
   // will cause an unnessecary amount of NAK frames transmitted.
   //
   // Empirical tests led me to set it to 18 at 115Kb.
   //
   if (aRxBuf[nSeq % NO_BUFS].nNaks++ % 18)
   {
      //pLogFile->Debug("NAK for frame %d skipped.\n", nSeq % (MAX_SEQ + 1));
      return;
   }
   else
   {
      pLogFile->Debug("TX: NAK for frame %d.\n", nSeq % (MAX_SEQ + 1));
   }

   SlwError.nNakPackets++;

   header[0] = (ubit8) (SLW_HDRUSED | (nSeq % (MAX_SEQ + 1)));
   header[1] = 0;

   TransmitFrame(header, 2);
}


void cSlw::SendNaks(void)
{
   int i;

   i = nRxLowWin;

   //pLogFile->Debug("Checkking NAKS between %d and %d\n", i, nRxTop);

   while (between(nRxLowWin, i, nRxTop + 1))
   {
      if (!aRxBuf[i % NO_BUFS].bArrived)
	SendNAK(i);

      i = (i + 1) % (MAX_SEQ + 1);
   }
}


// The Data header has the format:
//
//  [ SEQ ] [ ACK ]
//
void cSlw::SendData(int nFrameNo)
{
   //pLogFile->Debug("TX: DATA for frame %d.\n", nFrameNo);

   SlwError.nTxPackets++;

   // Merge frame number with piggybacked acknowledgement
   //
   aTxBuf[nFrameNo % NO_BUFS].aData[aTxBuf[nFrameNo % NO_BUFS].nLen] =
     (ubit8) (SLW_HDRUSED | nFrameNo);

   aTxBuf[nFrameNo % NO_BUFS].aData[aTxBuf[nFrameNo % NO_BUFS].nLen+1] =
     (ubit8) (SLW_HDRUSED | ((nRxLowWin + MAX_SEQ) % (MAX_SEQ + 1)));

   TransmitFrame(aTxBuf[nFrameNo % NO_BUFS].aData,
		 SLW_HEADER + aTxBuf[nFrameNo % NO_BUFS].nLen);

   UnregisterPiggyTimer();

   RegisterRetransTimer(); /* nFrameNo */
}


void cSlw::ArrivedUpdateAck(ubit8 ack)
{
   // Remains of a DATA frame or an ACK frame to be chekked here...
   //
   while (between(nTxLowWin, ack, nTxHighWin))
   {
      nNoBuffered--;
      assert(nNoBuffered >= 0);
      nTxLowWin = (nTxLowWin + 1) % (MAX_SEQ + 1);
   }

   if (nNoBuffered == 0)
     UnregisterRetransTimer();
}

void cSlw::ArrivedData(ubit8 *data, int len)
{
   ubit8 seq = (ubit8) (data[len-2] & SLW_HDRMASK);
   ubit8 ack = (ubit8) (data[len-1] & SLW_HDRMASK);

   //pLogFile->Debug("RX: DATA Seq %d (Ack %d).\n", seq, ack);

   if (!between(nRxLowWin, seq, nRxHighWin))
   {
      // We got a packet out of the window.
      // Most likely an ACK frame got lost! Send one now.

      pLogFile->Debug("Got packet %d outside window %d - %d, "
                      "Acking latest packet.\n", seq, nRxLowWin, nRxHighWin);

      RegisterPiggyTimer();

      free(data);
   }
   else if (aRxBuf[seq % NO_BUFS].bArrived) // Between, but duplicate!
   {
      // We got a duplicate packet in the window.
      // We must miss some previous segment of data, lets Nak them

      pLogFile->Debug("Got duplicate packet inside window.\n");

      SendNaks();

      //SendNAK(nRxLowWin + 1);

      free(data);
   }
   else  // Between and not a duplicate.
   {
      assert(aRxBuf[seq % NO_BUFS].pData == NULL);

      aRxBuf[seq % NO_BUFS].bArrived = TRUE;
      aRxBuf[seq % NO_BUFS].pData    = data;
      aRxBuf[seq % NO_BUFS].nLen     = (ubit8) (len - SLW_HEADER);

      nRxTop     = seq;

      if (seq != nRxLowWin)
      {
	 pLogFile->Debug("Got out of sequence packet, NAKing\n");
	 SendNaks();
	 //SendNAK(nRxLowWin + 1);
      }
      
      while (aRxBuf[nRxLowWin % NO_BUFS].bArrived)
      {
	 ubit8 *pTmpData = aRxBuf[nRxLowWin % NO_BUFS].pData;
	 int nTmpLen     = aRxBuf[nRxLowWin % NO_BUFS].nLen;

	 aRxBuf[nRxLowWin % NO_BUFS].bArrived = FALSE;
	 aRxBuf[nRxLowWin % NO_BUFS].nNaks    = 0;
	 aRxBuf[nRxLowWin % NO_BUFS].pData    = NULL;

	 if (nRxLowWin == nRxTop)
	   nRxTop   = (nRxLowWin + 1) % (MAX_SEQ + 1);

	 nRxLowWin  = (nRxLowWin + 1) % (MAX_SEQ + 1);
	 nRxHighWin = (nRxHighWin + 1) % (MAX_SEQ + 1);

	 class cQueueElem *pQe = new cQueueElem(pTmpData, nTmpLen, FALSE);
	 SlwFifo.Append(pQe);

	 RegisterPiggyTimer();
      }
   }

   ArrivedUpdateAck(ack);
}


void cSlw::ArrivedAck(ubit8 seq, int len)
{
   //pLogFile->Debug("RX: ACK %d.\n", seq & SLW_HDRMASK);

   ArrivedUpdateAck((ubit8) (seq & SLW_HDRMASK));
}


void cSlw::ArrivedNak(ubit8 seq, int len)
{
   ubit8 ack = (ubit8) ((seq & SLW_HDRMASK) % (MAX_SEQ + 1));

   pLogFile->Debug("RX: NAK %d.\n", ack);

   if (between(nTxLowWin, ack, nTxHighWin))
   {
      if (ack == nTxLowWin)
	UnregisterRetransTimer();

      SendData(ack);
   }
}



/* ========================= External Routines =========================== */

// It is important that we suspend the timers since, we can otherwise
// risk that the timer interrupts us in the middle of a transmission on
// the physical layers! If so we can in effect get a packet transmitted
// in the middle of a packet.
//


// Data is received allocated and is then free'ed.
//
void cSlw::EventFrameArrival(ubit8 *data, ubit32 len)
{
   static int nBlocked = 0;

   nBlocked++;
      
   if (!bActive)
   {
      // Uh oh, we may have to queue this so as to prevent strange calls
      // due to the poll()...
      //
      class cQueueElem *pQe = new cQueueElem(data, len);
      SlwFifo.Append(pQe);

      if (nBlocked <= 1)
      {
	 while (!SlwFifo.IsEmpty())
	 {
	    pQe = (class cQueueElem *) SlwFifo.GetHead();
	    
	    pfNetworkLayerEvent(pQe->Data(), pQe->Bytes());
	    
	    delete pQe;
	 }
      }
   }
   else
   {
      // We suspend the timer, because we do not want to get for example
      // a retransmission just as we are about to send one anyway!
      //
      TimeoutSuspend();

      if (len > SLW_HEADER)
      {
	 SlwError.nRxPackets++;
	 //pLogFile->Debug("SLW got a data frame.\n");
	 ArrivedData(data, len);
      }
      else
      {
	 if (data[0] & SLW_HDRUSED)   // NAK frame
	 {
	    //pLogFile->Debug("SLW got a NAK frame.\n");
	    ArrivedNak(data[0], len);
	 }

	 if (data[1] & SLW_HDRUSED)   // ACK frame
	 {
	    //pLogFile->Debug("SLW got an ACK frame.\n");
	    ArrivedAck(data[1], len);
	 }

	 free(data);
      }

      TimeoutResume();

      if (nBlocked <= 1)
      {
	 while (!SlwFifo.IsEmpty())
	 {
	    //pLogFile->Debug("Handing packet via pfNetworkLayerEvent.\n");
	    class cQueueElem *pE = (class cQueueElem *) SlwFifo.GetHead();
	    
	    pfNetworkLayerEvent(pE->Data(), pE->Bytes());
	    
	    delete pE;
	 }
      }
   }

   nBlocked--;
}


// If no outgoing frames were transmitted, we need to send a solo
// ACK message.
//
// No need to suspend, since we are called by the timer module, and
// it does not schedule another event until this has been processed.
// However, we must still prevent the timer from overlapping the
// physical transmissions.
//
void cSlw::EventPiggybackTimeout(void)
{
   //pLogFile->Debug("SLW got Piggyback timeout\n");
   SendACK();
}


// This is called when we have not received an acknowledgement for an
// already transmitted packet within the time limit. Retransmit the data.
//
// No need to suspend, since we are called by the timer module, and
// it does not schedule another event until this has been processed.
// However, we must still prevent the timer from overlapping the
// physical transmissions.
//
void cSlw::EventRetransmitTimeout(void)
{
   pLogFile->Debug("SLW got Retransmit timeout\n");

   assert(nNoBuffered != 0);

   SendData(nTxLowWin);  /* Was: Arg Passed */
}


// When an error occurs on the packet layer (i.e. CRC / Length error),
// this event is called.
//
// No need to suspend timer, since NAKs are unrelated to the timer events.
// Doesn't matter if we get interrupted.
// However, we must still prevent the timer from overlapping the
// physical transmissions.
//
void cSlw::EventPacketError(void)
{
   //pLogFile->Debug("SLW got an error frame!\n");

   TimeoutSuspend();

   SlwError.nCrcError++;

   SendNaks();
   // SendNAK(nRxLowWin + 1);

   TimeoutResume();
}


// Called by user of the protocol when a frame is transmitted.
//
//
int cSlw::Transmit(const ubit8 *data, ubit32 len)
{
   if (!bActive)
   {
      if (TransmitFrame(data, len) == -1)
	return -1;
      return 0;
   }

   if (nNoBuffered >= NO_BUFS)
     return -1;           // No more buffer space

   if (len > SLW_MAX_DATA || len < 1)
     return -2;           // Illegal amount of data

   TimeoutSuspend();

   nNoBuffered++;

   // SLW header is moved into 'aData[len]' at "runtime"
   //
   memcpy(aTxBuf[nTxHighWin % NO_BUFS].aData, data, len);
   aTxBuf[nTxHighWin % NO_BUFS].nLen = (ubit8) len;

   SendData(nTxHighWin);

   nTxHighWin = (nTxHighWin + 1) % (MAX_SEQ + 1);

   TimeoutResume();

   Poll();

   return 0;
}


// Returns how many packets are buffered. 0 means idle, otherwise up to
// NO_BUFS may be buffered. Useful for making priority transmissions.
//
sbit32 cSlw::nBuffered(void)
{
   return nNoBuffered;
}


// Set by user to point to his local handler for incoming packets.
//
void cSlw::SetArrivalFunction(pfSlwReceive pfNLE)
{
   pfNetworkLayerEvent = pfNLE;
}
