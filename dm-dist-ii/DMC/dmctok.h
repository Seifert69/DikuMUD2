/* *********************************************************************** *
 * File   : dmctok.h                                  Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : ???                                                            *
 *                                                                         *
 * Purpose: Tokens used by dmc                                             *
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

#ifndef _MUD_DMCTOK_H
#define _MUD_DMCTOK_H

struct symbols
{
   const char *nam;
   int val;
} sym[] = {
	/* Unit fields */
	{"alignment", ALIGNMENT},
	{"names", NAMES},
	{"title", TITLE},
	{"descr", DESCR},
	{"outside_descr", OUT_DESCR},
	{"inside_descr", IN_DESCR},

	{"extra", EXTRA},
	{"minv", MINV},
	{"key", KEY},
	{"open", OPEN},
	{"manipulate", MANIPULATE},
	{"flags", UFLAGS},
	{"weight", WEIGHT},
	{"capacity", CAPACITY},
	{"special", SPECIAL},
		{"time", TIME},
		{"bits", BITS},
		{"string", STRINGT},
	{"light", LIGHT},
	{"bright", BRIGHT},
	{"affect", AFFECT},
	{"id", ID},
	{"duration", DURATION},
	{"data", DATA},
	{"firstf", FIRSTF},
	{"tickf", TICKF},
	{"lastf", LASTF},
	{"applyf", APPLYF},
	{"dilcopy", DILCOPY},
	{"end", END},

	/* room fields */
        {"link", LINK},
	{"exit", EXIT},
	{"movement", MOVEMENT},
	{"to", TO},
	{"keyword", KEYWORD},
	{"in", IN},

        /* Room & Object */
	{"spell", SPELL},

	/* Object fields */
	{"value", VALUE},
	{"cost", COST},
	{"rent", RENT},
	{"type", TYPE},

	/* mobile fields */
	{"defensive", DEFENSIVE},
        {"offensive", OFFENSIVE},
	{"mana", MANA},
	{"hit", HIT},
	{"money", MONEY},
	{"exp", EXP},
	{"sex", SEX},
	{"level", LEVEL},
	{"height", HEIGHT},
	{"race", RACE},
	{"attack", ATTACK},
	{"armour", ARMOUR},
	{"speed", SPEED},
	{"position", POSITION},
	{"weapon", WEAPON},
	{"ability", ABILITY},
	{"default", DEFAULT},
	{"npcflags", ACT},
	{"romflags", FLAGS},

	/* zone & reset fields & commands */
	{"weather", WEATHER},
	{"reset", RESET_F},
	{"lifespan", LIFESPAN},
	{"creators", CREATORS},
	{"notes", NOTES},
	{"help", HELP},
	{"load", LOAD},
	{"follow", FOLLOW},
	{"max", MAX},
	{"zonemax", ZONEMAX},
	{"local", LOCAL},
	{"into", INTO},
	{"nop", NOP},
	{"equip", EQUIP},
	{"door", DOOR},
	{"purge", PURGE},
	{"remove", REMOVE},
	{"random", RANDOM},
	{"complete", COMPLETE},
	{0,0}
};

#endif /* _MUD_DMCTOK_H */


