/* *********************************************************************** *
 * File   : act_movement.c                            Part of Valhalla MUD *
 * Version: 1.60                                                           *
 * Author : All                                                            *
 *                                                                         *
 * Purpose: Routines for moving, entering, leaving, open, close,           *
 *          positions and follow.                                          *
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

/* 23/07/92 seifert: Fixed serious simple-move bug, now tests for != SHARE */
/* 26/07/92 seifert: Split find_door into two. For use with sfun_door..    */
/* 29/07/92 seifert: Fixed bug in simple_move                              */
/* 24/09/92 gnort  : do_follow no longer assumpts char picks char as leader*/
/* 30/09/92 seifert: Various additions and bug fixes                       */
/* 02/10/92 gnort  : do_sail, in boat/obj check && changed to ||           */
/* 07/10/92 gnort  : Various additions and bug fixes                       */
/* 17/11/92 HHS    : Reinstated endurance cost                             */
/* 17/11/92 HHS    : Added simle ride func. (like sail, does nothing yet)  */
/* 23/08/93 jubal  : Fixed messages in do_sail                             */
/* 23/08/93 jubal  : Added messages to leader when start/stop follow       */
/* 23/08/93 jubal  : Fixed (nearly - still acttrouble) msgs around open etc*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
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
#include "affect.h"
#include "movement.h"
#include "constants.h"
#include "limits.h"
#include "main.h"
#include "account.h"
#include "common.h"

/*   external vars  */

extern struct unit_data *unit_list;
extern struct command_info cmd_info[];

/* external functs */

struct unit_data *get_obj_in_list_vis(struct unit_data *ch, char *name,
				      struct unit_data *list);

/* Has 'pc' found the door at 'dir'? If direction exits and it is closed  */
/* and hidden then the door is found if it has been searched for.         */
int has_found_door(struct unit_data *pc, int dir)
{
   struct unit_affected_type *af;

   if (!IS_ROOM(UNIT_IN(pc)))
     return FALSE;

   if (!ROOM_EXIT(UNIT_IN(pc), dir))
     return FALSE;

   if (!IS_PC(pc))
     return TRUE;

   if (!IS_SET(ROOM_EXIT(UNIT_IN(pc), dir)->exit_info, EX_HIDDEN))
     return TRUE;

   if (IS_SET(ROOM_EXIT(UNIT_IN(pc), dir)->exit_info, EX_CLOSED))
   {
      for (af = UNIT_AFFECTED(UNIT_IN(pc)); af; af = af->next)
	if (af->id == ID_SPOTTED_SECRET && PC_ID(pc) == af->data[0])
	  return TRUE;
      return FALSE;
   }
   else
     return TRUE;
}


int pay_point_charlie(struct unit_data *ch,
		     struct unit_data *to)
{
   if (IS_PC(ch) && g_cServerConfig.m_bAccounting && IS_MORTAL(ch))
   {
      if (CHAR_DESCRIPTOR(ch))
	if (g_cServerConfig.FromLAN(CHAR_DESCRIPTOR(ch)->host))
	  return TRUE;

      if (UNIT_FI_ZONE(to) && UNIT_FI_ZONE(to)->payonly)
      {
	 if ((UNIT_FI_ZONE(to)->payonly == 1) &&
	     (PC_ACCOUNT(ch).total_credit <= 0))
	 {
	    account_paypoint(ch);
	    return FALSE;
	 }
	 else if ((UNIT_FI_ZONE(to)->payonly == 2) &&
		  (PC_ACCOUNT(ch).flatrate < (ubit32) time(0)))
	 {
	    account_paypoint(ch);
	    return FALSE;
	 }
	 else if ((UNIT_FI_ZONE(to)->payonly == 3) &&
		  ((PC_ACCOUNT(ch).total_credit > 0) ||
		   (PC_ACCOUNT(ch).flatrate > (ubit32) time(0))))
	 {
	    account_paypoint(ch);
	    return FALSE;
	 }
      }
   }

   return TRUE;
}

/* For sailing in boats! */
int do_simple_sail(struct unit_data *boat,
		   struct unit_data *captain, int direction)

 /* Asserts:

	* boat is in a room, going in a direction n,e,s,w,u or d

	* That the direction exists. That there is no door.

	Returns :
	1 : If succes.
	0 : If fail
	-1 : If destroyed!
	*/
{
   int res;
   struct unit_data *u, *was_in,*to;

   assert(UNIT_IN(boat) && IS_OBJ(boat));
   assert(IS_ROOM(UNIT_IN(boat)));
   assert(ROOM_EXIT(UNIT_IN(boat),direction));
   assert(ROOM_EXIT(UNIT_IN(boat),direction)->to_room);


   was_in = UNIT_IN(boat);
   to     = ROOM_EXIT(was_in, direction)->to_room;

   /* Ok, the boat must always issue a special call in case some room
      might want to catch it! Not like walking! */

   char mbuf[MAX_INPUT_LENGTH] = {0};
   res = send_preprocess(boat, &cmd_info[direction], mbuf);

   for (u = UNIT_CONTAINS(boat); u; u = u->next)
     if (!pay_point_charlie(u, to))
       return 0;

   if (is_destructed(DR_UNIT, boat))
     return -1;

   if (res != SFR_SHARE || was_in != UNIT_IN(boat))
      return 0;

   unit_from_unit(boat);

   if (UNIT_CONTAINS(was_in))
     act("$2n sails $3t.",
	 A_HIDEINV, UNIT_CONTAINS(was_in), boat, dirs[direction],TO_ALL);

   if (UNIT_CONTAINS(to))
     act("$2n has arrived from $3t.", A_HIDEINV,
	 UNIT_CONTAINS(to),
	 boat, enter_dirs[rev_dir[direction]],TO_ALL);

   unit_to_unit(boat, to);

   for (u = UNIT_CONTAINS(boat); u; u=u->next)
     if (IS_CHAR(u))
     {
	act("$1n sails $2t with you.",
	    A_SOMEONE, boat, dirs[direction], u, TO_VICT);
	char mbuf[MAX_INPUT_LENGTH] = {0};
	do_look(u, mbuf, &cmd_info[direction]);
     }

   send_done(boat, captain, was_in, direction, &cmd_auto_enter, "");

   return 1;
}


