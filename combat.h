/* *********************************************************************** *
 * File   : combat.h                                  Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: The global combat list, perform violence and combat speed.     *
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

#ifndef _MUD_COMBAT_H
#define _MUD_COMBAT_H

#ifdef __cplusplus

class cCombatList
{
  public:
   cCombatList();
   ~cCombatList();
   void PerformViolence();
   void add(class cCombat *pc);
   void sub(class cCombat *pc);
   void status(const struct unit_data *ch);

  private:
   void Sort();

   class cCombat **pElems;
   int nMaxTop;   // No of allocated elements
   int nTop;      // Current Max
   int nIdx;      // Updated when in Perform() and doing Sub()
};

class cCombat
{
  friend class cCombatList;

  public:
   cCombat(struct unit_data *owner, int bMelee = FALSE);
   ~cCombat();

   struct unit_data *Opponent(int i = 0);
   struct unit_data *FindOpponent(struct unit_data *tmp);

   inline struct unit_data *Owner(void) { return pOwner; }
   inline struct unit_data *Melee(void) { return pMelee; }
   inline int When() { return nWhen; }
   inline int NoOpponents(void) { return nNoOpponents; }

   void changeSpeed(int delta);
   void setMelee(struct unit_data *victim);
   void setCommand(char *arg);

   void addOpponent(struct unit_data *victim, int bMelee);
   void subOpponent(struct unit_data *victim);
   void status(const struct unit_data *ch);

  private:
   void add(struct unit_data *victim);
   void sub(int idx);
   int findOpponentIdx(struct unit_data *tmp);

   int nWhen;                     // What tick to attack / command at
   struct unit_data *pOwner;      // The owning unit
   struct unit_data *pMelee;      // The melee or kill pointer
   struct unit_data **pOpponents; // Array of opponents (given damage)
   int nNoOpponents;              // Number of opponents
   char cmd[MAX_INPUT_LENGTH+1];  // A combat command
};

extern class cCombatList CombatList;

void set_fighting(struct unit_data *ch,
		  struct unit_data *vict, int bMelee = FALSE);
void stop_fighting(struct unit_data *ch, struct unit_data *victim = NULL);

void stat_combat(const struct unit_data *ch, struct unit_data *u);

#endif

#endif
