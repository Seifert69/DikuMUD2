/* *********************************************************************** *
 * File   : mplex.c                                   Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: The host that connects to the server and lets people connect.  *
 *                                                                         *
 * Bugs   : -                                                              *
 *                                                                         *
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
#include <ctype.h>
#include <netdb.h>
#include <assert.h>
#include <signal.h>
#include <arpa/telnet.h>

#include "structs.h"
#include "network.h"
#include "protocol.h"
#include "essential.h"
#include "textutil.h"
#include "ttydef.h"
#include "db.h"
#include "utility.h"
#include "mplex.h"
#include "translate.h"
#include "unixshit.h"
#include "select.h"
#include "common.h"

void mud_went_down(void);
void Idle(cConHook *con, const char *cmd);

int connections_left = 0;

int bHadAlarm = FALSE;

void dumbPlayLoop(cConHook *con, const char *cmd)
{
   con->PlayLoop(cmd);
}

void dumbPressReturn(class cConHook *con, const char *cmd)
{
   con->PressReturn(cmd);
}

void dumbMenuSelect(class cConHook *con, const char *cmd)
{
   con->MenuSelect(cmd);
}

void dumbMudDown(class cConHook *con, const char *cmd)
{
   con->MudDown(cmd);
}


void alarm_check(int i)
{
   if (bHadAlarm)
   {
      slog(LOG_OFF, 0, "Alarm checkpoint %d.", i);
      bHadAlarm = FALSE;
   }
}

class cMotherHook : public cHook
{
  public:
   void Input(int nFlags)
   {
      if (nFlags & SELECT_EXCEPT)
      {
	 slog(LOG_OFF, 0, "MOTHER CONNECTION CLOSED DOWN!");
	 this->Unhook();
      }
      else if (nFlags & SELECT_READ)
      {
	 class cConHook *con = new cConHook();

	 if (connections_left <= 1)
	 {
	    slog(LOG_OFF, 0, "Can't accept more connections.");
	    con->SendCon("This server has no more available connections, try "
		    "another port.\n\n\r");
	 }
	 else
	 {
	    extern ubit32 memory_total_alloc;
	    slog(LOG_OFF, 0, "Connection from [%s] (%d left) (%d bytes "
		 "allocated).",
		 con->m_aHost, connections_left, memory_total_alloc);
	    con->m_pFptr(con, "");
	 }
      }
   }
};


class cMotherHook MotherHook;


class cMudHook : public cHook
{
  public:
   int read_mud(void);

   void Input(int nFlags)
   {
      if (nFlags & SELECT_EXCEPT)
      {
	 slog(LOG_OFF, 0, "MUD CONNECTION CLOSED DOWN!");
	 mud_went_down();
      }
      else if (nFlags & SELECT_READ)
      {
	 int n;

	 for (;;)
	 {
	    n = read_mud();
	    if ((n == 0) || (n == -1))
	      break;
	 }

	 if (n == -1)
	 {
	    slog(LOG_OFF, 0, "ERROR READING FROM MUD.");
	    mud_went_down();
	 }
	 else if (n == -2)
	 {
	    slog(LOG_OFF, 0, "PROTOCOL ERROR.");
	 }
      }
   }
};

class cMudHook MudHook;

struct arg_type
{
   int  nMotherPort;
   int  nMudPort;
   const char *pAddress;
} arg;

#define Assert(a,b) \
   do { if (!(a)) {fprintf(stderr, "%s\n", b); assert(a);} } while (0);

int g_bModeANSI   = FALSE;
int g_bModeEcho   = FALSE;
int g_bModeRedraw = FALSE;
int g_bModeTelnet = FALSE;
int g_bModeFull8bit = FALSE; // this flag adding by prool


class cConHook *connection_list = NULL;

char Outbuf[32768];

void ShowUsage(char *name)
{
   fprintf(stderr, "Usage: %s [-a] [-h] [-c] [-e] [-r] [-t] [-p <num>] [-d <path>] [-s <port>] [-a <address>]\n", name);
   fprintf(stderr, "  -h  This help screen.\n");
   fprintf(stderr, "  -c  ANSI Colour when set, TTY when not.\n");
   fprintf(stderr, "  -e  Echo mode (echo chars locally).\n");
   fprintf(stderr, "  -r  Redraw prompt lcoally (usually only in -e mode).\n");
   fprintf(stderr, "  -t  Telnet mode when set (IAC negotiation).\n");
   fprintf(stderr, "  -p  Player port number, default is 4242.\n");
   fprintf(stderr, "  -a  Internet address of server (localhost default).\n");
   fprintf(stderr, "  -s  Internet port of server (4999 default).\n");
   fprintf(stderr, "  -u  Full 8bit characters for UTF-8, koi8-r, cp1251, etc (by prool)\n");
   exit(0);
}


int ParseArg(int argc, char *argv[], struct arg_type *arg)
{
   int i, n;
   struct hostent *pHostInfo;
   struct in_addr *pAddr;
   char *c;
 
   arg->nMudPort     = 4999; /* Default port */
   arg->nMotherPort  = 4242; /* Default port */
   arg->pAddress     = DEF_SERVER_ADDR;

   for (i=1; i < argc; i++)
   {
      if (argv[i][0] != '-')
      {
	 fprintf(stderr, "Illegal argument '%s'.\n", argv[i]);

	 ShowUsage(argv[0]);
      }

      switch (argv[i][1])
      {
	case 'h':
	case '?':
	 ShowUsage(argv[0]);
	 break;

	case 'c':
	 g_bModeANSI = TRUE;
	 break;

	case 'e':
	 g_bModeEcho = TRUE;
	 break;

	case 'r':
	 g_bModeRedraw = TRUE;
	 break;

	case 't':
	 g_bModeTelnet = TRUE;
	 break;

	case 'u':
	 g_bModeFull8bit = TRUE;
	 break;

        case 'a':
         i++;
         Assert(i < argc, "No argument to internet address.");

	 c = argv[i];	 

         if (!isdigit(c[0]))
         {
            pHostInfo = gethostbyname(c);
            Assert(pHostInfo != NULL, "Could not lookup address.");
            pAddr = (struct in_addr*) (pHostInfo->h_addr_list[0]);
            c = inet_ntoa(*pAddr);
            Assert(c != NULL, "Error in address conversion");
         }

         arg->pAddress = str_dup(c);

         Assert(inet_addr(arg->pAddress) != (unsigned long) -1,
		"Illegal inet address");
         break;

 	case 'p':
         i++;
	 n = atoi(argv[i]);
	 Assert(n > 1000, "Port number in reserved area.");
	 arg->nMotherPort = n;
	 break;

 	case 's':
         i++;
	 n = atoi(argv[i]);
	 Assert(n > 1000, "Port number in reserved area.");
	 arg->nMudPort = n;
	 break;

	default:
	 fprintf(stderr, "Illegal option.\n");
	 ShowUsage(argv[0]);
	 exit(0);
      }
   }

   return TRUE;
}