/* For riding mounts! */
int do_simple_ride(struct unit_data *beast,
		   struct unit_data *master, int direction)

 /* Asserts:

	* beast is in a room, going in a direction n,e,s,w,u or d

	* That the direction exists. That there is no door.

	Returns :
	1 : If succes.
	0 : If fail
	-1 : If destroyed!
	*/
{
   int res, need_movement;
   struct unit_data *u, *was_in, *to;

   assert(UNIT_IN(beast) && IS_NPC(beast));
   assert(IS_ROOM(UNIT_IN(beast)));
   assert(ROOM_EXIT(UNIT_IN(beast),direction));
   assert(ROOM_EXIT(UNIT_IN(beast),direction)->to_room);

   was_in = UNIT_IN(beast);
   to     = ROOM_EXIT(was_in, direction)->to_room;

   char mbuf[MAX_INPUT_LENGTH] = {0};
   res = send_preprocess(beast, &cmd_info[direction], mbuf);

   for (u = UNIT_CONTAINS(beast); u; u = u->next)
     if (!pay_point_charlie(u, to))
       return 0;

   if (is_destructed(DR_UNIT, beast))
     return -1;

   if ((res != SFR_SHARE) || (was_in != UNIT_IN(beast)))
      return 0;

   if ((ROOM_LANDSCAPE(UNIT_IN(beast)) == SECT_WATER_SAIL) ||
       (ROOM_LANDSCAPE(to) == SECT_WATER_SAIL))
   {
      act("$1n refuses to go there.", A_SOMEONE, beast, 0, 0, TO_ROOM);
      return 0;
   }

   need_movement =
     (movement_loss[ROOM_LANDSCAPE(UNIT_IN(beast))]+
      movement_loss[ROOM_LANDSCAPE(to)]) / 2;

   if (CHAR_ENDURANCE(beast)<need_movement)
   {
      send_to_char("The beast is too exhausted.\n\r",UNIT_CONTAINS(beast));
      return 0;
   }

   CHAR_ENDURANCE(beast) -= need_movement;

   unit_from_unit(beast);

   if (UNIT_CONTAINS(was_in))
     act("$2n rides $3t.",
	 A_HIDEINV, UNIT_CONTAINS(was_in), beast, dirs[direction],TO_ALL);

   if (UNIT_CONTAINS(to))
     act("$2n has arrived from $3t.", A_HIDEINV,
	 UNIT_CONTAINS(to),
	 beast, enter_dirs[rev_dir[direction]],TO_ALL);

   unit_to_unit(beast, to);

   for (u = UNIT_CONTAINS(beast); u; u=u->next)
   {
      if (IS_CHAR(u))
      {
	 act("$1n rides $2t with you.",
	     A_SOMEONE, beast, dirs[direction], u, TO_VICT);
	 char mbuf[MAX_INPUT_LENGTH] = {0};
	 do_look(u, mbuf, &cmd_info[direction]);
      }
   }

   send_done(beast, master, was_in, direction, &cmd_auto_enter, "");

   return 1;
}

#define ALAS_NOWAY "Alas, you cannot go that way...\n\r"

int do_simple_move(struct unit_data *ch, int direction, int following)
     /* Asserts:
	0. ch is in a room, going in a direction n,e,s,w,u or d
	1. Does not assert anything about position.
	2. That the direction exists.
        3. Tests if the door is closed (to make follow work correctly
           after special is called!)

	Followers won't follow.

	Returns :
	1 : If succes.
	0 : If fail
	-1 : If dead.
	*/
{
   int need_movement, res;
   struct unit_data *was_in, *to, *u;
   const char *c;
   char dirbuf[2] = "c";

   assert(UNIT_IN(ch));
   assert(IS_ROOM(UNIT_IN(ch)));
   assert(ROOM_EXIT(UNIT_IN(ch),direction));
   assert(ROOM_EXIT(UNIT_IN(ch),direction)->to_room);

   was_in = UNIT_IN(ch);


   /* If we are not following, then we already issued the special
      in command interpreter! */
   if (following)
   {
      char mbuf[MAX_INPUT_LENGTH] = {0};
      res = send_preprocess(ch, &cmd_info[direction], mbuf);
      if (is_destructed(DR_UNIT, ch))
	return -1;
   }
   else
     res = SFR_SHARE;

   if ((res != SFR_SHARE) || (was_in != UNIT_IN(ch)))
      return 0;

   if (IS_SET(ROOM_EXIT(UNIT_IN(ch), direction)->exit_info, EX_CLOSED))
   {
      if (!has_found_door(ch, direction))
	send_to_char(ALAS_NOWAY, ch);
      else
	act("The $3t seems to be closed.",
	    A_SOMEONE, ch,0, ROOM_DOOR_NAME(UNIT_IN(ch), direction), TO_CHAR);
      return 0;
   }

   need_movement =
     (movement_loss[ROOM_LANDSCAPE(UNIT_IN(ch))]+
      movement_loss[ROOM_LANDSCAPE(ROOM_EXIT(UNIT_IN(ch),direction)->to_room)])
       / 2;

   if ((ROOM_LANDSCAPE(UNIT_IN(ch)) == SECT_WATER_SAIL) ||
       (ROOM_LANDSCAPE(ROOM_EXIT(UNIT_IN(ch),direction)->to_room) ==
	SECT_WATER_SAIL))
   {
	 send_to_char("You need sail in a boat to go there.\n\r", ch);
	 return 0;
   }

   if (CHAR_ENDURANCE(ch)<need_movement)
   {
      if (!following)
	send_to_char("You are too exhausted.\n\r",ch);
      else
	send_to_char("You are too exhausted to follow.\n\r",ch);

      return 0;
   }

   was_in = UNIT_IN(ch);
   to     = ROOM_EXIT(was_in, direction)->to_room;

   if (!pay_point_charlie(ch, to))
     return 0;

   for (u = UNIT_CONTAINS(ch); u; u = u->next)
     if (!pay_point_charlie(u, to))
       return 0;

   if (CHAR_LEVEL(ch) < 200)
     CHAR_ENDURANCE(ch) -= need_movement;

   dirbuf[0] = dirs[direction][0];

   c = single_unit_messg(UNIT_IN(ch),
			 "$leave_o",
			 dirbuf,
			 "$1n leaves $3t.");
   
   if (!CHAR_HAS_FLAG(ch, CHAR_SNEAK) && !str_is_empty(c))
     act(c, A_HIDEINV, ch, UNIT_IN(ch), dirs[direction], TO_ROOM);

   c = single_unit_messg(UNIT_IN(ch), "$leave_s",
				  dirbuf, "");

   if (!str_is_empty(c))
     act(c, A_ALWAYS, ch, UNIT_IN(ch), dirs[direction], TO_CHAR);
     
   unit_from_unit(ch);

   REMOVE_BIT(CHAR_FLAGS(ch), CHAR_LEGAL_TARGET | CHAR_SELF_DEFENCE);

   unit_to_unit(ch, ROOM_EXIT(was_in,direction)->to_room);

   dirbuf[0] = dirs[rev_dir[direction]][0];

   c = single_unit_messg(UNIT_IN(ch), "$arrive_o", dirbuf,
			 "$1n has arrived from $3t.");
   
   if (!CHAR_HAS_FLAG(ch, CHAR_SNEAK) && !str_is_empty(c))
     act(c, A_HIDEINV, ch, UNIT_IN(ch), enter_dirs[rev_dir[direction]],
	 TO_ROOM);

   char mbuf[MAX_INPUT_LENGTH] = {0};
   c = single_unit_messg(UNIT_IN(ch), "$arrive_s",
			 dirbuf, mbuf);

   if (!str_is_empty(c))
     act(c, A_ALWAYS, ch, UNIT_IN(ch),
	 enter_dirs[rev_dir[direction]], TO_CHAR);
     
   do_look(ch, mbuf, &cmd_info[direction]);

   send_done(ch, ch, was_in, direction, &cmd_auto_enter, "");

   return(1);
}

/* Following defaults to false. If it is set to TRUE, then it will generate
   the special to check if the move is allowed. */
