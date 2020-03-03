/* *********************************************************************** *
 * File   : network.c                                 Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert                                                        *
 *                                                                         *
 * Purpose: Basic Internet / Stream opening                                *
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/tcp.h>

#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <fcntl.h>
#include <errno.h>

#include "network.h"
#include "protocol.h"
#include "textutil.h"
#include "unixshit.h"

#if defined(MARCEL) || defined(AMIGA)
#include <machine/endian.h>
#endif


int OpenMother(int nPort)
{
   int n, fdMother;
   struct linger ld;
   struct sockaddr_in server_addr;

   memset(&server_addr, 0, sizeof(struct sockaddr_in));
   server_addr.sin_family      = AF_INET;
   server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   server_addr.sin_port        = htons(nPort);

   fdMother = socket(AF_INET, SOCK_STREAM, 0);

   if (fdMother == -1)
   {
      slog(LOG_OFF, 0, "Can't open Mother Connection");
      exit(1);
   }

   n = fcntl(fdMother, F_SETFL, FNDELAY);

   if (n == -1)
   {
      close(fdMother);
      slog(LOG_OFF, 0, "Non blocking set error.");
      exit(1);
   }

   n = 1;
   if (setsockopt(fdMother,SOL_SOCKET,SO_REUSEADDR,(char *) &n, sizeof(n)) < 0)
   {
      close(fdMother);
      slog(LOG_OFF, 0, "setsockopt REUSEADDR");
      exit(1);
   }

   ld.l_onoff = 0;
   ld.l_linger = 1000;

   if (setsockopt(fdMother,SOL_SOCKET,SO_LINGER, (char *)&ld, sizeof(ld)) < 0)
   {
      close(fdMother);
      slog(LOG_OFF, 0, "setsockopt LINGER");
      exit(1);
   }

   n = bind(fdMother, (struct sockaddr *) &server_addr,
            sizeof(struct sockaddr_in));

   if (n != 0)
   {
      close(fdMother);
      slog(LOG_OFF, 0, "Can't bind Mother Connection port %d (errno %d).",
	   nPort, errno);
      exit(1);
   }

   n = listen(fdMother, 5);

   if (n != 0)
   {
      close(fdMother);
      slog(LOG_OFF, 0, "Can't listen on Mother Connection.");
      exit(1);
   }

   return fdMother;
}


int OpenNetwork(int nPort, const char *pcAddress)
{
   struct sockaddr_in server_addr;
   int fdClient;
   int n;

   slog(LOG_ALL, 0, "Open connection to server on %s %d.",
	pcAddress, nPort);

   memset((char *) &server_addr, 0, sizeof(struct sockaddr_in));

   server_addr.sin_family         = AF_INET;
   server_addr.sin_addr.s_addr    = inet_addr(pcAddress);
   server_addr.sin_port           = htons(nPort);

   fdClient = socket(AF_INET, SOCK_STREAM, 0);

   if (fdClient == -1)
   {
      slog(LOG_OFF, 0, "No TCP/IP connection to server.");
      return -1;
   }

   n = connect(fdClient,
	       (struct sockaddr *) &server_addr,
	       sizeof(struct sockaddr_in));

   if (n == -1)
   {
      close(fdClient);
      slog(LOG_OFF, 0, "No connect allowed to server.");
      return -1;
   }

   n = fcntl(fdClient, F_SETFL, FNDELAY);

   if (n == -1)
   {
      close(fdClient);
      slog(LOG_OFF, 0, "Non blocking set error on server connection.");
      return -1;
   }

   int i;
   n = setsockopt(fdClient, IPPROTO_TCP, TCP_NODELAY, &i, sizeof(i));
   if (n == -1)
   {
      close(fdClient);
      slog(LOG_OFF, 0, "Setsockopt TCP_NODELAY failed on server connection.");
      exit(1);
   }
   return fdClient;
}
