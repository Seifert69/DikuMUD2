/* *********************************************************************** *
 * File   : spec_assign.c                             Part of Valhalla MUD *
 * Version: 1.04                                                           *
 * Author : All.                                                           *
 *                                                                         *
 * Purpose: Assignment of special functions constants.                     *
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


#include <stdio.h>

#include "utils.h"
#include "interpreter.h"
#include "spec_assign.h"
#include "handler.h"

int spec_unused(struct spec_arg *sarg)
{
   if (sarg->cmd->no != CMD_AUTO_EXTRACT)
   {
      slog(LOG_ALL, 0, "Unit %s@%s had undefined special routine.",
	   UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
   }

   return SFR_SHARE;
}


/* ********************************************************************
 *  Assignments                                                       *
 ******************************************************************** */

/* macro definitions for array indices are in values.h */
struct unit_function_array_type unit_function_array[] =
{
 {"Persist Internal", persist_intern, SD_NEVER, SFB_TICK|SFB_SAVE, 0},
 {"DIL Copy Internal", dil_direct_init, SD_NEVER, 0, PULSE_SEC*5},
 {"bulletin board", board, SD_ASCII,SFB_CMD, 0},
 {"mobile command", mob_command, SD_ASCII,SFB_TICK, PULSE_SEC*30},
 {"hunting", hunting, SD_NEVER,SFB_TICK, PULSE_SEC * 15},
 {"postman", postman, SD_NULL,SFB_CMD, 0},
 {"Internal shop keeper", shop_keeper, SD_NEVER, SFB_CMD | SFB_DEAD, 0},
 {"eat letter to delete", eat_and_delete, SD_NEVER,SFB_CMD, 0},
 {"accuse", accuse, SD_NULL,SFB_CMD, 0},
 {"protect lawful", protect_lawful, SD_NULL,SFB_COM|SFB_DEAD|SFB_TICK,
   PULSE_SEC * 15},
 {"P.A.I.N.", pain_exec, SD_NEVER, SFB_CMD|SFB_TICK, PULSE_SEC*15},
 {"P.A.I.N. Init", pain_init, SD_ASCII,SFB_CMD|SFB_TICK, PULSE_SEC*1},
 {"npc visit room", npc_visit_room, SD_NEVER, SFB_TICK|SFB_PRIORITY,
   PULSE_SEC*5},
 {"mercenary hire", mercenary_hire, SD_NULL, SFB_CMD, 0},
 {"mercenary hunt", mercenary_hunt, SD_NEVER, SFB_TICK | SFB_DEAD,
   PULSE_SEC * 10 },
 {"banking routine", bank, SD_NULL, SFB_CMD, 0},
 {"XXX UNUSED", spec_unused, SD_NULL, SFB_ALL, 0},
 {"beastly fido", fido, SD_NULL,SFB_TICK, PULSE_SEC * 30},
 {"janitor", janitor, SD_NULL,SFB_TICK, PULSE_SEC * 30},

 {"Chaos Daemon", chaos_daemon, SD_NULL, SFB_TICK | SFB_CMD, PULSE_SEC * 300},
 {"xxx", spec_unused, SD_NEVER, SFB_TICK, PULSE_SEC * 10},
 {"xxx", spec_unused, SD_ASCII, SFB_TICK, PULSE_SEC * 5},
 {"xxx", spec_unused, SD_NULL, SFB_CMD, 0},
 {"xxx", spec_unused, SD_NEVER, SFB_CMD, 0},
 {"xxx", spec_unused, SD_NULL,SFB_TICK, PULSE_SEC * 15},

 {"teach init", teach_init, SD_NEVER, SFB_CMD, 0},
 {"a teacher", teaching, SD_NEVER, SFB_CMD, 0},
 {"random global move", random_move, SD_NULL, SFB_TICK, PULSE_SEC * 60},
 {"random zone move", random_zonemove, SD_NULL, SFB_TICK, PULSE_SEC * 60},
 {"scavenger", scavenger, SD_NULL, SFB_TICK, PULSE_SEC * 45},
 {"aggressive", aggressive, SD_NULL, SFB_TICK, PULSE_SEC * 30},

 {"Log Object", log_object, SD_NULL, SFB_CMD|SFB_TICK, PULSE_SEC * 5},

 {"Reception Daemon", recep_daemon, SD_NEVER, SFB_CMD | SFB_TICK, PULSE_SEC * 500},
 {"combat magic-casting", combat_magic, SD_ASCII,SFB_COM, 0},
 {"blow away", blow_away, SD_ASCII, SFB_TICK, PULSE_SEC * 180},
 {"xxx", spec_unused, SD_NULL, SFB_CMD, 0},
 {"Competition Board", competition_board, SD_NULL, SFB_CMD, 0},
 {"combat poison sting", combat_poison_sting, SD_NULL,SFB_COM, 0},
 {"OBSOLETE USE SPELL GAS BREATH", NULL, SD_NULL,SFB_COM, 0},
 {"OBSOLETE", NULL, SD_NULL,SFB_CMD, 0},
 {"Combat Magic Heal", combat_magic_heal, SD_ASCII,SFB_COM, 0},

