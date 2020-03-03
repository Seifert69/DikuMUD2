/* *********************************************************************** *
 * File   : str_parse.cc                              Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Functions for easy char * manipulation when parsing user data. *
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

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "essential.h"
#include "system.h"


/* return string without leading spaces */
char *skip_blanks(const char *string)
{
   assert(string);

   for(; *string && isspace(*string); string++)
     ;

   return (char *) string;
}


/* Returns true is arg is empty */
ubit1 str_is_empty(const char *arg)
{
   if (arg == NULL)
     return TRUE;

   return *(skip_blanks(arg)) == '\0';
}


int isfilechar(char c)
{
   if (isalnum(c))
     return TRUE;

   if (c == '.')
     return TRUE;

   if (c == '-' || c == '+' || c == '_' || c == '@' || c == '&' || c == '#')
     return TRUE;

   return FALSE;
}

/* Make a string lowercase, and also return length of string */
int str_lower(char *s)
{
   int l;

   for(l = 0; *s; s++, l++)
     if (isupper(*s))
       *s = tolower(*s);

   return l;
}

int isfilename(char *fname)
{
   char *cp;

   if (str_is_empty(fname))
     return FALSE;

   if (!isalnum(*fname))
     return FALSE;

   str_lower(fname);

   if (strcmp(fname, DIR_FILE) == 0)
     return FALSE;

   for (cp = fname; *cp; cp++)
     if (!isfilechar(*cp))
       return FALSE;
   
   cp = strchr(fname, '.');

   if (cp)
   {
      cp = strchr(cp+1, '.');
      if (cp)
	return FALSE;
   }

   return TRUE;
}


/* Allocate space for a copy of source */
char *str_dup(register const char *source)
{
   register char *dest;

   if (source)
   {
      CREATE(dest, char, strlen(source) + 1);
      strcpy(dest, source);

      return dest;
   }

   return NULL;
}


/* Add a new name to the end of an existing namelist */
char **add_name(const char *name, char **namelist)
{
  int pos = 0;
  
  assert(name && namelist);

  while (namelist[pos])
    pos++;

  RECREATE(namelist, char *, pos+2);

  namelist[pos] = str_dup(name);
  namelist[pos + 1] = NULL;

  return namelist;
}


/* Delete a name from namelist, move last name to field */
char **del_name(const char *name, char **namelist)
{
  int pos = 0, end = 0;

  assert(name && namelist);

  if (!namelist[0] || !namelist[1])
    return namelist;

  while (namelist[end])
    end++;

  for (pos = 0; pos < end; pos++)
    if (strcmp(name, namelist[pos]) == 0)
    {
      free(namelist[pos]);
      while (++pos <= end)
	namelist[pos - 1] = namelist[pos];

      assert(end > 0);
      RECREATE(namelist, char *, end);
      return namelist;
    }

  return namelist;
}


/* free space allocated to a namelist */
void free_namelist(char **list)
{
  char **original;

  assert(list);

  original = list;

  while (*list)
  {
     free(*(list));
     /* MS: Well, ugly but we have to do while free macro is in use! */
     list++;
  }

  free(original);
}


/* Create an empty namelist */
char **create_namelist(void)
{
  char **list;

  CREATE(list, char *, 1);
  list[0] = NULL;

  return list;
}


/* returns a pointer to a (static) string wich contains s1 and s2 */
char *str_cc(const char *s1, const char *s2)
{
   static char buf[256];

   strcpy(buf, s1);
   strcat(buf, s2);

   return buf;
}


/*  As defined by 2nd Ed. of K&R ANSI C, but non case sensitive
 *  Return pointer to first occurence of ct in cs - or NULL
 *  Used to determine ei. "from" and "in"
 */
char *str_cstr(register const char *cs, register const char *ct)
{
   register char *si;
   register char *ti;

   do
   {
      for(si = (char *) cs, ti = (char *) ct;
	  tolower(*si) == tolower(*ti); si++)
	if (*++ti == '\0') return (char *) cs;
   } while (*cs++);

   return NULL;
}


