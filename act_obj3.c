/* *********************************************************************** *
 * File   : act_obj3.c                                Part of Valhalla MUD *
 * Version: 1.10                                                           *
 * Author : All.                                                           *
 *                                                                         *
 * Purpose: Handling of equipment.                                         *
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

/* 09/07/92 seifert: Corrected a few wear related bugs                     */
/* 30/01/93 gnort  : Changed various things in wear/remove                 */
/* 26/08/94 gnort  : Moved equipment stuff to act_obj3.c                   */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "structs.h"
#include "constants.h"
#include "utils.h"
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
#include "act.h"


/* Return 0 = No hands used                      */
/*        1 = One hand used                      */
/*        2 = Two hands used                     */
/*        2+ = Uhm???                            */
int hands_used(struct unit_data *ch)
{
   struct unit_data *i;
   int hands = 0;
   
   /* This is faster than calling the equipment() function */
   for (i = UNIT_CONTAINS(ch); i; i = i->next)
     if (IS_OBJ(i) && OBJ_EQP_POS(i))
     {
	switch (OBJ_EQP_POS(i))
	{
	  case WEAR_WIELD:
	  case WEAR_HOLD:
	  case WEAR_SHIELD:
	   hands++;
	   if (object_two_handed(i))
	     hands++;
	   break;
	}
     }

   return hands;
}

#ifdef SUSPEKT
static char *wear_weight(struct unit_data *ch, struct unit_data *obj)
{
   int str_diff;
   
   if (OBJ_TYPE(obj) == ITEM_ARMOR || OBJ_TYPE(obj) == ITEM_SHIELD)
   {
      if (OBJ_TYPE(obj) == ITEM_ARMOR)
      {
	 if (OBJ_VALUE(obj, 0) < 0)
	   return NULL;
	 str_diff = OBJ_VALUE(obj, 0) - CHAR_STR(ch);
      }
      else
      {
	 if (OBJ_VALUE(obj, 3) < 0)
	   return NULL;
	 str_diff = OBJ_VALUE(obj, 3) - CHAR_STR(ch);
      }

      if (str_diff < -30)
	return "featherlight";
      else if (str_diff < -20)
	return "very light";
      else if (str_diff < -10)
	return "light";
      else if (str_diff < -3)
	return "fairly light";
      else if (str_diff > 10)
	return "extremely heavy";
      else if (str_diff > 5)
	return "very heavy";
      else if (str_diff > 2)
	return "heavy";
      else if (str_diff > 1)
	return "slightly heavy";
   }
   
   return NULL;
}
#endif


/* MS: I allowed the wear command to be used with weapons to make it
   easier for new players to learn all the different commands. */

static int getkeyword(struct unit_data *obj)
{
   int keyword = -2;
   
   if (UNIT_WEAR(obj, MANIPULATE_WEAR_SHIELD)) keyword = WEAR_SHIELD;
   if (UNIT_WEAR(obj, MANIPULATE_WEAR_FINGER)) keyword = WEAR_FINGER_L;
   if (UNIT_WEAR(obj, MANIPULATE_WEAR_NECK))   keyword = WEAR_NECK_1;
   if (UNIT_WEAR(obj, MANIPULATE_WEAR_WRIST))  keyword = WEAR_WRIST_L;
   if (UNIT_WEAR(obj, MANIPULATE_WEAR_WAIST))  keyword = WEAR_WAIST;
   if (UNIT_WEAR(obj, MANIPULATE_WEAR_ARMS))   keyword = WEAR_ARMS;
   if (UNIT_WEAR(obj, MANIPULATE_WEAR_HANDS))  keyword = WEAR_HANDS;
   if (UNIT_WEAR(obj, MANIPULATE_WEAR_FEET))   keyword = WEAR_FEET;
   if (UNIT_WEAR(obj, MANIPULATE_WEAR_LEGS))   keyword = WEAR_LEGS;
   if (UNIT_WEAR(obj, MANIPULATE_WEAR_ABOUT))  keyword = WEAR_ABOUT;
   if (UNIT_WEAR(obj, MANIPULATE_WEAR_HEAD))   keyword = WEAR_HEAD;
   if (UNIT_WEAR(obj, MANIPULATE_WEAR_BODY))   keyword = WEAR_BODY;
   if (UNIT_WEAR(obj, MANIPULATE_WIELD))       keyword = WEAR_WIELD;
   if (UNIT_WEAR(obj, MANIPULATE_WEAR_CHEST))  keyword = WEAR_CHEST;
   if (UNIT_WEAR(obj, MANIPULATE_WEAR_BACK))   keyword = WEAR_BACK;
   if (UNIT_WEAR(obj, MANIPULATE_WEAR_EAR))    keyword = WEAR_EAR_L;
   if (UNIT_WEAR(obj, MANIPULATE_WEAR_ANKLE))  keyword = WEAR_ANKLE_L;

   return keyword;
}


