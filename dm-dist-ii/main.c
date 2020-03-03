/* *********************************************************************** *
 * File   : main.c                                    Part of Valhalla MUD *
 * Version: 1.10                                                           *
 * Author : bombman, quinn & seifert                                       *
 *                                                                         *
 * Purpose: Main loop and event control.                                   *
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

/* 29/1/93  HHS: Added GA (go ahead) for all prompts                       */
/* Sun Jun 27 1993 HHS: made vars for world status                         */
/* Tue Jul 6 1993 HHS: added exchangable lib dir                           */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <cerrno>

#ifdef DOS
#include <sys/time.h>
struct timezone {
  int tz_minuteswest;
  int tz_dsttime;
};
#endif

#include "sysport.h"

#include "structs.h"
#include "utils.h"
#include "utility.h"
#include "handler.h"
#include "system.h"
#include "comm.h"
#include "db.h"
#include "interpreter.h"
#include "main.h"
#include "textutil.h"
#include "files.h"

#include "unixshit.h"

extern ubit32 memory_total_alloc;

#define OPT_USEC 250000L       /* time delay corresponding to 4 passes/sec */
#define HEAPSPACE_INCREMENT 500

/* structures */
struct eventq_elem
{
   void (*func)(void *, void *);
   int when;
   void *arg1;
   void *arg2;
};

/* constants */
struct eventq_elem *event_heap = 0;
int heapspace = 0, events = 0;
struct descriptor_data *descriptor_list = NULL;
struct descriptor_data *next_to_process = NULL;

/* For multi-connectors */
class cMultiMaster Multi;

/* The global server configuration */

int mud_bootzone = 1;           /* Used when booting & resolving DIL templts */
int no_players = 0;             /* Statistics                                */
int max_no_players = 0;         /* Statistics                                */
int player_convert = FALSE;
int slow_death = 0;             /* Shut her down, Martha, she's sucking mud */
int mud_shutdown = 0;           /* clean shutdown */
int mud_reboot = 0;             /* reboot the game after a shutdown */
int wizlock = 0;                /* no mortals on now */
int tics = 0;                   /* number of tics since boot-time */

char world_boottime[64]="";/* boottime of world */

/* Had to move libdir to common.c /gnort */
extern char libdir[];        /* directory for libraryfiles */
char zondir[64] = ZONE_DIR; /* directory for zonefiles    */
char plydir[64] = PLAY_DIR; /* Directory for players */

const char *compile_date = __DATE__;
const char *compile_time = __TIME__;

/* external functions */
void string_add(struct descriptor_data *d, char *str);

void boot_db(void);

/* external functions in lib */
#ifdef GENERIC_BSD
  void srandom(int seed);
#endif
#if defined(SOLARIS) || defined(HPUX)
  void srand48(long seedval);
#endif

int gettimeofday(struct timeval *tp, struct timezone *tzp);

/* local functions */
void run_the_game(void);
void game_loop(void);
struct timeval timediff(struct timeval *a, struct timeval *b);

/* local events */
void game_event();
void check_idle_event(void *, void *);
void perform_violence_event(void *, void *);
void point_update_event(void *, void *);
void point_update();
void food_update_event(void *, void *);
void food_update();
void update_crimes_event(void *, void *);
void update_crimes();
void check_reboot_event(void *, void *);
void check_reboot();
void swap_check(void *, void *);

/* ******************************************************************* *
 *             main game loop and related stuff                        *
 * ******************************************************************* */

#ifdef LINUX
#include <sys/time.h>
#include <sys/resource.h>

/* int setrlimit(int, struct rlimit *); */
int getrlimit(int, struct rlimit *);
#endif