int do_advanced_move(struct unit_data *ch, int direction,
		     int following)
     /*
       Returns :
       1 : If succes.
       0 : If fail
       -1 : If dead.
       */
{
   struct unit_data *was_in;
   struct char_follow_type *k;

   if (!IS_ROOM(UNIT_IN(ch)) || !ROOM_EXIT(UNIT_IN(ch), direction))
   {
      send_to_char(ALAS_NOWAY, ch);
      return 0;
   }

   /* Direction is not closed */
   if (ROOM_EXIT(UNIT_IN(ch), direction)->to_room == 0)
   {
      send_to_char(ALAS_NOWAY, ch);
      return 0;
   }

   /* Next room exists */
   if (!CHAR_MASTER(ch) && !CHAR_FOLLOWERS(ch))
     return do_simple_move(ch, direction, following);

   was_in = UNIT_IN(ch);
   int res = do_simple_move(ch, direction, following);

   if (res == 1 && (UNIT_IN(ch) != was_in))
   {
      if (CHAR_FOLLOWERS(ch))
      {	
	 int i, j;
	    
	 for (i=0;;i++) /* This shit is needed because the follow  */
	 {		   /* structure can be destroyed by this move */
	    for (j=0,k = CHAR_FOLLOWERS(ch); k && j < i; j++, k = k->next)
	      ;
	      
	    if (k == NULL)
	      break;

	    if (was_in == UNIT_IN(k->follower) &&
		CHAR_POS(k->follower) >= POSITION_STANDING)
	    {
	       act("You follow $3n.\n\r", A_SOMEONE,
		   k->follower, 0, ch, TO_CHAR);
	       do_advanced_move(k->follower, direction, TRUE);
	    }
	 }
      }
   }

   return res;
}


void do_drag(struct unit_data *ch, char *aaa,
	     const struct command_info *cmd)
{
   char *argument = (char *) aaa;
   struct unit_data *thing;
   char arg[255];
   int direction;

   /* find unit to drag */
   if (str_is_empty(argument))
   {
      send_to_char("Drag what in which direction?\n\r",ch);
      return;
   }

   if (!(thing = find_unit(ch,&argument,0,FIND_UNIT_SURRO)))
   {
      send_to_char("No such thing here.\n\r", ch);
      return;
   }

   if (!IS_CHAR(thing) && !IS_SET(UNIT_MANIPULATE(thing), MANIPULATE_TAKE))
   {
      send_to_char("You can't move it.\n\r", ch);
      return;
   }

   /* calculate if drag is posible */
   if (IS_CHAR(thing) && (CHAR_POS(thing)>POSITION_SLEEPING))
   {
      act("Maybe $2e would object to that.",A_SOMEONE,ch,thing,0,TO_CHAR);
      return;
   }

   if (!char_can_drag_w(ch, UNIT_WEIGHT(thing)))
   {
      send_to_char("You can't drag that much weight.\n\r",ch);
      return;
   }

   one_argument(argument,arg);

   if ((direction = search_block(arg, dirs, FALSE)) == -1) /* Partial Match */
   {
      send_to_char("That's not a direction.\n\r", ch);
      return;
   }

   /* move char */
   if (do_advanced_move(ch, direction, TRUE) == 1)
   {
      /* drag unit */
      if (CHAR_LEVEL(ch)<200)
	CHAR_ENDURANCE(ch) -= UNIT_WEIGHT(thing)/5;

      act("$1n is dragged away by $3n.", A_HIDEINV, thing, 0, ch,TO_NOTVICT);
      if (IS_CHAR(thing))
	send_to_char("You are dragged along.\n\r",thing);
      unit_from_unit(thing);
      unit_to_unit(thing, UNIT_IN(ch));

      act("$1n is dragged along by $3n.", A_HIDEINV, thing, 0, ch,TO_NOTVICT);
      act("You drag $3m along.", A_ALWAYS, ch, 0, thing, TO_CHAR);

      send_done(ch, NULL, thing, 0, cmd, aaa);
   }
}


void do_ride(struct unit_data *ch, char *arg,
	     const struct command_info *cmd)
{
/*
  To be expanded:
  Position of mount
  Endurance of mount
*/

   int direction;
   struct unit_data *beast, *room, *to_room;
   char buf[MAX_INPUT_LENGTH];

   beast = UNIT_IN(ch);

   if (!IS_NPC(beast))
   {
      act("You must be on a mount if you want to ride.",
	  A_ALWAYS, ch, 0, 0, TO_CHAR);
      return;
   }

   if (CHAR_FIGHTING(beast))
   {
      act("You must be in control to do that.",
	  A_ALWAYS, ch, 0, 0, TO_CHAR);
      return;
   }

   for (struct unit_data *u = UNIT_CONTAINS(UNIT_IN(ch)); u; u = u->next)   
     if (IS_CHAR(u) && CHAR_FIGHTING(u))
     {
	act("You can't just ride away in the middle of a combat.",
	    A_ALWAYS, ch, 0, 0, TO_CHAR);
	return;
     }

   one_argument(arg, buf);

   if (str_is_empty(buf))
   {
      send_to_char("Ride in which direction?\n\r", ch);
      return;
   }

   if ((direction = search_block(buf, dirs, FALSE)) == -1) /* Partial Match */
   {
      send_to_char("That's not a direction.\n\r", ch);
      return;
   }

   room = UNIT_IN(beast);

   if (!room || !IS_ROOM(room) || !ROOM_EXIT(room, direction))
   {
      send_to_char("Alas, you cannot ride that way...\n\r", ch);
      return;
   }

   /* Direction is possible */
   if (IS_SET(ROOM_EXIT(room, direction)->exit_info, EX_CLOSED))
   {
      if (!has_found_door(ch, direction))
	send_to_char(ALAS_NOWAY, ch);
      else
	act("The $3t seems to be closed.",
	    A_SOMEONE, ch,0,ROOM_DOOR_NAME(room, direction), TO_CHAR);
      return;
   }

   to_room=ROOM_EXIT(room, direction)->to_room;

   if (ROOM_LANDSCAPE(to_room) == SECT_WATER_SAIL ||
       ROOM_LANDSCAPE(to_room) == SECT_WATER_SWIM)
   {
      send_to_char("Your mount refuses to carry you there!\n\r", ch);
      return;
   }

   do_simple_ride(beast, ch, direction);
}


/* Expects 'north'..'down' as argument */
void do_sail(struct unit_data *ch, char *aaa,
	     const struct command_info *cmd)
{
   char *arg = (char *) aaa;
   int direction;
   char buf[MAX_INPUT_LENGTH];
   struct unit_data *boat, *room, *u;

   one_argument(arg, buf);

   if (str_is_empty(buf))
   {
      send_to_char("Sail in which direction?\n\r", ch);
      return;
   }

   if ((direction = search_block(buf, dirs, FALSE)) == -1) /* Partial Match */
   {
      send_to_char("That's not a direction.\n\r", ch);
      return;
   }

   boat = UNIT_IN(ch);

   if (!IS_OBJ(boat) || OBJ_TYPE(boat) != ITEM_BOAT)
   {
      act("You must be inside a boat if you want to sail.",
	  A_ALWAYS, ch, 0, 0, TO_CHAR);
      return;
   }

   room = UNIT_IN(boat);

   if (!IS_ROOM(room) || !ROOM_EXIT(room, direction))
   {
      send_to_char("Alas, you cannot sail that way...\n\r", ch);
      return;
   }

   for (u = UNIT_CONTAINS(UNIT_IN(ch)); u; u = u->next)   
     if (IS_CHAR(u) && CHAR_FIGHTING(u))
     {
	act("You can't just sail away in the middle of a combat.",
	    A_ALWAYS, ch, 0, 0, TO_CHAR);
	return;
     }

   /* Direction is possible */
   if (IS_SET(ROOM_EXIT(room, direction)->exit_info, EX_CLOSED))
   {
      backdoor(ch, arg, cmd);

      act("The $3t seems to be closed.",A_SOMEONE,ch,0,
	  ROOM_DOOR_NAME(room, direction), TO_CHAR);
      return;
   }

   /* Direction is not closed */
   if (ROOM_EXIT(room, direction)->to_room == 0)
   {
      send_to_char("Alas, you can't sail that way.\n\r", ch);
      return;
   }

   if (ROOM_LANDSCAPE(room) != SECT_WATER_SAIL  &&
       ROOM_LANDSCAPE(room) != SECT_WATER_SWIM  &&
       ROOM_LANDSCAPE(ROOM_EXIT(room, direction)->to_room)!= SECT_WATER_SAIL &&
       ROOM_LANDSCAPE(ROOM_EXIT(room, direction)->to_room) != SECT_WATER_SWIM)
   {
      send_to_char("A boat requires water to sail in!\n\r", ch);
      return;
   }

   do_simple_sail(boat, ch, direction);
}



