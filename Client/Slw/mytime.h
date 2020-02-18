/* *********************************************************************** *
 * File   : mytime.h                                  Part of Valhalla MUD *
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
#ifndef _MYTIME_H
#define _MYTIME_H

#include "essential.h"

class cMyTime
{
  public:
   void gettime(void);

   int operator==(const cMyTime &cT1);
   int operator<=(const cMyTime &cT1);

   void operator+=(const int hunds);
   void operator+=(const cMyTime &cT1);

   void operator-=(const cMyTime &cT1);

   ubit32 secs;  // Number of seconds passed.
   sbit32 hunds; // Number of 1/100 seconds passed.
};

#endif
