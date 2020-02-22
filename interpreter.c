/* *********************************************************************** *
 * File   : interpreter.c                             Part of Valhalla MUD *
 * Version: 1.02                                                           *
 * Author : All                                                            *
 *                                                                         *
 * Purpose: Interpretation of commands.                                    *
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

/* This won't work (noop, 1989)                 */
/* It will now work even better (seifert, 1991) */

/* Aug 24 1993 gnort: cmd_interpreter will now strip trailing spaces of arg */
/* Aug 11 1994 gnort: got rid of do_qui & co.                               */
/* Aug 14 1994 gnort: Inserted new social-command system                    */

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "structs.h"
#include "textutil.h"
#include "comm.h"
#include "interpreter.h"
#include "db.h"
#include "utils.h"
#include "limits.h"
#include "skills.h"
#include "trie.h"
#include "utility.h"
#include "files.h"
#include "common.h"
#include "constants.h"
#include "unitfind.h"
#include "dilrun.h"

/* external fcntls */
extern struct unit_function_array_type unit_function_array[];

bool cmd_is_a_social(char *cmd, int complete);
bool perform_social(struct unit_data *, char *, const command_info *);

struct trie_type *intr_trie = NULL;


struct command_info cmd_info[] =
{
{ 0, 0, "north",CMD_NORTH, POSITION_SITTING, do_move, 0},
{ 0, 0, "east", CMD_EAST,  POSITION_SITTING, do_move, 0},
{ 0, 0, "south",CMD_SOUTH, POSITION_SITTING, do_move, 0},
{ 0, 0, "west", CMD_WEST,  POSITION_SITTING, do_move, 0},
{ 0, 0, "up",   CMD_UP,    POSITION_SITTING, do_move, 0},
{ 0, 0, "down", CMD_DOWN,  POSITION_SITTING, do_move, 0},

{ 0, 0, "look",      CMD_LOOK,POSITION_RESTING,do_look, 0},
{ 0, 0, "inventory", CMD_INVENTORY,POSITION_DEAD,do_inventory,0},
{12, 1, "get",       CMD_GET,POSITION_RESTING,do_get,0},
{ 0, 0, "tell",      CMD_TELL,POSITION_DEAD,do_tell,0},
{12, 1, "take",      CMD_GET,POSITION_RESTING,do_get,0},
{ 6, 1, "wear",      CMD_WEAR,POSITION_RESTING,do_wear,0},
{ 6, 1, "wield",     CMD_WIELD,POSITION_RESTING,do_wield,0},
{ 0, 0, "say",       CMD_SAY,POSITION_RESTING,do_say,0},
{ 0, 0, "shout",     CMD_SHOUT,POSITION_RESTING,do_shout,0},
{ 0, 0, "help",      CMD_HELP,POSITION_DEAD,do_help,0},
{ 0, 0, "equipment", CMD_EQUIPMENT,POSITION_SLEEPING,do_equipment,0},

{ 0, 0, "examine",   CMD_EXAMINE, POSITION_SITTING,  do_examine, 0},
{ 6, 1, "enter",     CMD_ENTER,   POSITION_STANDING, do_enter,   0},
{ 6, 1, "exit",      CMD_EXIT,    POSITION_FIGHTING, do_exit,    0},
{ 0, 0, "exits",     CMD_EXITS,   POSITION_RESTING,  do_exits,   0},
{ 6, 1, "mount",     CMD_MOUNT,   POSITION_STANDING, do_enter,   0},
{ 6, 1, "leave",     CMD_LEAVE,   POSITION_STANDING, do_leave,   0},
{ 0, 0, "doors",     CMD_EXITS,   POSITION_RESTING,  do_exits,   0},
{ 0, 0, "directions",CMD_EXITS,   POSITION_RESTING,  do_exits,   0},
{ 6, 1, "dismount",  CMD_DISMOUNT,POSITION_SITTING,  do_exit,    0},
{ 6, 1, "sail",      CMD_SAIL,    POSITION_SITTING,  do_sail,    0},
{ 6, 1, "ride",      CMD_RIDE,    POSITION_SITTING,  do_ride,    0},

{ 6, 1, "drink",CMD_DRINK,POSITION_RESTING,do_drink,0},
{ 6, 1, "eat",CMD_EAT,POSITION_RESTING,do_eat,0},
{ 0, 1, "kill",CMD_KILL,POSITION_FIGHTING,do_kill,1},
{ 6, 1, "decapitate",CMD_DECAPITATE,POSITION_SITTING,do_decapitate,0},

{ 0, 0, "who",CMD_WHO,POSITION_DEAD,do_who,0},
{ 0, 0, "emote",CMD_EMOTE,POSITION_SLEEPING,do_emote,1},
{ 0, 0, "stand",CMD_STAND,POSITION_RESTING,do_stand,0},

{ 0, 0, "guilds",CMD_GUILD,POSITION_DEAD,do_guild,0},
{ 0, 0, "score",CMD_SCORE,POSITION_DEAD,do_score,0},
{ 0, 0, "status", CMD_STATUS,POSITION_DEAD,do_status,0},

{ 0, 0, "sit",CMD_SIT,POSITION_RESTING,do_sit,0},
{ 0, 0, "rest",CMD_REST,POSITION_RESTING,do_rest,0},
{ 0, 0, "sleep",CMD_SLEEP,POSITION_SLEEPING,do_sleep,0},
{ 0, 0, "wake",CMD_WAKE,POSITION_SLEEPING,do_wake,0},
{ 0, 0, "news",CMD_NEWS,POSITION_SLEEPING,do_news,0},
{ 0, 0, "buy",CMD_BUY,POSITION_STANDING,do_not_here,0},
{ 0, 0, "sell",CMD_SELL,POSITION_STANDING,do_not_here,0},
{ 0, 0, "value",CMD_VALUE,POSITION_STANDING,do_not_here,0},
{ 0, 0, "list",CMD_LIST,POSITION_STANDING,do_not_here,0},
{ 3, 0, "drop",CMD_DROP,POSITION_RESTING,do_drop,0},
{ 0, 0, "weather",CMD_WEATHER,POSITION_RESTING,do_weather,0},
{ 6, 1, "read",CMD_READ,POSITION_RESTING,do_read,0},
{ 0, 0, "pour",CMD_POUR,POSITION_STANDING,do_pour,0},
{ 6, 1, "grab",CMD_HOLD,POSITION_RESTING,do_grab,0},
{ 6, 1, "remove",CMD_REMOVE,POSITION_RESTING,do_remove,0},
{12, 1, "put",CMD_PUT,POSITION_RESTING,do_put,0},
{ 0, 0, "save",CMD_SAVE,POSITION_SLEEPING,do_save,0},
{ 0, 1, "hit",CMD_HIT,POSITION_FIGHTING,do_hit,1},
{12, 1, "give",CMD_GIVE,POSITION_RESTING,do_give,0},

{ 0, 0, "time",CMD_TIME,POSITION_DEAD,do_time,0},
{ 0, 0, "idea",CMD_IDEA,POSITION_DEAD,do_ideatypobug,0},
{ 0, 0, "typo",CMD_TYPO,POSITION_DEAD,do_ideatypobug,0},
{ 0, 0, "bug",CMD_BUG,POSITION_DEAD,do_ideatypobug,0},
{ 0, 0, "whisper",CMD_WHISPER,POSITION_RESTING,do_whisper,0},
{ 0, 1, "cast",CMD_CAST,POSITION_MORTALLYW,do_cast,0},
{ 0, 0, "ask",CMD_ASK,POSITION_RESTING,do_ask,0},
{ 0, 0, "areas",CMD_AREAS,POSITION_DEAD,do_areas,0},
{ 0, 0, "order",CMD_ORDER,POSITION_RESTING,do_order,0},
{ 6, 1, "sip",CMD_SIP,POSITION_RESTING,do_sip,0},
{ 6, 1, "taste",CMD_TASTE,POSITION_RESTING,do_taste,0},
{ 0, 0, "follow",CMD_FOLLOW,POSITION_RESTING,do_follow,0},
{ 0, 0, "rent",CMD_RENT,POSITION_RESTING,do_rent,0},
{ 0, 0, "offer",CMD_OFFER,POSITION_STANDING,do_not_here,0},
{12, 1, "open",CMD_OPEN,POSITION_SITTING,do_open,0},
{12, 1, "close",CMD_CLOSE,POSITION_SITTING,do_close,0},
{12, 1, "lock",CMD_LOCK,POSITION_SITTING,do_lock,0},
{12, 1, "unlock",CMD_UNLOCK,POSITION_SITTING,do_unlock,0},
{ 0, 0, "write",CMD_WRITE,POSITION_RESTING,do_write,0},
{ 6, 1, "hold",CMD_HOLD,POSITION_RESTING,do_grab,0},
{ 6, 1, "flee",CMD_FLEE,POSITION_FIGHTING,do_flee,0},
{ 0, 1, "sneak",CMD_SNEAK,POSITION_STANDING,do_sneak, 0},
{ 0, 1, "hide",CMD_HIDE,POSITION_RESTING,do_hide, 0},
{12, 1, "backstab",CMD_BACKSTAB,POSITION_STANDING,do_backstab, 0},
{ 0, 1, "pick",CMD_PICK,POSITION_STANDING,do_pick, 0 },
{ 0, 1, "steal",CMD_STEAL,POSITION_STANDING,do_steal, 0 },
{ 6, 1, "bash",CMD_BASH,POSITION_FIGHTING,do_bash, 0 },
{12, 1, "rescue",CMD_RESCUE,POSITION_FIGHTING,do_rescue, 0},
{ 6, 1, "kick",CMD_KICK,POSITION_FIGHTING,NULL, 0 },
{ 0, 1, "search",CMD_SEARCH,POSITION_STANDING,do_search,0},
{ 0, 0, "practice",CMD_PRACTICE,POSITION_STANDING,do_practice,0},
{ 0, 0, "info",CMD_INFO,POSITION_SLEEPING,do_info,0},
{ 0, 0, "'",CMD_SAY,POSITION_RESTING,do_say,0},
{ 0, 0, "practise",CMD_PRACTICE,POSITION_STANDING,do_practice,0},
{ 6, 1, "use",CMD_USE,POSITION_SITTING,do_use,0},
{ 0, 0, "where",CMD_WHERE,POSITION_DEAD,do_where,0},
{ 0, 0, "level",CMD_LEVEL,POSITION_DEAD,do_level,0},
{ 0, 0, ",",CMD_EMOTE,POSITION_SLEEPING,do_emote,0},
//{ 0, 0, "brief",CMD_BRIEF,POSITION_DEAD,do_brief,0},

{ 0, 0, "wiz",CMD_WIZ,POSITION_RESTING,do_wiz,210},        /* lone wizcommands */
{ 0, 0, "echo",CMD_ECHO,POSITION_SLEEPING,do_echo,213},

{ 0, 0, "wizlist",CMD_WIZLIST,POSITION_DEAD,do_wizlist,0},
{ 6, 1, "consider",CMD_CONSIDER,POSITION_RESTING,do_consider,0},
{ 0, 0, "group",CMD_GROUP,POSITION_RESTING,do_group,0},
{ 6, 1, "quaff",CMD_QUAFF,POSITION_RESTING,do_quaff,0},
{24, 1, "recite",CMD_RECITE,POSITION_RESTING,do_recite,0},
{ 0, 0, "credits",CMD_CREDITS,POSITION_DEAD,do_credits,0},
//{ 0, 0, "compact",CMD_COMPACT,POSITION_DEAD,do_compact,0},
{ 0, 1, "dig",CMD_DIG,POSITION_STANDING,do_dig,0},
{ 0, 1, "bury",CMD_BURY,POSITION_STANDING,do_bury,0},
{12, 1, "turn",CMD_TURN,POSITION_FIGHTING,do_turn,0},
{ 6, 1, "diagnose",CMD_DIAGNOSE,POSITION_RESTING,NULL,0},
{ 6, 1, "appraise",CMD_APPRAISE,POSITION_RESTING,do_appraise,0},
{ 0, 1, "ventriloquate",CMD_VENTRILOQUATE,POSITION_RESTING,do_ventriloquate,0},
{ 6, 1, "aid", CMD_AID, POSITION_RESTING, do_aid, 0},
{ 0, 1, "climb", CMD_CLIMB, POSITION_STANDING, do_not_here, 0},
{ 0, 1, "trip", CMD_TRIP, POSITION_FIGHTING, NULL, 0},
{ 0, 1, "cuff", CMD_CUFF, POSITION_STANDING, NULL, 0},
{12, 1, "light",CMD_LIGHT,POSITION_RESTING,do_light,0},
{12, 1, "extinguish",CMD_EXTINGUISH,POSITION_RESTING,do_extinguish,0},
//{ 0, 0, "wimpy", CMD_WIMPY,POSITION_DEAD,do_wimpy,0},
//{ 0, 0, "peaceful",CMD_PEACEFUL,POSITION_DEAD,do_peaceful,0},
{ 0, 1, "drag",CMD_DRAG,POSITION_STANDING,do_drag,0},
{ 0, 0, "mail",CMD_MAIL,POSITION_RESTING,do_not_here,0},
{ 0, 0, "request",CMD_REQUEST,POSITION_RESTING,do_not_here,0},
{ 0, 0, "join",CMD_JOIN,POSITION_RESTING,do_not_here,0},
{ 0, 0, "contract",CMD_CONTRACT,POSITION_RESTING,do_not_here,0},
{ 0, 1, "break", CMD_BREAK,POSITION_STANDING,do_not_here,0},
{ 0, 0, "knock", CMD_KNOCK,POSITION_STANDING,do_knock,0},
{ 0, 0, "split", CMD_SPLIT,POSITION_RESTING,do_split,0},
//{ 0, 0, "prompt",CMD_PROMPT,POSITION_SLEEPING,do_prompt,0},
//{ 0, 0, "reply",CMD_REPLY,POSITION_RESTING,do_not_here,0},
//{ 0, 0, "echosay",CMD_ECHOSAY,POSITION_SLEEPING,do_echosay,0},
{ 0, 0, "quests",CMD_QUESTS,POSITION_SLEEPING,do_quests,0},
{ 0, 0, "purse",CMD_PURSE,POSITION_DEAD,do_purse,0},
{ 0, 0, "change",CMD_CHANGE,POSITION_DEAD,do_change,0},
{ 0, 0, "commands", CMD_COMMANDS, POSITION_DEAD, do_commands, 0},
{ 0, 0, "socials", CMD_SOCIALS, POSITION_DEAD, do_socials, 0},

{ 0, 0, "pose",CMD_POSE,POSITION_STANDING,do_pose,0},
{ 0, 0, "insult",CMD_INSULT,POSITION_RESTING,do_insult,0},
{ 0, 0, "pray",CMD_PRAY,POSITION_DEAD,do_pray,0},
{ 0, 0, "sacrifice",CMD_SACRIFICE,POSITION_RESTING,do_sacrifice,0},
{ 0, 0, "ignore",CMD_IGNORE, POSITION_DEAD, do_ignore, 0},
{ 0, 0, "reply",CMD_REPLY, POSITION_DEAD, do_reply, 0},

//{ 0, 0, "inform",CMD_INFORM,POSITION_SLEEPING,do_inform,0},
//{ 0, 0, "expert",CMD_EXPERT,POSITION_DEAD,do_expert,0},

{ 0, 1, "quit",CMD_QUIT,POSITION_DEAD,do_quit,0},
{ 0, 1, "resize", CMD_RESIZE, POSITION_STANDING, NULL, 0},
{ 0, 1, "evaluate", CMD_EVALUATE, POSITION_STANDING, NULL, 0},
{ 0, 1, "ditch", CMD_DITCH, POSITION_RESTING, NULL, 0},

/* Demi Gods 200 - 219 */
{ 0, 0, "wizhelp",CMD_WIZHELP,POSITION_SLEEPING,do_wizhelp,200},

/* Extended Where at 201 */
{ 0, 0, "users",CMD_USERS,POSITION_DEAD,do_users,202},

/* Extended WHO at 203 */
{ 0, 0, "title",CMD_TITLE,POSITION_DEAD,do_title,204}, /* other's at 240 */
{ 0, 0, "wstat",CMD_WSTAT,POSITION_DEAD,do_wstat,215},
{ 0, 0, "goto",CMD_GOTO,POSITION_SLEEPING,do_goto,219},

/* Foreign Gods 219..210 */
{ 0, 0, "message",CMD_MESSAGE,POSITION_DEAD,do_message,220},
{ 0, 0, "at",CMD_AT,POSITION_DEAD,do_at,220},

/* Gods of possible creation 229..220 */

/* Gods of Creation 239..230 */
{ 0, 0, "load",CMD_LOAD,POSITION_DEAD,do_load,230},
{ 0, 0, "purge",CMD_PURGE,POSITION_DEAD,do_purge,230},
{ 0, 0, "wizinv",CMD_WIZINV,POSITION_DEAD,do_wizinv,230},
{ 0, 0, "path",CMD_PATH,POSITION_DEAD,do_path,230},
{ 0, 0, "wizlock",CMD_WIZLOCK,POSITION_DEAD,do_wizlock,230},
{ 0, 0, "reset",CMD_RESET,POSITION_RESTING,do_reset,230},
{ 0, 0, "boards",CMD_BOARDS,POSITION_DEAD,do_boards,230},
{ 0, 0, "corpses",CMD_CORPSES,POSITION_DEAD,do_corpses,219},

/* Most trusted builders only (239) */
{ 0, 0, "finger", CMD_FINGER, POSITION_DEAD, do_finger, 239},
{ 0, 0, "transfer",CMD_TRANSFER,POSITION_SLEEPING,do_trans,235},
{ 0, 0, "force",CMD_FORCE,POSITION_SLEEPING,do_force,239},
{ 0, 0, "set",CMD_SET,POSITION_SLEEPING,do_set,230},
{ 0, 0, "setskill",CMD_SETSKILL,POSITION_SLEEPING,do_setskill,239},
{ 0, 0, "restore",CMD_RESTORE,POSITION_DEAD,do_restore,239},
{ 0, 0, "snoop",CMD_SNOOP,POSITION_DEAD,do_snoop,239},
{ 0, 0, "switch",CMD_SWITCH,POSITION_DEAD,do_switch,239},

/* Gods of World Validation and Exploration 249..240 */
{ 0, 0, "advance",CMD_ADVANCE,POSITION_DEAD,do_advance, OVERSEER_LEVEL},
{ 0, 0, "notell",CMD_NOTELL,POSITION_SLEEPING,do_notell, 200},
{ 0, 0, "noshout",CMD_NOSHOUT,POSITION_SLEEPING,do_noshout,200},
{ 0, 0, "broadcast",CMD_BROADCAST,POSITION_DEAD,do_broadcast,240},
{ 0, 0, "ban",CMD_BAN,POSITION_DEAD,do_ban,240},

/* Gods of Honour 253..250 */
{ 0, 0, "freeze", CMD_FREEZE,POSITION_DEAD,do_freeze,250},

/* Game Administrators 254 */
{ 0, 0, "delete",CMD_DELETE,POSITION_DEAD,do_delete, OVERSEER_LEVEL},
{ 0, 0, "shutdown",CMD_SHUTDOWN,POSITION_DEAD,do_shutdown,ULTIMATE_LEVEL},
{ 0, 0, "reboot", CMD_REBOOT,POSITION_DEAD,do_reboot,240},
{ 0, 0, "execute", CMD_EXECUTE,POSITION_DEAD,do_execute,ADMINISTRATOR_LEVEL},
{ 0, 0, "reroll",CMD_REROLL,POSITION_DEAD,do_reroll,235},

/* Implementors 255 */
{ 0, 0, "makemoney",CMD_MAKEMONEY,POSITION_DEAD,do_makemoney,239},
{ 0, 0, "file",CMD_FILE,POSITION_DEAD,do_file,253},
{ 0, 0, "crash", CMD_CRASH,POSITION_STANDING,do_crash,255},
{ 0, 0, "account", CMD_ACCOUNT, POSITION_DEAD, do_account, 0},

{ 0, 0, "ski0", CMD_PEEK, POSITION_DEAD, 0, 0},
{ 0, 0, "ski1", CMD_FILCH, POSITION_DEAD, 0, 0},
{ 0, 0, "ski2", CMD_PICK_POCKET, POSITION_DEAD, 0, 0},
{ 0, 0, "ski3", CMD_DISARM, POSITION_DEAD, 0, 0},
{ 0, 0, "ski4", CMD_DONATE, POSITION_DEAD, 0, 0},
{ 0, 0, "ski5", CMD_ASSIST, POSITION_DEAD, 0, 0},
{ 0, 0, "ski6", CMD_SKIN,   POSITION_DEAD, 0, 0},
{ 0, 0, "ski7", CMD_SKILL7, POSITION_DEAD, 0, 0},
{ 0, 0, "ski8", CMD_SKILL8, POSITION_DEAD, 0, 0},
{ 0, 0, "ski9", CMD_SKILL9, POSITION_DEAD, 0, 0},
{ 0, 0, "ski9", CMD_SKILL10, POSITION_DEAD, 0, 0},
{ 0, 0, "ski9", CMD_SKILL11, POSITION_DEAD, 0, 0},
{ 0, 0, "ski9", CMD_SKILL12, POSITION_DEAD, 0, 0},
{ 0, 0, "ski9", CMD_SKILL13, POSITION_DEAD, 0, 0},
{ 0, 0, "ski9", CMD_SKILL14, POSITION_DEAD, 0, 0},
{ 0, 0, "ski9", CMD_SKILL15, POSITION_DEAD, 0, 0},
{ 0, 0, "ski9", CMD_SKILL16, POSITION_DEAD, 0, 0},
{ 0, 0, "ski9", CMD_SKILL17, POSITION_DEAD, 0, 0},
{ 0, 0, "ski9", CMD_SKILL18, POSITION_DEAD, 0, 0},
{ 0, 0, "ski9", CMD_SKILL19, POSITION_DEAD, 0, 0},
{ 0, 0, "ski9", CMD_SKILL20, POSITION_DEAD, 0, 0},
{ 0, 0, "ski9", CMD_SKILL21, POSITION_DEAD, 0, 0},
{ 0, 0, "ski9", CMD_SKILL22, POSITION_DEAD, 0, 0},
{ 0, 0, "ski9", CMD_SKILL23, POSITION_DEAD, 0, 0},
{ 0, 0, "ski9", CMD_SKILL24, POSITION_DEAD, 0, 0},
{ 0, 0, "ski9", CMD_SKILL25, POSITION_DEAD, 0, 0},
{ 0, 0, "ski9", CMD_SKILL26, POSITION_DEAD, 0, 0},
{ 0, 0, "ski9", CMD_SKILL27, POSITION_DEAD, 0, 0},
{ 0, 0, "ski9", CMD_SKILL28, POSITION_DEAD, 0, 0},
{ 0, 0, "ski9", CMD_SKILL29, POSITION_DEAD, 0, 0},
{ 0, 0, "ski9", CMD_SKILL30, POSITION_DEAD, 0, 0},

{ 0, 0, "",0,0,0,0}
};

