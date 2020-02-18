/* *********************************************************************** *
 * File   : timeout.h                                 Part of Valhalla MUD *
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
#ifndef TIMEOUT_H
#define TIMEOUT_H

#include "slide.h"
#include "timer.h"

class cTimerInstance
{
  public:
   cTimerInstance(void)
   {
      bActive = FALSE;
   }

   ~cTimerInstance(void)
   {
      bActive = FALSE;
   };

   void insert(void)
   {
      bActive = TRUE;
   }

   void remove(void)
   {
      bActive = FALSE;
   }

   int is_empty(void)
   { 
      return !bActive;
   } 

   cMyTime sTime;

  private:
   int bActive;
};


class cSlwTimeout : public cTimer
{
  public:
   cSlwTimeout(void);
   virtual ~cSlwTimeout(void);

   virtual void EventPiggybackTimeout(void) = 0;
   virtual void EventRetransmitTimeout(void) = 0;

   void TimeoutSuspend(void) { SuspendEvent(); }
   void TimeoutResume(void);
   void TimeoutClear(void);
   
   void RegisterPiggyTimer(void);
   void UnregisterPiggyTimer(void);

   void RegisterRetransTimer(void);
   void UnregisterRetransTimer(void);

  private:
   void Event(void);

   void TimerActivateMinimal(void);
   void SetTimerEvent(cMyTime *sWhen, ubit32 n);

   void TimerPiggyHandler(void);
   void TimerRetransHandler(void);

   cTimerInstance sRetrans;
   cTimerInstance sPiggy;

   // These timeouts could be changed dynamically at runtime when the acutal
   // RTT times are calculated.
   ubit32  nTimeoutPiggy;
   ubit32  nTimeoutRetrans;

   ubit32  nEventCode;
};

#endif