/* Return NULL if unit fits, pointer to string otherwise */
static const char *wear_size(struct unit_data *ch,
		       struct unit_data *obj, int var)
{
   if (UNIT_SIZE(ch) == 0)
     return "error";

   int percent;

   if (UNIT_SIZE(ch) > 0)
     percent = (100*UNIT_SIZE(obj)) / UNIT_SIZE(ch);
   else
     percent = (100*UNIT_SIZE(obj));

   if (percent < 100-var-(100-var)/2)
     return "way too small";
   else if (percent < (100-var))
     return "too small";
   else if (percent > 100+var+(100+var)/2)
     return "way too large";
   else if (percent > (100+var))
     return "too large";

   return NULL;
}


const char *obj_wear_size(struct unit_data *ch, struct unit_data *obj,
		    int keyword)
{
   if (keyword == -1)
     keyword = getkeyword(obj);

   switch (keyword)
   {
     case WEAR_FINGER_R:
     case WEAR_FINGER_L:
      return wear_size(ch, obj, 50);

     case WEAR_NECK_1:
     case WEAR_NECK_2:
      return wear_size(ch, obj, 50);

     case WEAR_BODY:
      return wear_size(ch, obj, 40);

     case WEAR_HEAD:
      return wear_size(ch, obj, 50);

     case WEAR_LEGS:
      return wear_size(ch, obj, 50);

     case WEAR_FEET:
      return wear_size(ch, obj, 50);

     case WEAR_HANDS:
      return wear_size(ch, obj, 50);

     case WEAR_ARMS:
      return wear_size(ch, obj, 50);

     case WEAR_ABOUT:
      return wear_size(ch, obj, 50);

     case WEAR_WAIST:
      return wear_size(ch, obj, 50);

     case WEAR_WRIST_L:
     case WEAR_WRIST_R:
      return wear_size(ch, obj, 50);

     case WEAR_WIELD:
      return wear_size(ch, obj, 50);

     case WEAR_HOLD:
      if (OBJ_TYPE(obj) == ITEM_WEAPON)
	return wear_size(ch, obj, 50);
      else
	return NULL;

     case WEAR_SHIELD:
      return wear_size(ch, obj, 50);

     case WEAR_CHEST:
      return wear_size(ch, obj, 50);

     case WEAR_BACK:
      return wear_size(ch, obj, 50);

     case WEAR_EAR_L:
     case WEAR_EAR_R:
      return wear_size(ch, obj, 50);

     case WEAR_ANKLE_L:
     case WEAR_ANKLE_R:
      return wear_size(ch, obj, 50);
   }

   return NULL;
}


