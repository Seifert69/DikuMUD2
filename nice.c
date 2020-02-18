/* *********************************************************************** *
 * File   : nice.c                                    Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : quinn (?)                                                      *
 *                                                                         *
 * Purpose: Handling come, nice and reboot.                                *
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
#include <ctype.h>
#include <string.h>
#include <time.h>
#if !defined(DOS)
  #include <sys/time.h>

  #ifdef SOLARIS /* Yuk!  /gnort */
    #define _POSIX_C_SOURCE 3
  #endif
  #include <signal.h>
#endif

#include "structs.h"
#include "system.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "comm.h"
#include "textutil.h"
#include "utility.h"

#define REBOOT_AT    10  /* 0-23, time of optional reboot if -e lib/reboot */

/* extrenal vars */
extern int     tics;     /* number of tics since boot-time */
extern struct descriptor_data *descriptor_list, *next_to_process;

/* external procs */
#if !defined(DOS) && !defined(AMIGA)
  #ifndef HPUX
  int select (int width, fd_set *readfds,fd_set *writefds,fd_set *exceptfds,
	      struct timeval *timeout);
  #endif

  int system(const char *string);

  #ifdef GENERIC_BSD
    int sigsetmask(int mask);
  #endif
#endif

int close_connection(int s);
int new_connection(int s);

void night_watchman(void)
{
   long tc;
   struct tm *t_info;

   extern int mud_shutdown;

   tc = time(0);
   t_info = localtime(&tc);

   if ((t_info->tm_hour == 8) && (t_info->tm_wday > 0) &&
       (t_info->tm_wday < 6))
   {
     if (t_info->tm_min > 50)
     {
	slog(LOG_ALL, 0,"Leaving the scene for the serious folks.");
	send_to_all("Closing down. Thank you for flying " MUD_NAME ".\n\r");
	mud_shutdown = 1;
     }
     else if (t_info->tm_min > 40)
       send_to_all("ATTENTION: " MUD_NAME
		   " will shut down in 10 minutes.\n\r");
     else if (t_info->tm_min > 30)
       send_to_all("Warning: The game will close in 20 minutes.\n\r");
   }
}


void check_reboot(void)
{
#if !(defined(LINUX) || defined(DOS) || defined(AMIGA))
   long tc;
   struct tm *t_info;
   char dummy;
   FILE *boot;

   extern int mud_shutdown, mud_reboot;

   tc = time(0);
   t_info = localtime(&tc);

   if ((t_info->tm_hour + 1) == REBOOT_AT && t_info->tm_min > 30)
     if ((boot = fopen("./reboot", "r")))
     {
	if (t_info->tm_min > 50)
	{
	   slog(LOG_ALL, 0,"Reboot exists.");
	   fread(&dummy, sizeof(dummy), 1, boot);
	   if (!feof(boot))   /* the file is nonepty */
	   {

	      slog(LOG_ALL, 0, "Reboot is nonempty.");

#ifdef GENERIC_BSD
	      sigsetmask(sigmask(SIGUSR1)   | sigmask(SIGUSR2) |
			 sigmask(SIGPIPE)   | sigmask(SIGALRM) |
			 sigmask(SIGTERM)   | sigmask(SIGURG)  |
			 sigmask(SIGXCPU)   | sigmask(SIGHUP)  |
			 sigmask(SIGVTALRM) | sigmask(SIGINT));
#else /* POSIX */
	    {
	      sigset_t set;

	      sigemptyset(&set);

	      sigaddset(&set, SIGUSR1);
	      sigaddset(&set, SIGUSR2);
	      sigaddset(&set, SIGPIPE);
	      sigaddset(&set, SIGALRM);
	      sigaddset(&set, SIGTERM);
	      sigaddset(&set, SIGURG);
#ifndef HPUX
	      sigaddset(&set, SIGXCPU);
#endif
	      sigaddset(&set, SIGHUP);
	      sigaddset(&set, SIGVTALRM);
	      sigaddset(&set, SIGINT);

	      sigprocmask(SIG_SETMASK, &set, NULL);
	    }
#endif

	      if (system("./reboot"))
	      {
		 slog(LOG_ALL, 0,"Reboot script terminated abnormally");
		 send_to_all("The reboot was cancelled.\n\r");
		 rename("reboot", "reboot.FAILED");
		 fclose(boot);
#ifdef GENERIC_BSD
		 sigsetmask(0);
#else /* POSIX */
	       {
		 sigset_t set;
		 sigemptyset(&set);
		 sigprocmask(SIG_SETMASK, &set, NULL);
	       }
#endif
		 return;
	      }
	      else
		rename("reboot", "reboot.SUCCEEDED");
#ifdef GENERIC_BSD
	      sigsetmask(0);
#else
	    {
	      sigset_t set;
	      sigemptyset(&set);
	      sigprocmask(SIG_SETMASK, &set, NULL);
	    }
#endif
	   }

	   send_to_all("Automatic reboot. Come back in a little while.\n\r");
	   mud_shutdown = mud_reboot = 1;
	}
	else if (t_info->tm_min > 40)
	  send_to_all("ATTENTION: " MUD_NAME " will reboot "
		      "in 10 minutes.\n\r");
	else if (t_info->tm_min > 30)
	  send_to_all("Warning: The game will close and reboot "
		      "in 20 minutes.\n\r");

	fclose(boot);
     }
#endif
}


