/* *********************************************************************** *
 * File   : mergeguild.c                              Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : Unknown.                                                       *
 *                                                                         *
 * Purpose: Unknown.                                                      **
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
#include <assert.h>
#include <string.h>
#include <ctype.h>

char *skip_blanks(const char *string)
{
  assert(string);

  for(; *string && isspace(*string); string++)
    ;

  return (char *) string;
}


/* Returns true is arg is empty */
int str_is_empty(const char *arg)
{
   return *(skip_blanks(arg)) == '\0';
}


void str_char_subst(char *str, char c)
{
   char *cp;

   if (c == 0)
     return;

   while ((cp = strchr(str, c)))
     *cp++ = 0;
}

char *mystrtok(char *s, char delim)
{
   static char *cp = NULL;
   char *cp2;

   if (delim == 0)
     return NULL;

   if (s)
     cp = s;

   if (cp == NULL || *cp == 0)
     return NULL;

   cp2 = cp;

   cp  = strchr(cp, delim);
   if (cp == NULL)
   {
      cp = NULL;
      return cp2;
   }

   *cp++ = 0;

   return cp2;
}

void convert(int idx)
{
   char buf[4096];
   char *name;
   char *code;
   int i, section = 0;

   int costs[5][3] =
   {{ 5, 10, 15},
    { 8, 16, 24},
    {10, 20, 30},
    {15, 30, 45},
    {20, 40, 60}};

   while (!feof(stdin))
   {
      if (!fgets(buf, sizeof(buf), stdin))
	break;

      name = mystrtok(buf, ',');

      if (strncmp(name, "SECTION", 7) == 0)
      {
	 section++;
	 continue;
      }

      if (name && !str_is_empty(name))
      {
	 for (i=0; i < idx; i++)
	 {	   
	    code = mystrtok(NULL, ',');
	    if (code == NULL)
	      break;
	 }

	 if (code && !str_is_empty(code))
	 {
	    int level;
	    int cost;
	    int max;
	    int nums;
	    char *nc;

	    nc = strtok(code, "/");

	    cost  = nc[0] - 'A';
	    nums  = nc[1] - '0';

	    nc = strtok(NULL, "/");
	    level = atoi(nc);

	    nc = strtok(NULL, "/");
	    max = atoi(nc);

	    fprintf(stdout, "%d %2d; %4d; %-30s; %4d; %4d; ",
		    section,
		    level,
		    max,
		    name,
		    (cost + 1) * 50,
		    (cost + 1) * 500);

	    if ((nums >= 0 && nums <= 3) && (cost >= 0 && cost <= 4))
	    {
	       for (i = 0; i < nums; i++)
		 fprintf(stdout, " %2d;", costs[cost][i]);

	       for (; i < 3; i++)
		 fprintf(stdout, "    ");

	       fprintf(stdout, "  0;\n");
	    }
	    else
	    {
	       fprintf(stdout, "Error\n");
	    }
	    
	 }
      }
   }
}



int main(int argc, char *argv[])
{
   if (argc < 2)
   {
      fprintf(stderr, "Usage: %s <1..8>\n", argv[0]);
      return 0;
   }

   convert(atoi(argv[1]));

   return 0;
}
