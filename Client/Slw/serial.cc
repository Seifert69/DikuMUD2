/* *********************************************************************** *
 * File   : serial.cc                                 Part of Valhalla MUD *
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
#include "serial.h"
#include "log.h"

#define SERIAL_COMMAND_DELAY  (1000000/10)

#define SERIAL_DCD   0x01
#define SERIAL_DTR   0x02
#define SERIAL_DSR   0x04
#define SERIAL_RTS   0x08
#define SERIAL_CTS   0x10
#define SERIAL_RING  0x20

/* ------------------------------------------------------------------- */
/*                             P R I V A T E                           */
/* ------------------------------------------------------------------- */

/* 0 ok, -1 error */
int cSerial::SetRaw(void)
{
   struct termios tty;

   tcgetattr(tfd(), &tty);

   /* Set into raw, no echo mode */

   /* ----- Input flags, "_iflag" */

   REMOVE_BIT(tty.c_iflag, BRKINT | PARMRK | INPCK | ISTRIP | INLCR |
	      IGNCR | ICRNL | IXON | IXOFF | IUCLC | IXANY);
   SET_BIT(tty.c_iflag,  IGNBRK | IGNPAR | IMAXBEL);


   /* ----- Local flags, "_lflag" */
   REMOVE_BIT(tty.c_lflag, ISIG | ICANON | IEXTEN | ECHO | ECHOE | ECHOK |
	      ECHONL | ECHO);
   SET_BIT(tty.c_lflag, NOFLSH);


   /* ----- Output flags, "_oflag" */
   REMOVE_BIT(tty.c_oflag, OPOST);


   /* ----- Control mode flags, "_cflag" */
   REMOVE_BIT(tty.c_cflag, PARENB | PARODD | HUPCL | 
	      CSTOPB | CLOCAL);

   SET_BIT(tty.c_cflag, CREAD | CRTSCTS);

   tty.c_cc[VMIN] = 1;
   tty.c_cc[VTIME] = 5;

   return tcsetattr(tfd(), TCSANOW, &tty);
}


/* 0 ok, -1 error */
int cSerial::SetNormal(void)
{
   struct termios tty;

   tcgetattr(tfd(), &tty);

   /* Set into raw, no echo mode */

   /* ----- Input flags, "_iflag" */

   REMOVE_BIT(tty.c_iflag, BRKINT | PARMRK | INPCK | ISTRIP | INLCR |
	      IGNCR | ICRNL | IXON | IXOFF | IUCLC | IXANY);
   SET_BIT(tty.c_iflag,  IGNBRK | IGNPAR | IMAXBEL);


   /* ----- Local flags, "_lflag" */
   REMOVE_BIT(tty.c_lflag, ISIG | ICANON | IEXTEN | ECHO | ECHOE | ECHOK |
	      ECHONL | ECHO);
   SET_BIT(tty.c_lflag, NOFLSH);


   /* ----- Output flags, "_oflag" */
   REMOVE_BIT(tty.c_oflag, OPOST);


   /* ----- Control mode flags, "_cflag" */
   REMOVE_BIT(tty.c_cflag, PARENB | PARODD | HUPCL | 
	      CSTOPB | CLOCAL);

   SET_BIT(tty.c_cflag, CREAD | CRTSCTS);

   tty.c_cc[VMIN] = 1;
   tty.c_cc[VTIME] = 5;

   return tcsetattr(tfd(), TCSANOW, &tty);
}



int cSerial::SetMode(int baud, char par, char bits)
{
   struct termios tty;
   int spd = -1;

   if (tcgetattr(tfd(), &tty) == -1)
   {
      pLogFile->Log("Cannot get attr in serial set mode.\n");
      return -1;
   }

   switch(baud)
   {
     case 0:
      spd = B0;
      break;
     case 300:
      spd = B300;
      break;
     case 600:
      spd = B600;
      break;
     case 1200:
      spd = B1200;
      break;
     case 2400:
      spd = B2400;
      break;
     case 4800:
      spd = B4800;
      break;
     case 9600:
      spd = B9600;
      break;
     case 19200:
      spd = B19200;
      break;
     case 38400:
      spd = B38400;
      break;
     default:
      pLogFile->Log("Illegal baud rate.\n");
      return -1;
   }
  
   if (spd != -1)
   {
      int error = 0;
      if (cfsetospeed(&tty, (speed_t)spd) == -1)
      {
	 pLogFile->Log("Can't set output speed.\n");
	 error = 1;
      }

      if (cfsetispeed(&tty, (speed_t)spd) == -1)
      {
	 pLogFile->Log("Cant set input speed\n");
	 error = 1;
      }
      if (error)
	return -1;
   }

   SET_BIT(tty.c_cflag, CRTSCTS);
   
   REMOVE_BIT(tty.c_cflag, PARENB | PARODD);
   if (par == 'E')
     tty.c_cflag |= PARENB;
   else if (par == 'O')
     tty.c_cflag |= PARODD;

   switch (bits)
   {
     case '5':
      tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS5;
      break;
     case '6':
      tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS6;
      break;
     case '7':
      tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS7;
      break;
     case '8':
     default:
      tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
      break;
   }

   if (tcsetattr(tfd(), TCSANOW, &tty) == 1)
     return -1;

   return 0;
}