void do_move(struct unit_data *ch, char *argument,
	     const struct command_info *cmd)
{
   /* NOTICE! It uses cmd->no for efficiency, thus cmd->no MUST */
   /* be one of CMD_NORTH..CMD_DOWN which again MUST BE 0..5    */

   assert((cmd->no >= 0) && (cmd->no <= 5));

   if (IS_OBJ(UNIT_IN(ch)) && (OBJ_TYPE(UNIT_IN(ch)) == ITEM_BOAT))
   {
      do_sail(ch, (char *) dirs[cmd->no], cmd);
      return;
   }
   else if (IS_CHAR(UNIT_IN(ch)))
   {
      do_ride(ch, (char *) dirs[cmd->no], cmd);
      return;
   }

   if (CHAR_POS(ch) < POSITION_STANDING)
   {
      if (CHAR_POS(ch) == POSITION_FIGHTING)
	send_to_char("You are fighting for your life!\n\r", ch);
      else
	send_to_char("Perhaps you should get on your feet first?\n\r", ch);
      return;
   }

   do_advanced_move(ch, cmd->no);
}



/* Returns -1 if door is not found, otherwise 0..5 for door exit     */
/* If err_msg points to anything, an appropriate message is copied   */
/* into it (which should be sent in case -1 is returned)             */
/* Will find ALL doors, including hidden doors.                      */
/* check_hidden: if FALSE, hidden doors will be ignored, otherwise   */
/*               hidden doors will be considered.                    */
/* err_msg:      if TRUE, error messages will be shown.              */

int low_find_door(struct unit_data *ch, char *doorstr,
		  int err_msg, int check_hidden)
{
   char buf[256], dir[256];
   char *dirdoorstr;
   int door;

   dirdoorstr = one_argument(doorstr, dir);

   if (str_is_empty(dir))
   {
      if (err_msg)
	act("What?", A_ALWAYS, ch, 0, 0, TO_CHAR);
      return -1;
   }

   if (!IS_ROOM(UNIT_IN(ch)))
   {
      if (err_msg)
	act("You see no such exit.", A_ALWAYS, ch, 0, 0, TO_CHAR);
      return -1;
   }

   if ((door = search_block(dir, dirs, FALSE)) != -1) /* Partial Match */
   {
      /* A direction and name was specified */

      if (str_is_empty(dirdoorstr))
      {
	 if (err_msg)
	   act("What is the name of the exit to the $2t?",
	       A_ALWAYS, ch, dirs[door], 0, TO_CHAR);
	 return -1;
      }

      if (ROOM_EXIT(UNIT_IN(ch), door))
      {
	 if (ROOM_EXIT(UNIT_IN(ch), door)->open_name.IsName(dirdoorstr) &&
	     (!check_hidden || has_found_door(ch, door)))
	   return door;

	 if (err_msg)
	   act("You see no $2t in that direction.",
	       A_ALWAYS, ch, dirdoorstr, 0, TO_CHAR);
	 return -1;
      }

      if (err_msg)
	act("You see no exit in that direction.", A_ALWAYS, ch, 0, 0, TO_CHAR);
      return -1;
   }

   /* No direction was specified, try to locate the keyword */
   one_argument(doorstr, buf);

   for (door = 0; door <= 5; door++)
   {
      if (ROOM_EXIT(UNIT_IN(ch), door))
      {
	 if (ROOM_EXIT(UNIT_IN(ch), door)->open_name.IsName(doorstr) &&
	     (!check_hidden || has_found_door(ch, door)))
	   return door;
      }
   }

   if (err_msg)
     act("You see no $2t here.", A_ALWAYS, ch, doorstr, 0, TO_CHAR);

   return -1;
}


struct door_data *locate_lock(struct unit_data *ch, char *arg)
{
   struct unit_data *thing, *other_room, *back = NULL;
   static struct door_data a_door;
   int door;

#ifdef SUSPEKT
   /* Check if person is inside something he is trying to open */
   if (IS_SET(UNIT_OPEN_FLAGS(UNIT_IN(ch)), EX_INSIDE_OPEN) &&
       UNIT_NAMES(UNIT_IN(ch)).IsName(arg))
   {
      struct unit_data *u = UNIT_IN(ch);

      while (u && u != thing)
	u = UNIT_IN(u);

      if (u)
      {
	 a_door.reverse = UNIT_IN(thing);  // We are inside it
      }
      else
	a_door.reverse = UNIT_CONTAINS(thing);  // We are not inside it

      a_door.thing = UNIT_IN(ch);
      a_door.room = 0;
      a_door.flags = &UNIT_OPEN_FLAGS(UNIT_IN(ch));
      a_door.rev_flags = 0;
      a_door.key = UNIT_KEY(UNIT_IN(ch));
      a_door.name = UNIT_NAME(UNIT_IN(ch));

      return &a_door;
   }
#endif

   if ((thing = find_unit(ch, &arg, 0, FIND_UNIT_HERE)))
   {
      struct unit_data *u = UNIT_IN(ch);

      a_door.thing = thing;
      a_door.room = 0;
      a_door.flags = &UNIT_OPEN_FLAGS(thing);
      a_door.rev_flags = 0;
      a_door.key = UNIT_KEY(thing);
      a_door.name = UNIT_NAME(thing);

      while (u && u != thing)
	u = UNIT_IN(u);

      if (u)
      {
	 if (!IS_SET(UNIT_OPEN_FLAGS(thing), EX_INSIDE_OPEN))
	 {
	    a_door.flags = NULL;
	    a_door.key = NULL;
	 }

	 a_door.reverse = UNIT_IN(thing);  // We are inside
      }
      else
	a_door.reverse = thing;            // We are not inside

      return &a_door;
   }
   else if ((door = low_find_door(ch, arg, TRUE, TRUE)) >= 0)
   {
      a_door.direction = door;
      a_door.reverse   = 0;
      a_door.rev_flags = 0;
      a_door.thing     = 0;

      a_door.name  = ROOM_DOOR_NAME(UNIT_IN(ch), door);
      a_door.room  = UNIT_IN(ch);
      a_door.flags = &ROOM_EXIT(UNIT_IN(ch), door)->exit_info;
      a_door.key   = ROOM_EXIT(UNIT_IN(ch), door)->key;

      /* Locate other side of room */
      if ((other_room = ROOM_EXIT(UNIT_IN(ch), door)->to_room) != 0)
	if (ROOM_EXIT(other_room, rev_dir[door]) &&
	    (back = ROOM_EXIT(other_room, rev_dir[door])->to_room))
	  if (back == UNIT_IN(ch))
	  {
	     a_door.reverse = other_room;
	     a_door.rev_flags =
	       &ROOM_EXIT(other_room, rev_dir[door])->exit_info;
	  }

      return &a_door;
   }

