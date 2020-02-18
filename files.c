/* *********************************************************************** *
 * File   : files.c                                   Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : all.                                                           *
 *                                                                         *
 * Purpose: Various routines for administering files.                      *
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
#include <ctype.h>

#include "utils.h"
#include "utility.h"
#include "textutil.h"
#include "unixshit.h"
#include "files.h"


/* Returns the size of a file in bytes */
long fsize(FILE *f)
{
   long oldpos, size;

   oldpos = ftell(f);

   if (fseek(f, 0L, SEEK_END))    /* Seek to end of file */
      assert(FALSE);

   size = ftell(f);

   if (fseek(f, oldpos, SEEK_SET))    /* Seek to end of file */
      assert(FALSE);

   return size;
}


/* check if a file exists */
ubit1 file_exists(const char *name)
{
  FILE *fp;

  if ((fp = fopen(name, "r")) == NULL)
    return FALSE;

  fclose(fp);
  return TRUE;
}


/* create a file if it doesn't exist. if error, terminate */
void touch_file(char *name)
{
  FILE *fp;

  if (file_exists(name))
    return;

  if ((fp = fopen(name, "w")) == NULL)
  {
    fprintf(stderr, "touch_file(): Couldn't create %s...\n", name);
    assert(FALSE);
  }
  fclose(fp);
}

char *fread_line_commented(FILE *fl, char *buf, int max)
{
   char *s;

   for (;;)
   {
      s = fgets(buf, max, fl);

      if (s == NULL)
	break;

      if (*skip_spaces(buf) != '#')
	break;
   }

   return s;
}


/* read and allocate space for a '~'-terminated string from a given file */
char *fread_string_copy(FILE *fl, char *buf, int max)
{
   char *obuf;
   register char *point;
   int flag, total;

   memset(buf, '\0', max);
   total=0;
   obuf = buf;

   do
   {
      if (!fgets(buf, max - total, fl))
      {
	 error(HERE, "fread_string_copy");
      }

      total += strlen(buf);

      if (total >= max)
      {
	 error(HERE, "fread_string_copy: string too large (db.c)");
      }

      for (point = buf + strlen(buf) - 2; point >= buf && isspace(*point);
	   point--)   ;

      if ((flag = (*point == '~')))
	if (*(buf + strlen(buf) - 3) == '\n')
	{
	   *(buf + strlen(buf) - 2) = '\r';
	   *(buf + strlen(buf) - 1) = '\0';
	}
	else
	  *(buf + strlen(buf) -2) = '\0';
      else
      {
	 *(buf + strlen(buf) + 1) = '\0';
	 *(buf + strlen(buf)) = '\r';
      }
      TAIL(buf);
   }
   while (!flag);

   return obuf;
}


/* read and allocate space for a '~'-terminated string from a given file */
char *fread_string(FILE *fl)
{
   char buf[MAX_STRING_LENGTH];

   fread_string_copy(fl, buf, MAX_STRING_LENGTH);

   if (strlen(buf) > 0)
     return str_dup(buf);
   else
     return 0;
}

/* Read contents of a file, but skip all remark lines and blank lines. */
int config_file_to_string(char *name, char *buf, int max_len)
{
   FILE *fl;
   char tmp[500];

   *buf = '\0';

   if (!(fl = fopen(name, "r")))
   {
      DEBUG("File-to-string error.\n");
      *buf = '\0';
      return(-1);
   }

   do
   {
      if (fgets(tmp, sizeof(tmp) - 1, fl))
      {
	 if (tmp[0] == '#')
	   continue;

	 if (tmp[0] == 0)
	   continue;

	 if (strlen(buf) + strlen(tmp) + 2 > (ubit32) max_len)
	 {
	    DEBUG("fl->strng: string too big (db.c, file_to_string)\n");
	    strcpy(buf + strlen(buf) - 20, "TRUNCATED!");
	    fclose(fl);
	    return(-1);
	 }

	 strcat(buf, tmp);
	 *(buf + strlen(buf) + 1) = '\0';
	 *(buf + strlen(buf)) = '\r';
      }
   }
   while (!feof(fl));

   fclose(fl);

   return(0);
}


/* read contents of a text file, and place in buf */
int file_to_string(char *name, char *buf, int max_len)
{
   FILE *fl;
   char tmp[500];

   *buf = '\0';

   if (!(fl = fopen(name, "r")))
   {
      slog(LOG_ALL, 0, "File-to-string error");
      *buf = '\0';
      return(-1);
   }

   do
   {
      if (fgets(tmp, sizeof(tmp)-1, fl))
      {
	 if (strlen(buf) + strlen(tmp) + 2 > (ubit32) max_len)
	 {
	    slog(LOG_ALL,0,"fl->strng: string too big (db.c, file_to_string)");
	    strcpy(buf + strlen(buf) - 20, "TRUNCATED!");
	    return -1;
	 }

	 strcat(buf, tmp);
	 *(buf + strlen(buf) + 1) = '\0';
	 *(buf + strlen(buf)) = '\r';
      }
   }
   while (!feof(fl));

   fclose(fl);

   return 0;
}