/* { 0, 0, "manifest",CMD_MANIFEST,POSITION_DEAD,do_manifest,200},*/
/* { 0, 0, "verify",CMD_VERIFY,POSITION_DEAD,do_verify,250}, */
/* { 0, 0, "kickit",CMD_KICKIT,POSITION_DEAD,do_kickit,230}, */

struct command_info cmd_auto_tick =
{
  0,0,NULL, CMD_AUTO_TICK, POSITION_DEAD, NULL, 0,
};

struct command_info cmd_auto_enter =
{
  0,0,NULL, CMD_AUTO_ENTER, POSITION_STANDING, NULL, 0,
};

struct command_info cmd_auto_extract =
{
  0,0,NULL, CMD_AUTO_EXTRACT, POSITION_DEAD, NULL, 0,
};

struct command_info cmd_auto_death =
{
  0,0,NULL, CMD_AUTO_DEATH, POSITION_DEAD, NULL, 0,
};

struct command_info cmd_auto_combat =
{
  0,0,NULL, CMD_AUTO_COMBAT, POSITION_DEAD, NULL, 0,
};

struct command_info cmd_auto_unknown =
{
  0,0,NULL, CMD_AUTO_UNKNOWN, POSITION_DEAD, NULL, 0,
};

struct command_info cmd_auto_save =
{
  0,0,NULL, CMD_AUTO_SAVE, POSITION_DEAD, NULL, 0,
};

