/* *********************************************************************** *
 * File   : namelist.h                                Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: C++ Class of the popular Namelist                              *
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

#ifndef _MUD_NAMELIST_H
#define _MUD_NAMELIST_H

#include "hashstring.h"
#include "bytestring.h"

class cNamelist
{
  public:
   cNamelist(void);
   cNamelist(const char **list);
   ~cNamelist(void);

   void Free(void);
   void CopyList(const char **list);
   void CopyList(class cNamelist *);

   void AppendBuffer(CByteBuffer *pBuf);
   int  ReadBuffer(CByteBuffer *pBuf);
   void bread(ubit8 **b);

   void catnames(char *buf);

   void Remove(ubit32 idx);
   void RemoveName(const char *name);

   void Substitute(ubit32 idx, const char *newname);
   const char *Name(ubit32 idx = 0);
   cStringInstance *InstanceName(ubit32 idx = 0);

   void AppendName(const char *name);
   void PrependName(const char *name);

   cNamelist *Duplicate(void);

   const int IsNameIdx(const char *name);
   const int IsNameRawIdx(const char *name);
   const char *IsNameRaw(const char *name);
   const char *IsName(const char *name);

   inline ubit32 Length(void) { return length; }

  private:
   class cStringInstance **namelist;
   ubit32 length;
};

#endif