//
// Need this to be sure the typedefs are right for 64 bit architecture
//
void type_validate(void)
{
  assert(sizeof(void *) == 8);
  assert(sizeof(char) == 1);
  assert(sizeof(ubit1) == 1);
  assert(sizeof(ubit8) == 1);
  assert(sizeof(ubit16) == 2);
  assert(sizeof(ubit32) == 4);
  assert(sizeof(ubit64) == 8);
  slog(LOG_ALL, 0, "64-bit architecture checked out OK");
}

int main(int argc, char **argv)
{
   void cleanup_playerfile(int argc, char *argv[]);

   int pos = 1, sp;

   extern char **player_name_list;
   extern cNamelist persist_namelist;

   slog(LOG_ALL, 0, "DMSERVER COMPILED AT %s %s",
	compile_date, compile_time);

   type_validate(); // MS2020
   
#ifdef LINUX
   {
      int n;
      struct rlimit rlim;

      n = getrlimit(RLIMIT_CPU, &rlim);
      slog(LOG_ALL, 0, "RLIMIT CPU: %d / %d.", rlim.rlim_cur, rlim.rlim_max);

      n = getrlimit(RLIMIT_DATA, &rlim);
      slog(LOG_ALL, 0, "RLIMIT DATA: %d / %d.", rlim.rlim_cur, rlim.rlim_max);

      n = getrlimit(RLIMIT_STACK, &rlim);
      slog(LOG_ALL, 0, "RLIMIT STACK: %d / %d.", rlim.rlim_cur, rlim.rlim_max);

      n = getrlimit(RLIMIT_CORE, &rlim);
      slog(LOG_ALL, 0, "RLIMIT CORE: %d / %d.", rlim.rlim_cur, rlim.rlim_max);

      n = getrlimit(RLIMIT_RSS, &rlim);
      slog(LOG_ALL, 0, "RLIMIT RSS: %d / %d.", rlim.rlim_cur, rlim.rlim_max);
   }
#endif

   while ((pos < argc) && (*(argv[pos]) == '-'))
   {
      switch (*(argv[pos] + 1))
      {
	case '?':
	case 'h':
	 ShowUsage(argv[0]);
	 exit(0);

	case 'c':
	 slog(LOG_OFF,0,"Converting player file mode.");
	 player_convert = TRUE;
	 pos++;
	 player_name_list = create_namelist();
	 while ((pos < argc) && (*(argv[pos]) != '-'))
	 {
	    char *name = strrchr(argv[pos], '/');

	    if (name && strchr(name, '.') == NULL)
	      player_name_list = add_name(name+1, player_name_list);
	    pos++;
	 }

	 if (pos >= argc)
	   pos--;

	 while (*(argv[pos]) == '-')
	   pos--;
	 break;

	case 'p':
	 sp = pos;
	 pos++;
	 while ((pos < argc) && (*(argv[pos]) != '-'))
	 {
	    char *name = strrchr(argv[pos], '/');

	    if (!str_is_empty(name+1))
	      persist_namelist.AppendName(name+1);
	    pos++;
	 }

	 if (pos >= argc)
	   pos--;

	 if (sp != pos)
	   while (*(argv[pos]) == '-')
	     pos--;
	 break;

	case 'd':
	 if (*(argv[pos] + 2))
	   strcpy(libdir,argv[pos] + 2);
	 else if (++pos < argc)
	   strcpy(libdir,argv[pos]);
	 else
	 {
	    slog(LOG_OFF,0,"Directory arg expected after option -d.");
	    exit(1);
	 }
	 break;

	case 'z':
	 if (*(argv[pos] + 2))
	   strcpy(zondir,argv[pos] + 2);
	 else if (++pos < argc)
	   strcpy(zondir,argv[pos]);
	 else
	 {
	    slog(LOG_OFF,0,"Directory arg expected after option -z.");
	    exit(1);
	 }
	 break;

	default:
	 slog(LOG_OFF,0,"Unknown option -% in argument string.",
	      *(argv[pos] + 1));
	 break;
      }
      pos++;
   }

   if (pos < argc)
     if (!isdigit(*argv[pos]))
     {
	ShowUsage(argv[0]);
	exit(1);
     }

   slog(LOG_OFF,0, "Using 'root' as base directory.");

#if defined(GENERIC_SYSV) || defined(HPUX)
   srand48(time(0));
#elif defined(LINUX)
   /* Other used routines are declared obsolete by SVID3 */
   srand(time(0));
#else
   srandom(time(0));
#endif

   run_the_game();
   return(0);
}


