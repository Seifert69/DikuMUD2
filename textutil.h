/* *********************************************************************** *
 * File   : textutil.h                                Part of Valhalla MUD *
 * Version: 1.02                                                           *
 * Author : all                                                            *
 *                                                                         *
 * Purpose: String related headers.                                        *
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

#ifndef _MUD_TEXTUTIL_H
#define _MUD_TEXTUTIL_H

#define isaspace(c)  ((c) == ' ')

int str_escape_format(const char *str, char *dest, int destlen, int format = TRUE);
char *str_escape_format(const char *src, int formatting = TRUE);

char *text_read_line(char *buf, char *res);
char *text_skip_line(char *buf);
char *catnames(char *s, const char **names);
void str_insert(char *str, char *itxt);
void str_substitute(const char *old, const char *newstr, char *str);
void str_rem(char *s, char c);
void str_rem_punct(char *s);
void str_blank_punct(char *s);
void str_remspc(register char *s);
void str_chraround(char *str, char c);
 
char *is_name_raw(register const char *arg, register const char *names[]);
char *is_name(const char *arg, const char *names[]);

int str_lower(char *s);
int str_lower(const char *s, char *d, int nBufSize);

char *spc(int n);
char *str_line(const char *, char *);
#ifdef DOS
#define itoa(n) itoa_dos(n)
char *itoa_dos(int n);
#else
char *itoa(int n);
#endif
char *ltoa(long n);
int str_ccmp(const char *s, const char *d);
int str_nccmp(const char *s, const char *d, int n);
char *str_dup(const char *source);
char *str_str(const char *cs, const char *ct);
char *str_cstr(const char *cs, const char *ct);
ubit1 str_is_empty(const char *arg);
ubit1 str_is_number(const char *str);
ubit1 next_word_is_number(const char *str);
char *one_argument(const char *argument, char *first_arg);
int search_block_abbrevs(const char *oarg, const char **list, const char **end);
int search_block(const char *arg, const char **list, ubit1 exact);
int search_block_length(const char *arg, int length,
			const char **list, ubit1 exact);
int fill_word(const char *argument);
ubit1 is_abbrev(const char *arg1, const char *arg2);
char *str_cc(const char *s1, const char *s2);
char **add_name(const char *name, char **namelist);
char **del_name(const char *name, char **namelist);
char **create_namelist(void);
char **namestring_to_namelist(const char *str);
void free_namelist(char **list);
char *str_ccmp_next_word(const char *buf, const char *next_word);
char **copy_namelist(const char **source);
char *str_next_word_copy(const char *argument, char *first_arg);
char *str_next_word(const char *argument, char *first_arg);
void split_fi_ref(const char *str, char *zone, char *name);
char *skip_spaces(const char *);
char *skip_blanks(const char *string);
void strip_trailing_spaces(char *str);
void strip_trailing_blanks(char *str);

#ifdef toupper
  #undef toupper
#endif
#define toupper(c) ((c) >= 'a' && (c) <= 'z' ? ((c)+('A'-'a')) : (c))

#ifdef tolower
  #undef tolower
#endif
#define tolower(c) ((c) >= 'A' && (c) <= 'Z' ? ((c)+('a'-'A')) : (c))

#define str_delete(str, nDel, nStrLen)  \
   memmove(str, str + nDel, nStrLen - nDel + 1)

#endif /* _MUD_TEXTUTIL_H */
