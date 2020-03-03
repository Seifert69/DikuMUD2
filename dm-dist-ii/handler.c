/* *********************************************************************** *
 * File   : handler.c                                 Part of Valhalla MUD *
 * Version: 2.24                                                           *
 * Author : seifert@diku.dk and quinn@diku.dk                              *
 *                                                                         *
 * Purpose: Basic routines for handling units.                             *
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

/* 16/07/92 HHS+MS: Fixed bug in weight_change_unit                        */
/* 19/07/92 MS fixed sanity check in handler unit_down                     */
/* 19/07/92 MS made recusive from/to check routine                         */
/* 23/07/92 Included destruct code to avoid grave errors                   */
/* 26/07/92 seifert: Corrected error in extract unit - no longer recursive */
/* 02/09/92 seifert: Added unequip_object (speed)                          */
/* 13/10/92 seifert: Fixed unequip and object affects                      */
/* 4/6/93  HHS: Fixed light for transperant objects                        */
/* 29/08/93 jubal  : Find_unit doesn't start count at each area            */
/* 16/03/94 seifert : Find_unit SURRO can find rooms when !CAN_SEE         */
/* 06/04/94 seifert : Fixed bug in Find_unit to do with canoe bug          */
/* 21/08/94 gnort   : Added check and pile for money in unit_up/down etc   */
/* 14/09/94 gnort   : Made unit_vector dynamic rather than static (blegh)  */
/* 11/01/95 gnort   : Moved find_raw_ex_descr() from act_info.c over here  */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <time.h>

#include "utils.h"
#include "textutil.h"
#include "skills.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "interpreter.h"
#include "affect.h"
#include "utility.h"
#include "blkfile.h"
#include "money.h"
#include "files.h"
#include "unixshit.h"
#include "main.h"
#include "dilrun.h"

extern struct unit_data *unit_list;
extern struct unit_data *combat_list;
extern struct descriptor_data *descriptor_list;

/* External procedures */

void stop_special(struct unit_data *u, struct unit_fptr *fptr);

struct descriptor_data *unit_is_edited(struct unit_data *u)
{
   struct descriptor_data *d;

   for (d=descriptor_list; d; d=d->next)
     if (d->editing == u)
       return d;

   return NULL;
}

/* By using this, we can easily sort the list if ever needed */
void insert_in_unit_list(struct unit_data *u)
{
   struct unit_data *tmp_u;

   tmp_u = unit_list;

   if (!IS_PC(u))
   {     
      for (; tmp_u && IS_PC(tmp_u); tmp_u = tmp_u->gnext)
	;
   }

   if ((tmp_u==NULL) || (tmp_u == unit_list)) /* Head of list */
   {
      u->gnext = unit_list;
      u->gprevious = NULL;
      if (unit_list)
	unit_list->gprevious = u;
      unit_list = u;
   }
   else /* Middle of list  */
   {
      u->gnext = tmp_u;
      u->gprevious = tmp_u->gprevious;
      tmp_u->gprevious->gnext = u;
      tmp_u->gprevious = u;
   }
}


/* Remove a unit from the unit_list */
void remove_from_unit_list(struct unit_data *unit)
{
   assert(unit->gprevious || unit->gnext || (unit_list == unit));

   if (unit_list == unit)
     unit_list = unit->gnext;
   else  /* Then this is always true 'if (unit->gprevious)'  */
     unit->gprevious->gnext = unit->gnext;

   if (unit->gnext)
     unit->gnext->gprevious = unit->gprevious;

   unit->gnext = unit->gprevious = NULL;
}



struct unit_fptr *find_fptr(struct unit_data *u, ubit16 idx)
{
   struct unit_fptr *tf;

   for (tf = UNIT_FUNC(u); tf; tf = tf->next)
     if (tf->index == idx)
       return tf;

   return NULL;
}


struct unit_fptr *create_fptr(struct unit_data *u, ubit16 index,
			      ubit16 beat, ubit16 flags, void *data)
{
   struct unit_fptr *f;

   void start_special(struct unit_data *u, struct unit_fptr *fptr);

   CREATE(f, struct unit_fptr, 1);
   assert(f);
   assert(!is_destructed(DR_FUNC, f));

   f->index = index;
   f->heart_beat = beat;
   f->flags = flags;
   f->data  = data;

   f->next  = UNIT_FUNC(u);
   UNIT_FUNC(u) = f;

   start_special(u, f);

   return f;
}


