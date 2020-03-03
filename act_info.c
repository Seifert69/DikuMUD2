/* *********************************************************************** *
 * File   : act_info.c                                Part of Valhalla MUD *
 * Version: 1.15                                                           *
 * Author : All.                                                           *
 *                                                                         *
 * Purpose: Look/score etc.                                                *
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

/* 09/07/92 seifert: Fixed look in <room>                                  */
/* 09/07/92 seifert: Fixed invis in looks at himself                       */
/* 23/08/93 jubal  : Fixed transparency in listing objects outside         */
/* 23/08/93 jubal  : Made negative drinkcon values mean infinite contents  */
/* 23/08/93 jubal  : Fixed info                                            */
/* 23/08/93 jubal  : Added 'right beside you' in where                     */
/* 23/08/93 jubal  : Allow imms to see hidden thingys                      */
/* 10/02/94 gnort  : Rewrote do_who to be smaller and dynamic.             */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "handler.h"
#include "interpreter.h"
#include "files.h"
#include "skills.h"
#include "db.h"
#include "spells.h"
#include "limits.h"
#include "affect.h"
#include "utility.h"
#include "trie.h"
#include "textutil.h"
#include "blkfile.h"
#include "money.h"
#include "movement.h"
#include "protocol.h"
#include "constants.h"
#include "common.h"
#include "guild.h"

/* extern variables */

extern struct descriptor_data *descriptor_list;
extern struct unit_data *unit_list;

static void add_to_string(char **buf, int *size, int len, const char *str)
{
  if (*buf == NULL)
  {
    CREATE(*buf, char, *size);
    **buf = '\0';
  }
  else if (*size < len + 1)
  {
    *size *= 2;
    RECREATE(*buf, char, *size);
  }

  strcat(*buf, str);
}

/* also used in "corpses" wizard-command */
char *in_string(struct unit_data *ch, struct unit_data *u)
{
  static char in_str[512];
  char *tmp = in_str;

  while ((u = UNIT_IN(u)))
    if (IS_ROOM(u))
    {
      sprintf(tmp, "%s@%s",
	      UNIT_FI_NAME(u),
	      UNIT_FI_ZONENAME(u));
      return in_str;
    }
    else
    {
      sprintf(tmp, "%s/", UNIT_SEE_NAME(ch, u));
      TAIL(tmp);
    }

  error(HERE, "Something that is UNIT_IN, not in a room!");
  return NULL;
}



void do_exits(struct unit_data *ch, char *arg,
	      const struct command_info *cmd)
{
  int door;
  char buf[MAX_STRING_LENGTH], *b;
  struct unit_data *room;

  const char *exits[] =
  {
    "North", 
    "East ", 
    "South", 
    "West ", 
    "Up   ", 
    "Down "
  };
  
  int has_found_door(struct unit_data *pc, int dir);
  
  send_to_char("Obvious exits:\n\r", ch);
  
  *buf = '\0';
  b = buf;
  
  room = UNIT_IN(ch);
  if (!IS_ROOM(room) && UNIT_IS_TRANSPARENT(room))
    room = UNIT_IN(room);
  
  if (room == NULL || !IS_ROOM(room))
  {
    send_to_char("None.\n\r", ch);
    return;
  }

  for (door = 0; door <= 5; door++)
    if (ROOM_EXIT(room, door) && ROOM_EXIT(room, door)->to_room)
    {
      if (!IS_SET(ROOM_EXIT(room, door)->exit_info, EX_CLOSED))
      {
	if (UNIT_IS_DARK(ROOM_EXIT(room, door)->to_room))
	  sprintf(b, "%s - Too dark to tell\n\r", exits[door]);
	else
	  sprintf(b, "%s - %s\n\r", exits[door], 
		  UNIT_TITLE_STRING(ROOM_EXIT(room, door)->to_room));
	TAIL(b);
      }
      else if (has_found_door(ch, door))
      {
	sprintf(b, "%s - Closed %s.\n\r", 
		exits[door], STR(ROOM_EXIT(room, door)->open_name.Name()));
	TAIL(b);
      }
    }
  if (*buf)
    send_to_char(buf, ch);
  else
    send_to_char("None.\n\r", ch);
}