/* ======================= TEXT PARSE & ECHO INPUT ====================== */


/* Maybe this would be overall easier? */
char cConHook::AddInputChar(ubit8 c)
{
   char *cp = m_aInputBuf;

   if (c == '\x1B')
   {
      m_nEscapeCode = 1;
      return 0;
   }

   if (m_nEscapeCode > 0)
   {
      if (m_nEscapeCode == 1)
      {
	 if (c == '[')
	 {
	    m_nEscapeCode++;
	    return 0;
	 }
      }
      else if (m_nEscapeCode == 2)
      {
	 if (c == 'D')
	 {
	    m_nEscapeCode = 0;
	    c = '\b';
	 }
      }
      else
	m_nEscapeCode = 0;
   }

   TAIL(cp);
   if ((c == '\b') || (c == '\177'))
   {
      if (cp != m_aInputBuf)
      {
	 *(cp-1) = 0;
	 return '\b';
      }

      return 0;
   }

   m_nEscapeCode = 0;

   *cp++ = c;
   *cp = 0;

   if ( g_bModeFull8bit ) // prool's 
   {
   if ((c < ' ') || (c==255) || (m_sSetup.telnet && ((c==0xFD) || (c==0xFE))))
     *(cp-1) = 0;
   }
   else
   {
   if ((c < ' ') || (c==255) || (m_sSetup.telnet && (c > 127)))
     *(cp-1) = 0;
   }

   return *(cp - 1);
}

