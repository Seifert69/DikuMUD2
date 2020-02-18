/* *********************************************************************** *
 * File   : demigod.c                                 Part of Valhalla MUD *
 * Version: 1.01                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Handling the demigod stuff                                     *
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
/* Feb 21 1995, gnort:	You can't sacrifice to yourself, and you can't
 *			sacrifice characters.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
#include "affect.h"
#include "utility.h"
#include "money.h"

void save_player_file(struct unit_data *pc);

void add_sacrifice_info(struct unit_data *demi,
			struct unit_data *ch,
			long power)
{
   char Buf[200];
   struct extra_descr_data *exd;

   sprintf(Buf, "$S%s", UNIT_NAME(ch));
   str_lower(Buf+2);
   exd = PC_QUEST(demi)->find_raw(Buf);
   if (exd == NULL)
   {
      quest_add(demi, Buf, ltoa(power));
   }
   else
   {
      power += atol(exd->descr.String());
      exd->descr.Reassign(ltoa(power));
   }
}

void do_manifest(struct unit_data *ch, char *arg,
		 const struct command_info *cmd)
{
#ifndef DEMIGOD
   send_to_char("This command has been removed.\r\n", ch);
   return;
#else
   struct unit_data *player;
   struct unit_data *monster;

   extern struct file_index_type *demigod_fi;

   void switchbody(struct unit_data *ch, struct unit_data *victim);
   void unswitchbody(struct unit_data *npc);


   if (!CHAR_DESCRIPTOR(ch))
     return;

   if (CHAR_IS_SWITCHED(ch))
   {
      act("$1n dissolves.", A_HIDEINV, ch, 0, 0, TO_ROOM);
      unswitchbody(ch);
      extract_unit(ch);
      return;
   }

   if (!IS_DEMIGOD(ch))
   {
      send_to_char("Only works for demigods.\n\r", ch);
      return;
   }

   if (str_is_empty(arg))
   {
      send_to_char("You return home.\n\r", ch);
      act("$1n returns to $1s home in the sky.", A_HIDEINV, ch, 0, 0, TO_ROOM);
      unit_from_unit(ch);
      unit_to_unit(ch, hometown_unit(PC_HOME(ch)));
      return;
   }

   player = find_unit(ch, &arg, 0, FIND_UNIT_WORLD | FIND_UNIT_SURRO);

   if (!player || !IS_PC(player))
   {
      send_to_char("No such player around to manifest at.\n\r", ch);
      return;
   }

   if (IS_SET(UNIT_FLAGS(UNIT_IN(player)), UNIT_FL_PRIVATE))
   {
     struct unit_data *pers = UNIT_CONTAINS(UNIT_IN(player));
     int i;

     for (i = 0; pers; pers = pers->next)
       if (IS_PC(pers))
	  i++;

      if (i > 1)
      {
	send_to_char
	  ("There's a private conversation going on in that room.\n\r", ch);
	return;
      }
   }

   if (str_ccmp_next_word(arg, "self") && !unit_recursive(ch, player))
   {
      int power = MAX(1, CHAR_LEVEL(ch)-5) * 500;
      if (CHAR_EXP(ch) < power)
      {
	 send_to_char("You do not have enough power.\n\r", ch);
	 return;
      }
      CHAR_EXP(ch) -= power;

      act("$1n disappears in a flash of bright light.",
	  A_ALWAYS, ch, 0, 0, TO_ROOM);

      unit_from_unit(ch);
      unit_to_unit(ch, UNIT_IN(player));

      act("You appear as yourself.", A_ALWAYS, ch, 0, 0, TO_CHAR);

      act("$1n appears in front of you.", A_ALWAYS, ch, 0, 0, TO_ROOM);
      return;
   }

   if (!str_is_empty(arg))
   {
      int power;

      monster = find_unit(ch, &arg, 0, FIND_UNIT_WORLD | FIND_UNIT_SURRO);

      if (!monster || !IS_NPC(monster))
      {
	 send_to_char("No such monster around to appear as.\n\r", ch);
	 return;
      }

      if (!UNIT_FILE_INDEX(monster))
      {
	 send_to_char("That monster is not available.\n\r", ch);
	 return;
      }

      power = MAX(1, CHAR_LEVEL(monster)-5) * 500;
      if (CHAR_EXP(ch) < power)
      {
	 send_to_char("You do not have enough power.\n\r", ch);
	 return;
      }
      CHAR_EXP(ch) -= power;

      monster = read_unit(UNIT_FILE_INDEX(monster));

      while (UNIT_FUNC(monster))
	destroy_fptr(monster, UNIT_FUNC(monster));

      while (UNIT_AFFECTED(monster))
	destroy_affect(UNIT_AFFECTED(monster));

      while (UNIT_CONTAINS(monster))
	extract_unit(UNIT_CONTAINS(monster)); /* extract money */
   }
   else
   {
      char tmpbuf[50];
      struct file_index_type *fi;

      sprintf(tmpbuf, UNIT_NAME(ch));
      str_lower(tmpbuf);
      fi = find_file_index("demigod", tmpbuf);

      if (fi == NULL)
	monster = read_unit(demigod_fi);
      else
	monster = read_unit(fi);
   }

   unit_to_unit(monster, UNIT_IN(player));

   switchbody(ch, monster);

   act("You appear as $1n.", A_ALWAYS, monster, 0, 0, TO_CHAR);

   act("$1n appears in tiny puff of smoke.",
       A_ALWAYS, monster, 0, 0, TO_ROOM);