bool wear(struct unit_data *ch, struct unit_data *obj, int keyword, bool err,
	  const struct command_info *cmd, char *arg)
{
   const char *errstr = NULL;
   const char *c;

   assert(IS_OBJ(obj) && !OBJ_EQP_POS(obj));
   
   switch(keyword)
   {
     case WEAR_UNUSED:
      if (equipment(ch, WEAR_UNUSED))
	send_to_char("DOUBLE ERROR! UNUSED ALREADY USED! REPORT!\n\r", ch);
      else
	send_to_char("UNUSED EQUIPMENT ERROR! Please report!\n\r", ch);
      break;

     case WEAR_FINGER_R:
     case WEAR_FINGER_L:
      if (!UNIT_WEAR(obj, MANIPULATE_WEAR_FINGER))
	errstr = "You can't wear that on your finger.";
      else if (!equipment(ch, WEAR_FINGER_L))
      {
	 if ((c = obj_wear_size(ch, obj, keyword)))
	 {
	    act("$3n is $2t to fit your finger.",
		A_ALWAYS, ch, c, obj, TO_CHAR);
	    return FALSE;
	 }
	 unit_messg(ch, obj, "$wear",
		    "You put $2n on your left hand's ringfinger.",
		    "$1n puts $2n on $1s left hand's ringfinger.");
	 equip_char(ch, obj, WEAR_FINGER_L);
      }
      else if (!equipment(ch, WEAR_FINGER_R))
      {
	 if ((c = obj_wear_size(ch, obj, keyword)))
	 {
	    act("$3n is $2t to fit your finger.",
		A_ALWAYS, ch, c, obj, TO_CHAR);
	    return FALSE;
	 }
	 unit_messg(ch, obj, "$wear",
		    "You put $2n on your right hand's ringfinger.",
		    "$1n puts $2n on $1s right hand's ringfinger.");
	 equip_char(ch, obj, WEAR_FINGER_R);
      }
      else
	errstr = "You're already using two rings.";
      break;

     case WEAR_NECK_1:
     case WEAR_NECK_2:
      if (!UNIT_WEAR(obj, MANIPULATE_WEAR_NECK))
	errstr = "You can't wear that around your neck.";
      else if (!equipment(ch, WEAR_NECK_1))
      {
	 if ((c = obj_wear_size(ch, obj, keyword)))
	 {
	    act("$3n is $2t to fit around your neck.",
		A_ALWAYS, ch, c, obj, TO_CHAR);
	    return FALSE;
	 }
	 unit_messg(ch, obj, "$wear",
		    "You wear $2n around your neck.",
		    "$1n wears $2n around $1s neck.");
	 equip_char(ch, obj, WEAR_NECK_1);
      }
      else if (!equipment(ch, WEAR_NECK_2))
      {
	 if ((c = obj_wear_size(ch, obj, keyword)))
	 {
	    act("$3n is $2t to fit around your neck.",
		A_ALWAYS, ch, c, obj, TO_CHAR);
	    return FALSE;	 
	 }
	 unit_messg(ch, obj, "$wear",
		    "You wear $2n around your neck.",
		    "$1n wears $2n around $1s neck.");
	 equip_char(ch, obj, WEAR_NECK_2);
      }
      else
	errstr = "You can't wear any more around your neck.";
      break;

     case WEAR_BODY:
      if (!UNIT_WEAR(obj, MANIPULATE_WEAR_BODY))
	errstr = "You can't wear that on your body.";
      else if (!equipment(ch, WEAR_BODY))
      {
	 if ((c = obj_wear_size(ch, obj, keyword)))
	 {
	    act("$3n is $2t to fit your body.",
		A_ALWAYS, ch, c, obj, TO_CHAR);
	    return FALSE;
	 }
	 unit_messg(ch, obj, "$wear",
		    "You put $2n on your body.",
		    "$1n puts $2n on $1s body.");
	 equip_char(ch, obj, WEAR_BODY);
      }
      else
	errstr = "You already wear something on your body.";
      break;

     case WEAR_HEAD:
      if (!UNIT_WEAR(obj, MANIPULATE_WEAR_HEAD))
	errstr = "You can't wear that on your head.";
      else if (!equipment(ch, WEAR_HEAD))
      {
	 if ((c = obj_wear_size(ch, obj, keyword)))
	 {
	    act("$3n is $2t to fit your head.",
		A_ALWAYS, ch, c, obj, TO_CHAR);
	    return FALSE;
	 }
	 unit_messg(ch, obj, "$wear",
		    "You wear $2n on your head.",
		    "$1n wears $2n on $1s head.");
	 equip_char(ch, obj, WEAR_HEAD);
      }
      else
	errstr = "You already wear something on your head.";
      break;

     case WEAR_LEGS:
      if (!UNIT_WEAR(obj, MANIPULATE_WEAR_LEGS))
	errstr = "You can't wear that on your legs.";
      else if (!equipment(ch, WEAR_LEGS))
      {
	 if ((c = obj_wear_size(ch, obj, keyword)))
	 {
	    act("$3n are $2t to fit your legs.",
		A_ALWAYS, ch, c, obj, TO_CHAR);
	    return FALSE;
	 }
	 unit_messg(ch, obj, "$wear",
		    "You put $2n on your legs.",
		    "$1n puts $2n on $1s legs.");
	 equip_char(ch, obj, WEAR_LEGS);
      }
      else
	errstr = "You already wear something on your legs.";
      break;

     case WEAR_FEET:
      if (!UNIT_WEAR(obj, MANIPULATE_WEAR_FEET))
	errstr = "You can't wear that on your feet.";
      else if (!equipment(ch, WEAR_FEET))
      {
	 if ((c = obj_wear_size(ch, obj, keyword)))
	 {
	    act("$3n are $2t to fit your feet.",
		A_ALWAYS, ch, c, obj, TO_CHAR);
	    return FALSE;
	 }
	 unit_messg(ch, obj, "$wear",
		    "You put $2n on your feet.",
		    "$1n puts $2n on $1s feet.");
	 equip_char(ch, obj, WEAR_FEET);
      }
      else
	errstr = "You already wear something on your feet.";
      break;

     case WEAR_HANDS:
      if (!UNIT_WEAR(obj, MANIPULATE_WEAR_HANDS))
	errstr = "You can't wear that on your hands.";
      else if (!equipment(ch, WEAR_HANDS))
      {
	 if ((c = obj_wear_size(ch, obj, keyword)))
	 {
	    act("$3n are $2t to fit your hands.",
		A_ALWAYS, ch, c, obj, TO_CHAR);
	    return FALSE;
	 }
	 unit_messg(ch, obj, "$wear",
		    "You start using $2n.",
		    "$1n starts using $2n.");
	 equip_char(ch, obj, WEAR_HANDS);
      }
      else
	errstr = "You already wear something on your hands.";
      break;

     case WEAR_ARMS:
      if (!UNIT_WEAR(obj, MANIPULATE_WEAR_ARMS))
	errstr = "You can't wear that on your arms.";
      else if (!equipment(ch, WEAR_ARMS))
      {
	 if ((c = obj_wear_size(ch, obj, keyword)))
	 {
	    act("$3n are $2t to fit your arms.",
		A_ALWAYS, ch, c, obj, TO_CHAR);
	    return FALSE;
	 }
	 unit_messg(ch, obj, "$wear",
		    "You wear $2n on your arms.",
		    "$1n wears $2n on $1s arms.");
	 equip_char(ch, obj, WEAR_ARMS);
      }
      else
	errstr = "You already wear something on your arms.";
      break;

     case WEAR_ABOUT:
      if (!UNIT_WEAR(obj, MANIPULATE_WEAR_ABOUT))
	errstr = "You can't wear that about your body.";
      else if (!equipment(ch, WEAR_ABOUT))
      {
	 if ((c = obj_wear_size(ch, obj, keyword)))
	 {
	    act("$3n is $2t to fit about your body.",
		A_ALWAYS, ch, c, obj, TO_CHAR);
	    return FALSE;
	 }
	 unit_messg(ch, obj, "$wear",
		    "You wrap $2n about your body.",
		    "$1n wraps $2n about $1s body.");
	 equip_char(ch, obj, WEAR_ABOUT);
      }
      else
	errstr = "You already wear something about your body.";
      break;

     case WEAR_WAIST:
      if (!UNIT_WEAR(obj, MANIPULATE_WEAR_WAIST))
	errstr = "You can't wear that about your waist.";
      else if (!equipment(ch, WEAR_WAIST))
      {
	 if ((c = obj_wear_size(ch, obj, keyword)))
	 {
	    act("$3n is $2t to fit around your waist.",
		A_ALWAYS, ch, c, obj, TO_CHAR);
	    return FALSE;
	 }
	 unit_messg(ch, obj, "$wear",
		    "You strap $2n about your waist.",
		    "$1n straps $2n about $1s waist.");
	 equip_char(ch,  obj, WEAR_WAIST);
      }
      else
	errstr = "You already wear something about your waist.";
      break;

     case WEAR_WRIST_L:
     case WEAR_WRIST_R:
      if (!UNIT_WEAR(obj, MANIPULATE_WEAR_WRIST))
	errstr = "You can't wear that around your wrist.";
      else if (!equipment(ch, WEAR_WRIST_L))
      {
	 if ((c = obj_wear_size(ch, obj, keyword)))
	 {
	    act("$3n is $2t to fit around your wrist.",
		A_ALWAYS, ch, c, obj, TO_CHAR);
	    return FALSE;
	 }
	 unit_messg(ch, obj, "$wear",
		    "You wear $2n around your left wrist.",
		    "$1n wears $2n around $1s left wrist.");
	 equip_char(ch,  obj, WEAR_WRIST_L);
      }
      else if (!equipment(ch, WEAR_WRIST_R))
      {
	 if ((c = obj_wear_size(ch, obj, keyword)))
	 {
	    act("$3n is $2t to fit around your wrist.",
		A_ALWAYS, ch, c, obj, TO_CHAR);
	    return FALSE;
	 }
	 unit_messg(ch, obj, "$wear",
		    "You wear $2n around your right wrist.",
		    "$1n wears $2n around $1s right wrist.");
	 equip_char(ch, obj, WEAR_WRIST_R);
      }
      else
	errstr = "You already wear something around both your wrists.";
      break;

     case WEAR_WIELD:
      if (!UNIT_WEAR(obj, MANIPULATE_WIELD))
	errstr = "You can't wield that.";
      else if (hands_used(ch) >= 2)
	errstr = "You got no free hands.";
      else if (object_two_handed(obj) && hands_used(ch) > 0)
	errstr = "You need two free hands to wield this weapon.";
      else if (!equipment(ch, WEAR_WIELD))
      {
	 if ((c = obj_wear_size(ch, obj, keyword)))
	 {
	    act("$3n is $2t to be wielded by you.",
		A_ALWAYS, ch, c, obj, TO_CHAR);
	    return FALSE;
	 }
	 if (object_two_handed(obj))
	   unit_messg(ch, obj, "$wear",
		      "You wield $2n two-handedly.",
		      "$1n wields $2n two-handedly.");
	 else
	   unit_messg(ch, obj, "$wear",
		      "You wield $2n.",
		      "$1n wields $2n.");
	 equip_char(ch, obj, WEAR_WIELD);
	 if (IS_PC(ch) && OBJ_TYPE(obj) == ITEM_WEAPON)
	   act("You are $3t at fighting with $2n.",
	       A_SOMEONE, ch, obj,
	       skill_text(weapon_skills, PC_WPN_SKILL(ch, OBJ_VALUE(obj, 0))),
	       TO_CHAR);
      }
      else
	errstr = "You are already wielding something.";
      break;

     case WEAR_HOLD:
      if (!UNIT_WEAR(obj, MANIPULATE_HOLD|MANIPULATE_WIELD))
	errstr = "You can't hold this.";
      else if (hands_used(ch) >= 2)
	errstr = "Your hands are full.";
      else if (!equipment(ch, WEAR_HOLD))
      {
	 if (object_two_handed(obj) && hands_used(ch) > 0)
	 {
	    errstr = "It requires two free hands to use!";
	    break;
	 }

	 if (OBJ_TYPE(obj) == ITEM_WEAPON)
	 {
	    if (IS_SET(OBJ_FLAGS(obj), OBJ_NO_DUAL))
	    {
	       errstr = "This weapon is too large to be used as a second "
		 "weapon.";
	       break;
	    }
	    if ((c = obj_wear_size(ch, obj, keyword)))
	    {
	       act("$3n is $2t to be wielded by you.",
		   A_ALWAYS, ch, c, obj, TO_CHAR);
	       return FALSE;
	    }
	 }

	 unit_messg(ch, obj, "$wear",
		    "You start holding $2n.",
		    "$1n starts holding $2n.");
	 equip_char(ch, obj, WEAR_HOLD);
      }
      else
	errstr = "You are already holding something.";
      break;

     case WEAR_SHIELD:
      if (!UNIT_WEAR(obj, MANIPULATE_WEAR_SHIELD))
	errstr = "You can't use that as a shield.";
      else if (OBJ_TYPE(obj) != ITEM_SHIELD)
	errstr = "That is not a shield.";
      else if (hands_used(ch) >= 2)
	errstr = "Your hands are full.";
      else if (!equipment(ch, WEAR_SHIELD))
      {
	 if ((c = obj_wear_size(ch, obj, keyword)))
	 {
	    act("$3n are $2t to fit your arms.",
		A_ALWAYS, ch, c, obj, TO_CHAR);
	    return FALSE;
	 }

	 unit_messg(ch, obj, "$wear",
		    "You strap $2n to your arm.",
		    "$1n straps $2n on $1s arm.");
	 equip_char(ch, obj, WEAR_SHIELD);
      }
      else
	errstr = "You are already using a shield.";
      break;

     case WEAR_CHEST:
      if (!UNIT_WEAR(obj, MANIPULATE_WEAR_CHEST))
	errstr = "You can't wear that across your chest.";
      else if (!equipment(ch, WEAR_CHEST))
      {
	 if ((c = obj_wear_size(ch, obj, keyword)))
	 {
	    act("$3n is $2t to fit your chest.",
		A_ALWAYS, ch, c, obj, TO_CHAR);
	    return FALSE;
	 }

	 unit_messg(ch, obj, "$wear",
		    "You strap $2n across your chest.",
		    "$1n straps $2n across $1s chest.");
	 equip_char(ch,  obj, WEAR_CHEST);
      }
      else
	errstr = "You already wear something across your chest.";
      break;

     case WEAR_BACK:
      if (!UNIT_WEAR(obj, MANIPULATE_WEAR_BACK))
	errstr = "You can't wear that on your back.";
      else if (!equipment(ch, WEAR_BACK))
      {
	 if ((c = obj_wear_size(ch, obj, keyword)))
	 {
	    act("$3n is $2t to fit on your back.",
		A_ALWAYS, ch, c, obj, TO_CHAR);
	    return FALSE;
	 }

	 unit_messg(ch, obj, "$wear",
		    "You strap $2n on your back.",
		    "$1n straps $2n around $1s back.");
	 equip_char(ch,  obj, WEAR_BACK);
      }
      else
	errstr = "You already wear something on your back.";
      break;

     case WEAR_EAR_L:
     case WEAR_EAR_R:
      if (!UNIT_WEAR(obj, MANIPULATE_WEAR_EAR))
	errstr = "You can't wear that on your ear.";
      else if (!equipment(ch, WEAR_EAR_L))
      {
	 if ((c = obj_wear_size(ch, obj, keyword)))
	 {
	    act("$3n is $2t to fit your ear.",
		A_ALWAYS, ch, c, obj, TO_CHAR);
	    return FALSE;
	 }

	 unit_messg(ch, obj, "$wear",
		    "You wear $2n on your left ear.",
		    "$1n wears $2n on $1s left ear.");
	 equip_char(ch, obj, WEAR_EAR_L);
      }
      else if (!equipment(ch, WEAR_EAR_R))
      {
	 if ((c = obj_wear_size(ch, obj, keyword)))
	 {
	    act("$3n is $2t to fit your ear.",
		A_ALWAYS, ch, c, obj, TO_CHAR);
	    return FALSE;
	 }

	 unit_messg(ch, obj, "$wear",
		    "You wear $2n on your right ear.",
		    "$1n wears $2n on $1s right ear.");
	 equip_char(ch, obj, WEAR_EAR_R);
      }
      else
	errstr = "You can't wear anything else on your ear.";
      break;

     case WEAR_ANKLE_L:
     case WEAR_ANKLE_R:
      if (!UNIT_WEAR(obj, MANIPULATE_WEAR_ANKLE))
	errstr = "You can't wear that on your ankle.";
      else if (!equipment(ch, WEAR_ANKLE_L))
      {
	 if ((c = obj_wear_size(ch, obj, keyword)))
	 {
	    act("$3n is $2t to fit your ankle.",
		A_ALWAYS, ch, c, obj, TO_CHAR);
	    return FALSE;
	 }

	 unit_messg(ch, obj, "$wear",
		    "You wear $2n around your left ankle.",
		    "$1n wears $2n around $1s left ankle.");
	 equip_char(ch, obj, WEAR_ANKLE_L);
      }
      else if (!equipment(ch, WEAR_ANKLE_R))
      {
	 if ((c = obj_wear_size(ch, obj, keyword)))
	 {
	    act("$3n is $2t to fit your ankle.",
		A_ALWAYS, ch, c, obj, TO_CHAR);
	    return FALSE;
	 }

	 unit_messg(ch, obj, "$wear",
		    "You wear $2n around your right ankle.",
		    "$1n wears $2n around $1s right ankle.");
	 equip_char(ch, obj, WEAR_ANKLE_R);
      }
      else
	errstr = "You can't wear anything else on your ankles.";
      break;

     case -1:
      errstr = "Wear $2n where?";
      break;

     case -2:
      errstr = "You can not wear $2n.";
      break;

     default:
      errstr = "Unknown weartype for $2n. Please report.";
      slog(LOG_ALL, 0, "Unknown type (%d) called in wear.", keyword);
      break;
   }

   if (errstr == NULL)
   {
      send_done(ch, obj, NULL, 0, cmd, arg);
      return TRUE;
   }
   
   if (err)
     act(errstr, A_SOMEONE, ch, obj, 0, TO_CHAR);
   return FALSE;
}