struct command_info cmd_auto_msg =
{
  0,0,NULL, CMD_AUTO_MSG, POSITION_DEAD, NULL, 0,
};

/* This is so that we can distuinguish socials and unknowns... */
struct command_info cmd_a_social =
{
   0,0,NULL, CMD_A_SOCIAL, POSITION_DEAD, NULL, 0,
};

struct command_info cmd_auto_damage =
{
  0, 0, NULL, CMD_AUTO_DAMAGE, POSITION_DEAD, NULL, 0
};

struct command_info *cmd_follow = NULL;

void wrong_position(struct unit_data *ch)
{
  static const char *strings[] =
  {
    "Lie still; you are DEAD!!! :-( \n\r",                       /* Dead     */
    "You are in a pretty bad shape, unable to do anything!\n\r", /* Mortally */
    "You are in a pretty bad shape, unable to do anything!\n\r", /* Incap    */
    "All you can do right now, is think about the stars!\n\r",   /* Stunned  */
    "In your dreams, or what?\n\r",                              /* Sleeping */
    "You feel too relaxed to do that, perhaps you should stand up?\n\r",/*Rst*/
    "Maybe you should get on your feet first?\n\r",              /* Sitting  */
    "No way! You are fighting for your life!\n\r"                /* Fighting */
  };

  if (CHAR_POS(ch) < POSITION_STANDING)
    send_to_char(strings[CHAR_POS(ch)], ch);
}


