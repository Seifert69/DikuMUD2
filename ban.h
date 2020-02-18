/* *********************************************************************** *
 * File   : ban.h                                     Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : gnort@daimi.aau.dk                                             *
 *                                                                         *
 * Purpose: Macros and prototypes for the ban module.                      *
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

#ifndef _MUD_BAN_H
#define _MUD_BAN_H

char site_banned(char *cur_site);
void show_ban_text(char *site, struct descriptor_data *d);

#define NO_BAN        '0'
#define BAN_NEW_CHARS 'n' /* if(until == 0) ban is forever, otherwise      */
#define BAN_TOTAL     't' /* it will be lifted when time(0) passes "until" */

#define BAN_FILE "ban_list"


#endif /* _MUD_BAN_H */
