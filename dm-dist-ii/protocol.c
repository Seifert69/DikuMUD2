/* *********************************************************************** *
 * File   : protocol.c                                Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: The common protocol between server and multi-client-server.    *
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

#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>

#include "essential.h"
#include "protocol.h"
#include "utility.h"
#include "select.h"
#include "db_file.h"
#include "unixshit.h"

// Read as much as we can...
// -1 on error, 0 on ok.
//
int read_to_queue(int fd, cQueue *q)
{
   char buf[1460];
   int sofar = 0, thisround, errors = 0;

   for (;;)
   {
      thisround = read(fd, buf, sizeof(buf));

      if (thisround > 0)
      {
	 q->Append(new cQueueElem((ubit8*)buf, (ubit32) thisround));
      }
      else if (thisround == 0)
      {
	 slog(LOG_ALL, 0,"Read to queue: EOF on socket read (eno %d).",
	      errno);
	 return -1;
      }
      else /* (thisround < 0) */
      {
	 if (errno == EWOULDBLOCK)
	   return 0;

	 slog(LOG_ALL, 0, "Read from socket %d error %d", fd, errno);
	 return -1;
      }
   }

   return 0;
}


/* Create the standard header and send a close request to file             */
/* file descriptor 'fd'. If fd is -1 then don't send anything.             */
/* Return -1 on socket fail, 0 on amount fail, 1 on success                */
void protocol_send_close(cHook *Hook, ubit16 id)
{
   int n;
   ubit16 len = 0;
   ubit8 buf[20];

   if (!Hook->IsHooked())
     return;

   memcpy(&(buf[0]), MULTI_TERMINATE, 2);
   memcpy(&(buf[2]), &id, sizeof(id));
   memcpy(&(buf[4]), &len, sizeof(len));
   
   Hook->Write(buf, 6);
}


/* Create the standard header and send a connection confirm to             */
/* file descriptor 'fd'. If fd is -1 then don't send anything.             */
/* Return -1 on socket fail, 0 on amount fail, 1 on success                */
void protocol_send_confirm(cHook *Hook, ubit16 id)
{
   int n;
   ubit16 len = 0;
   ubit8 buf[20];
   
   if (!Hook->IsHooked())
     return;

   memcpy(&(buf[0]), MULTI_CONNECT_CON, 2);
   memcpy(&(buf[2]), &id, sizeof(id));
   memcpy(&(buf[4]), &len, sizeof(len));
   
   Hook->Write(buf, 6);
}


/* Create the standard header and send a connection request to             */
/* file descriptor 'fd'. If fd is -1 then don't send anything.             */
/* Return -1 on socket fail, 0 on amount fail, 1 on success                */
void protocol_send_request(cHook *Hook)
{
   int n;
   ubit16 id = 0, len = 0;
   ubit8 buf[10];

   if (!Hook->IsHooked())
     return;

   memcpy(&(buf[0]), MULTI_CONNECT_REQ, 2);
   memcpy(&(buf[2]), &id, sizeof(id));
   memcpy(&(buf[4]), &len, sizeof(len));

   Hook->Write(buf, 6);
}


/* Create the standard header and send (from mplex to server) the host     */
/* name information.     If fd is -1 then don't send anything.             */
/* Return -1 on socket fail, 0 on amount fail, 1 on success                */
void protocol_send_host(cHook *Hook, ubit16 id, char *host,
			ubit16 nPort, ubit8 nLine)
{
   char bufms[MAX_STRING_LENGTH] = "UNKNOWN HOST"; //MS2020
   int n;
   ubit16 len = 0;
   ubit8 buf[80];
   char *ptext = NULL; //MS2020
   ubit8 *b;

   if (!Hook->IsHooked())
     return;

   strncpy(bufms, host, sizeof(bufms)-1); //MS2020
   ptext = bufms; //MS2020
   
   if (host)
   {
      ptext = host;
      ptext[49] = 0;
   }

   len = strlen(ptext) + 4;

   b = buf;

   memcpy(b, MULTI_HOST, 2);
   b += 2;

   bwrite_ubit16(&b, id);
   bwrite_ubit16(&b, len);
   bwrite_ubit16(&b, nPort);
   bwrite_ubit8 (&b, nLine);
   bwrite_string(&b, ptext);

   Hook->Write(buf, b - buf);
}

/* Create the standard header and send text to                              */
/* file descriptor 'fd'. If fd is -1 then don't send anything.              */
/* 'type' is used to tell difference between normal text, page string text, */
/* etc.                                                                     */
/* Return -1 on socket fail, 0 on amount fail, 1 on success                 */
void protocol_send_text(cHook *Hook, const ubit16 id,
			const char *text, const ubit8 type)
{
#define MAX_TEXT_LEN (4*1460-6)

   int n;
   ubit16 len, txlen;
   ubit8 buf[6 + MAX_TEXT_LEN];

   assert(id != 0);
   
   if (!Hook->IsHooked())
     return;

   len = strlen(text) + 1;

   txlen = MIN(MAX_TEXT_LEN, len);
   
   if (txlen < len)
   {
      /* Fragmented text, break it up at a newline for
	 indentation purposes! (if possible) */

      for (; txlen != 0; txlen--)
	if (ISNEWL(text[txlen-1]))
	  break;

      if (txlen == 0)
	txlen = MIN(MAX_TEXT_LEN, len);
   }

   buf[0] = MULTI_UNIQUE_CHAR;
   buf[1] = type;
   memcpy(&(buf[2]), &id, sizeof(id));
   memcpy(&(buf[4]), &txlen, sizeof(txlen));
   memcpy(&(buf[6]), text, txlen);

   assert(txlen > 0);

   Hook->Write(buf, 6 + txlen);

   len -= txlen;

   if (len > 0)
     protocol_send_text(Hook, id, &text[txlen], type);
#undef MAX_TEXT_LEN
}