#ifdef DEBUG_HISTORY

#define MAX_DEBUG_HISTORY 20

class command_hist
{
  public:
   command_hist(void) { str[0] = pcname[0] = 0; fi = NULL; }

   char str[MAX_INPUT_LENGTH+50];
   char pcname[PC_MAX_NAME+1];
   struct file_index_type *fi;
} command_history_data[MAX_DEBUG_HISTORY];

static int command_history_pos = 0;

static void add_command_history(struct unit_data *u, char *str)
{
   if (IS_PC(u))
     strcpy(command_history_data[command_history_pos].pcname, UNIT_NAME(u));
   else
     command_history_data[command_history_pos].pcname[0] = 0;

   strcpy(command_history_data[command_history_pos].str, str);
   command_history_data[command_history_pos].fi  = UNIT_FILE_INDEX(u);

   command_history_pos = (command_history_pos + 1) % MAX_DEBUG_HISTORY;
}

static void dump_command_history(void)
{
   int i, j;
   for (j=0, i=command_history_pos; j < MAX_DEBUG_HISTORY; j++)
   {
      slog(LOG_ALL, 0, "CMD %s@%s [%s]",
	   command_history_data[i].pcname ?
	   command_history_data[i].pcname :
	   FI_NAME(command_history_data[i].fi),

	   command_history_data[i].pcname ?
	   "" : FI_ZONENAME(command_history_data[i].fi),
	   command_history_data[i].str);
      i = (i + 1) % MAX_DEBUG_HISTORY;
   }
}



