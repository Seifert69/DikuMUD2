/* *********************************************************************** *
 * File   : system.c                                  Part of Valhalla MUD *
 * Version: 2.01                                                           *
 * Author : seifert@diku.dk (formerly quinn@diku.dk & bombman@diku.dk)     *
 *                                                                         *
 * Purpose: Network communication and other system dependant things.       *
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

/* Sun Jun 26 1994 Gnort: Added Amiga support				   */
/* Thu Jan 11 1995 Gnort: Started work on moving Amiga support out of	   *
 *			  server, and into mplex'er			   */


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <netinet/tcp.h>

#ifndef DOS
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#endif
#ifdef LINUX
#include <sys/resource.h>
#endif

#include <fcntl.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "structs.h"
#include "utils.h"
#include "system.h"
#include "db.h"
#include "utility.h"
#include "comm.h"
#include "textutil.h"
#include "ban.h"
#include "handler.h"
#include "sysport.h"
#include "files.h"
#include "protocol.h"
#include "unixshit.h"
#include "main.h"
#include "account.h"
#include "limits.h"
#include "db_file.h"
#include "str_parse.h"
#include "common.h"

/* extern vars */
extern struct descriptor_data *descriptor_list;
extern struct descriptor_data *next_to_process;

class cMotherHook : public cHook
{
  public:
   void Input(int nFlags);
   void Close(void);
};

class cMotherHook MotherHook;



void MplexSendSetup(struct descriptor_data *d)
{
   assert(d);

   protocol_send_setup(d->multi, d->id, &PC_SETUP(d->character));
}

/* ----------------------------------------------------------------- */
/*                                                                   */
/*                      Descriptor Routines                          */
/*                                                                   */
/* ----------------------------------------------------------------- */

/* Call only once when creating a new char (guest)    */
void init_char(struct unit_data *ch)
{
   int new_player_id(void);

   int i, init_skills = 0;
   
   extern sbit32 player_id;
   
   int required_xp(int level);
   
   if (g_cServerConfig.m_bBBS)
   {
      PC_SETUP_ECHO(ch)      = TRUE;
      PC_SETUP_REDRAW(ch)    = TRUE;
      PC_SETUP_EMULATION(ch) = TERM_ANSI;
      PC_SETUP_TELNET(ch)    = FALSE;

      SET_BIT(PC_FLAGS(ch), PC_INFORM);
   }
   else
   {
      PC_SETUP_ECHO(ch)      = FALSE;
      PC_SETUP_REDRAW(ch)    = FALSE;
      PC_SETUP_EMULATION(ch) = TERM_TTY;
      PC_SETUP_TELNET(ch)    = TRUE;
   }
   
   PC_SETUP_WIDTH(ch)    = 80;
   PC_SETUP_HEIGHT(ch)   = 15;
   PC_SETUP_COLOUR(ch)   =  0;
   PC_CRACK_ATTEMPTS(ch) = 0;

   account_defaults(ch);
   
   PC_INFO(ch)           = NULL;
   PC_QUEST(ch)          = NULL;
   UNIT_EXTRA_DESCR(ch)  = NULL;
   PC_HOME(ch)           = NULL;
   PC_GUILD(ch)          = NULL;
   
   CHAR_POS(ch)          = POSITION_STANDING;
   CHAR_SPEED(ch)        = SPEED_DEFAULT;
   CHAR_RACE(ch)         = RACE_HUMAN;
   CHAR_SEX(ch)          = SEX_MALE;

   PC_TIME(ch).connect = PC_TIME(ch).birth = PC_TIME(ch).creation = time(0);
   PC_TIME(ch).played  =   0;
   PC_LIFESPAN(ch)     = 100;

   CHAR_EXP(ch)   =  0;
   CHAR_LEVEL(ch) =  0;
   PC_ID(ch)      = -1;
   PC_CRIMES(ch)  =  0;
   
   PC_ABILITY_POINTS(ch) = 0;
   PC_SKILL_POINTS(ch)   = 0;
   
   /* *** if this is our first player --- he be God *** */
   if (player_id == -7)
   {
      slog(LOG_ALL, 0, "The very first GOD is created!");
      CHAR_EXP(ch) = required_xp(255);
      CHAR_LEVEL(ch) = 255;

      init_skills = 200;

      CHAR_RACE(ch) = RACE_HUMAN;
      CHAR_SEX(ch)  = SEX_MALE;	/* God is male ;-) */

      player_id = 1;

      PC_ID(ch) = new_player_id();
   }
   
   for (i = 0; i < SPL_TREE_MAX; i++)
   {
      PC_SPL_SKILL(ch, i) = init_skills;
      PC_SPL_LVL(ch, i) = 0;
      PC_SPL_COST(ch, i) = 0;
   }
   for (i = 0; i < WPN_TREE_MAX; i++)
   {
      PC_WPN_SKILL(ch, i) = init_skills; 
      PC_WPN_LVL(ch, i) = 0;
      PC_WPN_COST(ch, i) = 0;
   }
   for (i = 0; i < SKI_TREE_MAX; i++)
   {
      PC_SKI_SKILL(ch, i) = init_skills;
      PC_SKI_LVL(ch, i) = 0;
      PC_SKI_COST(ch, i) = 0;
   }
   for (i = 0; i < ABIL_TREE_MAX; i++)
   {
      CHAR_ABILITY(ch, i) = init_skills;
      PC_ABI_LVL(ch,i) = 0;
      PC_ABI_COST(ch,i) = 0;
   }
   
   /* CHAR_HAND_QUALITY(ch) = 15;  15% */
   CHAR_ATTACK_TYPE(ch) = WPN_FIST;
   CHAR_NATURAL_ARMOUR(ch) = ARM_CLOTHES;
   
   /* UNIT_TGH(ch) = 0; */
   UNIT_HIT(ch) = UNIT_MAX_HIT(ch) = 1;
   
   CHAR_MANA(ch) = mana_limit(ch);
   CHAR_ENDURANCE(ch) = move_limit(ch);
   CHAR_LAST_ROOM(ch) = NULL;
   
   CHAR_FLAGS(ch) = 0;
   SET_BIT(CHAR_FLAGS(ch), CHAR_PROTECTED);
   
   for (i = 0; i < 3; i++)
     PC_COND(ch, i) = (CHAR_LEVEL(ch) >= 200 ? 48 : 24);
   
   PC_COND(ch, DRUNK) = 0;

   set_title(ch);
}