void do_purse(struct unit_data *ch, char *arg,
	      const struct command_info *cmd)
{
  struct unit_data *thing, *purse[MAX_MONEY + 1];
  ubit1 found = FALSE;
  int i;

  for (i = 0; i <= MAX_MONEY; ++i)
    purse[i] = NULL;

  send_to_char("Your purse contains:\n\r", ch);

  for (thing = UNIT_CONTAINS(ch); thing; thing = thing->next)
    if (CHAR_CAN_SEE(ch, thing) && IS_MONEY(thing))
      purse[MONEY_TYPE(thing)] = thing;

  for (i = MAX_MONEY; 0 <= i; --i)
    if (purse[i])
    {
      found = TRUE;
      act("  $2t", A_ALWAYS, ch, obj_money_string(purse[i], 0), 0, TO_CHAR);
    }

  if (!found)
    send_to_char("  Nothing.\n\r", ch);
}


void do_quests(struct unit_data *ch,char *arg,
	       const struct command_info *cmd)
{
   int bOutput = FALSE;
   char buf[256];
   struct extra_descr_data *exd;

   if (!IS_PC(ch))
   {
      send_to_char("You might be part of a quest?\n\r", ch);
      return;
   }

   for (exd = PC_QUEST(ch); exd; exd = exd->next)
   {
      if (!exd->names.Name())
	continue;

      if (*(exd->names.Name()) == '$')
	continue;

      sprintf(buf, "%s\n\r", exd->names.Name());
      send_to_char(buf, ch);
      bOutput = TRUE;
   }

   if (!bOutput)
   {
      send_to_char("You have not completed any quests.\n\r", ch);
      return;
   }
}

static void status_spells(struct unit_data *ch, ubit8 realm)
{
   int i, j, p;
   char buf[83 * SPL_TREE_MAX+512], tmpbuf[80], *b;

   b = buf;

   *b = 0;

   for (j = i = 0; i < SPL_TREE_MAX; i++)
   {
      if ((p = PC_SPL_SKILL(ch, i)) && TREE_ISLEAF(spl_tree, i) &&
	  spell_info[i].realm == realm)
      {
	 sprintf(tmpbuf, "You %s %s", 
		 skill_text(spell_skills, p), spl_text[i]);
	 sprintf(b, "%-39s", tmpbuf);
	 TAIL(b);
	 if ((++j % 2) == 0)
	 {
	    strcat(b, "\n\r");
	    TAIL(b);
	 }
      }
   }
   
   if (*buf)
   {
      if ((j % 2) != 0)
      {
	 strcat(b, "\n\r");
	 TAIL(b);
      }

      if (realm == ABIL_MAG)
	page_string(CHAR_DESCRIPTOR(ch),
		    "Spells known through your essence magic mastery:\n\r");
      else
	page_string(CHAR_DESCRIPTOR(ch),
	    "\n\rSpells known through your divine channeling powers:\n\r");

      page_string(CHAR_DESCRIPTOR(ch), buf);
   }
   else
   {
      if (realm == ABIL_MAG)
	send_to_char("You can not harness spells using essence magic "
		     "mastery:\n\r", ch);
      else
	send_to_char("\n\rYou can not harness spells using divine channeling "
		"powers:\n\r", ch);
   }
}

