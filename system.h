/* *********************************************************************** *
 * File   : system.h                                  Part of Valhalla MUD *
 * Version: 1.21                                                           *
 * Author : seifert / quinn / bombman                                      *
 *                                                                         *
 * Purpose: network communication and other system dependant things.       *
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

#ifndef _MUD_SYSTEM_H
#define _MUD_SYSTEM_H

#include <sys/time.h>
#include "select.h"

#define MAX_MULTI      5  /* Maximum five multiconnects */

class cMultiHook : public cHook
{
  public:
   cMultiHook(void);
   void Input(int nFlags);

   void Close(void);
   int  Read(void);

   int succ_err;           /* Number of successive errors */
};

class cMultiMaster
{
  public:
   cMultiMaster(void);

   int nCount;
   cMultiHook Multi[MAX_MULTI];
};

extern class cMultiMaster Multi;

#define MAX_HOSTNAME   256

void init_mother(int nPort);
void descriptor_close(struct descriptor_data *d, int bSendClose = TRUE);
void MplexSendSetup(struct descriptor_data *d);

#if defined(SUNOS4)
void bzero(char *b, int length);
#endif

int any_event(int port, struct multi_type *m);
int multi_any_connect(int port);  /* test for event on socket port */
int multi_any_freaky(struct multi_type *m);
int multi_new(int mother, struct multi_type *m);
void multi_close_all(void);

int any_input(int fd);        /* test for input         */
int any_output(int fd);       /* test for output        */

int multi_process_input(struct multi_element *pm);

int write_to_descriptor(int desc, char *txt);
int read_from_descriptor(int desc, char *txt);

#define SNOOP_PROMPT "% " /* probably not very nice to have here, but hey! */

#endif /* _MUD_SYSTEM_H */