void do_wear(struct unit_data *ch, char *arg,
	     const struct command_info *cmd)
{
   static const char *keywords[] =
   {
      "finger",
      "neck",
      "body",
      "head",
      "legs",
      "feet",
      "hands",
      "arms",
      "about",
      "waist",
      "wrist",
      "shield",
      "chest",
      "back",
      "ear",
      "ankle",
      NULL
   };

   static int keytrans[] =
   {
      WEAR_FINGER_L,
      WEAR_NECK_1,
      WEAR_BODY,
      WEAR_HEAD,
      WEAR_LEGS,
      WEAR_FEET,
      WEAR_HANDS,
      WEAR_ARMS,
      WEAR_ABOUT,
      WEAR_WAIST,
      WEAR_WRIST_L,
      WEAR_SHIELD,
      WEAR_CHEST,
      WEAR_BACK,
      WEAR_EAR_L,
      WEAR_ANKLE_L
   };

   struct unit_data *obj;
   char *oarg = arg;

   if (str_is_empty(arg))
     send_to_char("Wear what (where)?\n\r", ch);
   else if (!(obj = find_unit(ch, &arg, 0, FIND_UNIT_INVEN)))
   {
      char buf[MAX_INPUT_LENGTH];

      one_argument(arg, buf);

      if(!strcmp(buf, "all"))
      {
	 struct unit_data *next;
	 bool worn = FALSE;

	 for (obj = UNIT_CONTAINS(ch); obj; obj = next)
	 {
	    if (UNIT_IN(obj) != ch)
	    {
	       // Each iteration can cause illegal lists.
	       act("You stop wearing all, it seems unsafe.",
                   A_SOMEONE, ch, 0, 0, TO_CHAR);
	       break;
	    }

	    next = obj->next;
	    if (IS_OBJ(obj) && !OBJ_EQP_POS(obj))
	      worn = (wear(ch, obj, getkeyword(obj), FALSE, cmd, oarg)
		      || worn);
	 }
	 if (!worn)
	   act("You have nothing to wear.", A_SOMEONE, ch, 0, 0, TO_CHAR);
      }
      else
	send_to_char("You are carrying no such thing.\n\r", ch);
   }
   else if (!IS_OBJ(obj))
     send_to_char("You can't wear that.\n\r", ch);
   else if (!str_is_empty(arg))
   {
      char buf[MAX_INPUT_LENGTH];
      int keyword;

      arg = one_argument(arg, buf);

      keyword = search_block(buf, keywords, FALSE); /* Partial Match */

      if (keyword == -1)
	act("$2t is an unknown body location.", A_SOMEONE, ch, buf, 0, TO_CHAR);
      else
	wear(ch, obj, keytrans[keyword], TRUE, cmd, oarg);
   }
   else
     wear(ch, obj, getkeyword(obj), TRUE, cmd, oarg);
}


