/* *********************************************************************** *
 * File   : utility.c                                 Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : all.                                                           *
 *                                                                         *
 * Purpose: Various functions.                                             *
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
#include <time.h>
#include <stdarg.h>  /* va_args in slog()        */

#include "structs.h"
#include "utils.h"
#include "files.h"
#include "textutil.h"
#include "comm.h"
#include "db.h"
#include "utility.h"
#include "unixshit.h"


#ifndef HPUX
int MIN(int a, int b)
{
   return ((a < b) ? a : b);
}


int MAX(int a, int b)
{
   return ((a > b) ? a : b);
}
#endif /* HPUX */

#ifdef SUSPEKT /* Unused */
/* Entries is range of indexes (0..entries-1). Useful for arrays */
/* of strings like "bad", "average", "medium" with values.       */
int string_index(int entries, int value, int minv, int maxv)
{
   int step;
   int idx;

   step = (maxv - minv + 1) / entries;

   if (step == 0)
     return 0;

   idx = (value - minv) / step;

   /* Just for safety in case of illegal parameters */
   idx = MAX(0, idx);
   idx = MIN(idx, entries - 1);

   return idx;
}
#endif

#ifdef SOLARIS
long lrand48();
#endif

/* creates a random number in interval [from;to] */
int number(int from, int to)
{
   if (from > to)
   {
      slog(LOG_ALL, 0, "From %d and to %d in number()!", from, to);
      return (to-from)/2;
   }

#ifdef GENERIC_SYSV
   return((lrand48() % (to - from + 1)) + from);
#else
   return (int) ((rand() % (to - from + 1)) + from);
#endif
}



/* simulates dice roll */
int dice(int number, int size)
{
  int r;
  int sum = 0;

  assert(size >= 1);

  for (r = 1; r <= number; r++)
#ifdef GENERIC_SYSV
    sum += ((lrand48() % size) + 1);
#else
    sum += ((rand() % size) + 1);
#endif

  return sum;
}

struct log_buffer log_buf[MAXLOG];

/* writes a string to the log */
void slog(enum log_level level, ubit8 wizinv_level, const char *fmt, ...)
{
   static ubit8 idx = 0;
   static ubit32 log_size = 0;

   char buf[MAX_STRING_LENGTH], *t;
   va_list args;

   time_t now = time(0);
   char *tmstr = ctime(&now);

   tmstr[strlen(tmstr) - 1] = '\0';

   if (wizinv_level > 0)
     sprintf(buf, "(%d) ", wizinv_level);
   else
     *buf = '\0';
      
   t = buf;
   TAIL(t);

   va_start(args, fmt);
   vsprintf(t, fmt, args);
   va_end(args);

   /* 5 == " :: \n";  24 == tmstr (Tue Sep 20 18:41:23 1994)*/
   log_size += strlen(buf) + 5 + 24;
  
   if (log_size > 4000000) /* 4 meg is indeed a very big logfile! */
   {
      fprintf(stderr, "Log-file insanely big!  Going down.\n");
      abort(); // Dont use error, it calls syslog!!! *grin*
   }

   fprintf(stderr, "%s :: %s\n", tmstr, buf);
   
   if (level > LOG_OFF)
   {
      log_buf[idx].level = level;
      log_buf[idx].wizinv_level = wizinv_level;
      strncpy(log_buf[idx].str, buf, sizeof(log_buf[idx].str)-1);
      log_buf[idx].str[sizeof(log_buf[idx].str)-1] = 0;

      idx++;
      idx %= MAXLOG;  /* idx = 1 .. MAXLOG-1 */

      log_buf[idx].str[0] = 0;
   }
}

/* MS: Moved szonelog to handler.c to make this module independent. */

/*  Replacing slog/assert(FALSE)
 *  usage: error(HERE, "Bad id: %d", id);
 */
void error(const char *file, int line, const char *fmt, ...)
{
  char buf[512];
  va_list args;

  va_start(args, fmt);
  vsprintf(buf, fmt, args);
  va_end(args);

  slog(LOG_OFF, 0, "%s:%d: %s", file, line, buf);

  abort();
}


char *sprintbit(char *buf, ubit32 vektor, const char *names[])
{
  char *result = buf;
  long nr;

  *result = '\0';

  for (nr = 0; vektor; vektor >>= 1, nr += names[nr] ? 1 : 0)
    if (IS_SET(1, vektor))
    {
      sprintf(result, "%s ", names[nr] ? names[nr] : "UNDEFINED");
      TAIL(result);
    }

  if (!*buf)
    strcpy(buf, "NOBITS");

  return buf;
}
/* MS2020. What a messed up function :)) Looks like noone calls with with
           anywhting but NULL as first parameter
*/
const char *sprinttype(char *buf, int type, const char *names[])
{
  const char *str;
  int nr;

  for (nr = 0; names[nr]; nr++)
    ;

  str = (0 <= type && type < nr) ? (char *) names[type] : "UNDEFINED";

  if (buf)
    return strcpy(buf, str);
  else
    return str;
}

