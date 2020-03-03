/* *********************************************************************** *
 * File   : act_wizard.c                              Part of Valhalla MUD *
 * Version: 1.25                                                           *
 * Author : All.                                                           *
 *                                                                         *
 * Purpose: Implementation of wizard commands.                             *
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

/* 19/07/92 seifert: Recursive trans bug fixed.                            */
/* 26/07/92 seifert: Fixed do_purge to match new destruct system           */
/* 13/10/92 seifert: Fixed 'played:' time in do_stat                       */
/* 13/10/92 seifert: Fixed 'do_advance' to allow lower than 255 to advance */
/* 22/01/93  HHS added 'stat zone data' to give reset info (may purge you) */
/* Sun Jun 27 1993 HHS added stat world                                    */
/* Tue Jul 6 1993 HHS: added exchangable lib dir                           */
/* 23/08/93 jubal  : Load puts takable things in inventory                 */
/* 23/08/93 jubal  : Purge looks in inventory first                        */
/* 23/08/93 jubal  : Purge room doesn't remove non-takable items           */
/* 23/08/93 jubal  : Stat room data gives info on exits, and less lines    */
/* 29/08/93 jubal  : Expanded stat zone                                    */
/* 29/08/93 jubal  : Expanded goto to go to zones or enterable units       */
/* 25/02/94 gnort  : Fixed missing functionality/crashbug in stat zone     */
/* 11/08/94 gnort  : got rid of cras and shutdow                           */
/* 10/02/95 gnort  : Made do_users dynamic                                 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "structs.h"
#include "utils.h"
#include "skills.h"
#include "textutil.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "db_file.h"
#include "spells.h"
#include "limits.h"
#include "affect.h"
#include "magic.h"
#include "blkfile.h"
#include "common.h"
#include "utility.h"
#include "money.h"
#include "system.h"
#include "files.h"
#include "dijkstra.h"
#include "main.h"

/*   external vars  */

extern struct zone_info_type zone_info;
extern struct unit_data *unit_list;
extern struct descriptor_data *descriptor_list;
extern char libdir[];        /* from dikumud.c */
extern BLK_FILE *inven_bf;

/* external functs */

struct time_info_data age(struct unit_data *ch);
struct time_info_data real_time_passed(time_t t2, time_t t1);
struct zone_type *find_zone(const char *zonename);


static int WIZ_CMD_LEVEL = 210; /* No need to change this, it is also set
				   at runtime... */

struct descriptor_data *find_descriptor(const char *,
					struct descriptor_data *);
int player_exists(const char *pName);
int delete_player(const char *name);

void do_path(struct unit_data *ch, char *argument,
	     const struct command_info *cmd)
{
   int i;
   struct unit_data *thing;
   char buf[MAX_INPUT_LENGTH];
   char zone[MAX_INPUT_LENGTH];

   if (str_is_empty(argument))
   {
      send_to_char("Path where to? (symbolic / findunit)\n\r", ch);
      return;
   }

   if (!(thing = find_unit(ch,&argument,0,FIND_UNIT_WORLD)))
   {
      str_next_word(argument, argument);
      split_fi_ref(argument, zone, buf);

      if (!(thing = find_symbolic(zone, buf)))
      {
	 send_to_char("No such thing anywhere.\n\r", ch);
	 return;
      }
   }

   i = move_to(unit_room(ch), unit_room(thing));

   sprintf(buf, "Status: %d\n\r", i);
   send_to_char(buf, ch);
}


void do_users(struct unit_data *ch, char *argument,
	      const struct command_info *cmd)
{
   static char *buf = NULL;
   static int cur_size = 1024;

   int available_connections(void);

   struct descriptor_data *d;
   char tmp[256];
   int len, users = 0;

   if (buf == NULL)
     CREATE(buf, char, cur_size);

   strcpy(buf, "Connections:\n\r------------\n\r");
   len = strlen(buf);

   for (d = descriptor_list; d; d = d->next)
   {
      assert(d->character);
      if (CHAR_LEVEL(ch) >= UNIT_MINV(CHAR_ORIGINAL(d->character)))
      {
	 users++;
	 sprintf(tmp, "<%3d/%3d> %-16s %-10s [%c %4d %-3s %s]\n\r",
		 CHAR_LEVEL(CHAR_ORIGINAL(d->character)),
		 UNIT_MINV(CHAR_ORIGINAL(d->character)),
		 UNIT_NAME(CHAR_ORIGINAL(d->character)),
		 descriptor_is_playing(d) ? "Playing" : "Menu",
		 g_cServerConfig.FromLAN(d->host) ? 'L' : 'W',
		 d->nPort, d->nLine == 255 ? "---" : itoa(d->nLine), d->host);

	 len += strlen(tmp);
	 if (cur_size < len + 1)
	 {
	    cur_size *= 2;
	    RECREATE(buf, char, cur_size);
	 }
	 strcat(buf, tmp);
      }
   }

   sprintf(tmp, "\n\r%d players connected, %d players playing.\n\r",
	   users, no_players);
   
   len += strlen(tmp);
   if (cur_size < len + 1)
   {
      cur_size *= 2;
      RECREATE(buf, char, cur_size);
   }

   strcat(buf, tmp);

   page_string(CHAR_DESCRIPTOR(ch), buf);
   free(buf);
}


/* Reset the zone in which the char is in! */
void do_reset(struct unit_data *ch, char *arg,
	      const struct command_info *cmd)
{
   struct zone_type *zone;

   int zone_reset(struct zone_type *);

   if (!str_is_empty(arg))
   {
      send_to_char("You can only reset the zone you are in.\n\r", ch);
      return;
   }

   if (!(zone = unit_zone(ch)))
   {
      send_to_char("You are inside no zone to reset!\n\r", ch);
      return;
   }

   send_to_char("Ok.\n\r", ch);
   zone_reset(zone);
}


void do_echo(struct unit_data *ch, char *arg,
	     const struct command_info *cmd)
{
   if (str_is_empty(arg))
     send_to_char("Echo needs an argument.\n\r", ch);
   else
     act("$2t",A_ALWAYS,ch,arg,0,TO_ALL);
}


int frozen(struct spec_arg *sarg)
{
   if (sarg->activator != sarg->owner)
     return SFR_SHARE;

   switch (sarg->cmd->no)
   {
     case CMD_LOOK:
     case CMD_WHO:
      return SFR_SHARE;
      break;

     default:
      send_to_char("You're totally frozen\n\r",sarg->owner);
   }

   return SFR_BLOCK;
}

