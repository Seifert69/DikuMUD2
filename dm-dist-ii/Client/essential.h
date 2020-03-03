/* *********************************************************************** *
 * File   : essential.h                               Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : all                                                            *
 *                                                                         *
 * Purpose: Essential independent definitions for any module.              *
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

#ifndef _MUD_ESSENTIAL_H
#define _MUD_ESSENTIAL_H

#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <assert.h>

enum log_level { LOG_OFF, LOG_BRIEF, LOG_EXTENSIVE, LOG_ALL };

#define MAX_STRING_LENGTH   4096
#define MAX_INPUT_LENGTH     200

#define TRUE  1
#define FALSE 0

#if defined(NULL)

#undef NULL
/* I need this for various C++ modules, MS */
#define NULL (0)

/*   #define NULL ((void *) 0) */
#endif

#define BITCONV16(i) ((((ubit16) i)>>8)|(( ((ubit16) i)&255)<<8))
#define BITCONV32(i) ((((ubit32) i)>>16)|(( ((ubit32) i)&65535)<<16))

#define ISNEWL(ch) ((ch) == '\n' || (ch) == '\r')

#define SGN(i) (((i)>=0)?1:-1)

#define ODD(i)  ((i) & 1)
#define EVEN(i) (!((i) & 1))

#define TAIL(str) for(; *(str); (str)++)

#define STRCAT(p,c) strcat(p,c); TAIL(p)

#define STR(str) (str ? str : "(NIL POINTER)")

#define IF_STR(st) ((st) ? (st) : "")

#define ANA(c)     \
  (strchr("aeiouyAEIOUY", c) ? "an" : "a")

#define CAPC(st)  (*(st) = toupper(*(st)))
#define CAP(st)   (CAPC(st), (st))

#define CREATE(res, type, num) \
  do { if (((res) = (type *) calloc((num), sizeof(type))) == NULL) \
	 assert(FALSE); } while(0)

#define RECREATE(res, type, num) \
  do { if (((res) = (type *) realloc((res), sizeof(type) * (num))) == NULL) \
	 assert(FALSE); } while(0)

#define IS_SET(flag,bit)  ((flag) & (bit))

#define SET_BIT(var,bit)  ((var) |= (bit))

#define REMOVE_BIT(var,bit)  ((var) &= ~(bit) )

#define TOGGLE_BIT(var,bit)  ((var) ^= (bit))

#define SWITCH(a,b) { (a) ^= (b); (b) ^= (a); (a) ^= (b); }

#ifdef isascii
   #undef isascii
#endif


#define DEBUG(a...) \
   do { \
   fprintf(stderr, a); \
   } while (0)

#define isascii(ch) ((ch >= 32) && (ch <= 126))

#ifndef HPUX
typedef signed char        sbit8;
typedef unsigned char      ubit8;
typedef signed short int   sbit16;
typedef unsigned short int ubit16;

#ifdef DOS
typedef signed long        sbit32;
typedef unsigned long      ubit32;
#else
typedef signed int         sbit32;
typedef unsigned int       ubit32;
#endif

#endif /* HPUX */

typedef ubit8               ubit1; /* Boolean */

void slog(enum log_level, ubit8, const char *, ...);


#ifdef MEMORY_DEBUG
   void safe_free(void *p);
   void *safe_malloc(size_t size);
   void *safe_calloc(size_t nobj, size_t size);
   void *safe_realloc(void *p, size_t size);

   #define free(p)             (safe_free((void *) p), (p = NULL))
   #ifdef malloc
      #undef malloc
   #endif
   #define malloc(size)        safe_malloc(size)
   #ifdef calloc
      #undef calloc
   #endif
   #define calloc(nobj, size)  safe_calloc((nobj), (size))
   #define realloc(p, size)    safe_realloc((p), (size))
#endif
#endif