/* Does not free 'f' - it is done by clear_destruct by comm.c */
void destroy_fptr(struct unit_data *u, struct unit_fptr *f)
{
   struct unit_fptr *tf;
   struct spec_arg sarg;

   extern struct unit_function_array_type unit_function_array[];
   extern struct command_info cmd_auto_extract;

   void register_destruct(int i, void *ptr);
   void add_func_history(struct unit_data *u, ubit16, ubit16);

   assert(f);
   assert(!is_destructed(DR_FUNC, f));

   register_destruct(DR_FUNC, f);

#ifdef DEBUG_HISTORY
   add_func_history(u, f->index, 0);
#endif

   sarg.owner     = (struct unit_data *) u;
   sarg.activator = NULL;
   sarg.medium    = NULL;
   sarg.target    = NULL;
   sarg.pInt      = NULL;
   sarg.fptr      = f;
   sarg.cmd       = &cmd_auto_extract;
   sarg.arg       = "";
   sarg.mflags    = ((ubit16) 0);

   (*(unit_function_array[f->index].func)) (&sarg);

   /* Data is free'ed in destruct() if it is not NULL now */

   stop_special((struct unit_data *) u, f);

   /* Only unlink function, do not free it! */
   if (UNIT_FUNC(u) == f)
      UNIT_FUNC(u) = f->next;
   else
   {
      for (tf = UNIT_FUNC(u); tf && (tf->next != f); tf = tf->next)   ;
      if (tf)
      {
	 assert(tf->next == f);
	 tf->next = f->next;
      }
   }
}


/* Stop the 'ch' from following his master    */
/* Call die_follower if a person dies         */
void stop_following(struct unit_data *ch)
{
   struct char_follow_type *j, *k;

   extern struct command_info *cmd_follow;

   assert(CHAR_MASTER(ch));

   if (CHAR_FOLLOWERS(CHAR_MASTER(ch))->follower == ch) /* Head of list? */
   {
      k = CHAR_FOLLOWERS(CHAR_MASTER(ch));
      CHAR_FOLLOWERS(CHAR_MASTER(ch)) = k->next;
      free(k);
   } else { /* locate follower who is not head of list */
      for(k = CHAR_FOLLOWERS(CHAR_MASTER(ch));
	  k->next->follower!=ch; k=k->next)  ;
      j = k->next;
      k->next = j->next;
      free(j);
   }

   CHAR_MASTER(ch) = 0;

   send_done(ch, NULL, NULL, 0, cmd_follow, "");
}


/* Set 'ch' to follow leader. Circles allowed. */
void start_following(struct unit_data *ch, struct unit_data *leader)
{
   struct char_follow_type *k;

   extern struct command_info *cmd_follow;

   assert(!is_destructed(DR_UNIT, leader));
   assert(!is_destructed(DR_UNIT, ch));

   REMOVE_BIT(CHAR_FLAGS(ch), CHAR_GROUP);
   if (CHAR_MASTER(ch))
     stop_following(ch);
   CHAR_MASTER(ch) = leader;
   CREATE(k, struct char_follow_type, 1);
   k->follower = ch;
   k->next = CHAR_FOLLOWERS(leader);
   CHAR_FOLLOWERS(leader) = k;

   send_done(ch, NULL, leader, 0, cmd_follow, "");
}



/* Called by extract_unit when a character that follows/is followed dies */
void die_follower(struct unit_data *ch)
{
   struct char_follow_type *j, *k;

   if (CHAR_MASTER(ch))
     stop_following(ch);

   for (k=CHAR_FOLLOWERS(ch); k; k=j)
   {
      j = k->next;
      stop_following(k->follower);
   }
}


