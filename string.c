/* *********************************************************************** *
 * File   : string.c                                  Part of Valhalla MUD *
 * Version: 1.03                                                           *
 * Author : seifert@diku.dk / gnort@daimi.aau.dk                           *
 *                                                                         *
 * Purpose: To make a string allocation scheme for known constant strings. *
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
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "utils.h"
#include "textutil.h"
#include "comm.h"
#include "utility.h"
#include "hashstring.h"


#define STATISTICS 1


/* MS: there is a LOT of room for optimizations! Pass on hash-value
       exploit strlen in compare, use memcmp instead of strcmp. */


#if STATISTICS
unsigned long int alloced_strings    = 0;
unsigned long int alloced_memory     = 0;
unsigned long int potential_memory   = 0;
unsigned long int referenced_strings = 0;
#endif

#define HASH_SIZE 1511 /* MUST be prime! */

class cHashSystem
{
  public:
   cHashSystem(void);
   ubit32 Hash(const char *str);
   void Insert(class cStringConstant *p, ubit32 nHash);
   void Remove(class cStringConstant *p);
   class cStringConstant *Lookup(const char *str, ubit32 nLen, ubit32 nHash);
   ubit32 MaxDepth(ubit32 *);

  private:
   class cStringConstant *str_table[HASH_SIZE];
};

class cHashSystem Hash;

/* ===================================================================== */

cHashSystem::cHashSystem(void)
{
   int i;

   for (i=0; i < HASH_SIZE; i++)
     str_table[i] = NULL;
}

/*
 * Hash function hashpjw, from Aho et al's
 * "Compilers: Principles, Techniques and Tools", page 436
 */

ubit32 cHashSystem::MaxDepth(ubit32 *slots)
{
   class cStringConstant *tmp;
   ubit32 depth, i, tmpd;

   if (slots)
     *slots = 0;

   for (depth = i = 0; i < HASH_SIZE; ++i)
     if (str_table[i])
     {
	if (slots)
	  ++(*slots);
	for (tmpd = 0, tmp = str_table[i]; tmp; tmp = tmp->Next())
	  ++tmpd;
	depth = MAX(depth, tmpd);
     }

   return depth;
}

ubit32 cHashSystem::Hash(const char *str)
{
   register ubit32 h = 0, g;
  
   for (; *str; ++str)
     if ((g = ((h = (h << 4) + *str) & 0xf0000000)))
       h = (h ^ (g >> 24)) ^ g;

   return h % HASH_SIZE;
}


class cStringConstant *cHashSystem::Lookup(const char *str, ubit32 nLen,
					   ubit32 nHash)
{
   class cStringConstant *lookup;

   assert(nHash < HASH_SIZE);

   for (lookup = str_table[nHash]; lookup; lookup = lookup->pNext)
     if (nLen == lookup->nStrLen &&
	 memcmp(str, lookup->String(), nLen) == 0)
       return lookup;

   return NULL;
}

void cHashSystem::Insert(class cStringConstant *p, ubit32 nHash)
{
   p->pNext         = str_table[nHash];
   str_table[nHash] = p;
}

void cHashSystem::Remove(class cStringConstant *r)
{
   ubit32 h;
   class cStringConstant *o, *p;
  
   h = Hash(r->pStr); // Can improve speed by storing in struct.

   if (r == str_table[h])
     str_table[h] = str_table[h]->pNext;
   else
   {
      for (p = str_table[h], o = p->pNext; o; p = o, o = o->pNext)
	if (r == o)
	{
	   p->pNext = o->pNext;
	   break;
	}

      assert(r == o);
   }
}

/* ===================================================================== */


cStringConstant::cStringConstant(const char *str, ubit32 len, ubit32 nHash)
{
   class cStringConstant *tmp;

   assert(str);

   if ((tmp = Hash.Lookup(str, len, nHash)))
     tmp->nReferences++;
   else
   {
      nReferences  = 1;
      pStr         = str_dup(str);
      nStrLen      = len;

      Hash.Insert(this, nHash);

#if STATISTICS
      ++alloced_strings;
      alloced_memory += nStrLen;
#endif
   }
}


cStringConstant::~cStringConstant(void)
{
   Hash.Remove(this);
   free(pStr);
   
#if STATISTICS
   --alloced_strings;
   alloced_memory   -= nStrLen;
#endif
}

/* ===================================================================== */


void cStringInstance::Make(const char *str)
{
   if (str_is_empty(str))
   {
      pConst = NULL;
      return;
   }

   ubit32 h = Hash.Hash(str);
   ubit32 len = strlen(str);

   pConst = Hash.Lookup(str, len, h);
   
   if (pConst == NULL)
     pConst = new cStringConstant(str, len, h);
   else
     pConst->nReferences++;

#if STATISTICS
   ++referenced_strings;
   potential_memory += Length();
#endif
}


void cStringInstance::Reassign(const char *str)
{
   if (pConst)
   {
#if STATISTICS
      --referenced_strings;
      potential_memory -= Length();
#endif

      if (--pConst->nReferences == 0)
	delete pConst;
   }

   Make(str);
}

cStringInstance::cStringInstance(void)
{
   Make(NULL);
}

cStringInstance::cStringInstance(const char *str)
{
   Make(str);
}


cStringInstance::~cStringInstance(void)
{
   if (pConst)
   {
#if STATISTICS
      --referenced_strings;
      potential_memory -= Length();
#endif
      
      if (--pConst->nReferences == 0)
	delete pConst;
   }
}

/* ===================================================================== */

#ifdef DMSERVER

void string_statistics(struct unit_data *ch)
{
   ubit32 depth, slots;

#if STATISTICS
   char buf[4096];
   int overhead = (int) (sizeof Hash +
			 alloced_strings * sizeof(class cStringConstant));
   unsigned long int usage = alloced_memory + overhead;

   sprintf(buf,
	   "  %lu allocated strings with %lu references.\n\r"
	   "  %lu allocated bytes (plus overhead %d = %lu bytes),\n\r"
	   "  potentially allocated string-memory: %lu.\n\r"
	   "  saved memory: %ld bytes\n\r",
	   alloced_strings, referenced_strings,
	   alloced_memory, overhead, usage,
	   potential_memory, (long int) (potential_memory - usage));

#else
   char *buf = "  Not available.\n\r";
#endif

   send_to_char(buf, ch);

   depth = Hash.MaxDepth(&slots);

   sprintf(buf, "  Slots used in hash-list and deepest entry: %d/%d -- %d\n\r",
	   slots, HASH_SIZE, depth);

   send_to_char(buf, ch);
}

#endif

/* ===================================================================== */


/* Prime-finder proggy:
main() {
  static char array[10000];
  int i, t;
  for (i = 2; i < 10000; i++)
    if (!array[i]) {
      printf("%d\n", i);
      for (t = i * 2; t < 10000; t += i)
        array[t] = 1;
    }
}
*/

