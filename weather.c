/* *********************************************************************** *
 * File   : weather.c                                 Part of Valhalla MUD *
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

/* 23/08/93 jubal  : Fixed mudtime_strcpy                                  */
/* 10/08/94 gnort  : Fixed some ANSI time_t violations                     */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "handler.h"
#include "interpreter.h"
#include "db.h"
#include "utility.h"
#include "main.h"
#include "weather.h"


int sunlight = SUN_SET;   /* And how much sun. */
const time_t beginning_of_time = 650336715;   /* Sat Aug 11 01:05:15 1990 */

/* What a coincidence!
 * I actually (REALLY!) converted the time_t on:  11-Aug-94 12:38:58.
 *								/gnort
 */


char *timetodate(time_t t)
{
   static char d[20];
   strftime(d, 20, "%a %b %d", localtime(&t));
   return d;
}

/* Calculate the REAL time passed over the last t2-t1 centuries (secs) */
struct time_info_data real_time_passed(time_t t2, time_t t1)
{
   long secs;
   struct time_info_data now;

   secs = (long) difftime(t2, t1);

   now.hours = (secs/SECS_PER_REAL_HOUR) % 24; /* 0..23 hours */
   secs -= SECS_PER_REAL_HOUR*now.hours;

   now.day = (secs/SECS_PER_REAL_DAY); /* 0.. days  */
   secs -= SECS_PER_REAL_DAY*now.day;

   now.month = -1;
   now.year  = -1;

   return now;
}



/* Calculate the MUD date */
struct time_info_data mud_date(time_t t)
{
   struct time_info_data mdate;
   long p;
   extern const time_t beginning_of_time;

   p = (long) difftime(t, beginning_of_time);

   mdate.hours = (p/SECS_PER_MUD_HOUR) % 24; /* 0..23 hours */
   p -= SECS_PER_MUD_HOUR*mdate.hours;

   mdate.day = (p/SECS_PER_MUD_DAY) % MUD_MONTH; /* 0..20 days  */
   p -= SECS_PER_MUD_DAY*mdate.day;

   mdate.month = (p/SECS_PER_MUD_MONTH) % MUD_YEAR; /* 0..8 months */
   p -= SECS_PER_MUD_MONTH*mdate.month;

   mdate.year = (p/SECS_PER_MUD_YEAR); /* [0..[ years */

   return mdate;
}



/* Calculate the MUD time passed over the last t2-t1 centuries (secs) */
struct time_info_data mud_time_passed(time_t t2, time_t t1)
{
   long secs;
   struct time_info_data now;

   secs = (long) difftime(t2, t1);

   now.hours = (secs/SECS_PER_MUD_HOUR) % 24; /* 0..23 hours */
   secs -= SECS_PER_MUD_HOUR*now.hours;

   now.day = (secs/SECS_PER_MUD_DAY) % MUD_MONTH; /* 0..20 days  */
   secs -= SECS_PER_MUD_DAY*now.day;

   now.month = (secs/SECS_PER_MUD_MONTH) % MUD_YEAR; /* 0..8 months */
   secs -= SECS_PER_MUD_MONTH*now.month;

   now.year = (secs/SECS_PER_MUD_YEAR);	/* 0..XX? years */

   return now;
}


struct time_info_data age(struct unit_data *ch)
{
   static struct time_info_data player_age;

   if (IS_PC(ch))
     player_age = mud_time_passed(time(0),PC_TIME(ch).birth);
   else
     player_age = mud_time_passed(0, 0);

   return player_age;
}



/* Here comes the code */
static void another_hour(struct time_info_data time_data)
{
   switch (time_data.hours)
   {
     case 5:
      sunlight = SUN_RISE;
      send_to_outdoor("The sun rises in the east.\n\r");
      break;
      
     case 6:
      sunlight = SUN_LIGHT;
      send_to_outdoor("The day has begun.\n\r");
      break;
      
     case 21:
      sunlight = SUN_SET;
      send_to_outdoor("The sun slowly disappears in the west.\n\r");
      break;

     case 22:
      sunlight = SUN_DARK;
      send_to_outdoor("The night has begun.\n\r");
      break;
     
     default:
      break;
   }
}



static void weather_change(struct zone_type *zone,
			   struct time_info_data time_data)
{
   int diff, change;

   if (time_data.month <= 2)  /* Winter */
     diff = (zone->weather.pressure <= (zone->weather.base-25) ? 2 : -2);
   else if (time_data.month <= 4)     /* Spring */
     diff=(zone->weather.pressure <= (zone->weather.base +  5) ? 2 : -2);
   else if (time_data.month <= 6)     /* Summer */
     diff=(zone->weather.pressure <= (zone->weather.base + 20) ? 2 : -2);
   else  /* Fall   */
     diff=(zone->weather.pressure <= (zone->weather.base -  5) ? 2 : -2);

   zone->weather.change += (dice(1,4)*diff+dice(2,6)-dice(2,6));

   zone->weather.change = MIN(zone->weather.change, 12);
   zone->weather.change = MAX(zone->weather.change, -12);

   zone->weather.pressure += zone->weather.change;

   zone->weather.pressure = MIN(zone->weather.pressure,1050);
   zone->weather.pressure = MAX(zone->weather.pressure,950);

   change = 0;