/* Call this routine if you modify the brightness of a unit */
/* in order to correctly update the environment of the unit */
void modify_bright(struct unit_data *unit, int bright)
{
   struct unit_data *ext,*in;

   UNIT_BRIGHT(unit) += bright;

   if ((in=UNIT_IN(unit))) /* Light up what the unit is inside */
      UNIT_LIGHTS(in) += bright;

   if (IS_OBJ(unit) && OBJ_EQP_POS(unit))
     /* The char holding the torch light up the SAME way the torch does! */
     /* this works with the equib/unequib functions. This is NOT a case  */
     /* of transparancy.  */
      modify_bright(in,bright);
   else if (in && UNIT_IS_TRANSPARENT(in))
   {
      /* the unit is inside a transperant unit, so it lights up too */
      /* this works with actions in unit-up/down                    */
      UNIT_BRIGHT(in) += bright;
      if ((ext=UNIT_IN(in)))
      {
	 UNIT_LIGHTS(ext) += bright;
	 UNIT_ILLUM(in) += bright;
      }
   }
}


void trans_set(struct unit_data *u)
{
   struct unit_data *u2;
   int sum = 0;

   for (u2 = UNIT_CONTAINS(u); u2; u2 = u2->next)
     sum += UNIT_BRIGHT(u2);

   UNIT_ILLUM(u)   = sum;
   UNIT_BRIGHT(u) += sum;

   if (UNIT_IN(u))
     UNIT_LIGHTS(UNIT_IN(u)) += sum;
}


void trans_unset(struct unit_data *u)
{
   UNIT_BRIGHT(u) -= UNIT_ILLUM(u);

   if (UNIT_IN(u))
     UNIT_LIGHTS(UNIT_IN(u)) -= UNIT_ILLUM(u);

   UNIT_ILLUM(u)  = 0;
}


/*
void recalc_dex_red(struct unit_data *ch)
{
   struct unit_data *eq;
   int reduction;

   reduction = 0;

   for (eq = UNIT_CONTAINS(ch); eq; eq = eq->next)
     if (IS_OBJ(eq) && OBJ_EQP_POS(eq))
     {
       if (OBJ_TYPE(eq) == ITEM_ARMOR && CHAR_STR(ch) < OBJ_VALUE(eq, 0))
	 reduction += OBJ_VALUE(eq, 0) - CHAR_STR(ch);
       else if (OBJ_TYPE(eq) == ITEM_SHIELD && CHAR_STR(ch) < OBJ_VALUE(eq, 3))
	 reduction += OBJ_VALUE(eq, 3) - CHAR_STR(ch);
     }

   CHAR_DEX_RED(ch) = MIN(95, reduction);
}
*/


struct unit_data *equipment(struct unit_data *ch, ubit8 pos)
{
  struct unit_data *u;

  assert(IS_CHAR(ch));

  for (u = UNIT_CONTAINS(ch); u; u=u->next)
    if (IS_OBJ(u) && pos == OBJ_EQP_POS(u))
      return u;

  return NULL;
}


/* The following functions find armor / weapons on a person with     */
/* type checks (i.e. trash does not protect!).                       */
struct unit_data *equipment_type(struct unit_data *ch, int pos, ubit8 type)
{
   struct unit_data *obj;

   obj = equipment(ch, pos);

   if (obj && OBJ_TYPE(obj) == type)
     return obj;
   else
     return NULL;
}


void equip_char(struct unit_data *ch, struct unit_data *obj, ubit8 pos)
{
   struct unit_affected_type *af, newaf;

   assert(pos>0 && IS_OBJ(obj) && IS_CHAR(ch));
   assert(!equipment(ch, pos));
   assert(UNIT_IN(obj)==ch);  /* Must carry object in inventory */

   OBJ_EQP_POS(obj) = pos;

   /* recalc_dex_red(ch); */

   modify_bright(ch,UNIT_BRIGHT(obj));  /* Update light sources */

   for (af=UNIT_AFFECTED(obj); af; af=af->next)
     if (af->id < 0)  /* It is a transfer affect! */
     {
	newaf = *af;
	newaf.id = -newaf.id;       /* No longer a transfer    */
	newaf.duration = -1;        /* Permanent until unequip */
	create_affect(ch, &newaf);
     }
}


struct unit_data *unequip_object(struct unit_data *obj)
{
   struct unit_data *ch;
   struct unit_affected_type *af, *caf;

   ch = UNIT_IN(obj);

   assert(IS_OBJ(obj) && OBJ_EQP_POS(obj));
   assert(IS_CHAR(ch));

   OBJ_EQP_POS(obj) = 0;

   /* recalc_dex_red(ch); */

   modify_bright(ch,-UNIT_BRIGHT(obj));  /* Update light sources */

