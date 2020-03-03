/*
 *          NameGen - Jacob Bay Hansen and Mads Haahr in 1991.
 *
 * Version 1.0 - August 5th 1991.  This program is freely distributable.
 *
 *       To obtain the latest version, send 500 gold pieces ;-) to
 *
 *               jake@freja.diku.dk or maze@freja.diku.dk
 *
 *                 See NameGen.doc for further details.
 *
 *           Humbly Ported to Linux by seifert@valhalla-usa.com
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_PREFIX	40
#define MAX_MIDFIX	40
#define MAX_SURFIX	40

#define FIX_LENGTH	8

#define WIZARD		0
#define DWARF		1
#define ELF		2

/*
 * These are the structs ...
 */

struct group
{
   char prefix[MAX_PREFIX][FIX_LENGTH];
   char midfix[MAX_PREFIX][FIX_LENGTH];
   char surfix[MAX_PREFIX][FIX_LENGTH];
   int min_midfix, max_midfix;
};

struct group groups[]  =
{
{	/* Wizard like names - prefixes */
{"Ni",	"Ten",	"Pa",	"Fi",	"Ra",	"Ar",	"Mor",	"Ny",	"Bi",	"Oti",
"El",	"Ga",	"Za",	"Qui",	"Or",	"As",	"Ti",	"Zo",	"Ne",	"Er",
"Cha",	"Cu",	"Wel",	"Ir",	"Nu",	"Nam",	"Ja",	"Ma",	"U",	"Akh",
"An",	"De",	"Ol",	"Jo",	"Y",	"Ha",	"",	"",	"",	""}
,	/* Wizard like names - midfixes */
{"sto",	"ser",	"fa",	"min",	"po",	"den",	"don",	"dan",	"din",	"he",
"fe",	"qa",	"thus",	"the",	"do",	"fi",	"tor",	"kai",	"'",	"-",
"za",	"xa",	"fyl",	"zyn",	"lo",	"hi",	"ta",	"ti",	"zon",	"yar",
"di",	"zo",	"ra",	"te",	"mo",	"cro",	"no",	"mi",	"stu",	"stan"}
,	/* Wizard like names - surfixes */
{"lus",	"les",	"ser",	"nen",	"el",	"lin",	"tra",	"ster",	"mus",	"mis",
"alf",	"gast",	"gist",	"man",	"ron",	"don",	"mort",	"star",	"ban",	"staz",
"vix",	"ul",	"as",	"cus",	"deus",	"li",	"ac",	"con",	"roth",	"zir",
"tar",	"gul",	"zak",	"son",	"zen",	"fax",	"fox",	"vil",	"win",	"dow"},
 0, 4	/* From 0 to 4 midfixes for Wizard like names */
},{	/* Dwarf like names - prefixes */
{"Dwa",	"Ba",	"Tho",	"Do",	"No",	"Fi",	"Ki",	"Bi",	"Bo",	"Kao",
"Dwi",	"Du",	"Nu",	"O",	"Lo",	"Mo",	"Ko",	"Bu",	"Fu",	"Mu",
"Dwo",	"Thi",	"Thu",	"Tha",	"Bru",	"Dru",	"Za",	"Zi",	"Zo",	"Mi",
"",	"",	"",	"",	"",	"",	"",	"",	"",	""}
,	/* Dwarf like names - midfixes */
{"m",	"l",	"r",	"f",	"b",	"k",	"d",	"n",	"g",	"t",
"",	"",	"",	"",	"",	"",	"",	"",	"",	"",
"",	"",	"",	"",	"",	"",	"",	"",	"",	"",
"",	"",	"",	"",	"",	"",	"",	"",	"",	""}
,	/* Dwarf like names - surfixes */
{"in",	"ar",	"i",	"or",	"ur",	"a",	"ir",	"iz",	"er",	"",
"",	"",	"",	"",	"",	"",	"",	"",	"",	"",
"",	"",	"",	"",	"",	"",	"",	"",	"",	"",
"",	"",	"",	"",	"",	"",	"",	"",	"",	""}
, 1, 1	/* Always one midfix for Dwarf like names */
},{	/* Elf like names - prefixes */
{"Le",	"Ga",	"Lau",	"A",	"Si",	"Ce",	"E",	"Fa",	"Go",	"Is",
"",	"",	"",	"",	"",	"",	"",	"",	"",	"",
"",	"",	"",	"",	"",	"",	"",	"",	"",	"",
"",	"",	"",	"",	"",	"",	"",	"",	"",	""}
,	/* Elf like names - midfixes */
{"lo",	"la",	"le",	"li",	"ho",	"ha",	"he",	"hi",	"",	"",
"ro",	"ra",	"re",	"ri",	"mo",	"ma",	"me",	"mi",	"",	"",
"go",	"ga",	"ge",	"gi",	"",	"",	"",	"",	"",	"",
"",	"",	"",	"",	"",	"",	"",	"",	"",	""}
,	/* Elf like names - surfixes */
{"las",	"driel","na",	"kiel",	"mir",	"on",	"os",	"as",	"el",	"es",
"en",	"th",	"n",	"r",	"lan",	"",	"",	"",	"",	"",
"",	"",	"",	"",	"",	"",	"",	"",	"",	"",
"",	"",	"",	"",	"",	"",	"",	"",	"",	""}
, 1, 2	/* From one to two midfixes for Elf like names */
}
};