/* Return -1 on error, 0 on mismatch, 1 on match */
int cSerial::WaitFor(char *pStr, int nTimeOutSecs)
{
   ubit8 Buf[10];
   char *pCh;
   int i, j;
   struct timeval timeout;
   fd_set read_set;
   fd_set except_set;

   pLogFile->Log("MATCH '%s' %d seconds - \n\n", pStr, nTimeOutSecs);

   for (pCh = pStr; nTimeOutSecs > 0; )
   {
      timeout.tv_sec  = 1;
      timeout.tv_usec = 0;

      FD_ZERO(&read_set);
      FD_SET(tfd(), &read_set);

      FD_ZERO(&except_set);
      FD_SET(tfd(), &except_set);

      if (select(255, &read_set, NULL, &except_set, &timeout) == -1)
      {
	 pLogFile->Log("Select Error.\n");
	 return -1;
      }

      if (FD_ISSET(tfd(), &except_set))
      {
	 pLogFile->Log("Except Error.\n");
	 return -1;
      }

      if (!FD_ISSET(tfd(), &read_set))
      {
	 nTimeOutSecs--;
	 continue;
      }

      i = ReadBlock(Buf, 1);
      if (i == -1)
      {
	 pLogFile->Log("Read Error.\n");
	 return -1;
      }

      if (i == 0)
      {
	 nTimeOutSecs--;
	 continue;
      }

      for (j=0; j < i; j++)
      {
	 pLogFile->Log("%c", Buf[j]);
	 if (Buf[j] == *pCh)
	 {
	    pCh++;
	    if (*pCh == 0)
	    {
	       pLogFile->Log("\nFound.\n");
	       return 1;
	    }
	 }
	 else
	   pCh = pStr;
      }
   }

   pLogFile->Log("\nNot found\n");
   return 0;
}


/* -1 on failure, 0 otherwise  */
int cSerial::Command(char *txStr)
{
   char *orgStr = txStr;

   for (; *txStr; txStr++)
   {
      if (Send(*txStr) == -1)
      {
	 pLogFile->Log("Modem command failed!\n");
	 return -1;
      }
      usleep(SERIAL_COMMAND_DELAY);
   }

   if (WaitFor(orgStr, 8) != 1)
     return -1;

   return 0;
}

/* ------------------------------------------------------------------- */
/*                             P U B L I C                             */
/* ------------------------------------------------------------------- */

cSerial::cSerial(void)
{
}

cSerial::cSerial(char *pDev)
{
   Open(pDev);
}

cSerial::~cSerial(void)
{
   Close();
}

int cSerial::Open(char *cpDev, int nBaud)
{
   int fdSerial;

   assert(!IsHooked());

   pLogFile->Log("Opening Serial connection on %s\n", cpDev);

   fdSerial = open(cpDev, O_RDWR | O_NDELAY);

   if (fdSerial == -1)
   {
      pLogFile->Log("Error opening device %s (err %d).",
	  cpDev, errno);
      return -1;
   }

   CaptainHook.Hook(fdSerial, this);

   if (tcgetattr(tfd(), &org_tty) == -1)
   {
      pLogFile->Log("Open: getattr (err %d).", errno);
      Close();
      return -1;
   }

   ori_flags = fcntl(tfd(), F_GETFL);
   if (ori_flags == -1)
   {
      pLogFile->Log("Can't get original flags\n");
      return -1;
   }

   raw_tty = org_tty; /* For now... until reset */

   if (SetMode(nBaud, 'N', '8') == -1)
   {
      pLogFile->Log("Cannot set serial mode in open");
      return -1;
   }

   if (SetRaw() == -1)
   {
      pLogFile->Log("Error in serial set raw.");
      return -1;
   }

   raw_flags = fcntl(tfd(), F_GETFL);

   if (raw_flags == -1)
     return -1;

   if (tcgetattr(tfd(), &raw_tty))
     return -1;

   if (Flush() == -1)
     return -1;

   return tfd();
}


