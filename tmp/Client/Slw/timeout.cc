/* *********************************************************************** *
 * File   : timeout.cc                                Part of Valhalla MUD *
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

#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>

#include "essential.h"
#include "slide.h"
#include "mytime.h"

#include "cyclic.h"
#include "arraylist.h"

#include "timer.h"
#include "timeout.h"



/* ======================================================================= */

// Inherited from cTimer
void cSlwTimeout::Event(void)
{
   switch (nEventCode)
   {
     case 0:
      TimerPiggyHandler();
      break;

     case 1:
      TimerRetransHandler();
      break;

     default:
      abort();
   }
}

/* ======================================================================= */

cSlwTimeout::cSlwTimeout(void)
{
   nTimeoutPiggy    =   20;  // 0.2 seconds
   nTimeoutRetrans  =  200;  // 2.0 seconds
   nEventCode       =   10;  // Dummy
}


cSlwTimeout::~cSlwTimeout(void)
{
}

void cSlwTimeout::UnregisterPiggyTimer(void)
{
   sPiggy.remove();
}


void cSlwTimeout::UnregisterRetransTimer(void)
{
   sRetrans.remove();
}


void cSlwTimeout::TimeoutClear(void)
{
   UnregisterPiggyTimer();
   UnregisterRetransTimer();

   ClearEvent();
}

/* ======================================================================= */

void cSlwTimeout::TimerActivateMinimal(void)
{
   if (!sPiggy.is_empty() && !sRetrans.is_empty())
   {
      if (sPiggy.sTime <= sRetrans.sTime)
	SetTimerEvent(&sPiggy.sTime, 0);
      else
	SetTimerEvent(&sRetrans.sTime, 1);
   }

   else if (!sPiggy.is_empty())
     SetTimerEvent(&sPiggy.sTime, 0);

   else if (!sRetrans.is_empty())
     SetTimerEvent(&sRetrans.sTime, 1);

   else
     ClearEvent();
}



// The SIGVTALARM calls here when the timer expires.
// This is for the piggy queue.
// -1 is a direct signal! (i.e. no timeout!)
//
void cSlwTimeout::TimerPiggyHandler(void)
{
   assert(!sPiggy.is_empty());  // SLW may empty if ACK removes 

   // DEBUG("Piggy handler event\n");

   UnregisterPiggyTimer();

   EventPiggybackTimeout();

   TimerActivateMinimal();
}


// The SIGVTALARM calls here when the timer expires.
// This is for the retrans queue.
//
void cSlwTimeout::TimerRetransHandler(void)
{
   assert(!sRetrans.is_empty());

   // DEBUG("Retrans handler event\n");

   UnregisterRetransTimer();

   EventRetransmitTimeout();

   TimerActivateMinimal();
}


/* ======================================================================= */


void cSlwTimeout::SetTimerEvent(cMyTime *sWhen, ubit32 n)
{
   cMyTime sNow, sDummy;

   ClearEvent();

   nEventCode = n;

   sNow.gettime();

   if (*sWhen <= sNow)
   {
      DEBUG("Direct call\n");

      Event();

      // Ok, we gotta see if the next one will cause a wait!
      // Otherwise we will go no more timeouts!
      // So must try until done or one is waiting.
      //
      TimerActivateMinimal();
      return;
   }

   sDummy = *sWhen; // I cant make the fucking pik shit work!
   sDummy -= sNow;  // Anyone wanna buy a fucking good manual?

   SetEvent(&sDummy);
}

/* ======================================================================= */

// The register functions simply queue the timer and sequence number.
//
void cSlwTimeout::RegisterPiggyTimer(void)
{
   if (!sPiggy.is_empty())  // The piggy queue is only one big!
     return;

   sPiggy.sTime.gettime();
   sPiggy.sTime += nTimeoutPiggy;   // Add specified number of usecs.
   sPiggy.insert();
}


void cSlwTimeout::RegisterRetransTimer(void)
{
   if (!sPiggy.is_empty())  // If someone else is waiting let em wait...
     return;

   sRetrans.sTime.gettime();
   sRetrans.sTime += nTimeoutRetrans;   // Add specified number of usecs.
   sRetrans.insert();
}


/* ======================================================================= */

void cSlwTimeout::TimeoutResume(void)
{
   TimerActivateMinimal();

   ResumeEvent();
}
