/* *********************************************************************** *
 * File   : network.cc                                Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : Unknown.                                                       *
 *                                                                         *
 * Purpose: Unknown.                                                       *
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
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include "essential.h"
#include "network.h"
#include "log.h"

extern cLog *pLogFile;

int OpenNetwork(char *pcAddress, int nPort, struct sockaddr_in *server_addr)
{
   int fdClient;
   int n;

   memset((char *) server_addr, 0, sizeof(struct sockaddr_in));

   server_addr->sin_family         = AF_INET;
   server_addr->sin_addr.s_addr    = inet_addr(pcAddress);
   server_addr->sin_port           = htons(nPort);

   fdClient = socket(AF_INET, SOCK_STREAM, 0);

   if (fdClient == -1)
   {
      pLogFile->Log("No unix stream connection.");
      return -1;
   }

   n = connect(fdClient,
	       (struct sockaddr *) server_addr,
	       sizeof(struct sockaddr_in));

   if (n == -1)
   {
      close(fdClient);
      pLogFile->Log("No connect allowed.");
      return -1;
   }

   n = fcntl(fdClient, F_SETFL, FNDELAY);
   if (n == -1)
   {
      close(fdClient);
      pLogFile->Log("Non blocking set error.");
      exit(1);
   }

   return fdClient;
}

int TransmitChunk(int fd, char *pChunk, int nSize)
{
   int n;

   while (nSize > 0)
   {
      n = write(fd, pChunk, nSize);

      if (n < 0)
	return -1;

      nSize  -= n;
      pChunk += n;
      if (nSize > 0)
	usleep(1);
   }
   return 0;
}


int ReceiveChunk(int fd, char *pChunk, int nSize)
{
   int n;
   int nRead;

   for (nRead = 0; nRead < nSize;)
   {
      n = read(fd, pChunk, nSize - nRead);

      if (n > 0)
      {
	 pChunk += n;
	 nRead += n;
	 continue;
      }

      if (errno == EWOULDBLOCK)
	break;

      if (n == 0)
	break;
      else /* n < 0 */
      {
	 pLogFile->Log("Error reading from MUD socket (%d).\n", errno);
	 return -1;
      }
   }

   pChunk[0] = 0;
   return nRead;
}

int CloseNetwork(int fd)
{
   int n;
   n = close(fd);
   if (n == -1)
   {
      pLogFile->Log("ERROR: Closing descriptor.");
      return -1;
   }
   return 0;
}
