/* *********************************************************************** *
 * File   : log.h                                     Part of Valhalla MUD *
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
#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>

#define HERE __FILE__, __LINE__

class cLog
{
  public:
   cLog(char *name = "file.log");
   ~cLog(void);

   void Log(const char *format, ...);
   void Debug(const char *format, ...);
   void Error(const char *file, const int line, const char *format, ...);

  private:
   FILE *pFile;
};

extern cLog *pLogFile;

#endif
