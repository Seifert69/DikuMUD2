/* *********************************************************************** *
 * File   : zon_basis.c                               Part of Valhalla MUD *
 * Version: 1.01                                                           *
 * Author : seifert@diku.dk & gnort@daimi.aau.dk                           *
 *                                                                         *
 * Purpose: basis zone routines.                                           *
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
/* 12-Jan-95 Gnort:  Fixed security-hole in admin-obj...                   */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "structs.h"
#include "utils.h"
#include "skills.h"
#include "textutil.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "limits.h"
#include "justice.h"
#include "affect.h"
#include "magic.h"
#include "utility.h"
#include "money.h"
#include "spec_assign.h"
#include "main.h"
#include "fight.h"
#include "common.h"

#define DEFAULT_ENTRY_ZONE  "udgaard"
#define DEFAULT_ENTRY_NAME  "temple"

#define PC_DEATHLOC_NAME    "heaven_entry"
#define PC_PREDEATHLOC_NAME "deathseq_room"
#define PC_TIMEOUTLOC_NAME  "timeout"
#define VOID_ROOM           "void"
#define ZOMBIE_NAME         "zombie"
#define DEMIGOD_NAME        "demigod"
#define MAIL_NOTE_NAME      "letter"
#define CORPSE_NAME         "corpse"
#define HEAD_NAME           "head"

#define PC_DEATHOBJ_NAME    "death_seq"
#define DESTROY_ROOM        "destroy_room"


struct unit_data *void_room = 0;
struct unit_data *destroy_room = 0;
struct unit_data *heaven_room = 0;
struct unit_data *seq_room    = 0;
struct unit_data *time_room   = 0;
struct unit_data *entry_room  = 0;

struct file_index_type *demigod_fi  = 0; /* Default demigod shape */
struct file_index_type *zombie_fi   = 0;
struct file_index_type *letter_fi   = 0;
struct file_index_type *corpse_fi   = 0;
struct file_index_type *head_fi     = 0;
struct file_index_type *deathobj_fi = 0;
struct file_index_type *beginner_note = 0;

extern char zondir[];


void basis_boot(void)
{
   void_room = world_room(BASIS_ZONE, VOID_ROOM);
   assert(void_room);

   destroy_room = world_room(BASIS_ZONE, DESTROY_ROOM);
   assert(destroy_room);

   heaven_room = world_room(BASIS_ZONE, PC_DEATHLOC_NAME);
   assert(heaven_room);

   seq_room    = world_room(BASIS_ZONE, PC_PREDEATHLOC_NAME);
   assert(seq_room);

   time_room   = world_room(BASIS_ZONE, PC_TIMEOUTLOC_NAME);
   assert(time_room);

   letter_fi   = find_file_index(BASIS_ZONE, MAIL_NOTE_NAME);
   assert(letter_fi);

   zombie_fi   = find_file_index(BASIS_ZONE, ZOMBIE_NAME);
   assert(zombie_fi);

   demigod_fi  = find_file_index(BASIS_ZONE, DEMIGOD_NAME);
   if (demigod_fi == NULL)
     demigod_fi = zombie_fi;

   head_fi   = find_file_index(BASIS_ZONE, HEAD_NAME);
   assert(head_fi);

   corpse_fi   = find_file_index(BASIS_ZONE, CORPSE_NAME);
   assert(corpse_fi);

   deathobj_fi = find_file_index(BASIS_ZONE, PC_DEATHOBJ_NAME);
   assert(deathobj_fi);

   entry_room  = world_room(DEFAULT_ENTRY_ZONE, DEFAULT_ENTRY_NAME);
   if (entry_room == NULL)
   {
      slog(LOG_ALL, 0, "Entry room does not exist, using void.");
      entry_room = void_room;
   }
}

/* These events happen rarely (daemon is every 30 minutes) */
void random_event_world(void)
{
  struct unit_data *u;

  extern struct unit_data *unit_list;

  for (u = unit_list; u; u = u->gnext)
  {
     if (IS_NPC(u) && !number(0, 100))
     {
	set_reward_char(u, 0);
	return;
     }
  }
}