void cConHook::AddString(char *str)
{
   char *s;
   char echobuf[4096]; // ~= 12 * sizeof(str) = 12 * 256 = 3000
   char *eb;
   char c;

   eb = echobuf;

   for (s = str; *s; s++)
   {
      if (ISNEWL(*s) || (strlen(m_aInputBuf) >= MAX_INPUT_LENGTH - 4))
      {
	 while (ISNEWL(*(s+1)))
	   s++;
	 *eb++ = '\n';
	 *eb++ = '\r';
	 m_qInput.Append(new cQueueElem(m_aInputBuf));
	 m_aInputBuf[0] = 0;
      }
      else
      {
	int x_pos = (m_nPromptLen + strlen(m_aInputBuf)) %
	   m_sSetup.width;
	 c = AddInputChar(*s);
	 if (c)
	 {
	    if (c == '\b')
	    {
	       if (m_sSetup.emulation == TERM_ANSI)
	       {
		  if (x_pos == 0)
		  {
		     sprintf(eb, "[A[%dC[K", m_sSetup.width-1);
		     TAIL(eb);
		  }
		  else
		  {
		     strcpy(eb, "[D[K");
		     TAIL(eb);
		  }
	       }
	       else
	       {
		  *eb++ = '\b';
		  *eb++ = ' ';
		  *eb++ = '\b';
	       }
	    }
	    else
	      *eb++ = c;
	 }
      }
   }
   
   *eb = 0;
   assert(eb - echobuf < (int) sizeof(echobuf)-1);

   if (m_sSetup.echo)
     Write((ubit8 *) echobuf, eb - echobuf);
}

/* On -1 'con' was destroyed */
void cConHook::ParseInput(void)
{
   char buf[256];

   if (m_qInput.Size() > 50)
   {
      SendCon("STOP FLOODING ME!!!\n\r");
      
      if (m_qInput.Size() > 100)
      {
	 Close(TRUE);
	 return;
      }
   }
}

/* ======================= TEXT FORMATTING OUTPUT ====================== */

char *cConHook::IndentText(const char *source,
			   char *dest, int dest_size, int width)
{
   const char *last = source, *current;
   char *newptr;
   int column = 0, cutpoint = MIN(30, width / 2);

   if (!(current = source))
     return NULL;

   newptr = dest;

   while (*current)
   {
      if (*current == CONTROL_CHAR)
      {
	 current++;
	 protocol_translate(this, *current, &newptr);
	 current++;
	 continue;
      }

      if (isaspace(*current)) /* Remember last space */
	last = current;

      if (*current == '\n' || *current == '\r') /* Newlines signify new col. */
	column = 0;

      if ((++column <= width)) /* MS: Added '<=' Have some space.. */
	*(newptr++) = *(current++);
      else /* Out of space, so... */
      {
	 column = 0;
	 if (last == NULL || cutpoint < current - last) /* backtrack or cut */
	   last = current;
	 newptr -= (current - last);
	 current = ++last;
	 *(newptr++) = '\n';
	 *(newptr++) = '\r';
	 current = skip_spaces(current); /* Skip any double spaces, etc. */
	 last = NULL;
      }
   }

   *newptr = '\0';

   assert(dest + dest_size > newptr);

   return dest;
}


/* Parse the string 'text' and prepare it for output on 'con' */
const char *cConHook::ParseOutput(const char *text)
{
   if (!IsHooked())
     return "";

   return IndentText(text, Outbuf, sizeof(Outbuf), m_sSetup.width);
}


/* Attempts to erase the characters input after the prompt */

void cConHook::PromptErase(void)
{
   int n;
   char buf[MAX_STRING_LENGTH];
   char *b = buf;

   if (m_nPromptLen == 0)
     return;

   *b = 0;

   if ((m_sSetup.emulation == TERM_TTY) ||
       (m_sSetup.emulation == TERM_VT100))
   {
      for (n=0; m_aInputBuf[n]; n++)
	*b++ = '\b';
      for (n=0; m_aInputBuf[n]; n++)
	*b++ = ' ';
      *b = 0;
   }
   else if (m_sSetup.emulation == TERM_ANSI)
   {
      int len = strlen(m_aInputBuf);
      int lines = 0;
      int i;

      if (len > 0)
      {
	 lines = (m_nPromptLen + len - 1) / m_sSetup.width;

	 for (i=0; i < lines; i++)
	 {
	    sprintf(b, "[%dD[K[A",
		    m_sSetup.width);
	    TAIL(b);
	 }

	 if (m_nPromptLen > 1)
	   sprintf(b, "[%dD[%dC[K",
		   m_sSetup.width,
		   m_nPromptLen - 1);
	 else
	    sprintf(b, "[%dD[K",
		    m_sSetup.width);

	 TAIL(b);
      }
   }

   m_nPromptLen = 0;

   if (*buf)
     Write((ubit8 *) buf, strlen(buf));
}


