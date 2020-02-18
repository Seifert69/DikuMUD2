/* *********************************************************************** *
 * File   : main.h                                    Part of Valhalla MUD *
 * Version: 1.05                                                           *
 * Author : seifert@diku.dk and quinn@freja.diku.dk                        *
 *                                                                         *
 * Purpose: Header for the basis stuff.                                    *
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

#ifndef _MUD_MAIN_H
#define _MUD_MAIN_H

void event_enq(int when, void (*func)(void *, void *), void *arg1, void *arg2);
void event_deenq(void (*func)(void *, void *), void *arg1, void *arg2);
void event_deenq_relaxed(void (*func)(void *, void *), void *arg1, void *arg2);

extern struct descriptor_data *descriptor_list;
extern struct descriptor_data *next_to_process;

extern int mud_bootzone;
extern int no_connections;
extern int max_no_connections;
extern int no_players;
extern int max_no_players;
extern int player_convert;
extern int slow_death;
extern int mud_shutdown;
extern int mud_reboot;
extern int wizlock;
extern int tics;

extern int world_norooms;
extern int world_noobjects;
extern int world_nochars;
extern int world_nozones;
extern char world_boottime[64];

extern char zondir[64];
extern char plydir[64];

extern const char *compile_date;
extern const char *compile_time;

#endif
