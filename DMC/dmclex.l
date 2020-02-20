/* *********************************************************************** *
 * File   : dmclex.l                                  Part of Valhalla MUD *
 * Version: 1.20                                                           *
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

/* Fri Feb 25 00:25:58 MET 1994, gnort
 *   Fixed the line-counter bug (lines weren't counted inbetween "'s")
 *
 * Sun Aug  7 15:37:02 MET 1994, gnort
 *   Changed init_lex() to treat flex differently
 */


%{
/* #include <string.h> */
#include <stdio.h>
#include <ctype.h>
#include "dmc.h"
#include "tmp_dmcpar.h"
#include "dmctok.h"
#include "textutil.h"

#ifdef YYLMAX
#undef YYLMAX
#endif
#define YYLMAX 32678  /* For looong strings... */

#ifdef yywrap
#undef yywrap
#endif
/*MS2020 #define yywrap 1*/
#undef free // MS2020

extern char cur_filename[], top_filename[];
extern struct zone_info zone;
extern int istemplate;
extern int verbose;

int first_dil = TRUE;
int linenum = 1;
int startlinenum = 0;

int pos, i;
void setline(const char *str);
void warning(const char *str);
void fatal(const char *str);
void dmc_exit(int n);

char tmpstr[YYLMAX + 1], *a, *b;
%}


%%
\n			{
			  linenum++;
			}
"{"			|
"}"			|
"["			|
"]"			|
";"			|
","			|
"@"			|
"("                     |
")"                     |
"/"                     |
"*"                     |
"+"                     |
"`"                     |
"-"			{
			  return yytext[0];
			}
\"[^\"]*\"		{
                           char deststr[YYLMAX];
			   char buf[80];
                           /* Process string */
                           for (a = yytext + 1, b = tmpstr; *a != '"';a++, b++)
                           {
			      if (a - yytext >= sizeof(tmpstr))
			      {
				 fprintf(stderr,
					 "Fixed string too long, aborting.\n");
				 dmc_exit(1);
			      }

			      if ((*a < ' ') && !isspace(*a))
			      {
				 sprintf(buf, "Illegal character ascii %d.",
					 *a);
				 *b = 'X';
				 warning(buf);
				 continue;
			      }

			      if ((*b = *a) == '\n')
				linenum++;
			   }
			   *b = '\0';

			   str_escape_format(tmpstr, deststr, sizeof(deststr));

			   /* + 6 because of concatenated "\n\r" later on
			      and perhaps 3 leading spaces. */

			   yylval.str = (char *) mmalloc(strlen(deststr) + 6);

			   strcpy(yylval.str, deststr);

			   return STRING;
			}
([1-9][0-9]*)|0		{
			  /* Process number */
			  yylval.num = atoi(yytext);
			  return PNUM;
			}
0[xX][0-9a-fA-F]+	{
			  /* Process hexnumber */
			  sscanf(yytext + 2, "%x", &yylval.num);
			  return PNUM;
			}