void do_wield(struct unit_data *ch, char *arg,
	      const struct command_info *cmd)
{
   struct unit_data *obj;
   char *oarg = arg;

   if (str_is_empty(arg))
     send_to_char("Wield which weapon?\n\r", ch);
   else if (!(obj = find_unit(ch, &arg, 0, FIND_UNIT_INVEN)))
     send_to_char("You are carrying no such weapon.\n\r", ch);
   else if (IS_OBJ(obj) && OBJ_TYPE(obj) == ITEM_WEAPON)
   {
      switch(wpn_info[OBJ_VALUE(obj, 0)].hands)
      {
	case 2:
	 break;

#ifdef SUSPEKT
	case 3:			/* One'n'half handed */
	 arg = skip_spaces(arg);
	 if (!str_nccmp(arg, "two", 3))
	 {
	    break;
	 }
	 /* Fallthru */
#endif

	default:
	 break;
      }

      wear(ch, obj, WEAR_WIELD, TRUE, cmd, oarg);
   }
   else
     send_to_char("That is not a proper weapon.\n\r", ch);
}


void do_grab(struct unit_data *ch, char *arg,
	     const struct command_info *cmd)
{
   struct unit_data *obj;
   char *oarg = arg;

   if (str_is_empty(arg))
     send_to_char("Grab what?\n\r", ch);
   else if (!(obj = find_unit(ch, &arg, 0, FIND_UNIT_INVEN)))
     send_to_char("You are carrying no such thing.\n\r", ch);
   else if (!IS_OBJ(obj))
     send_to_char("You can't hold that in your hand.\n\r", ch);
   else
     wear(ch, obj, WEAR_HOLD, TRUE, cmd, oarg);
}


