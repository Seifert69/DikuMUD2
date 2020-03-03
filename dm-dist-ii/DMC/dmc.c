/* *********************************************************************** *
 * File   : dmc.c                                     Part of Valhalla MUD *
 * Version: 1.10                                                           *
 * Author : quinn@diku.dk                                                  *
 *                                                                         *
 * Purpose: Database compiler. See dmc.doc for specs.                      *
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
/* Mon 27 Jun 1994 Gnort: Changed the illustrious pipe to two temp-files.
 * Mon 30 Jan 1995 Gnort: Added support for include dirs.
 * Fri 10 Mar 1995 Gnort: Boots money now, for easy money support...
 * HHS : added DIL template writing
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "utils.h"
#include "dmc.h"
#include "dil.h"
#include "textutil.h"
#include "utility.h"
#include "db_file.h"
#include "common.h"

#define MEMBLOCK 65536
#define BUFS 30

int sunlight = 0;
const sbit8 time_light[24] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

struct zone_info zone;
char cur_filename[256], top_filename[256];

void boot_money(void);
void fix(const char *file);
void zone_reset(char *default_name);
void init_unit(struct unit_data *u);
void dump_zone(char *prefix);
long stat_mtime(const char *name);

int	make = 0,		/* cmp mod-times of files before compiling */
	nooutput = 0,		/* suppress output */
	verbose = 0,		/* be talkative */
	fatal_warnings = 0;	/* allow warnings */

extern int errcon;
extern char *error_zone_name;

char **ident_names = NULL;  /* Used to check unique ident */

struct mem
{
  char *bufs[BUFS + 1];
  int buf;
  long free;
} mm;

const char *inc_dirs[16]; /* Must be enough :-) */
int inc_count = 0;

void ShowUsage(char *name)
{
   fprintf(stderr, "Usage: %s [-msvlh] [-Idir ..] zonefile ...\n", name);
   fprintf(stderr, "   -m Compile only changed zones.\n");
   fprintf(stderr, "   -s Suppress output of data files.\n");
   fprintf(stderr, "   -v Verbose mode.\n");
   fprintf(stderr, "   -l Ordinary warnings become fatal warnings.\n");
   fprintf(stderr, "   -h This help.\n");
   fprintf(stderr, "   -I Search specified dir for include files.\n");
   fprintf(stderr, "Copyright 1994 - 1996 (C) by Valhalla.\n");
}


int main(int argc, char **argv)
{
   int pos;
   
   mem_init();
   boot_money();		/* I guess it was inevitable... /gnort */

   if (argc == 1)
   {
      ShowUsage(argv[0]);
      exit(1);
   }

   inc_dirs[inc_count++] = CPPI;

   fprintf(stderr, "DMC %s Copyright (C) 1995 by Valhalla [%s]\n",
	   VERSION, __DATE__);

   for (pos = 1; pos < argc; pos++)
     if (*argv[pos] == '-')
       switch (*(argv[pos] + 1))
       {
	 case 'I': inc_dirs[inc_count++] = argv[pos] + 2; break; 
	 case 'm': make = 1; break;
	 case 's': nooutput = 1; break;
	 case 'v': verbose = 1; break;
	 case 'l': fatal_warnings = 1; break;
	 case 'h':
	 case '?':
          ShowUsage(argv[0]);
	  exit(0);

	 default:
	  fprintf(stderr, "Unknown option '%c'.\n", *(argv[pos] + 1));
          ShowUsage(argv[0]);
	  exit(0);
       }
     else
     {
#ifndef DOS
	alarm(15*60);		/* If not done in 5 minutes, abort */
#endif
	fix(argv[pos]);
     }

   fprintf(stderr, "DMC Done.\n");

   return 0;
}

char tmpfile1[L_tmpnam] = "";
char tmpfile2[L_tmpnam] = "";

void dmc_exit(int n)
{
   if (*tmpfile1)
     remove(tmpfile1);

   if (*tmpfile2)
     remove(tmpfile2);

   exit(n);
}

