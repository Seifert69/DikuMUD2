/* *********************************************************************** *
 * File   : common.c                                  Part of Valhalla MUD *
 * Version: 1.11                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Common routines and data between dmserver and dmc.             *
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
#include <math.h> /* floor and pow */

#include "structs.h"
#include "utils.h"
#include "common.h"
#include "skills.h"
#include "utility.h"
#include "db.h"

char libdir[64] = DFLT_DIR;


const char *fillwords[] =
{
   "a",
   "an",
   "at",
   "from",
   "in",
   "on",
   "of",
   "the",
   "to",
   "with",
   "into",
   NULL
};

/* Used for converting general direction in dmc! */
const char *dirs[] =
{
  "north",
  "east",
  "south",
  "west",
  "up",
  "down",
  NULL
};


/* Used for sanity check in dmc! */
const char *drinks[LIQ_MAX+2] =
{
  "water",
  "beer",
  "wine",
  "ale",
  "dark ale",
  "whisky",
  "lemonade",
  "firebreather",
  "local speciality",
  "slime",
  "milk",
  "tea",
  "coffee",
  "blood",
  "salt water",
  "coke",
  "vodka",
  "brandy",
  NULL
};


struct shi_info_type shi_info[] =
{
  /* %age Chance of blocking an attack if ready to block */
  {15},    /* SHIELD_SMALL  */
  {20},    /* SHIELD_MEDIUM */
  {25}     /* SHIELD_LARGE  */
};


/* PS Algorithm 2                                                      */
/* This algorithm returns the amount of points gained at a particular  */
/* level. Level is given as an argument, and the amount of points you  */
/* will get for that level is returned                                 */
/* Example: A character is about to raise from level 2 to 3. Add       */
/*          ability_point_gain(3) to his ability points                */

int ability_point_gain(void)
{
   return AVERAGE_SKILL_COST * ABILITY_POINT_FACTOR;
}

int skill_point_gain(void)
{
   return AVERAGE_SKILL_COST * SKILL_POINT_FACTOR;
}

/* PS Algorithm 3                                                      */
/* This algorithm returns the total amount of points gained up to a    */
/* particular level.                                                   */
/* The formula is total up to the current level                        */

int ability_point_total(int level)
{
   return (1+level) * ability_point_gain();
}


int skill_point_total(int level)
{
   return (1+level) * skill_point_gain();
}


/* Algorithm PS 4                                                      */
/*                                                                     */
/* This algorithm takes a base cost (cost) and an amount of points     */
/* (points). It then calculates how many ability points you could buy  */
/* for the given amount of points (skill-buy-points)                   */
/*                                                                     */

int buy_points(int points, int level, int *error)
{
   int skill;
   int cost;
   int i;

   assert(level >= 0);

   skill = 0;
   cost = AVERAGE_SKILL_COST;

   while (points > 0)
   {
      for (i=0; (i <= level) && (points > 0); i++)
      {
	 points -= cost;
	 skill++;
      }
      cost += AVERAGE_SKILL_COST;
   }

   if (points < 0)
     skill--;

   if (error)
     *error = 0;

   if (skill > SKILL_MAX)
   {
      if (error)
	*error = skill;

      return SKILL_MAX;
   }
   if (skill < 0)
   {
      if (error)
	*error = skill;

      return 0;
   }

   return skill;
}



/* Algorithm PS 5                                                      */
/* Used to distribute points after a given percentage scheme. Each     */
/* ability (8 of them) contains the percentage number. The percentage  */
/* must add up to 100%.                                                */
/* Returns true if error */

int distribute_points(ubit8 *skills, int max, int points, int level)
{
   int i, error, sumerror;

   sumerror = 0;

   for(i=0; i < max; i++)
   {
      skills[i] = buy_points((int) ((double) skills[i] * points / 100.0),
			     level, &error);
      if (error > sumerror)
	sumerror = error;
   }
   
   return sumerror;
}


/* Apply quality to a number (ac, dam or other) */
int apply_quality(int num, int quality)
{
/* if ((quality < 0) || (quality > 200))
     slog(LOG_ALL, "ERROR: Quality is %d.", quality); */

   if (quality >= 100)
     return (quality*num)/100;
   else
     return ( MAX(1, 50+quality/2)*num )/100;
}



/* Given a level, it returns how much XP is required to reach the  */
/* next level. For example, given level 1, it returns that it      */
/* takes 310.000 xp to reach level 2.                              */

int level_xp(int level)
{
   if (level <= MORTAL_MAX_LEVEL)
     return 1650 + level*300;
   else
     return level_xp(MORTAL_MAX_LEVEL);
}




/* Given a level, it returns the total amount of experience-points */
/* which is minimum required to be that level. For example         */
/* required_xp(1) = 150,000 since it requires 150,000 xp to be     */
/* level one.                                                      */

int required_xp(int level)
{
   if (level <= MORTAL_MAX_LEVEL)
     return 1500*level + level*level*(300/2);
   else
     return required_xp(MORTAL_MAX_LEVEL) +
            level_xp(MORTAL_MAX_LEVEL) * (level - MORTAL_MAX_LEVEL);
}


/* Primarily used for shields, armours and weapons */
void set_hits(struct unit_data *obj, int craftsmanship)
{
   if (UNIT_HIT(obj) == 100)  // 100 is the *default* and is overridden
   {
      /* Hits are in [100..6000] based on craft, default == 1000 */

      if (craftsmanship >= 0)
	UNIT_MAX_HIT(obj) = 1000 + (1000 * craftsmanship) / 5;
      else
	UNIT_MAX_HIT(obj) = 1000 - (175 * -craftsmanship) / 5;
      
      UNIT_HIT(obj) = UNIT_MAX_HIT(obj);
   }
}

/* ----------------------------------------------------------------- */

int is_in(int a, int from, int to)
{
   return ((a >= from) && (a <= to));
}


/* WEAPONS                               */
/* Input:  Value[0] is weapon category   */
/*         Value[1] is craftsmanship     */
/*         Value[2] is magic bonus       */
/*         Value[3] is slaying race      */
/*         Value[4]                      */
/*                                       */
void set_weapon(struct unit_data *weapon)
{
   set_hits(weapon, OBJ_VALUE(weapon, 1));
}

/* ARMOURS                                */
/* INPUT:  Value[0] = Category            */
/*         Value[1] is craftsmanship      */
/*         Value[2] is magic bonus        */

void set_armour(struct unit_data *armour)
{
   set_hits(armour, OBJ_VALUE(armour, 1));
}


/* SHIELDS                                  */
/* INPUT:  Value[0] = Shield Category       */
/*         Value[1] is craftsmanship        */
/*         Value[2] is magic bonus          */
/*                                          */
void set_shield(struct unit_data *shield)
{
   set_hits(shield, OBJ_VALUE(shield, 1));
}