#define GR
#define NEW
#ifdef GR

int workhours()
{
   long tc;
   struct tm *t_info;

   tc = time(0);
   t_info = localtime(&tc);

   return((t_info->tm_wday > 0) &&
	  (t_info->tm_wday < 6) &&
	  (t_info->tm_hour >= 9) &&
	  (t_info->tm_hour < 17));
}


/*
* This procedure is *heavily* system dependent. If your system is not set up
* properly for this particular way of reading the system load (It's weird all
* right - but I couldn't think of anything better), change it, or don't use -l.
* It shouldn't be necessary to use -l anyhow. It's oppressive and unchristian
* to harness man's desire to play. Who needs a friggin' degree, anyhow?
*/

int load(void)
{
#ifdef DOS
   struct syslinfo {
      char    sl_date[12];    /* "Tue Sep 16\0" */
      char    sl_time[8];     /* "11:10\0" */
      char    sl_load1[6];    /* "12.0\0" */
      char    sl_load2[10];   /* "+2.3 14u\0" */
   } info;
   FILE *fl;
   int i, sum;
   static int previous[5];
   static int p_point = -1;
   extern int slow_death;

   if (!(fl = fopen("/tmp/.sysline", "r")))
   {
      perror("sysline. (dying)");
      slow_death = 1;
      return(-1);
   }
   if (!fread(&info, sizeof(info), 1, fl))
   {
      perror("fread sysline (dying)");
      slow_death = 1;
      return(-1);
   }
   fclose(fl);

   if (p_point < 0)
   {
      previous[0] = atoi(info.sl_load1);
      for (i = 1; i< 5; i++)
	previous[i] = previous[0];
      p_point = 1;
      return(previous[0]);
   }
   else
   {
      /* put new figure in table */
      previous[p_point] = atoi(info.sl_load1);
      if (++p_point > 4)
	p_point = 0;

      for (i = 0, sum = 0; i < 5; i++)
	sum += previous[i];
      return((int) sum / 5);
   }
#else
   return (0);
#endif
}




char *nogames(void)
{
#ifndef DOS
   static char text[200];
   FILE *fl;

   if ((fl = fopen("lib/nogames", "r")))
   {
      slog(LOG_ALL, 0,"/usr/games/nogames exists");
      char *ms2002 = fgets(text, 199,fl);
      return(text);
      fclose(fl);
   }
   else
#endif
     return(0);
}


#define COMA_SIGN \
"\n\r " MUD_NAME "is currently inactive due \
to excessive load on the host machine.\n\r\
Please try again later.\n\r\n\
\n\r\
   Sadly,\n\r\
\n\r\
    the " MUD_NAME " system operators\n\r\n\r"