void do_freeze(struct unit_data *ch, char *arg,
	       const struct command_info *cmd)
{
   struct unit_data *unit;
   struct unit_fptr *fptr;

   if (str_is_empty(arg))
     send_to_char("Freeze who?\n\r", ch);
   else if(!(unit = find_unit(ch, &arg, 0, FIND_UNIT_SURRO | FIND_UNIT_WORLD))
	   || !IS_PC(unit))
     send_to_char("No such person around./n/r", ch);
   else if(ch == unit)
     send_to_char("Hehe.  Most amusing, Sire."
		  "  (People have actually DONE this!)\n\r",ch);
   else if (CHAR_LEVEL(ch) <= CHAR_LEVEL(unit))
     send_to_char("It IS a little cold isn't is?.\n\r", ch);
   else if ((fptr = find_fptr(unit, SFUN_FROZEN)))
   {
      act("$1n is now unfrozen.",A_SOMEONE,unit,0,ch,TO_VICT);
      send_to_char("You unfreeze.\n\r", unit);
      destroy_fptr(unit, fptr);
   }
   else
   {
      act("$1n is now frozen.",A_SOMEONE,unit,0,ch,TO_VICT);
      send_to_char("You totally freeze.\n\r", unit);
      create_fptr(unit, SFUN_FROZEN, 0, SFB_PRIORITY|SFB_CMD|SFB_AWARE, NULL);
   }
}


void do_noshout(struct unit_data *ch, char *argument,
		const struct command_info *cmd)
{
   struct unit_data *victim;

   if (!IS_PC(ch))
     return;

   /* Below here is the WIZARD removal of shout / noshout */
   victim = find_unit(ch, &argument, 0, FIND_UNIT_WORLD);
   if (!victim || !IS_PC(victim))
   {
      send_to_char("No such player around.\n\r", ch);
      return;
   }

   if (CHAR_LEVEL(victim) >= CHAR_LEVEL(ch))
   {
      act("$3e might object to that... better not.",
	  A_SOMEONE,ch,0,victim,TO_CHAR);
      return;
   }

   if (IS_SET(PC_FLAGS(victim), PC_NOSHOUTING))
   {
      send_to_char("Your sore throath clears up.\n\r", victim);
      send_to_char("NOSHOUTING removed.\n\r", ch);
   }
   else
   {
      send_to_char("You suddenly get a sore throath!\n\r", victim);
      send_to_char("NOSHOUTING set.\n\r", ch);
   }
   TOGGLE_BIT(PC_FLAGS(victim), PC_NOSHOUTING);
}


void do_notell(struct unit_data *ch, char *argument,
	       const struct command_info *cmd)
{
   struct unit_data *victim;

   if (!IS_PC(ch))
     return;

   /* Below here is the WIZARD removal of shout / noshout */

   victim = find_unit(ch, &argument, 0, FIND_UNIT_WORLD);
   if (!victim || !IS_PC(victim))
   {
      send_to_char("No such player around.\n\r", ch);
      return;
   }


   if (CHAR_LEVEL(victim) >= CHAR_LEVEL(ch))
   {
      act("$3e might object to that... better not.", A_SOMEONE,
	  ch, 0, victim, TO_CHAR);
      return;
   }

   if (IS_SET(PC_FLAGS(victim), PC_NOTELLING))
   {
      send_to_char("You regain your telepatic ability.\n\r", victim);
      send_to_char("NOTELLING removed.\n\r", ch);
   }
   else
   {
      send_to_char("Your telepatic ability fades away!\n\r", victim);
      send_to_char("NOTELLING set.\n\r", ch);
   }
   TOGGLE_BIT(PC_FLAGS(victim), PC_NOTELLING);
}


void do_wizinv(struct unit_data *ch, char *arg,
	       const struct command_info *cmd)
{
  struct unit_data *unit;
  int level = GOD_LEVEL - 1;

  arg = skip_spaces(arg);

  if (str_is_empty(arg) || isdigit(*arg))
    unit = ch;
  else
  {
    unit = find_unit(ch, &arg, 0, FIND_UNIT_GLOBAL);

    if (!unit)
    {
       send_to_char("No such thing.\n\r", ch);
       return;
    }
  }

  arg = skip_spaces(arg);

  if (isdigit(*arg))
    level = atoi(arg);
  else if (UNIT_MINV(unit) != 0)
    level = 0;

  if (level > CHAR_LEVEL(ch))
  {
     send_to_char("Level cannot be beyond own level\r\n", ch);
     return;
  }

  if (level > 0)
  {
     act("$3n is now WIZI below level $2d.",
	 A_ALWAYS, ch, &level, unit, TO_CHAR);

     if (unit != ch)
       act("You are now WIZI below level $2d.",
	   A_ALWAYS, unit, &level, 0, TO_CHAR);
  }
  else
  {
     act("$3n is no longer WIZI.",
	 A_ALWAYS, ch, 0, unit, TO_CHAR);

     if (unit != ch)
       act("You are no longer WIZI.",
	   A_ALWAYS, unit, 0, 0, TO_CHAR);
  }

  UNIT_MINV(unit) = level;

  if (unit == ch)
    slog(LOG_BRIEF, level, "%s wizinv%s", UNIT_NAME(ch), 
	 0 < level ? "":" off");
}


void base_trans(struct unit_data *ch, struct unit_data *victim)
{
   assert(ch && victim);

   if (unit_recursive(victim, UNIT_IN(ch)))
   {
      send_to_char("Impossible: Recursive trans.\n\r", ch);
      return;
   }

   if (!may_teleport(victim, UNIT_IN(ch)) && CHAR_LEVEL(ch) < 240)
   {
      act("You are not allowed to transfer $3n.",
	  A_SOMEONE,ch,0,victim,TO_CHAR);
      return;
   }

   act("$1n disappears in a mushroom cloud.",A_HIDEINV,victim,0,0,TO_REST);
   unit_from_unit(victim);
   unit_to_unit(victim, UNIT_IN(ch));
   act("$1n arrives from a puff of smoke.",A_HIDEINV,victim,0,0,TO_REST);
   act("$1n has transferred you!",A_SOMEONE,ch,0,victim,TO_VICT);
   char mbuf[MAX_INPUT_LENGTH] = {0};
   do_look(victim, mbuf, &cmd_auto_unknown);
   for (victim = UNIT_CONTAINS(victim); victim; victim = victim->next)
     if (IS_CHAR(victim))
       do_look(victim, mbuf, &cmd_auto_unknown);
}


void do_trans(struct unit_data *ch, char *arg,
	      const struct command_info *cmd)
{
   struct descriptor_data *i;
   struct unit_data *victim;

   if (!IS_PC(ch))
     return;

   victim = find_unit(ch, &arg, 0, FIND_UNIT_WORLD);

   if (!victim || !IS_CHAR(victim))
   {
      if (str_nccmp("all",arg,3))
      {
	 send_to_char("No such person.\n\r",ch);
	 return;
      }

      for (i = descriptor_list; i; i = i->next)
	if ((i->character != ch) && descriptor_is_playing(i))
	  base_trans(ch,i->character);

      send_to_char("Ok.\n\r",ch);
      return;
   }

   /* We found a victim that is a character: */
   if (victim == ch)
   {
      send_to_char("HA! You thought I hadn't thought of it, eh?\n\r", ch);
      return;
   }

   base_trans(ch,victim);
   send_to_char("Ok.\n\r",ch);
}


