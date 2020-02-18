/* *********************************************************************** *
 * File   : calclex.l                                 Part of Valhalla MUD *
 * Version: 1.20                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Calculator                                                     *
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
 *   Fixed the line-counter bug (lines weren't counted inbetween "'s'")
 *
 * Sun Aug  7 15:37:02 MET 1994, gnort
 *   Changed init_lex() to treat flex differently
 */
%option noyywrap

%{

/* #include <string.h> */
#include <stdio.h>
#include <ctype.h>
#include "tmp_calcpar.h"
#include "calctok.h"

#ifdef YYLMAX
#undef YYLMAX
#endif
#define YYLMAX 512

#ifdef yywrap
#undef yywrap
#endif
#define yywrap() 1

%}

%%
"\n"                    |
";"                     {
			   return ENDMARK;
			}
","                     |
"{"                     |
"}"                     |
"<"                     |
">"                     |
"("                     |
")"                     |
"/"                     |
"*"                     |
"+"                     |
"-"			{
			   return yytext[0];
			}
\"[^\"]*\"		{
                           char tmpstr[YYLMAX];
                           char deststr[YYLMAX];
			   char buf[80];
                           char *a, *b;

                           /* Process string */
                           for (a = yytext + 1, b = tmpstr; *a != '"';a++, b++)
                           {
			      if (a - yytext >= sizeof(tmpstr))
			      {
				 fprintf(stderr,
					 "Fixed string too long, aborting.\n");
				 exit(1);
                               }
			   }

			   if (!*deststr)
			     strcpy(deststr, "");

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
%%

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

int main(void)
{
   int c;

   void init_lex(FILE *file);
   int yyparse(void);

   /* Examine filename */

   while (!feof(stdin))
   {
      c = fgetc(stdin);
      if (isspace(c))
	continue;
      ungetc(c, stdin);
      break;
   }

   if (!(isdigit(c) || c == '(' || c == '{'))
   {
      while (!feof(stdin))
      {
	 c = fgetc(stdin);
         if ((c != EOF) && (c != '\"'))
   	   fputc(c, stdout);
      }
      exit(0);
   }
	
   init_lex(stdin);
   yyparse();

   if (fclose(stdin))
   {
      perror("fclose");
      exit(1);
   }

   return 0;
}
