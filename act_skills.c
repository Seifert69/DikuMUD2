/* *********************************************************************** *
 * File   : act_skills.c                              Part of Valhalla MUD *
 * Version: 1.20                                                           *
 * Author : bombman@diku.dk                                                *
 *                                                                         *
 * Purpose: Skill implementations                                          *
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

/* 19/07/92 seifert: Recoded pick lock to use same system as unlock        */
/* 29/07/92 seifert: Fixed do_rescue bugs                                  */
/* 02/08/92 seifert: Fixed bug with hands-used (do_steal)                  */
/* 28/09/92 seifert: Fixed bug in ventriloquate and other stuff            */
/* 01/10/92 seifert: Fixed minor details                                   */
/* 23/08/93 jubal  : Made fleeing possible inside objs, or while non-fight */
/* 29/08/93 jubal  : Appraise uses trade price                             */
/* 07/11/93 gnort  : Fixed no-message `bug' in consider                    */
/* 12-Aug-94 gnort : Inserted spell-type check in recite and use           */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "structs.h"
#include "utils.h"
#include "textutil.h"
#include "skills.h"
#include "comm.h"
#include "interpreter.h"
#include "affect.h"
#include "handler.h"
#include "spells.h"
#include "justice.h"
#include "magic.h"
#include "utility.h"
#include "movement.h"
#include "money.h"
#include "common.h"
#include "dbfind.h"
#include "fight.h"
#include "limits.h"

/* The TURN_UNDEAD skill */
void do_turn(struct unit_data *ch, char *arg,
	     const struct command_info *cmd)
     /*
      * coded: Thu Jun 11 18:48:17 MET DST 1992 [HH]
      * tested: No
      */
{
   int skilla, skilld, hm;
   struct unit_data *vict;
   
   if (IS_PC(ch) && PC_SKI_SKILL(ch, SKI_TURN_UNDEAD) == 0)
   {
      send_to_char("You failed.\n\r", ch);
      return;
   }
   
   if (!(vict = find_unit(ch, &arg, 0, FIND_UNIT_SURRO)))
   {
      send_to_char("No such undead here?\n\r", ch);
      return;
   }
   
   if (!IS_CHAR(vict))
   {
      send_to_char("This one is stone dead for sure!\n\r", ch);
      return;
   }
   
   /* Turn a monster */
   
   skilla = IS_PC(ch)   ? PC_SKI_SKILL(ch,   SKI_TURN_UNDEAD) : CHAR_DIV(ch);
   skilld = IS_PC(vict) ? PC_SKI_SKILL(vict, SKI_TURN_UNDEAD) : CHAR_DIV(vict);
   
   hm = resistance_skill_check(CHAR_DIV(ch), CHAR_DIV(vict), skilla, skilld);
   
   if (hm >= 0 && CHAR_IS_UNDEAD(vict))
   {
      act("$3n is terrified!", A_SOMEONE, ch, 0, vict, TO_CHAR);
      act("$1n turns $3n!", A_SOMEONE, ch, 0, vict, TO_NOTVICT);
      act("You are turned by $1n!", A_SOMEONE, ch, 0, vict, TO_VICT);
      /* NB Make the monster flee here */
      char mstmp[MAX_INPUT_LENGTH]={0};
      do_flee(vict, mstmp, &cmd_auto_unknown);
   }
   else
   {
      act("$3n seems to ignore your attempt.",
	  A_SOMEONE, ch, 0, vict, TO_CHAR);
      act("$1n attempts to turn $3n.", A_SOMEONE, ch, 0, vict, TO_NOTVICT);
      act("$1n attempts to turn you.", A_SOMEONE, ch, 0, vict, TO_VICT);
   }
}



/* Not a skill, but closely related to scroll & wand code */
void do_quaff(struct unit_data *ch, char *arg,
	      const struct command_info *cmd)
{
   struct unit_data *potion;
   int i;

   if (str_is_empty(arg))
   {
      act("What do you want to quaff?",
	  A_ALWAYS, ch, NULL, NULL, TO_CHAR);
      return;
   }   
   
   potion = find_unit(ch, &arg, 0, FIND_UNIT_IN_ME);
   
   if (!potion)
   {
      act("You do not have that item.", A_SOMEONE, ch, 0, 0, TO_CHAR);
      return;
   }
   
   if (!IS_OBJ(potion))
   {
      act("You can't quaff that!", A_SOMEONE, ch, 0, 0, TO_CHAR);
      return;
   }
   
   if (OBJ_TYPE(potion) != ITEM_POTION)
   {
      act("You can only quaff potions.", A_SOMEONE, ch, 0, 0, TO_CHAR);
      return;
   }
   
   act("$1n quaffs $2n.", A_HIDEINV, ch, potion, 0, TO_ROOM);
   act("You quaff $2n which dissolves.", A_SOMEONE, ch, potion, 0, TO_CHAR);

   for (i = 1; i < 4; i++)
     if (OBJ_VALUE(potion, i) != SPL_NONE)
     {
        char mstmp[MAX_INPUT_LENGTH]={0};
	spell_perform(OBJ_VALUE(potion, i),
		      MEDIA_POTION,
		      ch, potion, ch, mstmp);
     }
   
   send_done(ch, potion, NULL, 0, cmd, arg);

   extract_unit(potion);
}


