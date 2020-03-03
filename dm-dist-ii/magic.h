/* *********************************************************************** *
 * File   : magic.h                                   Part of Valhalla MUD *
 * Version: 1.03                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Header for magic.c                                             *
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

#ifndef _MUD_MAGIC_H
#define _MUD_MAGIC_H

/* #define MAGIC_POWER(ch)  \
   MAX(CHAR_MAG(ch), CHAR_DIV(ch)) */
     
int dil_effect(const char *pStr, struct spell_args *sa);
int object_power(struct unit_data *unit);
ubit1 use_mana(struct unit_data *medium, int mana);
ubit1 cast_magic_now(struct unit_data *ch, int mana);

ubit1 may_teleport_away(struct unit_data *unit);
ubit1 may_teleport_to(struct unit_data *unit, struct unit_data *dest);
ubit1 may_teleport(struct unit_data *unit, struct unit_data *dest);


int spell_cast_check(struct unit_data *att, int spell);
int spell_resistance(struct unit_data *att, struct unit_data *def, int spell);
int spell_ability(struct unit_data *u, int ability, int spell);
int spell_attack_ability(struct unit_data *medium, int spell);
int spell_attack_skill(struct unit_data *unit, int spell);
int spell_defense_skill(struct unit_data *unit, int spell);

int spell_offensive(struct spell_args *sa, int spell_number, int bonus = 0);

int variation(int n, int d, int u);

/* These are all the spell definitions... */

void spell_bless(struct spell_args *sa);
void spell_curse(struct spell_args *sa);
void spell_remove_curse(struct spell_args *sa);
void spell_cure_wounds_1(struct spell_args *sa);
void spell_cure_wounds_2(struct spell_args *sa);
void spell_cure_wounds_3(struct spell_args *sa);
void spell_cause_wounds_1(struct spell_args *sa);
void spell_cause_wounds_2(struct spell_args *sa);
void spell_cause_wounds_3(struct spell_args *sa);
void spell_dispel_evil(struct spell_args *sa);
void spell_dispel_good(struct spell_args *sa);
void spell_repel_undead_1(struct spell_args *sa);
void spell_repel_undead_2(struct spell_args *sa);
void spell_blind(struct spell_args *sa);
void spell_cure_blind(struct spell_args *sa);
void spell_locate_object(struct spell_args *sa);
void spell_locate_char(struct spell_args *sa);
void spell_raise_mag(struct spell_args *sa);
void spell_raise_div(struct spell_args *sa);
void spell_raise_str(struct spell_args *sa);
void spell_raise_dex(struct spell_args *sa);
void spell_raise_con(struct spell_args *sa);
void spell_raise_cha(struct spell_args *sa);
void spell_raise_bra(struct spell_args *sa);
void spell_raise_tgh(struct spell_args *sa);
void spell_raise_divine(struct spell_args *sa);
void spell_raise_protection(struct spell_args *sa);
void spell_raise_detection(struct spell_args *sa);
void spell_raise_summoning(struct spell_args *sa);
void spell_raise_creation(struct spell_args *sa);
void spell_raise_mind(struct spell_args *sa);
void spell_raise_heat(struct spell_args *sa);
void spell_raise_cold(struct spell_args *sa);
void spell_raise_cell(struct spell_args *sa);
void spell_raise_internal(struct spell_args *sa);
void spell_raise_external(struct spell_args *sa);
void spell_pro_evil(struct spell_args *sa);
void spell_pro_good(struct spell_args *sa);
void spell_sanctuary(struct spell_args *sa);
void spell_dispel_magic(struct spell_args *sa);
void spell_sustain(struct spell_args *sa);
void spell_lock(struct spell_args *sa);
void spell_unlock(struct spell_args *sa);
void spell_magic_lock(struct spell_args *sa);
void spell_magic_unlock(struct spell_args *sa);
void spell_great_knock(struct spell_args *sa);
void spell_det_align(struct spell_args *sa);
void spell_det_invisible(struct spell_args *sa);
void spell_det_magic(struct spell_args *sa);
void spell_det_poison(struct spell_args *sa);
void spell_det_undead(struct spell_args *sa);
void spell_det_curse(struct spell_args *sa);
void spell_sense_life(struct spell_args *sa);
void spell_identify_1(struct spell_args *sa);
void spell_identify_2(struct spell_args *sa);
void spell_random_teleport(struct spell_args *sa);
void spell_clear_skies(struct spell_args *sa);
void spell_storm_call(struct spell_args *sa);
void spell_control_teleport(struct spell_args *sa);
void spell_summon_char_1(struct spell_args *sa);
void spell_summon_char_2(struct spell_args *sa);
void spell_create_food(struct spell_args *sa);
void spell_create_water(struct spell_args *sa);
void spell_light_1(struct spell_args *sa);
void spell_light_2(struct spell_args *sa);
void spell_darkness_1(struct spell_args *sa);
void spell_darkness_2(struct spell_args *sa);
void spell_enchant_weapon(struct spell_args *sa);
void spell_enchant_armour(struct spell_args *sa);
void spell_animate_dead(struct spell_args *sa);
void spell_heroism(struct spell_args *sa);
void spell_armour_fitting(struct spell_args *sa);
void spell_control_undead(struct spell_args *sa);
void spell_absorbtion(struct spell_args *sa);
void spell_permanency(struct spell_args *sa);
void spell_clone(struct spell_args *sa);
void spell_colourspray_1(struct spell_args *sa);
void spell_colourspray_2(struct spell_args *sa);
void spell_colourspray_3(struct spell_args *sa);
void spell_invisibility(struct spell_args *sa);
void spell_wizard_eye(struct spell_args *sa);
void spell_fear(struct spell_args *sa);
void spell_confusion(struct spell_args *sa);
void spell_xray_vision(struct spell_args *sa);
void spell_calm(struct spell_args *sa);
void spell_hold(struct spell_args *sa);
void spell_command(struct spell_args *sa);
void spell_charm(struct spell_args *sa);
void spell_fireball_1(struct spell_args *sa);
void spell_fireball_2(struct spell_args *sa);
void spell_fireball_3(struct spell_args *sa);
void spell_frostball_1(struct spell_args *sa);
void spell_frostball_2(struct spell_args *sa);
void spell_frostball_3(struct spell_args *sa);
void spell_lightning_1(struct spell_args *sa);
void spell_lightning_2(struct spell_args *sa);
void spell_lightning_3(struct spell_args *sa);
void spell_stinking_cloud_1(struct spell_args *sa);
void spell_stinking_cloud_2(struct spell_args *sa);
void spell_stinking_cloud_3(struct spell_args *sa);
void spell_poison(struct spell_args *sa);
void spell_remove_poison(struct spell_args *sa);
void spell_disease_1(struct spell_args *sa);
void spell_disease_2(struct spell_args *sa);
void spell_rem_disease(struct spell_args *sa);
void spell_acidball_1(struct spell_args *sa);
void spell_acidball_2(struct spell_args *sa);
void spell_acidball_3(struct spell_args *sa);
void spell_mana_boost(struct spell_args *sa);
void spell_find_path(struct spell_args *sa);
void spell_transport(struct spell_args *sa);
void spell_undead_door(struct spell_args *sa);

#endif /* _MUD_MAGIC_H */
