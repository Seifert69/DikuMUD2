/* *********************************************************************** *
 * File   : money.h                                   Part of Valhalla MUD *
 * Version: 2.12                                                           *
 * Author : gnort@daimi.aau.dk                                             *
 *                                                                         *
 * Purpose: Prototypes and macros for money handling.                      *
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

#ifndef _MUD_MONEY_H
#define _MUD_MONEY_H

#include "structs.h"

typedef sbit16 currency_t;
typedef sbit32 amount_t;

struct money_type
{
  currency_t currency;         /* Which currency for money */
  char **strings;              /* The money-strings */
  char *tails;                 /* What to print on the dark side of the coin */
  char *abbrev;                /* Small string for lists */
  ubit8 pl_idx;                /* The index in above to first plural string */
  sbit32 relative_value;       /* Relative to the internal value */
  sbit32 min_value;            /* Minimum internal value of the currency */
  ubit8 coins_per_weight;      /* How many coins per weight unit */
  struct file_index_type *fi;  /* Where is coin object in file */
};

/* You shouldn't have to refer to this array yourself.
 * Use the macros instead.
 */
extern struct money_type money_types[];

/* Give a number of coins (of a specific type) to unit
 * (type == -1 means money_to_unit with local_currency(unit)
 *  Used for database backwards compatibility...)
 */
void coins_to_unit(struct unit_data *, amount_t amt, int type);

/* Use this if an amount needs to be physically transfered.
 * Money is created/deleted if either unit is NULL.
 *
 * Impossible amounts are converted automagically
 */
void money_transfer(struct unit_data *from, struct unit_data *to,
		    amount_t amt, currency_t currency);

/*  Counts up what amount of a given currency a unit holds recursively in
 *  inventory.
 *  Use ANY_CURRENCY as currency-type to count up ALL money...
 */
amount_t unit_holds_total(struct unit_data *u, currency_t currency);

/*  Counts up what amount of a given currency char holds in inventory.
 *  Use ANY_CURRENCY as currency-type to count up ALL money...
 */
amount_t char_holds_amount(struct unit_data *ch, currency_t currency);

/* Can char afford amt in currency?
 *
 * Impossible amounts are converted automagically
 */
ubit1 char_can_afford(struct unit_data *ch, amount_t amt, currency_t currency);

/* Does unit contain any money of type?
 */
struct unit_data *unit_has_money_type(struct unit_data *unit, ubit8 type);

/* Split `money' into two objects.  Return object with amount `amt'
 */
struct unit_data *split_money(struct unit_data *money, amount_t amt);

/* Set all the values on money correctly according to amount - return money
 * In general: DON'T use, as the db handles this correctly...
 */
struct unit_data *set_money(struct unit_data *money, amount_t amt);

/* Check to see if UNIT_IN(money) contains any money of same type, and
 * if so, merge the piles
 */
void pile_money(struct unit_data *money);

/*  Round amount down/up to nearest `types' number of coins
 */
amount_t money_round(ubit1 up, amount_t amt, currency_t currency, int types);

#define money_round_up(a, c, t)		(money_round(TRUE,  (a), (c), (t)))
#define money_round_down(a, c, t)	(money_round(FALSE, (a), (c), (t)))

/* Local currency of unit, or DEF_CURRENCY if not defined.
 */
currency_t local_currency(struct unit_data *unit);

/* Print out representation of money-object with the amount amt .
 * (amt == 0 means all)
 */
char *obj_money_string(struct unit_data *obj, amount_t amt);

/* Print out optimal representation of amt in currency
 *
 * Impossible amounts are converted automagically
 */
const char *money_string(amount_t amt, currency_t currency, ubit1 verbose);

/* How many `coins' of given money-object can char carry, resp. unit contain
 *   (Naturally the amount of money is an upper bound)
 */
amount_t char_can_carry_amount(struct unit_data *ch, struct unit_data *money);
amount_t unit_can_hold_amount(struct unit_data *unit, struct unit_data *money);


#define money_pluralis_type(type) \
  (money_types[(type)].strings[money_types[(type)].pl_idx])
#define money_pluralis(unit) \
  (money_pluralis_type(MONEY_TYPE(unit)))

#define money_singularis_type(type) \
  (money_types[(type)].strings[0])
#define money_singularis(unit) \
  (money_singularis_type(MONEY_TYPE(unit)))

#define money_from_unit(unit, amt, currency) \
  (money_transfer((unit), NULL, (amt), (currency)))

#define money_to_unit(unit, amt, currency) \
  (money_transfer(NULL, (unit), (amt), (currency)))

#define IS_MONEY(unit) \
  (IS_OBJ(unit) && OBJ_TYPE(unit) == ITEM_MONEY)

#define MONEY_AMOUNT(unit) \
  (OBJ_PRICE(unit))   // MS2020: was  ((amount_t) OBJ_PRICE(unit))

/* Index into money-array */
#define MONEY_TYPE(obj) \
  (OBJ_VALUE((obj), 0))

#define MONEY_CURRENCY(obj) \
  (money_types[MONEY_TYPE(obj)].currency)

#define MONEY_RELATIVE(obj) \
  (money_types[MONEY_TYPE(obj)].relative_value)

#define MONEY_MIN_VALUE(obj) \
  (money_types[MONEY_TYPE(obj)].min_value)

#define MONEY_WEIGHT(obj) \
  (money_types[MONEY_TYPE(obj)].coins_per_weight)

#define MONEY_VALUE(obj) \
  (MONEY_AMOUNT(obj) * MONEY_RELATIVE(obj))

#define MONEY_WEIGHT_SUM(obj1, obj2) \
  ((MONEY_AMOUNT(obj1) + MONEY_AMOUNT(obj2)) / MONEY_WEIGHT(obj1))

#endif /* _MUD_MONEY_H */
