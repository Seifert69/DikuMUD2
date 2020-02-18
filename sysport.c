/* *********************************************************************** *
 * File   : sysport.c                                 Part of Valhalla MUD *
 * Version: 1.12                                                           *
 * Author : gnort@daimi.aau.dk & elascurn@daimi.aau.dk                     *
 *                                                                         *
 * Purpose: Routines for support of non-unix machines                      *
 *          (or other portability hacks)                                   *
 *                                                                         *
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

/* Notice:  To make this module (and possibly system.c too) compile cleanly
 * under AmigaDOS, you'll have to apply this patch to <devices/timer.h>.
 * I got it from the readme-files for AmigaDOS gcc2.6.x.
 *							/gnort
 *
 * Use whatever was included first, standard (sys/time.h) or Amiga
 * includes (jch).
 *
 * + #ifndef _SYS_TIME_H_
 *   struct timeval {
 *       ULONG tv_secs;
 *       ULONG tv_micro;
 *   };
 * + #else
 * + #define tv_secs  tv_sec
 * + #define tv_micro tv_usec
 * + #endif
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "structs.h"
#include "utils.h"
#include "utility.h"
#include "handler.h"
#include "comm.h"
#include "system.h"

#ifdef DOS
char *crypt(char *salt, char *password)
{
  return password;
}
#endif /* DOS */


#ifdef SUNOS4
/* I know, not very pretty, but it should work on all SunOS machines... */
double difftime(time_t t1, time_t t2)
{
  /* Can't remember the right order... oh well... */
  if (t1 < t2)
    return (double) t2 - t1;
  else
    return (double) t1 - t2;
}
#endif /* SUNOS4 */


#ifdef AMIGA1
  /* These cmd's clash rather unfortunately with AmigaDOS functionality. */
  #ifdef CMD_RESET
    #undef CMD_RESET
  #endif
  #ifdef CMD_READ
    #undef CMD_READ
  #endif
  #ifdef CMD_WRITE
    #undef CMD_WRITE
  #endif

  #include <devices/timer.h>
  #include <time.h>

  #include <exec/semaphores.h>
  #include <exec/memory.h>

  #include <clib/alib_protos.h>
  #include <clib/exec_protos.h>

/* Routines specifically for the Amiga version */

struct timerequest *tr = NULL;
struct timeval tval;
time_t oldtime, newtime;

char *crypt(char *text, char *key)
{
  return text;
}


void CloseTimer(void)
{
  struct MsgPort *tp;

  if (tr)
  {
    tp = tr->tr_node.io_Message.mn_ReplyPort;

    if (tp != 0)
      DeletePort(tp);

    CloseDevice((struct IORequest *) tr);
    DeleteExtIO((struct IORequest *) tr);
  }
}


int OpenTimer(void)
{
  struct MsgPort *timerport;

  if (!(timerport = (struct MsgPort *)CreatePort( 0, 0 )))
  {
    perror("No TimerPort!");
    return 0;
  }

  if (!(tr = (struct timerequest *)
	CreateExtIO( timerport, sizeof(struct timerequest))))
  {
    perror("TimerExtIO not got!");
    DeletePort(timerport);
    return 0;
  }

  if (OpenDevice((UBYTE *)TIMERNAME, UNIT_MICROHZ, (struct IORequest *)tr, 0L))
  {
    perror("Timer.device out for lunch");
    DeleteExtIO( (struct IORequest *) tr );
    DeletePort(timerport);
    return 0;
  }

  time(&oldtime);

  return 42;
}


int WaitForPulse(void)
{
  tval.tv_secs = 0;
  tr->tr_node.io_Command = TR_ADDREQUEST;
  tr->tr_time = tval;

  time(&newtime);
  if ((int) difftime(newtime, oldtime) > 25)
  {
    oldtime += 25;
    return 0;
  }

  tr->tr_time.tv_micro = 250000 - ((int) difftime(newtime, oldtime)) * 10000;

  if (tr->tr_time.tv_micro < 20000L)
  {
    oldtime += 25;
    return 0;
  }

  DoIO((struct IORequest *) tr );

  oldtime = newtime;

  return 0;
}

#endif /* AMIGA */
