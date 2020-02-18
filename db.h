/* *********************************************************************** *
 * File   : db.h                                      Part of Valhalla MUD *
 * Version: 1.05                                                           *
 * Author : seifert@diku.dk and quinn@freja.diku.dk                        *
 *                                                                         *
 * Purpose: Header for the database stuff.                                 *
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

#ifndef _MUD_DB_H
#define _MUD_DB_H

#include "bytestring.h"
#include "dbfind.h"
#include "config.h"

#define MUD_NAME "Valhalla"  /* For ease of localization */

#define DFLT_DIR       "lib/"       // Default data directory
#define ZONE_DIR       "zon/"       // Default zone directory
#define PLAY_DIR       "ply/"       // Default player directory
#define UNIT_DIR       "units/"     // Directory for DIL store / restore units
#define PERSIST_DIR    "persist/"   // Directory for persistant units
#define BBS_DIR        "bbs/"       // Default BBS shared directory

#define BASIS_ZONE     "basis"

/* Data files (LIB) used by the game system */

#define RACE_DEFS         "races.dat"
#define ABILITY_DEFS      "abilities.dat"
#define SPELL_DEFS        "spells.dat"
#define WEAPON_DEFS       "weapons.dat"
#define SKILL_DEFS        "skills.dat"

#define SERVER_CONFIG     "server.cfg"

#define EXECUTE_FILE      "execute.dat"
#define STATISTIC_FILE    "connectionsnew.dat"
#define COMMAND_FILE      "commands"
#define COMPETITION_FILE  "competition"
#define ACCOUNT_FILE      "account"
#define ACCOUNT_LOG       "account.log"
#define CREDITFILE_LOG    "ccinfo.log"
#define STATISTICS_FILE   "statis.dat"
#define SLIME_FILE        "slime"
#define CRIME_NUM_FILE    "crime_nr"
#define CRIME_ACCUSE_FILE "crime"
#define PLAYER_ID_NAME    "players.id"
#define MAIL_FILE_NAME    "mailbox.idx"
#define MAIL_BLOCK_NAME   "mailbox.blk"
#define CREDITS_FILE      "credits"    /* for the 'credits' command  */
#define NEWS_FILE         "news"       /* for the 'news' command     */
#define MOTD_FILE         "motd"       /* messages of today          */
#define LOGO_FILE         "logo"
#define WELCOME_FILE      "welcome"
#define GOODBYE_FILE      "goodbye"
#define NEWBIE_FILE       "newbie"
#define IDEA_FILE         "ideas"      /* for the 'idea'-command     */
#define TYPO_FILE         "typos"      /*         'typo'             */
#define BUG_FILE          "bugs"       /*         'bug'              */
#define MESS_FILE         "messages"   /* damage message             */
#define SOCMESS_FILE      "actions"    /* messgs for social acts     */
#define HELP_FILE         "help"       /* for HELP <keyword>         */
#define HELP_FILE_WIZ     "help.wiz"   /* for HELP <keyword>         */
#define HELP_FILE_LOCAL   "help.local" /* for HELP <keyword>         */
#define WIZLIST_FILE      "wizlist"    /* for WIZLIST                */
#define POSEMESS_FILE     "poses"      /* for 'pose'-command         */

#define MONEYDEF_FILE     "money"      /* For definition of money    */

#define ZONE_FILE_LIST    "zonelist"


struct help_index_type
{
   char *keyword;
   long pos;
};

struct zone_info_type
{
   int no_of_zones;                 /* Total number of zones       */
   struct zone_type *zone_list;     /* The Linked List of Zones    */
   struct bin_search_type *ba;      /* Array of structs for search */
   void **spmatrix;                 /* Inter zone shortest paths   */
};

struct unit_data *read_unit_string(CByteBuffer *pBuf, int type, int len,
				   int bSwapin, char *whom);
void read_unit_file(struct file_index_type *org_fi, CByteBuffer *pBuf);
struct unit_data *read_unit(struct file_index_type *fi);
void free_unit(struct unit_data *ch);
void free_extra_descr(struct extra_descr_data *ex);
void free_extra_descr_list(struct extra_descr_data *ex);

struct extra_descr_data *create_extra_descr(void);
struct unit_data *create_unit(ubit8 type);


/* --- The globals of db.c --- */

extern int room_number;
extern struct unit_data *unit_list;
extern struct zone_info_type zone_info;

#endif /* _MUD_DB_H */