   for (af=UNIT_AFFECTED(obj); af; af=af->next)
     if (af->id < 0)  /* It is a transfer affect! */
     {
	for (caf=UNIT_AFFECTED(ch); caf; caf=caf->next)
	{
	   if ((-caf->id == af->id) &&
	       (caf->duration == -1) &&
	       (caf->data[0] == af->data[0]) &&
	       (caf->data[1] == af->data[1]) &&
	       // THIS IS NOT TESTED! (caf->data[2] == af->data[2]) &&
	       (caf->applyf_i == af->applyf_i) &&
	       (caf->firstf_i == af->firstf_i) &&
	       (caf->lastf_i == af->lastf_i) &&
	       (caf->tickf_i == af->tickf_i))
	   {
	      destroy_affect(caf);
	      break;  /* Skip inner for loop since we found the affect */
	   }
	}
     }

   return obj;
}

struct unit_data *unequip_char(struct unit_data *ch, ubit8 pos)
{
   struct unit_data *obj;

   obj = equipment(ch, pos);
   assert(obj);

   unequip_object(obj);

   return obj;
}



/* Checks if a unit which is to be put inside another unit will pass */
/* through itself. For example if Papi carries a coffin and God is   */
/* inside the coffin, when God types trans Papi this function will   */
/* return TRUE                                                       */
int unit_recursive(struct unit_data *from, struct unit_data *to)
{
  struct unit_data *u;

  for (u = to; u; u = UNIT_IN(u))
    if (u == from)
      return TRUE;

  return FALSE;
}


struct zone_type *unit_zone(const struct unit_data *unit)
{
   struct unit_data *org = (struct unit_data *) unit;

   for(; unit; unit = UNIT_IN(unit))
     if (!UNIT_IN(unit))
     {
	assert(IS_ROOM(unit));
	return UNIT_FILE_INDEX(unit)->zone;
     }

   slog(LOG_ALL, 0,"ZONE: FATAL: %s@%s IN NO ROOMS WHILE NOT A ROOM!!",
	UNIT_FI_NAME(org), UNIT_FI_ZONENAME(org));
   return NULL;
}

struct unit_data *unit_room(struct unit_data *unit)
{
   struct unit_data *org = unit;

   for(; unit; unit = UNIT_IN(unit))
     if (IS_ROOM(unit))
       return unit;

   slog(LOG_ALL, 0,"ROOM: FATAL: %s@%s IN NO ROOMS WHILE NOT A ROOM!!",
	UNIT_FI_NAME(org), UNIT_FI_ZONENAME(org));
   return 0;
}


void intern_unit_up(struct unit_data *unit, ubit1 pile)
{
  struct unit_data *u, *in, *toin, *extin;
  sbit8 bright, selfb;

  assert(UNIT_IN(unit));

  /* resolve *ALL* light!!! */
  in = UNIT_IN(unit);                 /* where to move unit up to */
  toin = UNIT_IN(in);                 /* unit around in           */
  extin = toin ? UNIT_IN(toin) : 0;   /* unit around toin         */
  bright = UNIT_BRIGHT(unit);         /* brightness inc. trans    */
  selfb = bright - UNIT_ILLUM(unit);  /* brightness excl. trans   */

  UNIT_LIGHTS(in) -= bright; /* Subtract Light */
  if (UNIT_IS_TRANSPARENT(in))
  {
    UNIT_ILLUM(in)  -= selfb;
    UNIT_BRIGHT(in) -= selfb;
  }
  else if (toin)
    UNIT_LIGHTS(toin) += bright;

  if (toin && UNIT_IS_TRANSPARENT(toin))
  {
    UNIT_BRIGHT(toin) += selfb;
    UNIT_ILLUM(toin)  += selfb;
    if (extin)
      UNIT_LIGHTS(extin) += selfb;
  }

  if (IS_CHAR(unit))
    --UNIT_CHARS(UNIT_IN(unit));

  UNIT_WEIGHT(UNIT_IN(unit)) -= UNIT_WEIGHT(unit);

  if (unit == UNIT_CONTAINS(UNIT_IN(unit)))
    UNIT_CONTAINS(UNIT_IN(unit)) = unit->next;
  else
  {
    for (u = UNIT_CONTAINS(UNIT_IN(unit)); u->next != unit; u = u->next)
      ;
    u->next = unit->next;
  }

  unit->next = NULL;

  if ((UNIT_IN(unit) = UNIT_IN(UNIT_IN(unit))))
  {
    unit->next = UNIT_CONTAINS(UNIT_IN(unit));
    UNIT_CONTAINS(UNIT_IN(unit)) = unit;
    if (IS_CHAR(unit))
      ++UNIT_CHARS(UNIT_IN(unit));
  }

  if (pile && IS_MONEY(unit) && UNIT_IN(unit))
    pile_money(unit);
}


