/* *********************************************************************** *
 * File   : db_file.h                                 Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Prototypes for reading/writing from/to buffers.                *
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

#ifndef _MUD_DB_FILE_H
#define _MUD_DB_FILE_H

#include "structs.h"
#include "bytestring.h"
#include "dil.h"

extern CByteBuffer g_FileBuffer;   /* Defined in db_file.c */
extern int g_nCorrupt;             /*          "           */

void *bread_dil(CByteBuffer *pBuf, struct unit_data *, ubit8 version,
		struct unit_fptr *fptr);

struct diltemplate *bread_diltemplate(CByteBuffer *pBuf);
int bread_extra(CByteBuffer *pBuf, class extra_descr_data **ppExtra);

int bread_swap(CByteBuffer *pBuf, struct unit_data *u);
int bread_swap_skip(CByteBuffer *pBuf);
int    bread_affect(CByteBuffer *pBuf, struct unit_data *u, ubit8 nVersion);

struct unit_fptr *bread_func(CByteBuffer *pBuf, ubit8 version,
			     struct unit_data *owner);

void bread_block(FILE *datafile, long file_pos, int length, void *buffer);


void bwrite_extra_descr(CByteBuffer *pBuf, struct extra_descr_data *e);
void bwrite_swap(CByteBuffer *pBuf, struct unit_data *u);
void bwrite_affect(CByteBuffer *pBuf, struct unit_affected_type *af, ubit8 version);
void bwrite_func(CByteBuffer *pBuf, struct unit_fptr *fptr);
void bwrite_block(FILE *f, int length, void *buffer);
void bwrite_dil(CByteBuffer *pBuf, struct dilprg *prg);
void bwrite_diltemplate(CByteBuffer *pBuf, struct diltemplate *tmpl);

void write_unit(FILE *f, struct unit_data *u, char *fname);
void write_diltemplate(FILE *f, struct diltemplate *tmpl);
int write_unit_string(CByteBuffer *pBuf, struct unit_data *u);

#endif /* _MUD_DB_FILE_H */