#endif
}


void do_pray(struct unit_data *ch, char *arg,
	     const struct command_info *cmd)
{
   struct unit_data *target;


   if (str_is_empty(arg))
   {
      act("You feel righteous", A_ALWAYS, ch, 0, 0, TO_CHAR);
      if (CHAR_AWAKE(ch))
	act("$1n prays to all the gods.", A_HIDEINV, ch, 0, 0, TO_ROOM);
   }

   target = find_unit(ch, &arg, 0, FIND_UNIT_SURRO | FIND_UNIT_WORLD);

   if (target == NULL || !IS_CHAR(target))
   {
      send_to_char("No one by that name here.\n\r", ch);
      return;
   }

   if (ch == target)
   {
      act("You feel pretty selfish.", A_ALWAYS, ch, 0, 0, TO_CHAR);
      act("$1n prays to $1mself.", A_HIDEINV, ch, 0, 0, TO_ROOM);
      return;
   }

   act("You pray to $3n.", A_ALWAYS, ch, 0, target, TO_CHAR);
   act("$1n prays to $3n.", A_HIDEINV, ch, 0, target, TO_ROOM);

#ifdef DEMIGOD

   if (IS_DEMIGOD(target))
   {
      if (!CHAR_DESCRIPTOR(target))
      {
	 act("$3e can't hear your prayers.",
	     A_ALWAYS, ch, 0, target, TO_CHAR);
	 return;
      }

      arg = skip_spaces(arg);

      if (str_is_empty(arg))
	act("$1n prays to you.",
	    A_ALWAYS, ch, arg, target, TO_VICT);
      else
	act("$1n prays to you, '$2t'.",
	    A_ALWAYS, ch, arg, target, TO_VICT);
   }
#endif
}


int sacrifice_unit_power(struct unit_data *item, int demigod)
{
   if (!IS_OBJ(item))
     return 0;

   if (demigod)
   {
      if (affected_by_spell(item, ID_CORPSE))
	return OBJ_VALUE(item, 3) * 1000;
 
      switch (OBJ_TYPE(item))
      {
	case ITEM_TRASH:
	 return 10;
	case ITEM_MONEY:
	 return MONEY_VALUE(item) / 100;
	default:
	 return MIN(20000, OBJ_PRICE(item) / 10);
      }
   }
   else
   {
      if (affected_by_spell(item, ID_CORPSE))
	return 10;
 
      switch (OBJ_TYPE(item))
      {
	case ITEM_TRASH:
	 return 10;
	case ITEM_MONEY:
	 return MONEY_VALUE(item) / 8;
	default:
	 return MIN(20000, OBJ_PRICE(item) / 10);
      }
   }
}


