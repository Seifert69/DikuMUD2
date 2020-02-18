/* *********************************************************************** *
 * File   : sysport.h                                 Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : elascurn@daimi.aau.dk & gnort@daimi.aau.dk                     *
 *                                                                         *
 * Purpose: Macros and prototypes for support of non-unix machines         *
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

#ifndef _MUD_SYSPORT_H
#define _MUD_SYSPORT_H

#ifdef AMIGA1
void CloseTimer(void);
int OpenTimer(void);
int WaitForPulse(void);

/* Create this file to get play-window */
#define AMIGA_CONNECT_FILE	"RAM:PlayCon"

/* The play-window's definition */
#define AMIGA_PLAY_WINDOW	"CON:0/14/664/197/DikuMUD Window/CLOSE"

#endif /* AMIGA */

#endif /* _MUD_SYSPORT_H */