/* These events happen rarely (daemon is every 30 minutes) */
void random_event_player(struct unit_data *u, struct unit_data *daemon)
{
  int i;
  struct unit_data *tmpu;

  extern struct unit_data *unit_list;

  if (u == NULL)
  {
     random_event_world();
     return;
  }

  char mbuf[MAX_INPUT_LENGTH] = {0};

  switch ( number(0, 11) )
  {
    case 0:
    case 1:
    case 2:
    case 3:
     random_event_world();
     return;

   case 4:
    act("You feel an itch where you can not scratch.",
	A_ALWAYS, u, 0, 0, TO_CHAR);
    break;

   case 5:
    act("You feel happy.", A_ALWAYS, u, 0, 0, TO_CHAR);
    break;

   case 6:
    act("You feel sad.", A_ALWAYS, u, 0, 0, TO_CHAR);
    break;

   case 7:
    act("You feel lucky.", A_ALWAYS, u, 0, 0, TO_CHAR);
    break;

   case 8:
    tmpu = read_unit(find_file_index("udgaard", "tuborg"));
    if (tmpu)
    {
      unit_to_unit(tmpu, u);
      act("The Gods grant you nectar to quench your thirst.",
	  A_ALWAYS, u, 0, 0, TO_CHAR);
      act("Suddenly the sky opens and $2n drops into the hands of $1n.",
	  A_ALWAYS, u, tmpu, 0, TO_ROOM);
    }
    else
      act("You have an inexplainable craving for a refreshing Green Tuborg.",
	  A_ALWAYS, u, 0, 0, TO_CHAR);
    break;

   case 9:
    for (u = unit_list; u; u = u->gnext)
    {
       /* Only outlaw the ones that have signed */
       if (IS_PC(u) && !number(0,1000) &&
	   IS_SET(CHAR_FLAGS(u), PC_PK_RELAXED))
       {
	  set_reward_char(u, 0);
	  return;
       }
    }
    break;

   case 10:
    i = number(1, 1000 * CHAR_LEVEL(u));
    act("You learn that you have won $2t in the lottery.",
	A_ALWAYS, u, money_string(i, DEF_CURRENCY, TRUE), 0, TO_CHAR);
    money_to_unit(u, i, DEF_CURRENCY);
    break;

   case 11:
     if (number(0, 1))
       spell_perform(SPL_DISEASE_1, MEDIA_SPELL,
		     daemon, daemon, u, mbuf, NULL);
     else
       spell_perform(SPL_DISEASE_2, MEDIA_SPELL,
		     daemon, daemon, u, mbuf, NULL);
     break;

    /*  MANY OPTIONS! Ideas:
     *  Stumble upon random item
     *  Word of failure from bank
     *  etc. etc.
     */
  }
}


int error_rod(struct spec_arg *sarg)
{
   struct zone_type *zone;
   FILE *fl;
   char filename[256];

   if ((sarg->cmd->no != CMD_USE) || (!IS_PC(sarg->activator)) ||
       (OBJ_EQP_POS(sarg->owner) != WEAR_HOLD))
     return SFR_SHARE;

   zone = unit_zone(sarg->activator);

   strcpy(filename, UNIT_NAME(sarg->activator));

   if (!IS_ADMINISTRATOR(sarg->activator) &&
       !zone->creators.IsName(filename))
   {
      send_to_char("You are only allowed to erase errors "
		   "in your own zone.\n\r", sarg->activator);
      return SFR_BLOCK;
   }

   sprintf(filename, "%s%s.err", zondir, zone->filename);

   if (!(fl = fopen(filename, "w")))
   {
      slog(LOG_ALL, 0, "Could not clear the zone error-file");
      send_to_char("Could not clear the zone error-file.\n\r",
		   sarg->activator);
      return SFR_BLOCK;
   }
   fclose(fl);

   act("$1n uses $2n.", A_HIDEINV, sarg->activator, sarg->owner, 0, TO_ROOM);
   send_to_char("Error file was erased.\n\r", sarg->activator);
   slog(LOG_ALL, UNIT_MINV(sarg->activator), "%s cleared %s",
	UNIT_NAME(sarg->activator), filename);
   return SFR_BLOCK;
}


int info_rod(struct spec_arg *sarg)
{
   struct zone_type *zone;
   FILE *fl;
   char filename[256];

   if (!is_command(sarg->cmd, "wave") || !IS_PC(sarg->activator) ||
       OBJ_EQP_POS(sarg->owner) != WEAR_HOLD)
     return SFR_SHARE;

   zone = unit_zone(sarg->activator);

   strcpy(filename, UNIT_NAME(sarg->activator));

   if (!IS_ADMINISTRATOR(sarg->activator) &&
       !zone->creators.IsName(filename))
   {
      send_to_char("You are only allowed to erase user-information"
		   " in your own zone.", sarg->activator);
      return SFR_BLOCK;
   }

   sprintf(filename, "%s%s.inf", zondir, zone->filename);

   if (!(fl = fopen(filename, "w")))
   {
      slog(LOG_ALL, 0, "Could not clear the zone user info-file");
      send_to_char("Could not clear the zone user info-file.\n\r",
		   sarg->activator);
      return SFR_BLOCK;
   }
   fclose(fl);

   act("$1n uses $2n.", A_HIDEINV, sarg->activator, sarg->owner, 0, TO_ROOM);
   send_to_char("Zone user information file was erased.\n\r", sarg->activator);
   slog(LOG_ALL, UNIT_MINV(sarg->activator), "%s cleared %s",
	UNIT_NAME(sarg->activator), filename);
   return SFR_BLOCK;
}


