/* *********************************************************************** *
 * File   : log.cc                                    Part of Valhalla MUD *
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
#include "log.h"

cLog *pLogFile = NULL;


cLog::cLog(char *name)
{
   pFile = fopen(name, "a+b");
}

cLog::~cLog(void)
{
   if (pFile)
     fclose(pFile);
}

void cLog::Log(const char *fmt, ...)
{
   char buf[2048];
   va_list args;

   va_start(args, fmt);
   vsprintf(buf, fmt, args);
   va_end(args);

   if (pFile)
   {
      fprintf(pFile, "%s\n", buf);
      fflush(pFile);
   }
   else
     fprintf(stderr, "%s\n", buf);
}


void cLog::Debug(const char *fmt, ...)
{
   char buf[2048];
   va_list args;

   va_start(args, fmt);
   vsprintf(buf, fmt, args);
   va_end(args);

   if (pFile)
   {
      fprintf(pFile, "%s\n", buf);
      fflush(pFile);
   }

   fprintf(stderr, "%s\n", buf);
}


void cLog::Error(const char *file, const int line, const char *fmt, ...)
{
   char buf[2048];
   va_list args;

   va_start(args, fmt);
   vsprintf(buf, fmt, args);
   va_end(args);

   if (pFile)
   {
      fprintf(pFile, "%s\n", buf);
      fflush(pFile);
   }
   else
     fprintf(stderr, "%s\n", buf);

   abort();
}
