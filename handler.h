/* *********************************************************************** *
 * File   : handler.h                                 Part of Valhalla MUD *
 * Version: 2.02                                                           *
 * Author : seifert@diku.dk and quinn@diku.dk                              *
 *                                                                         *
 * Purpose: Prototypes for handling units.                                 *
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

#ifndef _MUD_HANDLER_H
#define _MUD_HANDLER_H

#include "unitfind.h"

struct descriptor_data *unit_is_edited(struct unit_data *u);

void unit_messg(struct unit_data *ch, struct unit_data *unit, const char *type,
		const char *mesg_s, const char *mesg_o);

const char *single_unit_messg(struct unit_data *unit,
			      const char *type,
			      const char *pSubStr,
			      const char *mesg);

void szonelog(struct zone_type *zone, const char *fmt, ...);

/* From pcsave.c - I'm just tired of specifying them everywhere */
void save_player(struct unit_data *pc);
struct unit_data *load_player(const char *pName);
void load_contents(const char *pFileName, struct unit_data *unit);
void save_player_contents(struct unit_data *pc, int fast);


/* handler.c */
struct extra_descr_data *quest_add(struct unit_data *ch,
				   char *name, char *descr);

void insert_in_unit_list(struct unit_data *u);
void remove_from_unit_list(struct unit_data *unit);

struct unit_fptr *find_fptr(struct unit_data *u, ubit16 index);
struct unit_fptr *create_fptr(struct unit_data *u, ubit16 index,
			      ubit16 beat, ubit16 flags, void *data);
void destroy_fptr(struct unit_data *u, struct unit_fptr *f);

void stop_following(struct unit_data *ch);
void start_following(struct unit_data *ch, struct unit_data *leader);

void modify_bright(struct unit_data *unit, int bright);
void trans_set(struct unit_data *u);
void trans_unset(struct unit_data *u);

struct unit_data *equipment(struct unit_data *ch, ubit8 pos);
struct unit_data *equipment_type(struct unit_data *ch, int pos, ubit8 type);
void equip_char(struct unit_data *ch, struct unit_data *obj, ubit8 pos);
struct unit_data *unequip_char(struct unit_data *ch, ubit8 pos);
struct unit_data *unequip_object(struct unit_data *obj);
void recalc_dex_red(struct unit_data *ch);

int unit_recursive(struct unit_data *from, struct unit_data *to);
struct zone_type *unit_zone(const struct unit_data *unit);
struct unit_data *unit_room(struct unit_data *unit);

/* If the transfered unit MIGHT be money, remember to pile_money() it!!!!! */
void unit_up(struct unit_data *unit);
void unit_from_unit(struct unit_data *unit);
void unit_down(struct unit_data *unit, struct unit_data *to);
void unit_to_unit(struct unit_data *unit, struct unit_data *to);

void extract_unit(struct unit_data *unit);

void weight_change_unit(struct unit_data *unit, int weight);

struct unit_data *find_unit_in_list_num(int num, struct unit_data *list);
struct unit_data *find_unit_num(int num);

#endif /* _MUD_HANDLER_H */