/* The SCROLL_USE skill */
void do_recite(struct unit_data *ch, char *arg,
	       const struct command_info *cmd)
     /*
      * coded: Thu Jun 11 19:10:52 MET DST 1992 [HH]
      * tested: No
      */
{
   struct unit_data *scroll;
   struct unit_data *target;
   int i, skilla, abila, hm;
   
   if (str_is_empty(arg))
   {
      act("What do you want to recite?",
	  A_ALWAYS, ch, NULL, NULL, TO_CHAR);
      return;
   }

   if (IS_PC(ch) && PC_SKI_SKILL(ch, SKI_SCROLL_USE) == 0)
   {
      send_to_char("You must practice first.\n\r", ch);
      return;
   }
   
   scroll = find_unit(ch, &arg, 0, FIND_UNIT_IN_ME);
   if (scroll == NULL)
   {
      act("You do not have such an item.", A_SOMEONE, ch, 0, 0, TO_CHAR);
      return;
   }
   
   if (!IS_OBJ(scroll) || OBJ_TYPE(scroll) != ITEM_SCROLL)
   {
      act("Recite is normally used for scroll's.",
	  A_SOMEONE, ch, 0, 0, TO_CHAR);
      return;
   }
   
   /* If no 'target' to recite on, assume reciter himself */
   if (str_is_empty(arg))
     target = ch;
   else
     target = find_unit(ch, &arg, 0, FIND_UNIT_HERE);
   
   if (target == NULL)
   {
      send_to_char("No such thing around to recite the scroll on.\n\r", ch);
      return;
   }
   
   act("$1n recites $2n.", A_HIDEINV, ch, scroll, 0, TO_ROOM);
   
   skilla = IS_PC(ch) ? PC_SKI_SKILL(ch, SKI_SCROLL_USE) : CHAR_BRA(ch);
   abila  = CHAR_BRA(ch);
   
   hm = resistance_skill_check(2*abila, object_power(scroll),
			       2*skilla, object_power(scroll));
   if (hm < -50)
   {
      act("You failed to recite the scroll properly.",
	  A_SOMEONE, ch, 0, 0, TO_CHAR);
      /* Maybe do "random" spell here? */
      act("You fail to recite $2n which then crumbles to dust.",
	  A_SOMEONE, ch, scroll, 0, TO_CHAR);
      extract_unit(scroll);
      return;
   }
   
   if (hm < 0)
   {
      act("You failed to recite the scroll properly.",
	  A_SOMEONE, ch, 0, 0, TO_CHAR);
      /* Maybe do "random" spell here? */
      return;
   }
   
   act("You recite $2n which dissolves.", A_SOMEONE, ch, scroll, 0, TO_CHAR);
   
   /* Value 1, 2, 3 are spells to cast (or SPL_NONE) */
   for (i = 1; i < 4; i++)
     if (OBJ_VALUE(scroll, i) != SPL_NONE)
     {
        char mstmp[MAX_INPUT_LENGTH]={0};
	spell_perform(OBJ_VALUE(scroll, i),
		      MEDIA_SCROLL,
		      ch, scroll, target, mstmp);
     }
   
   send_done(ch, scroll, NULL, 0, cmd, arg);

   if (OBJ_EQP_POS(scroll))
     unequip_object(scroll);
   extract_unit(scroll);
}



/* The Wand/Staff use skill */
void do_use(struct unit_data *ch, char *arg,
	    const struct command_info *cmd)
     /*
      * coded: Thu Jun 11 19:17:11 MET DST 1992
      * tested: No
      */
{
   struct unit_data *stick = NULL, *target = NULL;
   int skilla, abila, i, hm;
   
   if (str_is_empty(arg))
   {
      send_to_char("Use what?\n\r", ch);
      return;
   }
   
   /* maybe implement better errormessages here [HH] */
   
   stick = find_unit(ch, &arg, 0, FIND_UNIT_IN_ME);
   if (stick == NULL)
   {
      send_to_char("You do not have such an item.\n\r", ch);
      return;
   }
   
   if (!IS_OBJ(stick) ||
       (OBJ_TYPE(stick) != ITEM_WAND && OBJ_TYPE(stick) != ITEM_STAFF))
   {
      send_to_char("Use is used for wands and staffs.\n\r", ch);
      return;
   }
   
   if (OBJ_EQP_POS(stick) != WEAR_HOLD)
   {
      act("You do not hold it in your hand.", A_SOMEONE, ch, 0, 0, TO_CHAR);
      return;
   }
   
   skilla = IS_PC(ch) ? PC_SKI_SKILL(ch, SKI_WAND_USE) : CHAR_BRA(ch);
   if (!skilla)
   {
      send_to_char("You must practice first.\n\r", ch);
      return;
   }
   
   abila = CHAR_BRA(ch);
   hm = resistance_skill_check(2*abila, object_power(stick),
			       2*skilla, object_power(stick));
   
   if (OBJ_TYPE(stick) == ITEM_STAFF)
   {
      /* Are there any charges left? */
      if (OBJ_VALUE(stick, 1) <= 0)
      {
	 act("The $2N seems to be drained of all power.",
	     A_ALWAYS, ch, stick, 0, TO_CHAR);
	 return;
      }
      
      if (hm < 0)
      {
	 act("$1n taps $2n some times on the ground.",
	     A_HIDEINV, ch, stick, 0, TO_ROOM);
	 if (hm < -50)
	 {
	    act("You tap $2n some times on the ground and drain it of power.",
		A_SOMEONE, ch, stick, 0, TO_CHAR);
	    use_mana(stick, 1);
	 }
	 else
	   act("You tap $2n some times on the ground...Nothing happens.",
	       A_SOMEONE, ch, stick, 0, TO_CHAR);
	 return;
      }
      
      act("$1n taps $2n three times on the ground.",
	  A_HIDEINV, ch, stick, 0, TO_ROOM);
      act("You tap $2n three times on the ground.",
	  A_SOMEONE, ch, stick, 0, TO_CHAR);
      
      use_mana(stick, 1);

      for (i = 2; i < 4; i++)
	if (OBJ_VALUE(stick, i) != SPL_NONE)
	{
	   char mstmp[MAX_INPUT_LENGTH]={0};
	   spell_perform(OBJ_VALUE(stick, i),
			 MEDIA_STAFF,
			 ch, stick, NULL, mstmp);
	}
   }
   else /* Wand */
   {
      if (str_is_empty(arg))
      {
	 send_to_char("You must use it on somebody.\n\r", ch);
	 return;
      }
      
      target = find_unit(ch, &arg, 0, FIND_UNIT_HERE);
      if (!target)
      {
	 send_to_char("Can't find any such thing here.\n\r", ch);
	 return;
      }
      
      if (IS_CHAR(target))
      {
	 act("$1n points $2n at $3n.",
	     A_HIDEINV, ch, stick, target, TO_NOTVICT);
	 act("$1n points $2n at you.", A_HIDEINV, ch, stick, target, TO_VICT);
      }
      else
	act("$1n points $2n at $3n.", A_HIDEINV, ch, stick, target, TO_ROOM);
      
      act("You point $2n at $3n.", A_SOMEONE, ch, stick, target, TO_CHAR);
      
      if (hm < 0)
      {
	 if (hm < -50)
	 {
	    act("You foul up and drain power from the $2N.",
		A_HIDEINV, ch, stick, target, TO_CHAR);
	    use_mana(stick, 1);
	 }
	 else
	   act("You fail to use the $2N properly.",
	       A_HIDEINV, ch, stick, target, TO_CHAR);
	 return;
      }
      
      /* Are there any charges left? */
      if (OBJ_VALUE(stick, 1) <= 0)
      {
	 act("Nothing seems to happen with $2n.",
	     A_SOMEONE, ch, stick,0,TO_CHAR);
	 return;
      }
      
      use_mana(stick, 1);
      
      for (i = 2; i < 4; i++)
	if (OBJ_VALUE(stick, i) != SPL_NONE)
	{
	  char mbuf[MAX_INPUT_LENGTH] = {0};
	   spell_perform(OBJ_VALUE(stick, i),
			 MEDIA_WAND,
			 ch, stick, target, mbuf);
	}
   }
}