int no_connections = 0;         /* No of used descriptors                    */
int max_no_connections = 0;     /* Statistics                                */

descriptor_data::descriptor_data(cMultiHook *pe)
{
   static int nid = 0;

   void nanny_get_name(struct descriptor_data *d, char *arg);
  
   no_connections++;
   if (no_connections > max_no_connections)
     max_no_connections = no_connections;

   /* init desc data */
   multi          = pe;
   id             = ++nid;

   state          = 0;
   fptr           = nanny_get_name;
   wait           = 1;
   timer          = 0;
   prompt_mode    = PROMPT_SENT;
   *last_cmd      = *history = '\0';

   host[0] = 0;
   nPort   = 0;
   nLine   = 255;

   localstr       = NULL;
   postedit       = NULL;
   editing        = NULL;
   editref        = NULL;

   original       = 0;
   snoop.snooping = 0;
   snoop.snoop_by = 0;
   replyid        = (ubit32) -1;

   /* Make a new PC struct */
   character       = new (class unit_data)(UNIT_ST_PC);
   init_char(character);
   CHAR_DESCRIPTOR(character) = this;

   /* prepend to list */
   next            = descriptor_list;
   descriptor_list = this;
}

void descriptor_data::RemoveBBS(void)
{
   if (nLine != 255)
   {
      char buf[512];

      sprintf(buf, BBS_DIR"%d.%d", nPort, nLine);
      remove(buf);
   }
}

void descriptor_data::CreateBBS(void)
{
   if (nLine != 255)
   {
      char buf[512];
      FILE *f;

      sprintf(buf, BBS_DIR"%d.%d", nPort, nLine);

      if (!character)
      {
	 slog(LOG_ALL, 0, "No character in %s.", buf);
	 return;
      }

      f = fopen(buf, "wb");

      if (!f)
      {
	 slog(LOG_ALL, 0, "Could not create %s.", buf);
	 return;
      }

      if (account_is_overdue(this->character))
	fprintf(f, "1\n");
      else
	fprintf(f, "0\n");

      fprintf(f, "%d\n", PC_ACCOUNT(this->character).total_credit);
      fprintf(f, "%s\n", PC_FILENAME(this->character));
      fprintf(f, "%d\n", CHAR_LEVEL(this->character));
      fprintf(f, "%d\n", PC_ACCESS_LEVEL(this->character));

      fclose(f);
   }
}