   return 0;
}

void do_knock(struct unit_data *ch, char *argument,
	      const struct command_info *cmd)
{
   struct door_data *a_door;

   if (str_is_empty(argument))
   {
      send_to_char("Knock on what?\n\r", ch);
      return;
   }

   if (!(a_door = locate_lock(ch, argument)))
     return;

   if (a_door->thing)
   {
      act("You knock on $2m.", A_SOMEONE, ch, a_door->thing, 0, TO_CHAR);
      act("$1n knocks on $3n.", A_SOMEONE, ch, 0, a_door->thing,TO_NOTVICT);
      act("$1n knocks on you.", A_SOMEONE, ch, 0, a_door->thing, TO_VICT);

      if (a_door->reverse)
      {
	 if (!UNIT_IS_TRANSPARENT(a_door->thing) &&
	     UNIT_CONTAINS(a_door->reverse))
	 {
	    act("You hear a loud knocking from $2n.",
		A_SOMEONE, UNIT_CONTAINS(a_door->reverse),
		a_door->thing, 0, TO_ALL);
	 }
      }
   }
   else /* A room */
   {
      act("You knock on the $2t.", A_SOMEONE, ch, a_door->name, 0, TO_CHAR);
      act("$1n knocks on the $2t.", A_SOMEONE, ch, a_door->name, 0, TO_ROOM);

      if (a_door->thing)
	send_done(ch, NULL, a_door->thing, -1, cmd, "");
      else
	send_done(ch, NULL, a_door->room, a_door->direction, cmd, "");

      if (a_door->reverse && UNIT_CONTAINS(a_door->reverse))
      {
	 act("You hear a faint knocking on the $2t.",A_SOMEONE,
	     UNIT_CONTAINS(a_door->reverse), a_door->name, 0, TO_ALL);

	 if (a_door->thing)
	   send_done(ch, a_door->reverse, a_door->thing, -1, cmd, "",
		     a_door->reverse);
	 else
	   send_done(ch, a_door->reverse, a_door->room,
		     a_door->direction, cmd, "", a_door->reverse);
      }
   }
}

void do_open(struct unit_data *ch, char *aaa,
	     const struct command_info *cmd)
{
   char *argument = (char *) aaa;
   struct door_data *a_door;

   if (str_is_empty(argument))
   {
      send_to_char("Open what?\n\r", ch);
      return;
   }

   if (!(a_door = locate_lock(ch, argument)))
      return;

   if (!a_door->flags || !IS_SET(*a_door->flags, EX_OPEN_CLOSE))
     send_to_char("You can't do that.\n\r", ch);
   else if (!IS_SET(*a_door->flags, EX_CLOSED))
     send_to_char("But it's already open!\n\r", ch);
   else if (IS_SET(*a_door->flags, EX_LOCKED))
     send_to_char("It seems to be locked.\n\r", ch);
   else if (a_door->thing && IS_SET(UNIT_FLAGS(a_door->thing), UNIT_FL_BURIED))
     act("$2n has been buried, you can not open it.",
	 A_SOMEONE, ch, a_door->thing, 0, TO_CHAR);
   else
   {
      REMOVE_BIT(*a_door->flags, EX_CLOSED);
      act("You open the $2t.", A_SOMEONE, ch, a_door->name, 0, TO_CHAR);
      act("$1n opens the $2t.", A_SOMEONE, ch, a_door->name, 0, TO_ROOM);

      if (a_door->thing)
	send_done(ch, NULL, a_door->thing, -1, cmd, "");
      else
	send_done(ch, NULL, a_door->room, a_door->direction, cmd, "");

      if (a_door->reverse)
      {
	 if ((a_door->room || !UNIT_IS_TRANSPARENT(a_door->thing)) &&
	     UNIT_CONTAINS(a_door->reverse))
	   act("The $2t is opened.", A_ALWAYS, UNIT_CONTAINS(a_door->reverse),
	       a_door->name, 0, TO_ALL);

	 if (a_door->rev_flags)
	   REMOVE_BIT(*a_door->rev_flags, EX_CLOSED);

	 if (a_door->thing)
	   send_done(ch, a_door->reverse, a_door->thing, -1, cmd, "",
		     a_door->reverse);
	 else
	   send_done(ch, a_door->reverse, a_door->room,
		     a_door->direction, cmd, "", a_door->reverse);
      }
   }
}



void do_close(struct unit_data *ch, char *argument,
	      const struct command_info *cmd)
{
   struct door_data *a_door;

   if (str_is_empty(argument))
   {
      send_to_char("Close what?\n\r", ch);
      return;
   }

   if (!(a_door = locate_lock(ch, argument)))
     return;

   if (!a_door->flags || !IS_SET(*a_door->flags, EX_OPEN_CLOSE))
     send_to_char("That's impossible.\n\r", ch);
   else if (IS_SET(*a_door->flags, EX_CLOSED))
     send_to_char("But it's already closed!\n\r", ch);
   else if (a_door->thing && IS_SET(UNIT_FLAGS(a_door->thing), UNIT_FL_BURIED))
     act("$2n has been buried, you can not close it.",
	 A_SOMEONE, ch, a_door->thing, 0, TO_CHAR);
   else
   {
      SET_BIT(*a_door->flags, EX_CLOSED);
      act("You close the $2t.", A_SOMEONE, ch, a_door->name, 0, TO_CHAR);
      act("$1n closes the $2t.", A_SOMEONE, ch, a_door->name, 0, TO_ROOM);

      if (a_door->thing)
	send_done(ch, NULL, a_door->thing, -1, cmd, "");
      else
	send_done(ch, NULL, a_door->room, a_door->direction, cmd, "");

      if (a_door->reverse)
      {
	 if ((a_door->room || !UNIT_IS_TRANSPARENT(a_door->thing)) &&
	     UNIT_CONTAINS(a_door->reverse))
	   act("The $2t is closed.", A_ALWAYS, UNIT_CONTAINS(a_door->reverse),
		a_door->name, 0, TO_ALL);

	 if (a_door->rev_flags)
	   SET_BIT(*a_door->rev_flags, EX_CLOSED);

	 if (a_door->thing)
	   send_done(ch, a_door->reverse, a_door->thing, -1, cmd, "",
		     a_door->reverse);
	 else
	   send_done(ch, a_door->reverse, a_door->room,
		     a_door->direction, cmd, "", a_door->reverse);

      }
   }
}


int has_key(struct unit_data *ch, struct file_index_type *key)
{
   struct unit_data *o;

   /* Check all equipment and inventory */
   for (o = UNIT_CONTAINS(ch); o; o = o->next)
     if (UNIT_FILE_INDEX(o) == key)
       return TRUE;

   return FALSE;
}



