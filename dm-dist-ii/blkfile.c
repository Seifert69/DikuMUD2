/* *********************************************************************** *
 * File   : blkfile.c                                 Part of Valhalla MUD *
 * Version: 2.20                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Handling of single blocked files.                              *
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

/* 16/07/92 seifert: Now allows 0 byte long saves, may have been a problem */
/* 12/06/93 seifert: Removed buggy fflush() (to avoid Linux bug)           */
/* 27/08/93 seifert: Extensive testing of blocked file system              */
/* 07/09/93 seifert: Added much integrity checking. Now checks for legal   */
/*                   handles - but incompatible with old system.           */
/* 16/07/94 seifert: The free list uses much less memory now.              */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "structs.h"
#include "utils.h"
#include "blkfile.h"
#include "db.h"
#include "db_file.h"
#include "utility.h"
#include "textutil.h"
#include "files.h"
#include "unixshit.h"

#define BLK_RESERVED   0   /* Reserved block for special exceptions */
#define BLK_FREE      -1
#define BLK_END       -2
			   /* BLK_NULL == -3 in blkfile.h */
#define BLK_START_V1  -4   /* This is the start of a new block, used */
			   /* for integrity checking and versioning. */

#define BLK_MIN_SIZE (blk_length) (2*(2*sizeof(blk_handle)+sizeof(blk_length)))

/*
   Format of blocked file:

   <blk_handle next_block>
   if start of data then <blk_length length>
   <data>                data[]

*/

/*
   I could use the first free block as start of a linked list
   of free blocks. But this would require quite a lot of reading
   and writing of blocks when writing blocks (in order to find
   the next free block). But it would save memory (just a little),
   and it wouldn't be nessecary to create the list when the
   blocked file is opened
*/

static blk_handle blk_fsize(BLK_FILE *bf)
{
   return fsize(bf->f) / bf->bsize;
}


static void blk_read_block(BLK_FILE *bf, blk_handle index)
{
   long p;

   p = index * bf->bsize;

   if (fseek(bf->f, p, SEEK_SET))
     error(HERE, "%s: Unable to seek.", bf->name);

   if (fread(bf->buf, bf->bsize, 1, bf->f) != 1)
     error(HERE, "%s: Unable to read.", bf->name);
}



static void blk_write_block(BLK_FILE *bf, blk_handle index)
{
   if (fseek(bf->f, index*bf->bsize, SEEK_SET))
     error(HERE, "%s: Unable to seek.", bf->name);


   if (fwrite(bf->buf, bf->bsize, 1, bf->f) != 1)
     error(HERE, "%s: Unable to write.", bf->name);
}

static void register_free_block(BLK_FILE *bf, blk_handle idx)
{
   int i;

   bf->listtop++;

   if (bf->listtop > bf->listmax)
   {
      if (bf->listtop <= 1)
	CREATE(bf->list, blk_handle, bf->listtop);
      else
	RECREATE(bf->list, blk_handle, bf->listtop);
      bf->listmax = bf->listtop;
   }

   bf->list[bf->listtop-1] = idx;

   /* Insert the block in descending order so we get a nice and  */
   /* straight FORWARD writing of blocks (may help w/ readahead) */
   for (i=0; i < bf->listtop-1; i++)
     if (bf->list[i] < idx)
     {
	memmove(&bf->list[i+1], &bf->list[i],
		(bf->listtop-i-1)*sizeof(blk_handle));
	bf->list[i] = idx;
	break;
     }
}

/* Returns a free block and occupies it at the same time. */
static blk_handle find_occupy_free_blk(BLK_FILE *bf)
{
   ubit8 *b;

   if (bf->listtop > 0)  /* We got a free space in current list */
   {
      bf->listtop--;

      if (bf->list[bf->listtop] == BLK_RESERVED)
	error(HERE, "%s: Reserved block mess.", bf->name);

      return bf->list[bf->listtop];
   }

   if (bf->blktop == BLK_RESERVED)
   {
      b = (ubit8 *) bf->buf;
      bwrite_ubit16(&b, (ubit16) BLK_FREE);
      blk_write_block(bf, BLK_RESERVED);

      bf->blktop++;   /* Skip the reserved block */
   }

   return bf->blktop++;
}


