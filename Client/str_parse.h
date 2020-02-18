/* *********************************************************************** *
 * File   : str_parse.h                               Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Headers.                                                       *
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

#ifndef _MUD_STR_PARSE_H
#define _MUD_STR_PARSE_H

char *parse_match(char *pData, char *pMatch);
char *parse_name(char **pData);
int *parse_numlist(char **pData, int *count);
int parse_num(char **pData, int *pNum);
char **parse_namelist(char **pData);
int parse_match_num(char **pData, char *pMatch, int *pResult);
int *parse_match_numlist(char **pData, char *pMatch, int *count);
char *parse_match_name(char **pData, char *pMatch);
char **parse_match_namelist(char **pData, char *pMatch);

// From textutil
ubit1 str_is_empty(const char *arg);
int isfilename(char *fname);
char *str_dup(register const char *source);
char **add_name(const char *name, char **namelist);
char **del_name(const char *name, char **namelist);
void free_namelist(char **list);
char **create_namelist(void);
char *str_cc(const char *s1, const char *s2);
char *skip_blanks(const char *string);
char *str_cstr(register const char *cs, register const char *ct);

#endif
