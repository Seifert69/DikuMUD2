/* *********************************************************************** *
 * File   : postoffice.h                              Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Types for the postman.                                         *
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

#ifndef _MUD_POSTOFFICE_H
#define _MUD_POSTOFFICE_H

struct mail_file_info_type
{
   time_t     date;        /* Date of posting the letter      */
   blk_handle handle;
   int        sender;      /* PC ID */
   int        receipient;  /* PC ID */
   ubit8      loaded;      /* Boolean, has player got a note? */
};

#endif /* _MUD_POSTOFFICE_H */