void unit_up(struct unit_data *unit)
{
  intern_unit_up(unit, TRUE);
}


void unit_from_unit(struct unit_data *unit)
{
  while (UNIT_IN(unit))
    intern_unit_up(unit, FALSE);
}


void intern_unit_down(struct unit_data *unit, struct unit_data *to, ubit1 pile)
{
   struct unit_data *u,*in,*extin;
   sbit8 bright,selfb;
   
   assert(UNIT_IN(unit) == UNIT_IN(to));
   assert(unit != to);
   
   /* do *ALL* light here!!!! */
   in = UNIT_IN(unit);
   extin = in ? UNIT_IN(in) : 0;
   bright = UNIT_BRIGHT(unit);
   selfb = bright - UNIT_ILLUM(unit);
   
   UNIT_LIGHTS(to) += bright;
   if (UNIT_IS_TRANSPARENT(to))
   {
      UNIT_BRIGHT(to) += selfb;
      UNIT_ILLUM(to)  += selfb;
   }
   else if (in)
     UNIT_LIGHTS(in) -= bright;
   
   if (in && UNIT_IS_TRANSPARENT(in))
   {
      UNIT_BRIGHT(in) -= selfb;
      UNIT_ILLUM(in)  -= selfb;
      if (extin)
	UNIT_LIGHTS(extin) -= selfb;
   }
   
   if (UNIT_IN(unit))
   {
      if (IS_CHAR(unit))
	--UNIT_CHARS(UNIT_IN(unit));
      if (unit == UNIT_CONTAINS(UNIT_IN(unit)))
	UNIT_CONTAINS(UNIT_IN(unit)) = unit->next;
      else
      {
	 for (u = UNIT_CONTAINS(UNIT_IN(unit)); u->next != unit; u = u->next)
	   ;
	 u->next = unit->next;
      }
   }
   
   UNIT_IN(unit) = to;
   unit->next = UNIT_CONTAINS(to);
   UNIT_CONTAINS(to) = unit;
   
   if (IS_CHAR(unit))
     ++UNIT_CHARS(UNIT_IN(unit));
   UNIT_WEIGHT(to) += UNIT_WEIGHT(unit);
   
   if (pile && IS_MONEY(unit))
     pile_money(unit);
}


void unit_down(struct unit_data *unit, struct unit_data *to)
{
  intern_unit_down(unit, to, TRUE);
}


void intern_unit_to_unit(struct unit_data *unit,struct unit_data *to,ubit1 pile)
{
   assert(to);

   if (UNIT_IN(to))
     intern_unit_to_unit(unit, UNIT_IN(to), FALSE);

   intern_unit_down(unit, to, FALSE);

   if (pile && IS_MONEY(unit))
     pile_money(unit);
}


void unit_to_unit(struct unit_data *unit, struct unit_data *to)
{
   intern_unit_to_unit(unit, to, TRUE);
}


void snoop(struct unit_data *ch, struct unit_data *victim)
{
   assert(!is_destructed(DR_UNIT, victim));
   assert(!is_destructed(DR_UNIT, ch));

   assert(ch != victim);
   /*   assert(IS_PC(ch) && IS_PC(victim)); */
   assert(CHAR_DESCRIPTOR(ch) && CHAR_DESCRIPTOR(victim));
   assert(!CHAR_IS_SNOOPING(ch) && !CHAR_IS_SNOOPED(victim));
   assert(CHAR_LEVEL(CHAR_ORIGINAL(victim)) < CHAR_LEVEL(CHAR_ORIGINAL(ch)));
   /*   assert(!CHAR_IS_SWITCHED(victim)); */

   CHAR_DESCRIPTOR(ch)->snoop.snooping = victim;
   CHAR_DESCRIPTOR(victim)->snoop.snoop_by = ch;
}