descriptor_data::~descriptor_data(void)
{
   RemoveBBS();
   nLine = 255;
   no_connections--;
}


/* Pass the multi-fd which is to be associated with this new descriptor */
/* Note that id zero signifies that mplex descriptor has no mplex'er    */
struct descriptor_data *descriptor_new(class cMultiHook *pe)
{
   return new descriptor_data(pe);
}

/* Flush should be set to true, when there is noone to receive the  */
/* data, i.e. when for example the mplex'er is down or if the       */
/* mplex'er has sent a terminate request to the server. Nothing     */
/* bad happens if you mess up, but the mplex'er receives data to    */
/* an unknown id...                                                 */
void descriptor_close(struct descriptor_data *d, int bSendClose)
{
   struct descriptor_data *tmp;
   class cMultiHook *multi = NULL;

   void unsnoop(struct unit_data *ch, int mode);
   void unswitchbody(struct unit_data *npc);
   int is_destructed(int type, void *ptr);

   assert(d->character);

   /* Descriptor must be either in the game (UNIT_IN) or in menu.  */
   /* If unit has been extracted, then all his data is half erased */
   /* (affects, etc) and he shall not be saved!                    */
   if (!UNIT_IN(d->character)) /* In menu - extract completely */
   {
      assert(!UNIT_CONTAINS(d->character));
      assert(!UNIT_IN(d->character));
      assert(!d->character->gnext);
      assert(!d->character->gprevious);

      /* Important that we set to NULL before calling extract,
	 otherwise we just go to the menu... ... ... */
      CHAR_DESCRIPTOR(d->character) = NULL;
      extract_unit(d->character);
      d->character = NULL;
      /* Too much log slog(LOG_ALL, "Losing descriptor from menu."); */
   }
   else
   {
      if (d->localstr)
	free(d->localstr);

      d->localstr = NULL;
      d->postedit = NULL;
      d->editing  = NULL;
      d->editref  = NULL;

      if (CHAR_IS_SNOOPING(d->character) || CHAR_IS_SNOOPED(d->character))
	unsnoop(d->character, 1);

      if (CHAR_IS_SWITCHED(d->character))
	unswitchbody(d->character);
   
      assert(!d->snoop.snooping && !d->snoop.snoop_by);
      assert(!d->original);

      act("$1n has lost $1s link.", A_HIDEINV, d->character, 0, 0, TO_ROOM);
      slog(LOG_BRIEF, UNIT_MINV(d->character),
	   "Closing link and making link dead: %s.",
	   UNIT_NAME(d->character));

      if (!is_destructed(DR_UNIT, d->character))
      {
	 void disconnect_game(struct unit_data *pc);

	 disconnect_game(d->character);

	 if (!PC_IS_UNSAVED(d->character))
	 {
	    /* We need to save player to update his time status! */
	    save_player(d->character); /* Save non-guests */
	    save_player_contents(d->character, TRUE);

	    create_fptr(d->character, SFUN_LINK_DEAD, 0, SFB_CMD, NULL);
	 }
	 else
	   extract_unit(d->character); /* We extract guests */
      }

      /* Important we set tp null AFTER calling save - otherwise
	 time played does not get updated. */
      CHAR_DESCRIPTOR(d->character) = NULL;
      d->character = NULL;
   }

   if (bSendClose && d->multi->IsHooked())
     protocol_send_close(d->multi, d->id);

   if (next_to_process == d)  /* to avoid crashing the process loop */
     next_to_process = next_to_process->next;

   if (d == descriptor_list) /* this is the head of the list */
     descriptor_list = descriptor_list->next;
   else  /* This is somewhere inside the list */
   {
      /* Locate the previous element */
      for (tmp = descriptor_list; tmp && (tmp->next != d); tmp = tmp->next)
	;
      tmp->next = d->next;
   }

   delete d;
}

/* ----------------------------------------------------------------- */
/*                                                                   */
/*                        Multi-Protocol-Procedures                  */
/*                                                                   */
/* ----------------------------------------------------------------- */

cMultiMaster::cMultiMaster(void)
{
   nCount = 0;
}

cMultiHook::cMultiHook(void)
{
   succ_err = 0;
}

