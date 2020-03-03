/* *********************************************************************** *
 * File   : config.h                                  Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Server boot time configuration.                                *
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

#ifndef _MUD_CONFIG_H
#define _MUD_CONFIG_H

#include <arpa/inet.h>
#include "fight.h"

void ShowUsage(const char *c);

class CServerConfiguration
{
  public:
   CServerConfiguration(void);

   void Boot(void);

   int FromLAN(char *pFromHost);
   int ValidMplex(struct sockaddr_in *isa);
   void TripleColorFormat(struct SFightColorSet *pSet, char **ppList);

   int m_nMotherPort;
   int m_nRentModifier;
   int m_bAccounting;
   int m_bAliasShout;
   int m_bBBS;
   int m_bLawful;
   int m_bNoSpecials;
   int m_bBOB;
   int m_nShout;

   struct
   {
      struct SFightColorSet sMeleeHit;
      struct SFightColorSet sMeleeMiss;
      struct SFightColorSet sMeleeNodam;
      struct SFightColorSet sMeleeShield;
      struct SFightColorSet sMeleeDeath;

      const char *pDefault;
      const char *pPrompt;
      const char *pWhoTitle;
      const char *pWhoElem;
      const char *pRoomTitle;
      const char *pRoomText;
      const char *pExitsBegin;
      const char *pExitsEnd;
   } m_sColor;

   struct in_addr m_sSubnetMask;
   struct in_addr m_sLocalhost;

   struct in_addr m_aMplexHosts[10];

   char *m_pCredits;         /* the Credits List                */
   char *m_pNews;            /* the news                        */
   char *m_pMotd;            /* the messages of today           */
   char *m_pWizlist;         /* the wizlist                     */
   char *m_pLogo;            /* Intro screen                    */
   char *m_pWelcome;         /* Welcome Message                 */
   char *m_pGoodbye;         /* Goodbye message                 */
   char *m_pNewbie;          /* Newbie message                 */
};

extern class CServerConfiguration g_cServerConfig;

#endif // _MUD_CONFIG_H
