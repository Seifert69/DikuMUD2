/* *********************************************************************** *
 * File   : structs.c                                 Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : Unknown.                                                       *
 *                                                                         *
 * Purpose: Unknown.                                                       *
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

#include "structs.h"
#include "utils.h"
#include "main.h"
#include "handler.h"

int world_norooms   = 0;   /* number of rooms in the world   */
int world_noobjects = 0;   /* number of objects in the world */
int world_nochars   = 0;   /* number of chars in the world   */
int world_nonpc     = 0;   /* number of chars in the world   */
int world_nopc      = 0;   /* number of chars in the world   */
int world_nozones   = 0;   /* number of zones in the world   */

/* Descriptor stuff is in system.c */

room_direction_data::room_direction_data(void)
{
   key       = NULL;
   to_room   = NULL;
   exit_info = 0;
}


room_direction_data::~room_direction_data(void)
{
}



char_data::char_data(void)
{
   world_nochars++;

   specific.pc  = NULL;
   money        = NULL;
   descriptor   = NULL;
   Combat       = NULL;
   followers    = NULL;
   master       = NULL;
   last_room    = NULL;

   memset(&points, 0, sizeof(points));
}

char_data::~char_data(void)
{
   world_nochars--;
}


room_data::room_data(void)
{
   world_norooms++;
}

room_data::~room_data(void)
{
   world_norooms--;

   for (int i = 0; i <= 5; i++)
     if (dir_option[i])
       delete dir_option[i];
}

obj_data::obj_data(void)
{
   world_noobjects++;
   
   memset(value, 0, sizeof(value));
   cost = 0;
   cost_per_day = 0;
   flags = 0;
   type = 0;
   equip_pos = 0;
   resistance = 0;
}


obj_data::~obj_data(void)
{
   world_noobjects--;
}


pc_data::pc_data(void)
{
   world_nopc++;

   info = NULL;
   bank = NULL;
   guild = NULL;
   hometown = NULL;
   quest = NULL;

   memset(&setup, 0, sizeof(setup));
   memset(&time, 0, sizeof(setup));
   memset(&account, 0, sizeof(setup));

   guild_time = 0;
   vlvl = 0;
   id = -1;

   skill_points = 0;
   ability_points = 0;
   flags = 0;

   nr_of_crimes = 0;
   crack_attempts = 0;
   
   memset(spells, 0, sizeof(spells));
   memset(spell_lvl, 0, sizeof(spell_lvl));
   memset(spell_cost, 0, sizeof(spell_cost));

   memset(skills, 0, sizeof(skills));
   memset(skill_lvl, 0, sizeof(skill_lvl));
   memset(skill_cost, 0, sizeof(skill_cost));
   
   memset(weapons, 0, sizeof(weapons));
   memset(weapon_lvl, 0, sizeof(weapon_lvl));
   memset(weapon_cost,0, sizeof(weapon_cost));
   
   memset(ability_lvl, 0, sizeof(ability_lvl));
   memset(ability_cost, 0, sizeof(ability_cost));

   memset(conditions, 0, sizeof(conditions));

   nAccessLevel = 0;
   pwd[0] = 0;
   filename[0] = 0;
}


pc_data::~pc_data(void)
{
   world_nopc--;

   if (guild)
     free(guild);

   if (hometown)
     free(hometown);

   if (bank)
     free(bank);
   
   info->free_list();
   quest->free_list();
}


npc_data::npc_data(void)
{
   world_nonpc++;

   memset(weapons, 0, sizeof(weapons));
   memset(spells, 0, sizeof(spells));
   default_pos = POSITION_STANDING;
   flags = 0;
}

npc_data::~npc_data(void)
{
   world_nonpc--;
}


zone_type::zone_type(void)
{
   name = NULL;
   notes = NULL;
   help = NULL;
   filename = NULL;

   fi = NULL;
   ba = NULL;

   zri = NULL;
   next = NULL;

   tmpl   = NULL;
   tmplba = NULL;
   
   spmatrix = NULL;

   access = 255;
}