bool remove_equip(struct unit_data *ch, struct unit_data *obj,
	const struct command_info *cmd, char *arg)
{
   if (IS_SET(OBJ_FLAGS(obj), OBJ_NO_UNEQUIP))
   {
      act("The $3N is cursed, you can't remove it!",
	  A_SOMEONE, ch, 0, obj, TO_CHAR);
      act("$1n tries to get rid of $3n, but in vain!",
	  A_HIDEINV, ch, 0, obj, TO_ROOM);
      return FALSE;
   }

   unequip_object(obj);

   unit_messg(ch, obj, "$rem", "You stop using $2n.", "$1n stops using $2n.");

   send_done(ch, obj, NULL, 0, cmd, arg);

   return TRUE;
}


void do_remove(struct unit_data *ch, char *arg,
	       const struct command_info *cmd)
{
   struct unit_data *obj;
   char *oarg = arg;

   if (str_is_empty(arg))
     send_to_char("Remove what?\n\r", ch);
   else if (!(obj = find_unit(ch, &arg, 0, FIND_UNIT_EQUIP)))
   {
      char buf[MAX_INPUT_LENGTH];

      one_argument(arg, buf);	/* Makes lowercase */

      if (!strcmp(buf, "all"))
      {
	 struct unit_data *next;
	 bool removed = FALSE;

	 for (obj = UNIT_CONTAINS(ch); obj; obj = next)
	 {
	    if (UNIT_IN(obj) != ch)
	    {
	       // Each iteration can cause illegal lists.
	       act("You stop removing all, it seems unsafe.",
                   A_SOMEONE, ch, 0, 0, TO_CHAR);
	       break;
	    }

	    next = obj->next;
	    if (IS_OBJ(obj) && OBJ_EQP_POS(obj))
	      removed = (remove_equip(ch, obj, cmd, oarg) || removed);
	 }
	 if (!removed)
	   act("There was nothing to remove.", A_SOMEONE, ch, 0, 0, TO_CHAR);
      }
      else
	send_to_char("You are using no such thing.\n\r", ch);
   }
   else
     remove_equip(ch, obj, cmd, oarg);
}