void do_status(struct unit_data *ch, char *arg,
	       const struct command_info *cmd)
{
   int idx, i, p, j;
   struct time_info_data playing_time, years;
   char buf[2 * MAX_STRING_LENGTH], tmpbuf[80], *b;
  
   static const char *infos[] =
   {
      "weapons", 
      "spells", 
      "skills",
      NULL
   };

   extern const char *char_sex[];
   extern const char *pc_races[];
  
   struct time_info_data age(struct unit_data *ch);
   struct time_info_data real_time_passed(time_t t2, time_t t1);

   if (!IS_PC(ch))
     return;

   *buf = '\0';
   b = buf;

   if (!str_is_empty(arg))
   {
      arg = one_argument(arg, buf);
      idx = search_block(buf, infos, FALSE);
      *buf = '\0';
    
      switch (idx)
      {
	case 0: /* weapon */
	 for (i = j = 0; i < WPN_TREE_MAX; i++)
	   if ((p = PC_WPN_SKILL(ch, i)) && TREE_ISLEAF(wpn_tree, i))
	   {
	      sprintf(tmpbuf, "You are %s with a %s", 
		      skill_text(weapon_skills, p), wpn_text[i]);
	      sprintf(b, "%-39s", tmpbuf);
	      TAIL(b);
	      if ((++j % 2) == 0)
	      {
		 strcat(b, "\n\r");
		 TAIL(b);
	      }
	   }
	 if (*buf)
	   page_string(CHAR_DESCRIPTOR(ch), buf);
	 else
	   send_to_char("You're no good with weapons.\n\r", ch);
	 break;
      
	case 1: /* spell */
	 status_spells(ch, ABIL_MAG);
	 status_spells(ch, ABIL_DIV);
	 break;
      
	case 2: /* skill */
	 for (j = i =0; i < SKI_TREE_MAX; i++)
	   if ((p = PC_SKI_SKILL(ch, i)) && TREE_ISLEAF(ski_tree, i))
	   {
	      sprintf(tmpbuf, "You %s %s", 
		      skill_text(skill_skills, p), ski_text[i]);
	      sprintf(b, "%-39s", tmpbuf);
	      TAIL(b);
	      if ((++j % 2) == 0)
	      {
		 strcat(b, "\n\r");
		 TAIL(b);
	      }
	   }
	 if (*buf)
	   page_string(CHAR_DESCRIPTOR(ch), buf);
	 else
	   send_to_char("You're no good with skills.\n\r", ch);
	 break;
      
	default: /* wrong option */
	 send_to_char("Usage: status [skills|spells|weapons]\n\r", ch);
      }
      return;
   }
  
   /* Normal Condition */
  
   sprintf(b, "%s%s%s (%s alignment (%d))\n\r", 
	   UNIT_NAME(ch), *UNIT_TITLE_STRING(ch) == ',' ?  "" : " ",
	   UNIT_TITLE_STRING(ch),
	   UNIT_IS_GOOD(ch) ? "Good" : (UNIT_IS_EVIL(ch) ? "Evil" : "Neutral"),
	   UNIT_ALIGNMENT(ch));
   TAIL(b);

   years = age(ch);
   sprintf(b, "You are a %d year old %s %s at level %d. %s\n\r",
	   years.year, 
	   char_sex[CHAR_SEX(ch)], pc_races[CHAR_RACE(ch)],
	   CHAR_LEVEL(ch),
	   years.month == 0 && years.day == 0 ?
	   "It is your birthday today!":"");

   TAIL(b);

   if (PC_GUILD(ch))
   {
      sprintf(b, "You are guild level %d in the %s guild.\n\r",
	      char_guild_level(ch),
	      PC_GUILD(ch));
      TAIL(b);
   }
  
   playing_time = real_time_passed(PC_TIME(ch).played, 0);
  
   sprintf(b, "You have been playing for %d days and %d hours.\n\r", 
	   playing_time.day, 
	   playing_time.hours);
   TAIL(b);

   sprintf(b, "Str %3d, Dex %3d, Con %3d, Cha %3d, "
	   "Bra %3d, Mag %3d, Div %3d\n\r", 
	   CHAR_STR(ch), CHAR_DEX(ch), CHAR_CON(ch), CHAR_CHA(ch), 
	   CHAR_BRA(ch), CHAR_MAG(ch), CHAR_DIV(ch));
   TAIL(b);

   sprintf(b, "You have %ld ability, and %ld skill practice points.\n\r", 
	   (signed long) PC_ABILITY_POINTS(ch),
	   (signed long) PC_SKILL_POINTS(ch));
   TAIL(b);

   if (IS_SET(CHAR_FLAGS(ch), CHAR_WIMPY))
     sprintf(b, "You are in wimpy mode.\n\r");
   else
     sprintf(b, "You are in brave mode.\n\r");
   TAIL(b);
   
   if (CHAR_LEVEL(ch) == START_LEVEL)
   {
      sprintf(b, "\n\rTry also: status [skills|spells|weapons].\n\r");
      TAIL(b);
   }
  
   send_to_char(buf, ch);
}


