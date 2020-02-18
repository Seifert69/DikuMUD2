/* *********************************************************************** *
 * File   : dbfind.h                                  Part of Valhalla MUD *
 * Version: 1.05                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Header for the database stuff.                                 *
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

#ifndef _MUD_DBFIND_H
#define _MUD_DBFIND_H

struct descriptor_data *find_descriptor(const char *name,
					struct descriptor_data *except);
struct zone_type *find_zone(const char *zonename);
struct file_index_type *find_file_index(const char *zonename, const char *name);
struct diltemplate *find_dil_index(char *zonename, char *name);
struct unit_data *world_room(const char *zone, const char *name);
struct diltemplate *find_dil_template(const char *name);

struct file_index_type *str_to_file_index(const char *str);
struct file_index_type *pc_str_to_file_index(const struct unit_data *ch, const char *str);

#endif