/* Init sockets, run game, and cleanup sockets */
void run_the_game(void)
{
#ifdef PROFILE
   extern char etext;
#endif

   void signal_setup(void);
   int load(void);
   void coma(int s);

   /*
    * If you want BOOT TIME included in the PROFILE, use 
    * monstartup HERE! Otherwise it defaults to the one below.
    *
   #ifdef PROFILE
   monstartup((int) 2, etext);
   #endif
   */

   descriptor_list = NULL;

   slog(LOG_OFF, 0, "Reading server configuration.");

   g_cServerConfig.Boot();
  
   slog(LOG_OFF,0,"Opening mother connection on port %d.",
	g_cServerConfig.m_nMotherPort);

   init_mother(g_cServerConfig.m_nMotherPort);

   if (g_cServerConfig.m_bLawful && load() >= 6)
   {
      slog(LOG_OFF,0,"System load too high at startup.");
      coma(0);
   }

   slog(LOG_OFF,0,"Signal trapping.");
   signal_setup();

   boot_db();

/*
#ifdef PROFILE
   monstartup((int) 2, etext);
#endif
*/
   slog(LOG_OFF,0,"Priming eventqueue.");

   event_enq(PULSE_SEC * SECS_PER_REAL_MIN * 2,	check_idle_event,	0, 0);
   event_enq(PULSE_VIOLENCE,			perform_violence_event,	0, 0);
   event_enq(PULSE_POINTS,			point_update_event,	0, 0);
   event_enq(PULSE_SEC * SECS_PER_MUD_HOUR,	food_update_event,	0, 0);
   event_enq(PULSE_SEC * SECS_PER_REAL_MIN * 5,	update_crimes_event,	0, 0);
   event_enq(PULSE_SEC * SECS_PER_REAL_MIN * 10,check_reboot_event,	0, 0);
   event_enq(PULSE_SEC * 120,			swap_check,		0, 0);

   slog(LOG_OFF,0,"Entering game loop.");

#ifdef AMIGA1
   OpenTimer();
#endif

   game_loop();

#ifdef AMIGA1
   CloseTimer();
#endif

   CaptainHook.Close();

#ifdef PROFILE
   #if !defined(LINUX) 
   monitor(0);
   #endif
#endif

   fclose_cache();

   slog(LOG_OFF, 0, "Memory used when shutting down: %d bytes.",
	memory_total_alloc);

   void db_shutdown(void);
   db_shutdown();

   slog(LOG_OFF, 0, "Memory used at final checkpoint: %d bytes.",
	memory_total_alloc);

   if (mud_reboot)
   {
      slog(LOG_OFF,0,"Rebooting.");
      exit(42);
   }
   else
   {
      slog(LOG_OFF,0,"Normal termination of game.");
      exit(0);
   }
}

static void event_process(void)
{
   struct eventq_elem tmp_event, tmp, *newtop, *child;
   int child_index;

   while (events && event_heap->when <= tics)
   {
      /* dequeue & process event */
      tmp_event = *event_heap;
      *event_heap = *(event_heap + --events);
      child_index = 0;
      newtop = event_heap;
      for (;;)
      {
	 child_index = ((child_index + 1) << 1) - 1;
	 if (child_index >= events)
	   break;
	 /* select smaller child */
	 if (child_index + 1 < events &&
	     (event_heap + child_index)->when >
	     (event_heap + child_index + 1)->when)
	   child_index++;
	 child = event_heap + child_index;
	 if (child->when < newtop->when)
	 {
	    tmp = *child;
	    *child = *newtop;
	    *newtop = tmp;
	    newtop = child;
	 }
	 else
	   break;
      }
      if (tmp_event.func)
	(tmp_event.func)(tmp_event.arg1, tmp_event.arg2);
   }
}



