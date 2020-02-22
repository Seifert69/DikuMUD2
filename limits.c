/* *********************************************************************** *
 * File   : limits.c                                  Part of Valhalla MUD *
 * Version: 1.04                                                           *
 * Author : bombman, seifert and quinn (@diku.dk)                          *
 *                                                                         *
 * Purpose: Xp, hit, mana, endurance, food and age gain/modifications.     *
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

/* 23/07/92 seifert: Gain: Mana gain modified, gain fighting=>0 gain       */
/* 01/09/92 seifert: Mana gain now depends on Charisma!                    */
/* 10/04/94 seifert: Mana gain relative to Charisma & level!               */
/* 6/12/94  bombman: Demigods neen no food or drink                        */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "structs.h"
#include "skills.h"
#include "common.h"
#include "limits.h"
#include "utils.h"
#include "spells.h"
#include "comm.h"
#include "utility.h"
#include "textutil.h"
#include "handler.h"
#include "interpreter.h"
#include "guild.h"
#include "fight.h"
#include "constants.h"

extern struct unit_data *unit_list;

/* External procedures */


int required_xp(int level);                                  /* common.c   */
void update_pos( struct unit_data *victim );                 /* in fight.c */
struct time_info_data age(struct unit_data *ch);


/* Count the number of items a unit contains */
int char_carry_n(struct unit_data *unit)
{
   register int i;

   for (unit = UNIT_CONTAINS(unit), i = 0; unit; unit = unit->next)
     if (!IS_OBJ(unit) || ((OBJ_EQP_POS(unit) == 0) && (OBJ_TYPE(unit)
							!=ITEM_MONEY)))
       i++;

   return i;
}

int char_carry_n_limit(struct unit_data *ch)
{
   return 10 + (CHAR_DEX(ch) / 10);
}

int char_can_carry_n(struct unit_data *ch, int n)
{
   return (char_carry_n_limit(ch) >= (char_carry_n(ch) + n));
}

int char_carry_w_limit(struct unit_data *ch)
{
   return 50 + MAX(50, UNIT_BASE_WEIGHT(ch)/2) + CHAR_STR(ch)*2;
}

int char_can_carry_w(struct unit_data *ch, int weight)
{
   return (char_carry_w_limit(ch) >= (UNIT_CONTAINING_W(ch) + weight));
}


int char_can_carry_unit(struct unit_data *ch, struct unit_data *unit)
{
   return (char_can_carry_w(ch, UNIT_WEIGHT(unit)) && char_can_carry_n(ch, 1));
}

int char_can_get_unit(struct unit_data *ch, struct unit_data *unit)
{
   return UNIT_WEAR((unit), MANIPULATE_TAKE) &&
          CHAR_CAN_SEE(ch, unit) && char_can_carry_unit(ch, unit);
}


int char_drag_w_limit(struct unit_data *ch)
{
   return (3 * char_carry_w_limit(ch));
}

int char_can_drag_w(struct unit_data *ch, int weight)
{
   return (char_drag_w_limit(ch) >= weight);
}

/* For lifespan 100 (Human), result is                     */
/* When age in [ 0..16] calculate the line between p0 & p1 */
/* When age in [17..32] calculate the line between p1 & p2 */
/* When age in [33..48] calculate the line between p2 & p3 */
/* When age in [49..64] calculate the line between p3 & p4 */
/* When age in [65..80] calculate the line between p4 & p5 */
/* When age in [81..mx] calculate the line between p5 & p6 */
/* When age > mx return p7                                 */

int age_graph(int age, int lifespan,
	      int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7)
{
   int step = MAX(1, lifespan / 6);

   if (age <= step)
     return (int) (p0+(((age)*(p1-p0))/step));
   else if (age <= 2*step)
     return (int) (p1+(((age-step)*(p2-p1))/step));
   else if (age <= 3*step)
     return (int) (p2+(((age-2*step)*(p3-p2))/step));
   else if (age <= 4*step)
     return (int) (p3+(((age-3*step)*(p4-p3))/step));
   else if (age <= 5*step)
     return (int) (p4+(((age-4*step)*(p5-p4))/step));
   else
   {
      if (age > lifespan)
	return p7;
      else if (age >= lifespan)
	return p6;
      else
	return (int) (p5+(((age-5*step)*(p6-p5))/step));
   }
}


/* This function is copied into basis.zon - remember to update accordingly!! */