int recep_daemon(struct spec_arg *sarg)
{
#ifdef SUSPEKT
  static int idx = -1;
  time_t t1;

  if (sarg->cmd->no == CMD_AUTO_TICK ||
      (is_command(sarg->cmd, "tickle") &&
       daemon == find_unit(activator, &arg, 0, FIND_UNIT_SURRO)))
  {
    if (idx == -1)
      idx = number(0, (int) top_of_player_idx);

    act("$1n checks $3t's inventory ($2d)",
	A_ALWAYS, daemon, &idx, player_idx_info[idx].name, TO_ROOM);

    t1 = time(0) - player_idx_info[idx].keep_period;

    if (t1 >= 0)
    {
      act("$1n says, 'Overdue by $2d seconds.'",
	  A_ALWAYS, daemon, (int *) &t1, 0, TO_ROOM);
      act("$1n says, 'Snip Snap Snude, now the inventory is ude.'",
	  A_ALWAYS, daemon, 0, 0, TO_ROOM);
      /* Unless player is in game, load him and erase his inventory */
    }

    if (++idx > top_of_player_idx)
      idx = 0;
  }
#endif

  return SFR_SHARE;
}


int chaos_daemon(struct spec_arg *sarg)
{
   char *arg = (char *) sarg->arg;
   struct unit_data *u;

   if (sarg->cmd->no == CMD_AUTO_TICK ||
       (is_command(sarg->cmd, "tickle") && IS_ULTIMATE(sarg->activator) &&
	sarg->owner == find_unit(sarg->activator, &arg, 0, FIND_UNIT_SURRO)))
   {
      u = random_unit(sarg->owner, FIND_UNIT_WORLD, UNIT_ST_PC);
      if (u && IS_MORTAL(u))
      {
	 act("$1n grins evilly at $3n.", A_ALWAYS, sarg->owner, 0, u, TO_ROOM);
	 random_event_player(u, sarg->owner);
      }
      else
	random_event_player(NULL, sarg->owner);
   }

   return SFR_SHARE;
}


int death_room(struct spec_arg *sarg)
{
   int dam, i;

   if (sarg->cmd->no != CMD_AUTO_TICK)
     return SFR_SHARE;

   if (sarg->fptr->data)
     dam = atoi((char *) sarg->fptr->data);
   else
     dam = 10000;

   scan4_unit_room(sarg->owner, UNIT_ST_PC | UNIT_ST_NPC);

   for (i = 0; i < unit_vector.top; i++)
   {
      /* Damage below can theoretically cause death of other persons */
      if (is_destructed(DR_UNIT, unit_vector.units[i]))
	continue;

      damage(unit_vector.units[i], unit_vector.units[i], NULL, dam,
	     MSG_TYPE_OTHER, MSG_OTHER_BLEEDING, COM_MSG_EBODY);
   }

   return SFR_SHARE;
}


/* Log stuff below */

extern struct log_buffer log_buf[];

int log_object(struct spec_arg *sarg)
{
   ubit8 *ip;
   enum log_level lev = LOG_OFF;
   char c;
   struct unit_data *ch = UNIT_IN(sarg->owner);

   if (sarg->fptr->data == NULL)
   {
      CREATE(ip, ubit8, 1);
      *ip = 0;

      OBJ_VALUE(sarg->owner, 0) = 'b';
      sarg->fptr->data = ip;
   }
   else
     ip = (ubit8 *) sarg->fptr->data;

   c = OBJ_VALUE(sarg->owner, 0);

   switch (sarg->cmd->no)
   {
     case CMD_AUTO_EXTRACT:
      free(ip);
      sarg->fptr->data = 0;
      return SFR_SHARE;

     case CMD_AUTO_TICK:
      switch (c)
      {
	case 'o':
	 lev = LOG_OFF; break;
	case 'b':
	 lev = LOG_BRIEF; break;
	case 'e':
	 lev = LOG_EXTENSIVE; break;
	case 'a':
	 lev = LOG_ALL; break;
      }

      if (LOG_OFF < lev && IS_PC(ch) && PC_IMMORTAL(ch))
      {
	 while (!str_is_empty(log_buf[*ip].str))
	 {
	    if (log_buf[*ip].level <= lev 
		&& log_buf[*ip].wizinv_level <= CHAR_LEVEL(ch))
	      act("<LOG: $2t>", A_ALWAYS, ch, log_buf[*ip].str, 0, TO_CHAR);
	    *ip = ((*ip + 1) % MAXLOG);
	 }
	 return SFR_BLOCK;
      }
      return SFR_SHARE;

     default:
      if (sarg->cmd->cmd_str &&
	  sarg->activator == UNIT_IN(sarg->owner) &&
	  !strcmp("log",sarg->cmd->cmd_str))
      {
	 sarg->arg = skip_spaces(sarg->arg);
	 if (is_abbrev(sarg->arg, "all"))
	   c = 'a';
	 else if (is_abbrev(sarg->arg, "extensive"))
	   c = 'e';
	 else if (is_abbrev(sarg->arg, "brief"))
	   c = 'b';
	 else if (is_abbrev(sarg->arg, "off"))
	 {
	    act("Ok, log is now off.", A_ALWAYS, ch, 0, 0, TO_CHAR);
	    OBJ_VALUE(sarg->owner, 0) = 'o';
	    return SFR_BLOCK;
	 }
	 else if (is_abbrev(sarg->arg, "help"))
	 {
	    act("Possible settings are:\n\r off, brief, extensive, all.",
		A_ALWAYS, ch, 0, 0, TO_CHAR);
	    return SFR_BLOCK;
	 }
	 else
	 {
	    act("Current log level is `$2t'.", A_ALWAYS, ch,
		c == 'o' ? "off" : c == 'b' ? "brief" : c == 'a' ?
		"all" : "extensive", 0, TO_CHAR);
	    return SFR_BLOCK;
	 }
	
	 act("You will now see the $2t log.", A_ALWAYS, ch,
	     c == 'b' ?
	     "brief" : c == 'a' ? "entire" : "extensive", 0, TO_CHAR);
	 OBJ_VALUE(sarg->owner, 0) = c;
	 return SFR_BLOCK;
      }

      return SFR_SHARE;
   }
}