void do_lock(struct unit_data *ch, char *argument,
	     const struct command_info *cmd)
{
   struct door_data *a_door;

   if (str_is_empty(argument))
   {
      send_to_char("Lock what?\n\r", ch);
      return;
   }

   if (!(a_door = locate_lock(ch, argument)))
     return;

   if (!a_door->flags || !IS_SET(*a_door->flags, EX_OPEN_CLOSE))
     send_to_char("That's impossible.\n\r", ch);
   else if (!IS_SET(*a_door->flags, EX_CLOSED))
     send_to_char("Maybe you should close it first...\n\r", ch);
   else if (!a_door->key)
     send_to_char("Odd - you can't seem to find a key hole.\n\r", ch);
   else if ((CHAR_LEVEL(ch) < 200) && !has_key(ch, a_door->key))
     send_to_char("You don't seem to have the proper key.\n\r", ch);
   else if (IS_SET(*a_door->flags, EX_LOCKED))
     send_to_char("It is already locked!\n\r", ch);
   else
   {
      if (!has_key(ch, a_door->key))
      {
	 act("$1n inserts $1s finger into the $2t's keyhole.",
	     A_HIDEINV, ch, a_door->name, 0, TO_ROOM);
	 act("You insert your finger into the $2t's keyhole.",
	     A_ALWAYS, ch, a_door->name, 0, TO_CHAR);
      }

      SET_BIT(*a_door->flags, EX_LOCKED);
      act("You lock the $2t - *cluck*.",
	  A_SOMEONE, ch, a_door->name, 0, TO_CHAR);
      act("$1n locks the $2t - 'cluck', it says.",
	  A_SOMEONE, ch, a_door->name, 0, TO_ROOM);

      if (a_door->thing)
	send_done(ch, NULL, a_door->thing, -1, cmd, "");
      else
	send_done(ch, NULL, a_door->room, a_door->direction, cmd, "");

      if (a_door->reverse)
      {
	 if (UNIT_CONTAINS(a_door->reverse))
	    act("You hear a faint *cluck* from $2t.", A_SOMEONE,
		UNIT_CONTAINS(a_door->reverse), a_door->name, 0, TO_ALL);

	 if (a_door->rev_flags)
	   SET_BIT(*a_door->rev_flags, EX_LOCKED);

	 if (a_door->thing)
	   send_done(ch, a_door->reverse, a_door->thing, -1, cmd, "",
		     a_door->reverse);
	 else
	   send_done(ch, a_door->reverse, a_door->room,
		     a_door->direction, cmd, "", a_door->reverse);
      }
   }
}




void do_unlock(struct unit_data *ch, char *argument,
	       const struct command_info *cmd)
{
   struct door_data *a_door;

   if (str_is_empty(argument))
   {
      send_to_char("Unlock what?\n\r", ch);
      return;
   }

   if (!(a_door = locate_lock(ch, argument)))
     return;

   if (!a_door->flags || !IS_SET(*a_door->flags, EX_OPEN_CLOSE))
     send_to_char("That's impossible.\n\r", ch);
   else if (!IS_SET(*a_door->flags, EX_CLOSED))
     send_to_char("Silly - it ain't even closed!\n\r", ch);
   else if (!a_door->key)
     send_to_char("Odd - you can't seem to find a key hole.\n\r", ch);
   else if ((CHAR_LEVEL(ch) < 200) && !has_key(ch, a_door->key))
     send_to_char("You don't seem to have the proper key.\n\r", ch);
   else if (!IS_SET(*a_door->flags, EX_LOCKED))
     send_to_char("Oh... it wasn't locked, after all.\n\r", ch);
   else
   {
      if (!has_key(ch, a_door->key))
      {
	 act("$1n inserts $1s finger into the $2t's keyhole.",
	     A_HIDEINV, ch, a_door->name, 0, TO_ROOM);
	 act("You insert your finger into the $2t's keyhole.",
	     A_ALWAYS, ch, a_door->name, 0, TO_CHAR);
      }

      REMOVE_BIT(*a_door->flags, EX_LOCKED);
      act("You unlock the $2t - *click*.",
	  A_SOMEONE, ch, a_door->name, 0, TO_CHAR);
      act("$1n unlocks the $2t - *click*, it says.",
	  A_SOMEONE, ch, a_door->name, 0, TO_ROOM);

      if (a_door->thing)
	send_done(ch, NULL, a_door->thing, -1, cmd, "");
      else
	send_done(ch, NULL, a_door->room, a_door->direction, cmd, "");

      if (a_door->reverse)
      {
	 if (UNIT_CONTAINS(a_door->reverse))
	    act("You hear a faint *click* from $2t.", A_SOMEONE,
		UNIT_CONTAINS(a_door->reverse), a_door->name, 0, TO_ALL);

	 if (a_door->rev_flags)
	    REMOVE_BIT(*a_door->rev_flags, EX_LOCKED);

	 if (a_door->thing)
	   send_done(ch, a_door->reverse, a_door->thing, -1, cmd, "",
		     a_door->reverse);
	 else
	   send_done(ch, a_door->reverse, a_door->room,
		     a_door->direction, cmd, "", a_door->reverse);
      }
   }
}



void do_enter(struct unit_data *ch, char *arg,
	      const struct command_info *cmd)
{
   int door;
   struct unit_data *thing;
   char *oarg = arg;

   const char *mnt_ent = (cmd->no == CMD_MOUNT ? "mount" : "enter");

   if (str_is_empty(arg))
   {
      act("$2t what?", A_ALWAYS, ch, mnt_ent, 0, TO_CHAR);
      return;
   }

   /* Try to find object */
   if ((thing=find_unit(ch,&arg,0,FIND_UNIT_SURRO)))
   {
      /* We have an object - can we enter? */
      if (!IS_SET(UNIT_MANIPULATE(thing),MANIPULATE_ENTER))
      {
	 act("You can not $2t $3m!", A_ALWAYS, ch, mnt_ent, thing, TO_CHAR);
	 return;
      }

      if (thing == UNIT_IN(ch))
      {
	 send_to_char("You are already there!\n\r",ch);
	 return;
      }

      if (thing == ch)
      {
	 act("You attempt to $2t yourself, but find it too difficult!",
	     A_ALWAYS, ch, mnt_ent, thing, TO_CHAR);
	 return;
      }

      if (unit_recursive(ch, thing))
      {
	 act("You must drop $3m first.",
	     A_ALWAYS, ch, mnt_ent, thing, TO_CHAR);
	 return;
      }

      /* It is legal to enter, is it closed? */
      if (IS_SET(UNIT_OPEN_FLAGS(thing),EX_CLOSED))
      {
	 send_to_char("It appears to be closed.\n\r",ch);
	 return;
      }

      /* The thing is open, can we fit? */
      if (UNIT_CONTAINING_W(thing) + UNIT_WEIGHT(ch) > UNIT_CAPACITY(thing))
      {
	 send_to_char("Theres no room for you there.\n\r",ch);
	 return;
      }

      /* Move is possible, now do it! */
      if (IS_CHAR(thing))
      {
	 if (cmd->no == CMD_ENTER)
	 {
	    send_to_char("You can't enter that?\n\r", ch);
	    return;
	 }
	 unit_messg(ch, thing, "$enter", "You mount $2n.",
		    "$1n mounts $2n.");
	 CHAR_POS(ch)=POSITION_SITTING;
      }
      else
      {
	 if (cmd->no == CMD_MOUNT)
	 {
	    send_to_char("You can't mount that?\n\r", ch);
	    return;
	 }
	 unit_messg(ch, thing, "$enter", "You enter $2n.",
		    "$1n enters $2n.");
      }

      unit_from_unit(ch);
      unit_to_unit(ch, thing);

      char mbuf[MAX_INPUT_LENGTH] = {0};
      do_look(ch, mbuf, cmd);

      if (!UNIT_IS_TRANSPARENT(thing))
      {
	 if (IS_CHAR(thing))
	   act("$1n has mounted $3n.",
	       A_SOMEONE,ch,0,thing,TO_ROOM);
	 else
	   act("$1n has entered the $3N.",
	       A_SOMEONE,ch,0,thing,TO_ROOM);
      }
      send_done(ch, NULL, thing, 0, cmd, oarg);
   }
   else if ((door = low_find_door(ch, arg, TRUE, TRUE)) != -1)
   {
      do_advanced_move(ch, door, TRUE);
      return;
   }
   /* Otherwise low_find_door has sent an error message to character */
}