/*
 * And here come the prototypes ...
 */

void generate(int group_number, int number);
void about(void);
unsigned rnd(unsigned low, unsigned high);

/*
 * Main code ...
 */

int main(int argc, char *argv[])
{
   int group_number, number;
   char *count;

   srand(time(0));

   if (argc == 1)
   {
      about();
      exit(0);
   } else if (argc == 2 || argc == 3) {
      for(count=argv[1];*count!=0;count++)
	*count = tolower(*count);
      if (strcmp(argv[1], "-help")==0 ||
	  strcmp(argv[1], "help")==0 ||
	  strcmp(argv[1], "-h")==0 || strcmp(argv[1], "?")==0) {
	 about();
	 exit(0);
      } else if (strcmp(argv[1], "wizard")==0)
	group_number = WIZARD;
      else if (strcmp(argv[1], "dwarf")==0)
	group_number = DWARF;
      else if (strcmp(argv[1], "elf")==0)
	group_number = ELF;
      else {
	 printf("NameGen: Unknown group specified.\n");
	 exit(10);
      };
      if (argc == 2)
	number = 20;
      else
	number = atoi(argv[2]);
      generate(group_number, number);
   } else {
      printf("NameGen: Too many arguments.\n");
      exit(10);
   }
   exit(0);
}

/*
 * Subroutines ...
 */

void generate(int group_number, int number)
{
   int i, r, s;

   for(i=0;i<number;i++)
   {
      do
	r = rnd(0, MAX_PREFIX-1);
      while (groups[group_number].prefix[r][0] == 0);
      printf("%s", groups[group_number].prefix[r]);
      
      for(r = rnd(groups[group_number].min_midfix,
		  groups[group_number].max_midfix);r>=1;r--)
      {
	 do
	   s = rnd(0, MAX_MIDFIX-1);
	 while (groups[group_number].midfix[s][0] == 0);
	 printf("%s", groups[group_number].midfix[s]);
      }
      
      do
	r = rnd(0, MAX_SURFIX-1);
      while (groups[group_number].surfix[r][0] == 0);

      printf("%s\n", groups[group_number].surfix[r]);
   }
}


void about(void)
{
   printf("NameGen - Name generator (v1.0) for role playing games.\n");
   printf("          CopyLeft 1991 Jacob Bay Hansen and Mads Haahr.\n");
   printf("          This program is freely distributable.\n");
   printf("Usage:    \"NameGen <group> [number]\"\n");
   printf("          where <group> determines the style of the name as follows:\n");
   printf("             WIZARD - Wizard like names\n");
   printf("             DWARF  - Dwarf like names\n");
   printf("             ELF    - Elf like names\n");
   printf("          and [number] states the number of names wanted (default 20).\n");
   return;
}


unsigned rnd(unsigned low, unsigned high)
{
   return (rand() % (high - low + 1)) + low;
}