dilbegin[^\^]*\^        {
			  FILE *f;
			  char tmp1[L_tmpnam], tmp2[L_tmpnam];
			  char cmd[2 * L_tmpnam + 200], buf[256];


#ifdef MARCEL
			  sprintf(tmp1, "/tmp/dmc.XX");
			  if (tmp1 != mktemp(tmp1))
			  {
			    perror("dmclex mktemp tmp1");
                            dmc_exit(1);
			  }
#else
			  char *mbuf = tmpnam(tmp1);
#endif
			  if (!(f = fopen(tmp1, "wb")))
			  {
			    fprintf(stderr, "No DIL open.");
                            dmc_exit(1);
			  }

			  startlinenum = linenum;
			  /* Process string */
			  for (a = yytext; *a != '^'; a++)
			  {
			    fputc(*a, f);
			    if (*a == '\n')
			      linenum++;
			  }
			  fclose(f);

#ifdef MARCEL
			  sprintf(tmp2, "/tmp/dmc.XX");
			  if (tmp2 != mktemp(tmp2))
			  {
			    perror("mktemp");
                            dmc_exit(1);
			  }
#else
			  char *msbuf = tmpnam(tmp2);
#endif
			  /* During %dil section, tell compiler only to
			  generate templates */

			  sprintf(cmd, "DIL/dil %d %s %s -z %s < %s > %s",
				  startlinenum,
				  istemplate?"-t":"",
				  first_dil ? "-v" : "",
				  zone.z_zone.name,
				  tmp1, tmp2);

			  if (verbose)
			     fprintf(stderr, "verbose: system(%s)\n", cmd);

			  first_dil = FALSE;

			  if (system(cmd) != 0)
			  {
			    remove(tmp1);
			    remove(tmp2);

			    fatal("DIL not compiled properly.");
#ifdef AMIGA
			    /* Because the Amiga has trouble with stderr */
			    sprintf(cmd, "C:Type %s", tmp2);
			    system(cmd);
#endif
                            dmc_exit(1);
			  }
			  if (verbose)
			     fprintf(stderr, "verbose: DIL compiled\n");

			  remove(tmp1);

			  /* Get the created tmp-file */
			  if ((f = fopen(tmp2, "rb")) == NULL)
			  {
			    remove(tmp2);
			    perror(str_cc("dmclex fopen ", tmp2));
                            dmc_exit(1);
			  }

			  *tmp1 = '\0';
			  while (fgets(buf, sizeof buf, f))
			    if (sscanf(buf, "Data in: %s\n", tmp1))
			      break;
			  if (!*tmp1)
			    fatal("Wrong output from DIL.");
			  fclose(f);

			  remove(tmp2);

			  yylval.str = (char *) mmalloc(strlen(tmp1) + 1);
			  strcpy(yylval.str, tmp1);
			  if (verbose)
			     fprintf(stderr, "Done with DIL\n");
			  return DILPRG;
			}

[a-zA-Z_][a-zA-Z0-9_]*	{
			  for (pos = 0; sym[pos].nam; pos++)
			    if (!strcmp(sym[pos].nam, yytext))
			      return sym[pos].val;
			  strcpy(yylval.sym, yytext);
			  return SYMBOL;
			}
\%dil	  		{
			  return DIL;
	       		}
\%end                   {
                          if (verbose)
			     fprintf(stderr, "verbose: end\n");

                          return ENDMARK;
                        }
\%zone			{
			  return ZONE;
			}
\%reset			{
			  return RESET;
			}
\%mobiles		{
			  return MOBILES;
			}
\%rooms			{
			  return ROOMS;
			}
\%objects		{
			  return OBJECTS;
			}
^\#.*\n			{
			  setline(yytext); /* cpp file/line settings */
			}
[\t ]			/* ignore */
.			{
			  return UNKNOWN;
			}
%%
void setline(const char *str)
{
  char *p;

  while (!isdigit(*str) && *str)
    str++;

  if (!isdigit(*str))
    return;

  linenum = atoi(str);

  while (*str != '"' && *str)
    str++;

  if (!*str)
    return;

  str++;
  p = cur_filename;

  while (*str != '"' && *str)
    (*(p++) = *(str++));

  *p = '\0';

  if(!*cur_filename)
    strcpy(cur_filename, top_filename);
}

void init_lex(FILE *file)
{
   static int count = 0;

   extern FILE *yyin;

   yyin = file;

#ifdef FLEX_SCANNER
  /* ONLY RESTART **AFTER** FIRST RUN (MS) */
  if (count++ > 0)
    yyrestart(file);
#endif
}

void destroy_fptr(struct unit_data *u, struct unit_fptr *f)
{
}

void unlink_affect(struct unit_affected_type *u)
{
}

int yywrap()
{
   return 1;
}