/* Assumes that a newline has already been output */
void cConHook::PromptRedraw(const char *prompt)
{
   if (*prompt)
     Write((ubit8 *) prompt, strlen(prompt));
   
   if (*m_aInputBuf)
     Write((ubit8 *) m_aInputBuf, strlen(m_aInputBuf));

   m_nPromptLen = strlen(prompt);
}


/* ======================= output handling ====================== */

/* -1 on connection closed, 0 on success */
void cConHook::WriteCon(const char *text)
{ 
   if (text == NULL)
     return;

   Write((ubit8 *) text, strlen(text));
}


void cConHook::SendCon(const char *text)
{
   WriteCon(ParseOutput(text));
}


/* ======================= 'Con' handling ====================== */

void cConHook::SequenceCompare(ubit8 *pBuf, int *pnLen)
{
   //static const char *match = "a";
   static const char *match = "[W32-V/C@SiGn]";
   static int state  = 0;

   assert(m_nSequenceCompare != -1);

   for (int i=0; i < *pnLen; i++)
   {
      if (pBuf[i] == match[m_nSequenceCompare])
      {
	 if (match[m_nSequenceCompare+1] == 0)
	 {
	    m_nSequenceCompare = -1;

	    m_sSetup.echo      = FALSE;
	    m_sSetup.redraw    = FALSE;
	    m_sSetup.emulation = TERM_INTERNAL;
	    m_sSetup.telnet    = FALSE;

	    memmove(pBuf, pBuf + strlen(match), *pnLen - strlen(match));
	    *pnLen -= strlen(match);

	    return;
	 }
      }
      else
      {
	 m_nSequenceCompare = -1;
	 return;
      }

      m_nSequenceCompare++;
   }
}


cConHook::cConHook(void)
{
   m_nEscapeCode = 0;
   m_aOutput[0] = 0;
   m_aInputBuf[0] = 0;
   m_nState = 0;

   m_nSequenceCompare = 0;
   m_nId = 0;
   m_nFirst = 0;
   m_nLine = 255;

   m_nPromptMode = 0;
   m_nPromptLen  = 0;

   m_sSetup.echo      = g_bModeEcho;
   m_sSetup.redraw    = g_bModeRedraw;
   m_sSetup.telnet    = g_bModeTelnet;   

   if (g_bModeANSI)
     m_sSetup.emulation = TERM_ANSI;
   else
     m_sSetup.emulation = TERM_TTY;

   m_sSetup.height    = 15;
   m_sSetup.width     = 80;
   m_sSetup.colour_convert = 0;
   m_nBgColor       = CONTROL_BG_BLACK_CHAR;

   m_pNext = connection_list;
   connection_list = this;

   int fd;
   socklen_t size;
   struct sockaddr_in conaddr;
   struct sockaddr_in sock;
   struct hostent *from;
   unsigned char *addr;
   const char *hostname;

   assert(MotherHook.IsHooked());
  

    
   socklen_t j;
   j = sizeof(conaddr);
   //slog(LOG_OFF, 0, "accept() before.");
   fd = accept(MotherHook.tfd(), (struct sockaddr *) &conaddr, &j);
   //slog(LOG_OFF, 0, "accept() after.");
   Assert(fd >= 0, "ACCEPT");

   int ms2020;
   ms2020 = fcntl(fd, F_SETFL, FNDELAY);
   Assert(ms2020 != -1, "Non blocking set error.");

   /* We **want** Nagle to work on the individual socket connections
      to the outside world, but not between the servers & mplex'ers 
      int i;
      j = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &i, sizeof(i));
      if (j == -1)
      error(HERE, "No Setsockopt()"); */

   m_nState = 0;
   m_pFptr = dumbMenuSelect;
   m_nPromptMode = 0;

   size = sizeof(sock);
   if (getpeername(fd, (struct sockaddr *) &sock, &size) == -1)
   {
      slog(LOG_OFF, 0, "getpeername: socket %d error no %d.",
	   fd, errno);
      hostname = "NO GETPEERNAME";
   }
   else
   {
      /*
	 This occasionally causes freezing (for all players) of a duration
	 from ~1 - 60 seconds. Too bad... if I get threads I will put it back
	 in, I hate having only the numeric address. Quinn suggested an
	 alarm of a few seconds, I'll try that out.
	 */

      /*alarm(1);*/

/*
      if ((from = gethostbyaddr((char*) &sock.sin_addr,
				sizeof(sock.sin_addr),
				AF_INET)) == NULL)
      {
	 slog(LOG_ALL, 0, "No symbolic INET address, h_errno %d.", h_errno);
	 addr = (unsigned char *) &sock.sin_addr.s_addr;
	 hostname = inet_ntoa(sock.sin_addr);
      }
      else
      {	
	 m_aHostname = (char *) from->h_name;
      }
*/
      /* alarm(0); unschedule alarm */

      /* The fix */
      addr = (unsigned char *) &sock.sin_addr.s_addr;
      hostname = inet_ntoa(sock.sin_addr);
   }
 
   strncpy(m_aHost, hostname, 49);
   *(m_aHost + 49) = '\0';

   CaptainHook.Hook(fd, this);

   connections_left--;
}


