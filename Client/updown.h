/* *********************************************************************** *
 * File   : updown.h                                  Part of Valhalla MUD *
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

#ifndef _UPDOWN_H
#define _UPDOWN_H

#include "channel.h"


struct bbs_type
{
   int bOverdue;
   int nTotalCredit;
   char name[512];
   int nLevel;
   int nAccess;

   int bUpload;
   int bDownload;
};

extern struct bbs_type bbs;


class cUpload : public cChannel
{
  public:
   cUpload(ubit8 nChn, char *file);
   ~cUpload(void);
   
   void Receive(ubit8 *data, ubit32 len);
   const char *Status(void); // Returns how far we are
   void Thread(void);

  private:
   FILE *f;

   char *pFileName;

   ubit32 nState;
   ubit32 nLength;
   ubit16 nCrc;
   ubit32 nPos;

   char FileBuf[4 * (SLW_MAX_DATA + 1)];
};



class cDownload : public cChannel
{
  public:
   cDownload(ubit8 nChn, char *file);
   ~cDownload(void);

   void Receive(ubit8 *data, ubit32 len);
   const char *Status(void); // Returns how far we are

  private:
   FILE *f;
   FILE *fshadow;

   char *pBaseName;
   char *pFileName;
   char *pShadowName;
   char FileBuf[4 * SLW_MAX_DATA + 4];

   ubit32 nExpectedLength;
   ubit16 nExpectedCrc;

   ubit32 nState;
   ubit32 nLength;
   ubit16 nCrc;
   ubit8  bComplete;
};

#endif