void cMultiHook::Input(int nFlags)
{
   if (nFlags & SELECT_EXCEPT)
   {
      slog(LOG_ALL, 0, "Freaky multi!");
      Close();
   }
   if (nFlags & SELECT_READ)
   {
      int n;

      for (;;)
      {
	 n = Read();
	 if ((n == 0) || (n == -1))
	   break;
      }

      if (n == -1)
      {
	 slog(LOG_OFF, 0, "ERROR READING FROM MPLEX.");
	 Close();
      }
      else if (n == -2)
      {
	 slog(LOG_OFF, 0, "MPLEX PROTOCOL ERROR.");
      }
   }
}


void cMultiHook::Close(void)
{
   if (!IsHooked())
     return;

   slog(LOG_ALL, 0, "Closing connection to multi host.");

   Unhook();

   Multi.nCount--;
}


int cMultiHook::Read(void)
{
   struct descriptor_data *d = NULL;
   int p, n;
   ubit16 id;
   ubit16 len;
   char *data;
   ubit8 text_type;

   extern char *logo;

   p = protocol_parse_incoming(this, &id, &len, &data, &text_type);

   if (p <= 0)
     return p;

   if (id != 0)
   {
      for (d=descriptor_list; d; d=d->next)
	if (d->id == id)
	{
	   assert(d->multi == this);
	   break;
	}

      if (d == NULL)
      {
	 /* This could perhaps occur if a connected player issues a command
	    simultaneously with the server throwing the person out.
	    I will simply test if d == NULL in all cases below and then
	    ignore the message. */

	 slog(LOG_ALL, 0, "No destination ID (%d).", id);

	 if (succ_err++ > 5)
	 {
	    Close();
	    slog(LOG_ALL, 0, "Lost track of multi stream.");
	    if (data)
	      free(data);
	    return -1;
	 }
      }
      else
	succ_err = 0;
   }

   switch (p)
   {
     case MULTI_TERMINATE_CHAR:
      /* This is very nice, but it prevents descriptor_close to send
	 a connection_close to the mplex'er */
      if (d)
	descriptor_close(d, FALSE);
      if (data)
	free(data);
      break;

     case MULTI_CONNECT_REQ_CHAR:
      d = descriptor_new(this);
      protocol_send_confirm(this, d->id);
      send_to_descriptor(g_cServerConfig.m_pLogo, d);
      send_to_descriptor("By what name do they call you? ", d);
      break;

     case MULTI_HOST_CHAR:
      if (d && data)
      {
	 ubit8 *b = (ubit8 *) data;

	 d->nPort = bread_ubit16(&b);
	 d->nLine = bread_ubit8(&b);
	 strncpy(d->host, (char *) b, sizeof(d->host));
	 d->host[sizeof(d->host)-1] = 0;
      }
      if (data)
        free(data);
      break;

     case MULTI_TEXT_CHAR:
      if (d)
	d->qInput.Append(new cQueueElem(data, FALSE));
      /* Kept in queue */
      break;

     default:
      slog(LOG_ALL, 0, "Illegal unexpected unique multi character.");
      Close();
      if (data)
	free(data);
      return -1;
   }

   return p;
}



void multi_clear(void)
{
   struct descriptor_data *nextd, *d;

   for (d = descriptor_list; d; d = nextd)
   {
      nextd = d->next;
      if (!d->multi->IsHooked())
	descriptor_close(d);
   }
}


void multi_close_all(void)
{
   int i;

   slog(LOG_BRIEF, 0,"Closing all multi connections.");

   for (i=0; i < MAX_MULTI; i++)
     Multi.Multi[i].Close();

   multi_clear();
}


/* ----------------------------------------------------------------- */
/*                                                                   */
/*                         MotherHook                                */
/*                                                                   */
/* ----------------------------------------------------------------- */

