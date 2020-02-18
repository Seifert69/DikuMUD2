/* *********************************************************************** *
 * File   : shop.c                                    Part of Valhalla MUD *
 * Version: 2.10                                                           *
 * Author : noop@diku.dk and seifert@diku.dk                               *
 *                                                                         *
 * Purpose: Handling of shops.                                             *
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

/* 02/08/92 seifert: Modified a lot to make init from text string          */
/*    10/92 gnort  : Introduced object money                               */
/* 29/08/93 jubal  : Uses trade price                                      */
/* 14-08-94 gnort  : finalized object money (sheesh, two years! ;-) )      */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "structs.h"
#include "utils.h"
#include "skills.h"
#include "comm.h"
#include "handler.h"
#include "db.h"
#include "interpreter.h"
#include "textutil.h"
#include "utility.h"
#include "money.h"
#include "common.h"
#include "str_parse.h"
#include "spec_assign.h"
#include "limits.h"
#include "act.h"


struct shop_data
{
   struct file_index_type **prod; /* Array of producing items, last nil   */
   int *types;                    /* Which item types to trade            */
   int typecount;                 /* The number of item types             */
   float  profit_buy;             /* Factor to multiply cost with.        */
   float  profit_sell;            /* Factor to multiply cost with.        */
   char *no_such_item1;           /* Message if keeper hasn't got an item */
   char *no_such_item2;           /* Message if player hasn't got an item */
   char *missing_cash1;           /* Message if keeper hasn't got cash    */
   char *missing_cash2;           /* Message if player hasn't got cash    */
   char *do_not_buy;              /* If keeper dosn't buy such things.    */
   char *message_buy;             /* Message when player buys item        */
   char *message_sell;            /* Message when player sells item       */
   int *time1, *time2;            /* When does the shop open / close?     */
   int *currencies;               /* What currencies to accept.           */
   int currencycount;             /* Number of currencies.                */
};


amount_t obj_trade_price(struct unit_data *u)
{
   double d, f;

   d = OBJ_PRICE(u);

   if ((UNIT_MAX_HIT(u) > 0) && (UNIT_HIT(u) > 0))
   {
      f = (double) UNIT_HIT(u) / (double) UNIT_MAX_HIT(u);
      d *= f;
   }
   else
   {
      d = 0.0;
   }

   return (amount_t) d;
}


static bool is_ok(struct unit_data *keeper, struct unit_data *ch,
		  struct shop_data *sd)
{
   char buf[512];
   struct time_info_data time_info;

   struct time_info_data mud_date(time_t t);

   if (IS_GOD(ch))		/* Gods can always shop :) */
     return TRUE;

   time_info = mud_date(time(0));

   if (!CHAR_CAN_SEE(keeper, ch))
   {
      act("$1n says, 'I don't trade with someone I can't see!'",
	  A_SOMEONE, keeper, 0, 0, TO_ROOM);
      return FALSE;
   }

   if (!is_in(time_info.hours, sd->time1[0], sd->time1[1]) &&
       !is_in(time_info.hours, sd->time2[0], sd->time2[1]))
   {
      act("$1n says, 'Come back later $3n, we're closed.'",
	  A_SOMEONE, keeper, 0, ch, TO_ROOM);
      sprintf(buf, "We are open from %d to %d, and %d to %d - "
	      "it is now %d o'clock.",
	      sd->time1[0], sd->time1[1],
	      sd->time2[0], sd->time2[1],
	      time_info.hours);

      act("$1n says '$2t'", A_SOMEONE, keeper, buf, ch, TO_ROOM);

      return FALSE;
   }
   return TRUE;
}


static bool trade_with(struct unit_data *item, struct shop_data *sd)
{
   int i;

   if (!IS_OBJ(item))
     return FALSE;

   if (obj_trade_price(item) < 1)
     return FALSE;

   for (i=0; i < sd->typecount; i++)
     if (sd->types[i] == OBJ_TYPE(item))
       return TRUE;

   return FALSE;
}


static bool shop_producing(struct unit_data *item, struct shop_data *sd)
{
   int counter = 0;

   if (!UNIT_FILE_INDEX(item))
     return FALSE;

   if (sd->prod)
   {
      while (sd->prod[counter])
	if (sd->prod[counter++] == UNIT_FILE_INDEX(item))
	  return TRUE;
   }
   return FALSE;
}


