/* *********************************************************************** *
 * File   : hashstring.h                              Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: C++ Public Headers for the String Reuse system.                *
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

#ifndef _MUD_STRING_H
#define _MUD_STRING_H

#include "essential.h"

void string_statistics(struct unit_data *ch);

class cStringConstant
{
   friend class cStringInstance;
   friend class cHashSystem;

   /* None of these definitions ought to be used by anything but the
      friends of this class */

  private:
   cStringConstant(const char *c, ubit32 len, ubit32 h);
   ~cStringConstant(void);
   inline const char *String(void) { return pStr; }
   inline cStringConstant *Next(void) { return pNext; }

   char   *pStr;
   ubit32 nReferences;
   class  cStringConstant *pNext;
   ubit32 nStrLen;
   // Possibly consider storing the Hash value for speed on destruction
};


class cStringInstance
{
  public:
   cStringInstance(void);
   cStringInstance(const char *str);
   ~cStringInstance(void);
   inline const char *StringPtr(void) { return pConst ? pConst->pStr : 0; }
   inline const char *String(void) { return pConst ? pConst->pStr : ""; }
   void Reassign(const char *c);
   inline const ubit32 Length(void) { return pConst ? pConst->nStrLen : 0; }

  private:
   void Make(const char *str);
   class cStringConstant *pConst;
};

#endif
