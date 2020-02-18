/* *********************************************************************** *
 * File   : calcpar.y                                 Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : quinn@diku.dk                                                  *
 *                                                                         *
 * Purpose: Database compiler. See dmc.doc for specs.                      *
 *          Source for yacc/bison.                                         *
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
 *   Subtle(!) change:
 *     char yytext[];  changed to
 *     char *yytext;   due to sudden change from lex to flex....
 */

/* 16-Aug-94, gnort: Introduced object-money */

/*
 * Definitions and type associations.
 */

%{
#include <stdio.h>
#include <ctype.h>

extern char *yytext; /*  MUST be changed to
		      *  `extern char yytext[]'
		      *  if you use lex instead of flex.
		      */
int yylex(void);

#define yyerror(dum) syntax(dum)

void syntax(const char *str);
void fatal(const char *str);

%}


%union
{
  char *str;
  char sym[40 + 1];
  char **str_list;
  int num;
}

/* general */
%token UNKNOWN
%token ENDMARK
%token <num> PNUM
%token <str> STRING
%type <num> number expr term factor

%left '+' '-' '*' '/' SPLUS
%right UMINUS UPLUS

 /* MS 2020 %expect 2 I can't seem to get rid of the two shift/reduce in `expr'
	   *  And they're not really bothering anyone, so let's ignore them!
	   *  /gnort 08-Aug-94
	   */
%%

/*
 * specifications ************************************
 */

file		: calcsection ENDMARK
                        {
			   printf("\n");
                        }
                | ENDMARK
                        {
			   printf("\n");
                        }

		;

calcsection     : flags
                | STRING 
			{
			   printf("%s", $1);
			}
		;

flags		: number
			{
			   printf("%d", $1);
			}
		| '{' numbers '}'
			{
			}
		;

numbers		: flags
			{
			   printf(" ");
			}
		| numbers ',' flags
			{
			   printf(" ");
			}
		;


/*
 * UTILITY definitions ******************************
 */

number          : expr
			{
			  $$ = $1;
			}
		;
expr            : term
			{
			  $$ = $1;
			}
		| '-' term %prec UMINUS
			{
			  $$ = -$2;
			}
		| '+' term %prec UPLUS
			{
			  $$ = $2;
			}
		| expr '+' term
			{
			  $$ = $1 + $3;
			}
		| expr '-' term
			{
			  $$ = $1 - $3;
			}
		| expr '<' '<' term
			{
			  $$ = $1 << $4;
			}
		| expr '>' '>' term
			{
			  $$ = $1 >> $4;
			}
		;
term            : factor
			{
			  $$ = $1;
			}
		| term '*' factor
			{
			  $$ = $1 * $3;
			}
		| term '/' factor
			{
			  if ($3 != 0)
			    $$ = $1 / $3;
			  else
			  {
			     fatal("Division by zero!");
			     $$ = 0;
			  }
			}
		;
factor          : '(' expr ')'
			{
			  $$ = $2;
			}
		| PNUM
			{
			  $$ = $1;
			}
		;
%%

void syntax(const char *str)
{
   extern char *yytext;
   
   fprintf(stderr, "Token: '%s'\n", yytext);
}

void fatal(const char *str)
{
   fprintf(stderr, "FATAL: %s\n", str);
}