static void shopping_buy(char *arg, struct unit_data *ch,
			 struct unit_data *keeper, struct shop_data *sd)
{
   char buf[MAX_STRING_LENGTH];
   struct unit_data *temp1;
   currency_t currency = 0;
   amount_t price = 0;
   bool can_pay = FALSE;
   int i = 0;
   int refit = FALSE;

   if (!is_ok(keeper, ch, sd))
     return;

   if (str_is_empty(arg))
   {
      act("$1n asks you, 'What do you wish to buy?'",
	  A_SOMEONE, keeper, 0, ch, TO_VICT);
      return;
   }

   if ((temp1 = find_unit(keeper, &arg, 0, FIND_UNIT_INVEN)) == NULL
       || IS_MONEY(temp1))
   {
      act(sd->no_such_item1, A_SOMEONE, keeper, 0, ch, TO_ROOM);
      return;
   }

   if (!IS_OBJ(temp1))
   {
      act("$1n says, 'I only trade objects!'",
	  A_SOMEONE, keeper, 0, ch,TO_ROOM);
      return;
   }

   if (obj_trade_price(temp1) <= 0)
   {
      act(sd->no_such_item1, A_SOMEONE, keeper, 0, ch, TO_ROOM);
      extract_unit(temp1);
      return;
   }

   for (i=0; !can_pay && (i < sd->currencycount); i++)
     if (sd->currencies[i] != -1)
     {
        currency = sd->currencies[i];
	price    = money_round_up((int) (obj_trade_price(temp1) *
					 sd->profit_buy),
				  currency, 2);
	can_pay  = char_can_afford(ch, price, currency);
     }

   if (!can_pay && CHAR_LEVEL(ch) < GOD_LEVEL)
   {
      act(sd->missing_cash2, A_SOMEONE, keeper, temp1, ch, TO_ROOM);
      return;
   }

   if (!char_can_carry_n(ch, 1))
   {
      act("$2n : You can't carry that many items.",
	  A_SOMEONE, ch, temp1, 0, TO_CHAR);
      return;
   }

   if (!char_can_carry_w(ch, UNIT_WEIGHT(temp1)))
   {
      act("$2n : You can't carry that much weight.",
	  A_SOMEONE, ch, temp1, 0, TO_CHAR);
      return;
   }

   act("$1n buys $2n.", A_SOMEONE, ch, temp1, 0, TO_ROOM);

   sprintf(buf, sd->message_buy, money_string(price, currency, TRUE));

   if (shop_producing(temp1, sd))
   {
      if (obj_wear_size(ch, temp1))
      {
	 act("$1n says, 'A perfect fit, $3n!'",
	     A_SOMEONE, keeper, 0, ch, TO_ROOM);
	 refit = TRUE;
      }      
   }

   act(buf, A_SOMEONE, keeper, temp1, ch, TO_VICT);

   act("You now have $2n.", A_SOMEONE, ch, temp1, 0, TO_CHAR);

   if (!IS_GOD(ch))
     money_transfer(ch, keeper, price, currency);

   /* Test if producing shop ! */
   if (shop_producing(temp1, sd))
     temp1 = read_unit(UNIT_FILE_INDEX(temp1));
   else
     unit_from_unit(temp1);

   unit_to_unit(temp1, ch);

   if (refit)
     UNIT_SIZE(temp1) = UNIT_SIZE(ch);
}


static void shopping_sell(char *arg, struct unit_data *ch,
			  struct unit_data *keeper, struct shop_data *sd)
{
   currency_t currency = 0;
   amount_t price = 0;
   bool can_pay = FALSE;
   int i = 0;
   char buf[MAX_STRING_LENGTH], *tmparg;
   struct unit_data *temp1;

   if (!is_ok(keeper, ch, sd))
     return;

   if (str_is_empty(arg))
   {
      act("$1n says, 'What do you want to sell $3n?'",
	  A_SOMEONE, keeper, 0, ch, TO_ROOM);
      return;
   }