void game_loop()
{
   struct timeval now, old;
   long delay;

   void clear_destructed(void);

   tics = 0;
   gettimeofday(&old, (struct timezone *) 0);

   while (!mud_shutdown)
   {
      /* work */
      game_event();

      event_process();

      clear_destructed();
 
      tics++;

      /* Timer stuff. MUD is always at least OPT_USEC useconds in making
	 one cycle. */

      gettimeofday(&now, (struct timezone *) 0);

      delay = OPT_USEC - (1000000L*(now.tv_sec - old.tv_sec) +
			  (now.tv_usec - old.tv_usec));

      old = now;

      if (delay > 0)
      {
	 usleep(delay);
	 old.tv_usec += delay; /* This time has passed in usleep.
				  Overrun is not important. */
      }
   }

   multi_close_all();
}



/* Accept new connects, relay commands, and call 'heartbeat-functs' */
void game_event(void)
{
   int i;
   char *pcomm = NULL;
   struct descriptor_data *point;
   static char buf[80];
   static struct timeval null_time = {0, 0};

   void multi_close(struct multi_element *pe);
   void multi_clear(void);

   i = CaptainHook.Wait(&null_time);

   if (i < 0)
   {
      slog(LOG_ALL, 0, "Captain Hook error %d", errno);
      return;
   }

   /* process_commands; */
   for (point = descriptor_list; point; point = next_to_process)
   {
      next_to_process = point->next;

      if (--(point->wait) <= 0 && !point->qInput.IsEmpty())
      {
	 struct cQueueElem *qe = point->qInput.GetHead();
	 pcomm = (char *) qe->Data();
	 qe->SetNull();
	 delete qe;

	 point->wait = 1;
	 point->timer = 0;
	 point->prompt_mode = PROMPT_EXPECT;

	 if (point->snoop.snoop_by)
	 {
	    char buffer[MAX_INPUT_LENGTH + 10];
	    sprintf(buffer, "%s%s\n\r", SNOOP_PROMPT, pcomm);
	    send_to_descriptor(buffer, CHAR_DESCRIPTOR(point->snoop.snoop_by));
	 }

	 point->fptr(point, pcomm);
	 free(pcomm);
      }
   }

   /* give the people some prompts */
   for (point = descriptor_list; point; point = point->next)
   {
      if (point->prompt_mode == PROMPT_EXPECT)
      {
	 if (point->editing)
	   strcpy(buf, "] ");
	 else
	 {
	    *buf = 0;
	    if (descriptor_is_playing(point) &&
		!(point->character && IS_PC(point->character) &&
		  IS_SET(PC_FLAGS(point->character), PC_COMPACT)))
	      send_to_descriptor("\n\r", point);

	    if (descriptor_is_playing(point))
	    {
	       if (IS_NPC(point->character) ||
		   IS_SET(PC_FLAGS(point->character), PC_PROMPT))
	       {
		  if (IS_PC(point->character) && IS_CREATOR(point->character))
		    if (UNIT_MINV(point->character))
		      sprintf(buf, "%d> ", UNIT_MINV(point->character));
		    else
		      strcpy(buf, "> ");
		  else
		    sprintf(buf, g_cServerConfig.m_sColor.pPrompt,
			    CHAR_MANA(point->character),
			    CHAR_ENDURANCE(point->character),
			    (signed long) UNIT_HIT(point->character));
	       }
	       else
		 strcpy(buf, "> ");
	    }
	 }

	 if (*buf)
	 {
	    protocol_send_text(point->multi, point->id,
			       buf, MULTI_PROMPT_CHAR);
	 }

	 point->prompt_mode = PROMPT_IGNORE;
      } /* if  == PROMPT_EXPECT */
   } /* for */

   multi_clear();  /* Close all descriptors with no associated multi */
}

