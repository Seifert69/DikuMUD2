/* *********************************************************************** *
 * File   : weather.h                                 Part of Valhalla MUD *
 * Version: 1.03                                                           *
 * Author : noop@diku.dk                                                   *
 *                                                                         *
 * Purpose:  Weather handling.                                             *
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


#ifndef _MUD_WEATHER
#define _MUD_WEATHER

extern int sunlight;


extern const time_t beginning_of_time;

char *timetodate(time_t t);
struct time_info_data real_time_passed(time_t t2, time_t t1);
struct time_info_data mud_date(time_t t);
struct time_info_data mud_time_passed(time_t t2, time_t t1);
struct time_info_data age(struct unit_data *ch);
void weather_change(struct time_info_data time_data);
void mudtime_strcpy(struct time_info_data *time, char *str);

void update_time_and_weather(void);
void boot_time_and_weather(void);

#endif
