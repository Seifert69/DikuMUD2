/* *********************************************************************** *
 * File   : limits.h                                  Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : ???                                                            *
 *                                                                         *
 * Purpose: Prototypes for the Limit/Gain control module                   *
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

#ifndef _MUD_LIMITS_H
#define _MUD_LIMITS_H

#include "structs.h"

int char_can_carry_unit(struct unit_data *ch, struct unit_data *unit);
int char_can_get_unit(struct unit_data *ch, struct unit_data *unit);

int char_carry_n(struct unit_data *unit);
int char_carry_n_limit(struct unit_data *ch);
int char_can_carry_n(struct unit_data *ch, int n = 1);

int char_carry_w_limit(struct unit_data *ch);
int char_can_carry_w(struct unit_data *ch, int weight);
int char_drag_w_limit(struct unit_data *ch);
int char_can_drag_w(struct unit_data *ch, int weight);

int mana_limit(struct unit_data *ch);
int hit_limit_number(int);
int hit_limit(struct unit_data *ch);
int move_limit(struct unit_data *ch);

int mana_gain(struct unit_data *ch);
int hit_gain(struct unit_data *ch);
int move_gain(struct unit_data *ch);

void set_title(struct unit_data *ch);

void advance_level(struct unit_data *ch);
void gain_exp(struct unit_data *ch, int gain);
void gain_exp_regardless(struct unit_data *ch, int gain);
void gain_condition(struct unit_data *ch, int condition, int value);



#endif /* _MUD_LIMITS_H */