char *own_position(struct unit_data *ch)
{
  static char buf[256];

  switch (CHAR_POS(ch))
  {
   case POSITION_DEAD:
    sprintf(buf, "Sorry old chap, it seems like you're Dead!\n\r");
    break;
    
   case POSITION_MORTALLYW:
    sprintf(buf, "You are mortally wounded, you should seek help!\n\r");
    break;
    
   case POSITION_INCAP:
    sprintf(buf, "You are incapacitated, slowly fading away.\n\r");
    break;
    
   case POSITION_STUNNED:
    sprintf(buf, "You are stunned and unable to move.\n\r");
    break;
    
   case POSITION_SLEEPING:
    sprintf(buf, "You are sleeping.\n\r");
    break;
    
   case POSITION_RESTING:
    sprintf(buf, "You are resting.\n\r");
    break;
    
   case POSITION_SITTING:
    sprintf(buf, "You are sitting.\n\r");
    break;
    
   case POSITION_FIGHTING:
    if (CHAR_FIGHTING(ch))
      sprintf(buf, "You are fighting %s.\n\r",
	      UNIT_TITLE_STRING(CHAR_FIGHTING(ch)));
    else if (CHAR_COMBAT(ch))
      sprintf(buf, "You are fighting %s at a distance.\n\r",
	      UNIT_TITLE_STRING(CHAR_COMBAT(ch)->Opponent()));
    else
      sprintf(buf, "You are fighting thin air.\n\r");
    break;

   case POSITION_STANDING:
    sprintf(buf, "You are standing.\n\r");
    break;

   default:
    sprintf(buf, "You are floating.\n\r");
    break;
  }
  
  return buf;
}