long fread_num(FILE *fl)
{
   long tmp;

   int ms2020 = fscanf(fl, " %ld ", &tmp);
   return(tmp);
}


/* Read a null terminated string from file into str */
void fstrcpy(CByteBuffer *pBuf, FILE *f)
{
   int c;

   pBuf->Clear();

   while ((c = fgetc(f)) && (c != EOF))
     pBuf->Append8(c);

   pBuf->Append8(0);
}

/************************************************************************
 *  General purpose file Handling Procedures Below this line            *
 ********************************************************************** */

/* fopen_cache below this comment */

struct fcache_type
{
  int hits;
  int name_s;
  char *name;
  FILE *file;
};

static struct fcache_type fcache[FCACHE_MAX] =
{
  {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0},
  {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0},
  {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0},
  {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0},
  {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0},
  {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0},
  {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0},
  {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0},
  {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0},
  {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0},
  {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0},
  {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0},
  {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0},
  {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0},
  {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0},
  {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0},
  {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0},
  {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0},
  {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0},
  {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}
};


char *enl_strcpy(char *dest, char *source, int *dest_size)
{
   int len = strlen(source);

   if ((dest == NULL) || (*dest_size < len + 1))
   {
      if (dest)
	RECREATE(dest, char, len + 1);
      else
	CREATE(dest, char, 1 + len);
      
      *dest_size = len + 1;
   }

   return strcpy(dest, source);
}


/* The mode is interpreted as follows:

   If mode contains a 'w' then the file is always opened as "w+"
   If mode contains an 'r' then the file is initially opened as "r+"
   If mode contains an 'a' then the file is initially opened as "a+"

Then succeeding calls to fopen cache with any mode (but 'w') will cause
a fseek to the end or start (for 'a' or 'r'). Thus performance is gained,
BUT 'read-only' files may be written to!

*/

FILE *fopen_cache(char *name, const char *mode)
{
   int i, min_i, hit_i;
   static int pure_hits = 0, purge = 0;

   min_i = number(0, FCACHE_MAX - 1); /* In case many are equal */
   hit_i = -1;

   for (i = 0; i < FCACHE_MAX; i++)
   {
      if (fcache[i].hits < fcache[min_i].hits)
	min_i = i;
      if (fcache[i].name && !strcmp(name, fcache[i].name))
	hit_i = i;
      fcache[i].hits--;
   }

   if (hit_i == -1)
   {
      if (fcache[min_i].file)
      {
	 if (fclose(fcache[min_i].file) != 0)
	   error(HERE, "Error on fcache fclose() on file [%s].",
		 fcache[min_i].name);
	 purge++;
      }
      fcache[min_i].name = enl_strcpy(fcache[min_i].name, name,
				      &fcache[min_i].name_s);
      fcache[min_i].hits = 0;

      if (strchr(mode, 'w'))
	fcache[min_i].file = fopen(name, "w+b");
      else if (strchr(mode, 'a'))
	fcache[min_i].file = fopen(name, "a+b");
      else if (strchr(mode, 'r'))
	fcache[min_i].file = fopen(name, "r+b");
      else
	error(HERE, "Bad file mode [%s] for file [%s]", mode, name);
 
      return fcache[min_i].file;
   }
   else
   {
      if (strchr(mode, 'w'))
	fcache[hit_i].file = freopen(name, "w+b", fcache[hit_i].file);
      else if (strchr(mode, 'a'))
	fseek(fcache[hit_i].file, 0L, SEEK_END);
      else if (strchr(mode, 'r'))
	fseek(fcache[hit_i].file, 0L, SEEK_SET);
      else
	error(HERE, "Bad file mode [%s] for file [%s]", mode, name);
     
      pure_hits++;

      fcache[hit_i].hits = 0;

      /* Consider fflush(fcache[hit_i].file); */

      return fcache[hit_i].file;
   }
}

void fclose_cache(void)
{
   int i;

   /* closes all files in the cache! */

   for (i = 0; i < FCACHE_MAX; i++)
   {
      if (fcache[i].file)
      {
	 
	 if (fclose(fcache[i].file) != 0)
	 {
	    slog(LOG_ALL,0,"fcache close failed on file %s.", fcache[i].name);
	    return;
	 }
	 else
	 {
	    fcache[i].file = NULL;
	 }
      }
   }

}