 {"Guard Way", guard_way, SD_ASCII,SFB_CMD, 0},
 {"Guard Door", guard_door, SD_ASCII,SFB_CMD, 0},
 {"Guard Unit", guard_unit, SD_ASCII,SFB_CMD, 0},
 {"Rescue", rescue, SD_ASCII,SFB_COM, 0},
 {"Team Work", teamwork, SD_ASCII, SFB_COM, 0},
 {"Hide Away", hideaway, SD_ASCII, SFB_TICK, PULSE_SEC * 180},
 {"Animal Obey", obey_animal, SD_ASCII, SFB_CMD, 0},
 {"Shop keeper init", shop_init, SD_ASCII, SFB_CMD | SFB_DEAD, 0},
 {"Reverse Align-Aggressive", aggres_rev_align, SD_NULL,SFB_TICK,PULSE_SEC*15},
 {"Charm of Death", charm_of_death, SD_NULL,SFB_DEAD, 0},
 {"Dictionary", dictionary, SD_NULL,SFB_CMD|SFB_SAVE, 0},
 {"XXX", spec_unused, SD_NULL,SFB_CMD, 0},
 {"Guard Way Level", guard_way_level, SD_ASCII,SFB_CMD, 0},
 {"UNUSED", spec_unused, SD_NEVER,SFB_CMD, 0},
 {"Force Move", force_move, SD_ASCII,SFB_TICK, PULSE_SEC * 45},

 {"XXX UNUSED", spec_unused, SD_NEVER,SFB_DEAD, 0},
 {"XXX UNUSED", spec_unused, SD_NEVER,SFB_CMD, 0},
 {"XXX UNUSED", spec_unused, SD_NEVER,SFB_TICK, PULSE_SEC * 30},
 {"XXX UNUSED", spec_unused, SD_NEVER,SFB_CMD, 0},
 {"XXX UNUSED", spec_unused, SD_NEVER, 0, 0},
 {"XXX UNUSED", spec_unused, SD_NEVER, SFB_CMD, 0},
 {"XXX UNUSED", spec_unused, SD_NEVER, SFB_CMD, 0},
 {"XXX UNUSED", spec_unused, SD_NEVER, SFB_CMD, 0},

 {"Object for Good", obj_good, SD_NULL, SFB_CMD, 0},
 {"Object for Evil", obj_evil, SD_NULL, SFB_CMD, 0},
 {"Change Hometown", change_hometown,SD_NULL,SFB_CMD, 0},
 {"Guard Guild Way", guard_guild_way,SD_ASCII,SFB_CMD, 0},
 {"Teach Guild Members Only", teach_members_only, SD_ASCII, SFB_CMD, 0},
 {"Whistle", whistle, SD_NEVER, SFB_COM | SFB_DEAD, 0},
 {"Guild Master Init", guild_master_init, SD_ASCII, SFB_CMD, 0},
 {"Guild Basis", guild_basis, SD_ASCII, SFB_DEAD|SFB_COM, 0},
 {"Death Room", death_room, SD_NEVER, SFB_TICK, PULSE_SEC * 15},
 {"OBSOLETE", spec_unused, SD_ASCII, SFB_COM, 0},
 {"Groen Tuborg", green_tuborg, SD_NULL, SFB_CMD, 0},
 {"Guild Titles", guild_title, SD_NULL, SFB_CMD, 0},
 {"Reward Board", reward_board, SD_ASCII, SFB_CMD, 0},
 {"Give Reward", reward_give, SD_NEVER, SFB_CMD, 0},
 {"Evaluate", evaluate, SD_NEVER, SFB_CMD, 0},
 {"Error file clearing", error_rod, SD_ASCII, SFB_CMD, 0},
 {"User info file clearing", info_rod, SD_ASCII, SFB_CMD, 0},
 {"XXXX UNUSED", spec_unused, SD_ASCII, SFB_CMD, 0},
 {"DIL", run_dil, SD_NULL, SFB_TICK|SFB_CMD, PULSE_SEC * 1},
 {"Oracle", oracle, SD_NEVER, SFB_CMD, 0},
 {"Administrator", admin_obj, SD_NULL, SFB_CMD, 0},
 {"Obey", obey, SD_NULL, SFB_CMD, 0},
 {"Crystal Ball", ball, SD_NULL, SFB_CMD, 0},
 {"Administrator Slime", slime_obj, SD_NULL, SFB_CMD, 0},
 {"Frozen", frozen, SD_NULL, SFB_CMD | SFB_AWARE, 0},
 {"Return to Origin", return_to_origin, SD_NEVER, SFB_TICK, PULSE_SEC * 60},
 {"Guild Master Internal", guild_master, SD_NEVER, SFB_CMD, 0},
 {"Object Restrict", restrict_obj, SD_ASCII, SFB_CMD, 0},
 {"Demi Stuff", demi_stuff, SD_NEVER, SFB_CMD, 0},
 {"Link Dead", link_dead, SD_NEVER, SFB_CMD, 0},
 {"Sacrifice", sacrifice, SD_NULL, SFB_CMD, 0},

 {"XXX Arena", NULL, SD_NULL, SFB_CMD, 0},
 {"XXX Arena", NULL, SD_NULL, SFB_CMD, 0},
 {"XXX Arena", NULL, SD_NEVER, SFB_DEAD|SFB_CMD|SFB_COM, 0},
 {"Restrict Quest", obj_quest, SD_ASCII, SFB_CMD, 0},
 {"Restrict Guild", obj_guild, SD_ASCII, SFB_CMD, 0},
 {"XXX", spec_unused, SD_NULL, SFB_TICK, 0},
 {"DIL Init", dil_init, SD_ASCII, SFB_ALL, PULSE_SEC * 1}
};
