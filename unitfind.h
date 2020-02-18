/* *********************************************************************** *
 * File   : unitfind.h                                Part of Valhalla MUD *
 * Version: 2.24                                                           *
 * Author : Var.                                                           *
 *                                                                         *
 * Purpose: Basic routines for finding units.                              *
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


#ifndef _MUD_UNITFIND_H
#define _MUD_UNITFIND_H


#define UVI(i) (unit_vector.units[i])
#define UVITOP (unit_vector.top)

struct unit_vector_data
{
   struct unit_data **units;
   int size;
   int top;
};

extern struct unit_vector_data unit_vector;

ubit1 same_surroundings(struct unit_data *u1, struct unit_data *u2);
void scan4_unit_room(struct unit_data *room, ubit8 type);
void scan4_unit(struct unit_data *ch, ubit8 type);
struct unit_data *scan4_ref(struct unit_data *ch, struct unit_data *fu);

int random_direction(struct unit_data *ch);
struct unit_data *find_unit(const struct unit_data *ch, char **arg,
			    const struct unit_data *list,
			    const ubit32 bitvector);
struct unit_data *find_unit_general(const struct unit_data *viewer, 
			    const struct unit_data *ch, 
			    char **arg,
			    const struct unit_data *list,
			    const ubit32 bitvector);

struct unit_data *find_symbolic_instance(struct file_index_type *fi);
struct unit_data *find_symbolic(char *zone, char *name);
struct unit_data *find_symbolic_instance_ref(struct unit_data *ref,
					     struct file_index_type *fi,
					     ubit16 bitvector);
struct unit_data *random_unit(struct unit_data *ref, int sflags, int tflags);


#endif