void cMotherHook::Input(int nFlags)
{
   if (nFlags & SELECT_EXCEPT)
   {
      slog(LOG_ALL, 0, "Mother connection closed down.");
      Close();
   }

   if (nFlags & SELECT_READ)
   {
      struct sockaddr_in isa;
      int i, t;
      socklen_t len;       //MS2020

      len = sizeof(isa);

      if ((t = accept(this->tfd(), (struct sockaddr *) &isa, &len)) < 0)
      {
	 slog(LOG_ALL, 0, "Mother accept error %d", errno);
	 return;
      }

      if (!g_cServerConfig.ValidMplex(&isa))
      {
	 slog(LOG_ALL, 0, "Mplex not from trusted host, terminating.");
	 close(t);
	 return;
      }

      i = fcntl(t, F_SETFL, FNDELAY);

      if (i == -1)
	error(HERE, "Noblock");

      int n;
      n = setsockopt(t, IPPROTO_TCP, TCP_NODELAY, &i, sizeof(i));
      if (n == -1)
      {
	 error(HERE, "No Setsockopt()");
	 exit(1);
      }

      for (i=0; i < MAX_MULTI; i++)
	if (!Multi.Multi[i].IsHooked())
	  break;

      if ((i >= MAX_MULTI) || Multi.Multi[i].IsHooked())
      {
	 slog(LOG_ALL, 0, "No more multi connections allowed");
	 close(t);
	 return;
      }

      CaptainHook.Hook(t, &Multi.Multi[i]);

      Multi.nCount++;
   
      slog(LOG_ALL,0, "A multi-host has connected to the game.");
   }
}


void cMotherHook::Close(void)
{
   multi_close_all();

   Unhook();
}


void init_mother(int nPort)
{
   int n, fdMother;
   struct linger ld;
   struct sockaddr_in server_addr;

   memset(&server_addr, 0, sizeof(struct sockaddr_in));
   server_addr.sin_family      = AF_INET;
   server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   server_addr.sin_port        = htons(nPort);

   fdMother = socket(AF_INET, SOCK_STREAM, 0);

   if (fdMother == -1)
   {
      slog(LOG_OFF, 0, "Can't open Mother Connection");
      exit(0);
   }

   n = 1;
   if (setsockopt(fdMother,SOL_SOCKET,SO_REUSEADDR,(char *) &n, sizeof(n)) < 0)
   {
      close(fdMother);
      slog(LOG_OFF, 0, "OpenMother setsockopt REUSEADDR");
      exit(0);
   }

   ld.l_onoff = 0;
   ld.l_linger = 1000;

   if (setsockopt(fdMother,SOL_SOCKET,SO_LINGER, (char *)&ld, sizeof(ld)) < 0)
   {
      close(fdMother);
      slog(LOG_OFF, 0, "OpenMother setsockopt LINGER");
      exit(0);
   }

   n = bind(fdMother, (struct sockaddr *) &server_addr,
            sizeof(struct sockaddr_in));

   if (n != 0)
   {
      close(fdMother);
      slog(LOG_OFF, 0, "Can't bind Mother Connection port %d (errno %d).",
	   nPort, errno);
      exit(0);
   }

   n = listen(fdMother, 5);

   if (n != 0)
   {
      close(fdMother);
      slog(LOG_OFF, 0, "Can't listen on Mother Connection.");
      exit(0);
   }

   n = fcntl(fdMother, F_SETFL, FNDELAY);

   if (n == -1)
   {
      close(fdMother);
      slog(LOG_OFF, 0, "Non blocking set error.");
      exit(1);
   }

   int i;
   n = setsockopt(fdMother, IPPROTO_TCP, TCP_NODELAY, &i, sizeof(i));
   if (n == -1)
   {
      close(fdMother);
      slog(LOG_OFF, 0, "Setsockopt TCP_NODELAY failed.");
      exit(1);
   }

   CaptainHook.Hook(fdMother, &MotherHook);
}



/* Returns how much memory is in use by system. This may help spotting
   possible fragmentation of memory. */
void system_memory(struct unit_data *ch)
{
#ifdef LINUX
   struct rusage rusage_data;
   int n;
   char Buf[1024];

   n = getrusage(RUSAGE_CHILDREN, &rusage_data);

   if (n != 0)
     slog(LOG_ALL, 0, "System memory status error.");
   else
   {
      sprintf(Buf,
	      "Vol. Switches       %8ld\n\r"
	      "Max RSS             %8ld\n\r"
	      "Shared memory size  %8ld\n\r"
	      "Unshared data size  %8ld\n\r"
	      "Unshared stack size %8ld\n\r\n\r",
	      rusage_data.ru_nvcsw,
	      rusage_data.ru_maxrss,
	      rusage_data.ru_ixrss,
	      rusage_data.ru_isrss,
	      rusage_data.ru_idrss);
      send_to_char(Buf, ch);
   }
#endif
}