   tmparg = arg;
   if ((temp1 = find_unit(ch, &tmparg, 0, FIND_UNIT_INVEN)) == NULL)
   {
      act(sd->no_such_item2, A_SOMEONE, keeper, 0, ch, TO_ROOM);
      return;
   }

   if (!IS_OBJ(temp1))
     return;

   if (!trade_with(temp1, sd))
   {
      act(sd->do_not_buy, A_SOMEONE, keeper, temp1, ch, TO_ROOM);
      return;
   }

   for (i=0; !can_pay && (i < sd->currencycount); i++)
     if (sd->currencies[i] != -1)
     {
        currency = sd->currencies[i];
	price    = money_round_down((int) (obj_trade_price(temp1) *
					   sd->profit_sell),
				    currency, 2);
	can_pay  = char_can_afford(keeper, price, currency);
     }

   if (!can_pay)
   {
      act(sd->missing_cash1, A_SOMEONE, keeper, temp1, ch, TO_ROOM);
      return;
   }

   act("$1n sells $2n to $3n.", A_SOMEONE, ch, temp1, keeper, TO_ROOM);

   sprintf(buf, sd->message_sell, money_string(price, currency, TRUE));
   act(buf, A_SOMEONE, keeper, temp1, ch, TO_ROOM);

   act("$1n now has $2n.", A_SOMEONE, keeper, temp1, ch, TO_VICT);

   money_transfer(keeper, ch, price, currency);

   if (find_unit(keeper, &arg, 0, FIND_UNIT_INVEN) ||
       OBJ_TYPE(temp1) == ITEM_TRASH)
     extract_unit(temp1);
   else
   {
      unit_from_unit(temp1);
      unit_to_unit(temp1, keeper);
   }
}


static void shopping_value(char *arg, struct unit_data *ch,
			   struct unit_data *keeper, struct shop_data *sd)
{
   char buf[MAX_STRING_LENGTH];
   struct unit_data *temp1;
   char *fmt;
   currency_t currency = 0;
   amount_t price = 0;
   bool can_pay = FALSE;
   int i = 0;

   if (!is_ok(keeper, ch, sd))
     return;

   if (str_is_empty(arg))
   {
      act("$1n says, 'What do you want me to value, $3n?'",
	  A_SOMEONE, keeper, 0, ch, TO_ROOM);
      return;
   }

   if ((temp1 = find_unit(ch, &arg, 0, FIND_UNIT_IN_ME)) == NULL)
   {
      act(sd->no_such_item2, A_SOMEONE, keeper, temp1, ch, TO_ROOM);
      return;
   }

   if (!trade_with(temp1, sd))
   {
      act(sd->do_not_buy, A_SOMEONE, keeper, temp1, ch, TO_ROOM);
      return;
   }

   for (i=0; !can_pay && (i < sd->currencycount); i++)
     if (sd->currencies[i] != -1)
     {
        currency = sd->currencies[i];
	price    = money_round_down((int) (obj_trade_price(temp1) *
					   sd->profit_sell),
				    currency, 2);
	can_pay  = char_can_afford(keeper, price, currency);
     }

   
   /* MS2020   
   if (can_pay)
     fmt = "$1n says 'I'll give you %s for $2n, $3n!'";
   else
     fmt = "$1n says 'I'd give you %s for $2n, if I could afford it, $3n.'";

   sprintf(buf, fmt, money_string(price, currency, TRUE));
   */
   
   if (can_pay)
     sprintf(buf, "$1n says 'I'll give you %s for $2n, $3n!'",
	     money_string(price, currency, TRUE));
   else
     sprintf(buf, "$1n says 'I'd give you %s for $2n, if I could afford it, $3n.'",
	     money_string(price, currency, TRUE));
     
   act(buf, A_SOMEONE, keeper, temp1, ch, TO_ROOM);
}


static void shopping_list(char *arg, struct unit_data *ch,
			  struct unit_data *keeper, struct shop_data *sd)
{
   char buf[MAX_STRING_LENGTH], buf2[100], *b = buf;
   struct unit_data *temp1;
   bool found_obj = FALSE;
   amount_t price;
   currency_t currency;
   const char *diff_buf;

   if (!is_ok(keeper, ch, sd))
     return;

