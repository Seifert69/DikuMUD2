/* *********************************************************************** *
 * File   : help.c                                    Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : gnort@daimi.aau.dk and ??                                      *
 *                                                                         *
 * Purpose: Implementing the help command and its assisting procedures     *
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
#include <stdlib.h>
#include <ctype.h>

#include "structs.h"
#include "utils.h"
#include "db.h"
#include "textutil.h"
#include "utility.h"
#include "interpreter.h"
#include "comm.h"
#include "files.h"
#include "handler.h"
#include "textutil.h"

/*  Help-file contained 468 keywords on 12-Sep-94
 *  Potential waste: 8 * (HELP_INCREMENT - 1) bytes
 */

#define HELP_INCREMENT 100

struct help_file_type
{
   struct help_index_type *help_idx; /* the help keyword list */
   int elements, size;         /* info about the list   */
   char *filename;
};

static struct help_file_type help_file[3];

extern char libdir[];                           /* from dikumud.c        */


int search_help_cmp(const void *keyval, const void *datum)
{
  if (is_abbrev((char *)keyval, ((struct help_index_type *)datum)->keyword))
    return 0;
  else
    return str_ccmp((char *)keyval,((struct help_index_type *)datum)->keyword);
}


/* Returns TRUE if help was presented */
static int help(struct help_file_type *hlp,
		struct descriptor_data *d,
		char *arg)
{
   char buf[MAX_STRING_LENGTH], line[256];
   char buf2[MAX_STRING_LENGTH];
   struct help_index_type *tmp;
   FILE *help_fl;

   if ((hlp->help_idx == NULL) || (hlp->elements < 1))
     return FALSE;
  
   if ((tmp = (struct help_index_type *)
	bsearch(arg, hlp->help_idx, hlp->elements + 1,
		sizeof(struct help_index_type),
		search_help_cmp)))
   {
      int i = (tmp - hlp->help_idx);

      /*  Have to unroll backwards to make sure we find FIRST
       *  occurence of argument
       */
      while (0 < i && is_abbrev(arg, hlp->help_idx[i - 1].keyword))
	--i;

      help_fl = fopen_cache(hlp->filename, "r");
      assert(help_fl);

      fseek(help_fl, hlp->help_idx[i].pos, 0);

      char *ms2020 = fgets(line, sizeof(line), help_fl);
      strcat(line, "\n\r");

      send_to_descriptor(line, d);

      *buf = '\0';
      for (;;)
      {
	 ms2020 = fgets(line, sizeof(line), help_fl);
	 if (*line == '#')
	   break;
	 strcat(buf, line);
      }

      str_escape_format(buf, buf2, sizeof(buf2));
      page_string(d, buf2);
      return TRUE;
   }

   return FALSE;
}

/* Returns TRUE if help was found and displayed */

int help_base(struct descriptor_data *d, char *arg)
{
   ubit8 bHelp = FALSE;

   arg = skip_spaces(arg);
   str_lower(arg);
  
   if ((CHAR_LEVEL(d->character) >= IMMORTAL_LEVEL) &&
       help(&help_file[2], d, arg))
     bHelp = TRUE;

   if (!help(&help_file[0], d, arg) && !help(&help_file[1], d, arg))
     return bHelp;

   return TRUE;
}


void do_help(struct unit_data *ch, char *arg,
	     const struct command_info *cmd)
{
   if (!IS_PC(ch) || !CHAR_DESCRIPTOR(ch))
     return;

   if (str_is_empty(arg))
   {
      struct zone_type *zone = unit_zone(ch);

      page_string(CHAR_DESCRIPTOR(ch), zone->help);
      return;
   }

   if (!help_base(CHAR_DESCRIPTOR(ch), arg))
     act("There is no help available on the subject '$2t'.",
	 A_ALWAYS, ch, arg, 0, TO_CHAR);
}


/* BOOT stuff below */

/*  one_word is like one_argument, except that words in quotes "" are
 *  regarded as ONE word
 */
char *one_word(char *arg, char *first_arg)
{
   do
   {
      int look_at = 0;
    
      arg = skip_spaces(arg);

      if (*arg == '\"')		/* is it a quote " */
      {
	 for (arg++; arg[look_at] >= ' ' && arg[look_at] != '\"'; ++look_at)
	   first_arg[look_at] = tolower(arg[look_at]);

	 if (arg[look_at] == '\"') /* " (damn dumb emacs syntax colouring) */
	   arg++;
      }
      else
	for (look_at = 0; ' ' < arg[look_at]; look_at++)
	  first_arg[look_at] = tolower(arg[look_at]);

      first_arg[look_at] = '\0';
      arg += look_at;
   }
   while (fill_word(first_arg));

   return arg;
}


int build_help_cmp(const void *keyval, const void *datum)
{
   return str_ccmp(((struct help_index_type *) keyval)->keyword, 
		   ((struct help_index_type *) datum)->keyword);
}

static void generate_help_idx(struct help_file_type *hlp, const char *name)
{
   FILE *fl;
   char buf[256], tmp[256], *scan;
   long pos;

   hlp->help_idx  = NULL;
   hlp->elements = 0;
   hlp->filename  = str_dup(str_cc(libdir, name));

   touch_file(hlp->filename);

   if ((fl = fopen_cache(hlp->filename, "r")) == NULL)
   {     
      slog(LOG_OFF, 0,"   Could not open help file [%s]", hlp->filename);
      exit(0);
   }

   for (;;)
   {
      pos = ftell(fl);

      char *ms2020 = fgets(buf, sizeof buf, fl);

      buf[sizeof buf - 1]  = '\0'; /* Just in case... */
      buf[strlen(buf) - 1] = '\0'; /* Cut off trailing newline */

      scan = buf;

      for (;;)
      {
	 /* extract the keywords */
	 scan = one_word(scan, tmp);
	 if (*tmp == '\0')
	   break;

	 if (hlp->help_idx == NULL)
	 {
	    hlp->size = HELP_INCREMENT;
	    CREATE(hlp->help_idx, struct help_index_type, hlp->size);
	 }
	 else if (++hlp->elements == hlp->size)
	 {
	    hlp->size += HELP_INCREMENT;
	    RECREATE(hlp->help_idx, struct help_index_type, hlp->size);
	 }
      
	 hlp->help_idx[hlp->elements].pos = pos;
	 hlp->help_idx[hlp->elements].keyword = str_dup(tmp);
      }

      /* skip the text */
      do
	ms2020 = fgets(buf, sizeof buf, fl);
      while (*buf != '#' && !feof(fl));

      if (feof(fl))
	break;

      if (buf[1] == '~')
	break;
   }

   qsort(hlp->help_idx, hlp->elements + 1, sizeof(struct help_index_type),
	 build_help_cmp);
}


void boot_help(void)
{
   generate_help_idx(&help_file[0], HELP_FILE);
   generate_help_idx(&help_file[1], HELP_FILE_LOCAL);
   generate_help_idx(&help_file[2], HELP_FILE_WIZ);
}
