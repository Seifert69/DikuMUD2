/* *********************************************************************** *
 * File   : arg.h                                     Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : Unknown.                                                       *
 *                                                                         *
 * Purpose: Unknown.                                                       *
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

#ifndef _MUD_ARG_H
#define _MUD_ARG_H

#define DEF_CONNECTIONS 4
#define DEF_SERVER_PORT 4242
#define DEF_SERVER_ADDR "127.0.0.1"

struct arg_type
{
   int  nConnections;
   int  nPort;
   int  bModem;
   int  nBaud;

   char *pcAddress;

   char *pExt;
   char *pHeader;
   char *pDevice;
   char *pHomeDir;
   char *pFtpDir;

   char **ppInit[4];

   char *pLogName;
   char *pActivityName;
};

int ParseArg(int argc, char *argv[]);

extern struct arg_type arg;

#endif
