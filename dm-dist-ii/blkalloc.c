/* *********************************************************************** *
 * File   : blkalloc.c                                Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : bombman                                                        *
 *                                                                         *
 * Purpose: Block allocating 'static' data, SHOULD save 20%                *
 *          However, such data cannot be free'd                            *
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

#define BLOCKSIZE = 256000  /* nothing bigger than this please */

void *blkalloc_start=0;
void *blkalloc_end=0;

void *blkalloc(size_t size)
{
   void *res=0;

   if (size>=BLOCKSIZE)
     return(malloc(size));

   if (blkalloc_start+size>=blkalloc_end)
   {
      blkalloc_start=malloc(BLOCKSIZE);
      blkalloc_end=blkalloc_start+BLOCKSIZE;
   }

   res=blkalloc_start;
   blkalloc_start+=size;
   return res;
}