class func_hist
{
  public:
   func_hist(void) { idx=0; flags=0; fi=NULL;}
   ubit16 idx;
   ubit16 flags;
   struct file_index_type *fi;
} func_history_data[MAX_DEBUG_HISTORY];

static int func_history_pos = 0;

void add_func_history(struct unit_data *u, ubit16 idx, ubit16 flags)
{
   func_history_data[func_history_pos].idx = idx;
   func_history_data[func_history_pos].flags = flags;
   func_history_data[func_history_pos].fi = UNIT_FILE_INDEX(u);

   func_history_pos = (func_history_pos + 1) % MAX_DEBUG_HISTORY;
}

static void dump_func_history(void)
{
   int i, j;
   char buf2[512];

   for (j=0, i=func_history_pos; j < MAX_DEBUG_HISTORY; j++)
   {
      slog(LOG_ALL, 0, "FUNC %s@%s: '%s (%d) %s (%d)'",
	   FI_NAME(func_history_data[i].fi),
	   FI_ZONENAME(func_history_data[i].fi),
	   unit_function_array[func_history_data[i].idx].name,
	   func_history_data[i].idx,
	   sprintbit(buf2, func_history_data[i].flags, sfb_flags),
	   func_history_data[i].flags);
      i = (i + 1) % MAX_DEBUG_HISTORY;
   }
}

void dump_debug_history(void)
{
   dump_command_history();
   dump_func_history();
}
#endif