/* The APPRAISAL skill */
void do_appraise(struct unit_data *ch, char *arg,
		 const struct command_info *cmd)
{
   struct unit_data *item;
   int skilla, hm, val;
   
   if (IS_PC(ch) && PC_SKI_SKILL(ch, SKI_APPRAISAL) == 0)
   {
      send_to_char("You must practice first.\n\r", ch);
      return;
   }
   
   if (str_is_empty(arg))
   {
      send_to_char("Appraise what?\n\r", ch);
      return;
   }
   
   if ((item = find_unit(ch, &arg, 0, FIND_UNIT_HERE)) == NULL)
   {
      send_to_char("No such thing around.\n\r", ch);
      return;
   }
   
   if (!IS_OBJ(item))
   {
      act("It is not possible to estimate $3s value.",
	  A_HIDEINV, ch, 0, item, TO_CHAR);
      return;
   }
   
   skilla = IS_PC(ch) ? PC_SKI_SKILL(ch, SKI_APPRAISAL) : CHAR_BRA(ch);
   hm = resistance_skill_check(CHAR_BRA(ch), 50, skilla, 0);
   
   amount_t obj_trade_price(struct unit_data *u);

   val = obj_trade_price(item);
   
   if (hm < 0)
   {
      float f;
      
      f = -hm / 100.0;
      
      val = (int) ((float) val * f);
   }
   
   if (val < 8)
     val = 8;
   
   val = money_round_up(val, local_currency(ch), 2);
   
   act("It's probably worth $2t to the right person.",
       A_HIDEINV, ch, money_string(val, local_currency(ch), TRUE), 0, TO_CHAR);
}


/* The VENTRILOQUATE skill */
void do_ventriloquate(struct unit_data *ch, char *arg,
		      const struct command_info *cmd)
     /*
      * coded: 18/6/92 1:30:23 MET DST 1992
      * tested: No
      */
{
   struct unit_data *vict;
   int skilla, skillb, abila, abilb, hm;
   
   if (IS_PC(ch) && PC_SKI_SKILL(ch, SKI_VENTRILOQUATE) == 0)
   {
      send_to_char("You must practice first.\n\r", ch);
      return;
   }
   
   if (str_is_empty(arg))
   {
      send_to_char("Who or what should speak?\n\r", ch);
      return;
   }
   
   vict = find_unit(ch, &arg, 0, FIND_UNIT_SURRO);
   if (vict == NULL)
   {
      send_to_char("No such thing here\n\r", ch);
      return;
   }
   
   abila  = IS_CHAR(ch) ? MAX(CHAR_BRA(ch), CHAR_CHA(ch))     : 50;
   skilla = IS_PC(ch)   ? PC_SKI_SKILL(ch, SKI_VENTRILOQUATE) : abila;
   
   abilb  = IS_CHAR(vict) ? MAX(CHAR_BRA(vict), CHAR_CHA(vict))   : 50;
   skillb = IS_PC(vict)   ? PC_SKI_SKILL(vict, SKI_VENTRILOQUATE) : abilb;
   
   /* vict gets to save for the ventriloquate */
   
   hm = resistance_skill_check(abila, abilb, skilla, skillb);
   
   arg = skip_spaces(arg);
   if (hm >= 0)
   {
      act("$1n says '$2t'",        A_HIDEINV, vict, arg, ch, TO_NOTVICT);
      act("Someone says '$2t'",    A_HIDEINV, vict, arg, 0,  TO_CHAR);
      act("$1n seems to say '$2t'", A_SOMEONE, vict, arg, ch, TO_VICT);
   }
   else
   {
      act("$3n says '$2t'\n\rin a funny way.",
	  A_HIDEINV, vict, arg, ch, TO_NOTVICT);
      act("$3n says '$2t'\n\rin a funny way.",
	  A_HIDEINV, vict, arg, ch, TO_CHAR);
      act("$1n seems to say '$2t'", A_SOMEONE, vict, arg, ch, TO_VICT);
   }
}



/* The WEATHER_WATCHING skill */
void do_weather(struct unit_data *ch, char *arg,
		const struct command_info *cmd)
     /*
      * coded: Tue Jul  7 17:33:28 MET DST 1992 [HH]
      * tested: No
      */
{
   static const char *sky_look[] =
   {
      "cloudless",
      "cloudy",
      "rainy",
      "lit by flashes of lightning"
   };
   
   static char buf[100];
   int hm, chng, skilla;
   
   if (!UNIT_IS_OUTSIDE(ch) || IS_SET(UNIT_FLAGS(unit_room(ch)),
						 UNIT_FL_NO_WEATHER))
     send_to_char("You can't see the sky from here.\n\r", ch);
   else
   {
      sprintf(buf, "The sky is %s.\n\r",
	      sky_look[unit_zone(ch)->weather.sky]);
      send_to_char(buf, ch);
      
      /* Check for skill here */
      skilla = IS_PC(ch) ? PC_SKI_SKILL(ch, SKI_WEATHER_WATCH) : CHAR_BRA(ch);
      
      if (skilla > 0)
      {
	 hm = resistance_skill_check(CHAR_BRA(ch), 35, skilla, 50);
	 
	 chng = unit_zone(ch)->weather.change;
	 chng += MIN(0, hm) * chng * SGN(number(-1, 0));
	 
	 if (chng >= 0)
	   send_to_char("The weather will probably be better.\n\r", ch);
	 else
	   send_to_char("The weather will probably become worse.\n\r", ch);
      }
   }
}


