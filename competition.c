/* *********************************************************************** *
 * File   : competition.c                             Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: To implement an easy competition module.                       *
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
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <ctype.h>

#include "structs.h"
#include "utils.h"
#include "handler.h"
#include "utility.h"
#include "db.h"
#include "comm.h"
#include "interpreter.h"
#include "textutil.h"
#include "common.h"
#include "files.h"
#include "str_parse.h"
#include "competition.h"

extern char libdir[];

#define MAX_TOP_TEN  (11)

struct competition_entry
{
   char name[PC_MAX_NAME];
   int secs;
   int xp;
   int points;
};

struct top_ten_type
{
   int max_secs;
   int competitors; /* No of competitiors */
   struct competition_entry entry[MAX_TOP_TEN];
};

static struct competition_data
{
   char *name;
   char *descr;
   int  ongoing;
   int  start;
   int  stop;

   struct top_ten_type top_ten;
   
} *competition = NULL;

static int competition_top = 0;

struct extra_descr_data *competition_find(char *name,
					  struct extra_descr_data *pexd)
{
   class extra_descr_data *exd;

   do
   {
      exd = pexd->find_raw("$competition");

      if (exd == NULL)
	break;

      if (exd->names.Length() >= 4 &&
	  str_ccmp(exd->names.Name(1), name) == 0)
	return exd;

      pexd = exd->next;
   }
   while (pexd);

   return NULL;
}


static void competition_save(int idx)
{
   int n;
   char buf[128];
   FILE *f;

   sprintf(buf, "%scompete/%s", libdir, competition[idx].name);

   f = fopen_cache(buf, "wb");

   assert(f);

   n = fwrite(&competition[idx].top_ten, sizeof(struct top_ten_type), 1, f);
   assert(n == 1);
}


static void competition_load(int idx)
{
   int n;
   char buf[128];
   FILE *f;

   sprintf(buf, "%scompete/%s", libdir, competition[idx].name);

   touch_file(buf);

   f = fopen_cache(buf, "rb");
   assert(f);

   n = fread(&competition[idx].top_ten, sizeof(struct top_ten_type), 1, f);

   if (n != 1)
   {
      int i;

      competition[competition_top].top_ten.max_secs    = 0;
      competition[competition_top].top_ten.competitors = 0;

      for (i=0; i < MAX_TOP_TEN; i++)
      {
	 competition[competition_top].top_ten.entry[i].name[0] = 0;
	 competition[competition_top].top_ten.entry[i].points  = 0;
	 competition[competition_top].top_ten.entry[i].secs  = 0;
	 competition[competition_top].top_ten.entry[i].xp  = 0;
      }
   }
}


int competition_compare(const void *v1, const void *v2)
{
   const struct competition_entry *e1 = (struct competition_entry *) v1;
   const struct competition_entry *e2 = (struct competition_entry *) v2;


   if (e1->points > e2->points)
     return -1;
   else if (e1->points < e2->points)
     return 1;
   else
     return 0;
}


static void competition_recalc(int idx, struct unit_data *pc, int xp, int secs)
{
   int i;
   double points;

   assert(competition[idx].ongoing == TRUE);

   if (secs > competition[idx].top_ten.max_secs)
     competition[idx].top_ten.max_secs = secs;

   if (competition[idx].top_ten.max_secs <= 0)
     return;

   strcpy(competition[idx].top_ten.entry[MAX_TOP_TEN-1].name, UNIT_NAME(pc));
   competition[idx].top_ten.entry[MAX_TOP_TEN-1].xp   = xp;
   competition[idx].top_ten.entry[MAX_TOP_TEN-1].secs = secs;

   for (i=0; i < MAX_TOP_TEN; i++)
   {
      if (!competition[idx].top_ten.entry[i].name[0] ||
	  ((i < MAX_TOP_TEN-1) &&
	   str_ccmp(competition[idx].top_ten.entry[i].name, UNIT_NAME(pc))
	   == 0))
      {
	 competition[idx].top_ten.entry[i].name[0] = 0;
	 competition[idx].top_ten.entry[i].points  = 0;
      }
      else
      {
	 points  = (double) competition[idx].top_ten.entry[i].xp *
	   (double) competition[idx].top_ten.entry[i].secs;
	 if (competition[idx].top_ten.max_secs > 0)
	   points /= (double) competition[idx].top_ten.max_secs;
	 else
	   points = 0;

	 competition[idx].top_ten.entry[i].points = (int) points;
      }
   }

   qsort(&competition[idx].top_ten.entry[0],
	 MAX_TOP_TEN,
	 sizeof(struct competition_entry),
	 competition_compare);

   competition_save(idx);
}