int sacrifice_unit(struct unit_data *u, int pow, int demigod)
{
   if (UNIT_CONTAINS(u))
     pow += sacrifice_unit(UNIT_CONTAINS(u), pow, demigod);

   if (u->next)
     pow += sacrifice_unit(u->next, pow, demigod);

   return pow + sacrifice_unit_power(u, demigod);
}


static bool contains_character(struct unit_data *u)
{
  for (u = UNIT_CONTAINS(u); u; u = u->next)
    if (IS_CHAR(u) || (UNIT_CONTAINS(u) && contains_character(u)))
      return TRUE;

  return FALSE;
}


void base_sacrifice(struct unit_data *ch, char *arg, int noble)
{
#ifndef DEMIGOD
   send_to_char("This command has been removed.\r\n", ch);
   return;
#else
   struct unit_data *u, *god;
   char buf[MAX_INPUT_LENGTH];
   int loaded = FALSE;
   int power;

   struct unit_data *find_player(char *name);

   if (IS_NPC(ch))
   {
      send_to_char("You are a monster, you can't sacrifice!\n\r", ch);
      return;
   }

   if (str_is_empty(arg))
   {
      if (IS_IMMORTAL(ch) || IS_NOBLE(ch))
      {
	 struct extra_descr_data *exd;
      
	 for (exd = PC_QUEST(ch); exd; exd = exd->next)
	 {
	    if (exd->names[0] && *exd->names[0] == '$' &&
		exd->names[0][1] == 'S')
	    {
	       sprintf(buf, "%-20s %s\n\r",
		       &exd->names[0][2], exd->descr);
	       send_to_char(buf, ch);
	    }
	    
	 }
      }
      else
      {
	 send_to_char("What do you want to sacrifice?\n\r", ch);
      }
      return;
   }

   if ((u = find_unit(ch, &arg, 0, FIND_UNIT_INVEN)) == NULL)
   {
      send_to_char("You are carrying no such item.\n\r", ch);
      return;
   }

   if (IS_CHAR(u) || contains_character(u))
   {
     send_to_char("You're not allowed to sacrifice characters this way!\n\r",
		  ch);
     return;
   }

   arg = one_argument(arg, buf);

   if (str_is_empty(buf))
   {
      send_to_char("Who do you want to sacrifice to?\n\r", ch);
      return;
   }

   god = find_player(buf);

   /* Ok, this is a bug: If the player is in menu, then he'll be loaded
      and saved - if he then enters the game his current menu copy will
      be loaded! Nothing much we can do about this, except a lot of code
      which I'm not going to make now anyway... MS */

   if (god == NULL)
   {
      god = load_player(buf);
      if (god == NULL)
      {
	 send_to_char("There is no such person to sacrifice to!\n\r", ch);
	 return;
      }
      loaded = TRUE;
   }

   if (!(IS_DEMIGOD(CHAR_ORIGINAL(god)) ||
	 (noble && IS_NOBLE(CHAR_ORIGINAL(god)))))
   {
      act("You realize that $3n isn't a demigod.",
	  A_ALWAYS, ch, u, CHAR_ORIGINAL(god), TO_CHAR);
      return;
   }

   if (CHAR_ORIGINAL(god) == CHAR_ORIGINAL(ch))
   {
     send_to_char("Sacrificing to yourself??  "
		  "The word `narcissistic' springs to mind...\n\r", ch);
     return;
   }

   act("$1n sacrifices $2n to $3n.", A_ALWAYS,
       ch, u, CHAR_ORIGINAL(god), TO_NOTVICT);

   act("You sacrifice $2n to $3n.",
       A_ALWAYS, ch, u, CHAR_ORIGINAL(god), TO_CHAR);

   power = sacrifice_unit_power(u, IS_DEMIGOD(god));
   if (UNIT_CONTAINS(u))
     power += sacrifice_unit(UNIT_CONTAINS(u), 0,
			     IS_DEMIGOD(CHAR_ORIGINAL(god)));

   act("You sacrificed $2d power points.",
       A_ALWAYS, ch, &power, 0, TO_CHAR);

   add_sacrifice_info(CHAR_ORIGINAL(god), ch, power);
   CHAR_EXP(CHAR_ORIGINAL(god)) += power;

   act("$1n sacrifices $2d sacrificial points to you.",
       A_ALWAYS, ch, &power, god, TO_VICT);

   if (loaded)
     save_player_file(god);

   extract_unit(u);

   if (loaded)
     extract_unit(god);
#endif
}