/* Create the standard header and send setup information from the server   */
/* to the mplex'er.                                                        */
/* file descriptor 'fd'. If fd is -1 then don't send anything.             */
/* Return -1 on socket fail, 0 on amount fail, 1 on success                */

void protocol_send_setup(cHook *Hook, ubit16 id,
			 struct terminal_setup_type *setup)
{
   ubit16 len;
   ubit8 buf[sizeof(struct terminal_setup_type) + 6 + 4];

   assert(id != 0);

   if (!Hook->IsHooked())
     return;

   len = sizeof(struct terminal_setup_type);

   memcpy(&(buf[0]), MULTI_SETUP, 2);
   memcpy(&(buf[2]), &id, 2);
   memcpy(&(buf[4]), &len, 2);
   memcpy(&(buf[6]), setup, sizeof(struct terminal_setup_type));

   Hook->Write(buf, 6 + len);
}



/* Data is assumed ready on 'fd' and it is interpreted. Any of the three   */
/* pointers can be set to NIL if desired.                                  */
/*                                                                         */
/* fd        : Socket to read from.                                        */
/* pid       : Will be set to the destination id.                          */
/* plen      : The length of what ever data is received.                   */
/* str       : Set to point to the malloced data (if any)                  */
/*                                                                         */
/* Returns                                                                 */
/*       0 if there is nothing more to do (i.e. queue empty)               */
/*      -1 on socket fail                                                  */
/*      -2 on protocol fail                                                */
/* or type on success                                                      */
/*                                                                         */
int protocol_parse_incoming(cHook *Hook, ubit16 *pid,
			     ubit16 *plen, char **str,
			     ubit8 *text_type)
{
   int n;
   ubit16 id;
   ubit16 len;
   char buf[10];
   char *data;

   if (str)
     *str = NULL;

   if (!Hook->IsHooked())
     return 0;

   n = read_to_queue(Hook->tfd(), &Hook->qRX);

   if (n == -1)
   {
      slog(LOG_ALL, 0, "Protocol: parse_incoming error.");
      return -1;
   }

   if (Hook->qRX.Bytes() < 6)
     return 0;

   Hook->qRX.Copy((ubit8 *) buf, 6);

   if (buf[0] != MULTI_UNIQUE_CHAR)
   {
      slog(LOG_ALL, 0, "Illegal unexpected unique multi character.");
      return -2;
   }

   memcpy(&id,  &(buf[2]), sizeof(ubit16));
   memcpy(&len, &(buf[4]), sizeof(sbit16));

   if (Hook->qRX.Bytes() - 6 < len)
   {
      // slog(LOG_ALL, 0, "Short of data...");      
      return 0; /* We havn't got all the data yet! */
   }

   Hook->qRX.Cut(6);
   
   if (pid)
     *pid = id;

   if (plen)
     *plen = len;
   
   switch(buf[1])
   {
     case MULTI_TERMINATE_CHAR:
      if (id == 0)
      {
	 slog(LOG_ALL, 0, "Received ID zero on a terminate request!");
	 return -2;
      }
      return buf[1];


     case MULTI_CONNECT_CON_CHAR:
      if (id == 0)
      {
	 slog(LOG_ALL, 0, "ID 0 on connection confirm.");
	 return -2;
      }
      return buf[1];
 
     case MULTI_CONNECT_REQ_CHAR:
      if (id != 0)
      {
	 slog(LOG_ALL, 0, "Received non-zero ID on a connection request!");
	 return -2;
      }
      return buf[1];

     case MULTI_HOST_CHAR:
      if (id == 0)
      {
	 slog(LOG_ALL, 0, "Received zero ID on a host name transfer!");
	 return -2;
      }
      if (len <= 0)
      {
	 slog(LOG_ALL, 0, "Received 0 length host information.");
	 return -2;
      }
      break; /* Get text */

     case MULTI_SETUP_CHAR:
      if (id == 0)
      {
	 slog(LOG_ALL, 0, "Received setup from ID zero!");
	 return -2;
      }
      break; /* Get data */

 
     case MULTI_TEXT_CHAR:
     case MULTI_PAGE_CHAR:
     case MULTI_PROMPT_CHAR:
      if (id == 0)
	slog(LOG_ALL, 0, "Received text from ID zero!");
      break; /* Get text */

     default:
      slog(LOG_ALL, 0, "Illegal unexpected unique multi character.");
      return -2;
   }

   /* Get extra data into "text" buffer */
   assert(len > 0);

   CREATE(data, char, len + 1);

   Hook->qRX.CutCopy((ubit8 *) data, len);

   data[len] = 0;

   *str = data;
   
   return buf[1];
}
