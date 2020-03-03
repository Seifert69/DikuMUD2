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

#include <stdio.h>
#include <string.h>

#include "essential.h"
#include "config.h"
#include "files.h"
#include "textutil.h"
#include "db.h"
#include "str_parse.h"
#include "common.h"

class CServerConfiguration g_cServerConfig;

void ShowUsage(const char *name)
{
   fprintf(stderr, "Usage: %s [-h] [-d pathname] "
	   "[-z pathname] [-c {players}]\n", name);
   fprintf(stderr, "  -h: This help screen.\n");
   fprintf(stderr, "  -c: List or Convert playerfile\n");
   fprintf(stderr, "  -d: Alternate lib/ dir\n");
   fprintf(stderr, "  -z: Alternate zon/ dir\n");
   fprintf(stderr, "  -p: Persistant containers list\n");
   fprintf(stderr, "Copyright (C) 1994 - 1996 by Valhalla.\n");
}


CServerConfiguration::CServerConfiguration(void)
{
   m_bAccounting   = 0;
   m_bBBS          = FALSE;
   m_bLawful       = FALSE;
   m_bAliasShout   = TRUE;
   m_bNoSpecials   = FALSE;
   m_bBOB          = FALSE;
   m_nShout        = 1;
   m_nRentModifier = 10;
   m_nMotherPort   = 4999;

   // MS2020 memset(&m_sSubnetMask, sizeof(m_sSubnetMask), 0);
   // MS2020 memset(&m_sLocalhost, sizeof(m_sLocalhost), 0);
   memset(&m_sSubnetMask, 0, sizeof(m_sSubnetMask));
   memset(&m_sLocalhost, 0, sizeof(m_sLocalhost));

   m_pCredits = NULL;
   m_pNews    = NULL;
   m_pMotd    = NULL;
   m_pWizlist = NULL;
   m_pLogo    = NULL;
   m_pWelcome = NULL;
   m_pGoodbye = NULL;
   m_pNewbie  = NULL;
}


int CServerConfiguration::FromLAN(char *pFromHost)
{
   struct in_addr sTmp;

   if (inet_aton(pFromHost, &sTmp) == 0)
   {
      slog(LOG_ALL, 0, "Localhost invalid.");
      return FALSE;
   }

   return ((m_sSubnetMask.s_addr & m_sLocalhost.s_addr) ==
	   (m_sSubnetMask.s_addr & sTmp.s_addr));
}

int CServerConfiguration::ValidMplex(struct sockaddr_in *isa)
{
   struct in_addr sTmp;

   int i;

   for (i=0; i < 10; i++)
   {
     if (isa->sin_addr.s_addr == m_aMplexHosts[i].s_addr)
       return TRUE;
   }

   return FALSE;
}


void CServerConfiguration::TripleColorFormat(struct SFightColorSet *pSet,
					     char **ppList)
{
   if (ppList == NULL)
   {
      slog(LOG_ALL, 0, "Color triple non existant.");
      exit(0);
   }

   int i = 0;

   while (ppList[i])
     i++;

   if (i != 3)
   {
      slog(LOG_ALL, 0, "Color triple does not have three elements.");
      exit(0);
   }

   pSet->pAttacker = str_escape_format(ppList[0]);
   pSet->pDefender = str_escape_format(ppList[1]);
   pSet->pOthers   = str_escape_format(ppList[2]);

   free_namelist(ppList);
}