void do_at(struct unit_data *ch, char *argument,
	   const struct command_info *cmd)
{
   char buf[MAX_INPUT_LENGTH];
   struct unit_data *target, *original_loc;
   struct file_index_type *fi;

   if (!IS_PC(ch))
     return;

   if (str_is_empty(argument))
   {
      send_to_char("You must supply a unit name or zone reference.\n\r", ch);
      return;
   }

   if ((fi = pc_str_to_file_index(ch, argument)) && (fi->room_ptr))
   {
      target = fi->room_ptr;
      argument = one_argument(argument, buf);
   }
   else
   {
      if ((target = find_unit(ch, &argument, 0, FIND_UNIT_WORLD)))
	if (UNIT_IN(target))
	  target = UNIT_IN(target);
   }

   if (!target)
   {
      send_to_char("No such place around.\n\r", ch);
      return;
   }

   if (!IS_ROOM(target))
   {
      send_to_char("Sorry, you may only 'at' at a room!\n\r", ch);
      return;
   }

   original_loc = UNIT_IN(ch);
   unit_from_unit(ch);
   unit_to_unit(ch, target);
   command_interpreter(ch, argument);

   /* check if the guy's still there */
   if (!is_destructed(DR_UNIT, ch) && !is_destructed(DR_UNIT, original_loc) &&
       !unit_recursive(ch, original_loc))
   {
      unit_from_unit(ch);
      unit_to_unit(ch, original_loc);
   }
}



void do_goto(struct unit_data *ch, char *argument,
	     const struct command_info *cmd)
{
   struct unit_data *target, *pers;
   struct file_index_type *fi;
   struct zone_type *zone;
   int i;

   if (!IS_PC(ch))
     return;

   if (str_is_empty(argument))
   {
      send_to_char("You must supply a unit name or zone reference.\n\r", ch);
      return;
   }

   if ((fi = pc_str_to_file_index(ch, argument)) && (fi->room_ptr))
     target = fi->room_ptr;
   else
    if ((target = find_unit(ch, &argument, 0, FIND_UNIT_SURRO |
			    FIND_UNIT_ZONE|FIND_UNIT_WORLD)))
    {
       /* Find first container which can be entered */
       while ((!IS_ROOM(target)) && (UNIT_IN(target)) &&
	      (!IS_SET(UNIT_MANIPULATE(target),MANIPULATE_ENTER)))
	 target = UNIT_IN(target);
    }
    else
    { /* Try to goto a zone */
       zone = find_zone(argument);
       if (zone)
       {
	  /* Find the first room in the zone */
	  for (fi = zone->fi; (fi) && (fi->type != UNIT_ST_ROOM) ;
	       fi = fi->next)  ;
	  if (fi) target = fi->room_ptr;
       }
    }

   if (!target)
   {
      send_to_char("No such place around.\n\r", ch);
      return;
   }

   if (IS_SET(UNIT_FLAGS(target), UNIT_FL_PRIVATE))
   {
      for (i = 0, pers = UNIT_CONTAINS(target); pers; pers = pers->next)
	if (IS_PC(pers))
	  i++;

      if (i > 1)
      {
	 send_to_char
	   ("There's a private conversation going on in that room.\n\r", ch);
	 return;
      }
   }

   if (!may_teleport(ch, target))
      send_to_char("WARNING - this is a no teleport environment.\n\r", ch);

   if (unit_recursive(ch, target))
   {
      send_to_char("Recursive goto. Would destroy the universe.\n\r", ch);
      return;
   }

   act("$1n disappears into thin air.", A_HIDEINV, ch, 0, 0, TO_ROOM);
   unit_from_unit(ch);
   unit_to_unit(ch, target);
   act("$1n appears from thin air.", A_HIDEINV, ch, 0,0,TO_ROOM);
   char mbuf[MAX_INPUT_LENGTH] = {0};
   do_look(ch, mbuf, cmd);
}


void do_crash(struct unit_data *ch, char *argument,
	      const struct command_info *cmd)
{
  if (cmd_is_abbrev(ch, cmd))
  {
    send_to_char("If you want to crash the game - say so!\n\r", ch);
    return;
  }

  if(strcmp(argument, "the entire game..."))
    send_to_char("You must type 'crash the entire game...'\n\r",ch);
  else
    assert(FALSE); /* Bye bye */
}


void do_execute(struct unit_data *ch,char *argument,
		const struct command_info *cmd)
{
   int system_check(struct unit_data *pc, char *buf);
   void execute_append(struct unit_data *pc, char *str);

   argument = skip_spaces(argument);

   if (!system_check(ch, argument))
     return;

   execute_append(ch, argument);
   act("Executing $2t.\n\r", A_ALWAYS, ch, argument, 0, TO_CHAR);
}


void do_shutdown(struct unit_data *ch,char *argument,
		 const struct command_info *cmd)
{
   char buf[100];
   extern int mud_shutdown, mud_reboot;

   if (!IS_PC(ch))
     return;

   if (cmd_is_abbrev(ch, cmd))
   {
      send_to_char("If you want to shut something down - say so!\n\r", ch);
      return;
   }

   sprintf(buf, "Shutdown by %s.\n\r", UNIT_NAME(ch));
   send_to_all(buf);
   mud_shutdown = 1;
}


void do_reboot(struct unit_data *ch,char *argument,
	       const struct command_info *cmd)
{
   char buf[100], arg[MAX_INPUT_LENGTH];
   extern int mud_shutdown, mud_reboot;

   if (!IS_PC(ch))
     return;

   if (cmd_is_abbrev(ch, cmd))
   {
      send_to_char("If you want to reboot - say so!\n\r", ch);
      return;
   }

   struct descriptor_data *pDes;
	 
   for (pDes = descriptor_list; pDes; pDes = pDes->next)
   {
      if ((CHAR_LEVEL(CHAR_ORIGINAL(pDes->character)) > CHAR_LEVEL(ch)))
      {
	 send_to_char("Sorry, a God of higher level than yourself "
		      "is connected.\n\r", ch);
	 return;
      }
   }
   
   sprintf(buf, "Reboot by %s.\n\r", UNIT_NAME(ch));
   send_to_all(buf);
   mud_shutdown = mud_reboot = 1;
}


void do_snoop(struct unit_data *ch, char *argument,
	      const struct command_info *cmd)
{
   struct unit_data *victim;

   void unsnoop(struct unit_data *ch, int mode);
   void snoop(struct unit_data *ch, struct unit_data *victim);

   if (!CHAR_DESCRIPTOR(ch))
     return;

   if (str_is_empty(argument))
     victim = ch;
   else
     victim = find_unit(ch, &argument, 0, FIND_UNIT_WORLD);

   if (!victim)
   {
      send_to_char("No such player around.\n\r",ch);
      return;
   }

   if (!IS_CHAR(victim))
   {
      send_to_char("That's not a player / monster!\n\r",ch);
      return;
   }

   if (!CHAR_DESCRIPTOR(victim))
   {
      act("$3n has no descriptor-link.",A_SOMEONE,ch,0,victim,TO_CHAR);
      return;
   }

   if (victim == ch)
   {
      if (CHAR_IS_SNOOPING(ch))
	unsnoop(ch, 0);  /* Unsnoop just ch himself */
      else
	send_to_char("You are already snooping yourself.\n\r", ch);
      return;
   }

   if (CHAR_IS_SNOOPED(victim))
   {
      send_to_char("Busy already.\n\r",ch);
      return;
   }

   if (CHAR_LEVEL(CHAR_ORIGINAL(victim)) >= CHAR_LEVEL(CHAR_ORIGINAL(ch)))
   {
      send_to_char("You're not allowed to snoop someone your own status.\n\r",
		   ch);
      return;
   }

   send_to_char("Ok.\n\r",ch);

   if (CHAR_IS_SNOOPING(ch))
     unsnoop(ch, 0);   /* Unsnoop just ch himself */

   snoop(ch, victim);
}