cConHook::~cConHook(void)
{
   if (this == connection_list)
     connection_list = this->m_pNext;
   else
   {
      class cConHook *tmp;
   
      for (tmp = connection_list; tmp; tmp=tmp->m_pNext)
	if (tmp->m_pNext == this)
	  break;
      tmp->m_pNext = this->m_pNext;
   }

   connections_left++;
}

void cConHook::testChar(ubit8 c)
{
   switch (m_nFirst)
   {
     case 0:
      if (c == IAC)
	m_nFirst++;
      else
	m_nFirst = -1;
      break;

     case 1:
      if (c == WILL)
	m_nFirst++;
      else
	m_nFirst = -1;
      break;

     case 2:
      if (c == TELOPT_ECHO)
	m_nFirst++;
      else
	m_nFirst = -1;
      break;

     case 3:
      m_nLine = c;
      m_nFirst++;
      break;

     case 4:
      if (c == IAC)
	m_nFirst++;
      else
      {
	 m_nFirst = -1;
	 m_nLine = 255;
      }
      break;
	 
     case 5:
      if (c == WONT)
	 m_nFirst++;
      else
      {
	 m_nFirst = -1;
	 m_nLine = 255;
      }
      break;

     case 6:
      if (c == TELOPT_ECHO)
      {
	 m_nFirst = -2;
      }
      else
      {
	 m_nFirst = -1;
	 m_nLine = 255;
      }
      break;

     default:
      m_nFirst = -1;
      m_nLine = 255;
      break;
   }
}


void cConHook::getLine(ubit8 buf[], int *size)
{
   int i;

   for (i=0; i < *size; i++)
   {
      testChar(buf[i]);

      // slog(LOG_ALL, 0, "Testchar %d, first %d.", buf[i], m_nFirst);

      if (m_nFirst == -1)
	return;
      if (m_nFirst == -2)
	break;
   }

   if (i < *size)
   {
      memmove(buf, buf+i+1, *size - (i+1));
   }
   *size -= i;
}


void cConHook::Input(int nFlags)
{
   if (nFlags & SELECT_EXCEPT)
   {
      Close(TRUE);
   }
   else if (nFlags & SELECT_READ)
   {
      char *c;
      ubit8 buf[1024];

      int n = read(this->tfd(), buf, sizeof(buf) - 1);

      if (n == -1)
      {
	 if ((errno == EWOULDBLOCK) || (errno == EAGAIN))
	   return;

	 Close(TRUE);
	 return;
      }
      else if (n == 0)
      {   
	 Close(TRUE);
	 return;
      }

      if (m_nSequenceCompare != -1)
      {
	 SequenceCompare(buf, &n);
	 if (n <= 0)
	   return;
      }

      if (m_nFirst >= 0)
      {
	 getLine(buf, &n);
	 if (n <= 0)
	   return;
      }
	    
      buf[n] = 0;
      AddString((char *) buf);

      ParseInput();

      cQueueElem *qe;

      while ((qe = m_qInput.GetHead()))
      {
	 c = (char *) qe->Data();
	 assert(strlen(c) < MAX_INPUT_LENGTH);
	 m_pFptr(this, c);
	 delete qe;
      }
   }
}

