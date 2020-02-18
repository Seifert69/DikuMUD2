/* *********************************************************************** *
 * File   : account.h                                 Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: To implement accounting commands, status, etc.                 *
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

#ifndef _MUD_ACCOUNT_H
#define _MUD_ACCOUNT_H

#define MAX_FLATRATE 2

struct flatrate_type
{
   char *pMessage;
   ubit16 days;
   ubit32 price;
};

class CAccountConfig
{
  public:
   CAccountConfig(void);

   void Boot(void);

   char *m_pCoinName;
   char *m_pOverdueMessage;
   char *m_pClosedMessage;
   char *m_pPaypointMessage;

   int  m_nMaxCharge;
   int  m_nMinCharge;
   int  m_nFreeFromLevel;
   int  m_nAccountFree;
   int  m_nAccountLimit;
   int  m_nHourlyRate;
   int  m_bCreditCard;

   struct flatrate_type m_flatrate[MAX_FLATRATE];
};

extern class CAccountConfig g_cAccountConfig;

void account_flatrate_change(struct unit_data *god, struct unit_data *whom,
			     sbit32 days);

void account_cclog(struct unit_data *ch, int amount);

void account_insert(struct unit_data *god, struct unit_data *whom,
		    ubit32 amount);
void account_withdraw(struct unit_data *god, struct unit_data *whom,
		      ubit32 amount);
void account_global_stat(const struct unit_data *ch);
void account_local_stat(const struct unit_data *ch, struct unit_data *u);

void account_defaults(struct unit_data *pc);

void account_subtract(struct unit_data *pc, time_t from, time_t to);

int account_is_overdue(const struct unit_data *ch);
void account_overdue(const struct unit_data *ch);

void account_paypoint(struct unit_data *ch);
void account_closed(struct unit_data *ch);
int  account_is_closed(struct unit_data *ch);

#endif
