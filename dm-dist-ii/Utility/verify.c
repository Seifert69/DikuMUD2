/* *********************************************************************** *
 * File   : verify.c                                  Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : Unknown.                                                       *
 *                                                                         *
 * Purpose: Unknown.                                                      **
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

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "essential.h"

int main(int argc, char *argv[])
{
   char Buf[1000];
   time_t created, lastcrc, first_crc;

   char *msbuf = fgets(Buf, sizeof(Buf), stdin);
   sscanf(Buf, "%08x%08x", (ubit32 *) &created, (ubit32 *) &lastcrc);
   first_crc = created;
   created ^= 0xAF876162;

   printf("Account File Created at [%d] %s",
	  (ubit32) created, ctime(&created));
   printf("Initial CRC is [%08x]\n", (ubit32) first_crc);

   while (!feof(stdin))
   {
      char name1[100], name2[100];
      char action;
      int amount1, mxor, gid, pid, crc, amount, total, next_crc, check;
      time_t now;

      char *msbuf2 = fgets(Buf, sizeof(Buf), stdin);
      if (feof(stdin))
	break;

      sscanf(Buf,
	     "%c %s %s %d %*01x%08x%08x%08x%08x%08x%08x%08x%08x\n",
	     &action, name1, name2, &amount1,
	     (ubit32 *)&mxor, (ubit32 *)&gid, (ubit32 *)&crc,
	     (ubit32 *) &pid, (ubit32 *) &amount, (ubit32*) &total,
	     (ubit32 *) &next_crc, (ubit32 *) &now);

      check = gid + pid + total + amount + (ubit32) now;

      mxor       = ~mxor;
      next_crc ^= mxor;

      gid   ^= mxor;
      pid   ^= (mxor << 1);

      amount ^= mxor << 2;
      amount -= 13;
      total  ^= mxor << 3;
      total  -= 17;

      crc    ^= mxor << 4;

      printf("%c %s(%4d) %s(%4d) %5d [%6d]  %s",
	     action, name1, gid, name2, pid, amount, total,
	     ctime(&now));

      if (amount != amount1)
	printf("Amount mismatch!\n");

      if (crc != check)
	printf("\nCRC mismatch: %08x versus %08x\n",
	       crc, check);

      if (first_crc != next_crc)
	printf("Dependancy check [%08x] [%08x] [%08x] [%08x]!\n",
	       (ubit32) first_crc, next_crc,
	       (ubit32) first_crc ^ mxor, next_crc ^ mxor);

      first_crc = next_crc ^ mxor;
   }

   return 0;
}


