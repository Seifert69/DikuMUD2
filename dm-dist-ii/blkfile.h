/* *********************************************************************** *
 * File   : blkfile.h                                 Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Blocked file system for saving mail and players (so far).      *
 *                                                                         *
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

#ifndef _MUD_BLKFILE_H
#define _MUD_BLKFILE_H

#include <stdio.h>
#include "essential.h"

#define BLK_NULL      -3  /* Use this constant to locally mark when a handle */
			  /* is free                                         */

typedef sbit16    blk_handle;
typedef sbit32    blk_length;

struct blk_file_type
{
   FILE *f;              /* Pointer to file, closed after each call   */
   char *name;           /* name of the file to contain blocks        */
   void *buf;            /* Buffer to store blocks read from *f       */
   blk_handle *list;     /* Memory list of free indexes               */
   blk_handle listtop;   /* Top of memory list                        */
   blk_handle listmax;   /* Number of alloced elements                */
   blk_handle blktop;    /* The maximum block number                  */
   blk_length bsize;     /* The size of each block in the file        */
};

typedef struct blk_file_type BLK_FILE;

void blk_delete(BLK_FILE *bf, blk_handle index);
void *blk_read(BLK_FILE *bf, blk_handle index, blk_length *blen);
void *blk_read_reserved(BLK_FILE *bf, blk_length *blen);
blk_handle blk_write(BLK_FILE *bf, const void *data, blk_length len);
void blk_write_reserved(BLK_FILE *bf, const void *data, blk_length len);
BLK_FILE *blk_open(const char *name, blk_length block_size);
void blk_close(BLK_FILE *bf);

#endif /* _MUD_BLKFILE_H */