void do_sacrifice(struct unit_data *ch, char *arg,
		  const struct command_info *cmd)
{
#ifndef DEMIGOD
   send_to_char("This command has been removed.\r\n", ch);
   return;
#endif

   base_sacrifice(ch, arg, FALSE);
}

int sacrifice(struct spec_arg *sarg)
{
   if (sarg->cmd->no == CMD_SACRIFICE)
   {
#ifndef DEMIGOD
   send_to_char("This command has been removed.\r\n", sarg->activator);
   return SFR_BLOCK;
#endif

      base_sacrifice(sarg->activator, (char *) sarg->arg, TRUE);
      return SFR_BLOCK;
   }
   return SFR_SHARE;
}


void banish_demigod(struct unit_data *ch)
{
#ifdef DEMIGOD
   act("A horrible feeling comes upon you, like a giant hand, darkness"
       "comes down from above, grabbing your body, which begin to ache "
       "with striking pain from inside. Your head seems to be filled with "
       "deamons from another plane as your body dissolves into the "
       "elements of time and space itself. You loose your rank as immortal.",
       A_ALWAYS, ch, 0, 0, TO_CHAR);

   CHAR_LEVEL(ch) = 150;
   CHAR_EXP(ch) = required_xp(150);

   UNIT_TITLE(ch)->Reassign("the Fallen Demigod");

   if (PC_HOME(ch))
     free(PC_HOME(ch));
   PC_HOME(ch) = str_dup("udgaard/temple");

   unit_from_unit(ch);
   unit_to_unit(ch, hometown_unit(PC_HOME(ch)));
   act("$1n arrives powerless and naked in a mist of black smoke.",
       A_SOMEONE, ch, 0, 0, TO_ROOM);

   while (UNIT_CONTAINS(ch))
     extract_unit(UNIT_CONTAINS(ch));

   UNIT_HIT(ch) = 1;
   CHAR_ENDURANCE(ch) = 0;
#endif
}

void make_demigod(struct unit_data *ch)
{
#ifdef DEMIGOD
   int i;

   assert(IS_MORTAL(ch));

   act("$1n glows with a divine aura and disappears.",
       A_ALWAYS, ch, 0, 0, TO_ROOM);
   act("You are filled with immense power and raise to the ranks of "
       "immortality. You are teleported to your new world.",
       A_ALWAYS, ch, 0, 0, TO_CHAR);
   CHAR_LEVEL(ch) = DEMIGOD_LEVEL;
   CHAR_EXP(ch) = 0;
   if (PC_HOME(ch))
     free(PC_HOME(ch));
   PC_HOME(ch) = str_dup("demigod/temple");
   unit_from_unit(ch);
   unit_to_unit(ch, hometown_unit(PC_HOME(ch)));

   UNIT_TITLE(ch)->Reassign("the Demigod");

   for (i = 0; i < 3; i++)
     PC_COND(ch,i) = 48;   /* Permanent */
   PC_COND(ch, DRUNK) = 0; /* Sober     */
#endif
}



