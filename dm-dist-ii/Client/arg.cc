/* *********************************************************************** *
 * File   : arg.cc                                    Part of Valhalla MUD *
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

#include <stdlib.h>
#include <string.h>

#include "essential.h"
#include "arg.h"
#include "log.h"
#include "str_parse.h"

struct arg_type arg;

/* Read contents of a file, but skip all remark lines. */
int file_to_string(char *name, char *buf, int max_len)
{
   FILE *fl;
   char tmp[100];

   *buf = '\0';

   if (!(fl = fopen(name, "r")))
   {
      DEBUG("File-to-string error.\n");
      *buf = '\0';
      return(-1);
   }

   do
   {
      if (fgets(tmp, sizeof(tmp) - 1, fl))
      {
	 if (tmp[0] == '#')
	   continue;

	 if (tmp[0] == 0)
	   continue;

	 if ((int) (strlen(buf) + strlen(tmp) + 2) > max_len)
	 {
	    DEBUG("fl->strng: string too big (db.c, file_to_string)\n");
	    strcpy(buf + strlen(buf) - 20, "TRUNCATED!");
	    fclose(fl);
	    return(-1);
	 }

	 strcat(buf, tmp);
	 *(buf + strlen(buf) + 1) = '\0';
	 *(buf + strlen(buf)) = '\r';
      }
   }
   while (!feof(fl));

   fclose(fl);

   return(0);
}

// The name could for example be, "client.S16"
//
void ConfigFile(char *name)
{
   char Buf[4096];
   char *c, *b = Buf;
   char **list;

   arg.bModem = TRUE;
   arg.nBaud = 38400;

   if (file_to_string(name, Buf, sizeof(Buf)) != 0)
   {
      fprintf(stderr, "Unable to open '%s'.\n", name);
      return;
   }

   if ((c = parse_match_name(&b, "header")))
   {
      arg.pHeader = str_dup(str_cc("\n\r\n\r", c));
      free(c);
   }

   if ((c = parse_match_name(&b, "device")))
     arg.pDevice = c;

   parse_match_num(&b, "usemodem", &arg.bModem);
   parse_match_num(&b, "baud", &arg.nBaud);

   parse_match_num(&b, "port", &arg.nPort);

   memset(arg.ppInit, 0, sizeof(arg.ppInit));

   if ((list = parse_match_namelist(&b, "init1")))
     arg.ppInit[0] = list;

   if ((list = parse_match_namelist(&b, "init2")))
     arg.ppInit[1] = list;

   if ((list = parse_match_namelist(&b, "init3")))
     arg.ppInit[2] = list;

   if ((c = parse_match_name(&b, "homedir")))
     arg.pHomeDir = c;

   if ((c = parse_match_name(&b, "ftpdir")))
     arg.pFtpDir = c;
}


int ParseArg(int argc, char *argv[])
{
   int i, n;

   arg.nConnections = DEF_CONNECTIONS;
   arg.nPort        = DEF_SERVER_PORT;
   arg.pcAddress    = DEF_SERVER_ADDR;

   arg.pExt         = "S16";
   arg.pHeader      = "Connected to a Supra Modem.\n\r";
   arg.pDevice      = "/dev/ttyS16";

   arg.ppInit[0]    = NULL;
   arg.ppInit[1]    = NULL;
   arg.ppInit[2]    = NULL;

   for (i=1; i < argc; i++)
   {
      if (argv[i][0] != '-')
      {
	 fprintf(stderr, "Illegal argument '%s'.\n", argv[i]);
	 exit(1);
      }

      switch (argv[i][1])
      {
	case '?':
	case 'v':
	 fprintf(stderr, "Client version 1.0\n\n"
	                 "  -e <ext>  for line / extension.\n"
		         "  -?        for this text.\n"
			 "Copyright 1995(C)(R) by Valhalla I/S.\n"
		         "Use only permitted by license agreement.\n\n");
	 exit(0);
	 break;

	case 'e':
	 if (argv[i+1] == NULL)
	 {
	    fprintf(stderr, "Extension expected.\n");
	    exit(1);
	 }

	 arg.pExt = argv[++i];
	 break;

#ifdef SUSPEKT
	case 'p':
	 if (argv[i+1] == NULL)
	 {
	    fprintf(stderr, "Port number expected.\n");
	    exit(1);
	 }

	 n = atoi(argv[++i]);
	 if (n <= 1000)
	 {
	    fprintf(stderr, "Port number in reserved area.\n");
	    exit(1);
	 }
	 arg.nPort = n;
	 break;
#endif

	default:
	 fprintf(stderr, "Illegal option '%s'.\n", argv[i]);
	 exit(1);
      }
   }

   arg.pLogName      = str_dup(str_cc("logs/client.", arg.pExt));
   arg.pActivityName = str_dup(str_cc("logs/activity.", arg.pExt));
   
   ConfigFile(str_cc("config.", arg.pExt));

   return TRUE;
}