void do_flee(struct unit_data *ch, char *arg,
	     const struct command_info *cmd)
{
   int legal, attempt, die, hm, opos;
   struct unit_data *predator, *u;
   
   void set_hunting(struct unit_data *p, struct unit_data *v, int legal);
   int do_simple_move(struct unit_data *ch, int direction, int following);
   
   if (CHAR_POS(ch) < POSITION_FIGHTING)
   {
      send_to_char("You are not in a proper position.\n\r", ch);
      return;
   }
   
   for (u = UNIT_CONTAINS(UNIT_IN(ch)); u; u = u->next)
     if (IS_CHAR(u) && CHAR_FIGHTING(u) == ch)
       break;
   
   /* Why?
      if (!CHAR_FIGHTING(ch) && !u)
      {
      send_to_char("Nobody is fighting you - just leave...\n\r", ch);
      return;
      }
      */
   /* SCREAM!!! NONONO!
      if (!IS_ROOM(UNIT_IN(ch)))
      {
      send_to_char("Arghh! There are no directions to flee in!!\n\r", ch);
      return;
      }
      */
   
   if (CHAR_FIGHTING(ch))
     hm = resistance_skill_check(CHAR_DEX(ch),
				 CHAR_DEX(CHAR_FIGHTING(ch)) / 2,
				 IS_PC(ch) ? PC_SKI_SKILL(ch, SKI_FLEE) :
				 CHAR_DEX(ch),
				 IS_PC(CHAR_FIGHTING(ch)) ?
				 PC_SKI_SKILL(CHAR_FIGHTING(ch), SKI_FLEE) :
				 CHAR_DEX(CHAR_FIGHTING(ch)));
   else
     hm = resistance_skill_check(CHAR_DEX(ch), 0,
				 IS_PC(ch) ? PC_SKI_SKILL(ch, SKI_FLEE) :
				 CHAR_DEX(ch), 0);
   
   die = FALSE;
   
   opos = CHAR_POS(ch);
   CHAR_POS(ch) = POSITION_STANDING;
   
   legal = IS_SET(CHAR_FLAGS(ch), CHAR_LEGAL_TARGET);
   
   /* if you are inside something you try to get out/exit */
   if (hm >= 0)
   {
      if (!IS_ROOM(UNIT_IN(ch)))
      {
	 act("$1n panics, and flees head over heels.",
	     A_HIDEINV, ch, 0, 0, TO_ROOM);
	 send_to_char("You flee head over heels!\n\r", ch);
	 char mbuf[MAX_INPUT_LENGTH] = {0};
	 do_exit(ch, mbuf, cmd);
	 return;
      }
      else if ((attempt = random_direction(ch)) != -1)
      {
	 u = UNIT_IN(ch);
	 
	 act("$1n panics, and flees head over heels.",
	     A_HIDEINV, ch, 0, 0, TO_ROOM);
	 send_to_char("You flee head over heels!\n\r", ch);
	 
	 if ((die = do_simple_move(ch, attempt, TRUE))== 1)
	 {
	    if (IS_PC(ch))
	      for (predator = UNIT_CONTAINS(u);
		   predator;predator = predator->next)
		if (IS_NPC(predator) && CHAR_FIGHTING(predator) == ch)
		  set_hunting(predator, ch, legal);
	    
	    stop_fighting(ch);
	    return;
	 }
      }
   }
   
   /* No exits were found, or you didn't make your skill */
   if (!is_destructed(DR_UNIT, ch))
   {
      CHAR_POS(ch) = opos;
      act("$1n panics, and tries to flee, but can not!",
	  A_HIDEINV, ch, 0, 0, TO_ROOM);
      send_to_char("PANIC! You couldn't escape!\n\r", ch);
   }
}


void do_sneak(struct unit_data *ch, char *arg,
	      const struct command_info *cmd)
     /*
      * coded: Fri. 19 Jun 1992 22.27.57
      * tested: No
      */
{
   int hm, skilla;
   struct unit_affected_type *oaf;
   struct unit_affected_type af;
   
   skilla = IS_PC(ch) ? PC_SKI_SKILL(ch, SKI_SNEAK) :
     (effective_dex(ch) + CHAR_BRA(ch)) / 2;
   
   if (skilla <= 0)
   {
      send_to_char("You must practice first.\n\r", ch);
      return;
   }
   
   /* Implementing individual check for sneak will be to much */
   /* trouble. Only a basic dificulty remains                 */
   /* The skill should allow you to walk out of/into a        */
   /* locations unnoticed                                     */
   if ((oaf = affected_by_spell(ch, ID_SNEAK)))
   {
      destroy_affect(oaf);
      return;
   }
   
   hm = resistance_skill_check((effective_dex(ch) + CHAR_BRA(ch)) / 2,
			       60, skilla, 50);
   
   /* is this a complete faliure? */
   if (hm < 0)
   {
      act("You make a feeble attempt to move silently.",
	  A_HIDEINV, ch, 0, 0, TO_CHAR);
      act("$1n makes a feeble attempt to move silently.",
	  A_HIDEINV, ch, 0, 0, TO_ROOM);
      return;
   }
   
   af.id       = ID_SNEAK;
   af.duration = 6;
   af.beat     = 4 * (36 * hm) / 6 + 12;
   af.data[0]  = CHAR_SNEAK;
   af.data[1]  = af.data[2] = 0;
   
   af.firstf_i = TIF_SNEAK_ON;
   af.lastf_i  = TIF_SNEAK_OFF;
   af.tickf_i  = TIF_SNEAK_TICK;
   af.applyf_i = APF_MOD_CHAR_FLAGS;
   
   create_affect(ch, &af);
}


