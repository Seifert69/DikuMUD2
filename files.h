/* *********************************************************************** *
 * File   : files.h                                   Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : all.                                                           *
 *                                                                         *
 * Purpose: Various routines for administering files.                      *
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

#ifndef _MUD_FILES_H
#define _MUD_FILES_H

#include "bytestring.h"

#define FCACHE_MAX         100

void fstrcpy(CByteBuffer *pBuf, FILE *f);
char *fread_line_commented(FILE *fl, char *buf, int max);
long fsize(FILE *f);
void touch_file(char *name);
char *fread_string(FILE *fl);
char *fread_string_copy(FILE *fl, char *buf, int max);
int file_to_string(char *name, char *buf, int max_len);
int config_file_to_string(char *name, char *buf, int max_len);
long fread_num(FILE *fl);
FILE *fopen_cache(char *name, const char *mode);
void fclose_cache(void);
ubit1 file_exists(const char *name);

#endif