zone_type::~zone_type(void)
{
   if (name)
     free(name);

   if (title)
     free(title);

   if (notes)
     free(notes);

   if (help)
     free(help);

   if (filename)
     free(filename);

   struct file_index_type *p, *nextfi;

   for (p = fi; p; p = nextfi)
   {
      nextfi = p->next;
      delete p;
   }

   struct zone_reset_cmd *pzri, *nextzri;

   for (pzri = zri; pzri; pzri = nextzri)
   {
      nextzri = pzri->next;
      free(pzri);
   }

   /*
   struct diltemplate *pt, *nextpt;

   for (pt = tmpl; pt; pt = nextpt)
   {
      nextpt = pt->next;

      free(pt->prgname);
      free(pt->argt);
      free(pt->core);
      free(pt->vart);

      free(pt);
   }
*/


   // struct bin_search_type *ba;    /* Pointer to binarray of type      */
   // struct diltemplate *tmpl;      /* DIL templates in zone            */
   // struct bin_search_type *tmplba;/* Pointer to binarray of type      */
   
   free(spmatrix);
}


file_index_type::file_index_type(void)
{
   name     = NULL;
   zone     = NULL;
   next     = NULL;
   room_ptr = NULL;
}


file_index_type::~file_index_type(void)
{
   if (name)
     free(name);
}

unit_data::unit_data(ubit8 type)
{
   status    = type;
   
   data.ch   = NULL;
   func      = NULL;
   affected  = NULL;
   fi        = NULL;
   key       = NULL;
   outside   = NULL;
   inside    = NULL;
   next      = NULL;
   gnext     = NULL;
   gprevious = NULL;
   extra_descr = NULL;

   chars = 0;
   manipulate = 0;
   flags = 0;
   base_weight = 0;
   weight = 0;
   capacity = 0;
   open_flags = 0;
   light = 0;
   bright = 0;
   illum = 0;
   chars = 0;
   minv = 0;
   max_hp = 0;
   hp = 0;
   alignment = 0;
   
   if (IS_ROOM(this))
      U_ROOM(this) = new (class room_data);
   else if (IS_OBJ(this))
     U_OBJ(this) = new (class obj_data);
   else if (IS_CHAR(this))
   {
      U_CHAR(this) = new (class char_data);

      if (IS_PC(this))
	U_PC(this) = new (class pc_data);
      else
	U_NPC(this) = new (class npc_data);
   }
   else
     assert(FALSE);
}


unit_data::~unit_data(void)
{
#ifdef MEMORY_DEBUG
   extern int memory_pc_alloc;
   extern int memory_npc_alloc;
   extern int memory_obj_alloc;
   extern int memory_room_alloc;
   extern int memory_total_alloc;
   int memory_start = memory_total_alloc;
#endif

   ubit8 type;
   
   void unlink_affect(struct unit_data *u, struct unit_affected_type *af);
   
   /* Sanity due to wierd bug I saw (MS, 30/05-95) */

#ifdef DMSERVER
   extern class unit_data *unit_list;

   assert(gnext == NULL);
   assert(gprevious == NULL);
   assert(next == NULL);
   assert(unit_list != this);
#endif

   while (UNIT_FUNC(this))
     destroy_fptr(this, UNIT_FUNC(this));  /* Unlinks, no free */

   while (UNIT_AFFECTED(this))
     unlink_affect(this, UNIT_AFFECTED(this));
   
   type = UNIT_TYPE(this);
   
   /* Call functions of the unit which have any data                     */
   /* that they might want to work on.                                   */
   extra_descr->free_list();
   
   if (IS_OBJ(this))
     delete U_OBJ(this);
   else if (IS_ROOM(this))
     delete U_ROOM(this);
   else if (IS_CHAR(this))
   {
      if (IS_NPC(this))
	delete U_NPC(this);
      else
	delete U_PC(this);

      delete U_CHAR(this);
   }
   else
     assert(FALSE);
   
#ifdef MEMORY_DEBUG
   switch (type)
   {
     case UNIT_ST_PC:
      memory_pc_alloc -= memory_total_alloc - memory_start;
      break;
     case UNIT_ST_NPC:
      memory_npc_alloc -= memory_total_alloc - memory_start;
      break;
     case UNIT_ST_OBJ:
      memory_obj_alloc -= memory_total_alloc - memory_start;
      break;
     case UNIT_ST_ROOM:
      memory_room_alloc -= memory_total_alloc - memory_start;
      break;
   }
#endif
}
