/* *********************************************************************** *
 * File   : spec_assign.h                             Part of Valhalla MUD *
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
#ifndef _MUD_SPEC_ASS_H
#define _MUD_SPEC_ASS_H

#include "interpreter.h"

int persist_intern(struct spec_arg *sarg);
int persist_init(struct spec_arg *sarg);

int competition_board(struct spec_arg *sarg);
int dil_init(struct spec_arg *sarg);
int dil_direct_init(struct spec_arg *sarg);
int dead_only(struct spec_arg *sarg);
int death_obj(struct spec_arg *sarg);
int board(struct spec_arg *sarg);
int mob_command(struct spec_arg *sarg);
int hunting(struct spec_arg *sarg);
int postman(struct spec_arg *sarg);
int shop_keeper(struct spec_arg *sarg);
int eat_and_delete(struct spec_arg *sarg);
int accuse(struct spec_arg *sarg);
int protect_lawful(struct spec_arg *sarg);
int pain_init(struct spec_arg *sarg);
int pain_exec(struct spec_arg *sarg);
int npc_visit_room(struct spec_arg *sarg);
int mercenary_hire(struct spec_arg *sarg);
int mercenary_hunt(struct spec_arg *sarg);
int bank(struct spec_arg *sarg);
int dump(struct spec_arg *sarg);
int fido(struct spec_arg *sarg);
int janitor(struct spec_arg *sarg);

int teach_init(struct spec_arg *sarg);
int teaching(struct spec_arg *sarg);

int random_move(struct spec_arg *sarg);
int random_zonemove(struct spec_arg *sarg);
int scavenger(struct spec_arg *sarg);
int aggressive(struct spec_arg *sarg);
int aggres_rev_align(struct spec_arg *sarg);
int combat_magic(struct spec_arg *sarg);
int blow_away(struct spec_arg *sarg);
int combat_poison_sting(struct spec_arg *sarg);
int spider_room_attack(struct spec_arg *sarg);
int combat_magic_heal(struct spec_arg *sarg);
int guard_way(struct spec_arg *sarg);
int guard_door(struct spec_arg *sarg);
int guard_unit(struct spec_arg *sarg);
int rescue(struct spec_arg *sarg);
int teamwork(struct spec_arg *sarg);
int hideaway(struct spec_arg *sarg);
int shop_init(struct spec_arg *sarg);
int charm_of_death(struct spec_arg *sarg);
int dictionary(struct spec_arg *sarg);
int log_object(struct spec_arg *sarg);
int guard_way_level(struct spec_arg *sarg);
int odin_statue(struct spec_arg *sarg);
int force_move(struct spec_arg *sarg);
int ww_pool(struct spec_arg *sarg);
int ww_block_river(struct spec_arg *sarg);
int ww_stone_render (struct spec_arg *sarg);
int ww_earth_blood(struct spec_arg *sarg);
int ww_raver(struct spec_arg *sarg);
int ww_vat_machine(struct spec_arg *sarg);
int ww_quest_done (struct spec_arg *sarg);
int ww_block_attack (struct spec_arg *sarg);

int obj_good (struct spec_arg *sarg);
int obj_evil (struct spec_arg *sarg);
int obj_quest (struct spec_arg *sarg);
int obj_guild (struct spec_arg *sarg);

int change_hometown(struct spec_arg *sarg);

int guard_guild_way(struct spec_arg *sarg);
int teach_members_only(struct spec_arg *sarg);
int whistle(struct spec_arg *sarg);
int guild_master(struct spec_arg *sarg);
int guild_basis(struct spec_arg *sarg);
int guild_title(struct spec_arg *sarg);

int death_room(struct spec_arg *sarg);
int breath_weapon(struct spec_arg *sarg);
int green_tuborg(struct spec_arg *sarg);
int reward_board(struct spec_arg *sarg);
int reward_give(struct spec_arg *sarg);

int recep_daemon(struct spec_arg *sarg);
int chaos_daemon(struct spec_arg *sarg);

int evaluate(struct spec_arg *sarg);

int error_rod(struct spec_arg *sarg);
int info_rod(struct spec_arg *sarg);
int climb(struct spec_arg *sarg);
int run_dil(struct spec_arg *sarg);
int oracle(struct spec_arg *sarg);
int admin_obj(struct spec_arg *sarg);
int obey_animal(struct spec_arg *sarg);
int obey(struct spec_arg *sarg);
int ball(struct spec_arg *sarg);
int slime_obj(struct spec_arg *sarg);
int frozen(struct spec_arg *sarg);
int return_to_origin(struct spec_arg *sarg);
int guild_master_init(struct spec_arg *sarg);
int restrict_obj(struct spec_arg *sarg);
int demi_stuff(struct spec_arg *sarg);
int link_dead(struct spec_arg *sarg);
int sacrifice(struct spec_arg *sarg);

int justice_scales(struct spec_arg *sarg);

#endif