static int competition_points(struct unit_data *pc, int idx)
{
   struct extra_descr_data *exd;
   double xp, secs;
   double points = 0.0;

   if ((exd = competition_find(competition[idx].name, PC_QUEST(pc))))
   {
      xp   = CHAR_EXP(pc)       - atoi(exd->names.Name(2));
      secs = PC_TIME(pc).played - atoi(exd->names.Name(3));
	 
      points  = xp * secs;
      if (competition[idx].top_ten.max_secs > 0)
	points /= competition[idx].top_ten.max_secs;
      else
	points = 0.0;
   }

   return (int) points;
}


void competition_update(struct unit_data *pc)
{
   struct extra_descr_data *exd;
   int i, xp, secs;

   for (i=0; i < competition_top; i++)
   {
      if (competition[i].ongoing)
      {
	 if ((exd = competition_find(competition[i].name, PC_QUEST(pc))))
	 {
	    xp   = CHAR_EXP(pc)       - atoi(exd->names.Name(2));
	    secs = PC_TIME(pc).played - atoi(exd->names.Name(3));
	 
	    competition_recalc(i, pc, xp, secs);
	 }
      }
   }
}

void competition_enroll(struct unit_data *pc)
{
   int i;
   struct extra_descr_data *exd, *pexd;
   const char *names[2] = {"$competition", NULL};

   for (i=0; i < competition_top; i++)
   {
      if (competition[i].ongoing &&
	  is_in(CHAR_LEVEL(pc),competition[i].start,competition[i].stop))
      {
	 if (competition_find(competition[i].name, PC_QUEST(pc)) == NULL)
	 {
	    competition[i].top_ten.competitors++;
	    
	    act(COLOUR_ATTN
		"You are enrolled in $2t." COLOUR_NORMAL,
		A_ALWAYS, pc, competition[i].descr, NULL, TO_CHAR);

	    PC_QUEST(pc) = PC_QUEST(pc)->add(names, "");

	    PC_QUEST(pc)->names.AppendName(competition[i].name);

	    PC_QUEST(pc)->names.AppendName(itoa(CHAR_EXP(pc)));

	    PC_QUEST(pc)->names.AppendName(itoa(PC_TIME(pc).played));
	 }
      }
   }

   pexd = PC_QUEST(pc);

   for (;;)
   {
      exd = pexd->find_raw("$competition");

      if (!exd)
	break;

      for (i=0; i < competition_top; i++)
      {
	 if (exd->names.IsName(competition[i].name))
	   break;
      }

      pexd = exd->next;

      if (i >= competition_top) /* Obsolete Competition? */
	PC_QUEST(pc) = PC_QUEST(pc)->remove(exd);
   }
}


void competition_boot(void)
{
   char Buf[MAX_STRING_LENGTH];
   char *c;
   char *name, *descr;
   int i, len;
   int *numlist;
   int start_level, stop_level;

   extern char libdir[];

   slog(LOG_OFF, 0, "Booting competition system.");

   touch_file(str_cc(libdir, COMPETITION_FILE));

   config_file_to_string(str_cc(libdir, COMPETITION_FILE), Buf, sizeof(Buf));

   c = Buf;

   for (;; competition_top++)
   {
      name = parse_match_name(&c, "Name");
      if (!name)
	break;

      descr = parse_match_name(&c, "Descr");
      if (!descr)
      {
	 slog(LOG_ALL, 0, "Competition '%s' missing description.", name);
	 break;
      }

      numlist = parse_match_numlist(&c, "Range", &len);

      if (!numlist)
      {
	 slog(LOG_ALL, 0, "Competition '%s' missing level range.", name);
	 break;
      }

      if (len != 2)
      {
	 slog(LOG_ALL, 0, "Competition '%s': %d levels found, 2 expected.",
	      name, len);
	 free(numlist);
	 break;
      }

      start_level = numlist[0];
      stop_level  = numlist[1];
      free(numlist);

      if (!parse_match_num(&c, "Ongoing", &i))
      {
	 slog(LOG_ALL, 0, "Competition '%s' missing ongoing status.", name);
	 break;
      }


      if (competition_top == 0)
	CREATE(competition, struct competition_data, 1);
      else
	RECREATE(competition, struct competition_data, competition_top + 2);

      competition[competition_top].name        = name;
      competition[competition_top].descr       = descr;
      competition[competition_top].ongoing     = i;
      competition[competition_top].start       = start_level;
      competition[competition_top].stop        = stop_level;

      competition_load(competition_top);
   }
}