   strcpy(buf, "You can buy:\n\r");
   TAIL(b);

   for (temp1 = UNIT_CONTAINS(keeper); temp1; temp1 = temp1->next)
     if (IS_OBJ(temp1) && !OBJ_EQP_POS(temp1) &&
	 CHAR_CAN_SEE(ch, temp1) &&
	 obj_trade_price(temp1) > 0 && !IS_MONEY(temp1))
     {
        found_obj = TRUE;
	diff_buf = NULL;
	if (!shop_producing(temp1, sd))
	   diff_buf = obj_wear_size(ch, temp1);
	
	if (OBJ_TYPE(temp1) == ITEM_DRINKCON)
	  sprintf(buf2, "%s%s%s", STR(UNIT_TITLE_STRING(temp1)),
		  OBJ_VALUE(temp1, 1) ? " of " : "",
		  OBJ_VALUE(temp1, 1) ? drinks[OBJ_VALUE(temp1, 2)] : "");
	else if (OBJ_TYPE(temp1) == ITEM_WEAPON)
	  sprintf(buf2,"%s [%s]",
		  STR(UNIT_TITLE_STRING(temp1)),
		  wpn_text[OBJ_VALUE(temp1, 0)]);
	else if (OBJ_TYPE(temp1) == ITEM_ARMOR ||
		 OBJ_TYPE(temp1) == ITEM_SHIELD)
	  sprintf(buf2, "%s", STR(UNIT_TITLE_STRING(temp1)));
	else
	  sprintf(buf2 ,"%s", STR(UNIT_TITLE_STRING(temp1)));

	currency = sd->currencies[0];
	price    = money_round_up((int) (obj_trade_price(temp1) *
					 sd->profit_buy),
				  currency, 2);

	if (diff_buf)
	  sprintf(b, "  %s (" COLOUR_MOB "%s" COLOUR_NORMAL ") for %s\n\r", buf2,
		  diff_buf, money_string(price, currency, FALSE));
	else
	  sprintf(b, "  %s for %s\n\r", buf2,
		  money_string(price, currency, FALSE));

	TAIL(b);
     }

   if (!found_obj)
     strcat(buf, "  Nothing!\n\r");

   send_to_char(buf, ch);
}


static void shopping_price(char *arg, struct unit_data *ch,
			   struct unit_data *keeper, struct shop_data *sd)
{
   struct unit_data *temp1;
   bool destruct = FALSE, can_pay = FALSE;
   char buf[256];
   currency_t currency = 0;
   amount_t price = 0;
   int i = 0;

   if (!is_ok(keeper,ch, sd))
     return;

   if (str_is_empty(arg))
   {
      act("$1n says 'What do you want to know the price of, $2n?'",
	  A_SOMEONE, keeper, ch, 0,  TO_ROOM);
      return;
   }

   if ((temp1 = find_unit(keeper, &arg, 0, FIND_UNIT_INVEN)) == NULL
       || IS_MONEY(temp1) || (destruct = (obj_trade_price(temp1) <= 0)))
   {
      act(sd->no_such_item1, A_SOMEONE, keeper, ch, 0, TO_ROOM);
      if (destruct)
	extract_unit(temp1);
      return;
   }

   for (i=0; !can_pay && (i < sd->currencycount); i++)
     if (sd->currencies[i] != -1)
     {
	currency = sd->currencies[i];
	price    = money_round_up((int) (obj_trade_price(temp1) *
					 sd->profit_buy),
				  currency, 2);
	can_pay  = char_can_afford(ch, price, currency);
     }

   /* If not, go with the standard. */
   if (!can_pay)
     currency = sd->currencies[0];

   sprintf(buf, "$1n says 'I want %s from you for $3n, $2n!'",
	   money_string(price, currency, TRUE));

   act(buf, A_SOMEONE, keeper, ch, temp1, TO_ROOM);
}