/* sleep while the load is too high */
void coma(int s)
{
#ifdef SUSPEKT
#if !(defined(LINUX) || defined(DOS) || defined(AMIGA))
   fd_set input_set;
   static struct timeval timeout = { 60, 0 };
   int conn;

   int workhours(void);
   int load(void);

   slog(LOG_ALL, "Entering comatose state.");

#ifdef GENERIC_BSD
   sigsetmask(sigmask(SIGUSR1)  | sigmask(SIGUSR2) | sigmask(SIGINT)  |
	      sigmask(SIGPIPE)  | sigmask(SIGALRM) | sigmask(SIGTERM) |
	      sigmask(SIGURG)   | sigmask(SIGXCPU) | sigmask(SIGHUP)  |
	      sigmask(SIGVTALRM));
#else
 {
   sigset_t set;
   
   sigemptyset(&set);

   sigaddset(&set, SIGUSR1);
   sigaddset(&set, SIGUSR2);
   sigaddset(&set, SIGINT);
   sigaddset(&set, SIGPIPE);
   sigaddset(&set, SIGALRM);
   sigaddset(&set, SIGTERM);
   sigaddset(&set, SIGURG);
   sigaddset(&set, SIGXCPU);
   sigaddset(&set, SIGHUP);
   sigaddset(&set, SIGVTALRM);
   
   sigprocmask(SIG_SETMASK, &set, NULL);
 }
#endif
   
   while (descriptor_list)
     close_socket(descriptor_list);

   FD_ZERO(&input_set);
   do
   {
      FD_SET(s, &input_set);
      if (select(64, &input_set, 0, 0, &timeout) < 0)
      {
	 slog(LOG_ALL, 0, "coma select");
	 exit(1);
      }
      if (FD_ISSET(s, &input_set))
      {
	 if (load() < 6)
	 {
	    slog(LOG_ALL,"Leaving coma with visitor.");
#ifdef GENERIC_BSD
	    sigsetmask(0);
#else
	  {
	    sigset_t set;
	    sigemptyset(&set);
	    sigprocmask(SIG_SETMASK, &set, NULL);
	  }
#endif
	    return;
	 }
	 if ((conn = new_connection(s)) >= 0)
	 {
	    write_to_descriptor(conn, COMA_SIGN);
	    /*sleep(2); !!!!!!!!!!!!!!!!*/
	    close_connection(conn);
	 }
      }

      tics = 1;
      if (workhours())
      {
	 slog(LOG_ALL, 0, "Working hours collision during coma. Exit.");
	 exit(0);
      }
   }
   while (load() >= 6);

   slog(LOG_ALL,"Leaving coma.");
#ifdef GENERIC_BSD
   sigsetmask(0);
#else
 {
   sigset_t set;
   sigemptyset(&set);
   sigprocmask(SIG_SETMASK, &set, NULL);
 }
#endif
#endif
#endif
}

/* emulate the game regulator */
void gr(int s)
{
#ifdef SUSPEKT
#if !(defined(DOS) || defined(AMIGA))
   char *txt = 0, buf[1024];
   int ld = 0;
   static char *warnings[3] =
   {
      "If things don't look better within 3 minutes, the game will pause.\n\r",
      "The game will close temporarily 2 minutes from now.\n\r",
      "WARNING: The game will close in 1 minute.\n\r"
   };
   static int wnr = 0;

   extern int slow_death, mud_shutdown;

   void coma(int s);

   if (((ld = load()) >= 6) || (txt = nogames()) || slow_death)
   {
      if (ld >= 6)
      {
	 sprintf(buf, "The system load is greater than 6.0 (%d)\n\r", ld);
	 send_to_all(buf);
      }
      else if (slow_death)
	send_to_all("The game is dying.\n\r");
      else
      {
	 strcpy(buf,"Gameplaying is no longer permitted on this machine:\n\r");
	 strcat(buf, txt);
	 strcat(buf, "\n\r");
	 send_to_all(buf);
      }

      if (wnr < 3)
	send_to_all(warnings[wnr++]);
      else
	if (ld >= 6)
	{
	   coma(s);
	   wnr = 0;
	}
	else
	  mud_shutdown = 1;
   }
   else if (workhours())
     mud_shutdown = 1;                     /* this shouldn't happen */
   else if (wnr)
   {
      send_to_all("Things look brighter now - you can continue playing.\n\r");
      wnr = 0;
   }
#endif
#endif
}
#endif




