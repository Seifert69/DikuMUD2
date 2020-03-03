/* *********************************************************************** *
 * File   : affect.h                                  Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Header for the Affect system.                                  *
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

#ifndef _MUD_AFFECT_H
#define _MUD_AFFECT_H

void apply_affect(struct unit_data *unit);
void start_affect(struct unit_data *unit);
void stop_affect(struct unit_data *unit);
struct unit_affected_type *affected_by_spell(const struct unit_data *unit,
					     sbit16 id);

void create_affect(struct unit_data *unit, struct unit_affected_type *orgaf);
void destroy_affect(struct unit_affected_type *af);
void affect_clear_unit(struct unit_data *unit);


/* These functions may not send messages - nor destroy units. Only */
/* affect a units values                                           */
struct apply_function_type
{
  const char *descr;
  ubit1 (*func)(struct unit_affected_type *af, struct unit_data *unit,ubit1 set);
};


/* These functions may send messages and destroy any unit. */
struct tick_function_type
{
  const char *descr;
  void (*func)(struct unit_affected_type *af, struct unit_data *unit);
};

#endif /* _MUD_AFFECT_H */