/* ********************************** *
 *     Some eventqueue-handling       *	
 * ********************************** */

/* insert new event in heap */
void event_enq(int when, void (*func)(void *, void *), void *arg1, void *arg2)
{
  struct eventq_elem *end, *parent, tmp;
  int parent_index;

  if (when <= 0)
  {
     slog(LOG_ALL,0, "Error: %d EVENT", when);
  }

  if ((events + 1) > heapspace)
  {
    if (!heapspace)
      CREATE(event_heap, struct eventq_elem, heapspace =
	     HEAPSPACE_INCREMENT);
    else
      RECREATE(event_heap, struct eventq_elem, heapspace +=
	       HEAPSPACE_INCREMENT);
  }
  end = event_heap + events;
  end->when = tics + when;
  end->func = func;
  end->arg1 = arg1;
  end->arg2 = arg2;
  /* roll event into its proper place in da heap */
  parent_index = events;
  for (;;)
  {
    if (!parent_index)
      break;
    parent_index = ((parent_index + 1) >> 1) - 1;
    parent = event_heap + parent_index;
    if (end->when < parent->when)
    {
      tmp = *parent;
      *parent = *end;
      *end = tmp;
      end = parent;
    }
    else
      break;
  }
  events++;
}


/* deallocate event and remove from queue */
void event_deenq(void (*func)(void *, void *), void *arg1, void *arg2)
{
  int i;

  for (i = 0; i < events; i++)
    if (event_heap[i].func == func && event_heap[i].arg1 == arg1 &&
	event_heap[i].arg2 == arg2)
      event_heap[i].func = 0;
}


/* deallocate event and remove from queue */
/* By MS. NULL is considered a match... use with care. */
void event_deenq_relaxed(void (*func)(void *, void *), void *arg1, void *arg2)
{
  int i;

  for (i = 0; i < events; i++)
    if ((event_heap[i].func == func) && (!arg1 || (event_heap[i].arg1==arg1))
	&& (!arg2 || (event_heap[i].arg2 == arg2)))
      event_heap[i].func = 0;
}


/* events */

void check_idle_event(void *p1, void *p2)
{
  void check_idle(void);

  check_idle();
  event_enq(PULSE_ZONE, check_idle_event, 0, 0);
}


void perform_violence_event(void *p1, void *p2)
{
   CombatList.PerformViolence();
   event_enq(PULSE_VIOLENCE, perform_violence_event, 0, 0);
}

void point_update_event(void *p1, void *p2)
{
  point_update();
  event_enq(PULSE_POINTS, point_update_event, 0, 0);
}

void food_update_event(void *p1, void *p2)
{
  food_update();
  event_enq(WAIT_SEC*SECS_PER_MUD_HOUR, food_update_event, 0, 0);
}

void update_crimes_event(void *p1, void *p2)
{
   update_crimes();
   event_enq(1200, update_crimes_event, 0, 0);
}

void check_reboot_event(void *p1, void *p2)
{
  check_reboot();
  event_enq(2400, check_reboot_event, 0, 0);
}


/* utility procedure */
struct timeval timediff(struct timeval *a, struct timeval *b)
{
  struct timeval rslt, tmp;

  tmp = *a;

  if ((rslt.tv_usec = tmp.tv_usec - b->tv_usec) < 0)
  {
    rslt.tv_usec += 1000000;
    --(tmp.tv_sec);
  }
  if (tmp.tv_sec - b->tv_sec < 0)
  {
    rslt.tv_usec = 0;
    rslt.tv_sec =0;
  }
  else
    rslt.tv_sec = tmp.tv_sec - b->tv_sec;

  return rslt;
}

