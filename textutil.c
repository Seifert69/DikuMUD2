/* *********************************************************************** *
 * File   : textutil.c                                Part of Valhalla MUD *
 * Version: 1.02                                                           *
 * Author : Mostly seifert@diku.dk and quinn@diku.dk                       *
 *                                                                         *
 * Purpose: Routines handling manipulation of text strings.                *
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

/* 26/07/93 MS+HHS : Fixed del-name bug (again)                            */
/* 06/07/93 HHS    : added str_cc                                          */
/* 23/08/93 jubal  : Corrections in indent_text                            */
/* 27/08/94 gnort  : Changes in indent_text                                */
/* 03/09/94 gnort  : Added CAP to indent_text                              */
/* 12/09/94 gnort  : Added const'ness to various functions' string args    */
/* 14/09/94 seifert: Optimized and split is_name (is_name_raw)             */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "structs.h"
#include "utils.h"
#include "utility.h"
#include "textutil.h"
#include "common.h"

/*  From char * input stream 'str' copy characters into 'buf' until
 *  end of string or newline. Returns position of 'str' after copied
 *  characters.
 */
char *str_line(const char *str, char *buf)
{
   if (str == NULL || buf == NULL)
     return NULL;

   for (; *str == ' ' || *str == '\n' || *str == '\r'; str++)
     ;

   if (*str == '\0')
     return NULL;

   for (; (*buf = *str); buf++, str++)
     if (*str == '\n' || *str == '\r')
       break;
  
   *buf = '\0';			/* Erase last separator */

   return (char *)str;
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


/* Make a string lowercase in another buffer. Also return length of string */

int str_lower(const char *s, char *d, int nBufSize)
{
   int l;

   for (l = 0; *s && (l < nBufSize); s++, d++, l++)
     if (isupper(*s))
       *d = tolower(*s);
     else
       *d = *s;

   if (l < nBufSize)
     *d = 0;
   else
   {
     slog(LOG_ALL, 0, "ERROR: str_lower destination buffer too "
	  "small! Advise Papi.");
     *(d-1) = 0;
   }

   return l;
}


/* Return a string consisting of `n' spaces */
char *spc(int n)
{
   static char buf[256];

   buf[n] = '\0';

   if (n > 256)
     n = 256;

   for (;n;)
     buf[--n] = ' ';

   return buf;
}


/*  Return a pointer to the string containing the ascii reresentation
 *  of the integer 'n'
 *  I've made it the easy way :)
 */
#ifdef DOS
char *itoa_dos(int n)
#else
char *itoa(int n)
#endif
{
  static char buf[32];  /* 32 digits can even cope with 64 bit ints */

  sprintf(buf, "%d", n);
  return buf;
}


/*  Return a pointer to the string containing the ascii reresentation
 *  of the integer 'n'
 *  I've made it the easy way :)
 */
char *ltoa(long n)
{
  static char buf[32];  /* 32 digits can even cope with 64 bit ints */

  sprintf(buf, "%ld", n);
  return buf;
}

/*  STR Convention: "str_" [n] [c] <meaning>
 *  n means - take number as argument, to limit operation
 *  c means - ignore possible difference in case sensitivity
 *  n always precedes c
 *  Example: str_nccmp - compare n letters non-casesensitive
 */


/*  Compare two strings without case-sensitivity
 *  Return result as defined in strcmp
 */
int str_ccmp(const char *s, const char *d)
{
  if (s == d)
    return 0;

  if (s == NULL)
    return -1;
  else if (d == NULL)
    return 1;

  for (; tolower(*s) == tolower(*d); s++, d++)
    if (*s == '\0')
      return 0;
  
  return (tolower(*s) - tolower(*d));
}


/*  Compare max n chars without case-sensitivity
 *  Return result as defined in strcmp
 */
int str_nccmp(const char *s, const char *d, int n)
{
   if (s == d)
     return 0;

   if (s == NULL)
     return -1;
   else if (d == NULL)
     return 1;

   for (n--; tolower(*s) == tolower(*d); s++, d++, n--)
     if (*s == '\0' || n <= 0)
       return 0;

   return (tolower(*s) - tolower(*d));
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


/*  As defined by 2nd Ed. of K&R ANSI C
 *  Return pointer to first occurence of ct in cs - or NULL
 *  Used to determine ei. "from" and "in"
 */
char *str_str(register const char *cs, register const char *ct)
{
   register char *si;
   register char *ti;

   do
   {
      si = (char *) cs;
      ti = (char *) ct;

      while (*si++ == *ti++)
	if (*ti == '\0') return (char *) cs;

   } while (*cs++);

   return NULL;
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


/* return string without leading spaces */
char *skip_blanks(const char *string)
{
   if (string == NULL)
     return NULL;

   for(; *string && isspace(*string); string++)
     ;

   return (char *) string;
}


/* return string without leading spaces */
char *skip_spaces(const char *string)
{
   if (string == NULL)
     return NULL;

   for(; *string && isaspace(*string); string++)
     ;

   return (char *)string;
}


void strip_trailing_blanks(char *str)
{
   if (!*str)			/* empty string: return at once      */
     return;

   for ( ; *str; ++str)		/* wind to end of string             */
     ;

   if (!isspace(*--str))	/* Not a spaceterminated string      */
     return;			/* This is mainly for `inter-code' strings */

   while (isspace(*--str))	/* rewind to last nonspace character */
     ;

   *++str = '\0';		/* step ahead and end string         */
}


void strip_trailing_spaces(char *str)
{
   if (!*str)			/* empty string: return at once      */
     return;

   for ( ; *str; ++str)		/* wind to end of string             */
     ;

   if (!isaspace(*--str))	/* Not a spaceterminated string      */
     return;			/* This is mainly for `inter-code' strings */

   while (isaspace(*--str))	/* rewind to last nonspace character */
     ;

   *++str = '\0';		/* step ahead and end string         */
}


/* Returns true is arg is empty */
ubit1 str_is_empty(const char *arg)
{
   if (arg == NULL)
     return TRUE;

   return *(skip_blanks(arg)) == '\0';
}


/* Check if the string contains nothing but numbers */
ubit1 str_is_number(const char *str)
{
   if (!*str)
     return FALSE;

   for (; *str; str++)
     if (!isdigit(*str))
       return FALSE;

   return TRUE;
}


/* Check if the next word in str is a number */
ubit1 next_word_is_number(const char *str)
{
   char tmp[MAX_STRING_LENGTH];
   str_next_word(str, tmp);
   return str_is_number(tmp);
}


/* Block must end with null pointer, -1 if not found otherwise index */
/* Warning, argument 1 is made into lowercase!                       */
/* Warning, list of names must be in lowercase to match up! */
int search_block(const char *oarg, const char **list, ubit1 exact)
{
   char arg[4096];
   register int i, l;

   /* Make into lower case, and get length of string */
   l = str_lower(oarg, arg, sizeof(arg));

   if (exact)
   {
      for (i = 0; list[i]; i++)
	if (strcmp(arg, list[i]) == 0)
	  return i;
   }
   else
   {
      if (!l)
	l = 1;	  /* Avoid "" to match the first available string */

      for(i = 0; list[i]; i++)
	if (strncmp(arg, list[i], l) == 0)
	  return i;
   }
  
   return -1;
}


/* Block must end with null pointer                                  */
/* Warning, argument 1 is made into lowercase!                       */
int search_block_length(const char *oarg, int length,
			const char **list, ubit1 exact)
{
   char arg[4096];
   register int i;

   /* Make into lower case, and get length of string */
   str_lower(oarg, arg, sizeof(arg));

   if (exact)
   {
      for (i = 0; list[i]; i++)
	if (!strncmp(arg, list[i], length))
	  if (list[i][length] <= ' ')
	    return i;
   }
   else
   {
      if (!length)
	length = 1;	/* Avoid "" to match the first available string */
      for (i = 0; list[i]; i++)
	if (!strncmp(arg, list[i], length))
	  return i;
   }

   return -1;
}


int fill_word(const char *argument)
{
   return (search_block(argument, fillwords, TRUE) >= 0);
}


/* Exactly as str_next-word, except it wont change the case */
char *str_next_word_copy(const char *argument, char *first_arg)
{
   /* Find first non blank */
   argument = skip_spaces(argument);

   /* Copy next word and make it lower case */
   for (; *argument > ' '; argument++)
     *first_arg++ = *argument;

   *first_arg = '\0';

   return skip_spaces(argument);
}


/* Copy next word from argument into first_arg and make lowercase*/
char *str_next_word(const char *argument, char *first_arg)
{
   /* Find first non blank */
   argument = skip_spaces(argument);

   /* Copy next word and make it lower case */
   for (; *argument > ' '; argument++)
     *first_arg++ = tolower(*argument);

   *first_arg = '\0';

   return skip_spaces(argument);
}


/*  Find the first sub-argument of a string,
 *  return pointer to first char in primary argument, following the sub-arg
 */
char *one_argument(const char *argument, char *first_arg)
{
   assert(argument && first_arg);

   do
     argument = str_next_word(argument,first_arg);
   while (fill_word(first_arg));

   return (char *)argument;
}


/* determine if 'arg1' is an abbreviation of 'arg2'. */
ubit1 is_abbrev(const char *arg1, const char *arg2)
{
   if (!*arg1)
     return FALSE;

   for (; *arg1; arg1++, arg2++)
     if (tolower(*arg1) != tolower(*arg2))
       return FALSE;
  
   return TRUE;
}



/* determine if 'arg1' is an abbreviation of 'arg2'.          */
/* The empty string is never an abbrevation of another string */
ubit1 is_multi_abbrev(const char *arg1, const char *arg2)
{
   if (!*arg1)
     return FALSE;

   for (; *arg1 && !isspace(*arg1); arg1++, arg2++)
     if (tolower(*arg1) != tolower(*arg2))
       return FALSE;

   return TRUE;
}


/* Block must end with null pointer, -1 if not found otherwise index */
/* Warning, argument 1 is converted into lowercase!                  */
int search_block_abbrevs(const char *oarg, const char **list, const char **end)
{
   char arg[4096];
   char buf1[MAX_INPUT_LENGTH], buf2[MAX_INPUT_LENGTH];
   char *s, *ps, *bestpos;
   const char *d, *pd;
   int i;
   int match, bestidx;

   i = str_lower(skip_spaces(oarg), arg, sizeof(arg));

   bestidx = -1;
   bestpos = NULL;

   for (i = 0; list[i]; i++)
   {
      for (match = 0, s = arg, d = list[i];;)
      {
	 ps = s;
	 pd = d;
	 s = str_next_word(s, buf1);
	 d = str_next_word(d, buf2);
	 if (is_multi_abbrev(buf1, buf2))
	 {
	    s = skip_spaces(s);
	    d = skip_spaces(d);
	    match++;
	 }
	 else
	   break;
      }
      if (match && ps > bestpos)
      {
	 bestpos = ps;
	 bestidx = i;
      }
   }
  
   if (bestidx != -1)
   {
      *end = oarg + (bestpos - arg);
      return bestidx;
   }

   *end = oarg;
  
   return -1;
}


/* returns a pointer to a (static) string wich contains s1 and s2 */
char *str_cc(const char *s1, const char *s2)
{
   static char buf[1024];

   strncpy(buf, s1, sizeof(buf)-1);
   strncat(buf, s2, sizeof(buf)-1-strlen(buf));

   return buf;
}


/* =================================================================== */
/* Insert the string "str" at the first position in the "itxt" string. */

void str_insert(char *str, char *itxt)
{
   int nlen, slen;

   nlen = strlen(itxt);
   slen = strlen(str);

   memmove(str+nlen, str, slen+1);
   memcpy(str, itxt, nlen);
}

/*
   Scan the string 'str' for the string OLD and replace each
   occurence with new.
   */
void str_substitute(const char *old, const char *newstr, char *str)
{
   char *b;
   int olen, nlen, slen;

   olen = strlen(old);
   nlen = strlen(newstr);
   slen = strlen(str);

   while ((b = str_str(str, old)))
   {
      if (nlen <= olen)
      {
	 memcpy(b, newstr, nlen);
	 memmove(b+nlen, b+olen, slen-(b-str+olen)+1);
	 slen -= olen-nlen;
      }
      else
      {
	 memmove(b+nlen, b+olen, slen-(b-str));
	 memcpy(b, newstr, nlen);
	 slen += nlen-olen;
      }
   }
}


/* Remove all occurences of c in s */
void str_rem(char *s, char c)
{
   int diff;

   if (c == '\0')
     return;

   for (diff=0; *(s+diff);)
   {
      if (*s == c)
	diff++;
      else
	s++;

      *s = *(s+diff);
   }
}


/* Replace all occurences of non alfa, digit with space in s */
void str_rem_punct(char *s)
{
   int diff;

   for (diff=0; *s; s++)
     if (ispunct(*s))
       *s = ' ';
}


/* Replace all occurences of non alfa, digit with space in s */
void str_blank_punct(char *s)
{
   static char c[3] = {' ', ' ', 0};
   int diff;

   for (diff=0; *s; s++)
     if (ispunct(*s))
     {
	c[0] = *s;
	*s = ' ';
	str_insert(s+1, c);
	s += 2;
     }
}


/* Remove all multiple space occurences in s */
void str_remspc(register char *s)
{
   register char *cp;

   while (*s && (*s != ' ' || *(s+1) != ' '))
     s++;

   if (*s == 0)
     return;

   cp = s;

   while (*cp)
   {
      while (*cp == ' ' && *(cp+1) == ' ')
	cp++;

      *s++ = *cp++;
   }
   *s = 0;
}


void str_chraround(char *str, char c)
{
   int l;

   l = strlen(str);
   memmove(str+1, str, l);
   str[0] = c;
   str[l+1] = c;
   str[l+2] = 0;
}


/* Names has the format :                                 */
/*     names[0] pointer to a string                       */
/*     names[N] pointer to the last N'th string           */
/*     names[N+1] NIL pointer                             */
/*                                                        */
/*   Assumes nothing that arg is without leading spaces,  */
/*   no double spaces and contains text                   */

char *is_name_raw(register const char *arg, register const char *names[])
{
   register int i, j;

   for (i=0; names[i]; i++)
   {
      for (j=0; names[i][j]; j++)
	if ( tolower(arg[j]) != tolower(names[i][j]) )
	  break;

      if (!names[i][j])
	if ( !arg[j] || isaspace(arg[j]) )
	  return ((char *) arg)+j;
   }

   return NULL;
}




/*
 ******** Utility routines for the name lists. *******
 */

/*  Names has the format:
 *     names[0] pointer to a string
 *     names[N] pointer to the last N'th string
 *     names[N+1] NIL pointer
 *  Assumes nothing nice about arg - only about names[]
 */
/* We need to copy to BUF in order to prevent crash when */
/* str_remspc might want to change "constant" strings    */

char *is_name(register const char *arg, register const char *names[])
{
   char buf[MAX_INPUT_LENGTH];

   for (; isaspace(*arg); arg++)
     ;

   if (!*arg)
     return 0;

   strcpy(buf, arg);
   str_remspc(buf);

   return is_name_raw(buf, names);
}


/* Create an empty namelist */
char **create_namelist(void)
{
  char **list;

  CREATE(list, char *, 1);
  list[0] = NULL;

  return list;
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


/* don't look too closely at this obscenity - it won't stay long */
char **namestring_to_namelist(const char *str)
{
   char buf[100], **rslt;
   const char *save;

   assert(str && *str);

   save = str;

   rslt = create_namelist();
   while ((str = one_argument(str, buf)), *buf)
     rslt = add_name(buf, rslt);

   free(save);			/* yes, it is pathetic */

   return rslt;
}


char **copy_namelist(char **source)
{
   char **rslt;

   assert(source);

   rslt = create_namelist();

   while (*source)
   {
      rslt = add_name(*source, rslt);
      source++;
   }

   return rslt;
}


/*  Compare if 'next_word' is the next word in buf - if true return pointer
 *  Put no space before or after 'next_word'
 *  Return NIL if not identical, otherwise return pointer to location
 *  just after the match
 */
char *str_ccmp_next_word(const char *buf, const char *next_word)
{
   buf = skip_spaces(buf);

   for(; *next_word; next_word++, buf++)
     if (*next_word != *buf)
       return NULL;

   if (!*buf || isaspace(*buf)) /* Buf must end here or be word separated */
     return (char *)buf;

   return NULL;
}


/*  Must receive a string of the format 'name@zone\0' or
 *  'zone/name\0'. It sabotages the special symbol '@'
 *  or '/' if any. If no special symbol name will
 *  contain the string
 */
void split_fi_ref(const char *str, char *zone, char *name)
{
   const char *c, *t;
   int  l;

   str = skip_spaces(str);

   if ((c = strchr(str, '@')))
   {
      l = MIN(c - str, FI_MAX_UNITNAME);
      strncpy(name, str, l);
      name[l] = '\0';

      l = MIN(strlen(c + 1), FI_MAX_ZONENAME);
      if ((t = strchr(c+1, ' ')))
	l = MIN(l, t-(c+1));
      strncpy(zone, c + 1, l);
      zone[l] = 0;
   }
   else if ((c = strchr(str, '/')))
   {
      l = MIN(c - str, FI_MAX_ZONENAME);
      strncpy(zone, str, l);
      zone[l] = '\0';

      l = MIN(strlen(c + 1), FI_MAX_UNITNAME);
      if ((t = strchr(c+1, ' ')))
	l = MIN(l, t-(c+1));
      strncpy(name, c + 1, l);
      name[l] = 0;
   }
   else
   {
      if ((c = strchr(str, ' ')))
      {
	 l = MIN(c - str, FI_MAX_UNITNAME);
	 strncpy(name, str, l);
	 name[l] = '\0';
      }
      else
      {
	 l = MIN(strlen(str), FI_MAX_UNITNAME);
	 strncpy(name, str, l);
	 name[l] = 0;
      }
    
      *zone = '\0';
   }
}


char *catnames(char *s, const char **names)
{
   const char **nam;
   ubit1 ok = FALSE;

   if (names)
   {
      strcpy(s, "{");
      TAIL(s);
      for (nam = names; *nam; nam++)
      {
	 ok = TRUE;
	 sprintf(s, "\"%s\",", *nam);
	 TAIL(s);
      }
      if (ok)
	s--;			/* remove the comma */
      strcpy(s, "}");
   }
   else
     sprintf(s, "NULL");

   TAIL(s);

   return s;
}


/* Takes a string and return a pointer to the string on the next line */
/* or the end of string                                               */
char *text_skip_line(char *buf)
{
   char *c;

   if (!*buf)
     return NULL;

   if (!(c = strchr(buf, '\n')) || !(c = strchr(buf, '\r')))
   {
      TAIL(buf);
      return buf;
   }
   
   while (*c == '\n' || *c == '\r')
     ++c;

   return c;
}


char *text_read_line(char *buf, char *res)
{
   while (*buf && *buf != '\n' && *buf != '\r')
     *res++ = *buf++;

   *res = '\0';

   while (*buf == '\n' || *buf == '\r')
     buf++;

   return buf;
}


/* Format the string "str" following the format rules. No longer than */
/* 'destlen' result. Returns resulting length.                        */

int str_escape_format(const char *src, char *dest, int destlen, int formatting)
{
   const char *org_dest = dest;
   
   /* Process string */

   if (formatting)
     src = skip_blanks(src);
   
   while (*src)
   {
      if (org_dest - dest > destlen - 3)
	break;

      /* Make double space after '.' */
      if (formatting && *src == '.' && isspace(*(src+1)))
      {
	 *dest++ = *src++;
	 *dest++ = ' ';
	 continue;
      }

      if (*src == '&')
      {
	 src++;
	 switch (tolower(*src))
	 {
	   case 'l':
	    /* if (!formatting)
	       slog(LOG_ALL, 0, "Redundant format code '&l'."); */
	    formatting = FALSE;
	    break;

	   case 'h':
	    strcpy(dest, CONTROL_HOME);
	    TAIL(dest);
	    break;

           case 'f':
	    /* if (formatting)
	       slog(LOG_ALL, 0, "Redundant format code '&f'."); */
	    formatting = TRUE;
	    break;

	   case '&':
	    *dest++ = '&';
	    break;

	   case 'c':
	  {
	     int bright = FALSE;

	     if (*++src == '+')
	     {
		bright = TRUE;
		src++;
	     }

	     switch (tolower(*src))
	     {
	       case 'n':
		if (bright)
		  strcpy(dest, CONTROL_FGB_BLACK);
		else
		  strcpy(dest, CONTROL_FG_BLACK);
		break;

	       case 'r':
		if (bright)
		  strcpy(dest, CONTROL_FGB_RED);
		else
		  strcpy(dest, CONTROL_FG_RED);
		break;

	       case 'g':
		if (bright)
		  strcpy(dest, CONTROL_FGB_GREEN);
		else
		  strcpy(dest, CONTROL_FG_GREEN);
		break;

	       case 'y':
		if (bright)
		  strcpy(dest, CONTROL_FGB_YELLOW);
		else
		  strcpy(dest, CONTROL_FG_YELLOW);
		break;

	       case 'b':
		if (bright)
		  strcpy(dest, CONTROL_FGB_BLUE);
		else
		  strcpy(dest, CONTROL_FG_BLUE);
		break;

	       case 'm':
		if (bright)
		  strcpy(dest, CONTROL_FGB_MAGENTA);
		else
		  strcpy(dest, CONTROL_FG_MAGENTA);
		break;

	       case 'c':
		if (bright)
		  strcpy(dest, CONTROL_FGB_CYAN);
		else
		  strcpy(dest, CONTROL_FG_CYAN);
		break;

	       case 'w':
		if (bright)
		  strcpy(dest, CONTROL_FGB_WHITE);
		else
		  strcpy(dest, CONTROL_FG_WHITE);
		break;

	       default:
		slog(LOG_OFF, 0, "Illegal color value '&f%c'", *src);
		break;
	     }
	     TAIL(dest);
	     break;
	  }

	   case 'b':
	  {
	     src++;
	     switch (tolower(*src))
	     {
	       case 'n':
		strcpy(dest, CONTROL_BG_BLACK);
		break;
	       case 'r':
		strcpy(dest, CONTROL_BG_RED);
		break;
	       case 'g':
		strcpy(dest, CONTROL_BG_GREEN);
		break;
	       case 'y':
		strcpy(dest, CONTROL_BG_YELLOW);
		break;
	       case 'b':
		strcpy(dest, CONTROL_BG_BLUE);
		break;
	       case 'm':
		strcpy(dest, CONTROL_BG_MAGENTA);
		break;
	       case 'c':
		strcpy(dest, CONTROL_BG_CYAN);
		break;
	       case 'w':
		strcpy(dest, CONTROL_BG_WHITE);
		break;
	       default:
		slog(LOG_OFF, 0, "Illegal color value '&b%c'", *src);
		break;
	     }
	     TAIL(dest);
	     break;
	  }

	   case 's':
	  {
	     char Buf[10];
	     const char *c;
	     int rep = 0;
	     int i;

	     src++;
	     c = src;
	     while (isdigit(*c) && rep < (int) sizeof(Buf)-1)
	       Buf[rep++] = *c++;
	     Buf[rep] = 0;
				      
	     if (rep > 0)
	     {
		src = c;
		rep = atoi(Buf);
	     }
	     else
	       rep = 1;

	     if (org_dest - dest + rep > destlen - 3)
	       break;
	      
	     for (i=0; i < rep; i++)
	       *dest++ = ' ';

	     continue;
	  }
	    
	   case 'n':
	    *dest++ = '\n';
	    *dest++ = '\r';
	    src++;
	    while (isspace(*src))
	      src++;
	    continue;

	   default:
	  {
	     slog(LOG_OFF, 0, "Illegal escape character, '&%c'", *src);
	     break;
	  }
	 }
	 if (*src)
	   src++;
	 continue;
      } /* if ('&') */

      if (formatting)
      {
	 if (isspace(*src))
	 {
	    if (!isspace(*(src+1)))
	      *dest++ = ' ';
	    src++;
	 }
	 else
	   *dest++ = *src++;
      }
      else
      {
	 if (*src == '\n')
	 {
	    *dest++ = *src++;
	    *dest++ = '\r';
	 }
	 else
	   *dest++ = *src++;
      }
   }
   
   *dest = '\0';
   
   /* while (dest > org_dest)
      if (isspace(*(dest-1)))
      *--dest = '\0';
      else
      break; */
   
   return org_dest - dest;
}

char *str_escape_format(const char *src, int formatting)
{
   char buf[MAX_STRING_LENGTH] = "";
   str_escape_format(src, buf, sizeof(buf), formatting);

   return str_dup(buf);
}

