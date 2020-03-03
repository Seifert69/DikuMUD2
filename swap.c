/* *********************************************************************** *
 * File   : swap.c                                    Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Swapping handling for the server.                              *
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

#include <strings.h>
#include "utils.h"
#include "db_file.h"
#include "textutil.h"
#include "db.h"
#include "comm.h"
#include "utility.h"
#include "handler.h"
#include "main.h"
#include "files.h"

#define SWAP_FILE_NAME  "swap.blk"
#define SWAP_BLK_LENGTH 512

extern struct unit_data  *unit_list;

static BLK_FILE *swap_file = NULL;
static int nSwapNoRooms = 0;
static int nSwapNoNpcs = 0;
static int nSwapNoObjs = 0;

static int nSwapOut = 0;
static int nSwapIn  = 0;
static int nSwapSumLen = 0;

void swap_in(struct unit_data *u)
{
  return;
     /*
  CByteBuffer Buf;
   ubit8 *p;

   assert(UNIT_SWAP(u) == NULL);

   if (UNIT_SWAP_HANDLE(u) == BLK_NULL) // In '.data' file
   {
      FILE *f;
      char str[256];
      ubit8 version;

      assert(UNIT_FILE_INDEX(u));

      sprintf(str, "%s%s.data", zondir, UNIT_FILE_INDEX(u)->zone->filename);
      if ((f = fopen_cache(str, "rb")) == NULL)
	error(HERE, "Couldn't open %s for reading.", str);

      int e = Buf.FileRead(f, UNIT_FILE_INDEX(u)->filepos,
		           UNIT_FILE_INDEX(u)->length);

      assert(e == (int) UNIT_FILE_INDEX(u)->length);

      Buf.Skip8();
      Buf.SkipNames();

      bread_swap(&Buf, &UNIT_SWAP(u));

      return;
   }

   blk_length n;
   p = (ubit8 *) blk_read(swap_file, UNIT_SWAP_HANDLE(u), &n);
   assert(p != NULL);  // If read error here, we are fucked bad

   Buf.SetData(p, n);

   nSwapSumLen -= n;

   blk_delete(swap_file, UNIT_SWAP_HANDLE(u));

   UNIT_SWAP_HANDLE(u) = BLK_NULL;
   
   bread_swap(&Buf, &UNIT_SWAP(u));

   switch (UNIT_TYPE(u))
   {
     case UNIT_ST_ROOM:
      nSwapNoRooms--;
      break;
     case UNIT_ST_NPC:
      nSwapNoNpcs--;
      break;
     case UNIT_ST_OBJ:
      nSwapNoObjs--;
      break;
     default:
      abort();
   }
   nSwapIn++;
*/
}


void swap_out(struct unit_data *u)
{
  return;
/*
   CByteBuffer *pBuf = &g_FileBuffer;
   ubit8 *p;

   assert(UNIT_SWAP(u) != NULL);

   pBuf->Clear();
   bwrite_swap(pBuf, u);

   UNIT_SWAP_HANDLE(u) = blk_write(swap_file,
				   pBuf->GetData(),
				   pBuf->GetLength());

   delete UNIT_SWAP(u);
   UNIT_SWAP(u) = NULL;

   nSwapOut++;
   nSwapSumLen += pBuf->GetLength();

   switch (UNIT_TYPE(u))
   {
     case UNIT_ST_ROOM:
      nSwapNoRooms++;
      break;
     case UNIT_ST_NPC:
      nSwapNoNpcs++;
      break;
     case UNIT_ST_OBJ:
      nSwapNoObjs++;
      break;
     default:
      abort();
   }
*/
}

void swap_status(struct unit_data *ch)
{
  return;
  /*
   char Buf[1024];

   sprintf(Buf, "\n\rSwapping status:\n\r"
	   "   %7d Rooms swapped.\n\r"
	   "   %7d Objects swapped.\n\r"
	   "   %7d Mobiles swapped.\n\r"
	   "   %7d swaps from disk & %d swaps to disk.\n\r"
	   "   %7d bytes total swapped, %d average swap length.\n\r",
	   nSwapNoRooms, nSwapNoObjs, nSwapNoNpcs,
	   nSwapIn, nSwapOut, nSwapSumLen,
	   (nSwapOut-nSwapIn) > 0 ? nSwapSumLen / (nSwapOut-nSwapIn) : 0);
   send_to_char(Buf, ch);
  */
}

void swap_check(void *p1, void *p2)
{
  return;
  /*
   struct unit_data *u;

#ifdef SUSPEKT
   static int nOldSwapIn = 0;
   if (nOldSwapIn != nSwapIn)
   {
      // We had swap in activity, lets wait until we get an idle moment
      nOldSwapIn = nSwapIn;
      return;
   }
#endif

   event_enq(PULSE_SEC*60*5, swap_check, 0, 0);

   // Find some stuff to swap out... This is not very pretty, but unless
   // I put in all kind of timing shit, I see no better way. The reason
   // for not putting in timing is efficiency.

   for (u = unit_list; u; u = u->gnext)
   {
      if (UNIT_SWAP(u) && UNIT_FILE_INDEX(u) &&
	  UNIT_FILE_INDEX(u)->no_in_mem == 1)
      {
	 if (number(0, 25) != 0)
	   continue;

	 swap_out(u);
      }
   }
  */
}

void boot_swap(void)
{
  return;
  /*
   FILE *f;

   extern char libdir[];

   // Create swapfile
   if (!(f=fopen(str_cc(libdir, SWAP_FILE_NAME), "w")))
     abort();
   fclose(f);

   swap_file = blk_open(str_cc(libdir, SWAP_FILE_NAME), SWAP_BLK_LENGTH);
  */
}
