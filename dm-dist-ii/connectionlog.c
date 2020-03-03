/* *********************************************************************** *
 * File   : connectionlog.c                           Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Connection log.                                                *
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

#include <stdio.h>
#include <time.h>

#include "essential.h"
#include "values.h"
#include "utils.h"

#include "files.h"
#include "textutil.h"
#include "common.h"
#include "db.h"
#include "account.h"
#include "connectionlog.h"


static void ConnectionWrite(ubit32 nId, ubit32 nLogon,
			    ubit32 nLogoff, ubit32 nLanPay)
{
   FILE *f = fopen_cache(str_cc(libdir, STATISTIC_FILE), "ab+");

   assert(f);

   ubit8 buf[40], *b = buf;

   bwrite_ubit32(&b, nId);
   bwrite_ubit32(&b, nLogon);
   bwrite_ubit32(&b, nLogoff);
   bwrite_ubit32(&b, nLanPay);

   fwrite(buf, sizeof(ubit8), b - buf, f);

   fflush(f);
}


void ConnectionBegin(class unit_data *pc)
{
   ConnectionWrite( PC_ID(pc), CHAR_DESCRIPTOR(pc)->logon, 0, 0);
}


void ConnectionLog(class unit_data *pc)
{
   time_t t0 = time(0);

   ubit32 nLanPay = 0;

   if (g_cServerConfig.FromLAN(CHAR_DESCRIPTOR(pc)->host))
     nLanPay |= 0x0001;

   if (g_cServerConfig.m_bAccounting)
     nLanPay |= 0x0002;

   if (CHAR_LEVEL(pc) < g_cAccountConfig.m_nFreeFromLevel)
     nLanPay |= 0x0004;

   if (PC_ACCOUNT(pc).flatrate > (ubit32) time(0))
     nLanPay |= 0x0008;

   if (PC_ACCOUNT(pc).discount >= 100)
     nLanPay |= 0x0010;

   if (PC_ACCOUNT(pc).credit > 0.0)
     nLanPay |= 0x0020;

   ConnectionWrite( PC_ID(pc), CHAR_DESCRIPTOR(pc)->logon, t0, nLanPay);
}

void ConnectionBoot(void)
{
   touch_file(str_cc(libdir, STATISTIC_FILE));
   ConnectionWrite( 0xFFFFFFFF, time(0), 0xFFFFFFFF, 0xFFFFFFFF);
}