/* Copy data starting at blk_start into data, and return pointer */
/* to immediate next free space in data.                         */
static void *blk_extract_data(BLK_FILE *bf, void *blk_start, void *data,
			      blk_length *len)
{
   int no, used;

   if (*len == 0)
     return 0;

   used = (ubit8 *) blk_start - (ubit8 *) bf->buf;
   no   = bf->bsize - used;

   no = MIN(*len, no);               /* Make sure we don't copy too much */

   if (no < 0)
     error(HERE, "%s: Illegal length.", bf->name);

   if (no == 0)
     return data;

   memcpy(data, blk_start, no);

   *len -= no;                       /* We've not got len-no left to copy */

   return (ubit8 *) data + no;
}



/* Copy from data into blk_start the apropriate number of bytes */
/* Returns pointer just past last copied element from data      */
static const void *blk_put_data(BLK_FILE *bf, void *blk_start,
				const void *data)
{
   int no, used;

   used = (ubit8 *) blk_start - (ubit8 *) bf->buf;
   no   = bf->bsize - used;

   if (no < 0)
     error(HERE, "%s: Illegal length.", bf->name);

   if (no == 0)
     return data;

   memcpy(blk_start, data, no);   /* Copy data */

   return (ubit8 *) data + no;
}



/* Delete message starting at index "index" */
void blk_delete(BLK_FILE *bf, blk_handle index)
{
   blk_handle next_block;
   int first;
   ubit8 *b;

   if (!(bf->f=fopen_cache(bf->name, "r+b")))
     error(HERE, "%s: Cache open.", bf->name);

   first = TRUE;

   do
   {
      blk_read_block(bf, index);      /* Read the block to find next block */

      b = (ubit8 *) bf->buf;

      next_block = (blk_handle) bread_ubit16(&b);

      if (next_block == BLK_FREE)
      {
	 slog(LOG_ALL, 0, "BLK_DELETE: Attempt to access free disk "
	      "or mem block!");
	 return;
      }

      if (first)
      {
	 if (next_block != BLK_START_V1)
	 {
	    slog(LOG_ALL, 0, "BLK_DELETE: Illegal delete handle "
		 "in '%s'", bf->name);
	    return;
	 }
	 next_block = (blk_handle) bread_ubit16(&b);
	 first = FALSE;
      }

      b = (ubit8 *) bf->buf;
      bwrite_ubit16(&b, (ubit16) BLK_FREE);

      blk_write_block(bf, index);     /* Save on disk                      */

      register_free_block(bf, index);

      index = next_block;
   }
   while (index != BLK_END);

   if (fflush(bf->f))
     error(HERE, "%s: Flush", bf->name);
}



/* Read message starting at index "index", and return pointer to the */
/* result buffer, if given pointer to a blk_length type, save the    */
/* length of the buffer in this variable                             */
/* Returns NULL if error                                             */
void *blk_read(BLK_FILE *bf, blk_handle index, blk_length *blen)
{
   void *blk_ptr, *data = 0, *odata = 0;
   blk_handle next_block, oindex;
   blk_length len = 0;

   if (!(bf->f=fopen_cache(bf->name, "r+b")))
     error(HERE, "%s: Cache open.", bf->name);

   oindex = index;
   do
   {
      blk_read_block(bf, index);     /* Read the block into bf->buf */

      blk_ptr = bf->buf;
      next_block = bread_ubit16((ubit8 **) &blk_ptr);

      if (next_block == BLK_FREE)
      {
	 slog(LOG_ALL, 0, "BLK_READ: Attempt to access free "
	      "block in '%s'", bf->name);
	 if (blen)
	   *blen = 0;
	 return NULL;
      }

      if (oindex == index)
      {
	 if (next_block != BLK_START_V1)
	 {
	    slog(LOG_ALL, 0, "BLK_READ: Illegal read handle "
		 "in '%s'.", bf->name);
	    if (blen)
	      *blen = 0;
	    return NULL;
	 }

	 /* This was the first block, now read the real next_block */
	 next_block = bread_ubit16((ubit8 **) &blk_ptr);

	 len = bread_ubit32((ubit8 **) &blk_ptr);
	 if (len > 0)
	   CREATE(data, ubit8, len);  /* Alloc space for the buffer */
	 else
	   data = 0;
	 odata = data;
	 if (blen)
	   *blen = len;
      }

      data = blk_extract_data(bf, blk_ptr, data, &len);

      index = next_block;
   }
   while (index != BLK_END);

   return odata;
}