   switch(zone->weather.sky)
   {
     case SKY_CLOUDLESS :
     {
	if (zone->weather.pressure < 990)
	  change = 1;
	else if (zone->weather.pressure < 1010)
	  if (dice(1,4)==1)
	    change = 1;
	break;
     }
    case SKY_CLOUDY :
    {
       if (zone->weather.pressure < 970)
	 change = 2;
       else if (zone->weather.pressure < 990)
	 if(dice(1,4)==1)
	   change = 2;
	 else
	   change = 0;
       else if (zone->weather.pressure > 1030)
	 if(dice(1,4)==1)
	   change = 3;

       break;
    }
    case SKY_RAINING :
    {
       if (zone->weather.pressure < 970)
	 if(dice(1,4)==1)
	   change = 4;
	 else
	   change = 0;
       else if (zone->weather.pressure > 1030)
	 change = 5;
       else if (zone->weather.pressure > 1010)
	 if(dice(1,4)==1)
	   change = 5;

       break;
    }
    case SKY_LIGHTNING :
    {
       if (zone->weather.pressure > 1010)
	 change = 6;
       else if (zone->weather.pressure > 990)
	 if(dice(1,4)==1)
	   change = 6;

       break;
    }
    default :
    {
       change = 0;
       zone->weather.sky=SKY_CLOUDLESS;
       break;
    }
  }

   switch(change)
   {
     case 0 :
       break;

     case 1 :
       send_to_zone_outdoor(zone, "The sky is getting cloudy.\n\r");
       zone->weather.sky=SKY_CLOUDY;
       break;

     case 2 :
       send_to_zone_outdoor(zone, "It starts to rain.\n\r");
       zone->weather.sky=SKY_RAINING;
       break;

     case 3 :
       send_to_zone_outdoor(zone, "The clouds disappear.\n\r");
       zone->weather.sky=SKY_CLOUDLESS;
       break;

     case 4 :
       send_to_zone_outdoor(zone, "Lightning starts to show in the sky.\n\r");
       zone->weather.sky=SKY_LIGHTNING;
       break;

     case 5 :
       send_to_zone_outdoor(zone, "The rain stopped.\n\r");
       zone->weather.sky=SKY_CLOUDY;
       break;

     case 6 :
       send_to_zone_outdoor(zone, "The lightning has stopped.\n\r");
       zone->weather.sky=SKY_RAINING;
       break;

     default :
       break;
   }
}


void update_time_and_weather(void)
{
   struct time_info_data time_info;
   struct zone_type *z;

   time_info = mud_date(time(0));

   another_hour(time_info);

   for (z = zone_info.zone_list; z; z = z->next)
     weather_change(z, time_info);
}



/* Convert 'time' into text, and copy it into str */
void mudtime_strcpy(struct time_info_data *time, char *str)
{
   char *b;
   const char *suf;
   char buf[500], tmp[500];
   int weekday, day;

   extern const char *weekdays[];
   extern const char *month_name[];

   b = buf;
   sprintf(b, "%d o'clock %s, on ",
	   ((time->hours % 12 == 0) ? 12 : ((time->hours) % 12)),
	   (((time->hours > 12) || (time->hours == 0)) ? "after Noon" :
	    "past Midnight") );
   TAIL(b);

   weekday = ((MUD_MONTH*time->month)+time->day+1) % MUD_WEEK;
   /* 21 days in a month */

   strcat(b, weekdays[weekday]);
   TAIL(b);

   day = time->day + 1;
   /* day in [1..21] */

   if (day == 1)
     suf = "st";
   else if (day == 2)
     suf = "nd";
   else if (day == 3)
     suf = "rd";
   else if (day < 20)
     suf = "th";
   else if ((day % 10) == 1)
     suf = "st";
   else if ((day % 10) == 2)
     suf = "nd";
   else if ((day % 10) == 3)
     suf = "rd";
   else
     suf = "th";

   sprintf(tmp, ",\n\r   the %d%s Day of the %s, Year %d.\n\r",
	   day,
	   suf,
	   month_name[(int)time->month],
	   time->year);
   strcat(b, tmp);

   strcpy(str, buf);
}

static void weather_and_time_event(void *p1, void *p2)
{
   update_time_and_weather();
   event_enq(SECS_PER_MUD_HOUR * 4, weather_and_time_event, 0, 0);
}


/* reset the time in the game from file */
void boot_time_and_weather(void)
{
   struct zone_type *z;
   struct time_info_data mud_time_passed(time_t t2, time_t t1);

   extern char world_boottime[];

   struct time_info_data time_info;
   time_t now = time(0);
   char *p = ctime(&now);
   p[strlen(p) - 1] = '\0';

   sprintf(world_boottime, "%s", p);

   time_info = mud_time_passed(time(0), beginning_of_time);

   if (time_info.hours == 5)
     sunlight = SUN_RISE;
   else if (6 <= time_info.hours && time_info.hours <= 20)
     sunlight = SUN_LIGHT;
   else if (time_info.hours == 21)
     sunlight = SUN_SET;
   else
     sunlight = SUN_DARK;

   slog(LOG_OFF, 0, "   Current Gametime: %dH %dD %dM %dY.",
	time_info.hours, time_info.day, time_info.month, time_info.year);

   for (z = zone_info.zone_list; z; z = z->next)
   {
      z->weather.pressure = z->weather.base;

      if (time_info.month >= 7 && time_info.month <= 12)
	z->weather.pressure += number(-6, 6);
      else
	z->weather.pressure += number(-10, 10);
      
      z->weather.change = number(-2, 2);

      if (z->weather.pressure <= 980)
	z->weather.sky = SKY_LIGHTNING;
      else if (z->weather.pressure <= 1000)
	z->weather.sky = SKY_RAINING;
      else if (z->weather.pressure <= 1020)
	z->weather.sky = SKY_CLOUDY;
      else
	z->weather.sky = SKY_CLOUDLESS;
   }

   event_enq(PULSE_SEC * SECS_PER_MUD_HOUR, weather_and_time_event, 0, 0);
}