void do_exit(struct unit_data *ch, char *arg,
	     const struct command_info *cmd)
{
   struct unit_data *to_unit,*from_unit;
   char *oarg = arg;

   /* Is it meaningfull to exit */
   if (!UNIT_IN(ch))
   {
      switch (cmd->no)
      {
	case CMD_DISMOUNT:
	 send_to_char("But you are not riding anything?.\n\r", ch);
	 break;

	case CMD_EXIT:
	default:
	 send_to_char("But you are not inside anything?.\n\r",ch);
	 break;
      }
      return;
   }

   /* if you're fighting and not fleeing */
   if (CHAR_FIGHTING(ch) && (cmd->no != CMD_FLEE))
   {
      send_to_char("No way! You are fighting for your life!\n\r", ch);
      return;
   }

   /* right command for right situation */
   if (!IS_CHAR(UNIT_IN(ch)) && (cmd->no == CMD_DISMOUNT))
   {
      send_to_char("But you are not riding anything?.\n\r", ch);
      return;
   }

   if (IS_CHAR(UNIT_IN(ch)) && (cmd->no == CMD_EXIT))
   {
      send_to_char("But you are not inside anything?.\n\r", ch);
      return;
   }

   /* Is there an outside (room, toplevel) */
   if (!UNIT_IN(UNIT_IN(ch)))
   {
      send_to_char("But there is nowhere to exit to.\n\r", ch);
      if (CHAR_LEVEL(ch) == START_LEVEL)
	send_to_char("Perhaps you really meant EXITS? Try help directions\n\r",
		     ch);
      return;
   }

   if (IS_ROOM(UNIT_IN(UNIT_IN(ch))) &&
       ROOM_LANDSCAPE(UNIT_IN(UNIT_IN(ch))) == SECT_WATER_SAIL)
   {
      send_to_char("You can't exit into the water, it's too dangerous.\n\r",
		   ch);
      return;
   }

   /* Is this unit closed? */
   if (!IS_CHAR(UNIT_IN(ch)) && IS_SET(UNIT_OPEN_FLAGS(UNIT_IN(ch)),EX_CLOSED))
   {
      act("But the $3N seems to be closed.", 
	  A_HIDEINV, ch, 0, UNIT_IN(ch), TO_CHAR);
      return;
   }

   /* Can you enter the outside? Not if your container is buried, or */
   /* if the outside has no "ENTER" flags                            */
   if (IS_SET(UNIT_FLAGS(UNIT_IN(ch)),UNIT_FL_BURIED))
   {
      act("It seems like you have been buried - alive!",
	  A_SOMEONE,ch,0,0,TO_CHAR);
      return;
   }

   if (!IS_SET(UNIT_MANIPULATE(UNIT_IN(UNIT_IN(ch))),MANIPULATE_ENTER))
   {
      act("Someone seems to be blocking the exit.",A_SOMEONE,ch,0,0,TO_CHAR);
      return;
   }

   /* OK you can exit this thing */
   to_unit = UNIT_IN(UNIT_IN(ch));
   from_unit = UNIT_IN(ch);

   if (IS_CHAR(from_unit))
   {
      unit_messg(ch, from_unit, "$exit",
		 "You dismount $2n.",
		 "$1n dismounts $2n.");
      CHAR_POS(ch)=POSITION_STANDING;
   }
   else
   {
      unit_messg(ch, from_unit, "$exit",
		 "You exit the $2N.",
		 "$1n exits the $2N.");
   }

   if (!UNIT_IS_TRANSPARENT(from_unit))
   {
      unit_from_unit(ch);
      unit_to_unit(ch,to_unit);
      act("$1n emerges from $3n.",A_HIDEINV,ch,0,from_unit, TO_ROOM);
   }
   else
   {
      unit_from_unit(ch);
      unit_to_unit(ch,to_unit);
   }

   if (IS_CHAR(UNIT_IN(ch)))
     CHAR_POS(ch)=POSITION_SITTING;

   char mbuf[MAX_INPUT_LENGTH] = {0};
   do_look(ch, mbuf, cmd);
   send_done(ch, NULL, from_unit, 0, cmd, oarg);
}


void do_leave(struct unit_data *ch, char *arg,
	      const struct command_info *cmd)
{
#ifdef SUSPEKT

   if (!IS_SET(world[ch->in_room].room_flags, INDOORS))
     send_to_char("You are outside.. where do you want to go?\n\r", ch);
   else
   {
      for (door = 0; door <= 5; door++)
	if (ROOM_EXIT(UNIT_IN(ch), door))
	  if (ROOM_EXIT(UNIT_IN(ch), door)->to_room != NOWHERE)
	    if (!IS_SET(ROOM_EXIT(UNIT_IN(ch), door)->exit_info, EX_CLOSED) &&
		!IS_SET(world[ROOM_EXIT(UNIT_IN(ch),door)->to_room].room_flags,
			INDOORS))
	    {
	       do_move(ch, "", ++door);
	       return;
	    }
      send_to_char("You see no obvious exits to the outside.\n\r", ch);
   }
#else
   do_exit(ch,arg,cmd);
#endif
}


void do_stand(struct unit_data *ch, char *arg,
	      const struct command_info *cmd)
{
   /* not on a 'horse' type char */
   if (IS_CHAR(UNIT_IN(ch)))
   {
      act("Not here you don't.",A_SOMEONE, ch,0,0,TO_CHAR);
      return;
   }

   switch(CHAR_POS(ch))
   {
     case POSITION_STANDING:
      act("You are already standing.",A_SOMEONE, ch,0,0,TO_CHAR);
      break;
     case POSITION_SITTING:
      act("You stand up.", A_SOMEONE, ch,0,0,TO_CHAR);
      act("$1n clambers on $1s feet.",A_HIDEINV, ch, 0, 0, TO_ROOM);
      CHAR_POS(ch) = POSITION_STANDING;
      break;
     case POSITION_RESTING:
      act("You stop resting, and stand up.", A_SOMEONE, ch,0,0,TO_CHAR);
      act("$1n stops resting, and clambers on $1s feet.",
	  A_HIDEINV,ch,0,0,TO_ROOM);
      CHAR_POS(ch) = POSITION_STANDING;
      break;
     case POSITION_SLEEPING:
      act("You have to wake up first!", A_SOMEONE, ch, 0,0,TO_CHAR);
      break;
     case POSITION_FIGHTING:
      act("Do you not consider fighting as standing?",
	  A_SOMEONE,ch,0,0,TO_CHAR);
      break;
     default:
      act("You stop floating around, and put your feet on the ground.",
	  A_SOMEONE, ch, 0, 0, TO_CHAR);
      act("$1n stops floating around, and puts $1s feet on the ground.",
	  A_HIDEINV, ch, 0, 0, TO_ROOM);
      break;
   }
}


