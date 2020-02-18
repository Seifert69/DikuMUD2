/* *********************************************************************** *
 * File   : common.h                                  Part of Valhalla MUD *
 * Version: 0.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Common ".h" between dmserver and dmc.                          *
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

#ifndef _MUD_COMMON_H
#define _MUD_COMMON_H

extern const char *fillwords[];

extern struct wpn_info_type wpn_info[];

extern const char *drinks[];
extern const char *dirs[];

extern char libdir[64];

int required_xp(int level);
int level_xp(int level);



int is_in(int a, int from, int to);
void set_hits(struct unit_data *obj, int craftsmanship);

int skill_point_gain(void);
int ability_point_gain(void);
int ability_point_total(int level);
int skill_point_total(int level);
double damage_str(int strength);

int distribute_points(ubit8 *skills, int max, int points, int level);
int buy_points(int points, int level, int *error);

int apply_quality(int num, int quality);

int hitpoint_total(int hpp);

void set_weapon(struct unit_data *);
void set_shield(struct unit_data *);
void set_armour(struct unit_data *);


/* ..................................................................... */
/*                           A B I L I T I E S                           */
/* ..................................................................... */


#define SKILL_MAX              250  /* Maximum of any skill / ability */
#define AVERAGE_SKILL_COST      10
#define HITPOINTS_BASE         125
#define ABILITY_POINT_FACTOR    (4) /* Abilitypoints multiplied by this */
#define SKILL_POINT_FACTOR      (8) /* Skillpoints   multiplied by this */

/* ..................................................................... */
/*                           A R M O U R S                               */
/* ..................................................................... */

struct shi_info_type
{
   int melee;  /* Protection against melee attacks */
};

struct arm_info_type
{
  int slash;
  int bludgeon;
  int pierce;
};

#endif /* _MUD_COMMON_H */
