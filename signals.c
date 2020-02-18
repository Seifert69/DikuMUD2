/* *********************************************************************** *
 * File   : signals.c                                 Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : ???                                                            *
 *                                                                         *
 * Purpose: Trapping of signals from Unix.                                 *
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

#include <stdlib.h>
#include <stdio.h>

#ifndef DOS
  #include <signal.h>
  #include <sys/time.h>
#endif

#include "structs.h"
#include "utils.h"
#include "utility.h"

#if defined(SUNOS4) || defined(VAX)
int setitimer(int which, struct itimerval *value, struct itimerval *ovalue);
#endif


void other_signal(int signal_no)
{
   slog(LOG_ALL, 0, "RECEIVED SIGNAL %d.", signal_no);
}


void checkpointing(int signal_no)
{
  extern int tics;
  static int last_tick = 0;

  extern int player_convert;

  if (player_convert)
    return;

  if (last_tick != 0 && tics == last_tick)
  {
     slog(LOG_ALL, 0, "CHECKPOINT shutdown: tics not updated");
     assert(FALSE);
  }

  assert(tics != 0);
  last_tick = tics;

#ifdef POSIX
  /* Default is usually a one_shot */
  signal(SIGVTALRM, checkpointing);
#endif
}


void shutdown_request(int signal_no)
{
  extern int mud_shutdown;

  slog(LOG_ALL, 0, "Received USR2 - shutdown request");
  mud_shutdown = 1;
}


/* kick out players etc */
void hupsig(int signal)
{
   slog(LOG_ALL, 0, "Received signal #%d (SIGHUP, SIGINT, or SIGTERM). "
	"Shutting down", signal);
   exit(0);   /* something more elegant should perhaps be substituted */
}


void logsig(int signal)
{
  slog(LOG_ALL, 0, "Signal #%d received. Ignoring.", signal);
}

#ifdef DEBUG_HISTORY

void dump_debug_history(void);

void (*sigiot_func)(int)   = NULL;
void (*sigsegv_func)(int)  = NULL;
void (*sigbus_func)(int)   = NULL;
void (*sigabrt_func)(int)  = NULL;

void sig_debugdump(int tsignal)
{
   static int here = FALSE;

   slog(LOG_ALL, 0, "SIG_IOT #%d received!", tsignal);
   if (!here)
   {
      dump_debug_history();
      here = TRUE;
   }
//	signal(SIGSEGV,SIG_DFL);
//	signal(SIGBUS,SIG_DFL);
//	signal(SIGIOT,SIG_DFL);
	
   signal(SIGBUS,  sigbus_func);
      signal(SIGIOT,  sigiot_func);
      signal(SIGSEGV, sigsegv_func); 
   assert(FALSE);
}

#endif /* DEBUG_HISTORY */


void signal_setup(void)
{
#ifndef DOS
  struct itimerval itime;
  struct timeval interval;

  /* just to be on the safe side: */

signal(SIGPIPE, SIG_IGN);
  signal(SIGHUP,  hupsig);
  signal(SIGINT,  hupsig);
  signal(SIGALRM, logsig);
  signal(SIGTERM, hupsig);

  signal(SIGUSR2, shutdown_request);

#ifdef DEBUG_HISTORY
  sigbus_func  = signal(SIGBUS,  sig_debugdump);
  sigiot_func  = signal(SIGIOT,  sig_debugdump);
  sigsegv_func = signal(SIGSEGV, sig_debugdump);
#endif

  /* set up the deadlock-protection */

  interval.tv_sec = 600;    /* 600 = 10 minutes */
  interval.tv_usec = 0;
  itime.it_interval = interval;
  itime.it_value = interval;
  setitimer(ITIMER_VIRTUAL, &itime, 0);
  signal(SIGVTALRM, checkpointing);
#endif /* !DOS */
}
