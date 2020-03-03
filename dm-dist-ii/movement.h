/* *********************************************************************** *
 * File   : movement.h                                Part of Valhalla MUD *
 * Version: 1.50                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Header for movement.                                           *
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

#ifndef _MUD_MOVEMENT_H
#define _MUD_MOVEMENT_H

#include "dijkstra.h"
#include "interpreter.h"

#define ROOM_DOOR_NAME(room, dir)  \
  (ROOM_EXIT((room), (dir))->open_name.Name() ?                  \
    ROOM_EXIT((room), (dir))->open_name.Name() : "UNDEFINED")

void backdoor(struct unit_data *ch, char *arg,
	      const struct command_info *cmd);

int low_find_door(struct unit_data *ch, char *doorstr,
		  int err_msg, int check_hidden);

#define MOVE_GOAL    0 /* The NPC is now at it's destination         */
#define MOVE_CLOSER  1 /* The NPC was moved closer to destination    */
#define MOVE_DOOR    2 /* The NPC is working on a door/lock/room     */
#define MOVE_BUSY    3 /* The NPC is busy and can't do anything yet  */
#define MOVE_FAILED  4 /* It is currently impossible to reach desti. */
#define MOVE_DEAD    5 /* The NPC is dead, abort now!                */

#define DESTROY_ME   12

struct door_data
{
   struct unit_data *thing;
   struct unit_data *room;
   struct unit_data *reverse; /* Reverse Room or Inside thing */
   struct file_index_type *key;
   ubit8 direction; /* For rooms, which direction was picked? */
   ubit8 *flags;
   ubit8 *rev_flags;
   const char *name;
};


struct visit_data
{
   int state;
   struct unit_data *go_to;

   struct unit_data *start_room;
   struct unit_data *dest_room;

   /* Return DESTROY_ME to destroy moving function        */
   /*        SFR_SHARE to allow lower functions to handle */
   /*        SFR_SUPREME to not allow lower functions     */
   int (*what_now) (const struct unit_data *, struct visit_data *);

   void *data;
   int non_tick_return;  /* What to return upon non-ticks (SFR_...) */
};

void npc_set_visit(struct unit_data *npc, struct unit_data *dest_room,
		   int what_now(const struct unit_data *,
				struct visit_data *),
		   void *data, int non_tick_return);

int do_advanced_move(struct unit_data *ch, int direction,
		     int following = FALSE);

#endif /* _MUD_MOVEMENT_H */