static int hit_limit_number(struct unit_data *ch, int point)
{
   if (IS_PC(ch))
   {
      if (IS_IMMORTAL(ch)) //MS2020
	return 3*point + 20;
	
      if (age(ch).year > PC_LIFESPAN(ch))
	return -1;
      
      return 3*point + 20;
   }
   else
     return 3*point + 10;
}


int hit_limit(struct unit_data *ch)
{
   return hit_limit_number(ch, CHAR_HPP(ch));
}


/* Hitpoint gain pr. game hour */
int hit_gain(struct unit_data *ch)
{
   int gain;

   assert(IS_CHAR(ch));

   /* 10 turns to regenerate */
   if (CHAR_POS(ch) != POSITION_FIGHTING)
     gain = 1+hit_limit_number(ch, CHAR_CON(ch))/10;
   else
     gain = 0;

   switch (CHAR_POS(ch))
   {
     case POSITION_SLEEPING:
     case POSITION_RESTING:
      gain += gain/2;  /* gain *= 1.5  */
      break;

     case POSITION_SITTING:
      gain += gain/4;  /* gain *= 1.25 */
      break;
   }

   struct unit_data *u = ch;

   while (u)
   {
      if (IS_SET(UNIT_FLAGS(u), UNIT_FL_SACRED))
      {
	 gain *= 2;
	 break;
      }
      u = UNIT_IN(u);
   }

   if (IS_PC(ch))
   {
      /* gain = graf(age(ch).year, 2,5,10,18,6,4,2); */
      if ((PC_COND(ch,FULL)<0) || (PC_COND(ch,THIRST)<0))
	 gain += 3*MIN(PC_COND(ch,FULL), 3*PC_COND(ch,THIRST));
   }

   return gain;
}



int move_limit(struct unit_data *ch)
{
   int ml = CHAR_CON(ch)*2+150;

   if (IS_PC(ch))
   {
      int pct;

      if (IS_IMMORTAL(ch)) //MS2020
	return ml;
	
      pct = age_graph(age(ch).year, PC_LIFESPAN(ch),
		      80, 130, 110, 90, 70, 50, 20, 0);

      return (pct * ml) / 100; /* actually it is (100 * pct) / 100 */
   }
   else
     return ml;
}


/* move gain pr. game hour */
int move_gain(struct unit_data *ch)
{
   int gain;

   assert(IS_CHAR(ch));

   if (CHAR_POS(ch) != POSITION_FIGHTING)
     gain = 1 + move_limit(ch) / 10;  /* 10 turns to regenerate */
   else
     gain = 0;

   /* Position calculations    */
   switch (CHAR_POS(ch))
   {
       case POSITION_SLEEPING:
       case POSITION_RESTING:
	 gain += gain/2; /* Divide by 4 */
	 break;

       case POSITION_SITTING:
	 gain += gain/4; /* Divide by 8 */
	 break;
   }

   struct unit_data *u = ch;

   while (u)
   {
      if (IS_SET(UNIT_FLAGS(u), UNIT_FL_SACRED))
      {
	 gain *= 2;
	 break;
      }
      u = UNIT_IN(u);
   }

   if (IS_PC(ch))
   {
      /* gain = graf(age(ch).year, ... Age calcs? */
      if ((PC_COND(ch,FULL)<0) || (PC_COND(ch,THIRST)<0))
	gain += 3*MIN(PC_COND(ch,FULL), 3*PC_COND(ch,THIRST));
   }

   return gain;
}


int mana_limit(struct unit_data *ch)
{
   assert(IS_CHAR(ch));

   int ml = MIN(200, 100 + (CHAR_BRA(ch) + CHAR_CHA(ch)) / 2);

   if (IS_PC(ch))
   {
      int pct;

      if (IS_IMMORTAL(ch)) //MS2020
	return ml;
	
      pct = age_graph(age(ch).year, PC_LIFESPAN(ch),
		      0, 100, 105, 110, 120, 130, 140, 0);

      return (ml*pct) / 100;
   }
   else
     return ml;
}