void fix(const char *file)
{
   char *p, tmp[500], tmp2[256], filename_prefix[256], filename[256];
   char incstr[512], *inc;
   FILE *cpp;
   int i, result, compressed = 0;

   void init_lex(FILE *file);
   int yyparse(void);

   /* Examine filename */

   strcpy(filename_prefix, file);
   /* Check for .-suffix */
   if ((p = strrchr(filename_prefix, '.'))
       && (!strcmp(p, ".Z") || !strcmp(p, ".gz")))
   {
      compressed = 1;
      *p = '\0';
   }

   /* find filename w/o .zon-suffix */
   if (!(p = strrchr(filename_prefix, '.')) || strcmp(p + 1, INPUT_SUFFIX))
   {
      fprintf(stderr, "%s: Filename format error.\n", file);
      exit(1);
   }
   *p = '\0';
   if (make)
   {
      sprintf(tmp, "%s.%s", filename_prefix, OUTPUT_WSUFFIX);
      sprintf(tmp2, "%s.%s", filename_prefix, OUTPUT_RSUFFIX);
      result = stat_mtime(file);
      if (result <= stat_mtime(tmp) && result <= stat_mtime(tmp2))
	return;
   }
   /* find filename component of path w/o .zon-suffix */
   if (!(p = strrchr(filename_prefix, '/')))
     p = filename_prefix;
   else
     ++p;
   strcpy(filename, p);
   strcpy(cur_filename, file);
   strcpy(top_filename, file);

   /* read & write */
   fprintf(stderr, "Compiling '%s'\n", cur_filename);
   zone_reset(filename);

#ifdef MARCEL
   sprintf(tmpfile1, "/tmp/dmc.XX");
   if (tmpfile1 != mktemp(tmpfile1))
   {
      perror("mktemp");
      exit(1);
   }
#else
   char *ms2020 = tmpnam(tmpfile1);
#endif

   sprintf(tmp, "%s %s > %s", compressed ? ZCAT : CAT, file, tmpfile1);
   if (verbose)
     fprintf(stderr, "verbose: system(%s)\n", tmp);

   i = system(tmp);
   if (i == 127 || i == -1)
   {
      perror("system");
      exit(1);
   }
   else if (i != 0)
   {
      perror("system command");
      exit(1);     
   }

#ifdef MARCEL
   sprintf(tmpfile2, "/tmp/dmc.XX");
   if (tmpfile2 != mktemp(tmpfile2))
   {
      perror("mktemp");
      exit(1);
   }
#else
   char *ms20202 = tmpnam(tmpfile2);
#endif

   for (inc = incstr, i = 0; i < inc_count; i++)
   {
      sprintf(inc, "-I%s ", inc_dirs[i]);
      TAIL(inc);
   }

   // MS2020 WAS: sprintf(tmp, "%s %s< %s > %s", CPP, incstr, tmpfile1, tmpfile2);
   sprintf(tmp, "./%s %s %s > %s", CPP, tmpfile1, incstr, tmpfile2);
   if (verbose)
     fprintf(stderr, "verbose: system(%s)\n", tmp);
   i = system(tmp);
   if (i == 127 || i == -1)
   {
      remove(tmpfile1);
      remove(tmpfile2);
      perror("system");
      exit(1);
   }
   else if (i != 0)
   {
      remove(tmpfile1);
      remove(tmpfile2);
      perror("system command");
      exit(1);     
   }

   remove(tmpfile1);

   if ((cpp = fopen(tmpfile2, "r")) == NULL)
   {
      remove(tmpfile2);
      perror("dmc fopen tmpfile2 ");
      exit(1);
   }

   if (verbose)
     fprintf(stderr, "verbose: init_lex\n");

   init_lex(cpp);

   if (verbose)
     fprintf(stderr, "verbose: yyparse\n");

   result = yyparse();

   if (verbose)
     fprintf(stderr, "verbose: yyparse() done\n");

   if (fclose(cpp))
   {
      remove(tmpfile2);
      perror("fclose ");
      exit(1);
   }
   remove(tmpfile2);

   if (result)
   {
      fprintf(stderr, "Grave errors in file '%s'.\n", cur_filename);
      exit(1);
   }
   else if (errcon)
   {
      if (fatal_warnings)
	fprintf(stderr, "Warnings have been treated as fatal errors.\n");

      fprintf(stderr, "Compilation aborted.\n");
      exit(1);
   }
   else
     dump_zone(filename_prefix);
}


void zone_reset(char *default_name)
{
   static const char *dummy = "";
   static const char *zonehelp = "No help has been written for this zone yet.";

   mem_reset();
   zone.z_rooms = 0;
   zone.z_mobiles = 0;
   zone.z_objects = 0;
   zone.z_table = 0;
   zone.z_zone.name = default_name;
   zone.z_zone.lifespan = 60;
   zone.z_zone.reset_mode = 0;
   zone.z_zone.creators = 0;
   zone.z_zone.title = 0;
   zone.z_zone.notes = dummy;
   zone.z_zone.help = zonehelp;
   zone.z_zone.weather = 1000;
   zone.z_tmpl = NULL;
}


