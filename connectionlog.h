/* *********************************************************************** *
 * File   : connectionlog.h                           Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : all.                                                           *
 *                                                                         *
 * Purpose: Headers.                                                       *
 *                                                                         *
 * Bugs   : Unknown.                                                       *
 * Status : Published.                                                     *
 *                                                                         *
 * Copyright (C) 1994 - 1996 by Valhalla (This work is published).         *
 *                                                                         *
 * This work is a property of:                                             *
 *                                                                         *
 *        Valhalla I/S                                                     *
 *        Noerre Soegade 37A, 4th floor                                    *
 *        1370 Copenhagen K.                                               *
 *        Denmark                                                          *
 *                                                                         *
 * This work is copyrighted. No part of this work may be copied,           *
 * reproduced, translated or reduced to any medium without the prior       *
 * written consent of Valhalla.                                            *
 * *********************************************************************** */

#ifndef _MUD_CONNECTIONLOG_H
#define _MUD_CONNECTIONLOG_H

#include "structs.h"

void ConnectionBoot( void );
void ConnectionBegin(class unit_data *pc);
void ConnectionLog(class unit_data *pc);

#endif // _MUD_CONNECTIONLOG_H
