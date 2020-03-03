/* *********************************************************************** *
 * File   : utility.h                                 Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : all.                                                           *
 *                                                                         *
 * Purpose: Various functions.                                             *
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

#ifndef _MUD_UTILITY_H
#define _MUD_UTILITY_H

#ifdef MIN
#undef MIN
#endif

#ifdef MAX
#undef MAX
#endif

int MIN(int a, int b);
int MAX(int a, int b);

int number(int from, int to);
int dice(int number, int size);

char *sprintbit(char *buf, ubit32 vektor, const char *names[]);
char *sprinttype(char *buf, int type, const char *names[]);

/* in game log stuff below */

#define MAXLOG 10

#define HERE __FILE__, __LINE__

/* Use like this:  error(HERE, "Something went wrong: %s", buf); */
void error(char *file, int line, char *fmt, ...);

struct unit_data *hometown_unit(char *str);

class log_buffer
{
  public:
   log_buffer(void) { str[0] = 0; }

   char str[MAX_INPUT_LENGTH + 50];
   enum log_level level;
   ubit8 wizinv_level;
};

/* For the printing of obj_type information, as used by the identify spell and
 * wstat command.  Look in act_stat.c for details.
 */
struct obj_type_t
{
  char *fmt;
  ubit8 v[5];
};

char *stat_obj_data(struct unit_data *u, struct obj_type_t *obj_data);


#endif
