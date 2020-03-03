/* *********************************************************************** *
 * File   : comm.h                                    Part of Valhalla MUD *
 * Version: 1.03                                                           *
 * Author : all.                                                           *
 *                                                                         *
 * Purpose: Headers.                                                       *
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

#ifndef _MUD_COMM_H
#define _MUD_COMM_H

void page_string(struct descriptor_data *d, const char *);

void send_to_outdoor(const char *messg);
void send_to_zone_outdoor(const struct zone_type *z,
			  const char *messg);
void send_to_descriptor(const char *messg, struct descriptor_data *d);
void send_to_all(const char *messg);
void send_to_char(const char *messg, const struct unit_data *ch);
void send_to_room(const char *messg, struct unit_data *room);

/*  Please note that act() does NOT accept TRUE or FALSE as second argument
 *  anymore...
 */
void act(const char *str, int hide_invisible, const void *arg1,
	 const void *arg2, const void *arg3, int type);
void act_generate(char *buf, const char *str, int show_type,
		  const void *arg1, const void *arg2, const void *arg3,
		  int type, struct unit_data *to);
     
int process_output(struct descriptor_data *t);
int process_input(struct descriptor_data *t);

#define PROMPT_SENT   0
#define PROMPT_EXPECT 1
#define PROMPT_IGNORE 2

#endif /* _MUD_COMM_H */