char *parse_match(char *pData, char *pMatch)
{
   char *pTmp1, *pTmp2;

   pTmp1 = str_cstr(pData, pMatch);
   if (pTmp1 == NULL)
     return NULL;

   pTmp2 = skip_blanks(pTmp1 + strlen(pMatch));
   if (*pTmp2 != '=')
     return NULL;

   pTmp2 = skip_blanks(pTmp2 + 1);

   if (*pTmp2)
     return pTmp2;
   else
     return NULL;
}

char *parse_name(char **pData)
{
   char Buf[MAX_STRING_LENGTH];
   char *pTmp1, *pTmp2;

   Buf[0] = 0;

   pTmp1 = skip_blanks(*pData);
   if (*pTmp1 != '~')
     return NULL;

   pTmp1++;

   for (pTmp2 = pTmp1; *pTmp2; pTmp2++)
     if (*pTmp2 == '~')
       break;

   if (*pTmp2 != '~')
     return NULL;

   memcpy(Buf, pTmp1, pTmp2 - pTmp1);
   Buf[pTmp2 - pTmp1] = 0;
   
   *pData = pTmp1 + (1+ pTmp2 - pTmp1); /* +1 to skip the last ~ */
   
   return str_dup(Buf);
}

int parse_num(char **pData, int *pNum)
{
   char *pTmp1;

   *pNum = 0;

   pTmp1 = skip_blanks(*pData);
   if (!isdigit(*pTmp1) && *pTmp1 != '+' && *pTmp1 != '-')
     return FALSE;

   *pNum = atoi(pTmp1);
   if (*pTmp1 == '+' || *pTmp1 == '-')
     pTmp1++;

   while (isdigit(*pTmp1))
     pTmp1++;

   if (!isspace(*pTmp1) && *pTmp1 != 0)
     return FALSE;

   *pData = pTmp1;

   return TRUE;
}

int *parse_numlist(char **pData, int *int_count)
{
   int *nums = NULL;
   int count = 0, i;

   while (parse_num(pData, &i))
   {
      count++;
      if (count == 1)
	CREATE(nums, int, count);
      else
	RECREATE(nums, int, count);

      nums[count-1] = i;
      while ((isspace(**pData) || ispunct(**pData)))
	(*pData)++;
   }

   *int_count = count;
   return nums;
}

char **parse_namelist(char **pData)
{
   char *pTmp1, *pTmp2;
   char **pNamelist;

   pTmp1 = *pData;

   pNamelist = create_namelist();

   while ((pTmp2 = parse_name(&pTmp1)))
   {
      pNamelist = add_name(pTmp2, pNamelist);
      while (*pTmp1 != '~' && (isspace(*pTmp1) || ispunct(*pTmp1)))
	pTmp1++;
   }

   if (pNamelist[0] == NULL)
   {
      free_namelist(pNamelist);
      return NULL;
   }

   *pData = pTmp1;

   return pNamelist;
}

int parse_match_num(char **pData, char *pMatch, int *pResult)
{
   char *pTmp;

   pTmp = parse_match(*pData, pMatch);
   if (pTmp == NULL)
     return FALSE;

   
   if (!(parse_num(&pTmp, pResult)))
     return FALSE;

   *pData = pTmp;

   return TRUE;
}

int *parse_match_numlist(char **pData, char *pMatch, int *count)
{
   char *pTmp;
   int *i;

   *count = 0;
   pTmp = parse_match(*pData, pMatch);
   if (pTmp == NULL)
     return NULL;
   
   i = parse_numlist(&pTmp, count);

   *pData = pTmp;

   return i;
}

char *parse_match_name(char **pData, char *pMatch)
{
   char *pName, *pTmp;

   pTmp = parse_match(*pData, pMatch);
   if (pTmp == NULL)
     return NULL;

   pName = parse_name(&pTmp);

   *pData = pTmp;

   return pName;
}

char **parse_match_namelist(char **pData, char *pMatch)
{
   char **pNamelist, *pTmp;

   pTmp = parse_match(*pData, pMatch);
   if (pTmp == NULL)
     return NULL;

   pNamelist = parse_namelist(&pTmp);

   *pData = pTmp;

   return pNamelist;
}


