/* *********************************************************************** *
 * File   : spec_procs.c                              Part of Valhalla MUD *
 * Version: 1.43                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Special routines                                               *
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

/* 26/07/92 seifert: Added blockway, and door/unit access prevention       */
/* 30/07/92 seifert: Added rescue, teamwork, hide                          */
/* 30/07/92 seifert: Fixed exclude check from block procedures             */
/* 01/10/92 seifert: modified various routines                             */
/* 22/01/93 hhs: corrected drop bug in blow away                           */
/* 02/08/94 gnort: Fixed a bug in force_move; added support for 2nd string */

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
#include "movement.h"
#include "magic.h"
#include "utility.h"
#include "money.h"
#include "str_parse.h"
#include "files.h"
#include "trie.h"
#include "spec_assign.h"
#include "fight.h"

/*   external vars  */
extern struct unit_data *unit_list;
extern struct descriptor_data *descriptor_list;
extern char *dirs[];

/* extern procedures */

void modify_hit(struct unit_data *ch, int hit);
amount_t obj_trade_price(struct unit_data *u);

/* ------------------------------------------------------------------------- */
/*                        R O O M   R O U T I N E S                          */
/* ------------------------------------------------------------------------- */

/* syntax: "destination!str1[@str2]"                                       */
/* Examples: SFUN_FORCE_MOVE                                               */
/*   "river/gentle_stream!You flow down the stream to a more gentle area." */
/*   "haunted_house/second_floor!The floorboards break under your weight!@$1n crashes through the floor." */
/* Ticks. */
int force_move(struct spec_arg *sarg)
{
   char *c = NULL, *c2, *s = (char *) sarg->fptr->data;
   struct file_index_type *fi;
   struct unit_data *u, *ut, *next;

   if (sarg->cmd->no != CMD_AUTO_TICK)
     return SFR_SHARE;

   if (s == NULL || (c = strchr(s, '!')) == NULL)
   {
      szonelog(UNIT_FI_ZONE(sarg->owner), "%s@%s: %s data to force move.",
	       UNIT_FI_NAME(sarg->owner),
	       UNIT_FI_ZONENAME(sarg->owner), s ? "Wrong" : "No");
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_SHARE;
   }

   *c = 0;
   fi = pc_str_to_file_index(sarg->owner, s);
   *c = '!';

   c2 = strchr(c, '@');

   if (fi == NULL || fi->room_ptr == NULL)
   {
      szonelog(UNIT_FI_ZONE(sarg->owner), "%s@%s: Force move, no such room: %s",
	       UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner), s);
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_SHARE;
   }

   if ((u = UNIT_CONTAINS(sarg->owner)))
   {
      if (c2)
	*c2 = '\0';

      act(c + 1, A_ALWAYS, u, 0, 0, TO_ALL);

      if (c2)
	*c2 = '@';

      for (; u; u = next)
      {
	 next = u->next;
	 if (!IS_ROOM(u))
	 {
	    if (IS_CHAR(u) && CHAR_COMBAT(u))
	      continue;

	    if (UNIT_CHARS(u))
	    {
	       struct unit_data *tu;

	       for (tu = UNIT_CONTAINS(u); tu; tu = tu->next)
		 if (IS_CHAR(tu) && CHAR_COMBAT(tu))
		   break;

	       if (tu)
		 continue;
	    }

	    if (c2)
	      act(c2 + 1, A_HIDEINV, u, 0, 0, TO_ROOM);

	    if (UNIT_CONTAINS(fi->room_ptr))
	      act("$2n has arrived.",
		  A_HIDEINV, UNIT_CONTAINS(fi->room_ptr), u, 0, TO_ALL);

	    unit_from_unit(u);
	    unit_to_unit(u, fi->room_ptr);
	    char mbuf[MAX_INPUT_LENGTH] = {0};
	    if (IS_CHAR(u))
	      do_look(u, mbuf, sarg->cmd);
	    if (UNIT_IS_TRANSPARENT(u))
	      for (ut = UNIT_CONTAINS(u); ut; ut = ut->next)
		if (IS_CHAR(ut))
		  do_look(ut, mbuf, sarg->cmd);
	 }
      }
   }

   return SFR_BLOCK;
}


/* ------------------------------------------------------------------------- */
/*                     M O B I L E   R O U T I N E S                         */
/* ------------------------------------------------------------------------- */

