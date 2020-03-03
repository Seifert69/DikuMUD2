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
#include "help.h"

/*  Help-file contained 468 keywords on 12-Sep-94
 *  Potential waste: 8 * (HELP_INCREMENT - 1) bytes
 */

#define HELP_INCREMENT 100

help_index_type::help_index_type(void)
{
   pos = 0;
   keyword = NULL;
}

help_index_type::~help_index_type(void)
{
   if (keyword)
     free(keyword);
}

help_file_type::help_file_type(void)
{
   help_idx = NULL;
   elements = 0;
   size     = 0;
   filename = 0;
}

help_file_type::~help_file_type(void)
{
   if (filename)
     free(filename);
}

static class help_file_type help_file[3];

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

      fgets(line, sizeof(line), help_fl);
      strcat(line, "\n\r");

      send_to_descriptor(line, d);

      *buf = '\0';
      for (;;)
      {
	 fgets(line, sizeof(line), help_fl);
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

void help_file_type::generate_help_idx(char *name)
{
   FILE *fl;
   char buf[256], tmp[256], *scan;
   long pos;

   filename  = str_dup(str_cc(libdir, name));
   touch_file(filename);

   if ((fl = fopen_cache(filename, "r")) == NULL)
   {     
      slog(LOG_OFF, 0,"   Could not open help file [%s]", filename);
      exit(0);
   }

   for (;;)
   {
      pos = ftell(fl);

      fgets(buf, sizeof buf, fl);

      buf[sizeof buf - 1]  = '\0'; /* Just in case... */
      buf[strlen(buf) - 1] = '\0'; /* Cut off trailing newline */

      scan = buf;

      for (;;)
      {
	 /* extract the keywords */
	 scan = one_word(scan, tmp);
	 if (*tmp == '\0')
	   break;

	 if (help_idx == NULL)
	 {
	    size = HELP_INCREMENT;
	    help_idx = new help_index_type[size];
	 }
	 else if (++elements == size)
	 {
	    size += HELP_INCREMENT;
	    help_idx = RECREATE(help_idx, class help_index_type, size);
	 }
      
	 help_idx[elements].pos = pos;
	 help_idx[elements].keyword = str_dup(tmp);
      }

      /* skip the text */
      do
	fgets(buf, sizeof buf, fl);
      while (*buf != '#' && !feof(fl));

      if (feof(fl))
	break;

      if (buf[1] == '~')
	break;
   }

   qsort(help_idx, elements + 1, sizeof(class help_index_type),
	 build_help_cmp);
}


void boot_help(void)
{
   help_file[0].generate_help_idx(HELP_FILE);
   help_file[1].generate_help_idx(HELP_FILE_LOCAL);
   help_file[2].generate_help_idx(HELP_FILE_WIZ);
}
