/* *********************************************************************** *
 * File   : diltok.h                                  Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : ???                                                            *
 *                                                                         *
 * Purpose: Tokens used by dil                                             *
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

/* Wed Jan 22 14:57:30 PST 1997   HHS added paycheck dilfun DILSE_PCK */

#ifndef _MUD_DILTOK_H
#define _MUD_DILTOK_H

struct symbols
{
   const char *nam;
   int val;
} sym[] = {

   /* DIL Symbol Code */
   {"dilbegin",DILSC_BEG},
   {"var",DILSC_VAR},
   {"code",DILSC_COD},
   {"external",DILSC_EXT},
   {"dilend",DILSC_END},
   {"external",DILSC_EXT},
   {"recall", DILSC_REC},
   {"aware", DILSC_AWA},

   /* DIL Symbol Expr (functions) */

   {"spellindex", DILSE_SPLX},
   {"spellinfo", DILSE_SPLI},
   {"moneystring", DILSE_MONS},
   {"pathto", DILSE_PATH},
   {"can_carry", DILSE_CARY},
   {"fits", DILSE_FIT},
   {"restore", DILSE_REST},
   {"openroll", DILSE_OPRO},
   {"equipment", DILSE_EQPM},
   {"meleeattack", DILSE_MEL},
   {"cast_spell", DILSE_CST},
   {"interrupt",DILSE_INTR},
   {"clear",DILSI_CLI},
   {"and",DILSE_LAND},
   {"or",DILSE_LOR},
   {"not", DILSE_NOT},
   {"atoi",DILSE_ATOI},
   {"itoa",DILSE_ITOA},
   {"rnd",DILSE_RND},
   {"dildestroy",DILSE_DLD},
   {"dilfind",DILSE_DLF},
   {"findunit",DILSE_FNDU},
   {"findsymbolic",DILSE_FNDS},
   {"findroom",DILSE_FNDR},
   {"findrndunit",DILSE_FNDRU},
   {"load",DILSE_LOAD},
   {"isset",DILSE_ISS},
   {"isaff", DILSE_ISA},
   {"in",DILSE_IN},
   {"getword",DILSE_GETW},
   {"getwords",DILSE_GETWS},
   {"length",DILSE_LEN},
   {"command",DILSE_CMDS},
   {"skip", DILSE_SKIP},
   {"null",DILSE_NULL},
   {"self",DILSE_SELF},
   {"activator",DILSE_ACTI},
   {"medium",DILSE_MEDI},
   {"target",DILSE_TARG},
   {"power",DILSE_POWE},
   {"argument",DILSE_ARGM},
   {"cmdstr",DILSE_CMST},
   {"heartbeat",DILSE_HRT},
   {"mudday",DILSE_TDA},
   {"mudhour",DILSE_THO},
   {"mudmonth",DILSE_TMD},
   {"mudyear",DILSE_TYE},
   {"realtime",DILSE_RTI},
   {"weather",DILSE_WEAT},
   {"attack_spell", DILSE_ATSP},
   {"textformat", DILSE_TXF},
   {"asctime", DILSE_AST},
   {"paycheck", DILSE_PCK},

   /* DIL Symbol Inst (procedures) */

   {"follow",DILSI_FOLO},
   {"logcrime",DILSI_LCRI},

   {"sendtext",DILSI_SETE},
   {"store",DILSI_STOR},
   {"set_fighting",DILSI_SETF},
   {"change_speed",DILSI_CHAS},
   {"walkto",DILSI_WLK},
   {"setweight",DILSI_SWT},
   {"setbright",DILSI_SBT},
   {"dilcopy",DILSI_DLC},
   {"return",DILSI_RTS},
   {"log", DILSI_LOG},
   {"sendtoall",DILSI_SNTA},
   {"sendtoalldil",DILSI_SNTADIL},
   {"position_update",DILSI_PUP},
   {"wait",DILSI_WIT},
   {"while",DILSI_WHI},
   {"on",DILSI_ON},
   {"if",DILSI_IF},
   {"else",DILSI_ELS},
   {"act",DILSI_ACT},
   {"goto",DILSI_GOT},
   {"block",DILSI_BLK},
   {"priority",DILSI_PRI},
   {"nopriority",DILSI_NPR},
   {"quit",DILSI_QUIT},
   {"link",DILSI_LNK},
   {"experience", DILSI_EXP},
   {"set",DILSI_SET},
   {"unset",DILSI_UST},
   {"addstring",DILSI_ADL},
   {"substring",DILSI_SUL},
   {"addextra",DILSI_ADE},
   {"subextra",DILSI_SUE},
   {"destroy",DILSI_DST},
   {"exec",DILSI_EXE},
   {"send",DILSI_SND},
   {"sendto",DILSI_SNT},
   {"secure",DILSI_SEC},
   {"unsecure",DILSI_USE},
   {"on_activation",DILSI_OAC},
   {"subaff", DILSI_SUA},
   {"addaff", DILSI_ADA},
   {"addequip", DILSI_EQP},
   {"unequip", DILSI_UEQ},
   {"for", DILSI_FOR},
   {"foreach", DILSI_FOE},
   {"break", DILSI_BRK},
   {"continue", DILSI_CNT},
   {"acc_modify",DILSI_AMOD},
   
   /* DIL Symbol Field */
   {"info", DILSF_INFO},

   {"lifespan", DILSF_LSA},

   {"acc_balance", DILSF_ABAL},
   {"acc_total",   DILSF_ATOT},
   
   {"speed", DILSF_SPD},
   {"loadcount", DILSF_LCN},
   {"master", DILSF_MAS},
   {"follower", DILSF_FOL},
   {"zoneidx",DILSF_ZOI},
   {"nameidx",DILSF_NMI},
   {"idx",DILSF_IDX},
   {"type",DILSF_TYP},
   {"next",DILSF_NXT},
   {"names",DILSF_NMS},
   {"name",DILSF_NAM},
   {"outside_descr",DILSF_ODES},
   {"inside_descr",DILSF_IDES},
   {"descr",DILSF_DES},
   {"title",DILSF_TIT},
   {"extra",DILSF_EXT},
   {"outside",DILSF_OUT},
   {"inside",DILSF_INS},
   {"gnext",DILSF_GNX},
   {"gprevious",DILSF_GPR},
   {"light",DILSF_LGT},
   {"bright",DILSF_BGT},
   {"minv",DILSF_MIV},
   {"illum",DILSF_ILL},
   {"flags",DILSF_FL},
   {"manipulate",DILSF_MAN},
   {"openflags",DILSF_OFL},
   {"max_hp",DILSF_MHP},
   {"max_mana",DILSF_MMA},
   {"max_endurance",DILSF_MED},
   {"hp",DILSF_CHP},
   {"baseweight",DILSF_BWT},
   {"weight",DILSF_WGT},
   {"capacity",DILSF_CAP},
   {"alignment",DILSF_ALG},
   {"spells",DILSF_SPL},
   {"hasfunc",DILSF_FUN},
   {"zone",DILSF_ZON},

   {"visible",DILSE_VISI},
   {"opponent",DILSE_OPPO},
   {"purse",DILSE_PURS},
   {"transfermoney",DILSE_TRMO},

   {"objecttype",DILSF_OTY},
   {"value",DILSF_VAL},
   {"objectflags",DILSF_EFL},
   {"cost",DILSF_CST},
   {"rent",DILSF_RNT},
   {"equip",DILSF_EQP},

   {"exit_names",DILSF_ONM},
   {"exit_info",DILSF_XNF},
   {"exit_to",DILSF_TOR},
   {"roomflags",DILSF_RFL},
   {"movement",DILSF_MOV},

   {"sex",DILSF_SEX},
   {"race",DILSF_RCE},
   {"abilities",DILSF_ABL},
   {"exp",DILSF_EXP},
   {"level",DILSF_LVL},
   {"height",DILSF_HGT},
   {"position",DILSF_POS},
   {"attack_type",DILSF_ATY},
   {"mana",DILSF_MNA},
   {"endurance",DILSF_END},
   {"charflags",DILSF_AFF},
   {"dex_red",DILSF_DRE},
   {"fighting",DILSF_FGT},
   {"weapons",DILSF_WPN},

   {"defaultpos",DILSF_DEF},
   {"npcflags",DILSF_ACT},

   {"birth", DILSF_BIR},
   {"playtime",DILSF_PTI},
   {"crimes",DILSF_CRM},
   {"full",DILSF_FLL},
   {"thirst",DILSF_THR},
   {"drunk",DILSF_DRK},
   {"skill_points",DILSF_SPT},
   {"ability_points",DILSF_APT},
   {"guild",DILSF_GLD},
   {"quests",DILSF_QST},
   {"skills",DILSF_SKL},
   {"pcflags",DILSF_PCF},
   {"hometown",DILSF_HOME},

   /* DIL Symbol Type */
   {"unitptr",DILST_UP},
   {"integer",DILST_INT},
   {"extraptr",DILST_EDP},
   {"string",DILST_SP},
   {"stringlist",DILST_SLP},
   {0,0}
  };

#endif /* _MUD_DILTOK_H */