int demi_stuff(struct spec_arg *sarg)
{
#ifdef DEMIGOD
   int i, nExp, nMember;
   struct char_follow_type *f;
   struct unit_data *pVict, *paper;
   char Buf[MAX_STRING_LENGTH];
   int loaded = FALSE;
   struct extra_descr_data *exd;

   if (cmd->no == CMD_LEVEL)
   {
      if (!IS_PC(ch))
      {
	 send_to_char("Buzz off and leave me alone!\n\r", ch);
	 return SFR_BLOCK;
      }

      if (!IS_DEMIGOD(ch))
      {
	 send_to_char("You're not a demigod.\n\r", ch);
	 return SFR_BLOCK;
      }

      if (CHAR_LEVEL(ch) + 1 >= GOD_LEVEL)
      {
	 send_to_char("You have reached the maximum level.\n\r", ch);
	 return SFR_BLOCK;
      }

      i = (DEMIGOD_LEVEL_XP + 10000000*(1+CHAR_LEVEL(ch)-DEMIGOD_LEVEL))
	- CHAR_EXP(ch);

      if (str_ccmp_next_word(arg, "god") == NULL)
      {
	 if (i > 0)
	   act("You need $2d additional power points to raise a level.",
	       A_ALWAYS, ch, &i, 0, TO_CHAR);

	 if (CHAR_EXP(ch) >= DEMIGOD_LEVEL_XP)
	   send_to_char("You must write 'level god' "
			"if you wish to level.\n\r", ch);
	 return SFR_BLOCK;
      }
      
      if (i > 0)
      {
	 act("You need $2d power points to raise a level.",
	     A_ALWAYS, ch, &i, 0, TO_CHAR);
	 return SFR_BLOCK;
      }

      act("You raise a level", A_ALWAYS, ch, 0, 0, TO_CHAR);
      CHAR_LEVEL(ch)++;
      CHAR_EXP(ch) -= DEMIGOD_LEVEL_XP;
      return SFR_BLOCK;
   }
   else if (is_command(cmd, "banish") || is_command(cmd, "accept"))
   {
      if (!IS_PC(ch))
      {
	 send_to_char("Buzz off and leave me alone!\n\r", ch);
	 return SFR_BLOCK;
      }

      if (!IS_DEMIGOD(ch))
      {
	 send_to_char("You're not a demigod.\n\r", ch);
	 return SFR_BLOCK;
      }

      if (CHAR_MASTER(ch))
      {
	 send_to_char("Only the leader in your group can issue "
		      "the orders.\n\r", ch);
	 return SFR_BLOCK;
      }

      nExp = CHAR_EXP(ch);
      nMember = 1;

      for (f=CHAR_FOLLOWERS(ch); f; f=f->next)
	if (IS_DEMIGOD(f->follower))
	{
	   nExp += CHAR_EXP(f->follower);
	   nMember++;
	}

      if (nMember < 3)
      {
	 send_to_char("You must be a group of at least 3 demigods.\n\r", ch);
	 return SFR_BLOCK;
      }

      one_argument(arg, Buf);

      if (is_command(cmd, "accept"))
      {
	 pVict = find_unit(ch, &arg, 0, FIND_UNIT_WORLD);
	 
	 if (pVict == NULL || !IS_PC(pVict))
	 {
	    send_to_char("No such mortal to grant eternal life.\n\r", ch);
	    return SFR_BLOCK;
	 }

	 if (!IS_MORTAL(pVict))
	 {
	    act("$2e is not a mortal.", A_SOMEONE, ch, pVict, 0, TO_CHAR);
	    return SFR_BLOCK;
	 }

	 act("The group of gods leaded by $1n chants '$2n' and makes "
	     "$2m immortal!",
	     A_SOMEONE, ch, pVict, 0, TO_ROOM);
	 act("You chant the name of '$2n' and grant $2m eternal life.",
	     A_SOMEONE, ch, pVict, 0, TO_CHAR);
	 make_demigod(pVict);
      }
      else /* Banish (even quitted players!) */
      {
	 pVict = find_unit(ch, &arg, 0, FIND_UNIT_WORLD);
	 
	 if (pVict && !IS_PC(pVict))
	 {
	    send_to_char("That is not a player.\n\r", ch);
	    return SFR_BLOCK;
	 }

	 if (pVict == NULL)
	 {
	    pVict = load_player(Buf);
	    if (pVict == NULL)
	    {
	       send_to_char("No such player in game nor file.\n\r", ch);
	       return SFR_BLOCK;
	    }
	    loaded = TRUE;
	 }

	 if (!IS_DEMIGOD(pVict))
	 {
	    act("$2e is not a demigod.", A_SOMEONE, ch, pVict, 0, TO_CHAR);
	    if (loaded)
	      extract_unit(pVict);
	    return SFR_BLOCK;
	 }

	 if (loaded)
	 {
	    load_contents(Buf, pVict);
	    unit_to_unit(pVict, UNIT_IN(ch));
	    insert_in_unit_list(pVict);
	 }

	 if (UNIT_IN(pVict) != UNIT_IN(ch))
	 {
	    act("$1n is taken away to be punished by the gods.",
		A_HIDEINV, pVict, 0, 0, TO_ROOM);
	    act("You are transferred home to be punished!",
		A_ALWAYS, pVict, 0, 0, TO_CHAR);
	    unit_from_unit(pVict);
	    unit_to_unit(pVict, UNIT_IN(ch));
	    act("$1n arrives in a mist of cold darkness.",
		A_SOMEONE, pVict, 0, 0, TO_ROOM);
	 }

	 act("The group of gods leaded by $1n attempts to banish $3n.",
	     A_HIDEINV, ch, 0, pVict, TO_NOTVICT);
	 act("You attempt to banish $3n.",
	     A_HIDEINV, ch, 0, pVict, TO_CHAR);
	 act("The group of gods leaded by $1n attempts to banish you!",
	     A_HIDEINV, ch, 0, pVict, TO_VICT);

	 i = CHAR_EXP(pVict) * 3 + 1000000;
	 if (i > nExp)
	 {
	    act("$1n resists as $1e is too powerful!",
		A_SOMEONE, pVict, 0, 0, TO_ROOM);
	    act("You resist the banishment due to your high power.",
		A_SOMEONE, pVict, 0, 0, TO_CHAR);
	    paper = read_unit(find_file_index("demigod", "banned_fail"));
	    if (paper)
	    {
	       unit_to_unit(paper, pVict);
	       exd = unit_find_extra(UNIT_NAME(paper), paper);
	       if (exd)
	       {
		  sprintf(Buf, exd->descr->String(),
			  UNIT_NAME(pVict), UNIT_NAME(ch));
		  exd->descr->Reassign(Buf);
	       }
	    }

	    if (loaded)
	      extract_unit(pVict); /* Player saved since in unit... */
	    return SFR_BLOCK;
	 }

	 act("$1n is banished!", A_SOMEONE, pVict, 0, 0, TO_ROOM);

	 i /= nMember;

	 CHAR_EXP(ch) = MAX(0, CHAR_EXP(ch) - i);

	 for (f=CHAR_FOLLOWERS(ch); f; f=f->next)
	   if (IS_DEMIGOD(f->follower))
	     CHAR_EXP(f->follower) = MAX(0, CHAR_EXP(f->follower) - i);

	 banish_demigod(pVict);

	 paper = read_unit(find_file_index("demigod", "banned_ltr"));
	 if (paper)
	 {
	    unit_to_unit(paper, pVict);
	    exd = unit_find_extra(UNIT_NAME(paper), paper);
	    if (exd)
	    {
	       sprintf(Buf, exd->descr->String(),
		       UNIT_NAME(pVict), UNIT_NAME(ch));
	       exd->descr->Reassign(Buf);
	    }
	 }

	 if (loaded)
	   extract_unit(pVict); /* Player saved since in unit... */
      }

      return SFR_BLOCK;
   }
#endif

   return SFR_SHARE;
}