void mem_init()
{
   extern char **tmplnames;
   int i;

   ident_names = NULL;
   tmplnames=create_namelist();
   CREATE(mm.bufs[0], char, MEMBLOCK);
   mm.buf = 0;
   for (i = 1; i <= BUFS; i++)
     mm.bufs[i] = 0;
   mm.free = 0;
}

void mem_reset()
{
   if (ident_names)
     free_namelist(ident_names);

   ident_names = create_namelist();

   mm.buf = 0;
   mm.free = 0;
   mm.buf = 0;
}


void *mmalloc(int size)
{
   long ret;

   if (size > MEMBLOCK)
   {
      fprintf(stderr, "trying to allocate too-large chunk.\n");
      exit(1);
   }
   ret = mm.free;

   mm.free += size + (sizeof(int) - (size%sizeof(int)));
   if (mm.free > (MEMBLOCK - 1))
   {
      if (++mm.buf >= BUFS)
      {
	 fprintf(stderr, "Out of memory.\n");
	 exit(1);
      }
      if (!mm.bufs[mm.buf])
	CREATE(mm.bufs[mm.buf], char, MEMBLOCK);
      mm.free = size + (sizeof(int) - (size % sizeof(int)));
      ret = 0;
   }
   memset(mm.bufs[mm.buf] + ret, '\0', size);
   return mm.bufs[mm.buf] + ret;
}


struct unit_data *mcreate_unit(int type)
{
   struct unit_data *rslt;

   rslt = new (class unit_data)(type);

   init_unit(rslt);
   return rslt;
}


struct unit_fptr *mcreate_func(void)
{
   return (struct unit_fptr *) mmalloc(sizeof(struct unit_fptr));
}


struct room_direction_data *mcreate_exit(void)
{
   struct room_direction_data *rslt;

   rslt = new (struct room_direction_data);

   rslt->open_name = 0;
   rslt->exit_info = 0;
   rslt->key = 0;
   rslt->to_room = 0;
   return rslt;
}


struct unit_affected_type *mcreate_affect(void)
{
   struct unit_affected_type *rs;

   MCREATE(rs, struct unit_affected_type, 1);
   rs->next = 0;
   rs->id = 0;
   rs->duration = 0;
   rs->data[0] = rs->data[1] = rs->data[2] = 0;
   rs->firstf_i = rs->tickf_i = rs->lastf_i = rs->applyf_i = 0;
   return rs;
}

void write_resetcom(FILE *fl, struct reset_command *c)
{
   char *t;
   static char nul[] = { 0, 0 };

   fwrite(&c->cmd_no, sizeof(c->cmd_no), 1, fl);
   if (c->ref1)
   {
      t = c->ref1;
      fwrite(t, 1, strlen(t) + 1, fl);
      t += strlen(t) + 1;
      fwrite(t, 1, strlen(t) + 1, fl);
   }
   else
     fwrite(nul, 1, 2, fl);
   if (c->ref2)
   {
      t = c->ref2;
      fwrite(t, 1, strlen(t) + 1, fl);
      t += strlen(t) + 1;
      fwrite(t, 1, strlen(t) + 1, fl);
   }
   else
     fwrite(nul, 1, 2, fl);
   fwrite(&c->num1, sizeof(c->num1), 1, fl);
   fwrite(&c->num2, sizeof(c->num2), 1, fl);
   fwrite(&c->num3, sizeof(c->num3), 1, fl);
   fwrite(&c->cmpl, sizeof(c->cmpl), 1, fl);
   fwrite(&c->direction, sizeof(c->direction), 1, fl);
}


void check_unique_ident(struct unit_data *u)
{
   if (is_name(UNIT_IDENT(u), (const char **) ident_names))
   {
      fprintf(stderr,"Error: Duplicate symbolic reference '%s'\n",
	      UNIT_IDENT(u));
      exit(1);
   }

   ident_names = add_name(UNIT_IDENT(u), ident_names);
}


/*
   #define write_unit(x,y,z) fprintf(stderr, "Writing: %s\n", z)
   #define write_diltemplate(x,y) fprintf(stderr, "Writing tmpl: %s\n", y->prgname)
   */
