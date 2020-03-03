/* *********************************************************************** *
 * File   : interfc.h                                 Part of Valhalla MUD *
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
#ifndef _MS_NETINTERFC_H
#define _MS_NETINTERFC_H



class cNetInterface
{
  public:
   virtual int  Open(char *pDev)    { return 0;     }
   virtual int  Close(void)         { return 0;     }

   virtual int  Send(ubit8 data)                 = 0;
   virtual int  Send(const ubit8 *data, ubit32 len)    = 0;

   virtual int  SendString(char *String)         = 0;

   virtual void Receive(ubit8 *data, ubit32 len) = 0;

   virtual void Poll(void)          { return;       }

   virtual int  Online(void)        { return FALSE; }
   virtual int  Hangup(void)        { return -1;    }
   virtual int  fd(void)            { return -1;    }
};


#endif