/* Return TRUE if ok, FALSE if not */
int system_check(struct unit_data *pc, char *buf)
{
   /* Check for `` and ; in system-string */
   if (strchr(buf, '`') || strchr(buf, ';'))
   {
      send_to_char("You can not use the ' and ; characters\n\r", pc);
      slog(LOG_ALL, 0, "%s may have tried to break security with %s",
	   UNIT_NAME(pc), buf);
      return FALSE;
   }

   return TRUE;
}

void execute_append(struct unit_data *pc, char *str)
{
   FILE *f;

   f = fopen(str_cc(libdir, EXECUTE_FILE), "ab+");

   if (f == NULL)
   {
      slog(LOG_ALL, 0, "Error appending to execute file.");
      return;
   }

   fprintf(f, "%s\n", str);

   slog(LOG_ALL, UNIT_MINV(pc), "EXECUTE(%s): %s", UNIT_NAME(pc), str);

   fclose(f);
}


int admin_obj(struct spec_arg *sarg)
{
   char buf[512];
   int zonelist;
   struct zone_type *zone;
   struct extra_descr_data *exdp;

   if (sarg->cmd->no != CMD_AUTO_UNKNOWN)
     return SFR_SHARE;

   if (!IS_PC(sarg->activator))
     return SFR_SHARE;

   if (str_ccmp(sarg->cmd->cmd_str, "email") == 0)
   {
      zonelist = FALSE;
   }
   else if (str_ccmp(sarg->cmd->cmd_str, "zonelist") == 0)
   {
      if (!IS_ADMINISTRATOR(sarg->activator))
      {
	 send_to_char("Only administrators can use this function.\n\r",
		      sarg->activator);
	 return SFR_BLOCK;
      }
      zonelist = TRUE;
   }
   else
     return SFR_SHARE;

   if ((exdp = PC_INFO(sarg->activator)->find_raw("$email")) == NULL)
   {
      send_to_char("You do not have an email address registered.\n\r",
		   sarg->activator);
      return SFR_BLOCK;
   }

   if (str_is_empty(exdp->descr.String()))
   {
      send_to_char("Your email is incorrectly registered.\n\r",
		   sarg->activator);
      return SFR_BLOCK;
   }

   if (zonelist)
     sprintf(buf, "mail zone zonelist %s", exdp->descr.String());
   else if ((zone = unit_zone(sarg->activator)) == NULL)
   {
      send_to_char("You are inside no zone?", sarg->activator);
      return SFR_BLOCK;
   }
   else
   {
      if ((!zone->creators.IsName(UNIT_NAME(sarg->activator))) &&
	  (!IS_OVERSEER(sarg->activator)))
      {
	 send_to_char("Only overseers can use this function.\n\r",
		      sarg->activator);
	 return SFR_BLOCK;
      }
      sprintf(buf, "mail zone %s %s", zone->filename, exdp->descr.String());
   }

   if (!system_check(sarg->activator, buf))
     return SFR_BLOCK;

   execute_append(sarg->activator, buf);

   strcat(buf, "\n\r");
   send_to_char(buf, sarg->activator);

   return SFR_BLOCK;
}
