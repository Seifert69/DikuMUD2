/* *********************************************************************** *
 * File   : justice.h                                 Part of Valhalla MUD *
 * Version: 1.01                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Header for justice.c                                           *
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

#ifndef _MUD_JUSTICE_H
#define _MUD_JUSTICE_H

#define CRIME_LIFE   12      /* Remember crime for (5 mins * CRIME_LIFE)    */

#define CRIME_NONPRO  8      /* Person will be non-protected                */
#define CRIME_OUTLAW 32      /* Person is wanted                            */
#define CRIME_REWARD 64      /* Person will be wanted dead for reward.      */

/* STRUCTS ---------------------------------------------------------------*/

void offend_legal_state(class unit_data *ch, class unit_data *victim);

void log_crime(struct unit_data *criminal, struct unit_data *victim,
	       ubit8 crime_type, int active = TRUE);

void set_reward_char(struct unit_data *ch, int crimes);

#endif /* _MUD_JUSTICE_H */