void do_score(struct unit_data *ch, char *arg,
	      const struct command_info *cmd)
{
   static char buf[MAX_STRING_LENGTH], *b;
   struct unit_data *vict;
   struct char_follow_type *f;
   int members = FALSE;

   struct time_info_data age(const struct unit_data *ch);
   struct time_info_data real_time_passed(time_t t2, time_t t1);
  
   if (!IS_PC(ch))
     return;
  
   b = buf;
  
   if (!strncmp(arg, "group", 5))
   {
      if (!IS_SET(CHAR_FLAGS(ch), CHAR_GROUP))
      {
	 send_to_char("What group?", ch);
	 return;
      }
    
      /* who's head of group? */
      vict = CHAR_MASTER(ch) ? CHAR_MASTER(ch) : ch;
    
      if (vict != ch && IS_SET(CHAR_FLAGS(vict), CHAR_GROUP))
      {
	 sprintf(b, "%s %ld(%ld) hit, %d(%d) mana and %d(%d) endurance.\n\r", 
		 UNIT_NAME(vict),
		 (signed long) UNIT_HIT(vict),
		 (signed long) UNIT_MAX_HIT(vict),
		 CHAR_MANA(vict), mana_limit(vict), 
		 CHAR_ENDURANCE(vict), move_limit(vict));
	 TAIL(b);
	 members = TRUE;
      }

      for (f = CHAR_FOLLOWERS(vict); f; f = f->next)
	if (ch != f->follower && IS_SET(CHAR_FLAGS(f->follower), CHAR_GROUP))
	{
	   sprintf(b, "%s %ld(%ld) hit, %d(%d) mana and %d(%d) endurance.\n\r",
 		   UNIT_NAME(f->follower),
		   (signed long) UNIT_HIT(f->follower),
		   (signed long) UNIT_MAX_HIT(f->follower),
		   CHAR_MANA(f->follower), mana_limit(f->follower),
		   CHAR_ENDURANCE(f->follower), move_limit(f->follower));
	   TAIL(b);
	   members = TRUE;
	}

      if (members)
	send_to_char(buf, ch);
      else
	send_to_char("There are none of your group members here?\n\r", ch);

      return;
   }
  
   sprintf(b, "%s%s%s (%s)\n\r", UNIT_NAME(ch), 
	   *UNIT_TITLE_STRING(ch) == ',' ?  "" : " ",
	   UNIT_TITLE_STRING(ch),
	   UNIT_IS_GOOD(ch) ? "Good" : (UNIT_IS_EVIL(ch) ?
					"Evil" : "Neutral"));
   TAIL(b);

   strcpy(b, "You are");
   if (PC_COND(ch, DRUNK) > 10)
     strcat(b, " intoxicated, ");
   if (PC_COND(ch, THIRST) < 4)
     strcat(b, " thirsty, ");
   if (PC_COND(ch, FULL) < 4)
     strcat(b, " hungry, ");
  
   if (b[7]) /* If any status were concatenated */
   {
      b[strlen(b)-1] = '.';
      strcat(b, "\n\r");
      TAIL(b);
   } 
   else
     *b = '\0';
  
   if (IS_MORTAL(ch))
   {
#ifdef NOBLE
      if (IS_NOBLE(ch))
      {
	 sprintf(b, "You have %ld noble points.\n\r",
		 (signed long) CHAR_EXP(ch));
	 TAIL(b);
      }
      else
#endif
      {
	 sprintf(b,"You have %ld experience and need %ld for next level.\n\r", 
		 (signed long) CHAR_EXP(ch), 
		 (signed long) required_xp(PC_VIRTUAL_LEVEL(ch)+1)
		               - CHAR_EXP(ch));
	 TAIL(b);
      }
   }
#ifdef DEMIGOD
   else if (IS_DEMIGOD(ch))
   {
      sprintf(b, "You have %ld divine power points.\n\r",
	      (signed long) CHAR_EXP(ch));
      TAIL(b);
   }
#endif

   sprintf(b, "You have %ld(%ld) hit, %d(%d) mana "
	   "and %d(%d) endurance points.\n\r", 
	   (signed long) UNIT_HIT(ch), (signed long) UNIT_MAX_HIT(ch), 
	   CHAR_MANA(ch), mana_limit(ch), 
	   CHAR_ENDURANCE(ch), move_limit(ch));
   TAIL(b);
   
   strcpy(b, own_position(ch));
   TAIL(b);

   if (IS_SET(CHAR_FLAGS(ch), CHAR_PROTECTED))
   {
      strcpy(b, "You are a protected citizen");
      TAIL(b);
      if (IS_SET(CHAR_FLAGS(ch), CHAR_LEGAL_TARGET))
	strcpy(b, ", but people may kill you now!\n\r");
      else
	strcpy(b, ".\n\r");
      TAIL(b);
   }

   if (IS_SET(CHAR_FLAGS(ch), CHAR_OUTLAW))
   {
      sprintf(b, "You are wanted%s alive by the law.\n\r", 
	      IS_SET(CHAR_FLAGS(ch), CHAR_PROTECTED) ? "" : " dead or");
      TAIL(b);
   }

   if (affected_by_spell(ch, ID_REWARD))
   {
      strcpy(b, "There is a reward on your head.\n\r");
      TAIL(b);
   }

   if (g_cServerConfig.m_bBOB && IS_SET(PC_FLAGS(ch), PC_PK_RELAXED))
     strcpy(b, "You have signed the Book of Blood.\r\n");
   
   if (CHAR_LEVEL(ch) == START_LEVEL)
   {
      strcpy(b, "\n\rTry also the 'status' command.\n\r");
      TAIL(b);
   }
   
   if (PC_IS_UNSAVED(ch))
   {
      strcpy(b, "\n\rYou remain a GUEST until you 'save' yourself\n\r");
      TAIL(b);
   }
   
   send_to_char(buf, ch);
}


void do_time(struct unit_data *ch,char *arg,
	     const struct command_info *cmd)
{
  char *b;
  char buf[200];
  struct time_info_data game_time;
  
  void mudtime_strcpy(struct time_info_data *time, char *str);
  struct time_info_data mud_date(time_t t);

  game_time = mud_date(time(0));

  b = buf;
  strcpy(b, "It is ");
  TAIL(b);

  mudtime_strcpy(&game_time, b);

  send_to_char(buf, ch);
}


void do_credits(struct unit_data *ch, char *arg,
		const struct command_info *cmd)
{
  page_string(CHAR_DESCRIPTOR(ch), g_cServerConfig.m_pCredits);
}