void do_backstab(struct unit_data *ch, char *arg,
		 const struct command_info *cmd)
     /*
      * coded: Mon Jun 22 00:22:44 MET DST 1992 [HH]
      * tested: No
      */
{
   struct unit_affected_type af, *paf = NULL;
   struct unit_data *vict, *stabber;
   int skilla, skillb, hm;
   char *oarg = arg;

   if (str_is_empty(arg))
   {
      send_to_char("Backstab who?\n\r", ch);
      return;
   }
   
   skilla = IS_PC(ch) ? PC_SKI_SKILL(ch, SKI_BACKSTAB) : CHAR_BRA(ch);
   
   if (skilla == 0)
   {
      send_to_char("You must practice first.\n\r", ch);
      return;
   }
   
   if ((vict = find_unit(ch, &arg, 0, FIND_UNIT_SURRO)) == NULL)
   {
      send_to_char("Noone here by that name.\n\r", ch);
      return;
   }
   
   if (!IS_CHAR(vict))
   {
      send_to_char("But it's dead already.\n\r", ch);
      return;
   }
   
   if (vict == ch)
   {
      send_to_char("How can you sneak up on yourself?\n\r", ch);
      return;
   }
   
   if (pk_test(ch, vict, TRUE))
     return;
   
   if ((stabber = equipment_type(ch, WEAR_WIELD, ITEM_WEAPON)) == NULL)
   {
      send_to_char("You need to wield a weapon, to make it a succes.\n\r", ch);
      return;
   }
   
   if (OBJ_VALUE(stabber, 0) != WPN_DAGGER &&
       OBJ_VALUE(stabber, 0) != WPN_SHORT_SWORD &&
       OBJ_VALUE(stabber, 0) != WPN_WAKIZASHI)
   {
      send_to_char("Only small piercing weapons can be used "
		   "for backstabbing.\n\r", ch);
      return;
   }
   
   if (CHAR_FIGHTING(vict))
   {
      send_to_char("You can't backstab a fighting person, too alert!\n\r", ch);
      return;
   }
   
   if (CHAR_AWAKE(vict))
   {
      skillb = IS_PC(vict) ? PC_SKI_SKILL(vict, SKI_BACKSTAB) :
	CHAR_BRA(vict);

      /* For each recent backstab, victim gets a +50 bonus to resist. */

      if ((paf = affected_by_spell(vict, ID_BACKSTABBED)))
	skillb += 50 * paf->data[0];

      hm = resistance_skill_check(effective_dex(ch), CHAR_DEX(vict),
				  skilla, skillb);
   }
   else
     hm = resistance_skill_check(effective_dex(ch), 0,
				 skilla, 0);

   if (paf == NULL)
   {
      af.id       = ID_BACKSTABBED;
      af.duration = 15;
      af.beat     = WAIT_SEC * 60;
      af.firstf_i = TIF_NONE;
      af.tickf_i  = TIF_NONE;
      af.lastf_i  = TIF_NONE;
      af.applyf_i = APF_NONE;
      af.data[0]  = 1;
      create_affect(vict, &af);
   }
   else
   {
      paf->duration = 15;
      paf->beat     = WAIT_SEC * 60;
      paf->data[0]++;
   }

   
   if (hm < 0)
   {
      damage(ch, vict, stabber, 0,
	     MSG_TYPE_SKILL, SKI_BACKSTAB, COM_MSG_MISS);
   }
   else
   {
      int dam;
      int att_bonus;
      
      att_bonus = melee_bonus(ch, vict, hit_location(ch, vict),
			      NULL, NULL, NULL, NULL);
      
      att_bonus += open100() + hm;
      
      dam = weapon_damage(att_bonus, OBJ_VALUE(stabber, 0), ARM_CLOTHES);

      send_done(ch, stabber, vict, dam, cmd, oarg);

      damage(ch, vict, stabber, dam,
	     MSG_TYPE_SKILL, SKI_BACKSTAB, COM_MSG_EBODY);
   }
}


void do_hide(struct unit_data *ch, char *arg,
	     const struct command_info *cmd)
     /*
      * coded: Mon Jun 22 02:09:04 MET DST 1992 [HH]
      * tested: No
      */
{
   int hm, skilla;
   struct unit_affected_type *oaf, af;
   
   if (IS_PC(ch) && PC_SKI_SKILL(ch, SKI_HIDE) <= 1)
   {
      send_to_char("You must practice first.\n\r", ch);
      return;
   }
   
   if ((oaf = affected_by_spell(ch, ID_HIDE)))
   {
      destroy_affect(oaf);
      if (oaf->tickf_i == TIF_HIDE_TICK)
	act("$1n suddenly appears from somewhere.",
	    A_HIDEINV, ch, 0,0, TO_ROOM);
      else
	act("$1n stop pretending $1e's hiding.", A_HIDEINV, ch, 0, 0, TO_ROOM);
      return;
   }
   
   /* if the hide fails, you will still get messages, but they will */
   /* tell you something else eventually                            */
   
   skilla = IS_PC(ch) ? PC_SKI_SKILL(ch, SKI_HIDE):
     (CHAR_DEX(ch) + CHAR_BRA(ch)) / 2;
   
   hm = resistance_skill_check((CHAR_DEX(ch) + CHAR_BRA(ch)) / 2,
			       20, skilla, 50);
   
   af.id       = ID_HIDE;
   af.duration = 5;
   af.data[0]  = CHAR_HIDE;
   af.data[1]  = 5;  /* for the tick */
   af.data[2]  = 0;
   af.firstf_i = TIF_HIDE_ON;
   af.lastf_i  = TIF_HIDE_OFF;
   
   if (hm < 0)
   {
      hm = -hm;
      af.tickf_i  = TIF_NOHIDE_TICK;
      af.applyf_i = APF_NONE;
   }
   else
   {
      af.tickf_i  = TIF_HIDE_TICK;
      af.applyf_i = APF_MOD_CHAR_FLAGS;
   }
   
   /* this make it take longer to find out if the faliure is big */
   /* and it makes the hide take longer if the success is big    */
   af.beat     = 4 * (24 * hm) / 6 + 12;
   
   create_affect(ch, &af);
}


/* The FIRST AID skill */
void do_aid(struct unit_data *ch, char *arg,
	    const struct command_info *cmd)
{
   struct unit_data *vict;
   int skilla, hm;
   
   void modify_hit(struct unit_data *ch, int hit);
   
   skilla = IS_PC(ch)?PC_SKI_SKILL(ch, SKI_FIRST_AID):
     (CHAR_BRA(ch) + CHAR_DIV(ch)) / 2;
   if (!skilla)
   {
      send_to_char("You must practice first.\n\r", ch);
      return;
   }
   
   if (str_is_empty(arg))
   {
      send_to_char("Perform first aid on who?\n\r", ch);
      return;
   }
   
   if ((vict = find_unit(ch, &arg, 0, FIND_UNIT_SURRO)) == NULL)
   {
      send_to_char("No such person here.\n\r", ch);
      return;
   }
   
   if (!IS_CHAR(vict))
   {
      send_to_char("Surely you can't perform first aid on that?\n\r", ch);
      return;
   }
   
   if (UNIT_HIT(vict) > -1)
   {
      act("You can't do much more for $2m.", A_SOMEONE, ch, vict, 0, TO_CHAR);
      return;
   }
   
   hm = resistance_skill_check((CHAR_BRA(ch) + CHAR_DIV(ch)) / 2,
			       10, skilla, 40);
   
   if (hm >= 0)
   {
      act("You perform first aid on $2n.", A_SOMEONE, ch, vict, 0, TO_CHAR);
      send_to_char("Someone performs first aid on you.\n\r", vict);
      modify_hit(vict, 1);
   }
   else
   {
      if (hm < -50)
      {
	 send_to_char("You fumble... Real bad..\n\r", ch);
	 send_to_char("You feel someone poke in your wounds.\n\r", vict);
	 modify_hit(vict, -1);
      }
      else
      {
	 send_to_char("You fumble...\n\r", ch);
	 send_to_char("Someone tries to perform first aid on you.\n\r", vict);
      }
   }
}