/* return -> switch <no_arg> */

void do_switch(struct unit_data *ch, char *argument,
	       const struct command_info *cmd)
{
   struct unit_data *victim;

   void switchbody(struct unit_data *ch, struct unit_data *victim);
   void unswitchbody(struct unit_data *npc);

   if (!CHAR_DESCRIPTOR(ch))
     return;

   if(str_is_empty(argument))
   {
      if(CHAR_IS_SWITCHED(ch))
	unswitchbody(ch);
      else
	send_to_char("You are already home in your good old body.\n\r", ch);
      return;
   }

   victim = find_unit(ch, &argument, 0, FIND_UNIT_WORLD | FIND_UNIT_SURRO);

   if (!victim || !IS_NPC(victim))
   {
      send_to_char("No such monster around.\n\r", ch);
      return;
   }

   if (ch == victim)
   {
      send_to_char("He he he... We are jolly funny today, eh?\n\r", ch);
      return;
   }

   if (CHAR_DESCRIPTOR(victim))
     act("$3n's body is already in use!",A_ALWAYS,ch,0,victim,TO_CHAR);
   else
   {
      send_to_char("Ok.\n\r",ch);
      switchbody(ch, victim);
   }
}


void base_force(struct unit_data *ch, struct unit_data *victim, char *arg)
{
   if (CHAR_LEVEL(ch) < CHAR_LEVEL(victim) && CHAR_CAN_SEE(ch,victim))
     send_to_char("Oh no you don't!!\n\r", ch);
   else
   {
      act("$1n has forced you to '$2t'",A_SOMEONE,ch,arg,victim,TO_VICT);
      command_interpreter(victim, arg);
   }
}


void do_force(struct unit_data *ch, char *argument,
	      const struct command_info *cmd)
{
  struct descriptor_data *i;
  struct unit_data *victim;

  if (!IS_PC(ch))
    return;

  victim = find_unit(ch, &argument, 0, FIND_UNIT_SURRO | FIND_UNIT_ZONE |
		     FIND_UNIT_WORLD);

  if (!victim || !IS_CHAR(victim))
  {
    if (str_nccmp("all", argument, 3))
    {
      send_to_char("No such character around.\n\r", ch);
      return;
    }
    argument = skip_spaces(argument + 3);
    
    if (!*argument)
    {
      send_to_char("Who do you wish to force to do what?\n\r", ch);
      return;
    }

    for (i = descriptor_list; i; i = i->next)
      if (i->character != ch && descriptor_is_playing(i))
	base_force(ch,i->character,argument);
    
    send_to_char("Ok.\n\r", ch);
    return;
  }

  argument = skip_spaces(argument);

  /* We now have a victim : */
  if (!*argument)
    send_to_char("Who do you wish to force to do what?\n\r", ch);
  else
  {
    base_force(ch,victim,argument);
    send_to_char("Ok.\n\r", ch);
  }
}


void do_finger(struct unit_data *ch, char *arg,
	       const struct command_info *cmd)
{
   char buf[MAX_INPUT_LENGTH];

   void reset_char(struct unit_data *ch);
   int player_exists(char *pName);
   void enter_game(struct unit_data *ch);

   if (str_is_empty(arg))
   {
      send_to_char("Finger who?\n\r",ch);
      return;
   }


   arg = one_argument(arg, buf);

   if (find_descriptor(buf, NULL))
   {
      send_to_char("A player by that name is connected - do a wstat.\n\r", ch);
      return;
   }

   send_to_char("This command will soon give finger info about other "
		"players.\n\r", ch);
}


void do_load(struct unit_data *ch, char *arg,
	     const struct command_info *cmd)
{
   char buf[MAX_INPUT_LENGTH];
   struct file_index_type *fi;
   struct unit_data *u, *tmp;

   void reset_char(struct unit_data *ch);
   void enter_game(struct unit_data *ch);

   if(str_is_empty(arg))
   {
      send_to_char("Load? Load what??\n\r",ch);
      return;
   }

   arg = one_argument(arg, buf);

   if (find_descriptor(buf, NULL))
   {
      send_to_char("A player by that name is connected.\n\r", ch);
      return;
   }

   if ((fi = pc_str_to_file_index(ch, buf)) == NULL)
   {
      for (tmp = unit_list; tmp; tmp = tmp->gnext)
	if (IS_PC(tmp) && !str_ccmp(UNIT_NAME(tmp), buf))
	{
	   send_to_char("A player by that name is linkdead in the game.\n\r",
			ch);
	   return;
	}

      if (player_exists(buf))
      {
	 if ((u = load_player(buf)) == NULL)
	 {
	    send_to_char("Load error\n\r", ch);
	    return;
	 }

	 enter_game(u);

	 unit_from_unit(u);
	 unit_to_unit(u, UNIT_IN(ch));
	 send_to_char("You have loaded the player.\n\r", ch);

	 if (UNIT_CONTAINS(u))
	   send_to_char("Inventory loaded.\n\r", ch);

	 if (CHAR_LEVEL(u) > CHAR_LEVEL(ch))
	   slog(LOG_EXTENSIVE, UNIT_MINV(ch),
		"LEVEL: %s loaded %s when lower level.",
		UNIT_NAME(ch),UNIT_NAME(u));
	 return;
      }

      send_to_char("No such file index reference found.\n\r", ch);
      return;
   }

   if (fi->room_ptr || fi->type == UNIT_ST_ROOM)
   {
      send_to_char("Sorry, you are not allowed to load rooms.\n\r", ch);
      return;
   }

   if (CHAR_LEVEL(ch) < fi->zone->loadlevel)
   {
      if (!fi->zone->creators.IsName(UNIT_NAME(ch)))
      {
	 int i = fi->zone->loadlevel;

	 act("Level $2d is required to load items from this zone.",
	     A_ALWAYS, ch, &i, 0 , TO_CHAR);
	 return;
      }
   }

   u = read_unit(fi);
  
   if (IS_MONEY(u))
   {
      if (!IS_ADMINISTRATOR(ch))
      {
	 send_to_char("No you don't.\n\r", ch);
	 extract_unit(u);
	 return;
      }

      set_money(u, MONEY_AMOUNT(u));
   }


   if (IS_OBJ(u) && IS_SET(UNIT_MANIPULATE(u), MANIPULATE_TAKE))
   {
      unit_to_unit(u,ch);
      act("You secretly load $2n.", A_SOMEONE, ch, u, 0, TO_CHAR);
   }
   else
   {
      unit_to_unit(u, UNIT_IN(ch));
      act("$1n opens an interdimensional gate and fetches $3n.",
	  A_SOMEONE, ch, 0, u, TO_ROOM);
      act("$1n says, 'Hello World!'", A_SOMEONE, u, 0, 0, TO_ROOM);
   }
}