int cSerial::Close(void)
{
   if (!IsHooked())
     return 0;

   if (Hangup() == -1)
   {
      pLogFile->Log("Error on hangup in close (err %d)\n", errno);
   }

   if (tcsetattr(tfd(), TCSANOW, &org_tty) == -1)
   {
      pLogFile->Log("tcsetattr error in close (err %d)\n", errno);
   }

   Unhook();

   return 0;
}


int cSerial::Break(void)
{
   int n;

   if (!IsHooked())
     return -1;

   n = tcsendbreak(tfd(), 0);
   if (n == -1)
   {
      pLogFile->Log("Send Break Failure.\n");
      return -1;
   }
   return 0;
}

int cSerial::Status(void)
{
   int n = 0;
   int nStatus;

   if (!IsHooked())
     return -1;

   if (ioctl(tfd(), TIOCMGET, &nStatus) == -1)
   {
     pLogFile->Log("ioctl error in reading Serial status (err %d, fd %d)\n",
	 errno, tfd());
     return -1;
   }

   if (IS_SET(nStatus, TIOCM_CAR))
     SET_BIT(n, SERIAL_DCD);
   if (IS_SET(nStatus, TIOCM_DTR))
     SET_BIT(n, SERIAL_DTR);
   if (IS_SET(nStatus, TIOCM_DSR))
     SET_BIT(n, SERIAL_DSR);
   if (IS_SET(nStatus, TIOCM_RTS))
     SET_BIT(n, SERIAL_RTS);
   if (IS_SET(nStatus, TIOCM_CTS))
     SET_BIT(n, SERIAL_CTS);
   if (IS_SET(nStatus, TIOCM_RNG))
     SET_BIT(n, SERIAL_RING);

   return n;
}

/* Toggle DTR for one second. */
int cSerial::Hangup(void)
{
   struct termios old, tty;
   
   if (!IsHooked())
     return -1;

   if (tcgetattr(tfd(), &tty) == -1)
     return -1;

   old = tty;

   if (cfsetospeed(&tty, B0) == -1)
   {
      pLogFile->Log("hangup: seto B0\n");
      return -1;
   }

   if (tcsetattr(tfd(), TCSADRAIN, &tty))
   {
      pLogFile->Log("hangup: tcsetattr\n");
      return -1;
   }

   usleep(500000);

   if (tcsetattr(tfd(), TCSADRAIN, &old) == -1)
   {
      DisplayStatus();
      pLogFile->Log("Hangup restore old.\n");
      return -1;
   }

   usleep(500000);

   return 0;
}

int cSerial::DisplayStatus(void)
{
   int nStatus;

   nStatus = Status();
   if (nStatus == -1)
     return -1;

   pLogFile->Log("Serial STATUS: ");

   if (IS_SET(nStatus, SERIAL_DCD))
     pLogFile->Log("DCD ");

   if (IS_SET(nStatus, SERIAL_DTR))
     pLogFile->Log("DTR ");
    
   if (IS_SET(nStatus, SERIAL_DSR))
     pLogFile->Log("DSR ");

   if (IS_SET(nStatus, SERIAL_RTS))
     pLogFile->Log("RTS ");

   if (IS_SET(nStatus, SERIAL_CTS))
     pLogFile->Log("CTS ");

   if (IS_SET(nStatus, SERIAL_RING))
     pLogFile->Log("RING ");

   pLogFile->Log("\n");
   return 0;
}

int cSerial::Online(void)
{
   int n;
   static int tolerance = 0;

   if (!IsHooked())
     return FALSE;

   n = Status();

   if (n == -1)
   {
      tolerance = 0;
      return FALSE;
   }
   else
   {
      if (IS_SET(n, SERIAL_DCD))
      {
	 if (tolerance > 0)
	 {
	    pLogFile->Log("DCD TOLERANCE PREVENTED LINE HANGUP!");
	    tolerance = 0;
	 }
	 return TRUE;
      }
      else
      {
	 if (tolerance++ > 5)
	 {
	    tolerance = 0;
	    return FALSE;
	 }
	 else
	 {
	    pLogFile->Log("WOW! SLEEPING IN ONLINE...");
	    usleep(20000);
	    return Online();
	 }
      }
   }
}