static void free_shop(struct shop_data *sd)
{
   if (sd->prod)          free(sd->prod);
   if (sd->types)         free(sd->types);
   if (sd->no_such_item1) free(sd->no_such_item1);
   if (sd->no_such_item2) free(sd->no_such_item2);
   if (sd->missing_cash1) free(sd->missing_cash1);
   if (sd->missing_cash2) free(sd->missing_cash2);
   if (sd->do_not_buy)    free(sd->do_not_buy);
   if (sd->message_buy)   free(sd->message_buy);
   if (sd->message_sell)  free(sd->message_sell);
   if (sd->time1)         free(sd->time1);
   if (sd->time2)         free(sd->time2);
   if (sd->currencies)    free(sd->currencies);
   if (sd)                free(sd);
}


/* Special routine for all the shopkeepers */
/* DON'T USE, use the init function */
int shop_keeper(struct spec_arg *sarg)
{
   struct shop_data *sd = (struct shop_data *) sarg->fptr->data;

   if (sarg->cmd->no == CMD_BUY)
     shopping_buy((char *) sarg->arg, sarg->activator, sarg->owner, sd);
   else if (sarg->cmd->no == CMD_SELL)
     shopping_sell((char *) sarg->arg, sarg->activator, sarg->owner, sd);
   else if (sarg->cmd->no == CMD_VALUE)
     shopping_value((char *) sarg->arg, sarg->activator, sarg->owner, sd);
   else if (sarg->cmd->no == CMD_REQUEST)
     shopping_price((char *) sarg->arg, sarg->activator, sarg->owner, sd);
   else if (sarg->cmd->no == CMD_LIST)
     shopping_list((char *) sarg->arg, sarg->activator, sarg->owner, sd);
   else if (sarg->cmd->no == CMD_AUTO_DEATH && sarg->owner == sarg->activator)
   {
      /* Get rid of contents */
      while (UNIT_CONTAINS(sarg->owner))
	extract_unit(UNIT_CONTAINS(sarg->owner));
   }
   else if (sarg->cmd->no == CMD_AUTO_EXTRACT)
   {
      free_shop(sd);
      sarg->fptr->data = NULL;
   }
   else
     return SFR_SHARE;
   
   return SFR_BLOCK;
}

static struct shop_data *parse_shop(struct unit_data *keeper, char *data)
{
   int i;
   struct shop_data *sd;
   char **names;

   if (data == NULL)
     return NULL;

   CREATE(sd, struct shop_data, 1);

   parse_match_num(&data, "Profit sell", &i);
   if (!is_in(i, 100, 500))
   {
      szonelog(UNIT_FI_ZONE(keeper),
	       "SHOP-ERROR (%s@%s): Illegal sell profit %d",
	       UNIT_FI_NAME(keeper), UNIT_FI_ZONENAME(keeper), i);
      i = 150;
   }
   sd->profit_buy = (float) i / 100.0;

   parse_match_num(&data, "Profit buy", &i);
   if (!is_in(i, 1, 99))
   {
      szonelog(UNIT_FI_ZONE(keeper),
	       "SHOP-ERROR (%s@%s): Illegal buy profit %d",
	       UNIT_FI_NAME(keeper), UNIT_FI_ZONENAME(keeper), i);
      i = 10;
   }
   sd->profit_sell  = (float) i / 100.0;

   /* read in currencies shopkeeper will accept
    * He will normally work with the 1st currency.
    * array is { DEF_CURRENCY } if none
    */
   sd->currencies =
     parse_match_numlist(&data, "Currencies", &sd->currencycount);

   if (sd->currencycount == 0)
   {
      sd->currencycount = 1;
      CREATE(sd->currencies, int, 1);
      sd->currencies[0] = DEF_CURRENCY;
   }

   for (i=0; i < sd->currencycount; i++)
     if ((sd->currencies[i] < 0) || (sd->currencies[i] > MAX_MONEY))
     {
	szonelog(UNIT_FI_ZONE(keeper),
	         "SHOP-ERROR (%s@%s): Illegal currency types",
		 UNIT_FI_NAME(keeper), UNIT_FI_ZONENAME(keeper));
	free_shop(sd);
	return NULL;
     }

