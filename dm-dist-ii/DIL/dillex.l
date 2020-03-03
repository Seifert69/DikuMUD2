/* *********************************************************************** *
 * File   : dillex.l                                  Part of Valhalla MUD *
 * Version: 1.01                                                           *
 * Author : bombman@diku.dk & quinn@diku.dk                                *
 *                                                                         *
 * Purpose: DIL compiler. See dil.doc for specs.                           *
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

%option noyywrap

%{

#include <ctype.h>
#include <string.h>
#include "essential.h"
#include "dilpar.h"
#include "dil.h"
#include "tmp_dilpar.h"
#include "diltok.h"
#include "textutil.h"
#include "structs.h"

#ifdef YYLMAX
#undef YYLMAX
#endif
#define YYLMAX 4096

#ifdef SUSPEKT
 "="			|
 "#"                     |
 "$"                     |
 "!"			|
 "<"                     |
 ">"                     |
#endif

// #define yywrap(a...)
#undef free // MS2020

extern char cur_filename[];

int linenum = 1;

int pos;
void setline(const char *str);
void warning(const char *str);
void fatal(const char *str);
void syntax(const char *str);

char tmpstr[YYLMAX * 2 + 1], *a, *b;
%}

%%
\n			linenum++;
":="                    return DILTO_ASS;
"=="                    return DILTO_EQ;
"!="                    return DILTO_NEQ;
"#="                    return DILTO_PEQ;
"$="                    return DILTO_SEQ;
"<="                    return DILTO_LEQ;
">="                    return DILTO_GEQ;
"<"                     return DILTO_LE;
">"                     return DILTO_GT;
"&"                     return DILSE_AND;
"|"                     return DILSE_OR;
"{"			|
"}"			|
"["			|
"]"			|
":"			|
";"			|
","			|
"("                     |
")"                     |
"/"                     |
"*"                     |
"+"                     |
"%"                     |
"`"                     |
"."                     |
"-"			return yytext[0];
\"[^\"]*\"		{
                          char deststr[YYLMAX];
                          char buf[80];

			  /* Process string */
			  for (a = yytext + 1, b = tmpstr; *a != '"';a++, b++)
			  {
			     if ((*a < ' ') && !isspace(*a))
			     {
				sprintf(buf, "Illegal character ascii %d.",
					*a);
				warning(buf);
				*b = 'X';
				continue;
			     }

			     if ((*b = *a) == '\n')
			     {
				linenum++;
				*(++b) = '\r';
			     }
			  }
			  *b = '\0';

			  str_escape_format(tmpstr, deststr, sizeof(deststr),
                                            FALSE);

			  yylval.str = (char *) malloc(strlen(deststr) + 3);

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
[a-zA-Z_][a-zA-Z@0-9_]*	{
			  for (pos = 0; sym[pos].nam; pos++)
			    if (!strcmp(sym[pos].nam, yytext))
			      return sym[pos].val;
			  strcpy(yylval.sym, yytext);
			  return SYMBOL;
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
}


void destroy_fptr(struct unit_data *u, struct unit_fptr *f)
{
}

void unlink_affect(struct unit_data *u, struct unit_affected_type *af)
{
   u->affected = 0;
}