void do_delete(struct unit_data *ch, char *arg,
	       const struct command_info *cmd)
{
   char buf[MAX_INPUT_LENGTH];
   struct unit_data *tmp;

   if (cmd_is_abbrev(ch, cmd))
   {
     send_to_char("If you want to delete someone - say so!\n\r", ch);
     return;
   }

   if (str_is_empty(arg))
   {
      send_to_char("Delete what player???\n\r", ch);
      return;
   }

   arg = one_argument(arg, buf);

   if (find_descriptor(buf, NULL))
   {
      send_to_char("A player by that name is connected. Purge first.\n\r", ch);
      return;
   }

   for (tmp = unit_list; tmp; tmp = tmp->gnext)
     if (IS_PC(tmp) && !str_ccmp(UNIT_NAME(tmp), buf))
     {
	send_to_char("A player by that name is linkdead in the game.\n\r", ch);
	return;
     }

   if (delete_player(buf))
     send_to_char("You have deleted the player!\n\r", ch);
   else
     send_to_char("No such player found in index file.\n\r", ch);
}


/* clean a room of all mobiles and objects */
void do_purge(struct unit_data *ch, char *argument,
	      const struct command_info *cmd)
{
   char buf[MAX_INPUT_LENGTH];
   struct unit_data *thing, *next_thing;
   struct descriptor_data *d;

   void close_socket(struct descriptor_data *d);

   if (!IS_PC(ch))
     return;

   one_argument(argument, buf);

   if (str_is_empty(argument))
   {
      act("$1n gestures... You are surrounded by scorching flames!",
	  A_SOMEONE, ch, 0, 0, TO_ROOM);
      act("You are surrounded by scorching flames!",
	  A_SOMEONE, ch, 0, 0, TO_CHAR);

      for (thing = UNIT_CONTAINS(UNIT_IN(ch)); thing; thing = next_thing)
      {
	 next_thing = thing->next;
	 if (!IS_PC(thing) && !IS_ROOM(thing) &&
	     !find_fptr(thing, SFUN_PERSIST_INTERNAL))
	   if (!IS_OBJ(thing) ||
	       IS_SET(UNIT_MANIPULATE(thing),MANIPULATE_TAKE))
	     extract_unit(thing);
      }
      return;
   }

   if (!IS_ADMINISTRATOR(ch))
   {
      send_to_char("You can only purge to clean rooms...", ch);
      return;
   }

   if (str_ccmp_next_word(buf, "_doomsday_") && IS_ULTIMATE(ch))
   {
      send_to_char("You do a MAJOR cleanup!\n\r", ch);

      for (thing = unit_list; thing; thing = next_thing)
      {
	 next_thing = thing->gnext;
	 if (!IS_PC(thing) && !IS_ROOM(thing))
	   if (IS_ROOM(UNIT_IN(thing)))
	     extract_unit(thing);
      }

      return;
   }

   if ((d = find_descriptor(buf, NULL)))
   {
      if (d->character && (CHAR_LEVEL(ch) < CHAR_LEVEL(d->character)))
      {
	 send_to_char("Fuuuuuuuuu!\n\r", ch);
	 return;
      }

      send_to_char("Purged completely from the game.\n\r", ch);

      if (d->fptr == descriptor_interpreter)
	extract_unit(d->character);
      descriptor_close(d);
   }
   else if((thing = find_unit(ch,&argument,0,FIND_UNIT_INVEN|FIND_UNIT_SURRO)))
   {
      if (IS_PC(thing) && (CHAR_LEVEL(ch) <= CHAR_LEVEL(thing)))
	send_to_char("Fuuuuuuuuu!\n\r", ch);
      else if (!IS_ROOM(thing))
      {
	 act("$1n disintegrates $3n.", A_SOMEONE, ch, 0, thing, TO_NOTVICT);
	 act("You disintegrate $3n.", A_SOMEONE, ch, 0, thing, TO_CHAR);
	 extract_unit(thing);
      }
      else
	act("You may not purge rooms.", A_SOMEONE, ch, 0, 0, TO_CHAR);
   }
   else
     send_to_char("No such thing found...\n\r", ch);
}

void do_advance(struct unit_data *ch, char *argument,
		const struct command_info *cmd)
{
   struct unit_data *victim;
   char name[100], level[100];
   int newlevel;

   void gain_exp_regardless(struct unit_data *ch, int gain);

   if (!IS_PC(ch))
     return;

   argument_interpreter(argument, name, level);

   if (*name)
   {
      if (!(victim = find_unit(ch, &argument, 0, FIND_UNIT_SURRO |
			       FIND_UNIT_WORLD)) || !IS_PC(victim))
      {
	 send_to_char("That player is not here.\n\r", ch);
	 return;
      }
   }
   else
   {
      send_to_char("Advance who?\n\r", ch);
      return;
   }

   /*   if (CHAR_LEVEL(victim) < START_LEVEL)
	{
	send_to_char("Player must be minimum start level.\n\r", ch);
	return;
	}*/

   if (!*level)
   {
      send_to_char("You must supply a level number.\n\r", ch);
      return;
   }

   if (!isdigit(*level))
   {
      send_to_char("Second argument must be a positive integer.\n\r",ch);
      return;
   }

   newlevel = atoi(level);

   if (newlevel > ULTIMATE_LEVEL || newlevel < START_LEVEL)
   {
      send_to_char("New level must be between 4 and 255.\n\r", ch);
      return;
   }

   if (newlevel < CHAR_LEVEL(victim) && newlevel >= IMMORTAL_LEVEL &&
       IS_ADMINISTRATOR(ch) && CHAR_LEVEL(victim) < CHAR_LEVEL(ch))
   {
      CHAR_EXP(victim) = 0;
      CHAR_LEVEL(victim) = newlevel;
      send_to_char("You feel pretty awesome!\n\r", ch);
      act("$3n makes some strange gestures. "
	  "A horrible feeling comes upon you, like a giant hand, darkness"
	  "comes down from above, grabbing your body, which begin to ache "
	  "with striking pain from inside. Your head seems to be filled with "
	  "deamons from another plane as your body dissolves into the "
	  "elements of time and space itself. You feel less powerful.",
	  A_ALWAYS, victim, 0, ch, TO_CHAR);
      return;
   }

   if (newlevel <= CHAR_LEVEL(victim))
   {
      send_to_char("Can't diminish that players status.\n\r", ch);
      return;
   }

   if (CHAR_LEVEL(ch) < ULTIMATE_LEVEL && newlevel >= CHAR_LEVEL(ch))
   {
      send_to_char("Thou art not godly enough.\n\r", ch);
      return;
   }

   send_to_char("You feel generous.\n\r", ch);
   act("$3n makes some strange gestures. "
       "A strange feeling comes upon you, like a giant hand, light comes "
       "down from above, grabbing your body, which begins to pulse with "
       "coloured lights from inside. Your head seems to be filled with "
       "deamons from another plane as your body dissolves into the elements "
       "of time and space itself. Suddenly a silent explosion of light snaps "
       "you back to reality. You feel slightly different.",
       A_ALWAYS, victim, 0, ch, TO_CHAR);

   if (newlevel >= IMMORTAL_LEVEL)
   {
      CHAR_LEVEL(victim) = newlevel;
      CHAR_EXP(ch) = 0;
   }
   else
     gain_exp_regardless(victim, required_xp(newlevel)-CHAR_EXP(victim));
}