/* Mode 0: Stop ch from snooping a person       */
/* Mode 1: Mode 0 + stop any person snooping ch */
void unsnoop(struct unit_data *ch, int mode)
{
   assert(CHAR_DESCRIPTOR(ch));
   assert(CHAR_IS_SNOOPING(ch) || CHAR_IS_SNOOPED(ch));

   if (CHAR_IS_SNOOPING(ch))
   {
      act("You no longer snoop $3n.",
	  A_SOMEONE,ch,0,CHAR_DESCRIPTOR(ch)->snoop.snooping,TO_CHAR);
      CHAR_DESCRIPTOR(CHAR_DESCRIPTOR(ch)->snoop.snooping)->snoop.snoop_by = 0;
      CHAR_DESCRIPTOR(ch)->snoop.snooping = 0;
   }

   if (CHAR_IS_SNOOPED(ch) && mode)
   {
      act("You no longer snoop $3n, $3e was extracted.",
	  A_SOMEONE,CHAR_DESCRIPTOR(ch)->snoop.snoop_by,0,ch,TO_CHAR);
      CHAR_DESCRIPTOR(CHAR_DESCRIPTOR(ch)->snoop.snoop_by)->snoop.snooping = 0;
      CHAR_DESCRIPTOR(ch)->snoop.snoop_by = 0;
   }
}


void switchbody(struct unit_data *ch, struct unit_data *vict)
{
   assert(CHAR_DESCRIPTOR(ch) && IS_NPC(vict));
   assert(!CHAR_DESCRIPTOR(vict));
   assert(!CHAR_IS_SNOOPING(ch) || CHAR_DESCRIPTOR(CHAR_IS_SNOOPING(ch)));
   assert(!CHAR_IS_SNOOPED(ch)  || CHAR_DESCRIPTOR(CHAR_IS_SNOOPED(ch)));
   assert(!is_destructed(DR_UNIT, vict));
   assert(!is_destructed(DR_UNIT, ch));

   CHAR_DESCRIPTOR(ch)->character = vict;

   if(IS_PC(ch))
     CHAR_DESCRIPTOR(ch)->original = ch;
   if(CHAR_IS_SNOOPING(ch))
     CHAR_DESCRIPTOR(CHAR_DESCRIPTOR(ch)->snoop.snooping)->snoop.snoop_by
       = vict;
   if(CHAR_IS_SNOOPED(ch))
     CHAR_DESCRIPTOR(CHAR_DESCRIPTOR(ch)->snoop.snoop_by)->snoop.snooping
       = vict;

   CHAR_DESCRIPTOR(vict) = CHAR_DESCRIPTOR(ch);
   CHAR_DESCRIPTOR(ch)   = NULL;
   CHAR_LAST_ROOM(vict)  = NULL;
}


void unswitchbody(struct unit_data *npc)
{
   assert(IS_NPC(npc) && CHAR_DESCRIPTOR(npc));
   assert(CHAR_IS_SWITCHED(npc));
   assert(!CHAR_IS_SNOOPING(npc) || CHAR_DESCRIPTOR(CHAR_IS_SNOOPING(npc)));
   assert(!CHAR_IS_SNOOPED(npc)  || CHAR_DESCRIPTOR(CHAR_IS_SNOOPED(npc)));

   send_to_char("You return to your original body.\n\r", npc);

   if(CHAR_IS_SNOOPING(npc))
     CHAR_DESCRIPTOR(CHAR_DESCRIPTOR(npc)->snoop.snooping)->snoop.snoop_by
       = CHAR_ORIGINAL(npc);

   if(CHAR_IS_SNOOPED(npc))
     CHAR_DESCRIPTOR(CHAR_DESCRIPTOR(npc)->snoop.snoop_by)->snoop.snooping
       = CHAR_ORIGINAL(npc);

   CHAR_DESCRIPTOR(npc)->character = CHAR_ORIGINAL(npc);
   CHAR_DESCRIPTOR(npc)->original = NULL;

   CHAR_DESCRIPTOR(CHAR_DESCRIPTOR(npc)->character) = CHAR_DESCRIPTOR(npc);
   CHAR_DESCRIPTOR(npc) = NULL;
}