void do_news(struct unit_data *ch, char *arg,
	     const struct command_info *cmd)
{
  page_string(CHAR_DESCRIPTOR(ch), g_cServerConfig.m_pNews);
}


void do_info(struct unit_data *ch, char *arg,
	     const struct command_info *cmd)
{
  send_to_char("You can't do that here (find a teacher, try `status' or"
	       " `help info'\n\r", ch);
}


void do_wizlist(struct unit_data *ch, char *arg,
		const struct command_info *cmd)
{
  page_string(CHAR_DESCRIPTOR(ch), g_cServerConfig.m_pWizlist);
}


void player_where(struct unit_data *ch, char *arg)
{
   char buf[160];
   struct descriptor_data *d;
   int any = FALSE;

   for (d = descriptor_list; d; d = d->next)
   {
      if (d->character && (d->character != ch)
	  && UNIT_IN(d->character)
	  && descriptor_is_playing(d)
	  && (str_is_empty(arg) || !str_ccmp(arg, UNIT_NAME(d->character)))
	  && CHAR_LEVEL(ch) >= UNIT_MINV(d->character)
	  && d->original == NULL && CHAR_CAN_SEE(ch, d->character) &&
	  unit_zone(ch) == unit_zone(d->character))
      {
	 sprintf(buf, "%-30s at %s\n\r", UNIT_NAME(d->character),
		 TITLENAME(unit_room(d->character)));
	 send_to_char(buf, ch);
	 any = TRUE;
      }
   }

   if (!any)
   {
      if (str_is_empty(arg))
	send_to_char("No other visible players in this area.\n\r", ch);
      else
	send_to_char("No such player found in this area.\n\r", ch);
   }
}


void do_where(struct unit_data *ch, char *aaa,
	      const struct command_info *cmd)
{
   char *buf = NULL, buf1[1024], buf2[512];
   register struct unit_data *i;
   struct descriptor_data *d;
   int len, cur_size = 2048;
   char *arg = (char *) aaa;

   if (IS_MORTAL(ch))
   {
      player_where(ch, arg);
      return;
   }

   if (str_is_empty(arg))
   {
      strcpy(buf1, "Players\n\r-------\n\r");
      len = strlen(buf1);
      add_to_string(&buf, &cur_size, len, buf1);

      for (d = descriptor_list; d; d = d->next)
	if (d->character
	    && UNIT_IN(d->character)
	    && descriptor_is_playing(d)
	    && CHAR_LEVEL(ch) >= UNIT_MINV(d->character)
	    && (d->original == NULL
		|| CHAR_LEVEL(ch) >= UNIT_MINV(d->original)))
	{
	   if (d->original) /* If switched */
	     sprintf(buf2, " In body of %s", UNIT_NAME(d->character));
	   else
	     buf2[0] = '\0';
	      
	   sprintf(buf1, "%-20s - %s [%s]%s\n\r",
		   UNIT_NAME(CHAR_ORIGINAL(d->character)),
		   UNIT_SEE_TITLE(ch, UNIT_IN(d->character)),
		   in_string(ch, d->character),
		   buf2);
	   len += strlen(buf1);
	   add_to_string(&buf, &cur_size, len, buf1);
	}
   }
   else  /* Arg was not empty */
   {
      len = 0;
      add_to_string(&buf, &cur_size, len, "");
	 
      for (i = unit_list; i; i = i->gnext)
	if (UNIT_IN(i)
	    && UNIT_NAMES(i).IsName(arg)
	    && CHAR_LEVEL(ch) >= UNIT_MINV(i))
	{
	   sprintf(buf1, "%-30s - %s [%s]\n\r",
		   TITLENAME(i),
		   UNIT_SEE_TITLE(ch, UNIT_IN(i)),
		   in_string(ch, i));
	   
	   len += strlen(buf1);
	   add_to_string(&buf, &cur_size, len, buf1);
	}
   }      

   if (*buf == '\0')
     send_to_char("Couldn't find any such thing.\n\r", ch);
   else
     page_string(CHAR_DESCRIPTOR(ch), buf);
      
   free(buf);
}