/* -1 on error */
int cSerial::WaitOnline(void)
{
   fd_set read_set, except_set;
   struct timeval timeout;
   int n;

   if (!IsHooked())
     return -1;

   for (;;)
   {
      if (Hangup() == -1)
	return -1;

      if (Status() != (SERIAL_CTS | SERIAL_RTS | SERIAL_DTR | SERIAL_DSR))
      {
	 pLogFile->Log("ILLEGAL STATUS: ");
	 DisplayStatus();
	 return -1;
      }

      pLogFile->Log("Waiting for Serial to become active.\n");

      do
      {
	 timeout.tv_sec  = 2;
	 timeout.tv_usec = 0;
	 
	 FD_ZERO(&read_set);
	 FD_SET(tfd(), &read_set);
	 FD_ZERO(&except_set);
	 FD_SET(tfd(), &except_set);

	 if (select(255, &read_set, NULL, &except_set, &timeout) == -1)
	   return -1;

	 if (FD_ISSET(tfd(), &except_set))
	   return -1;
      }
      while (!FD_ISSET(tfd(), &read_set));
      
      pLogFile->Log("ONLINE: ");
      n = WaitFor("CONNECT", 90);

      if (n == -1)
	return -1;
      else if (n == 1)
      {
	 /* pLogFile->Log("GOT CONNECT"); */
	 DisplayStatus();
	 pLogFile->Log("\nSerial Connection established.\n");
	 return 1;
      }
      else
      {
	 /* pLogFile->Log("GOT NO CONNECT"); */
      }
   }

   return 0;
}


/* The number of bytes if sent ok, -1 if error */

int cSerial::Send(const ubit8 *pChunk, ubit32 nSize)
{
   int n, olen = nSize;

   if (!IsHooked())
     return nSize; //Pretend its being sent, we want to return asap...

   //if (number(0,10000) == 0)
   //  pChunk[number(0,nSize-1)] = (ubit8) number(0,255);

   do
   {
      for (;;)
      {
	 n = write(tfd(), pChunk, nSize);

	 if (n > 0)
	   break;

	 if (n == -1)
	 {
	    if ((errno != ENOBUFS) && (errno != EAGAIN))
	    {
	       Unhook();
	       pLogFile->Log("Write error (errno %d)\n", errno);
	       return -1;
	    }
	 }
	 Poll();
	 usleep(1);
      }

      nSize  -= n;
      pChunk += n;
   }
   while (nSize > 0);

   return olen;
}


int cSerial::Send(ubit8 data)
{
   return Send(&data, 1);
}


int cSerial::SendString(char *pStr)
{
   return Send((ubit8 *) pStr, (ubit32) strlen(pStr));
}


int cSerial::ReadBlock(ubit8 *pChunk, ubit32 nSize)
{
   int n;
   ubit32 nRead = 0;

   if (!IsHooked())
     return -1;

   do
   {
      n = read(tfd(), pChunk, nSize - nRead);

      //pLogFile->Log("n = %d : Readblock fd %d, bytes %d",
      //n, tfd(), nSize - nRead);

      if (n > 0)
      {
	 pChunk += n;
	 nRead += n;
	 continue;
      }
      else if (n == 0) // End of File...
      {
	 Unhook();
	 pLogFile->Log("EOF on reading.");
	 break;
      }
      else /* n < 0 */
      {
	 if (errno == EWOULDBLOCK)
	   break;

	 Unhook();
	 pLogFile->Log("Error reading from socket (%d).\n", errno);
	 return -1;
      }
   }
   while (nRead < nSize);

   return nRead;
}


void cSerial::Poll(void)
{
   ubit8  buf[1024];
   int n;
   static int blocked = 0;

   if (blocked)
     return;

   blocked++;

   while (IsHooked())
   {
      n = ReadBlock(buf, sizeof(buf));

      if (n > 0)
      {
	 Receive(buf, n);
	 continue;
      }
      break;
   }

   blocked--;
}


int cSerial::Flush(void)
{
   ubit8 Buf[400];

   while (ReadBlock(Buf, sizeof(Buf)-1) > 0)
     usleep(1000000/10);

   if (tcdrain(tfd()) == -1)
   {
      pLogFile->Log("Flush failure.\n");
      return -1;
   }

   if (tcflush(tfd(), TCIOFLUSH) == -1)
   {
      pLogFile->Log("Flush failure.\n");
      return -1;
   }
   return 0;
}


/* Return FALSE on failure */
int cSerial::ModemInit(char **init_string[])
{
   int i;
   char Buf[256];

   if (tfd() == -1)
     return -1;

   if (Hangup() == -1)
     return -1;

   for (i = 0; init_string[i]; i++)
   {
      if (init_string[i][0])
      {
	 sprintf(Buf, "%s\r", init_string[i][0]);
	 if (Command(Buf) == -1)
	   return -1;

	 for (int j = 1; init_string[i][j]; j++)
	   if (WaitFor(init_string[i][j], 8) != 1)
	     return -1;
      }
   }

   pLogFile->Log("Modem Post-Init Status:");
   DisplayStatus();

   return Flush();
}
