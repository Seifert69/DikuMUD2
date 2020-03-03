/* *********************************************************************** *
 * File   : ttydef.h                                  Part of Valhalla MUD *
 * Version: 1.01                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Terminal definitions for ANSI, vt100, etc.                     *
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

#ifndef _MUD_TTYDEF_H
#define _MUD_TTYDEF_H


#define BACKSPACE1_CHAR		'\b'    /* The two standard BACKSPACE chars */
#define BACKSPACE2_CHAR		'\177'

#define ANSI_DELETE		"^[\177"  /* Literal! */
#define ANSI_BACKSPACE		"^[[D"  /* Literal! */

#define ANSI_CURSOR_UP		"[1A"
#define ANSI_CURSOR_DOWN	"[1B"
#define ANSI_CURSOR_FORWARD	"[1C"
#define ANSI_CURSOR_BACK	"[1D"
#define ANSI_ERASE_LINE		"[K"


#define ANSI_RESET		"[0m"
#define ANSI_BOLD		"[1m"
#define ANSI_REVERSE		"[7m"
#define ANSI_CONCEALED		"[8m"
#define ANSI_HOME		"[2J"

#define ANSI_FG_BLACK		"[0;30m"
#define ANSI_FG_RED		"[0;31m"
#define ANSI_FG_GREEN		"[0;32m"
#define ANSI_FG_YELLOW		"[0;33m"
#define ANSI_FG_BLUE		"[0;34m"
#define ANSI_FG_MAGENTA		"[0;35m"
#define ANSI_FG_CYAN		"[0;36m"
#define ANSI_FG_WHITE		"[0;37m"

#define ANSI_FGB_BLACK		"[1;30m"
#define ANSI_FGB_RED		"[1;31m"
#define ANSI_FGB_GREEN		"[1;32m"
#define ANSI_FGB_YELLOW		"[1;33m"
#define ANSI_FGB_BLUE		"[1;34m"
#define ANSI_FGB_MAGENTA	"[1;35m"
#define ANSI_FGB_CYAN		"[1;36m"
#define ANSI_FGB_WHITE		"[1;37m"

#define ANSI_BG_BLACK		"[40m"
#define ANSI_BG_RED		"[41m"
#define ANSI_BG_GREEN		"[42m"
#define ANSI_BG_YELLOW		"[43m"
#define ANSI_BG_BLUE		"[44m"
#define ANSI_BG_MAGENTA		"[45m"
#define ANSI_BG_CYAN		"[46m"
#define ANSI_BG_WHITE		"[47m"

#endif