/* Save the letter pointed to by str in the blk file */
/* and return a handle to the message                */
blk_handle blk_write(BLK_FILE *bf, const void *data, blk_length len)
{
   blk_handle index, next_block, first_block;
   const void *org;
   void *blk_ptr;

   if (!(bf->f=fopen_cache(bf->name, "r+b")))
     error(HERE, "%s: Cache open.", bf->name);

   if (len == 0)
     slog(LOG_ALL, 0, "Zero length save to file %s", bf->name);

   first_block = index = find_occupy_free_blk(bf);

   org = data;

   do
   {
      blk_ptr = bf->buf;

      if (first_block == index)
	bread_ubit16((ubit8 **) &blk_ptr);

      bread_ubit16((ubit8 **) &blk_ptr);

      if (first_block == index)
	 bread_ubit32((ubit8 **) &blk_ptr);

      data = blk_put_data(bf, blk_ptr, data);

      if((ubit8 *) data - (ubit8 *) org < len)
	 next_block = find_occupy_free_blk(bf);
      else
	 next_block = BLK_END;

      blk_ptr = bf->buf;

      if (first_block == index)
	bwrite_ubit16((ubit8 **) &blk_ptr, (ubit16) BLK_START_V1);

      bwrite_ubit16((ubit8 **) &blk_ptr, (ubit16) next_block);

      if (first_block == index)
	bwrite_ubit32((ubit8 **) &blk_ptr, (ubit32) len);

      blk_write_block(bf, index);
      index = next_block;
   }
   while (index != BLK_END);

   if (fflush(bf->f))
     error(HERE, "%s: Flush", bf->name);

   return first_block;
}


void blk_write_reserved(BLK_FILE *bf, const void *data, blk_length len)
{
   blk_handle i;
   ubit8 *b;

   blk_read_block(bf, BLK_RESERVED);
   b = (ubit8 *) bf->buf;
   i = bread_ubit16(&b);

   if (i != BLK_FREE)
     blk_delete(bf, i);

   i = blk_write(bf, data, len);

   b = (ubit8 *) bf->buf;
   bwrite_ubit16(&b, i);
   blk_write_block(bf, BLK_RESERVED);
}


void *blk_read_reserved(BLK_FILE *bf, blk_length *blen)
{
   blk_handle i;
   ubit8 *b;

   if (bf->blktop == 0)
   {
      if (blen)
	*blen = 0;
      return NULL;
   }

   blk_read_block(bf, BLK_RESERVED);
   b = (ubit8 *) bf->buf;
   i = bread_ubit16(&b);

   if (i == BLK_FREE)
   {
      if (blen)
	*blen = 0;
      return NULL;
   }

   return blk_read(bf, i, blen);
}


/* block size is the number of bytes contained in each block. Try 128+ */
BLK_FILE *blk_open(const char *name, blk_length block_size)
{
   blk_handle index;

   BLK_FILE *bf;

   if (block_size < BLK_MIN_SIZE)
     error(HERE, "%s: Illegal block size.", name);

   CREATE(bf, BLK_FILE, 1);
   bf->name      = str_dup(name);
   bf->listtop   = 0;
   bf->listmax   = 0;
   bf->blktop    = 0;
   bf->list      = 0;
   bf->bsize     = block_size;
   bf->f         = 0;
   CREATE(bf->buf, ubit8, bf->bsize);

   if (!(bf->f=fopen_cache(bf->name, "r+b")))
     error(HERE, "%s: Cache open.", bf->name);

   bf->blktop = blk_fsize(bf);

   for (index = 0; ; index++)
   {
      if (fread(bf->buf, bf->bsize, 1, bf->f) != 1)
	break;

      if (index >= bf->blktop)
      {
	 bf->blktop = index + 1;
	 slog(LOG_ALL,0,"Apparently blk_fsize doesn't work!");
      }

      if ((*((blk_handle *) bf->buf) == BLK_FREE) && (index != BLK_RESERVED))
	register_free_block(bf, index);
   }

   return bf;
}

/* I only put this routine here since it makes the whole thing seem more */
/* complete. It will probably never be called, since there is no need to */
/* close a blocked file (it uses no resources other than a little mem).  */
void blk_close(BLK_FILE *bf)
{
   free(bf->name);
   free(bf->buf);
   if (bf->list)
      free(bf->list);
   free(bf);
}