void do_pick(struct unit_data *ch, char *arg,
	     const struct command_info *cmd)
{
   int skilla, hm, prot;
   struct door_data *a_door;
   char *oarg = arg;

   struct door_data *locate_lock(struct unit_data *ch, char *arg);
   
   skilla = IS_PC(ch) ? PC_SKI_SKILL(ch, SKI_PICK_LOCK) :
     (CHAR_DEX(ch) + CHAR_BRA(ch)) / 2;
   
   if (str_is_empty(arg))
   {
      send_to_char("Pick what, where?\n\r", ch);
      return;
   }
   
   if ((a_door = locate_lock(ch, arg)) == NULL)
     return;
   
   prot = spell_attack_ability(a_door->thing ? a_door->thing : UNIT_IN(ch),
			       SPL_PROTECTION);
   
   hm = resistance_skill_check((CHAR_DEX(ch)+CHAR_BRA(ch))/2,
			       prot, skilla, prot);
   
   if (!a_door->flags || !IS_SET(*a_door->flags, EX_OPEN_CLOSE))
     send_to_char("That's impossible.\n\r", ch);
   else if (!IS_SET(*a_door->flags, EX_CLOSED))
   {
      backdoor(ch, arg, cmd);

      send_to_char("Silly - it ain't even closed!\n\r", ch);
   }
   else if (skilla <= 0)
     send_to_char("You must practice first.\n\r", ch);
   else if (!a_door->key)
     send_to_char("Odd - you can't seem to find a key hole.\n\r", ch);
   else if (!IS_SET(*a_door->flags, EX_LOCKED))
     send_to_char("Oh.. it wasn't locked, after all.\n\r", ch);
   else if (IS_SET(*a_door->flags, EX_PICKPROOF))
     send_to_char("It's beyond your ability.\n\r", ch);
   else
   {
      if (hm < 0)
      {
	 act("You fail to pick the $2t.",
	     A_ALWAYS, ch, a_door->name, 0, TO_CHAR);
	 act("$1n fails to pick the $2t.",
	     A_HIDEINV,ch, a_door->name, 0, TO_ROOM);
	 return;
      }
      
      REMOVE_BIT(*a_door->flags, EX_LOCKED);
      act("You successfully pick the $2t - *click*.",
	  A_SOMEONE, ch, a_door->name, 0, TO_CHAR);
      act("$1n picks the $2t - *click*, it says.",
	  A_SOMEONE, ch, a_door->name, 0, TO_ROOM);
      
      if (a_door->reverse)
      {
	 if (UNIT_CONTAINS(a_door->reverse))
	   act("*click*",A_SOMEONE, UNIT_CONTAINS(a_door->reverse),
	       0, 0, TO_ALL);
	 
	 if (a_door->rev_flags)
	   REMOVE_BIT(*a_door->rev_flags, EX_LOCKED);
      }

      send_done(ch, NULL, a_door->thing ? a_door->thing : a_door->room,
		a_door->thing ? -1 : a_door->direction, cmd, oarg);
   }
}


