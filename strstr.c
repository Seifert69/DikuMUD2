/* *********************************************************************** *
 * File   : strstr.c                                  Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : Unknown.                                                       *
 *                                                                         *
 * Purpose: Unknown.                                                       *
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
/*
From: torek@elf.bsdi.com (Chris Torek)
Newsgroups: comp.lang.c
Subject: Re: question about strstr() algorithm
Date: 12 Oct 1994 04:18:55 -0700
Organization: Berkeley Software Design, Inc.

As the author of the 4.4BSD strstr(), I am not sure if I am supposed
to be offended. :-)

Here is the code, minus copyright and includes:
*/

/*
 * Find the first occurrence of find in s.
 */
char *strstr2(register const char *s, register const char *find)
{
  register char c, sc;
  register size_t len;

  if ((c = *find++) != 0)
  {
    len = strlen(find);
    do {
      do {
	if ((sc = *s++) == 0)
	  return (NULL);
      }
      while (sc != c);
    }
    while (strncmp(s, find, len) != 0);
    s--;
  }
  return ((char *)s);
}
