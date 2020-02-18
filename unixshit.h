/* *********************************************************************** *
 * File   : unixshit.h                                Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : all                                                            *
 *                                                                         *
 * Purpose: Declarations of undeclared things                              *
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

#ifndef _MUD_UNIXSHIT_H
#define _MUD_UNIXSHIT_H

#ifdef SUNOS4
int vsprintf(char *s, const char *format, va_list ap);
#endif

#ifdef GENERIC_SYSV
#define O_NDELAY FNDELAY
#endif

/* UNIX stinks! I hate all thoses incomplete include files, so I have */
/* to fix them myself, here... :(                                     */
/* May Unix burn in hell (Michael, 1992)                              */
/* This should be.. may Unix vendors burn in hell (Hans H. 1992)      */

/* Mark this day: 27 April, 1995... Linux compiles without this shit,
   guess it should be... burn all commercial unix vendors... */


/* ======================== UNISTD.H ============================= */


#if defined(MARCEL) && defined(SOCK_STREAM)

/* external functions */
/*
int read(int fd, char *buf, int nbyte);
int write(int fd, char *buf, int nbyte);
*/
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
           struct timeval *timeout);
int accept(int s, struct sockaddr *addr, int *addrlen);
int bind(int s, struct sockaddr *name, int namelen);
int getpeername(int s, struct sockaddr *name, int *namelen);
int connect(int s, struct sockaddr *name, int namelen);
int socket(int domain, int type, int protocol);
int listen(int s, int backlog);
int setsockopt(int s, int level, int optname, char *optval, int optlen);
#endif


#if defined(GENERIC_BSD)
int getdtablesize(void);
#endif

/* ======================== STDIO.H ============================= */

#if defined(VAX) 

void perror(const char *s);

#ifndef SEEK_SET
  #define SEEK_SET 0
  #define SEEK_CUR 1
  #define SEEK_END 2
#endif

typedef char *va_list;
int sscanf(const char *s, const char *format, ...);
/* int vsprintf(char *s, const char *format, va_list ap); */
size_t fread(void *ptr, size_t size, size_t nobj, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nobj, FILE *stream);
int ungetc(int c, FILE *stream);

int fseek(FILE *stream, long offset, int origin);
int fclose(FILE *stream);
void rewind(FILE *stream);

int fscanf(FILE *stream, const char *format, ...);
int fprintf(FILE *stream, const char *format, ...);

/* int sprintf(char *s, const char *format, ...); */
int printf(const char *format, ...);

int fflush(FILE *stream);
int fgetc(FILE *stream);
int fputc(int c, FILE *stream);
int fputs(const char *s, FILE *stream);
#endif

#if defined(DOS)

#ifndef SEEK_SET
  #define SEEK_SET 0
  #define SEEK_CUR 1
  #define SEEK_END 2
#endif

#endif

/* ======================== TIME.H ============================= */

#if defined(SUNOS4)

//MS2020: Oddity - char *strchr(const char *cs, int c);
time_t time(time_t *tp);

/*  difftime() is not supported at all under SunOS, so I hacked one together
 *  in sysport.c
 */
double difftime(time_t t1, time_t t2);
#endif

/* ======================== STRING.H ============================= */

#if defined(SUNOS4)
 #ifdef GENERIC_BSD
  void bcopy(char *, char *, int);
  #define memmove(dst, src, len) bcopy((char *)(src),(char *)(dst),(int)(len))
 #endif
#endif

/* ======================== STDLIB.H ============================= */

#if defined(SUNOS4)
int remove(const char *path);
int system(const char *string);
#endif

/* ======================== ???.H ============================= */
#ifdef MARCEL
int usleep(unsigned int);
#endif

#endif /* _MUD_UNIXSHIT_H */