void do_steal(struct unit_data *ch, char *arg,
	      const struct command_info *cmd)
     /*
      * coded:Tue Jul  7 21:10:25 MET DST 1992 [HH]
      * tested: Yup
      */
{
   struct unit_data *vict, *obj;
   char *split, *oarg = arg;
   int hm, skilla, skillb;
   
   int hands_used(struct unit_data *ch);
   
   if (str_is_empty(arg))
   {
      send_to_char("Steal what from whom?\n\r", ch);
      return;
   }
   
   if (equipment(ch, WEAR_HOLD) || hands_used(ch) >= 2)
   {
      send_to_char("You must have a free hand.\n\r", ch);
      return;
   }
   
   if ((split = str_str(arg, " from ")) == NULL)
   {
      send_to_char("You must specify FROM whom you wish to steal.\n\r", ch);
      return;
   }
   split += 6;  /* Point past from */
   
   if ((vict = find_unit(ch, &split, 0, FIND_UNIT_SURRO)) == NULL)
   {
      send_to_char("Nobody here by that name\n\r", ch);
      return;
   }
   
   /* in future, insert check here for horses */
   
   if (!IS_CHAR(vict))
   {
      send_to_char("Why not just take it?\n\r", ch);
      return;
   }
   
   if (vict == ch)
   {
      send_to_char("You'll probably get very rich this way!\n\r", ch);
      return;
   }

   if (pk_test(ch, vict, TRUE))
     return;
   
   if (CHAR_COMBAT(vict))
   {
      act("$3e is in combat and is moving around too fast!",
	  A_SOMEONE, ch, 0, vict, TO_CHAR);
      return;
   }
   
   if ((obj = find_unit(ch, &arg, UNIT_CONTAINS(vict), 0)) == NULL)
   {
      if (str_ccmp_next_word(arg, "money"))
      {
	 bool found;
	
	 /* Pick a random money-object in vict's inventory */
	 do
	 {
	    for (found = FALSE,obj = UNIT_CONTAINS(vict); obj; obj = obj->next)
	      if (IS_MONEY(obj))
	      {
		 found = TRUE;
		 if (number(0, 1))
		   break;
	      }
	 }
	 while (found && obj == NULL);
      }

      if (obj == NULL)
      {
	 act("$3e does not seem to carry such an object.",
	     A_SOMEONE, ch, 0, vict, TO_CHAR);
	 return;
      }
   }
   
   if (!IS_OBJ(obj))
   {
      send_to_char("Uhm... Surely you wouldn't try that?.\n\r", ch);
      return;
   }
   
   skilla = IS_PC(ch)   ? PC_SKI_SKILL(ch, SKI_STEAL)   : CHAR_DEX(ch);

   if (skilla == 0)
   {
      send_to_char("You must practice first.\n\r", ch);
      return;
   }
   
   skillb = IS_PC(vict) ? PC_SKI_SKILL(vict, SKI_STEAL) : CHAR_BRA(vict);
   
   /* Builtin advantage for defender */
   hm = resistance_skill_check(CHAR_DEX(ch), CHAR_BRA(vict),
			       skilla, skillb) - 10;
   
   if (OBJ_EQP_POS(obj))
     hm -= 50;  /* Equipment is MUCH harder */
   
   hm -= UNIT_WEIGHT(obj) / 4;  /* Heavy is harder */
   
   if (CHAR_POS(vict) == POSITION_SLEEPING)
     hm += 10; /* Better Chance */
   else if (CHAR_POS(vict) <= POSITION_STUNNED)
     hm = 100; /* Always success */
   
   if (IS_IMMORTAL(vict)) /* No stealing from me :) */
     hm = -100; /* Failure */
   
   if (hm < 0 || !char_can_carry_unit(ch, obj))
   {
      if (CHAR_POS(vict) == POSITION_SLEEPING)
      {
	 act("You wake up, finding $1n going through your belongings!",
	     A_SOMEONE, ch, 0, vict, TO_VICT);
	 act("$1n awakens abruptly!", A_HIDEINV, ch, 0, vict, TO_ROOM);
	 CHAR_POS(vict) = POSITION_RESTING;
      }

      if (CHAR_AWAKE(vict))
	act("$3e caught you!", A_SOMEONE, ch, 0, vict, TO_CHAR);
      else
	act("You clumsilly make everyone aware that you are trying to steal "
	    "from $3n,", A_SOMEONE, ch, 0, vict, TO_CHAR);

      if (IS_SET(UNIT_FLAGS(vict), UNIT_FL_INVISIBLE))
      {
	 struct unit_affected_type *paf;

	 act("The sudden motion makes your invisibility wear off.",
	     A_SOMEONE, ch, 0, vict, TO_CHAR);
	 while ((paf = affected_by_spell(vict, ID_INVISIBILITY)))
	   destroy_affect(paf);
	 REMOVE_BIT(UNIT_FLAGS(vict), UNIT_FL_INVISIBLE);
      }
      
      act("$1n tried to steal from you!", A_ALWAYS, ch, 0, vict, TO_VICT);
      act("$1n tries to steal something from $3n.", A_HIDEINV,
	  ch, 0, vict, TO_NOTVICT);

      log_crime(ch, vict, CRIME_STEALING);
      
      REMOVE_BIT(CHAR_FLAGS(vict), CHAR_LEGAL_TARGET);
      REMOVE_BIT(CHAR_FLAGS(ch), CHAR_SELF_DEFENCE);

      if (IS_PC(ch) && IS_PC(vict))
	slog(LOG_ALL, 0, "%s tried to steal from %s.",
	     UNIT_NAME(ch), UNIT_NAME(vict));

      provoked_attack(vict, ch);
      
      return;
   }
   
   /* Steal the item */
   if (UNIT_IS_EQUIPPED(obj))
     unequip_object(obj);
   
   if (IS_MONEY(obj))
   {
      amount_t got = (MONEY_AMOUNT(obj) * number(30, 300)) / 1000;
      
      if (IS_PC(ch) && IS_PC(vict))
	slog(LOG_ALL, 0, "%s tried to steal from %s.",
	     UNIT_NAME(ch), UNIT_NAME(vict));

      if (got > 0)
      {
	 obj = split_money(obj, got);
	 act("Bingo!  You got $2t.",
	     A_SOMEONE, ch, obj_money_string(obj, 0), 0, TO_CHAR);
      }
      else
      {
	 log_crime(ch, vict, CRIME_STEALING, FALSE);
      	 send_to_char("You couldn't get any...\n\r", ch);
	 return;
      }
   }
   
   if (IS_PC(ch) && IS_PC(vict))
     slog(LOG_ALL, 0, "%s tried to steal from %s.",
	  UNIT_NAME(ch), UNIT_NAME(vict));

   log_crime(ch, vict, CRIME_STEALING, FALSE);

   unit_from_unit(obj);
   unit_to_unit(obj, ch);
   
   if (!IS_MONEY(obj))
     send_to_char("You got it!\n\r", ch);

   send_done(ch, obj, vict, hm, cmd, oarg);
}


void base_rescue(struct unit_data *ch, struct unit_data *vict)
{
   struct unit_data *tmp_ch;
   int hm, skilla, skillb;
   
   if (vict == ch)
   {
      send_to_char("What about fleeing instead?\n\r", ch);
      return;
   }
   
   if (CHAR_FIGHTING(ch) == vict)
   {
      send_to_char("How can you rescue someone you are trying to kill?\n\r",
		   ch);
      return;
   }
   
   for (tmp_ch = UNIT_CONTAINS(UNIT_IN(ch)); tmp_ch; tmp_ch = tmp_ch->next)
     if (IS_CHAR(tmp_ch) && CHAR_FIGHTING(tmp_ch) == vict)
       break;
   
   if (tmp_ch == NULL)
   {
      act("Nobody is fighting $3m?", A_SOMEONE, ch, 0, vict, TO_CHAR);
      return;
   }
   
   skilla = IS_PC(ch) ? PC_SKI_SKILL(ch, SKI_RESCUE) :
     (CHAR_DEX(ch) + CHAR_CHA(ch)) / 2;
   
   if (IS_PC(ch) && PC_SKI_SKILL(ch, SKI_RESCUE) <= 0)
   {
      send_to_char("You'd better practice first.\n\r", ch);
      return;
   }
   
   skillb = IS_PC(tmp_ch) ? PC_SKI_SKILL(tmp_ch, SKI_RESCUE) :
     (CHAR_DEX(tmp_ch) + CHAR_CHA(tmp_ch)) / 2;
   
   hm = resistance_skill_check(CHAR_CHA(ch), CHAR_CHA(tmp_ch), skilla, skillb);
   
   if (hm < 0)
   {
      send_to_char("You bulge in from the left...\n\r"
		   "...and continues out to the right.\n\r", ch);
      act("$1n bulge in from the left...\n\r..."
	  "and continues out to the right.",
	  A_SOMEONE, ch, 0, vict, TO_VICT);
      act("$1n makes a feeble attempt to rescue $3n.",
	  A_SOMEONE, ch, 0, vict, TO_NOTVICT);
      return;
   }
   
   send_to_char("Banzai! To the rescue...\n\r", ch);
   
   act("You are rescued by $3n, you are confused!",
       A_SOMEONE, vict, 0, ch, TO_CHAR);
   act("$1n heroically rescues $3n.", A_SOMEONE, ch, 0, vict, TO_NOTVICT);
   
   if (CHAR_FIGHTING(vict) == tmp_ch)
     stop_fighting(vict, tmp_ch);
   
   if (CHAR_FIGHTING(ch))
     stop_fighting(ch, CHAR_FIGHTING(ch));

   if (IS_SET(CHAR_FLAGS(tmp_ch), CHAR_SELF_DEFENCE))
     SET_BIT(CHAR_FLAGS(ch), CHAR_SELF_DEFENCE);
   
   set_fighting(ch, tmp_ch, TRUE);
   set_fighting(tmp_ch, ch, TRUE);
   set_fighting(vict, tmp_ch, FALSE);

   if (CHAR_COMBAT(tmp_ch))
     CHAR_COMBAT(tmp_ch)->setMelee(ch);
}