void do_verify(struct unit_data *ch, char *arg,
	       const struct command_info *cmd)
{
#ifdef SUSPEKT
   struct unit_data *pc, *obj;
   int i, j;
   float asum, ssum;
   float atot, stot;
   char buf[256];

   extern struct requirement_type pc_race_base[];

   if (!IS_PC(ch))
     return;

   if (str_is_empty(arg))
     send_to_char("Verify who?\n\r",ch);

   if (!(pc = find_unit(ch, &arg, 0, FIND_UNIT_SURRO)) || !IS_PC(pc))
   {
      send_to_char("No such player in the game.\n\r", ch);
      return;
   }

   if (CHAR_LEVEL(ch) <= CHAR_LEVEL(pc))
   {
      send_to_char("No. You don't want to do that.\n\r",ch);
      return;
   }

   if (IS_IMMORTAL(pc))
   {
      send_to_char("Only on mortal PC's.\n\r",ch);
      return;
   }

   for (obj = UNIT_CONTAINS(pc); obj; obj = obj->next)
     if (IS_OBJ(obj) && OBJ_EQP_POS(obj))
       unequip_object(obj);

   affect_clear_unit(pc);

   if (UNIT_AFFECTED(pc))
   {
      send_to_char("ERROR: Could not entirely destroy affects!\n\r", ch);
      return;
   }

   asum = PC_ABILITY_POINTS(pc);
   ssum = PC_SKILL_POINTS(pc);

   for (i=0; i < ABIL_TREE_MAX; i++)
   {
      for (j=0; j < CHAR_ABILITY(pc, i); j++)
	asum += next_point_cost((int)
				pc_race_base[CHAR_RACE(pc)].abilities[i], j);
   }

   PC_SKI_SKILL(pc, SKI_FLEE)        -= 50;
   PC_SKI_SKILL(pc, SKI_CONSIDER)    -= 50;
   PC_SKI_SKILL(pc, SKI_DIAGNOSTICS) -= 50;

   for (i=0; i < SKI_TREE_MAX; i++)
   {
      for (j=0; j < PC_SKI_SKILL(pc, i); j++)
	ssum += next_point_cost(100, j);
   }

   PC_SKI_SKILL(pc, SKI_FLEE)        += 50;
   PC_SKI_SKILL(pc, SKI_CONSIDER)    += 50;
   PC_SKI_SKILL(pc, SKI_DIAGNOSTICS) += 50;

   for (i=0; i < SPL_TREE_MAX; i++)
   {
      for (j=0; j < PC_SPL_SKILL(pc, i); j++)
	ssum += next_point_cost(100, j);
   }

   for (i=0; i < WPN_TREE_MAX; i++)
   {
      for (j=0; j < PC_WPN_SKILL(pc, i); j++)
	ssum += next_point_cost(100, j);
   }

   atot = ability_point_total(START_LEVEL);
   stot = skill_point_total(START_LEVEL);

   for (i=START_LEVEL+1; i <= CHAR_LEVEL(pc); i++)
   {
      atot += (int) ability_point_gain(i);
      stot += (int) skill_point_gain(i);
   }

   sprintf(buf, "Has %.0f (excess) ability points (%.0f / %.0f).\n\r",
	   asum - atot, asum, atot);
   send_to_char(buf, ch);
   sprintf(buf, "Has %.0f (excess) skill points (%.0f / %.0f).\n\r",
	   ssum - stot, ssum, stot);
   send_to_char(buf, ch);
#endif
}




void reroll(struct unit_data *victim)
{
   struct extra_descr_data *exd, *nextexd;
   struct unit_data *obj;
   int i;

   void race_cost(struct unit_data *ch);
   void points_reset(struct unit_data *ch);
   void race_cost(struct unit_data *ch);
   void clear_training_level(struct unit_data *ch);

   if (IS_IMMORTAL(victim))
     return;

   for (obj = UNIT_CONTAINS(victim); obj; obj = obj->next)
     if (IS_OBJ(obj) && OBJ_EQP_POS(obj))
       unequip_object(obj);

   affect_clear_unit(victim);

   if (UNIT_AFFECTED(victim))
   {
      send_to_char("You were not rerolled!\n\r", victim);
      return;
   }

   race_cost(victim);
   race_cost(victim);
   points_reset(victim);

   UNIT_MAX_HIT(victim) = UNIT_HIT(victim) = hit_limit(victim);

   send_to_char("You are re-initialized. Go practice.\n\r",victim);

   clear_training_level(victim);

   CHAR_LEVEL(victim) = PC_VIRTUAL_LEVEL(victim) = 0;

   PC_SKILL_POINTS(victim)   = skill_point_total(0);
   PC_ABILITY_POINTS(victim) = ability_point_total(0);

   if (PC_GUILD(victim))
   {
      free(PC_GUILD(victim));
      PC_GUILD(victim) = NULL;
   }

   for (exd = PC_QUEST(victim); exd; exd = nextexd)
   {
      nextexd = exd->next;

      if (exd->names.Name() && exd->names.Name()[0]=='$')
      {
	 PC_QUEST(victim) = PC_QUEST(victim)->remove(exd->names.Name());
      }
   }
}

void do_reroll(struct unit_data *ch, char *arg,
	       const struct command_info *cmd)
{
   struct unit_data *victim;

   if(!IS_PC(ch))
     return;

   if (str_is_empty(arg))
     send_to_char("Reroll who?\n\r",ch);

   if (!(victim = find_unit(ch, &arg, 0, FIND_UNIT_WORLD)))
   {
      send_to_char("No such player in the game.\n\r", ch);
      return;
   }

   if (!IS_PC(victim))
   {
      send_to_char("Only on PC's.\n\r",ch);
      return;
   }

   if (CHAR_LEVEL(ch) <= CHAR_LEVEL(victim))
   {
      send_to_char("No. You don't want to do that.\n\r",ch);
      return;
   }

   reroll(victim);

   send_to_char("Rerolled.\n\r",ch);
}