int combat_poison_sting(struct spec_arg *sarg)
{
   struct unit_data *activator = NULL;

   if (!IS_CHAR(sarg->owner))
   {
      slog(LOG_ALL, 0, "%s@%s: Owner not char!",
	   UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_SHARE;
   }

   if (sarg->cmd->no == CMD_AUTO_COMBAT && CHAR_AWAKE(sarg->owner)
       && (activator = CHAR_FIGHTING(sarg->owner)) &&
       cast_magic_now(sarg->owner, 20))
   {
      CHAR_MANA(sarg->owner) -= 20;
      
      act("$1n bites $3n.", A_SOMEONE, sarg->owner, 0, activator, TO_NOTVICT);
      act("$1n bites you.", A_SOMEONE, sarg->owner, 0, activator, TO_VICT);
      
      char mbuf[MAX_INPUT_LENGTH] = {0};
      spell_perform(SPL_POISON,
		    MEDIA_SPELL,
		    sarg->owner, sarg->owner, sarg->activator, mbuf);
      return SFR_BLOCK;
   }

   return SFR_SHARE;
}



/* A restricted obey proper for animals... */

int obey_animal(struct spec_arg *sarg)
{
   char *arg = (char *) sarg->arg;
   struct unit_data *u;

   extern struct trie_type *intr_trie;

   if (sarg->cmd->no == CMD_AUTO_TICK)
   {
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_SHARE;
   }

   if (!IS_CHAR(sarg->owner))
   {
      slog(LOG_ALL, 0, "%s@%s: Owner not char!",
	   UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_SHARE;
   }

   if (sarg->cmd->no == CMD_TELL)
   {
      u = find_unit(sarg->activator, &arg, 0,
		    FIND_UNIT_SURRO | FIND_UNIT_INVEN);

      arg = skip_spaces(arg);

      if (str_is_empty(arg))
      {
	 act("Command $3N to do what?",
	     A_ALWAYS, sarg->activator, 0, sarg->owner, TO_CHAR);
	 return SFR_BLOCK;
      }

      if ((u == sarg->owner) && (CHAR_MASTER(sarg->owner) == sarg->activator))
      {
	 char cmd[MAX_INPUT_LENGTH];
	 struct command_info *cmd_ptr;
	 
	 act("You command $2n to '$3t'",
	     A_ALWAYS, sarg->activator, sarg->owner, arg, TO_CHAR);
	 act("$1n commands you to '$2t'",
	     A_ALWAYS, sarg->activator, arg, sarg->owner, TO_VICT);

	 str_next_word(arg, cmd);
	 if ((cmd_ptr = (struct command_info *) search_trie(cmd, intr_trie)))
	 {
	    switch (cmd_ptr->no)
	    {
	      case CMD_SACRIFICE:
	      case CMD_PRAY:
	      case CMD_INSULT:
	      case CMD_POSE:
	      case CMD_CONTRACT:
	      case CMD_EXTINGUISH:
	      case CMD_LIGHT:
	      case CMD_AID:
	      case CMD_VENTRILOQUATE:
	      case CMD_TURN:
	      case CMD_RECITE:
	      case CMD_USE:
	      case CMD_BACKSTAB:
	      case CMD_PICK:
	      case CMD_STEAL:
	      case CMD_BASH:
	      case CMD_RESCUE:
	      case CMD_SEARCH:
	      case CMD_HIDE:
	      case CMD_SNEAK:
	      case CMD_WRITE:
	      case CMD_OPEN:
	      case CMD_CLOSE:
	      case CMD_ORDER:
	      case CMD_CAST:
	      case CMD_WHISPER:
	      case CMD_GIVE:
	      case CMD_PUT:
	      case CMD_POUR:
	      case CMD_VALUE:
	      case CMD_BUY:
	      case CMD_SELL:
	      case CMD_EMOTE:
	      case CMD_DECAPITATE:
	      case CMD_WEAR:
	      case CMD_WIELD:
	      case CMD_KICK:
	      case CMD_SHOUT:
	      case CMD_SAY:
	      case CMD_ASK:
	      case CMD_HOLD:
	      case CMD_UNLOCK:
	      case CMD_LOCK:
	       return SFR_BLOCK;
	    }
	 }
	 command_interpreter(sarg->owner, arg);
	 return SFR_BLOCK;
      }
   }

   return SFR_SHARE;
}


int obey(struct spec_arg *sarg)
{
   char *arg = (char *) sarg->arg;
   struct unit_data *u;

   if (sarg->cmd->no == CMD_AUTO_TICK)
   {
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_SHARE;
   }

   if (!IS_CHAR(sarg->owner))
   {
      slog(LOG_ALL, 0, "%s@%s: Owner not char!",
	   UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_SHARE;
   }

   if (sarg->cmd->no == CMD_TELL)
   {
      u = find_unit(sarg->activator, &arg, 0,
		    FIND_UNIT_SURRO | FIND_UNIT_INVEN);

      arg = skip_spaces(arg);

      if (str_is_empty(arg))
      {
	 act("Command $3N to do what?",
	     A_ALWAYS, sarg->activator, 0, sarg->owner, TO_CHAR);
	 return SFR_BLOCK;
      }

      if ((u == sarg->owner) && (CHAR_MASTER(sarg->owner) == sarg->activator))
      {
	 act("You command $2n to '$3t'",
	     A_ALWAYS, sarg->activator, sarg->owner, arg, TO_CHAR);
	 act("$1n commands you to '$2t'",
	     A_ALWAYS, sarg->activator, arg, sarg->owner, TO_VICT);
	 command_interpreter(sarg->owner, arg);
	 return SFR_BLOCK;
      }
   }

   return SFR_SHARE;
}


int random_zonemove(struct spec_arg *sarg)
{
   int door;
   struct unit_data *to_room;

   if (sarg->cmd->no != CMD_AUTO_TICK)
     return SFR_SHARE;

   if (!IS_CHAR(sarg->owner))
   {
      slog(LOG_ALL, 0, "%s@%s: Owner not char!",
	   UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_SHARE;
   }

   if (CHAR_AWAKE(sarg->owner) && (CHAR_POS(sarg->owner) == POSITION_STANDING))
   {
      if ((door = random_direction(sarg->owner)) != -1)
      {
	 to_room = ROOM_EXIT(UNIT_IN(sarg->owner),door)->to_room;
	 if (!IS_SET(UNIT_FLAGS(to_room), UNIT_FL_NO_MOB))
	 {
	    if (CHAR_LAST_ROOM(sarg->owner) != to_room   &&
		unit_zone(to_room) == unit_zone(UNIT_IN(sarg->owner)))
	      command_interpreter(sarg->owner, dirs[door]);

	    if (is_destructed(DR_UNIT, sarg->owner))
	      return SFR_BLOCK;

	    CHAR_LAST_ROOM(sarg->owner) = UNIT_IN(sarg->owner);
	 }
      }
   }

   return SFR_SHARE;
}



int random_move(struct spec_arg *sarg)
{
   int door;
   struct unit_data *to_room;

   if (sarg->cmd->no != CMD_AUTO_TICK)
     return SFR_SHARE;

   if (!IS_CHAR(sarg->owner))
   {
      slog(LOG_ALL, 0, "%s@%s: Owner not char!",
	   UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_SHARE;
   }

   if (CHAR_AWAKE(sarg->owner) && (CHAR_POS(sarg->owner) == POSITION_STANDING))
   {
      if ((door = random_direction(sarg->owner)) != -1)
      {
	 to_room = ROOM_EXIT(UNIT_IN(sarg->owner),door)->to_room;

	 if (!IS_SET(UNIT_FLAGS(to_room), UNIT_FL_NO_MOB))
	 {
	    /* Do this to increase probability of not walking back'n forth */
	    if (CHAR_LAST_ROOM(sarg->owner) != to_room)
	      command_interpreter(sarg->owner, dirs[door]);

	    if (is_destructed(DR_UNIT, sarg->owner))
	      return SFR_BLOCK;

	    CHAR_LAST_ROOM(sarg->owner) = UNIT_IN(sarg->owner);
	 }
      }
   }

   return SFR_SHARE;
}



int scavenger(struct spec_arg *sarg)
{
  int max;
  struct unit_data *best_obj, *obj;

   if (!IS_CHAR(sarg->owner))
   {
      slog(LOG_ALL, 0, "%s@%s: Owner not char!",
	   UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_SHARE;
   }

  if (sarg->cmd->no == CMD_AUTO_TICK && CHAR_AWAKE(sarg->owner) &&
      !(CHAR_FIGHTING(sarg->owner)))
  {
    for (max = 50, best_obj = 0, obj = UNIT_CONTAINS(UNIT_IN(sarg->owner));
	 obj; obj = obj->next)
      if (IS_OBJ(obj) && (obj_trade_price(obj) > (sbit32) max) &&
	  !UNIT_CHARS(obj) && char_can_get_unit(sarg->owner, obj))
      {
	best_obj = obj;
	max = obj_trade_price(obj);
      }

    if (best_obj)
    {
      act("$1n gets $2n.", A_SOMEONE, sarg->owner, best_obj, 0, TO_ROOM);

      unit_from_unit(best_obj);
      unit_to_unit(best_obj, sarg->owner);

      return SFR_BLOCK;
    }
  }

  return SFR_SHARE;
}


int aggressive(struct spec_arg *sarg)
{
   int i;

   if (!IS_CHAR(sarg->owner))
   {
      slog(LOG_ALL, 0, "%s@%s: Owner not char!",
	   UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_SHARE;
   }

   if ((sarg->cmd->no == CMD_AUTO_TICK) && CHAR_AWAKE(sarg->owner) &&
       !CHAR_FIGHTING(sarg->owner))
   {
      scan4_unit(sarg->owner, UNIT_ST_PC);

      for (i = 0; i < unit_vector.top; i++)
      {
	 if (CHAR_CAN_SEE(sarg->owner, UVI(i)) &&
	     IS_MORTAL(UVI(i)) && number(0,1))
	   if (!affected_by_spell(UVI(i), ID_SANCTUARY))
	   {
	      if (sarg->fptr->data)
		act((char *) sarg->fptr->data,
		    A_SOMEONE, sarg->owner, 0, UVI(i), TO_ROOM);
	      simple_one_hit(sarg->owner, UVI(i));
	      return SFR_BLOCK;
	   }
      }
   }
   return SFR_SHARE;
}


/* Will make the mob attack players with reverse alignment of its own.    */
/* Ticks.                                                                 */
int aggres_rev_align(struct spec_arg *sarg)
{
   int i;

   if (!IS_CHAR(sarg->owner))
   {
      slog(LOG_ALL, 0, "%s@%s: Owner not char!",
	   UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_SHARE;
   }

   if (sarg->cmd->no == CMD_AUTO_TICK && CHAR_IS_READY(sarg->owner))
   {
      scan4_unit(sarg->owner, UNIT_ST_PC | UNIT_ST_NPC);

      for (i = 0; i < unit_vector.top; i++)
      {
	 if (CHAR_CAN_SEE(sarg->owner, UVI(i)) && IS_MORTAL(UVI(i)) &&
	     abs(UNIT_ALIGNMENT(sarg->owner)-UNIT_ALIGNMENT(UVI(i))) >= 1000)
	{
	   if (!affected_by_spell(UVI(i), ID_SANCTUARY))
	   {
	      simple_one_hit(sarg->owner, UVI(i));
	      return SFR_BLOCK;
	   }
	}
      }
   }

   return SFR_SHARE;
}


/* This replaces puff() and ransay()                 */
/* When called it executes the command in fptr->data */
/* via command_interpreter                           */
int mob_command(struct spec_arg *sarg)
{
   if (!IS_CHAR(sarg->owner))
   {
      slog(LOG_ALL, 0, "%s@%s: Owner not char!",
	   UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_SHARE;
   }

   if (sarg->cmd->no == CMD_AUTO_TICK)
   {
      if (!sarg->fptr->data)
      {
	 slog(LOG_ALL, 0, "%s@%s: No data in mob command.",
	     UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
	 destroy_fptr(sarg->owner, sarg->fptr);
	 return SFR_BLOCK;
      }
      command_interpreter(sarg->owner, (char *) sarg->fptr->data);
      return SFR_BLOCK;
   }
   else
     return SFR_SHARE;
}



int combat_magic(struct spec_arg *sarg)
{
   if (!IS_CHAR(sarg->owner))
   {
      slog(LOG_ALL, 0, "%s@%s: Owner not char!",
	   UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_SHARE;
   }

   if ((sarg->cmd->no == CMD_AUTO_COMBAT) && CHAR_FIGHTING(sarg->owner) &&
       CHAR_AWAKE(sarg->owner) && (CHAR_FIGHTING(sarg->owner)==sarg->activator))
   {
      if (!sarg->fptr->data)
      {
	 slog(LOG_ALL, 0, "%s@%s: No data combat magic.",
	      UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
	 destroy_fptr(sarg->owner, sarg->fptr);
	 return SFR_BLOCK;
      }

      if (cast_magic_now(sarg->owner, 12))
      {
	 command_interpreter(sarg->owner, (char *) sarg->fptr->data);
	 return SFR_BLOCK;
      }
   }

   return SFR_SHARE;
}



/* Monster magic which 'helps' the monster - will cast as soon as hp is */
/* below 52%                                                            */
int combat_magic_heal(struct spec_arg *sarg)
{
   int left;

   if (!IS_CHAR(sarg->owner))
   {
      slog(LOG_ALL, 0, "%s@%s: Owner not char!",
	   UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_SHARE;
   }

   if ((sarg->cmd->no == CMD_AUTO_COMBAT) && CHAR_FIGHTING(sarg->owner)
       && CHAR_AWAKE(sarg->owner) && (CHAR_FIGHTING(sarg->owner)==sarg->activator))
   {
      if (!sarg->fptr->data)
      {
	 slog(LOG_ALL, 0, "%s@%s: No data in mob command.",
	      UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
	 destroy_fptr(sarg->owner, sarg->fptr);
	 return SFR_BLOCK;
      }

      if (CHAR_MANA(sarg->owner) >= 12)
      {
	 left = (100*UNIT_HIT(sarg->owner))/UNIT_MAX_HIT(sarg->owner);

	 if (left < 52)
	 {
	    command_interpreter(sarg->owner, (char *) sarg->fptr->data);
	    return SFR_BLOCK;
	 }
      }
   }

   return SFR_SHARE;
}



/* String has format "<char name>{</><char-name>}<0> "   */
/* Example: "king welmar/tim/tom"                        */
/* Players are automatically non-excluded                */
/* Empty list means none                                 */
int charname_in_list(struct unit_data *ch, char *arg)
{
   char *c;

   if (!arg || !*arg || IS_PC(ch))
      return FALSE;

   while ((c = strchr(arg, '/')))
   {
      *c = '\0';
      if (UNIT_NAMES(ch).IsName(arg))
      {
	 *c = '/';
	 return TRUE;
      }
      *c = '/';
      arg = ++c;
   }
   if (UNIT_NAMES(ch).IsName(arg))
      return TRUE;

   return FALSE;
}


/* Fptr->data is a string, containing                                     */
/*    "<exitname>[location]@[exclude]@<msg1>@<msg2>"                      */
/* Where <exitname> is 0,1,2,3,4,5 for n,e,s,w,u,d respectively           */
/* <location> is the symbolic name of the room/obj the mobile must be in  */
/* The message is sent to room, you can use $1 and $3n                    */

int guard_way(struct spec_arg *sarg)
{
   char *str, *location, *excl = NULL, *msg1=NULL, *msg2=NULL;

   if (!IS_CHAR(sarg->owner))
   {
      slog(LOG_ALL, 0, "%s@%s: Owner not char!",
	   UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_SHARE;
   }

   if ((str = (char *) sarg->fptr->data) &&
       (sarg->cmd->no == (*str - '0')) && CHAR_IS_READY(sarg->owner))
   {
      if ( !(location = str+1)  ||  !(excl = strchr(location, '@'))  ||
	   !(msg1 = strchr(excl+1, '@'))  || !(msg2 = strchr(msg1+1, '@'))  )
      {
	 slog(LOG_ALL, 0, "Illegal data string in guard_way: %s", str);
	 return SFR_SHARE;
      }

      *excl = '\0';
      if (*location && strcmp(location, UNIT_FI_NAME(UNIT_IN(sarg->owner))))
      {
	 *excl = '@';
	 return SFR_SHARE;
      }
      *excl = '@';

      *msg1 = '\0';
      if (charname_in_list(sarg->activator, excl+1))
      {
	 *msg1 = '@';
	 return SFR_SHARE;
      }
      *msg1 = '@';

      *msg2 = '\0';
      act(msg1+1, A_SOMEONE, sarg->owner, 0, sarg->activator, TO_VICT);
      act(msg2+1, A_SOMEONE, sarg->owner, 0, sarg->activator, TO_NOTVICT);
      *msg2 = '@';
      return SFR_BLOCK;
   }

   return SFR_SHARE;
}



/* Prevents the 'activator' from opening/unlocking the unit.         */
/* Fptr->data is a string containing:                                */
/*    "[location]@<unit-name>@[exclude]@<msg1>@<msg2>"               */

int guard_unit(struct spec_arg *sarg)
{
   char *arg = (char *) sarg->arg;
   char *str=NULL, *location, *excl=NULL, *msg1=NULL, *msg2=NULL;
   char *unitname = NULL, *c;
   struct unit_data *u1, *u2;

   if (!IS_CHAR(sarg->owner))
   {
      slog(LOG_ALL, 0, "%s@%s: Owner not char!",
	   UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_SHARE;
   }

   if (((sarg->cmd->no == CMD_OPEN) || (sarg->cmd->no == CMD_UNLOCK) ||
	(sarg->cmd->no == CMD_PICK)) && (str = (char *) sarg->fptr->data)
       && CHAR_IS_READY(sarg->owner))
   {
      if ( !(location = str)  ||  !(unitname = strchr(location, '@')) ||
	   !(excl = strchr(unitname+1, '@')) ||
	   !(msg1 = strchr(excl + 1, '@')) || !(msg2 = strchr(msg1 + 1, '@')))
      {
	 slog(LOG_ALL, 0,"Illegal data string in guard_way: %s", str);
	 return SFR_SHARE;
      }

      *unitname = '\0';
      if (*location && strcmp(location, UNIT_FI_NAME(UNIT_IN(sarg->owner))))
      {
	 *unitname = '@';
	 return SFR_SHARE;
      }
      *unitname = '@';


      *excl = '\0';
      c = unitname + 1;
      u1 = find_unit(sarg->owner, &c, 0, FIND_UNIT_SURRO);
      *excl = '@';
      u2 = find_unit(sarg->activator, &arg, 0, FIND_UNIT_SURRO);

      if (!u1 || (u1 != u2))
	 return SFR_SHARE;

      *msg1 = '\0';
      if (charname_in_list(sarg->activator, excl+1))
      {
	 *msg1 = '@';
	 return SFR_SHARE;
      }
      *msg1 = '@';

      *msg2 = '\0';
      act(msg1+1, A_SOMEONE, sarg->owner, 0, sarg->activator, TO_VICT);
      act(msg2+1, A_SOMEONE, sarg->owner, 0, sarg->activator, TO_NOTVICT);
      *msg2 = '@';
      return SFR_BLOCK;
   }

   return SFR_SHARE;
}


/* Prevents the 'activator' from opening/unlocking the door.            */
/* Fptr->data is a string containing:                                   */
/*    "[location]@<door-name>@[exclude]@<message>@<message>             */

int guard_door(struct spec_arg *sarg)
{
   char *arg = (char *) sarg->arg;
   char *str=NULL, *excl=NULL, *msg1=NULL, *msg2=NULL;
   char *location, *doorname = NULL;
   int i1, i2;

   if (!IS_CHAR(sarg->owner))
   {
      slog(LOG_ALL, 0, "%s@%s: Owner not char!",
	   UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_SHARE;
   }

   if (((sarg->cmd->no == CMD_OPEN) || (sarg->cmd->no == CMD_UNLOCK) ||
	(sarg->cmd->no == CMD_PICK)) && (str = (char *) sarg->fptr->data)
       && CHAR_IS_READY(sarg->owner))
   {
      if (!(location = str)  ||  !(doorname = strchr(location, '@')) ||
	  !(excl = strchr(doorname+1, '@')) ||
	  !(msg1 = strchr(excl + 1, '@')) || !(msg2 = strchr(msg1 + 1, '@')) )
      {
	 slog(LOG_ALL, 0,"Illegal data string in guard_way: %s", str);
	 return SFR_SHARE;
      }

      *doorname = '\0';
      if (*location && strcmp(location, UNIT_FI_NAME(UNIT_IN(sarg->owner))))
      {
	 *doorname = '@';
	 return SFR_SHARE;
      }
      *doorname = '@';

      *excl = '\0';
      i1 = low_find_door(sarg->owner, doorname+1, FALSE, FALSE);
      *excl = '@';

      if (i1 == -1)
	 return SFR_SHARE;

      if (i1 != (i2 = low_find_door(sarg->activator, arg, FALSE, FALSE)))
	 return SFR_SHARE;

      *msg1 = '\0';
      if (charname_in_list(sarg->activator, excl+1))
      {
	 *msg1 = '@';
	 return SFR_SHARE;
      }
      *msg1 = '@';

      *msg2 = '\0';
      act(msg1+1, A_SOMEONE, sarg->owner, 0, sarg->activator, TO_VICT);
      act(msg2+1, A_SOMEONE, sarg->owner, 0, sarg->activator, TO_NOTVICT);
      *msg2 = '@';
      return SFR_BLOCK;
   }
   return SFR_SHARE;
}

/* Fptr->data is a string, containing                                     */
/*    "<exitno>[location]@<level_min>-<level_max>@<msg1>@<msg2>"          */
/* Where <exitno> is 0,1,2,3,4,5 for n,e,s,w,u,d respectively             */
/* <location> is the symbolic name of the room/obj the mobile must be in  */
/* The message is sent to room, you can use $1n and $3n                   */

int guard_way_level(struct spec_arg *sarg)
{
   char *str, *location, *min=NULL, *max=NULL, *msg1=NULL, *msg2=NULL;

   if (!IS_CHAR(sarg->owner))
   {
      slog(LOG_ALL, 0, "%s@%s: Owner not char!",
	   UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_SHARE;
   }

   if ((str= (char *) sarg->fptr->data) &&
       (sarg->cmd->no == (*str - '0')) && CHAR_IS_READY(sarg->owner))
   {
      if (!(location = str+1)  ||  !(min = strchr(location, '@'))  ||
	  !(max = strchr(min+1, '-')) ||
	  !(msg1 = strchr(max+1, '@'))  || !(msg2 = strchr(msg1+1, '@'))  )
      {
	 slog(LOG_ALL, 0, "Illegal data string in guard_way: %s", str);
	 return SFR_SHARE;
      }

      *min = '\0';
      if (*location && strcmp(location, UNIT_FI_NAME(UNIT_IN(sarg->owner))))
      {
	 *min = '@';
	 return SFR_SHARE;
      }
      *min = '@';

      *max = '\0';
      *msg1 = '\0';

      if(CHAR_LEVEL(sarg->activator) >= atoi(min+1) || CHAR_LEVEL(sarg->activator) <= atoi(max+1))
      {
	 *max = '-';
	 *msg1 = '@';
	 return SFR_SHARE;
      }
      *max = '-';
      *msg1 = '@';

      *msg2 = '\0';
      act(msg1+1, A_SOMEONE, sarg->owner, 0, sarg->activator, TO_VICT);
      act(msg2+1, A_SOMEONE, sarg->owner, 0, sarg->activator, TO_NOTVICT);
      *msg2 = '@';
      return SFR_BLOCK;
   }

   return SFR_SHARE;
}



/* data is sting with name(s), for example "king welmar/tim/tom" */
int rescue(struct spec_arg *sarg)
{
   void base_rescue(struct unit_data *ch, struct unit_data *victim);

   if (!IS_CHAR(sarg->owner))
   {
      slog(LOG_ALL, 0, "%s@%s: Owner not char!",
	   UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_SHARE;
   }

   if (sarg->cmd->no == CMD_AUTO_COMBAT  &&  CHAR_FIGHTING(sarg->activator)  &&
       (CHAR_FIGHTING(sarg->activator) != sarg->owner) && CHAR_AWAKE(sarg->owner))
   {
      if (!sarg->fptr->data)
      {
	 slog(LOG_ALL, 0, "%s@%s: No data in rescue.",
	      UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
	 destroy_fptr(sarg->owner, sarg->fptr);
	 return SFR_BLOCK;
      }

      if (((100*UNIT_HIT(sarg->owner))/UNIT_MAX_HIT(sarg->owner) >= 50) &&
	  charname_in_list(CHAR_FIGHTING(sarg->activator),
			   (char *) sarg->fptr->data))
      {
	 /* If victim has less than 50% hp, and our sarg->owner has more than */
	 /* half of his left, then let him rescue our victim           */
	 if ( ((100*UNIT_HIT(CHAR_FIGHTING(sarg->activator)))  /
	       UNIT_MAX_HIT(CHAR_FIGHTING(sarg->activator)) < 50))
	 {
	    /* Do NOT rescue if legal taget & protected */
	    if (IS_SET(CHAR_FLAGS(CHAR_FIGHTING(sarg->activator)),
		       CHAR_PROTECTED) &&
		IS_SET(CHAR_FLAGS(CHAR_FIGHTING(sarg->activator)),
		       CHAR_LEGAL_TARGET))
	      return SFR_SHARE;

	    base_rescue(sarg->owner, CHAR_FIGHTING(sarg->activator));
	    if (number(0,1))
	      return SFR_SHARE;
	    else
	      return SFR_BLOCK;
	 }
      }
   }
   return SFR_SHARE;
}


/* data is sting with name(s), for example "king welmar/tim/tom" */
/* Is usually used with rescue                                   */
int teamwork(struct spec_arg *sarg)
{
   if (!IS_CHAR(sarg->owner))
   {
      slog(LOG_ALL, 0, "%s@%s: Owner not char!",
	   UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_SHARE;
   }

   if (sarg->cmd->no == CMD_AUTO_COMBAT  &&  !CHAR_FIGHTING(sarg->owner) &&
       CHAR_AWAKE(sarg->owner) && CHAR_FIGHTING(sarg->activator)  &&
       (CHAR_FIGHTING(sarg->activator) != sarg->owner) )
   {
      if (!sarg->fptr->data)
      {
	 slog(LOG_ALL, 0, "%s@%s: No data in mob command.",
	      UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
	 destroy_fptr(sarg->owner, sarg->fptr);
	 return SFR_BLOCK;
      }

      if (charname_in_list(CHAR_FIGHTING(sarg->activator),
			   (char *) sarg->fptr->data))
      {
	 /* The 'sarg->owner' isn't fighting, and 'sarg->activator' is fightn*/
	 /* someone from his group, go kill the bastard!            */

	 /* Do NOT attack if legal taget & protected */
	 if (IS_SET(CHAR_FLAGS(sarg->activator), CHAR_PROTECTED) &&
	     IS_SET(CHAR_FLAGS(sarg->activator), CHAR_LEGAL_TARGET))
	   return SFR_SHARE;

	 simple_one_hit(sarg->owner, sarg->activator);
	 return SFR_BLOCK;
      }
   }
   return SFR_SHARE;
}


int hideaway(struct spec_arg *sarg)
{
   if (sarg->cmd->no == CMD_AUTO_EXTRACT)
     return SFR_SHARE;

   if (!IS_CHAR(sarg->owner))
   {
      szonelog(UNIT_FI_ZONE(sarg->owner),
	       "%s@%s: HIDE-AWAY Funciton can only be on CHARs",
	       UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_BLOCK;
   }

   if (sarg->cmd->no == CMD_AUTO_TICK && CHAR_AWAKE(sarg->owner) &&
       CHAR_POS(sarg->owner) != POSITION_FIGHTING)
      SET_BIT(CHAR_FLAGS(sarg->owner), CHAR_HIDE);

   return SFR_SHARE;
}

/* ------------------------------------------------------------------------ */

struct mercenary_data
{
   int ticks;
   char *victim_name;
   struct unit_data *destination;
};


int mercenary_hire(struct spec_arg *sarg)
{
   char *arg = (char *) sarg->arg;
   char buf2[MAX_INPUT_LENGTH];
   struct unit_data *u, *victim;
   struct mercenary_data *md;
   struct extra_descr_data *exd;
   amount_t price;
   currency_t currency = local_currency(sarg->owner);

   void start_special(struct unit_data *u, struct unit_fptr *fptr);

   if (!IS_CHAR(sarg->owner))
   {
      slog(LOG_ALL, 0, "%s@%s: Owner not char!",
	   UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_SHARE;
   }

   if (CHAR_AWAKE(sarg->owner))
   {
      if (sarg->cmd->no == CMD_CONTRACT)
      {
	 act("$1n tries to make a deal with $3n.",
	     A_SOMEONE,sarg->activator,0,sarg->owner,TO_ROOM);

	 if (!(victim = find_unit(sarg->owner, &arg,0, FIND_UNIT_WORLD)) ||
	     !IS_CHAR(victim))
	 {
	    act("$1n says, 'Never heard of him.'",
		A_SOMEONE, sarg->owner, 0, 0, TO_ROOM);
	    return SFR_BLOCK;
	 }
	 if (victim == sarg->owner)
	 {
	    act("$1n says, 'Get lost, punk!'",A_SOMEONE,sarg->owner,0,0,TO_ROOM);
	    return SFR_BLOCK;
	 }
	 price = MAX(5000, 500*
		     (CHAR_LEVEL(victim)+CHAR_LEVEL(sarg->owner)*CHAR_LEVEL(sarg->owner)));

	 if (!char_can_afford(sarg->activator, price, currency))
	 {
	    act("$1n says, 'Get me $2t'", A_SOMEONE, sarg->owner,
		money_string(price, currency, TRUE), 0, TO_ROOM);
	    return SFR_BLOCK;
	 }

	 for (u = UNIT_CONTAINS(sarg->owner); u; u = u->next)
	 {
	    if (IS_OBJ(u) && OBJ_TYPE(u) == ITEM_NOTE)
	    {
	       sprintf(buf2, "Contract on %s (%s) for %s.\n\r\n\r",
		       STR(UNIT_TITLE_STRING(victim)),
		       UNIT_NAME(sarg->activator),
		       money_string(price, currency, TRUE));

	       exd = unit_find_extra(UNIT_NAME(u), u);
	       if (!exd)
		 UNIT_EXTRA_DESCR(u) =
		   UNIT_EXTRA_DESCR(u)->add((char *) NULL, buf2);
	       else
	       {
		  char buf[MAX_STRING_LENGTH];

		  if (strlen(buf2) + exd->descr.Length() > sizeof(buf))
		    strcpy(buf, buf2);
		  else
		  {
		     strcpy(buf, STR(exd->descr.String()));
		     strcat(buf, buf2);
		  }

		  exd->descr.Reassign(buf);
	       }
	       break;
	    }
	 }

	 money_transfer(sarg->activator, sarg->owner, price, currency);

	 CREATE(md, struct mercenary_data, 1);
	 md->victim_name = str_dup(sarg->arg);
	 md->ticks = 0;
	 md->destination = 0;

	 sarg->fptr->data = md;
	 sarg->fptr->index = SFUN_MERCENARY_HUNT;
	 sarg->fptr->heart_beat = WAIT_SEC*5;
	 sarg->fptr->flags = SFB_PRIORITY | SFB_RANTIME | SFB_TICK | SFB_DEAD;
	 start_special(sarg->owner, sarg->fptr);

	 act("$1n says, 'Consider $2n dead.'",A_SOMEONE,sarg->owner,victim,0,TO_ROOM);
	 return SFR_BLOCK;
      }
      else if (sarg->cmd->no == CMD_OFFER)
      {
	 act("$1n talks to $3n.",
	     A_SOMEONE,sarg->activator,0,sarg->owner,TO_ROOM);

	 if (!(victim = find_unit(sarg->owner, &arg, 0, FIND_UNIT_WORLD)) ||
	     !IS_CHAR(victim))
	 {
	    act("$1n says, 'Never heard of him.'",
		A_SOMEONE, sarg->owner, 0, 0, TO_ROOM);
	    return SFR_BLOCK;
	 }
	 if (victim == sarg->owner)
	 {
	    act("$1n says, 'Get lost, punk!'",A_SOMEONE,sarg->owner,0,0,TO_ROOM);
	    return SFR_BLOCK;
	 }
	 price = MAX(5000, 500*
		     (CHAR_LEVEL(victim)+CHAR_LEVEL(sarg->owner)*CHAR_LEVEL(sarg->owner)));

	 act("$1n says, '$3t, and $2n is history.'", A_SOMEONE, sarg->owner, victim,
	     money_string(price, currency, TRUE), TO_ROOM);
	 return SFR_BLOCK;
      }
   }
   return SFR_SHARE;
}



int mercenary_hunt(struct spec_arg *sarg)
{
   struct mercenary_data *md;
   struct unit_data *u;
   char *c;
   int i;

   md = (struct mercenary_data *) sarg->fptr->data;

   if (sarg->cmd->no == CMD_AUTO_TICK)
   {
      if (!md)
      {
	 slog(LOG_ALL, 0, "%s@%s: No data in hunt.",
	      UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
	 destroy_fptr(sarg->owner, sarg->fptr);
	 return SFR_BLOCK;
      }

      md->ticks++;

      if (md->ticks % 7 == 0)
      {
	 c = md->victim_name;

	 if ((md->destination = find_unit(sarg->owner, &c, 0, FIND_UNIT_WORLD)))
	 {
	    if (IS_CHAR(md->destination))
	      for (; !IS_ROOM(md->destination);
		   md->destination = UNIT_IN(md->destination))   ;
	    else
	      md->destination = NULL;
	 }
      }

      if (!md->destination)
	return SFR_BLOCK;

      i = npc_move(sarg->owner, md->destination);

      if (i == MOVE_DEAD)  /* NPC Died */
	return SFR_BLOCK;

      if (i == MOVE_GOAL)  /* Here... */
      {
	 c = md->victim_name;
	 u = find_unit(sarg->owner, &c, 0, FIND_UNIT_SURRO);

	 if (u)
	 {
	    simple_one_hit(sarg->owner, u);
	    return SFR_BLOCK;
	 }
	 else
	   md->destination = 0;  /* The chicken has left... :) */
      }

      return SFR_BLOCK; /* Prevent other routines from taking over */
   }
   else if (sarg->cmd->no == CMD_AUTO_EXTRACT)
   {
      free(md->victim_name);
      free(md);
      sarg->fptr->data = 0;
      return SFR_BLOCK;
   }
   else if (sarg->cmd->no == CMD_AUTO_DEATH)
   {
      if (UNIT_NAME(sarg->activator) &&
	  (str_ccmp(UNIT_NAME(sarg->activator), md->victim_name)==0))
      {
	 char mbuf[MAX_INPUT_LENGTH];
	 strcpy(mbuf, "get all from corpse");
	 command_interpreter(sarg->owner, mbuf);
	 free(md->victim_name);
	 free(md);
	 sarg->fptr->data = 0;
	 sarg->fptr->index = SFUN_MERCENARY_HIRE;
	 sarg->fptr->heart_beat = 0;
	 sarg->fptr->flags = SFB_CMD;
	 return SFR_BLOCK;
      }
   }

   return SFR_SHARE;
}


/* ------------------------------------------------------------------------- */
/*                     O B J E C T   R O U T I N E S                         */
/* ------------------------------------------------------------------------- */

static void tuborg_log(const char *name, int cmd)
{
   FILE *f;
   extern char libdir[];

   f = fopen_cache(str_cc(libdir, STATISTICS_FILE), "a+b");
   assert(f);
   fprintf(f, "TUBORG %s %d %ld\n", name, cmd, (long) time(0));
}

int green_tuborg(struct spec_arg *sarg)
{
   struct unit_affected_type *af;
   int oldval;
   char *c = (char *) sarg->arg;

   if (sarg->cmd->no != CMD_TASTE &&
       sarg->cmd->no != CMD_SIP && sarg->cmd->no != CMD_DRINK)
     return SFR_SHARE;
   
   if (find_unit(sarg->activator, &c, 0, FIND_UNIT_HERE) != sarg->owner)
     return SFR_SHARE;
   
   if (!CHAR_AWAKE(sarg->activator))
     return SFR_SHARE;

   if (OBJ_VALUE(sarg->owner, 1) == 0) /* Empty */
     return SFR_SHARE;

   oldval = OBJ_VALUE(sarg->owner, 1);

   switch (sarg->cmd->no)
   {
     case CMD_SIP:
     case CMD_TASTE:
      act("$1n tastes $2n enjoying every drop.", A_HIDEINV,
	  sarg->activator, sarg->owner, 0, TO_ROOM);
      act("The taste of $2n is nothing less than divine.", A_HIDEINV,
	  sarg->activator, sarg->owner, 0, TO_CHAR);
      tuborg_log(UNIT_NAME(sarg->activator), sarg->cmd->no);
      return SFR_BLOCK;
    
     case CMD_DRINK:
      if (IS_PC(sarg->activator))
      {
	 PC_COND(sarg->activator, THIRST) = 16;
	 PC_COND(sarg->activator, FULL) = 16;
      }

      do_drink(sarg->activator, (char *) sarg->arg, sarg->cmd); 
      if (OBJ_VALUE(sarg->owner, 1) == oldval) /* No change??? Wierd... ? */
	return SFR_BLOCK;
   
      if (IS_PC(sarg->activator))
      {
	 if (IS_MORTAL(sarg->activator))
	 {
	    PC_COND(sarg->activator, THIRST) = 24;
	    PC_COND(sarg->activator, FULL) = 24;
	 } else {
	    PC_COND(sarg->activator, THIRST) = 48;
	    PC_COND(sarg->activator, FULL) = 48;
	 }
      }
    
      act("The $2N has made you feel more energetic!",
	  A_HIDEINV, sarg->activator, sarg->owner, 0, TO_CHAR);
      act("$1n drinks $2n and looks more energetic!",
	  A_HIDEINV, sarg->activator, sarg->owner, 0, TO_ROOM);
      CHAR_ENDURANCE(sarg->activator) = move_limit(sarg->activator);
      UNIT_HIT(sarg->activator) = MIN(UNIT_MAX_HIT(sarg->activator), UNIT_HIT(sarg->activator) + 3);
      if ((af = affected_by_spell(sarg->activator, ID_SLEEP)))
      {
	 act("This fabulous $2N has made you feel less sleepy!",
	     A_ALWAYS, sarg->activator, sarg->owner, 0, TO_CHAR);
	 act("$1n seems less sleepy after drinking the $2N!",
	     A_ALWAYS, sarg->activator, sarg->owner, 0, TO_CHAR);
	 destroy_affect(af);
      }
      tuborg_log(UNIT_NAME(sarg->activator), sarg->cmd->no);
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_BLOCK;
   }

   return SFR_SHARE;
}


int obj_guild(struct spec_arg *sarg)
{
   void modify_hit(struct unit_data *ch, int hit);

   void modify_hit(struct unit_data *ch, int hit);

   char *guild = (char *) sarg->fptr->data;

   if (sarg->cmd->no == CMD_AUTO_EXTRACT)
     return SFR_SHARE;

   if (guild == NULL)
   {
      slog(LOG_ALL, 0, "%s@%s: No data in obj_guild.",
	   UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_BLOCK;
   }

   if (sarg->activator && UNIT_IN(sarg->owner) == sarg->activator &&
       IS_PC(sarg->activator) &&
       (!PC_GUILD(sarg->activator) ||
	(strcmp(PC_GUILD(sarg->activator), guild) != 0)))
   {
      act("$3n burns your hands, and you must drop $3m.\n\r"
	  "Being in the right guild would probably help.",
	  A_SOMEONE, sarg->activator, 0, sarg->owner, TO_CHAR);
      act("$1n is burnt by $3n and drops $3m immediately.",
	  A_SOMEONE, sarg->activator, 0, sarg->owner, TO_ROOM);
      if (IS_OBJ(sarg->owner) && OBJ_EQP_POS(sarg->owner))
	unequip_object(sarg->owner);
      unit_up(sarg->owner);

      return SFR_BLOCK;
   }

   return SFR_SHARE;
}


int obj_quest(struct spec_arg *sarg)
{
   struct extra_descr_data *find_quest(char *word, struct unit_data *unit);
   char *quest = (char *) sarg->fptr->data;

   if (quest == NULL)
   {
      slog(LOG_ALL, 0, "%s@%s: No data in obj_quest.",
	   UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_BLOCK;
   }

   if (sarg->activator && UNIT_IN(sarg->owner) == sarg->activator &&
       !find_quest(quest, sarg->activator))
   {
      act("$3n burns your hands, and you must drop $3m.\n\r"
	  "You have apparantly not completed the right quest!",
	  A_SOMEONE, sarg->activator, 0, sarg->owner, TO_CHAR);
      act("$1n is burnt by $3n and drops $3m immediately.",
	  A_SOMEONE, sarg->activator, 0, sarg->owner, TO_ROOM);
      if (IS_OBJ(sarg->owner) && OBJ_EQP_POS(sarg->owner))
	unequip_object(sarg->owner);
      unit_up(sarg->owner);

      return SFR_BLOCK;
   }

   return SFR_SHARE;
}


int obj_good(struct spec_arg *sarg)
{
   if (sarg->activator && UNIT_IN(sarg->owner) == sarg->activator &&
       !UNIT_IS_GOOD(sarg->activator))
   {
      act("$3n burns your hands, and you must drop it.",
	  A_SOMEONE, sarg->activator, 0, sarg->owner, TO_CHAR);
      act("$1n is burnt by $3n and drops $3m immediately.",
	  A_SOMEONE, sarg->activator, 0, sarg->owner, TO_ROOM);
      if (IS_OBJ(sarg->owner) && OBJ_EQP_POS(sarg->owner))
	unequip_object(sarg->owner);
      unit_up(sarg->owner);

      if (IS_CHAR(sarg->activator))
	modify_hit(sarg->activator, -UNIT_MAX_HIT(sarg->activator)/4);

      return SFR_BLOCK;
   }

   return SFR_SHARE;
}


int obj_evil(struct spec_arg *sarg)
{
   if (sarg->activator  &&
       UNIT_IN(sarg->owner) == sarg->activator &&
       !UNIT_IS_EVIL(sarg->activator))
   {
      act("$3n burns your hands, and you must drop it.",
	  A_SOMEONE, sarg->activator, 0, sarg->owner, TO_CHAR);
      act("$1n is burnt by $3n and drops $3m immediately.",
	  A_SOMEONE, sarg->activator, 0, sarg->owner, TO_ROOM);
      if (IS_OBJ(sarg->owner) && OBJ_EQP_POS(sarg->owner))
	unequip_object(sarg->owner);
      unit_up(sarg->owner);

      if (IS_CHAR(sarg->activator))
	modify_hit(sarg->activator, -UNIT_MAX_HIT(sarg->activator)/4);

      return SFR_BLOCK;
   }

   return SFR_SHARE;
}


int blow_away(struct spec_arg *sarg)
{
   if (sarg->cmd->no == CMD_AUTO_TICK)
   {
      if (IS_ROOM(UNIT_IN(sarg->owner)))
      {
	 if (sarg->fptr->data)
	   act((char *) sarg->fptr->data, A_SOMEONE, sarg->owner, 0, 0, TO_ROOM);
	 else
	   act("The $1N blows away.",A_SOMEONE, sarg->owner, 0, 0, TO_ROOM);
	 extract_unit(sarg->owner);
      }
      return SFR_BLOCK;
   }

   return SFR_SHARE;
}


/* Will make the user of the charm lose up to 200k less exp at death! */
/* value[3] decides how many 'charges' are in it.                     */
int charm_of_death(struct spec_arg *sarg)
{
   void gain_exp(struct unit_data *ch, int gain);
   extern int lose_exp(struct unit_data *);

   if (sarg->cmd->no == CMD_AUTO_DEATH)
     if (UNIT_IN(sarg->owner) == sarg->activator && OBJ_EQP_POS(sarg->owner) && IS_PC(sarg->activator) &&
	 CHAR_LEVEL(sarg->activator) <= MORTAL_MAX_LEVEL && OBJ_VALUE(sarg->owner,3) > 0)
     {
	int loss = lose_exp(sarg->activator);
	
	gain_exp(sarg->activator, loss/2);
	
	if (!--OBJ_VALUE(sarg->owner,3))
	{
	   act("$3n emits a faint blue light, then vanishes.",
	       A_SOMEONE,sarg->activator,0,sarg->owner,TO_ROOM);
	   extract_unit(sarg->owner);
	   return SFR_SHARE;
	}
     }

   return SFR_SHARE;
}


/* Used by "summoned" monsters when combat is over. */
int return_to_origin(struct spec_arg *sarg)
{
   if (sarg->cmd->no == CMD_AUTO_EXTRACT)
   {
      sarg->fptr->data = NULL;
      return SFR_SHARE;
   }

   if ((sarg->cmd->no == CMD_AUTO_TICK) && CHAR_IS_READY(sarg->owner))
   {
      act("$1n disappears into a rift!",
	  A_ALWAYS, sarg->owner, 0, 0, TO_ROOM);
      unit_from_unit(sarg->owner);
      unit_to_unit(sarg->owner, (struct unit_data *) sarg->fptr->data);
      act("$1n disappears appears though a rift!",
	  A_ALWAYS, sarg->owner, 0, 0, TO_ROOM);
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_BLOCK;
   }

   return SFR_SHARE;
}


/* Used by "summoned" monsters when combat is over. */
int restrict_obj(struct spec_arg *sarg)
{
   if ((sarg->cmd->no == CMD_WEAR ||
        sarg->cmd->no == CMD_WIELD ||
        sarg->cmd->no == CMD_HOLD) &&
       IS_OBJ(sarg->owner) &&
       UNIT_IN(sarg->owner) == sarg->activator &&
       !OBJ_EQP_POS(sarg->owner))
   {
      char *arg = (char *) sarg->arg;
      char pMsg[80];
      char *pTmp, *pOrg;
      int i, ok, all;

      pMsg[0] = 0;

      all = (str_ccmp_next_word(arg, "all") != NULL);
      if (!all)
	all = (str_cstr(arg, ".all") != NULL);
 
      if ((sarg->owner !=
	   find_unit(sarg->activator, &arg, 0, FIND_UNIT_INVEN)) && !all)
        return SFR_SHARE;

      pTmp = pOrg = (char *) sarg->fptr->data;
      pTmp = pOrg; ok = parse_match_num(&pTmp, "Str", &i);
      if (ok && CHAR_STR(sarg->activator) < i)
	sprintf(pMsg, "strong (%d strength needed)",
		i - CHAR_STR(sarg->activator));

      pTmp = pOrg; ok = parse_match_num(&pTmp, "Dex", &i);
      if (ok && CHAR_DEX(sarg->activator) < i)
	sprintf(pMsg, "dexterous (%d dexterity needed)",
		i - CHAR_DEX(sarg->activator));

      pTmp = pOrg; ok = parse_match_num(&pTmp, "Con", &i);
      if (ok && CHAR_CON(sarg->activator) < i)
	sprintf(pMsg, "sturdy (%d constitution needed)",
		i - CHAR_CON(sarg->activator));

      pTmp = pOrg; ok = parse_match_num(&pTmp, "Bra", &i);
      if (ok && CHAR_BRA(sarg->activator) < i)
	sprintf(pMsg, "smart (%d brain needed)", i - CHAR_BRA(sarg->activator));

      pTmp = pOrg; ok = parse_match_num(&pTmp, "Mag", &i); 
      if (ok && CHAR_MAG(sarg->activator) < i)
	sprintf(pMsg, "magical (%d magic needed)", i - CHAR_MAG(sarg->activator));

      pTmp = pOrg; ok = parse_match_num(&pTmp, "Div", &i);
      if (ok && CHAR_DIV(sarg->activator) < i)
	sprintf(pMsg, "divine (%d divine needed)", i - CHAR_DIV(sarg->activator));

      pTmp = pOrg; ok = parse_match_num(&pTmp, "Hpp", &i);
      if (ok && CHAR_HPP(sarg->activator) < i)
	sprintf(pMsg, "tough (%d strength needed)",
		i - CHAR_HPP(sarg->activator));

      pTmp = pOrg; ok = parse_match_num(&pTmp, "Level", &i);
      if (ok && CHAR_LEVEL(sarg->activator) < i)
	sprintf(pMsg, "experienced (%d levels needed)",
		i - CHAR_LEVEL(sarg->activator));

      if (*pMsg)
      {
         if (all)
         {
            act("As you attempt to use the $3N you realize that you are not "
		"$2t enough to use it.",
                A_ALWAYS, sarg->activator, pMsg, sarg->owner, TO_CHAR);
            return SFR_BLOCK;
         }
         else
         {
            act("You are not $2t enough to use the $3N.",
                A_ALWAYS, sarg->activator, pMsg, sarg->owner, TO_CHAR);
            return SFR_BLOCK;
	 }
      }
   }

   return SFR_SHARE;
}

/* Ok, the char is LD, lets get rid of him as soon as combat is resolved! */
/* Note that he may be extracted with less than zero hitpoints if incap   */
/* or mortally wounded                                                    */

int link_dead(struct spec_arg *sarg)
{
   if ((sarg->cmd->no >= CMD_NORTH) && sarg->activator && (sarg->activator != sarg->owner))
   {
      if (CHAR_DESCRIPTOR(sarg->owner))
      {
	 destroy_fptr(sarg->owner, sarg->fptr);
	 return SFR_SHARE;
      }

      if (CHAR_FIGHTING(sarg->owner))
	return SFR_SHARE;

      act("$1n is link dead and so leaves the game.",
	  A_HIDEINV, sarg->owner, 0, 0, TO_ROOM);
      extract_unit(sarg->owner);
      return SFR_SHARE;
   }

   return SFR_SHARE;
}
