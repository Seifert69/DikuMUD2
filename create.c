/* *********************************************************************** *
 * File   : create.c                                  Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Routines for creating stuff like corpses and gold.             *
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
#include <string.h>

#include "structs.h"
#include "utils.h"
#include "skills.h"
#include "comm.h"
#include "handler.h"
#include "interpreter.h"
#include "db.h"
#include "spells.h"
#include "textutil.h"
#include "affect.h"
#include "utility.h"

extern struct unit_data *unit_list;
extern struct file_index_type *corpse_fi;


struct unit_data *make_corpse(struct unit_data *ch)
{
   char buf[MAX_INPUT_LENGTH];
   struct unit_data *corpse, *u, *next_dude;
   struct unit_affected_type af, *taf1;

   void persist_create(struct unit_data *u);

   corpse = read_unit(corpse_fi);

   sprintf(buf, STR(UNIT_OUT_DESCR_STRING(corpse)), TITLENAME(ch));
   UNIT_OUT_DESCR(corpse).Reassign(buf);

   if (IS_PC(ch))
     UNIT_NAMES(corpse).PrependName(str_cc("corpse of ", UNIT_NAME(ch)));

   UNIT_WEIGHT(corpse) = UNIT_BASE_WEIGHT(ch);
   UNIT_BASE_WEIGHT(corpse) = UNIT_WEIGHT(corpse);

   OBJ_TYPE(corpse) = ITEM_CONTAINER;
   OBJ_PRICE(corpse) = 0;
   OBJ_PRICE_DAY(corpse) = 0;

   /* *** pointers *** */

   for (u = UNIT_CONTAINS(ch); u; u = u->next)
     if (UNIT_IS_EQUIPPED(u))
       unequip_object(u);

   for (u = UNIT_CONTAINS(ch); u; u = next_dude)
   {
      next_dude = u->next;

      if (CHAR_LEVEL(ch) >= UNIT_MINV(u))
      {
	 unit_from_unit(u);
	 unit_to_unit(u, corpse);
      }
   }

   unit_to_unit(corpse, UNIT_IN(ch));

   af.id = ID_CORPSE;
   af.duration = (IS_PC(ch) ? (2*60) : 5);  /* In minutes, ply corpses 2hr */
   af.beat     = (WAIT_SEC * 60);
   af.data[0] = 0;
   af.data[1] = 0;
   af.data[2] = 0;
   af.firstf_i = TIF_NONE;
   af.tickf_i  = TIF_CORPSE_DECAY;
   af.lastf_i  = TIF_CORPSE_ZAP;
   af.applyf_i = APF_NONE;

   create_affect(corpse, &af);

   OBJ_VALUE(corpse, 2) = IS_PC(ch) ? 1 : 0;
   OBJ_VALUE(corpse, 3) = CHAR_LEVEL(ch);
   OBJ_VALUE(corpse, 4) = CHAR_RACE(ch);

   if (IS_PC(ch))
   {
      if (UNIT_CONTAINS(corpse))
	persist_create(corpse);

      if (IS_SET(PC_FLAGS(ch), PC_PK_RELAXED))
	UNIT_EXTRA_DESCR(corpse) =
	  UNIT_EXTRA_DESCR(corpse)->add("$BOB", "");
   }

   return corpse;
}