void cConHook::Close(int bNotifyMud)
{
   if (bNotifyMud && m_nId != 0 && MudHook.IsHooked())
   {
      slog(LOG_ALL, 0, "Closing con id %d.", m_nId);
      protocol_send_close(&MudHook, m_nId);
   }

   if (IsHooked())
     Unhook();

   m_aOutput[0] = 0;
   m_aInputBuf[0] = 0;
   m_nState = 0;
   m_pFptr = Idle;
   m_nId = 0;
}


void ClearUnhooked(void)
{
   class cConHook *con, *nextcon;

   for (con = connection_list; con; con = nextcon)
   {
      nextcon = con->m_pNext;

      if (!con->IsHooked())
      {
	 con->Close(TRUE);
	 delete con;
      }
   }
}


/* ================================================================== */


void cConHook::TransmitCommand(const char *text)
{
   char *d;
   ubit16 len;

   if ((d = (char *) strchr(text, '\r'))) //MS2020
     *d = 0;

   if ((d = (char *) strchr(text, '\n'))) //MS2020
     *d = 0;

   strip_trailing_spaces((char *) text); //MS2020

   if ((len = strlen(text)) >= MAX_INPUT_LENGTH)
   {
      /* Perhaps warn user? */
     ((char *) text)[MAX_INPUT_LENGTH - 1] = 0;
   }

   m_nPromptLen = 0;

   protocol_send_text(&MudHook, m_nId, text, MULTI_TEXT_CHAR);
}




void Idle(cConHook *con, const char *cmd)
{
   /* This should not happen, but I am tryng to find a bug... */

   slog(LOG_ALL, 0, "AHA! Ilde was called! Tell Paps about this!");
}

void cConHook::PlayLoop(const char *cmd)
{
   if (m_nId == 0)
   {
      m_nState++;

      if (m_nState < 100)
      {
	 WriteCon(MUD_NAME " has not yet finished rebooting.\n\r");
	 return;
      }
      else
      {
	 SendCon("Please retry.\n\r");
	 m_nState = 0;
	 m_pFptr = dumbMenuSelect;
      }
   }
   else /* m_nId != 0 */
     TransmitCommand(cmd);
}

void cConHook::MenuSelect(const char *cmd)
{
   int n;
   const char *c;

   if (!MudHook.IsHooked())
   {
      m_pFptr = dumbMudDown;
      return;
   }

   for (c = cmd; isspace(*c); c++)
     ;

   protocol_send_request(&MudHook);

   if (MudHook.IsHooked())
   {
      m_nState = 0;
      m_pFptr = dumbPlayLoop;
   }
   else
     SendCon(MUD_NAME " is unreachable right now...\n\r\n\r");
}



void cConHook::PressReturn(const char *cmd)
{
   int oldmode = m_nPromptMode;

   if (*skip_blanks(cmd))
   {
      SendCon(CONTROL_FG_RED "*** Read aborted ***"
	      CONTROL_FG_WHITE "\n\r");

      m_qPaged.Flush();
      m_pFptr = dumbPlayLoop;
      m_nPromptMode = 0;
      PlayLoop(cmd);
      return;
   }

   ShowChunk();

   if (m_nPromptMode == 1)
   {
      /* XXX DONT FORCE A PROMPT     if (oldmode == 1)
	 PlayLoop(con, ""); */
   }
   else if (m_nPromptMode == 0)
   {
      m_qPaged.Flush();
      m_pFptr = dumbPlayLoop;
      m_nPromptMode = 0;

      if (oldmode != m_nPromptMode)
      {
	 char buf[1000];
	 strcpy(buf, ParseOutput(CONTROL_FG_RED "\n\r<Read Done>"
				 CONTROL_FG_WHITE "\n\r"));
	 Write((ubit8 *) buf, strlen(buf));
	 PlayLoop("");
      }
   }
}


