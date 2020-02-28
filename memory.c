/* *********************************************************************** *
 * File   : memory.c                                  Part of Valhalla MUD *
 * Version: 1.10                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Trapping illegal free() and realloc() calls.                   *
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

/* 28/03/93 gnort  : Fixed sanitycheck error                               */
/* 10/04/94 seifert: Added memory alloc bytes total.                       */
/* 29/09/94 seifert: Added overhead count and changed CRC to one byte.     */
/*                   This makes the system more shaky, but less mem hungry */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "essential.h"

#define CRC_SIZE 4
#define RAN_SIZE 4

#ifdef MEMORY_DEBUG /* Endif is at the very bottom! */

ubit32 memory_total_limit     = 64L * 1024L * 1024L;

ubit32 memory_total_alloc     = 0L;
ubit32 memory_total_overhead  = 0L;
ubit32 memory_dijkstra_alloc  = 0L;
ubit32 memory_pc_alloc        = 0L;
ubit32 memory_npc_alloc       = 0L;
ubit32 memory_obj_alloc       = 0L;
ubit32 memory_room_alloc      = 0L;
ubit32 memory_roomread_alloc  = 0L;
ubit32 memory_zoneidx_alloc   = 0L;
ubit32 memory_zonereset_alloc = 0L;

#define MAX_MEMORY_CHUNK (1000000L)  /* Purely for sanity checking! */

void memory_status(char *Buf)
{
   sprintf(Buf, "\n\rMemory Status:\n\r"
           "   %8d bytes soft coded memory limit.\n\r"
	   "   %8d safe-chunks allocated (%d bytes overhead)\n\r"
	   "   %8d bytes Total safe malloced memory.\n\r"
	   "   %8d bytes used by dijkstra.\n\r"
	   "   %8d bytes used by Zone indexes\n\r"
	   "   %8d bytes used by room allocation\n\r"
	   "   %8d bytes used by boottime reset of all zones.\n\r"
	   "   %8d bytes used by PC alloc (very approx).\n\r"
	   "   %8d bytes used by NPC alloc (very approx).\n\r"
	   "   %8d bytes used by OBJ alloc (very approx).\n\r"
	   "   %8d bytes used by ROOM alloc (very approx).\n\r",
           memory_total_limit,
	   memory_total_overhead,
	   memory_total_overhead*(RAN_SIZE+CRC_SIZE),
	   memory_total_alloc,
	   memory_dijkstra_alloc,
	   memory_zoneidx_alloc,
	   memory_roomread_alloc,
	   memory_zonereset_alloc,
	   memory_pc_alloc,
	   memory_npc_alloc,
	   memory_obj_alloc,
	   memory_room_alloc);
}
#else
void memory_status(char *Buf)
{
   sprintf(Buf, "\n\rMemory Status:\n\r"
	   "   Memory debug is off.\n\r");
}
#endif

#ifdef MEMORY_DEBUG /* Endif is at the very bottom! */
   #undef malloc
   #undef calloc
   #undef free
   #undef realloc


/* Set debug info into allocated memory block (size + info_Size) */
/* Save no of bytes alloced as RAN_SIZE                          */
void safe_set_info(void *p, ubit32 len)
{
   ubit32 crc;
   ubit16 low, high;
   ubit8 *ptr;

   assert(len > 0);

   if (p == NULL)
   {
      perror("Unable to satisfy memory allocation request");
      assert(FALSE);
   }

   crc = len ^ 0xAAAAAAAA;

   ptr = (ubit8 *) p;

   low  = crc & 0xFFFF;
   high = crc>>16 & 0xFFFF;

   memcpy(ptr, &low, sizeof(ubit16));
   memcpy(sizeof(ubit16) + ptr, &len, sizeof(len));
   ptr = (ubit8 *) p + len - CRC_SIZE/2;
   memcpy(ptr, &high, sizeof(ubit16));
}


/* Reset and check debug info from memory block */
/* Return len of data checkked.                 */
ubit32 safe_check_info(void *p)
{
   ubit32 crc;
   ubit16 low, high;
   ubit32 len;
   ubit8 *pEnd;

   if (!p)
   {
      perror("NULL pointer to memory block.");
      assert(FALSE);
   }

   pEnd = (ubit8 *) p;
   memcpy(&low, pEnd, sizeof(ubit16));
   memcpy(&len, sizeof(ubit16) + pEnd, sizeof(len));

   assert(len < MAX_MEMORY_CHUNK);

   pEnd = (ubit8 *)p + len - CRC_SIZE/2;
   memcpy(&high, pEnd, sizeof(ubit16));

   crc = (((ubit32) high) <<16) | (ubit32) low;

   if (crc != (len ^ 0xAAAAAAAA))
   {
      perror("CRC mismatch!");
      assert(FALSE);
   }

   return len;
}


void *safe_malloc(size_t size)
{
   void *p;

   assert(size > 0);
   assert(size < MAX_MEMORY_CHUNK);

   memory_total_alloc += size + RAN_SIZE + CRC_SIZE;
   assert(memory_total_alloc < memory_total_limit);

   p  = malloc(size + RAN_SIZE + CRC_SIZE);
   memory_total_overhead++;

   safe_set_info(p, size + RAN_SIZE + CRC_SIZE);

   return (ubit8 *) p + RAN_SIZE + CRC_SIZE/2; /* Skip control info */
}


void *safe_realloc(void *p, size_t size)
{
   assert(size > 0);

   p = (ubit8 *)p - (RAN_SIZE + CRC_SIZE / 2);

   memory_total_alloc -= safe_check_info(p);
   memory_total_alloc += size + RAN_SIZE + CRC_SIZE;
   assert(memory_total_alloc < memory_total_limit);

   p = realloc(p, size + RAN_SIZE + CRC_SIZE);
   safe_set_info(p, size + RAN_SIZE + CRC_SIZE);


   return (ubit8 *) p + RAN_SIZE + CRC_SIZE/2; /* Skip control info */
}


void *safe_calloc(size_t nobj, size_t size)
{
   void *p;
   size_t sum;

   sum = nobj*size;
   p = safe_malloc(sum);
   memset(p, 0, sum);

   return p;
}


void safe_free(void *p)
{
   ubit32 len;

   if (p == NULL)
     assert(FALSE);
   p = (ubit8 *) p - (RAN_SIZE + CRC_SIZE/2);
   len = safe_check_info(p);
   memset(p, 255, len);

   memory_total_alloc -= len;
   memory_total_overhead--;

   free((ubit8 *) p );
}
#endif

