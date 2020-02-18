/* *********************************************************************** *
 * File   : spec_procs.c                              Part of Valhalla MUD *
 * Version: 1.43                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Special routines                                               *
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

/* 26/07/92 seifert: Added blockway, and door/unit access prevention       */
/* 30/07/92 seifert: Added rescue, teamwork, hide                          */
/* 30/07/92 seifert: Fixed exclude check from block procedures             */
/* 01/10/92 seifert: modified various routines                             */
/* 22/01/93 hhs: corrected drop bug in blow away                           */
/* 02/08/94 gnort: Fixed a bug in force_move; added support for 2nd string */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "structs.h"
#include "utils.h"
#include "textutil.h"
#include "handler.h"
#include "db.h"
#include "utility.h"
#include "files.h"
#include "common.h"
#include "spec_assign.h"

struct persist_type
{
   char name[30];
   struct file_index_type *in;
   sbit32 weight;
};

cNamelist persist_namelist;

void persist_save(struct unit_data *u, struct persist_type *pt)
{
   void basic_save_contents(const char *pFileName, struct unit_data *unit,
			    int fast, int bContainer);

   basic_save_contents(pt->name, u, FALSE, TRUE);

   pt->weight = UNIT_WEIGHT(u);
   pt->in = UNIT_FILE_INDEX(UNIT_IN(u));
}


void persist_remove(struct unit_data *u, struct persist_type *pt)
{
   remove(pt->name);
}


void persist_create(struct unit_data *u)
{
   char *c;
   struct persist_type *pt;

   CREATE(pt, struct persist_type, 1);

   strcpy(pt->name, str_cc(libdir, PERSIST_DIR));
   strcat(pt->name, "XXXXXX");

   for (int i=0; i < 10; i++)
     if ( (c = mktemp(pt->name)) )
       break;

   if (c == NULL)
     slog(LOG_ALL, 0, "MONSTER STRANGENESS: MKTEMP FAILED WITH NULL");

   assert(c  != NULL);
   assert(u  != NULL);
   assert(pt != NULL);

   create_fptr(u, SFUN_PERSIST_INTERNAL, PULSE_SEC * 30,
	       SFB_SAVE|SFB_TICK, pt);

   persist_save(u, pt);
}


void persist_recreate(struct unit_data *u, char *name)
{
   struct persist_type *pt;

   CREATE(pt, struct persist_type, 1);

   strcpy(pt->name, name);

   assert(u);
   assert(pt);

   create_fptr(u, SFUN_PERSIST_INTERNAL, PULSE_SEC * 30,
	       SFB_SAVE|SFB_TICK, pt);

   pt->weight = UNIT_WEIGHT(u);
   pt->in = UNIT_FILE_INDEX(UNIT_IN(u));
}


/* Corpses rely on this.... */
int persist_intern(struct spec_arg *sarg)
{
   struct persist_type *pt = (struct persist_type *) sarg->fptr->data;

   assert(pt);

   if (sarg->cmd->no == CMD_AUTO_EXTRACT)
   {
      persist_remove(sarg->owner, pt);
      return SFR_SHARE;
   }

   if (sarg->cmd->no == CMD_AUTO_SAVE)
   {
      /* Erase if it is saved within something else... */
      if (sarg->activator != sarg->owner)
      {
	 destroy_fptr(sarg->owner, sarg->fptr);
	 return SFR_BLOCK;
      }
   }

   if ((UNIT_WEIGHT(sarg->owner) != pt->weight) ||
       (UNIT_FILE_INDEX(UNIT_IN(sarg->owner)) != pt->in))
     persist_save(sarg->owner, pt);

   return SFR_SHARE;
}


void persist_boot(void)
{
   char name[50];
   struct unit_data *u;

   struct unit_data *base_load_contents(const char *pFileName,
					const struct unit_data *unit);

   for (ubit32 i=0; i < persist_namelist.Length(); i++)
   {
      strcpy(name, str_cc(libdir, PERSIST_DIR));
      strcat(name, persist_namelist.Name(i));

      u = base_load_contents(name, NULL);

      if (u)
	persist_recreate(u, name);
      else
	remove(name);
   }

   persist_namelist.Free();
}