void command_interpreter(struct unit_data *ch, char *arg)
{
   char cmd[MAX_INPUT_LENGTH + 10];
   char argstr[MAX_INPUT_LENGTH + 10];
   char *orgarg = arg;
   struct command_info *cmd_ptr;
   int is_social = FALSE;

   assert(IS_CHAR(ch));

   if (is_destructed(DR_UNIT, ch))
     return;

   if (strlen(arg) > MAX_INPUT_LENGTH)
   {
      slog(LOG_ALL, 0, "%s issued command which was too long: %s",
	   UNIT_NAME(ch), arg);
      send_to_char("The issued command was aborted because it was too long, "
		   "please report how you made this happen.\n\r", ch);
      return;
   }

   /* Find first non blank */
   arg = skip_spaces(arg);

   if (CHAR_DESCRIPTOR(ch))
   {
     if (*arg == '!')
       arg = CHAR_DESCRIPTOR(ch)->history;
     else
       strcpy(CHAR_DESCRIPTOR(ch)->history, arg);
   }

#ifdef DEBUG_HISTORY
   add_command_history(ch, arg);
#endif

   arg = str_next_word(arg, cmd);

   if (CHAR_DESCRIPTOR(ch))
     strcpy(CHAR_DESCRIPTOR(ch)->last_cmd, cmd);

   if (!cmd[0])
     return;

   strncpy(argstr, skip_blanks(arg), MAX(0, MAX_INPUT_LENGTH-1-strlen(cmd)));
   argstr[MAX(0,MAX_INPUT_LENGTH-1-strlen(cmd))] = 0;

   strip_trailing_spaces(argstr);

   if ((cmd_ptr = (struct command_info *) search_trie(cmd, intr_trie)) == NULL
       || (is_social = cmd_is_a_social(cmd, TRUE)))
   {
      struct command_info the_cmd = 
        {0,0,NULL, CMD_AUTO_UNKNOWN, POSITION_DEAD, NULL, 0};

      if (is_social || (cmd_ptr == NULL && cmd_is_a_social(cmd, FALSE)))
	the_cmd.no = CMD_A_SOCIAL;

      the_cmd.cmd_str = str_dup(cmd);

      if (send_preprocess(ch, &the_cmd, argstr) == SFR_SHARE)
	if (!perform_social(ch, argstr, &the_cmd))
	  act("$2t is not a known command.",
	      A_ALWAYS, ch, cmd, NULL, TO_CHAR);

      if (the_cmd.cmd_str)
	free(the_cmd.cmd_str);
      return;
   }

   if (cmd_ptr->combat_buffer && CHAR_COMBAT(ch))
   {
      if (CHAR_COMBAT(ch)->When() >= SPEED_DEFAULT)
      {
	 CHAR_COMBAT(ch)->setCommand(orgarg);
	 return;
      }
      else
	CHAR_COMBAT(ch)->changeSpeed(cmd_ptr->combat_speed);
   }

   if (send_preprocess(ch, cmd_ptr, argstr) != SFR_SHARE)
     return;

   if (is_destructed(DR_UNIT, ch))
     return;

   if (CHAR_LEVEL(CHAR_ORIGINAL(ch)) < cmd_ptr->minimum_level )
   {
      if (cmd_ptr->minimum_level >= 200)
	send_to_char("Arglebargle, glop-glyf!?!\n\r", ch);
      else
	send_to_char("Sorry, this command is not available at your level.\n\r",
		     ch);
      return;
   }

   if (CHAR_POS(ch) < cmd_ptr->minimum_position)
   {
      wrong_position(ch);
      return;
   }

   if (cmd_ptr->log_level)
     slog(LOG_ALL, MAX(CHAR_LEVEL(ch), cmd_ptr->log_level), "CMDLOG %s: %s %s",
	  UNIT_NAME(ch), cmd_ptr->cmd_str, argstr);

   if (cmd_ptr->tmpl)
   {
      struct dilprg *prg;

      prg = dil_copy_template(cmd_ptr->tmpl, ch, NULL);
      prg->waitcmd = WAITCMD_MAXINST - 1; // The usual hack, see db_file

      prg->sp->vars[0].val.string  = str_dup(argstr);

      dil_activate(prg);
   }
   else if (cmd_ptr->cmd_fptr)
     ((*cmd_ptr->cmd_fptr) (ch, argstr, cmd_ptr));
   else
   {
      if (IS_MORTAL(ch))
	send_to_char("Arglebargle, glop-glyf!?!\n\r", ch);
      else
	send_to_char("Sorry, that command is not yet implemented...\n\r", ch);
   }
}

int char_is_playing(struct unit_data *u)
{
   return UNIT_IN(u) != NULL;
}

int descriptor_is_playing(struct descriptor_data *d)
{
   return d && d->character && char_is_playing(d->character);
}

void descriptor_interpreter(struct descriptor_data *d, char *arg)
{
   assert(d->character);
   command_interpreter(d->character, arg);
}


/* Check to see if the full command was typed */
ubit1 cmd_is_abbrev(struct unit_data *ch, const struct command_info *cmd)
{
  return CHAR_DESCRIPTOR(ch)
    && str_ccmp(CHAR_DESCRIPTOR(ch)->last_cmd, cmd->cmd_str);
}


/* To check for commands by string */
ubit1 is_command(const struct command_info *cmd, const char *str)
{
   if ((cmd->no == CMD_AUTO_UNKNOWN) || (cmd->no == CMD_A_SOCIAL))
   {
      return cmd->cmd_str && is_abbrev(cmd->cmd_str, str);
   }
   else
   {
      struct command_info *cmd_ptr =
	(struct command_info *) search_trie(str, intr_trie);
      
      return cmd_ptr && cmd->no == cmd_ptr->no;
   }
}


void argument_interpreter(const char *argument,
			  char *first_arg, char *second_arg )
{
   argument = (char *) one_argument(argument, first_arg);

   one_argument(argument, second_arg);
}



int function_activate(struct unit_data *u, struct spec_arg *sarg)
{
   if ((u != sarg->activator) ||
       IS_SET(sarg->fptr->flags, SFB_AWARE) ||
       IS_SET(sarg->mflags, SFB_AWARE))
   {
      if (IS_SET(sarg->fptr->flags, sarg->mflags & ~SFB_AWARE))
      {
#ifdef DEBUG_HISTORY
	 add_func_history(u, sarg->fptr->index, sarg->mflags);
#endif
	 if (unit_function_array[sarg->fptr->index].func)
	   return (*(unit_function_array[sarg->fptr->index].func)) (sarg);
	 else
	   slog(LOG_ALL, 0,"Interpreter: Null function call!");
      }
   }
   return SFR_SHARE;
}