/* Returns new point (NULL if done) */
void cConHook::ShowChunk(void)
{
   char buffer[160 * 60 + 600];
   char *scan, *point;
   const int max_lines = m_sSetup.height;
   int lines = 0;

   scan = buffer;

   if (m_qPaged.IsEmpty())
     return;

   cQueueElem *qe = m_qPaged.GetHead();

   point = (char *) qe->Data();

   for (;;)
   {
      if (!*point)
      {
	 if (m_qPaged.IsEmpty())
	   break;

	 delete qe;
	 qe = m_qPaged.GetHead();
	 point = (char *) qe->Data();
	 continue;
      }

      *scan = *point++;

      if (*scan == '\n')
	lines++;

      if (max_lines <= lines)
	break;

      scan++;

      if (scan >= buffer + sizeof(buffer) - 200)
      {
	 sprintf(scan, "TRUNCATED!");
	 break;
      }
   }

   *scan = '\0';

   /* Insert the rest of the un-paged stringback into buffer */
   if (!str_is_empty(point))
     m_qPaged.Prepend(new cQueueElem(point));

   if (qe)
     delete qe;

   m_nPromptMode = !m_qPaged.IsEmpty();

   if (m_nPromptMode == 1)
   {
      strcat(buffer, ParseOutput(CONTROL_FG_RED "\n\r<Return for more>"
				 CONTROL_FG_WHITE "\n\r"));
   }

   assert(strlen(buffer) < sizeof(buffer));

   Write((ubit8 *) buffer, strlen(buffer));
}


void cConHook::ProcessPaged(void)
{
   if (m_pFptr != dumbPressReturn)
   {
      m_pFptr = dumbPressReturn;
      PressReturn("");
   }   
}


/* -1 on error. 0 when no more data. +1 When processed one request. */
int cMudHook::read_mud(void)
{
   class cConHook *con;
   ubit16 id, len;
   int p, n;
   char *data;
   ubit8 text_type;

   p = protocol_parse_incoming(this, &id, &len, &data, &text_type);

   if (p <= 0)
     return p;

   switch (p)
   {
     case MULTI_SETUP_CHAR:
      if (len != sizeof(struct terminal_setup_type))
      {
	 slog(LOG_OFF, 0, "Illegal size m_sSetup received.");
	 break;
      }

      for (con = connection_list; con; con = con->m_pNext)
      {
	 if (con->m_nId == id)
	 {
	    n = (con->m_sSetup.emulation == TERM_INTERNAL);
   
	    memcpy(&(con->m_sSetup), data, len);

	    if (n)
	      con->m_sSetup.emulation = TERM_INTERNAL;

	    assert(is_in(con->m_sSetup.emulation, TERM_DUMB, TERM_INTERNAL));

	    assert(con->m_sSetup.width >= 40 && 
		   con->m_sSetup.width <= 160);

	    assert(con->m_sSetup.height >= 15 && 
		   con->m_sSetup.height <= 60);

	    con->SendCon(CONTROL_RESET CONTROL_FG_WHITE
			 CONTROL_BG_BLACK);
	    n = 0;
	    break;
	 }
      }
      if (con == NULL)
	slog(LOG_OFF, 0, "ERROR: Unknown ID m_sSetup received.");
      
      break;

     case MULTI_TERMINATE_CHAR:
      if (data)
	free(data);
      for (con = connection_list; con; con = con->m_pNext)
      {
	 if (con->m_nId == id)
	 {
	    con->SendCon(CONTROL_RESET "Your connection was requested "
			 "terminated.\n\n\r");
	    con->Close(FALSE);
	    return 1;
	 }
      }
      if (con == NULL)
	slog(LOG_OFF, 0, "ERROR: Unknown ID requested terminated.");
      return 1;

     case MULTI_CONNECT_CON_CHAR:
      if (data)
	free(data);
      for (con = connection_list; con; con = con->m_pNext)
      {
	 if ((con->m_nId == 0) && (con->m_pFptr == dumbPlayLoop))
	 {
	    con->m_nId = id;
	    protocol_send_host(&MudHook, id, con->m_aHost,
			       arg.nMotherPort, con->m_nLine);
	    break;
	 }
      }
      if (con == NULL)
      {
	 //slog(LOG_OFF,0,"Unknown destination for confirm! Requesting term.");
	 protocol_send_close(&MudHook, id);
      }
      return 1;

     case MULTI_TEXT_CHAR:
     case MULTI_PAGE_CHAR:
     case MULTI_PROMPT_CHAR:
      if (len == 0)
	break;
      
      for (con = connection_list; con; con = con->m_pNext)
      {
	 if (id == con->m_nId)
	 {
	    char *parsed;

	    parsed = (char *) con->ParseOutput(data);

	    switch (p)
	    {
	      case MULTI_PAGE_CHAR:
	       con->m_qPaged.Append(new cQueueElem(parsed));
	       con->ProcessPaged();
	       break;

	      case MULTI_PROMPT_CHAR:
	       con->PromptRedraw(parsed);
	       break;

	      default:
	       con->Write((ubit8 *) parsed, strlen(parsed));
	       break;
	    }
	    break;
	 }
      }

      if (con == NULL)
      {
	 // slog(LOG_OFF, 0, "Unknown destination text received.");
      }
      break;
      
     default:
      abort();
   }

   if (data)
     free(data);

   return 1;
}


