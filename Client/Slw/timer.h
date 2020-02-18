/* *********************************************************************** *
 * File   : timer.h                                   Part of Valhalla MUD *
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
#ifndef TIMER_H
#define TIMER_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

#include "mytime.h"

// An attempt to create a general purpose timer interface which is
// equivalent between DOS and UNIX.
//
class cTimer
{
  friend void _AlarmHandler(int);

  public:
   cTimer();
   virtual ~cTimer();

   void SetEvent(cMyTime *psWhen); // , pfTimerEvent fptr);
   void ClearEvent(void);    // Call to clear any pending timeout

   void SuspendEvent(void);  // Call to suspend timeouts
   void ResumeEvent(void);   // Call when allowed again, possibly releasing one

   virtual void Event(void) {} // Inherit this to catch the timeout events.

  private:
   void InternEvent(void);

   ubit8 bSuspended;
   ubit8 bActive;
   ubit8 bPending;

   struct itimerval itimer;

   //pfTimerEvent pfTimer;
};

#endif
