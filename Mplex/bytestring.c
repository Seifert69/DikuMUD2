/* *********************************************************************** *
 * File   : bytestring.c                              Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Routines for writing and reading bytestrings.                  *
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "essential.h"
#include "bytestring.h"
#include "textutil.h"

ubit8 bread_ubit8(ubit8 **b)
{
   ubit8 i;

   memcpy((ubit8 *) &i, *b, sizeof(ubit8));
   *b += sizeof(ubit8);

   return i;
}


ubit16 bread_ubit16(ubit8 **b)
{
   ubit16 i;

   memcpy((ubit8 *) &i, *b, sizeof(ubit16));
   *b += sizeof(ubit16);

   return i;
}



ubit32 bread_ubit32(ubit8 **b)
{
   ubit32 i;

   memcpy((ubit8 *) &i, *b, sizeof(ubit32));
   *b += sizeof(ubit32);
   
   return i;
}

float bread_float(ubit8 **b)
{
   float f;

   memcpy((ubit8 *) &f, *b, sizeof(float));
   *b += sizeof(float);

   return f;
}


ubit8 *bread_data(ubit8 **b, ubit32 *plen)
{
   ubit32 len;
   ubit8 *data;

   data = NULL;
   len = bread_ubit32(b);
   if (plen)
     *plen = len;

   if (len > 0)
   {
      CREATE(data, ubit8, len);
      memcpy(data, *b, len);
      *b += len;
   }

   return data;
}

/* Stored: as Null terminated string            */
/* Copy string from **b into *str               */
void bread_strcpy(ubit8 **b, char *str)
{
   for (;(*str++ = **b); (*b)++)
     ;
   (*b)++;
}



/*  Stored: as Null terminated string
 *  Will allocate space for string, if the read
 *  string is one or more characters, and return
 *  pointer to allocated string (or 0)
 */
char *bread_str_alloc(ubit8 **b)
{
   if (**b)
   { 
      char *c, *t;
      t = (char *) *b;

      c = str_dup(t);

      *b += strlen(c) + 1;
      return c;
   }
   
   (*b)++;
   return NULL;
}

/* Returns pointer to the string and skips past the end to next
   point in buffer */
char *bread_str_skip(ubit8 **b)
{
   char *o = (char *) *b;

   TAIL(*b);
   (*b)++;

   return o;
}

/* Stored: As 'N' strings followed by the empty */
/* string ("")                                  */
/* Returns * to nameblock, nameblock may be     */
/* but is never null ({""}).                    */
char **bread_nameblock(ubit8 **b)
{
   char buf[MAX_STRING_LENGTH];
   char **nb;

   nb = create_namelist();

   for (;;)
   {
      bread_strcpy(b, buf);
      if (*buf)
	nb = add_name(buf, nb);
      else
	break;
   }

   return nb;
}


void bwrite_ubit8(ubit8 **b, ubit8 i)
{
   **b = i;
   *b += sizeof(ubit8);
}



void bwrite_ubit16(ubit8 **b, ubit16 i)
{
   memcpy(*b, (ubit8 *) &i, sizeof(ubit16));
   *b += sizeof(ubit16);
}



void bwrite_ubit32(ubit8 **b, ubit32 i)
{
   memcpy(*b, (ubit8 *) &i, sizeof(ubit32));
   *b += sizeof(ubit32);
}


void bwrite_float(ubit8 **b, float f)
{
   memcpy(*b, (ubit8 *) &f, sizeof(float));
   *b += sizeof(float);
}


void bwrite_data(ubit8 **b, ubit8 *data, ubit32 len)
{
   bwrite_ubit32(b, len);
   if (len > 0)
   {
      memcpy(*b, data, len);
      *b += len;
   }
}


/* String is stored as Null terminated string   */
/* Space is NOT allocated if string is 0 length */
/* but NIL is returned                          */
void bwrite_string(ubit8 **b, const char *str)
{
   if (str)
   {
      for(; *str; str++, (*b)++)
	 **b = *str;

      **b = '\0';
      *b += 1;
   }
   else
   {
      **b = '\0';
      *b += 1;
   }
}




/* Write a string of the format:  ssss\0ssss\0 */
void bwrite_double_string(ubit8 **b, char *str)
{
   int i;

   if (str)
   {
      for(i=0; i<2; str++, (*b)++, (*str ? 0 : i++))
	 **b = *str;

      **b = '\0';
      *b += 1;
   }
   else
   {
      bwrite_string(b, "");
      bwrite_string(b, "");
   }
}



/* Stored: As 'N' strings followed by the empty string ("") */
void bwrite_nameblock(ubit8 **b, char **nb)
{
   if (nb)
     for (; *nb && **nb; nb++)
       bwrite_string(b, *nb);

   bwrite_string(b, "");
}