void CServerConfiguration::Boot(void)
{
   char Buf[2 * MAX_STRING_LENGTH];
   char *c, **list;
   const char *d;
   int i;

   extern char libdir[];

   slog(LOG_OFF, 0, "Booting server.");

   if (!file_exists(str_cc(libdir, SERVER_CONFIG)))
   {
      slog(LOG_ALL, 0, "No server configuration file.");
      return;
   }

   config_file_to_string(str_cc(libdir, SERVER_CONFIG), Buf, sizeof(Buf));

   c = Buf;

   if (parse_match_num(&c, "Port", &i))
   {
      m_nMotherPort = i;
   }

   if (!is_in(m_nMotherPort, 2000, 8000))
   {
      slog(LOG_ALL, 0, "Mother port not in [2000..8000].");
      exit(0);
   }

   if (parse_match_num(&c, "Rent", &i))
   {
      m_nRentModifier = i;
   }

   if (!is_in(m_nRentModifier, 0, 100))
   {
      slog(LOG_ALL, 0, "Rent modifier not in [0..100].");
      exit(0);
   }

   if (parse_match_num(&c, "BOB", &i))
   {
      m_bBOB = i;
   }

   if (!is_in(m_bBOB, 0, 1))
   {
     slog(LOG_ALL, 0, "BOB not 0 or 1");
     exit(0);
   }

   if (parse_match_num(&c, "Alias Shout", &i))
   {
      m_bAliasShout = i;
   }

   if (!is_in(m_bAliasShout, 0, 1))
   {
     slog(LOG_ALL, 0, "Alias Shout not 0 or 1");
     exit(0);
   }

   if (parse_match_num(&c, "No Specials", &i))
   {
      m_bNoSpecials = i;
   }

   if (!is_in(m_bNoSpecials, 0, 1))
   {
      slog(LOG_ALL, 0, "Specials not 0 or 1");
      exit(0);
   }

   if (parse_match_num(&c, "Lawful", &i))
   {
      m_bLawful = i;
   }

   if (!is_in(m_bLawful, 0, 1))
   {
      slog(LOG_ALL, 0, "Lawful not 0 or 1");
      exit(0);
   }

   if (parse_match_num(&c, "BBS", &i))
   {
      m_bBBS = i;
   }

   if (!is_in(m_bBBS, 0, 1))
   {
     slog(LOG_ALL, 0, "BBS not 0 or 1");
     exit(0);
   }

   if (parse_match_num(&c, "Accounting", &i))
   {
      m_bAccounting = i;
   }

   if (!is_in(m_bAccounting, 0, 1))
   {
     slog(LOG_ALL, 0, "Accounting not 0 or 1");
     exit(0);
   }

   if (parse_match_num(&c, "Shout", &i))
   {
      m_nShout = i;
   }

   if (!is_in(m_nShout, 0, 255))
   {
     slog(LOG_ALL, 0, "Shout level not [0..255]");
     exit(0);
   }

   d = parse_match_name(&c, "subnetmask");

   if (d == NULL)
     d = "255.255.255.255";

   if (inet_aton(d, &m_sSubnetMask) == 0)
   {
      slog(LOG_ALL, 0, "SubnetMask invalid.");
      exit(0);
   }

   d = parse_match_name(&c, "localhost");

   if (d == NULL)
     d = "127.0.0.1";

   if (inet_aton(d, &m_sLocalhost) == 0)
   {
      slog(LOG_ALL, 0, "Localhost invalid.");
      exit(0);
   }

   char **ppNames;

   ppNames = parse_match_namelist(&c, "mplex hosts");

   if (ppNames == NULL)
   {
      slog(LOG_ALL, 0, "Mplex hosts must be specified.");
      exit(0);
   }

   int l = 0;

   while (ppNames[l])
     l++;

   if (l < 1)
   {
      slog(LOG_ALL, 0, "Mplex hosts must have at least one entry.");
      exit(0);
   }

   if (l > 10)
   {
      slog(LOG_ALL, 0, "10 is maximum number of mplex hosts.");
      exit(0);
   }

   for (i = 0; i < 10; i++)
   {
      if (i < l)
      {
	 if (inet_aton(ppNames[i], &m_aMplexHosts[i]) == 0)
	 {
	   slog(LOG_ALL, 0, "Mplex host invalid IP.");
	   exit(0);
	 }
      }
      else
	m_aMplexHosts[i] = m_aMplexHosts[i-1];
   }


   m_sColor.pDefault = str_escape_format("&cw");

   d = parse_match_name(&c, "color prompt");
   m_sColor.pPrompt = d ? str_escape_format(d) : "";
   free(d);

   d = parse_match_name(&c, "color who");
   m_sColor.pWhoTitle = d ? str_escape_format(d) : "";
   free(d);

   d = parse_match_name(&c, "color who elem");
   m_sColor.pWhoElem = d ? str_escape_format(d) : "";
   free(d);

   d = parse_match_name(&c, "color room title");
   m_sColor.pRoomTitle = d ? str_escape_format(d) : "";
   free(d);

   d = parse_match_name(&c, "color room text");
   m_sColor.pRoomText = d ? str_escape_format(d) : "";
   free(d);

   d = parse_match_name(&c, "color exits begin");
   m_sColor.pExitsBegin = d ? str_escape_format(d) : "";
   free(d);

   d = parse_match_name(&c, "color exits end");
   m_sColor.pExitsEnd = d ? str_escape_format(d) : "";
   free(d);

   /* MELEE STUFF */

   list = parse_match_namelist(&c, "color melee hit");
   TripleColorFormat(&m_sColor.sMeleeHit, list);

   list = parse_match_namelist(&c, "color melee miss");
   TripleColorFormat(&m_sColor.sMeleeMiss, list);

   list = parse_match_namelist(&c, "color melee nodam");
   TripleColorFormat(&m_sColor.sMeleeNodam, list);

   list = parse_match_namelist(&c, "color melee shield");
   TripleColorFormat(&m_sColor.sMeleeShield, list);

   list = parse_match_namelist(&c, "color melee death");
   TripleColorFormat(&m_sColor.sMeleeDeath, list);

   


   /* News files, etc. */

   slog(LOG_OFF, 0, "Reading newsfile, credits, wizlist and motd.");

   char *read_info_file(char *name, char *oldstr);

   touch_file(str_cc(libdir, WIZLIST_FILE));
   m_pWizlist = read_info_file(str_cc(libdir, WIZLIST_FILE), m_pWizlist);

   touch_file(str_cc(libdir, NEWS_FILE));
   m_pNews    = read_info_file(str_cc(libdir, NEWS_FILE),    m_pNews);

   touch_file(str_cc(libdir, CREDITS_FILE));
   m_pCredits = read_info_file(str_cc(libdir, CREDITS_FILE), m_pCredits);

   touch_file(str_cc(libdir, MOTD_FILE));
   m_pMotd    = read_info_file(str_cc(libdir, MOTD_FILE),    m_pMotd);

   touch_file(str_cc(libdir, LOGO_FILE));
   m_pLogo    = read_info_file(str_cc(libdir, LOGO_FILE), m_pLogo);
  
   touch_file(str_cc(libdir, WELCOME_FILE));
   m_pWelcome  = read_info_file(str_cc(libdir, WELCOME_FILE), m_pWelcome);
  
   touch_file(str_cc(libdir, GOODBYE_FILE));
   m_pGoodbye  = read_info_file(str_cc(libdir, GOODBYE_FILE), m_pGoodbye);

   touch_file(str_cc(libdir, NEWBIE_FILE));
   m_pNewbie  = read_info_file(str_cc(libdir, NEWBIE_FILE), m_pNewbie);
}