void do_sit(struct unit_data *ch, char *arg,
	    const struct command_info *cmd)
{
   switch(CHAR_POS(ch))
   {
     case POSITION_STANDING:
      act("You sit down.", A_SOMEONE, ch, 0,0, TO_CHAR);
      act("$1n sits down.", A_SOMEONE, ch, 0,0, TO_ROOM);
      CHAR_POS(ch) = POSITION_SITTING;
      break;
     case POSITION_SITTING:
      send_to_char("You are sitting already.\n\r", ch);
      break;
     case POSITION_RESTING:
      act("You stop resting, and sit up.", A_SOMEONE, ch,0,0,TO_CHAR);
      act("$1n stops resting.", A_HIDEINV, ch, 0,0,TO_ROOM);
      CHAR_POS(ch) = POSITION_SITTING;
      break;
     case POSITION_SLEEPING:
      act("You have to wake up first.", A_SOMEONE, ch, 0, 0, TO_CHAR);
      break;
     case POSITION_FIGHTING:
      act("Sit down while fighting? are you MAD?", A_SOMEONE, ch,0,0,TO_CHAR);
      break;
     default:
      act("You stop floating around, and sit down.", A_SOMEONE,ch,0,0,TO_CHAR);
      act("$1n stops floating around, and sits down.",
	  A_HIDEINV,ch,0,0,TO_ROOM);
      CHAR_POS(ch) = POSITION_SITTING;
      break;
   }
}


void do_rest(struct unit_data *ch, char *arg,
	     const struct command_info *cmd)
{

   switch(CHAR_POS(ch))
   {
     case POSITION_STANDING:
      act("You sit down and rest your tired bones.", A_SOMEONE,
	  ch, 0, 0, TO_CHAR);
      act("$1n sits down and rests.", A_HIDEINV, ch, 0, 0, TO_ROOM);
      CHAR_POS(ch) = POSITION_RESTING;
      break;
     case POSITION_SITTING:
      act("You rest your tired bones.", A_SOMEONE, ch, 0, 0, TO_CHAR);
      act("$1n rests.", A_HIDEINV, ch, 0, 0, TO_ROOM);
      CHAR_POS(ch) = POSITION_RESTING;
      break;
     case POSITION_RESTING:
      act("You are already resting.", A_SOMEONE, ch, 0, 0, TO_CHAR);
      break;
     case POSITION_SLEEPING:
      act("You have to wake up first.", A_SOMEONE, ch, 0, 0, TO_CHAR);
      break;
     case POSITION_FIGHTING:
      act("Rest while fighting? are you MAD?", A_SOMEONE, ch, 0, 0, TO_CHAR);
      break;
     default:
      act("You stop floating around, and stop to rest your tired bones.",
	  A_SOMEONE, ch, 0, 0, TO_CHAR);
      act("$1n stops floating around, and rests.", A_SOMEONE,ch,0,0,TO_ROOM);
      CHAR_POS(ch) = POSITION_SITTING;
    break;
   }
}


void do_sleep(struct unit_data *ch, char *arg,
	      const struct command_info *cmd)
{
   switch(CHAR_POS(ch))
   {
     case POSITION_STANDING:
     case POSITION_SITTING:
     case POSITION_RESTING:
      send_to_char("You go to sleep.\n\r", ch);
      act("$1n lies down and falls asleep.", A_HIDEINV, ch, 0, 0, TO_ROOM);
      CHAR_POS(ch) = POSITION_SLEEPING;
      break;
     case POSITION_SLEEPING:
      send_to_char("You are already sound asleep.\n\r", ch);
      break;
     case POSITION_FIGHTING:
      send_to_char("Sleep while fighting? are you MAD?\n\r", ch);
      break;
     default:
      act("You stop floating around, and lie down to sleep.",
	  A_SOMEONE, ch, 0, 0, TO_CHAR);
      act("$1n stops floating around, and lie down to sleep.",
	  A_HIDEINV, ch, 0, 0, TO_ROOM);
      CHAR_POS(ch) = POSITION_SLEEPING;
      break;
   }
}


void do_wake(struct unit_data *ch, char *arg,
	     const struct command_info *cmd)
{
   struct unit_data *tmp_char;

   if (str_is_empty(arg))
   {
      if (CHAR_POS(ch) > POSITION_SLEEPING)
	send_to_char("You are already awake.\n\r", ch);
      else
      {
	 send_to_char("You awaken, and start resting.\n\r", ch);
	 act("$1n awakens.", A_HIDEINV, ch, 0, 0, TO_ROOM);
	 CHAR_POS(ch) = POSITION_RESTING;
      }
   }
   else
   {
      if (!CHAR_AWAKE(ch))
	act("You can't wake people up if you are asleep yourself!",
	    A_SOMEONE, ch,0,0,TO_CHAR);
      else
      {
	 tmp_char = find_unit(ch, &arg, 0, FIND_UNIT_SURRO);
	 if (!tmp_char || !IS_CHAR(tmp_char))
	 {
	    send_to_char("You do not see that person here.\n\r", ch);
	    return;
	 }

	 if (tmp_char == ch)
	 {
	    act("If you want to wake yourself up, just type 'wake'",
		A_SOMEONE, ch,0,0,TO_CHAR);
	    return;
	 }
	 if (CHAR_AWAKE(tmp_char))
	 {
	    act("$3n is already awake.",A_SOMEONE,ch,0,tmp_char, TO_CHAR);
	    return;
	 }
	 if (CHAR_POS(tmp_char) < POSITION_SLEEPING)
	 {
	    act("You can't wake $3m up!",A_SOMEONE,ch,0,tmp_char, TO_CHAR);
	    return;
	 }

	 if (CHAR_POS(tmp_char) == POSITION_SLEEPING)
	 {
	    act("You wake $3m up.", A_SOMEONE, ch, 0, tmp_char, TO_CHAR);
	    act("$3n is awakened by $1n.",A_SOMEONE,ch,0,tmp_char,TO_NOTVICT);
	    CHAR_POS(tmp_char) = POSITION_RESTING;
	    act("You are awakened by $1n.",A_SOMEONE,ch,0,tmp_char,TO_VICT);
	 }
      }
   }
}



void do_follow(struct unit_data *ch, char *arg,
	       const struct command_info *cmd)
{
   struct unit_data *leader = NULL;

   void stop_follower(struct unit_data *ch);
   void add_follower(struct unit_data *ch, struct unit_data *leader);

   if (str_is_empty(arg) ||
       ((leader = find_unit(ch,&arg,0,FIND_UNIT_SURRO)) == ch))
   {
      if (CHAR_MASTER(ch))
      {
	 act("You stop following $3n.",A_SOMEONE,ch,0,CHAR_MASTER(ch),TO_CHAR);
	 if (same_surroundings(ch, CHAR_MASTER(ch)))
	   act("$1n stops following you.",
	       A_HIDEINV, ch, 0, CHAR_MASTER(ch), TO_VICT);
	 stop_following(ch);
      }
      else
	send_to_char("Who do you wish to follow?\n\r", ch);

      return;
   }

   if ((leader == NULL) || !IS_CHAR(leader))
   {
      send_to_char("You see no person by that name here!\n\r", ch);
      return;
   }

   if (CHAR_MASTER(ch))
   {
      act("You stop following $3n.",A_SOMEONE,ch,0,
	  CHAR_MASTER(ch),TO_CHAR);
      if (same_surroundings(ch, CHAR_MASTER(ch)))
	act("$1n stops following you.",A_HIDEINV,
	    ch, 0, CHAR_MASTER(ch), TO_VICT);
      stop_following(ch);
   }

   start_following(ch, leader);
   
   act("You now follow $3n.",A_SOMEONE,ch,0,leader,TO_CHAR);
   act("$1n starts following you.",A_HIDEINV,ch, 0, leader, TO_VICT);
}