/* u is the owner of the function on which the scan is performed */
/* This function sets the 'sarg->fptr' and 'sarg->owner'         */

int unit_function_scan(struct unit_data *u, struct spec_arg *sarg)
{
   int res = SFR_SHARE;
   int priority = 0;
   struct unit_fptr *next;

   if (g_cServerConfig.m_bNoSpecials)
     return SFR_SHARE;

   sarg->owner = u;

   if (is_destructed(DR_UNIT, u))
     return SFR_SHARE;

   if (IS_PC(u) && !char_is_playing(u))
     return SFR_SHARE;
   
   for (sarg->fptr = UNIT_FUNC(u); !priority && sarg->fptr;
 	    sarg->fptr = next)
   {
      next = sarg->fptr->next;   /* Next dude trick */

      res = function_activate(u, sarg);

      if (res != SFR_SHARE)
	return res;

      priority |= IS_SET(sarg->fptr->flags, SFB_PRIORITY);
   }

   return SFR_SHARE;
}



/* TODO: Specials may cause movement of one unit to another unit.
   All next-dude tricks below are therefore not good enough,
   they ought to check if the nextdude is still in the same
   location before activating it ?! */

/* Explanation of the spec_arg stuff:

   owner is set by unit_function_scan and points to the unit which has
   been activated (self / this)

   [activator] the unit which caused this message.
   [medium]    the unit which was used as a medium of the operation.
   [target]    the unit which was the target of the operation.
   [pInt]      possible value which was used in the operation.

   This function sets the sarg->mflags to equal mflt.
   if extra_target is set, then also send message to that unit
*/

int basic_special(struct unit_data *ch, struct spec_arg *sarg, ubit16 mflt,
		  struct unit_data *extra_target)
{
   register struct unit_data *u, *uu, *next, *nextt;

   sarg->mflags = mflt;

   if (extra_target && !same_surroundings(ch, extra_target))
     if ( (unit_function_scan(extra_target, sarg)) != SFR_SHARE)
       return SFR_BLOCK;

   /* special in room? */
   if (UNIT_FUNC(UNIT_IN(ch)))
   {
      if ( (unit_function_scan(UNIT_IN(ch), sarg)) != SFR_SHARE)
	return SFR_BLOCK;
   }

   /* special in inventory or equipment? */
   for (u = UNIT_CONTAINS(ch); u; u = next)
   {
      next = u->next;  /* Next dude trick */
      if (UNIT_FUNC(u) &&
	  (unit_function_scan(u, sarg)) != SFR_SHARE)
	return SFR_BLOCK;
   }

   /* special in room present? */
   for (u = UNIT_CONTAINS(UNIT_IN(ch)); u; u = next)
   {
      next = u->next;  /* Next dude trick */

      if (UNIT_FUNC(u) &&
	  (unit_function_scan(u, sarg)) != SFR_SHARE)
	return SFR_BLOCK;

      if (u != ch)
      {
	 if (UNIT_IS_TRANSPARENT(u))
	 {
	    for (uu = UNIT_CONTAINS(u); uu; uu=nextt)
	    {
	      nextt = uu->next; /* next dude double trick */
	      if (UNIT_FUNC(uu) &&
		  (unit_function_scan(uu, sarg)) != SFR_SHARE)
		return SFR_BLOCK;
	    }
	 }
	 else if (IS_CHAR(u))
	 {
	   /* in equipment? */
	   for (uu = UNIT_CONTAINS(u); uu; uu = nextt)
	   {
	     nextt = uu->next;  /* Next dude trick */
	     if (UNIT_FUNC(uu) && IS_OBJ(uu) && OBJ_EQP_POS(uu) &&
		 (unit_function_scan(uu, sarg) != SFR_SHARE))
	       return SFR_BLOCK;
	   }
	 }
      }
   }

   /* specials outside room */
   if (UNIT_IS_TRANSPARENT(UNIT_IN(ch)) && UNIT_IN(UNIT_IN(ch)))
   {
      /* special in outside room? */
      if (UNIT_FUNC(UNIT_IN(UNIT_IN(ch))))
      {
	 if (unit_function_scan(UNIT_IN(UNIT_IN(ch)), sarg) != SFR_SHARE)
	   return SFR_BLOCK;
      }

      if (UNIT_IN(UNIT_IN(ch)))
      {
	 for (u = UNIT_CONTAINS(UNIT_IN(UNIT_IN(ch))); u; u = next)
	 {
	    next = u->next;  /* Next dude trick */
	    
	    /* No self activation except when dying... */
	    if (UNIT_FUNC(u) &&
		(unit_function_scan(u, sarg)) != SFR_SHARE)
	      return SFR_BLOCK;
	    
	    if (!UNIT_IN(UNIT_IN(ch)))
	      break;

	    
	    if (u != UNIT_IN(ch))
	    {
	       if (UNIT_IS_TRANSPARENT(u))
	       {
		  for (uu = UNIT_CONTAINS(u); uu; uu = nextt)
		  {
		    nextt = uu->next; /* next dude double trick */
		    if (UNIT_FUNC(uu) &&
			(unit_function_scan(uu, sarg)) != SFR_SHARE)
		      return SFR_BLOCK;
		  }
	       }
	       else if (IS_CHAR(u))
	       {
		  /* in equipment? */
		  for (uu = UNIT_CONTAINS(u); uu; uu = nextt)
		  {
		    nextt = uu->next;  /* Next dude trick */
		    if (UNIT_FUNC(uu) && IS_OBJ(uu) && OBJ_EQP_POS(uu) &&
			(unit_function_scan(uu, sarg) != SFR_SHARE))
		      return SFR_BLOCK;
		  }
	       }
	    }
	 }
      }
   }

   return SFR_SHARE;
}


/* Preprocessed commands */
int send_preprocess(struct unit_data *ch, const struct command_info *cmd,
		    char *arg)
{
   struct spec_arg sarg;

   sarg.activator = ch;
   sarg.medium    = NULL;
   sarg.target    = NULL;
   sarg.pInt      = NULL;
   sarg.cmd       = (struct command_info *) cmd;
   sarg.arg       = arg;

   return basic_special(ch, &sarg, SFB_CMD);
}