/* manapoint gain pr. game hour */
int mana_gain(struct unit_data *ch)
{
   int gain;

   assert(IS_CHAR(ch));

   /* 10 turns to regenerate is default when CHA == LEVEL */
   if (CHAR_POS(ch) != POSITION_FIGHTING)
   {
      gain = 1 + mana_limit(ch) / 10;
      gain += (CHAR_CHA(ch) - MAX(CHAR_MAG(ch), CHAR_DIV(ch)))/3;
      gain = MAX(1, gain);
   }
   else
     gain = 0;

   switch (CHAR_POS(ch))
   {
     case POSITION_SLEEPING:
     case POSITION_RESTING:
	gain += gain/2;  /* Divide by 2 */
	break;
     case POSITION_SITTING:
	gain += gain/4;  /* Divide by 4 */
	break;
   }

   struct unit_data *u = ch;

   while (u)
   {
      if (IS_SET(UNIT_FLAGS(u), UNIT_FL_SACRED))
      {
	 gain *= 2;
	 break;
      }
      u = UNIT_IN(u);
   }

   if (IS_PC(ch))
   {
      if ((PC_COND(ch,FULL)<0) || (PC_COND(ch,THIRST)<0))
	 gain += 3*MIN(PC_COND(ch,FULL), 3*PC_COND(ch,THIRST));
   }

   return gain;
}

/* Gain maximum in various points */
void advance_level(struct unit_data *ch)
{
   void clear_training_level(struct unit_data *ch);   

   assert(IS_PC(ch));

   if (IS_IMMORTAL(ch))
     return;

#ifdef NOBLE
   if (IS_NOBLE(ch))
     return;
#endif

   send_to_char("You raise a level!\n\r", ch);
   clear_training_level(ch);

   advance_guild_level(ch);
   PC_VIRTUAL_LEVEL(ch)++;
   PC_SKILL_POINTS(ch)   += skill_point_gain();

   if (CHAR_LEVEL(ch) < MORTAL_MAX_LEVEL)
   {
      PC_ABILITY_POINTS(ch) += ability_point_gain();
      CHAR_LEVEL(ch)++;
   }

#ifdef NOBLE
   if (IS_NOBLE(ch))
     send_to_char("You become a noble, seek audience "
		  "with King Welmar.\n\r", ch);

#endif
}


void gain_condition(struct unit_data *ch, int condition, int value)
{
   bool intoxicated;

   if (!IS_PC(ch) || (PC_COND(ch, condition) >= 48))
     return;

   /* No change in sacred rooms */

   intoxicated=(PC_COND(ch, DRUNK) > 0);

   PC_COND(ch, condition) += value;

   PC_COND(ch, condition) = MIN(24,PC_COND(ch, condition));

   if (condition == DRUNK)  /* How can one be less sober than 0? */
     PC_COND(ch, condition) = MAX(0,PC_COND(ch, condition));
   else
     PC_COND(ch, condition) = MAX(-96,PC_COND(ch, condition));

   if (PC_COND(ch,condition) > 3)
     return;

   switch (condition)
   {
     case FULL:
      if (condition > -4)
	send_to_char("You are hungry.\n\r",ch);
      else if (condition > -8)
	send_to_char("You are very hungry.\n\r",ch);
      else if (condition > -12)
	send_to_char("You are starving.\n\r",ch);
      break;

     case THIRST :
       if (condition > -4)
	 send_to_char("You are thirsty.\n\r",ch);
       else if (condition > -8)
	 send_to_char("You are very thirsty.\n\r",ch);
       else if (condition > -12)
	 send_to_char("You are dehydrated.\n\r",ch);
      break;

     case DRUNK :
       if (intoxicated && (PC_COND(ch, DRUNK)==0))
	 send_to_char("You are now sober.\n\r",ch);
      break;

     default : break;
   }
}


/* Update both PC's & NPC's */
void point_update( void )
{
   struct unit_data *u, *next_dude;
   int hgain;

   /* characters */
   for (u = unit_list; u; u = next_dude)
   {
      next_dude = u->gnext;
      if (IS_NPC(u) || (IS_PC(u) && !IS_SET(PC_FLAGS(u), PC_SPIRIT)))
      {
	 if (CHAR_POS(u) >= POSITION_STUNNED)
	 {
	    CHAR_MANA(u) = MIN(CHAR_MANA(u) + mana_gain(u), mana_limit(u));

	    CHAR_ENDURANCE(u) =
	      MIN(CHAR_ENDURANCE(u) + move_gain(u), move_limit(u));

	    hgain = hit_gain(u);
	    if (hgain >= 0)
	      UNIT_HIT(u)  = MIN(UNIT_HIT(u)  + hgain,  hit_limit(u));
	    else
	      damage(u, u, 0, -hgain, MSG_TYPE_OTHER, MSG_OTHER_STARVATION,
		     COM_MSG_EBODY);

	    CHAR_MANA(u) = MAX(0, CHAR_MANA(u));
	    CHAR_ENDURANCE(u) = MAX(0, CHAR_ENDURANCE(u));

	    if (CHAR_POS(u) == POSITION_STUNNED)
	      update_pos( u );
	 }
	 else if  (CHAR_POS(u) == POSITION_INCAP)
	   damage(u, u, 0, 1, MSG_TYPE_OTHER, MSG_OTHER_BLEEDING,
		  COM_MSG_EBODY);
	 else if (IS_PC(u) && (CHAR_POS(u) == POSITION_MORTALLYW))
	   damage(u, u, 0, 2, MSG_TYPE_OTHER, MSG_OTHER_BLEEDING,
		  COM_MSG_EBODY);
      }
   }
}


