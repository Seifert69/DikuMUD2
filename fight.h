/* *********************************************************************** *
 * File   : fight.h                                   Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Fight include file                                             *
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

#ifndef _MUD_FIGHT_H
#define _MUD_FIGHT_H

struct SFightColorSet
{
  char *pAttacker;
  char *pDefender;
  char *pOthers;
};

class unit_data *raw_kill(struct unit_data *ch);

int provoked_attack(class unit_data *victim, class unit_data *ch);

void update_pos( struct unit_data *victim );

void damage(struct unit_data *ch, struct unit_data *victim,
	    struct unit_data *medium,
	    int damage, int attackcat, int weapontype, int hitloc,
	    int bDisplay = TRUE);

int pk_test(struct unit_data *att, struct unit_data *def, int message);
int one_hit(struct unit_data *att, struct unit_data *def,
	    int bonus, int wpn_type, int primary = TRUE);
int simple_one_hit(struct unit_data *att, struct unit_data *def);
int char_dual_wield(struct unit_data *ch);
void melee_violence(struct unit_data *ch, int primary);
int melee_bonus(struct unit_data *att, struct unit_data *def,
		int hit_loc,
		int *pAtt_weapon_type, struct unit_data **pAtt_weapon,
		int *pDef_armour_type, struct unit_data **pDef_armour,
		int primary = TRUE);
int shield_bonus(struct unit_data *att, struct unit_data *def,
		 struct unit_data **pDef_shield);
#endif
