/* *********************************************************************** *
 * File   : mytime.cc                                 Part of Valhalla MUD *
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
// This module makes the timer interface between DOS, UNIX, etc.
//

#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>

#include "mytime.h"

void cMyTime::gettime(void)
{
#ifdef LINUX
   struct timeval sNow;

   gettimeofday(&sNow, NULL);

   secs  = sNow.tv_sec;
   hunds = sNow.tv_usec / 10000;
#endif
}

int cMyTime::operator==(const cMyTime &sT1)
{
   if (hunds != sT1.hunds)
     return FALSE;

   if (secs != sT1.secs)
     return FALSE;

   return TRUE;
}


int cMyTime::operator<=(const cMyTime &sT1)
{
   if (secs < sT1.secs)
     return TRUE;
   else if ((secs == sT1.secs) && (hunds <= sT1.hunds))
     return TRUE;

   return FALSE;
}


void cMyTime::operator+=(const int nHunds)
{
   hunds += nHunds;

   while (hunds >= 100)  // Could do % and /, think this is faster
   {
      secs++;
      hunds -= 100;
   }
}


void cMyTime::operator+=(const cMyTime &sT1)
{
   secs  += sT1.secs;
   hunds += sT1.hunds;

   if (hunds >= 100)
   {
      hunds -= 100;
      secs++;
   }
}


// Subtract sT2 from sT1. If sT1 < sT2 result seconds are negative!
//
void cMyTime::operator-=(const cMyTime &sT1)
{
   secs  -= sT1.secs;
   hunds -= sT1.hunds;

   if (hunds < 0)
   {
      hunds += 100;
      secs--;
   }
}