void do_who(struct unit_data *ch, char *arg,
	    const struct command_info *cmd)
{
   static char *buf = NULL;
   static int cur_size = 1024;
   
   struct descriptor_data *d;
   char tmp[256], tmp2[512];
   int len;
   
   if (buf)
     *buf = '\0';
   
   strcpy(tmp, g_cServerConfig.m_sColor.pWhoTitle);
   strcat(tmp, "\n\r");
   len = strlen(tmp);
   add_to_string(&buf, &cur_size, len, tmp);
   
   for (d = descriptor_list; d; d = d->next)
     if (descriptor_is_playing(d))
     {
	sprintf(tmp, g_cServerConfig.m_sColor.pWhoElem, 
		UNIT_NAME(CHAR_ORIGINAL(d->character)),
		*UNIT_TITLE_STRING(CHAR_ORIGINAL(d->character))
		== ',' ?  "" : " ",
		UNIT_TITLE_STRING(CHAR_ORIGINAL(d->character)),
		PC_GUILD(CHAR_ORIGINAL(d->character)) ? " (" : "",
		PC_GUILD(CHAR_ORIGINAL(d->character)) ?
		PC_GUILD(CHAR_ORIGINAL(d->character)) : "",
		PC_GUILD(CHAR_ORIGINAL(d->character)) ? ")" : "");

	if (CHAR_LEVEL(ch) < 203)
	{
	   if (IS_PC(d->character) && CHAR_CAN_SEE(ch, d->character))
	     sprintf(tmp2, "%s\n\r", tmp);
	   else
	     tmp2[0] = '\0';
	}
	else if (CHAR_CAN_SEE(ch, CHAR_ORIGINAL(d->character)))
	{
	   if (d->original)	/* If switched */
	   {
	      strcat(tmp, "SWITCHED INTO ");
	      strcat(tmp, UNIT_NAME(d->character));
	   }

	   sprintf(tmp2, "%s %s\n\r",
		   tmp, UNIT_MINV(d->character)
		   ? str_cc("WIZI ", itoa(UNIT_MINV(d->character))) : "");
	}
	else
	  continue;

	len += strlen(tmp2);
	add_to_string(&buf, &cur_size, len, tmp2);
     }
   
   page_string(CHAR_DESCRIPTOR(ch), buf);
   free(buf);
}


void do_commands(struct unit_data *ch, char *arg,
		 const struct command_info *cmd)
{
   char buf[MAX_STRING_LENGTH], *b;
   int no, i;
   extern struct command_info cmd_info[];

   if (!IS_PC(ch))
     return;

   send_to_char("The following commands are available:\n\r\n\r", ch);
  
   *buf = '\0';
   b = buf;

   for (no = 1, i = 0; *cmd_info[i].cmd_str; i++)
     if (CHAR_LEVEL(ch) >= cmd_info[i].minimum_level &&
	 cmd_info[i].minimum_level < IMMORTAL_LEVEL)
     {
	if (IS_MORTAL(ch) &&
	    (cmd_info[i].cmd_fptr == NULL) && (cmd_info[i].tmpl == 0))
	  continue;

	sprintf(b, "%-15s", cmd_info[i].cmd_str);
	if (!(no % 5))
	  strcat(buf, "\n\r");
	no++;
	TAIL(b);
     }

   strcpy(b, "\n\r");

   page_string(CHAR_DESCRIPTOR(ch), buf);
}


void do_areas(struct unit_data *ch, char *arg,
	      const struct command_info *cmd)
{
   char buf[2*MAX_STRING_LENGTH], *b;
   int no;
   struct zone_type *z;

   if (!IS_PC(ch))
     return;

   send_to_char("The following areas are available:\n\r\n\r", ch);
  
   *buf = '\0';
   b = buf;
   no = 0;

   for (z = zone_info.zone_list; z; z = z->next)
   {
      if (!str_is_empty(z->title))
      {
	 sprintf(b, "%-28s %s  ",
		 z->title,
		 z->payonly ? "Donation" : "        ");
	 if (!(no % 2))
	   strcat(b, "\n\r");
	 no++;
	 TAIL(b);
      }
   }

   strcpy(b, "\n\r");

   page_string(CHAR_DESCRIPTOR(ch), buf);
}
