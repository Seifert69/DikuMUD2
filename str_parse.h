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
int parse_match_num(char **pData, const char *pMatch, int *pResult);
int *parse_match_numlist(char **pData, const char *pMatch, int *count);
char *parse_match_name(char **pData, const char *pMatch);
char **parse_match_namelist(char **pData, const char *pMatch);

#endif