void food_update( void )
{
   struct unit_data *u, *next_dude;

   /* characters */
   for (u = unit_list; u; u = next_dude)
   {
      next_dude = u->gnext;

      if (IS_PC(u) && CHAR_DESCRIPTOR(u))
      {
	 struct unit_data *tu = u;
	 
	 while (tu)
	 {
	    if (IS_SET(UNIT_FLAGS(tu), UNIT_FL_SACRED))
	      break;
	    tu = UNIT_IN(tu);
	 }

	 if (tu)
	   continue;

	 if (PC_COND(u, FULL) < 48)
	   gain_condition(u,FULL,-1);
	 if (PC_COND(u, THIRST) < 48)
	   gain_condition(u,THIRST,-1);
	 if (PC_COND(u, DRUNK) > 0)
	   gain_condition(u,DRUNK,-1);
      }
   }
}


void set_title(struct unit_data *ch)
{
   char buf[256];

   assert(IS_PC(ch));

   if (CHAR_LEVEL(ch)==0)
     UNIT_TITLE(ch).Reassign("the guest");
   else if (CHAR_LEVEL(ch) <= START_LEVEL)
   {
      assert(CHAR_RACE(ch) < PC_RACE_MAX);
      sprintf(buf, "the %s", pc_races[CHAR_RACE(ch)]);
      UNIT_TITLE(ch).Reassign(buf);
   }
   else if (IS_IMMORTAL(ch))
     UNIT_TITLE(ch).Reassign("the God");
}


void gain_exp_regardless(struct unit_data *ch, int gain)
{
   int j;

   if (!IS_PC(ch))
     return;

   if (gain > 0)
   {
      j = required_xp(PC_VIRTUAL_LEVEL(ch)+1);

      if ((CHAR_EXP(ch) + gain >= j) && (CHAR_EXP(ch) < j))
      {
	 send_to_char("You can now raise a level by using the 'level' "
		      "command!\n\r", ch);
      }

      CHAR_EXP(ch) += gain;
   }
   else  /* gain <= 0 */
   {
      if ((((sbit32) CHAR_EXP(ch)) + gain) < required_xp(START_LEVEL))
	CHAR_EXP(ch) = required_xp(START_LEVEL);
      else
	CHAR_EXP(ch) += gain;
   }
}


void gain_exp(struct unit_data *ch, int gain)
{
   if (IS_MORTAL(ch))
     gain_exp_regardless(ch, gain);
}


void do_level(struct unit_data *ch, char *arg, const struct command_info *cmd)
{
   int now;

   if (!IS_PC(ch))
   {
      send_to_char("You ain't nothin' but a hound-dog.\n\r", ch);
      return;
   }

#ifdef NOBLE
   if (CHAR_LEVEL(ch) >= NOBLE_OCCURANCE)
   {
      send_to_char("You can't raise levels this way.\n\r", ch);
      return;
   }
#endif

   if (CHAR_LEVEL(ch) > MORTAL_MAX_LEVEL)
   {
      send_to_char("You can't raise levels this way.\n\r", ch);
      return;
   }

   if (required_xp(1+PC_VIRTUAL_LEVEL(ch)) > CHAR_EXP(ch))
   {
      send_to_char("You are not experienced enough.\n\r", ch);
      return;
   }

   now = str_ccmp_next_word(arg, "now") != NULL;

   if (!now && PC_SKILL_POINTS(ch) >= skill_point_gain())
   {
      send_to_char("You havn't used your skill points at all, if you "
		   "really want to level now, type 'level now'\n\r", ch);
      return;
   }

   if (!now && PC_ABILITY_POINTS(ch) >= ability_point_gain())
   {
      send_to_char("You havn't used your ability points at all, if you "
		   "really want to level now, type 'level now'\n\r", ch);
      return;
   }

   advance_level(ch);
}
