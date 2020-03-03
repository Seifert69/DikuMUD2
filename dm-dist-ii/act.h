/* *********************************************************************** *
 * File   : act.h                                     Part of Valhalla MUD *
 * Version: 1.10                                                           *
 * Author : seifert                                                        *
 *                                                                         *
 * Purpose: Headers for the act files.                                     *
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

#ifndef _MUD_ACT_H
#define _MUD_ACT_H

const char *obj_wear_size(struct unit_data *ch, struct unit_data *obj,
			  int keyword = -1);


#endif