static void show_competition(struct unit_data *ch, const int i)
{
   int j;
   char buf[256];
   int found = FALSE;

   sprintf(buf, COLOUR_MENU "%s%s" COLOUR_NORMAL
	   " (Level %d to %d with %d competitors)\n\r",
	   competition[i].descr,
	   competition[i].ongoing ? "" : " [ENDED]",
	   competition[i].start,
	   competition[i].stop,
	   competition[i].top_ten.competitors);
   
   send_to_char(buf, ch);

   found = FALSE;

   for (j=0; j < MAX_TOP_TEN-1; j++)
   {
      if (competition[i].top_ten.entry[j].name[0])
      {
	 found = TRUE;
	 sprintf(buf, "   %2d. %-15s  (%8d points)\n\r",
		 j + 1, competition[i].top_ten.entry[j].name,
		 competition[i].top_ten.entry[j].points);
	 send_to_char(buf, ch);
      }
   }
   
   if (!found)
     send_to_char("   No competitors yet.\n\r", ch);


   sprintf(buf, "You have scored %d points.\n\r",
	   competition_points(ch, i));
   send_to_char(buf, ch);
}


static int competition_read_board(struct unit_data *ch, const char *arg)
{
   char number[MAX_INPUT_LENGTH];
   int msg;

   if (!IS_PC(ch))
   {
      send_to_char("Monsters are not a part of the competitions.\n\r", ch);
      return SFR_BLOCK;
   }

   one_argument(arg, number);

   if (!str_is_number(number))
     return SFR_SHARE;

   msg = atoi(number);

   if (competition_top < 1)
   {
      send_to_char("But the board is empty!\n\r", ch);
      return SFR_BLOCK;
   }

   if (msg < 1 || msg > competition_top)
   {
      send_to_char("That competition exists only within the boundaries of your"
		   " mind...\n\r", ch);
      return SFR_BLOCK;
   }

   show_competition(ch, msg - 1);

   return SFR_BLOCK;
}


int competition_board(struct spec_arg *sarg)
{
   int i;
   char buf[256];
   char *c = (char *) sarg->arg;

   if ((sarg->cmd->no != CMD_LOOK) && (sarg->cmd->no != CMD_READ))
     return SFR_SHARE;

   if (sarg->cmd->no == CMD_READ)
     return competition_read_board(sarg->activator, sarg->arg);

   if (!IS_PC(sarg->activator))
     return SFR_SHARE;

   if (find_unit(sarg->activator, &c, 0, FIND_UNIT_SURRO) != sarg->owner)
     return SFR_SHARE;

   act("$1n looks at $2n.",
       A_ALWAYS, sarg->activator, sarg->owner, 0, TO_ROOM);

   send_to_char("This is a competition board. Usage: READ <competition #>\n\r",
		sarg->activator);

   if (competition_top < 1)
   {
      send_to_char("No competitions currently active.\n\r", sarg->activator);
      return SFR_BLOCK;
   }


   for (i = 0; i < competition_top; i++)
   {
      sprintf(buf, "%2d : %-*s %s\n\r", i + 1,
	      65 - 7,
	      competition[i].descr,
	      competition[i].ongoing ? "" : "[ENDED]");
      send_to_char(buf, sarg->activator);
   }

   return SFR_BLOCK;
}