int send_message(struct unit_data *ch, char *arg)
{
   struct spec_arg sarg;

   sarg.activator = ch;
   sarg.medium    = NULL;
   sarg.target    = NULL;
   sarg.pInt      = NULL;
   sarg.cmd       = &cmd_auto_msg;
   sarg.arg       = arg;

   return basic_special(ch, &sarg, SFB_MSG);
}



int send_death(struct unit_data *ch)
{
   struct spec_arg sarg;

   sarg.activator = ch;
   sarg.medium    = NULL;
   sarg.target    = NULL;
   sarg.pInt      = NULL;
   sarg.cmd       = &cmd_auto_death;
   sarg.arg       = "";

   return basic_special(ch, &sarg, SFB_DEAD|SFB_AWARE);
}


int send_combat(struct unit_data *ch)
{
   struct spec_arg sarg;

   sarg.activator = ch;
   sarg.medium    = NULL;
   sarg.target    = NULL;
   sarg.pInt      = NULL;
   sarg.cmd       = &cmd_auto_combat;
   sarg.arg       = "";

   return basic_special(ch, &sarg, SFB_COM);
}

int send_save_to(struct unit_data *from, struct unit_data *to)
{
   struct spec_arg sarg;

   assert(to);
   assert(from);

   sarg.activator = from;
   sarg.medium    = NULL;
   sarg.target    = NULL;
   sarg.pInt      = NULL;
   sarg.fptr      = NULL; /* Set by unit_function_scan */
   sarg.cmd       = &cmd_auto_save;
   sarg.arg       = "";
   sarg.mflags    = SFB_SAVE;

   return unit_function_scan(to, &sarg);
}


int send_ack(struct unit_data *activator, struct unit_data *medium,
	     struct unit_data *target,
	     int *i,
	     const struct command_info *cmd,
	     const char *arg,
	     struct unit_data *extra_target)
{
   struct spec_arg sarg;
   int j = 0;

   sarg.activator = activator;
   sarg.medium    = medium;
   sarg.target    = target;

   if (i)
     sarg.pInt      = i;
   else
     sarg.pInt      = &j;

   sarg.cmd       = (struct command_info *) cmd;
   sarg.arg       = (char *) arg;

   return basic_special(activator, &sarg, SFB_PRE, extra_target);
}


void send_done(struct unit_data *activator, struct unit_data *medium,
	       struct unit_data *target, int i, const struct command_info *cmd,
	       const char *arg, struct unit_data *extra_target)
{
   struct spec_arg sarg;

   sarg.activator = activator;
   sarg.medium    = medium;
   sarg.target    = target;
   sarg.pInt      = &i;
   sarg.cmd       = (struct command_info *) cmd;
   sarg.arg       = (char *) arg;

   basic_special(activator, &sarg, SFB_DONE, extra_target);
}


static void read_command_file(void)
{
   char cmd[256], *c;
   struct command_info *cmd_ptr;
   FILE *f;
   char Buf[200];
   int i;

   touch_file(str_cc(DFLT_DIR, COMMAND_FILE));

   f = fopen(str_cc(DFLT_DIR, COMMAND_FILE), "rb");
   if (f == NULL)
   {
      slog(LOG_ALL, 0, "Open error on file %s/%s", DFLT_DIR, COMMAND_FILE);
      exit(0);
   }

   while (!feof(f))
   {
      char *ms2020 = fgets(Buf, sizeof(Buf)-1, f);

      if (!str_is_empty(Buf) && Buf[0] != '#')
      {
	 c = Buf;
	 c = str_next_word(Buf, cmd);
	 
	 if ((cmd_ptr = (struct command_info *) search_trie(cmd, intr_trie)))
	 {
	    c = str_next_word(c, cmd);
	    i = atoi(cmd);

	    if (cmd_ptr->minimum_level >= 200 && !is_in(i, 200, 255))
	      slog(LOG_ALL,0,"Error: command %s set to non-immortals!",  Buf);
	    else
	      cmd_ptr->minimum_level = i;

	    c = str_next_word(c, cmd);
	    cmd_ptr->log_level = atoi(cmd);
	 }
	 else
	 {
	    slog(LOG_ALL, 0, "Unknown command setup : %s", Buf);
	 }
      }
   }

   fclose(f);
}

/* Build the trie here :) */
void assign_command_pointers(void)
{
   int i;

   intr_trie = 0;

   for (i = 0; *cmd_info[i].cmd_str; i++)
     intr_trie = add_trienode(cmd_info[i].cmd_str, intr_trie);

   qsort_triedata(intr_trie);

   for (i = 0; *cmd_info[i].cmd_str; i++)
     set_triedata(cmd_info[i].cmd_str, intr_trie, &cmd_info[i], FALSE);

  read_command_file();

  cmd_follow = (struct command_info *) search_trie("follow", intr_trie);
}

void interpreter_dil_check(void)
{
   for (int i=0; *(cmd_info[i].cmd_str); i++) //MS2020 bug, missing *
   {
      if (cmd_info[i].tmpl == NULL)
	continue;

      if (cmd_info[i].tmpl->argc != 1)
      {
	 slog(LOG_ALL, 0, "Interpreter DIL %s expected 1 argument.",
	      cmd_info[i].tmpl->prgname);
	 cmd_info[i].tmpl = NULL;
	 continue;
      }

      if (cmd_info[i].tmpl->argt[0] != DILV_SP)
      {
	 slog(LOG_ALL, 0, "Interpreter DIL %s argument 1 mismatch.",
	      cmd_info[i].tmpl->prgname);
	 cmd_info[i].tmpl = NULL;
	 continue;
      }
   }
}

void boot_interpreter(void)
{
   for (int i = 0; *cmd_info[i].cmd_str; i++)
   {
      /* What the heck, I'll also initialize un-initialized fields here */
      cmd_info[i].tmpl = NULL;
      cmd_info[i].log_level = 0;
   }
}