void do_rescue(struct unit_data *ch, char *arg,
	       const struct command_info *cmd)
     /*
      * coded: Wed Jul  8 00:21:38 MET DST 1992
      * tested: No
      */
{
   struct unit_data *vict;
   
   if (str_is_empty(arg))
   {
      send_to_char("Rescue who?\n\r", ch);
      return;
   }
   
   vict = find_unit(ch, &arg, 0, FIND_UNIT_SURRO);
   if (vict == NULL || !IS_CHAR(vict))
   {
      send_to_char("Who do you want to rescue?\n\r", ch);
      return;
   }
   
   base_rescue(ch, vict);
}


void do_bash(struct unit_data *ch, char *arg,
	     const struct command_info *cmd)
     /*
      * coded:Wed Jul  8 00:23:59 MET DST 1992 [HH]
      * tested: No
      */
{
   struct unit_data *vict, *shield;
   int hm, att_skill, def_skill;
   char *oarg = arg;

   att_skill = IS_PC(ch) ? PC_SKI_SKILL(ch, SKI_BASH) : CHAR_STR(ch);
      
   if (att_skill <= 0)
   {
      send_to_char("You must practice first.\n\r", ch);
      return;
   }
   
   if ((vict = find_unit(ch, &arg, 0, FIND_UNIT_SURRO)) == NULL)
     vict = CHAR_FIGHTING(ch);
   
   if (vict == NULL || !IS_CHAR(vict))
   {
      send_to_char("Bash who?\n\r", ch);
      return;
   }
   
   if (vict == ch)
   {
      send_to_char("Aren't we funny today...\n\r", ch);
      return;
   }
   
   if (pk_test(ch, vict, TRUE))
     return;
   
   if ((shield = equipment_type(ch, WEAR_SHIELD, ITEM_SHIELD)) == NULL)
   {
      send_to_char("You need to use a shield, to make it a success.\n\r", ch);
      return;
   }
   
   if (UNIT_BASE_WEIGHT(vict) > 2*UNIT_BASE_WEIGHT(ch))
   {
      act("$3n is too large to be bashed!",
	  A_SOMEONE, ch, 0, vict, TO_CHAR);
      return;
   }
   
   if (!CHAR_IS_HUMANOID(vict) && !CHAR_IS_MAMMAL(vict))
   {
      act("$3n can't be bashed at all!",
	  A_SOMEONE, ch, 0, vict, TO_CHAR);
      return;
   }
   
   def_skill = IS_PC(vict) ? PC_SKI_SKILL(vict, SKI_BASH) :
                 effective_dex(vict);

   hm = resistance_skill_check(effective_dex(ch), effective_dex(vict),
			       att_skill, def_skill);
   
   if (hm < 0)
   {
      send_done(ch, shield, vict, hm, cmd, oarg);

      damage(ch, vict, shield, 0, MSG_TYPE_SKILL, SKI_BASH, COM_MSG_MISS);

      if (CHAR_COMBAT(ch))
	CHAR_COMBAT(ch)->changeSpeed(SPEED_DEFAULT);
   }
   else
   {
      send_done(ch, shield, vict, hm, cmd, oarg); 

      damage(ch, vict, shield,1, MSG_TYPE_SKILL, SKI_BASH, COM_MSG_EBODY);

      if (CHAR_COMBAT(vict))
	CHAR_COMBAT(vict)->changeSpeed(SPEED_DEFAULT);
   }
}



void do_search(struct unit_data *ch, char *arg,
	       const struct command_info *cmd)
{
   int dir, skill, i;
   struct unit_affected_type af, *taf;
   
   if (str_is_empty(arg))
   {
      send_to_char("What is the name of the exit you wish to search for?\n\r",
		   ch);
      return;
   }
   
   if (CHAR_ENDURANCE(ch) < 10)
   {
      send_to_char("You are too exhausted.\n\r", ch);
      return;
   }
   else
     CHAR_ENDURANCE(ch) -= 10;
   
   if ((dir = low_find_door(ch, arg, FALSE, FALSE)) == -1)
   {
      act("You search but find nothing.", A_SOMEONE, ch, 0, 0, TO_CHAR);
      return;
   }
   
   assert(IS_ROOM(UNIT_IN(ch)));
   
   if (!IS_SET(ROOM_EXIT(UNIT_IN(ch), dir)->exit_info, EX_HIDDEN))
   {
      act("It is not hidden!", A_SOMEONE, ch, 0, 0, TO_CHAR);
      return;
   }
   
   if (!IS_SET(ROOM_EXIT(UNIT_IN(ch), dir)->exit_info, EX_CLOSED))
   {
      act("Well, it is already open, so why search for it?", 
	  A_SOMEONE, ch, 0, 0, TO_CHAR);
      return;
   }
   
   if (!IS_PC(ch))
   {
      send_to_char("You can find everything, anytime! :)\n\r", ch);
      return;
   }
   
   skill = 25; /* Default */
   
   for (taf = UNIT_AFFECTED(UNIT_IN(ch)); taf; taf = taf->next)
     if (taf->id == ID_HIDDEN_DIFFICULTY && taf->data[0] == dir)
       skill = taf->data[1];
     else if (taf->id == ID_SPOTTED_SECRET && PC_ID(ch) == taf->data[0])
     {
	send_to_char("You have already spotted it.\n\r", ch);
	return;
     }
   
   i = resistance_skill_check(CHAR_BRA(ch), skill,
			      PC_SKI_SKILL(ch, SKI_SEARCH), skill);
   
   if (i < 0)
   {
      act("You search but find nothing.", A_SOMEONE, ch, 0, 0, TO_CHAR);
      return;
   }
   
   act("As you search you discover a hidden $2t.", A_ALWAYS, ch, 
       ROOM_EXIT(UNIT_IN(ch), dir)->open_name.Name(), 0, TO_CHAR);
   
   af.id       = ID_SPOTTED_SECRET;
   af.duration = 1;
   af.beat     = WAIT_SEC*60*10;  /* 10 minutes */
   af.firstf_i = TIF_NONE;
   af.tickf_i  = TIF_NONE;
   af.lastf_i  = TIF_NONE;
   af.applyf_i = APF_NONE;
   af.data[0]  = PC_ID(ch);
   
   create_affect(UNIT_IN(ch), &af);
}
