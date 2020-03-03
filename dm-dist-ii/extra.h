/* *********************************************************************** *
 * File   : extra.h                                   Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert                                                        *
 *                                                                         *
 * Purpose: Extra descriptions                                             *
 * Bugs   : None known.                                                    *
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

#ifndef _MUD_EXTRA_H
#define _MUD_EXTRA_H

#include "essential.h"
#include "namelist.h"

class extra_descr_data
{
  public:
   extra_descr_data(void);
   ~extra_descr_data(void);

   void AppendBuffer(CByteBuffer *pBuf);

   class extra_descr_data *find_raw(const char *name);
   class extra_descr_data *add(const char *name, const char *descr);
   class extra_descr_data *add(const char **names, const char *descr);
   class extra_descr_data *remove(class extra_descr_data *exd);
   class extra_descr_data *remove(const char *name);


   void free_list(void);

   class cNamelist names;         /* Keyword in look/examine          */
   class cStringInstance descr;   /* What to see                      */
   class extra_descr_data *next;  /* Next in list                     */
};

const char *unit_find_extra_string(class unit_data *ch,
				   char *word, class unit_data *list);

class extra_descr_data *char_unit_find_extra(class unit_data *ch,
					     class unit_data **target,
					     char *word,
					     class unit_data *list);

class extra_descr_data *unit_find_extra(const char *word,
					class unit_data *unit);

#endif /* _MUD_STRUCTS_H */