/* Used when a unit is to be extracted from the game */
/* Extracts recursively                              */
void extract_unit(struct unit_data *unit)
{
   struct descriptor_data *d;

   extern struct unit_data *destroy_room;

   void register_destruct(int i, void *ptr);
   void nanny_menu(struct descriptor_data *d, char *arg);
   void stop_all_special(struct unit_data *u);

   /* Prevent recursive calling on extracted units. */
   /* This happens on for example corpses. When the */
   /* destruct_affect is called inside extract we   */
   /* got a recursive call.                         */

   if (is_destructed(DR_UNIT, unit))
     return;

   /* We can't extract rooms! Sanity, MS 300595, wierd bug... */
   assert(!IS_ROOM(unit));

   if (IS_PC(unit) && UNIT_IN(unit) && (!PC_IS_UNSAVED(unit)))
   {
      save_player(unit);
      save_player_contents(unit, TRUE);
   }
      
   DeactivateDil(unit);

   register_destruct(DR_UNIT, unit);

   if (UNIT_IS_EQUIPPED(unit))
     unequip_object(unit);

   stop_all_special(unit);
   stop_affect(unit);

   while (UNIT_CONTAINS(unit))
     extract_unit(UNIT_CONTAINS(unit));

   if (!IS_PC(unit) || UNIT_IN(unit))
   {
      while ((d = unit_is_edited(unit)))
      {
	 send_to_char("\n\rUnit was extracted, "
		      "sorry.\n\r", d->character);
	 
	 set_descriptor_fptr(d, descriptor_interpreter, FALSE);
      }

      if (IS_CHAR(unit))
      {
	 if (CHAR_IS_SWITCHED(unit))
	   unswitchbody(unit);

	 /* If the PC which is switched is extracted, then unswitch */
	 if (IS_PC(unit) && !CHAR_DESCRIPTOR(unit))
	   for (d = descriptor_list; d; d = d->next)
	     if (d->original == unit)
	     {
		unswitchbody(d->character);
		break;
	     }

	 if (CHAR_DESCRIPTOR(unit))
	 {
	    void disconnect_game(struct unit_data *pc);

	    disconnect_game(unit);
	 }

	 if (CHAR_FOLLOWERS(unit) || CHAR_MASTER(unit))
	   die_follower(unit);

	 stop_fighting(unit);

	 if (CHAR_IS_SNOOPING(unit) || CHAR_IS_SNOOPED(unit))
	   unsnoop(unit, 1);  /* Remove all snoopers */
      }

      if (UNIT_IN(unit))
	unit_from_unit(unit);

      unit_to_unit(unit, destroy_room);

      /* Otherwise find_unit will find it AFTER it has been extracted!! */
      remove_from_unit_list(unit);
   }
}



/* ***********************************************************************
   Here follows high-level versions of some earlier routines, ie functions
   which incorporate the actual player-data.
   *********************************************************************** */



/* Add weight to the unit and change everything it is in */
/* (It will not change the -basic- weight of a player)   */
void weight_change_unit(struct unit_data *unit, int weight)
{
  for (; unit; unit = UNIT_IN(unit))
    UNIT_WEIGHT(unit) += weight;
}


struct extra_descr_data *quest_add(struct unit_data *ch,
				   char *name, char *descr)
{
   const char *namelist[2];

   assert(name != NULL);
   assert(name[0]);

   namelist[0] = name;
   namelist[1] = NULL;
   
   return (PC_QUEST(ch) = PC_QUEST(ch)->add(namelist, descr));
}

/* void szonelog(char *zonename, const char *fmt, ...) */
void szonelog(struct zone_type *zone, const char *fmt, ...)
{
   char name[256];
   char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
   va_list args;
   FILE *f;

   extern char zondir[];

   time_t now = time(0);
   char *tmstr = ctime(&now);

   tmstr[strlen(tmstr) - 1] = '\0';

   va_start(args, fmt);
   vsprintf(buf, fmt, args);
   va_end(args);

   if (zone == NULL)
   {
      slog(LOG_ALL, 0, buf);
      return;
   }

   sprintf(buf2, "%s/%s", zone->name, buf);
   slog(LOG_ALL, 0, buf2);
  
   sprintf(name, "%s%s.err", zondir, zone->filename);

  if ((f = fopen_cache(name, "a")) == NULL)
    slog(LOG_ALL, 0, "Unable to append to zonelog '%s'", name);
  else
    fprintf(f, "%s :: %s\n", tmstr, buf);
}
