/* *********************************************************************** *
 * File   : skills.h                                  Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Header for skills.c                                            *
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

#ifndef _MUD_SKILLS_H
#define _MUD_SKILLS_H

struct skill_interval
{
   const int   skill;
   const char *descr;
};

const char *skill_text(const struct skill_interval *si, int skill);

/* ---------------- COMBAT MESSAGE SYSTEM -------------------- */

#define COM_MAX_MSGS   60

#define COM_MSG_DEAD   -1
#define COM_MSG_MISS   -2
#define COM_MSG_NODAM  -3
#define COM_MSG_EBODY  -4

/* ---------------- RACES -------------------- */

#define RACE_IS_HUMANOID(race) \
  ((race) <= RACE_OTHER_HUMANOID)

#define CHAR_IS_MAMMAL(ch)     \
  RACE_IS_MAMMAL(CHAR_RACE(ch))

#define RACE_IS_MAMMAL(race) \
  (((race) > RACE_OTHER_HUMANOID) && ((race) <= RACE_OTHER_MAMMAL))

#define RACE_IS_UNDEAD(race)     \
  (((race) > RACE_OTHER_CREATURE) && ((race) <= RACE_OTHER_UNDEAD))

#define CHAR_IS_HUMANOID(ch)   \
  RACE_IS_HUMANOID(CHAR_RACE(ch))

#define CHAR_IS_UNDEAD(ch)     \
  RACE_IS_UNDEAD(CHAR_RACE(ch))

struct dice_type
{
   ubit16 reps;
   ubit16 size;
};

struct base_race_info_type
{
   ubit16 height;
   struct dice_type height_dice;

   ubit16 weight;
   struct dice_type weight_dice;

   ubit16 lifespan;
   struct dice_type lifespan_dice;
};


struct race_info_type
{
   struct base_race_info_type male;
   struct base_race_info_type female;

   ubit16 age;
   struct dice_type age_dice;
};


struct damage_chart_element_type
{
   int offset;  /* When does damage start         */
   int basedam; /* The damage given at 'offset'   */
   int alpha;   /* Step size of damage as 1/alpha */
};

struct damage_chart_type
{
   int fumble;  /* from 01 - fuble => fumble      */

   struct damage_chart_element_type element[5];
};

#define TREE_PARENT(tree, node)        (tree[node].parent)
#define TREE_GRANDPARENT(tree, node)   (tree[tree[node].parent].parent)
#define TREE_ISROOT(tree, node)        ((node) == (tree[node].parent))
#define TREE_ISLEAF(tree, node)        (tree[node].isleaf)

struct tree_type
{
  int parent;
  ubit8 isleaf;
};

/* Tree has a pointer to parent for each node. 0 pointer from root */
struct wpn_info_type
{
   int hands;    /* 0=N/A, 1 = 1, 2 = 1.5, 3 = 2          */
   int speed;    /* Speed modification by weapon 0..      */
   int type;     /* Is the weapon slashing/piercing...    */
   ubit8 shield; /* Shield method SHIELD_M_XXX            */
};


#define DEMIGOD_LEVEL_XP (40000000)

int object_two_handed(struct unit_data *obj);
void roll_description(struct unit_data *att, const char *text, int roll);
int open_ended_roll(int size, int end);
inline int open100(void) { return open_ended_roll(100, 5); }

int resistance_skill_check(int att_skill1, int def_skill1,
			   int att_skill2, int def_skill2);
int resistance_level_check(int att_level, int def_level,
			   int att_skill, int def_skill);
int skill_duration(int howmuch);


int weapon_fumble(struct unit_data *weapon, int roll);
int chart_damage(int roll, struct damage_chart_element_type *element);
int chart_size_damage(int roll, struct damage_chart_element_type *element,
		      int lbs);
int weapon_damage(int roll, int weapon_type, int armour_type);
int natural_damage(int roll, int weapon_type, int armour_type, int lbs);


int basic_char_tgh_absorb(struct unit_data *ch);
int basic_armor_absorb(struct unit_data *armour, int att_type);
int basic_char_absorb(struct unit_data *ch, struct unit_data *armor,
		      int att_type);


int basic_char_weapon_dam(struct unit_data *ch, struct unit_data *weapon);
int basic_char_hand_dam(struct unit_data *ch);
int char_weapon_dam(struct unit_data *ch, struct unit_data *weapon);
int char_hand_dam(struct unit_data *ch);

int relative_level(int l1, int l2);
int weapon_defense_skill(struct unit_data *ch, int skill);
int weapon_attack_skill(struct unit_data *ch, int skill);
int hit_location(struct unit_data *att, struct unit_data *def);
int effective_dex(struct unit_data *ch);

int av_value(int abila, int abilb, int skilla, int skillb);
int av_howmuch(int av);
int av_makes(int av);
void check_fitting(struct unit_data *u);

extern struct race_info_type race_info[PC_RACE_MAX];
extern const char *pc_races[PC_RACE_MAX+1];
extern const char *pc_race_adverbs[PC_RACE_MAX+1];


extern struct damage_chart_type spell_chart[SPL_TREE_MAX];
extern sbit8 racial_ability[ABIL_TREE_MAX][PC_RACE_MAX];
extern sbit8 racial_weapons[WPN_TREE_MAX][PC_RACE_MAX];
extern sbit8 racial_skills[SKI_TREE_MAX][PC_RACE_MAX];
extern sbit8 racial_spells[SPL_TREE_MAX][PC_RACE_MAX];

extern struct tree_type wpn_tree[WPN_TREE_MAX+1];
extern struct tree_type spl_tree[SPL_TREE_MAX+1];
extern struct tree_type ski_tree[SKI_TREE_MAX+1];
extern struct tree_type abil_tree[ABIL_TREE_MAX+1];

extern int hit_location_table[];

extern const char *wpn_text[WPN_TREE_MAX+1];
extern const char *spl_text[SPL_TREE_MAX+1];
extern const char *ski_text[SKI_TREE_MAX+1];
extern const char *abil_text[ABIL_TREE_MAX+1];

#endif /* _MUD_SKILLS_H */