   names = parse_match_namelist(&data, "Production");
   if (names == NULL)
     sd->prod = NULL;
   else
   {
      int j;

      for (i=0; names[i]; i++)
	;
      CREATE(sd->prod, struct file_index_type *, i + 1);

      for (j=i=0; names[i]; i++)
      {
	 sd->prod[j] = str_to_file_index(names[i]);
	 if (sd->prod[j])
	   j++;
	 else
	   szonelog(UNIT_FI_ZONE(keeper),
		    "SHOP-ERROR (%s@%s): Illegal file-index: %s",
		    UNIT_FI_NAME(keeper), UNIT_FI_ZONENAME(keeper), names[i]);
      }
      sd->prod[j] = NULL;
      free_namelist(names);
   }

   sd->types = parse_match_numlist(&data, "Trade types", &sd->typecount);
   for (i=0; i < sd->typecount; i++)
   {
      if (sd->types[i] == ITEM_MONEY)
      {
	 szonelog(UNIT_FI_ZONE(keeper),
		  "SHOP-ERROR (%s@%s): Shops can't trade money!",
		  UNIT_FI_NAME(keeper), UNIT_FI_ZONENAME(keeper));
	 sd->types[i] = -1;
      }
   }

   sd->no_such_item1 = parse_match_name(&data, "Msg1");
   if (sd->no_such_item1 == NULL)
     sd->no_such_item1 = str_dup("$1n says, 'I've got no such item!'");

   sd->no_such_item2 = parse_match_name(&data, "Msg2");
   if (sd->no_such_item2 == NULL)
     sd->no_such_item2 = str_dup("$1n says, '$3n, you haven't even got it!'");

   sd->do_not_buy    = parse_match_name(&data, "Msg3");
   if (sd->do_not_buy == NULL)
     sd->do_not_buy =
       str_dup("$1n says, 'I don't trade with things such as $2n'");

   sd->missing_cash1 = parse_match_name(&data, "Msg4");
   if (sd->missing_cash1 == NULL)
     sd->missing_cash1 = str_dup("$1n says, 'I can't afford it, sorry!'");

   sd->missing_cash2 = parse_match_name(&data, "Msg5");
   if (sd->missing_cash2 == NULL)
     sd->missing_cash2 = str_dup("$1n says, '$3n, you can't afford $2n'");

   sd->message_buy   = parse_match_name(&data, "Msg6");
   if (sd->message_buy == NULL)
     sd->message_buy = str_dup("$1n says, 'That's %s for $2n'");

   sd->message_sell  = parse_match_name(&data, "Msg7");
   if (sd->message_sell == NULL)
     sd->message_sell =
       str_dup("$1n says, 'Thank you $3n, here are %s for $2n.'");


   sd->time1 = parse_match_numlist(&data, "Hours1", &i);
   if ((i != 2) || (sd->time1[0] >= sd->time1[1]))
   {
      szonelog(UNIT_FI_ZONE(keeper),
	       "SHOP-ERROR (%s@%s): Illegal shop hours",
	       UNIT_FI_NAME(keeper), UNIT_FI_ZONENAME(keeper));
      free_shop(sd);
      return NULL;
   }

   sd->time2 = parse_match_numlist(&data, "Hours2", &i);
   if ((i != 2) || (sd->time2[0] >= sd->time2[1]))
   {
      szonelog(UNIT_FI_ZONE(keeper),
	       "SHOP-ERROR (%s@%s): Illegal shop hours",
	       UNIT_FI_NAME(keeper), UNIT_FI_ZONENAME(keeper));
      free_shop(sd);
      return NULL;
   }
   
   return sd;
}


int shop_init(struct spec_arg *sarg)
{
   struct shop_data *sd;

   if (sarg->cmd->no != CMD_AUTO_EXTRACT)
   {
      sd = parse_shop(sarg->owner, (char *) sarg->fptr->data);

      if (sarg->fptr->data)
      {
	 free(sarg->fptr->data);
	 sarg->fptr->data = NULL;
      }

      if (sd == NULL)
      {
	 szonelog(UNIT_FI_ZONE(sarg->owner),
		  "Shop-keeper destroyed due to error in data: %s@%s",
		  UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
	 destroy_fptr(sarg->owner, sarg->fptr);
      }
      else
      {
	 sarg->fptr->data  = (void *) sd;
	 sarg->fptr->index = SFUN_INTERN_SHOP;
	 return shop_keeper(sarg);
      }
   }

   return SFR_SHARE;
}
