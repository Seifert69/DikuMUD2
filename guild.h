/* *********************************************************************** *
 * File   : guild.h                                   Part of Valhalla MUD *
 * Version: 1.10                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Basic special routines handling guilds.                        *
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

#ifndef _MUD_GUILD_H
#define _MUD_GUILD_H

int char_guild_level(struct unit_data *ch);
void advance_guild_level(struct unit_data *ch);


#endif