/* Test if the mud is up again... */
void test_mud_up(void)
{
   int n, fd;
   class cConHook *nextcon, *con;

   fd = OpenNetwork(arg.nMudPort, arg.pAddress);

   if (fd == -1)
   {
      sleep(1);
      return;
   }

   slog(LOG_OFF, 0, "Stream to server was re-opened!");

   CaptainHook.Hook(fd, &MudHook);

   for (con = connection_list; con; con = nextcon)
   {
      nextcon = con->m_pNext;
      con->SendCon(MUD_NAME " has begun rebooting... please wait.\n\n\r");

      con->m_pFptr = dumbMenuSelect;
      con->m_nState = 0;
      con->m_qInput.Flush();
      con->m_pFptr(con, "");
   }
}


void cConHook::MudDown(const char *cmd)
{
   SendCon("There is still no connection to " MUD_NAME ".\n\n\r");
}


void mud_went_down(void)
{
   int n, tmpFd;
   class cConHook *con;

   if (MudHook.IsHooked())
     MudHook.Unhook();

   for (con = connection_list; con; con = con->m_pNext)
   {
      con->SendCon("\n\n\rThe connection to " MUD_NAME " was broken. "
		  "Please be patient...\n\n\r");
      con->m_nId = 0;
      con->m_pFptr = dumbMudDown;
      con->m_nState = 0;
   }
}

void Control(void)
{
   static int tries = 0;
   int n;

   for (;;)
   {
      if (!MotherHook.IsHooked())
	return;

      if (!MudHook.IsHooked())
      {
	 if (tries++ > 20)
	 {
	    slog(LOG_ALL, 0, "Giving up connecting to mud, retrying...");
	    return;
	 }
	 test_mud_up(); /* Need to do this first... */
      }
      else
	tries = 0;


      ClearUnhooked(); /* Clear all closed down connections */

      n = CaptainHook.Wait(NULL);

      if (n == -1)
      {
	 slog(LOG_ALL, 0, "Select error (errno %d)", errno);
	 return;
      }
   }
}


void bye_signal(int signal)
{
   CaptainHook.Close();

   slog(LOG_OFF, 0, "Received signal #%d (SIGQUIT, SIGHUP, SIGINT or SIGTERM)."
	"  Shutting down", signal);

   exit(1);
}


void alarm_signal(int sig)
{
   signal(SIGALRM, alarm_signal);
   slog(LOG_OFF, 0, "Received alarm signal.");
   bHadAlarm = TRUE;
}


int main(int argc, char *argv[])
{
   static int i = 0;
   int fd;

#ifdef PROFILE
   extern etext();
   monstartup((int) 2, etext);
#endif

   assert(i++ == 0); /* Make sure we dont call ourselves... cheap hack! :) */

   if ( !ParseArg(argc, argv, &arg) )
     exit(0);

   signal(SIGQUIT, bye_signal);
   signal(SIGHUP, bye_signal);
   signal(SIGINT, bye_signal);
   signal(SIGTERM, bye_signal);
   signal(SIGALRM, alarm_signal);

   translate_init();

   fd = OpenMother(arg.nMotherPort);
   Assert(fd != -1, "NO MOTHER CONNECTION.");

   CaptainHook.Hook(fd, &MotherHook);

   slog(LOG_OFF, 0, "mplex/prool mod. Mother connection on port %d opened.",
	arg.nMotherPort);

   /* Subtract stdout, stdin, stderr, fdmud, fdmother and 2 to be safe. */
   connections_left = getdtablesize() - 3 - 2 - 2;

   Control();

   MudHook.Unhook();
   MotherHook.Unhook();

   return 0;
}
