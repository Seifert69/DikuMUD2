/* *********************************************************************** *
 * File   : timer.cc                                  Part of Valhalla MUD *
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
#include <signal.h>
#include <assert.h>

#include "essential.h"
#include "timer.h"
#include "mytime.h"

/* ======================================================================= */

/* We need to make this pointer reference such that the AlarmHandler is
   able to call the Timer object! No way around it due to the nature of
   the signal routine! */

static cTimer *pcTimer = NULL;

void _AlarmHandler(int signal_no)
{
   signal(SIGALRM, _AlarmHandler);

   assert(pcTimer);

   pcTimer->InternEvent();
}


/* ======================================================================= */

cTimer::cTimer()
{
   bSuspended   = FALSE;
   bActive      = FALSE;
   bPending     = FALSE;
   pcTimer      = this;

   itimer.it_interval.tv_sec  = 0;
   itimer.it_interval.tv_usec = 0;

   //pfTimer    = NULL;

   signal(SIGALRM, _AlarmHandler);
}


cTimer::~cTimer()
{
   ClearEvent();
}


/* ======================================================================= */


void cTimer::InternEvent(void)
{
   assert(bActive);

   if (bSuspended)
   {
      bPending = TRUE;
      return;
   }

   bActive  = FALSE;
   bPending = FALSE;

   Event();
   //(*pfTimer) ();
}

void cTimer::SetEvent(cMyTime *psWhen) // , pfTimerEvent TimerEvent)
{
   assert(!bActive);
   assert(!bPending);

   bActive = TRUE;
   // pfTimer = TimerEvent;

#ifdef LINUX
   itimer.it_value.tv_sec  = psWhen->secs;
   itimer.it_value.tv_usec = psWhen->hunds * 10000;

   if (setitimer(ITIMER_REAL, &itimer, NULL) != 0)
     exit(1);
#endif
}



void cTimer::ClearEvent(void)
{
   // When given to setitimer, this value causes the next timer to abort.
   //
   static struct itimerval iabort = {{0,0},{0,0}};

   // assert(bSuspended);

   if (setitimer(ITIMER_REAL, &iabort, NULL) != 0)
     exit(1);

   bActive  = FALSE;
   bPending = FALSE;
}



void cTimer::SuspendEvent(void)
{
   bSuspended = TRUE;
}



void cTimer::ResumeEvent(void)
{
   if (bPending)
   {
      bActive = FALSE;

      DEBUG("PENDING");

      Event(); // (*pfTimer) ();
   }

   bSuspended = FALSE;
}