void dump_zone(char *prefix)
{
   FILE *fl;
   char filename[256];
   char **creators;
   struct unit_data *u;
   struct reset_command *c;
   int no_rooms = 0;
   struct diltemplate *tmpl;
   ubit32 dummy;  
   void dmc_error(int fatal, const char *fmt, ...);

   /* Quinn, I do this to get all the sematic errors and info */
   /* appear when nooutput = TRUE - it didn't before!         */

   error_zone_name = prefix;

   /* Purge the error file */
   sprintf(filename, "%s.err", error_zone_name);
   fl = fopen(filename, "w");
   assert(fl);
   fclose(fl);

   if (!is_in(zone.z_zone.weather, 960, 1040))
   { 
      dmc_error(TRUE, "Error: Zone weather pressure of %d is outside the "
		"valid range of [960..1040]. (Latter bad, former best).",
		zone.z_zone.weather);
   }

   for (u = zone.z_rooms; u; u = u->next)
   {
      if (IS_ROOM(u))
	no_rooms++;
      check_unique_ident(u);
      process_unit(u);
   }

   if (no_rooms > 150)
   {
      dmc_error(FALSE, "Warning: %3d rooms in zone [100 maximum suggested "
		"due to the O(n^2)\n         nature of the shortest path "
		"algorithm].\n", no_rooms);
   }

   for (u = zone.z_objects; u; u = u->next)
   {
      check_unique_ident(u);
      process_unit(u);
   }

   for (u = zone.z_mobiles; u; u = u->next)
   {
      check_unique_ident(u);
      process_unit(u);
   }

   if (errcon)
   {
      dmc_error(FALSE, "Fatal errors in zone.");
      exit(1);
   }

   if (nooutput)
     return;
   sprintf(filename, "%s.%s", prefix, OUTPUT_WSUFFIX);
   if (!(fl = fopen(filename, "w")))
   {
      perror(filename);
      exit(1);
   }

#ifndef DEMO_VERSION
   fwrite(zone.z_zone.name, sizeof(char), strlen(zone.z_zone.name)+1, fl);
   fwrite(&zone.z_zone.weather, sizeof(int), 1, fl);

   /* More data inserted here */
   fwrite(zone.z_zone.notes, sizeof(char), strlen(zone.z_zone.notes)+1, fl);
   fwrite(zone.z_zone.help, sizeof(char), strlen(zone.z_zone.help)+1, fl);
   if (zone.z_zone.creators)
   {
      for (creators=zone.z_zone.creators; *creators; creators++)
	fwrite(*creators, sizeof(char), strlen(*creators)+1,fl);
   }
   fwrite("", sizeof(char), 1, fl);

   if (zone.z_zone.title)
     fwrite(zone.z_zone.title, sizeof(char), strlen(zone.z_zone.title)+1, fl);
   else
     fwrite("", sizeof(char), 1, fl);

   /* write DIL templates */
   for (tmpl = zone.z_tmpl; tmpl; tmpl = tmpl->next)
     write_diltemplate(fl, tmpl);

   /* end of DIL templates marker */    
   dummy=0;
   if (fwrite(&dummy, sizeof(dummy), 1, fl) != 1)
     error(HERE, "Failed to fwrite() end of DIL templates");

   for (u = zone.z_rooms; u; u = u->next)
     write_unit(fl, u, UNIT_IDENT(u));

   for (u = zone.z_rooms; u; u = u->next)
     delete u;

   for (u = zone.z_objects; u; u = u->next)
     write_unit(fl, u, UNIT_IDENT(u));

   for (u = zone.z_objects; u; u = u->next)
     delete u;

   for (u = zone.z_mobiles; u; u = u->next)
     write_unit(fl, u, UNIT_IDENT(u));

   for (u = zone.z_mobiles; u; u = u->next)
     delete u;

   fwrite("DMC", sizeof(char), 3, fl);
   fclose(fl);

   sprintf(filename, "%s.%s", prefix, OUTPUT_RSUFFIX);
   if (!(fl = fopen(filename, "w"))) {
      perror(filename);
      exit(1);
   }
   fwrite(&zone.z_zone.lifespan, sizeof(unsigned short), 1, fl);
   fwrite(&zone.z_zone.reset_mode, sizeof(unsigned char), 1, fl);

   for (c = zone.z_table; c; c = c->next)
     write_resetcom(fl, c);

   fwrite("DMC", sizeof(char), 3, fl);
   fclose(fl);
#endif
}

long stat_mtime(const char *name)
{
   struct stat buf;

   if (stat(name, &buf) < 0)
     return 0;
   return buf.st_mtime;
}

void szonelog(struct zone_type *zone, const char *fmt,...)
{
   /* Not pretty, but it's this or linking with handler.c ;) */
   fprintf(stderr, "%s\n", fmt);
}