void do_restore(struct unit_data *ch, char *argument,
		const struct command_info *cmd)
{
   int i;
   struct unit_data *victim;

   void update_pos( struct unit_data *victim );

   if (!IS_PC(ch))
     return;

   victim = find_unit(ch, &argument, 0, FIND_UNIT_WORLD);

   if (!victim || !IS_CHAR(victim))
   {
      send_to_char("No such character to restore.\n\r",ch);
      return;
   }

   CHAR_MANA(victim) = mana_limit(victim);
   UNIT_HIT(victim)  = UNIT_MAX_HIT(victim) = hit_limit(victim);
   CHAR_ENDURANCE(victim) = move_limit(victim);

   if (IS_PC(victim))
   {
      if (IS_GOD(victim))
      {
	 for (i = 0; i < ABIL_TREE_MAX; i++)
	   CHAR_ABILITY(ch, i) = 150;
	 for (i = 0; i < SPL_TREE_MAX; i++)
	   PC_SPL_SKILL(ch, i) = 150;
	 for (i = 0; i < WPN_TREE_MAX; i++)
	   PC_WPN_SKILL(ch, i) = 150;
	 for (i = 0; i < SKI_TREE_MAX; i++)
	   PC_SKI_SKILL(ch, i) = 150;
      }
      else
      {
	 PC_COND(victim, FULL) = 24;
	 PC_COND(victim, THIRST) = 24;
	 PC_COND(victim, DRUNK) = 0;
      }
   }

   update_pos( victim );
   send_to_char("Done.\n\r", ch);
   act("You have been fully healed by $3n!",
       A_SOMEONE, victim, 0, ch, TO_CHAR);
}


/****************************
 * The command `file' below *
 ****************************/

extern char *wizlist, *news, *credits, *motd, *goodbye;
char *read_info_file(char *name, char *oldstr);

static bool file_install(char *file, bool bNew)
{
  char buf[256];

  sprintf(buf, "%s%s", libdir, file);

  if (bNew)
  {
    if (file_exists(str_cc(buf, ".new")))
    {
      rename(buf, str_cc(buf, ".old"));
      rename(str_cc(buf, ".new"), buf);
    }
    else
      return FALSE;
  }
  else
  {
    if (file_exists(str_cc(buf, ".old")))
      rename(str_cc(buf, ".old"), buf);
    else
      return FALSE;
  }

  return TRUE;
}


void do_file(struct unit_data *ch, char *argument,
	     const struct command_info *cmd)
{
  char buf[MAX_INPUT_LENGTH];
  const char *str = "$2t installed.";

  argument = one_argument(argument, buf);

  if (strcmp(buf, "new") == 0)
  {
    one_argument(argument, buf);
      
    if (file_install(buf, TRUE))
      str = "$2t installed.  Backup in $2t.old";
    else
      str = "No new file for $2t detected.";
  }
  else if (strcmp(buf, "old") == 0)
  {
    one_argument(argument, buf);

    if (file_install(buf, FALSE))
      str = "$2t.old re-installed.";
    else
      str = "No backup for $2t present.";
  }

  if (strcmp(buf, "wizlist") == 0)
    g_cServerConfig.m_pWizlist = read_info_file(str_cc(libdir, WIZLIST_FILE), g_cServerConfig.m_pWizlist);
  else if (strcmp(buf, "news") == 0)
    g_cServerConfig.m_pNews    = read_info_file(str_cc(libdir, NEWS_FILE), g_cServerConfig.m_pNews);
  else if (strcmp(buf, "motd") == 0)
    g_cServerConfig.m_pMotd    = read_info_file(str_cc(libdir, MOTD_FILE), g_cServerConfig.m_pMotd);
  else if (strcmp(buf, "credits") == 0)
    g_cServerConfig.m_pCredits = read_info_file(str_cc(libdir, CREDITS_FILE), g_cServerConfig.m_pCredits);
  else if (strcmp(buf, "goodbye") == 0)
    g_cServerConfig.m_pGoodbye = read_info_file(str_cc(libdir, GOODBYE_FILE), g_cServerConfig.m_pGoodbye);
  else
    str = "Usage:\n\r"
      "file [new|old] <wizlist | news | credits | motd | goodbye>";

  act(str, A_ALWAYS, ch, buf, 0, TO_CHAR);
}

/* end file */


void do_message(struct unit_data *ch, char *arg,
		const struct command_info *cmd)
{
   struct unit_data *vict;

   if(str_is_empty(arg))
     send_to_char("Who? What??\n\r", ch);
   else if(!(vict = find_unit(ch, &arg, 0, FIND_UNIT_WORLD)))
     send_to_char("No-one by that name here.\n\r", ch);
   else if(vict == ch)
     send_to_char("You recieve a message from yourself.\n\r", ch);
   else if(str_is_empty(arg))
     send_to_char("What??\n\r", ch);
   else
   {
      act("$2t",A_ALWAYS,vict,skip_spaces(arg),0,TO_CHAR);
      send_to_char("Ok.\n\r", ch);
   }
}


void do_broadcast(struct unit_data *ch, char *arg,
		  const struct command_info *cmd)
{
   struct descriptor_data *d;

   if(str_is_empty(arg))
     send_to_char("Yeah, that makes a LOT of sense!\n\r", ch);
   else
     for(d = descriptor_list; d; d = d->next)
       if (descriptor_is_playing(d))
	 act("$2t",A_ALWAYS,d->character,arg,0,TO_CHAR);
}


void list_wizards(struct unit_data *ch, bool value)
{
  struct descriptor_data *d;
  bool any = FALSE;
  char buf[MAX_STRING_LENGTH];
  char *s = buf;

  sprintf(s, "Wizards %sline:\n\r", value ? "on" : "off");
  TAIL(s);

  for (d = descriptor_list; d; d = d->next)
    if (descriptor_is_playing(d)
	&& WIZ_CMD_LEVEL <= CHAR_LEVEL(CHAR_ORIGINAL(d->character))
	&& CHAR_CAN_SEE(ch, CHAR_ORIGINAL(d->character)))
    {
      bool nowiz = IS_SET(PC_FLAGS(CHAR_ORIGINAL(d->character)), PC_NOWIZ);

      if ((value && !nowiz) || (!value && nowiz))
      {
	any = TRUE;
	sprintf(s, "%s%s(%d), ",
                UNIT_MINV(d->character) > 0 ? "*" : "",
                UNIT_NAME(CHAR_ORIGINAL(d->character)),
		CHAR_LEVEL(CHAR_ORIGINAL(d->character)));
	TAIL(s);
      }
    }

  if (any)
  {
    *(s - 2) = '\0';
    act("$2t.", A_ALWAYS, ch, buf, 0, TO_CHAR);
  }
}


void do_wiz(struct unit_data *ch, char *arg,
	    const struct command_info *cmd)
{
   struct descriptor_data *d;
   char tmp[MAX_INPUT_LENGTH];
   bool emote = FALSE;
   int level;

   if (!CHAR_DESCRIPTOR(ch))
     return;

   if (cmd->no == CMD_WIZ)
     WIZ_CMD_LEVEL = cmd->minimum_level;

   if (IS_SET(UNIT_FLAGS(ch), UNIT_FL_BURIED))
   {
      send_to_char("You can not talk while buried.\n\r", ch);
      return;
   }

   level = MAX(WIZ_CMD_LEVEL, UNIT_MINV(CHAR_ORIGINAL(ch)));

   switch (*arg)
   {
     case '@':
      emote = TRUE;
      /* fall-thru! */
    
     case '#':
      one_argument(arg+1,tmp);
      if (str_is_number(tmp))
      {
	 arg = one_argument(++arg, tmp);
	 level = MAX(atoi(tmp), WIZ_CMD_LEVEL);
      }
      else if (emote)
	arg++;
      break;

     case '?':
      if (str_is_empty(arg + 1))
      {
	 list_wizards(ch, TRUE);
	 list_wizards(ch, FALSE);
	 return;
      }
      break;
      
     case '-':
      if (str_is_empty(arg + 1))
      {
	 if (IS_SET(PC_FLAGS(CHAR_ORIGINAL(ch)), PC_NOWIZ))
	   send_to_char("But you're ALREADY offline!\n\r", ch);
	 else
	 {
	    send_to_char("You won't hear the wizline from now on.\n\r", ch);
	    SET_BIT(PC_FLAGS(CHAR_ORIGINAL(ch)), PC_NOWIZ);
	 }
	 return;
      }
      break;
      
     case '+':
      if (str_is_empty(arg + 1))
      {
	 if (!IS_SET(PC_FLAGS(CHAR_ORIGINAL(ch)), PC_NOWIZ))
	   send_to_char("But you're ALREADY online!\n\r", ch);
	 else
	 {
	    send_to_char("You can now hear the wizline again.\n\r", ch);
	    REMOVE_BIT(PC_FLAGS(CHAR_ORIGINAL(ch)), PC_NOWIZ);
	 }
	 return;
      }
      break;
   }

   if (IS_SET(PC_FLAGS(CHAR_ORIGINAL(ch)), PC_NOWIZ))
   {
      send_to_char("You're offline!!\n\r", ch);
      return;
   }
 
   arg = skip_spaces(arg);
   
   if (str_is_empty(arg))
   {
      send_to_char("Gods don't like being bothered like that!\n\r", ch);
      return;
   }
   
   sprintf(tmp, COLOUR_COMM ":%s: $1n %s$2t" COLOUR_NORMAL,
	   WIZ_CMD_LEVEL < level ? itoa(level) : "", emote ? "" : ":: ");
   
   for (d = descriptor_list; d; d = d->next)
     if (descriptor_is_playing(d)
	 && level <= CHAR_LEVEL(CHAR_ORIGINAL(d->character))
	 && !IS_SET(PC_FLAGS(CHAR_ORIGINAL(d->character)), PC_NOWIZ))
       act(tmp, A_SOMEONE, CHAR_ORIGINAL(ch), arg, d->character, TO_VICT);
}


void do_title(struct unit_data *ch, char *arg,
	      const struct command_info *cmd)
{
   struct unit_data *u;
   char *oldarg = arg;

   if (!IS_PC(ch) || str_is_empty(arg))
   {
      send_to_char("That's rather silly, I think.\n\r",ch);
      return;
   }

   u = find_unit(ch, &arg, 0, FIND_UNIT_SURRO | FIND_UNIT_WORLD);

   if (u == NULL || !IS_PC(u))
   {
      arg = oldarg;
      u = ch;
   }
   else
   {
      arg = skip_spaces(arg);

      if (str_is_empty(arg))
      {
	 send_to_char("You can't assign an empty title\n\r", ch);
	 return;
      }
   }

   if (strlen(arg) > 50)
   {
      send_to_char("Title too long - Truncated.\n\r",ch);
      arg[50] = '\0';
   }
   else
     send_to_char("Ok.\n\r",ch);

   UNIT_TITLE(u).Reassign(arg);
}


/*  0: free access
 * >0: locked for below this level
 */
void do_wizlock(struct unit_data *ch, char *arg,
		const struct command_info *cmd)
{
  extern int wizlock;
  int lvl;
  char buf[128];

  arg = one_argument(arg,buf);

  if (*buf)
    lvl = atoi(buf) + 1;
  else
    lvl = GOD_LEVEL;

  if (lvl >= CHAR_LEVEL(ch))
    lvl = CHAR_LEVEL(ch);
  if (lvl == 0)
    lvl = 1;

  if (wizlock && !*buf)
  {
    send_to_char("Game is no longer wizlocked.\n\r", ch);
    wizlock = 0;
  } 
  else
  {
    sprintf(buf, "Game is now wizlocked for level %d%s.\n\r", lvl - 1,
	    lvl - 1 > 0 ? " and down" : "");
    send_to_char(buf, ch);
    wizlock = lvl;
  }
}

void do_wizhelp(struct unit_data *ch, char *arg,
		const struct command_info *cmd)
{
  char buf[MAX_STRING_LENGTH], *b;
  int no, i;
  extern struct command_info cmd_info[];

  if (!IS_PC(ch))
    return;

  send_to_char("The following privileged commands are available:\n\r\n\r", ch);
  
  *buf = '\0';
  b = buf;

  for (no = 1, i = 0; *cmd_info[i].cmd_str; i++)
    if (CHAR_LEVEL(ch) >= cmd_info[i].minimum_level &&
	cmd_info[i].minimum_level >= IMMORTAL_LEVEL)
    {
      sprintf(b, "%3d %-10s", cmd_info[i].minimum_level, cmd_info[i].cmd_str);
      if (!(no % 5))
	strcat(buf, "\n\r");
      no++;
      TAIL(b);
    }

  strcpy(b, "\n\r");

  page_string(CHAR_DESCRIPTOR(ch), buf);
}

void do_kickit(struct unit_data *ch, char *arg,
	       const struct command_info *cmd)
{
   send_to_char("No compiler in this version, I guess.\n\r", ch);
}


void do_corpses(struct unit_data *ch, char *arg,
		const struct command_info *cmd)
{ 
   extern char *in_string(struct unit_data *ch, struct unit_data *u);

   struct unit_data *c;
   bool found = FALSE;
   char *c1, *c2, buf[512];

   send_to_char("The following player corpses were found:\n\r", ch);

   for (c = unit_list; c; c = c->gnext)
     if (IS_OBJ(c) 
	 && OBJ_VALUE(c, 2) == 1 /* 1 means player corpse, if a corpse */
	 && UNIT_CONTAINS(c))    /* skip empty corpses */
     {
	c1 = str_str(UNIT_OUT_DESCR_STRING(c), " corpse of ");
	c2 = str_str(UNIT_OUT_DESCR_STRING(c), " is here.");

	if (c1 == NULL || c2 == NULL)
	  continue;

	found = TRUE;

	strncpy(buf, c1 + 1, c2 - (c1 + 1));
	buf[c2 - (c1 + 1)] = '\0';

	act("  $2t: $3t", A_ALWAYS, ch, buf, in_string(ch, c), TO_CHAR);
     }

   if (!found)
     send_to_char("  None!\n\r", ch);
}
