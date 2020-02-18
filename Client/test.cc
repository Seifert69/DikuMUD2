#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/times.h>

#include "essential.h"
#include "slide.h"
#include "log.h"

cSlw *Slw = NULL;

void TransmitFrame(ubit8 *data, int len)
{
   int n;

   for (;;)
   {
      n = Slw->Transmit(data, len);

      if (n == -1)
      {
	 Slw->Poll();
	 continue;
      }
      
      if (n == -2)
      {
	 DEBUG("Transmission link error.\n");
	 exit(1);
      }

      break;
   }
}


int nReceived = 0;
int nSum = 0;

void ReceiveFrame(ubit8 *data, ubit32 len)
{
   if (data[0] != (nReceived % 256))
   {
      fprintf(stderr, "Error on %d vs received %d\n ",
	      data[0], nReceived % 256);
   }

   nReceived++;
   nSum += len;
}


void test(int receive)
{
   ubit8 data[1024];
   int i, j;
   clock_t start = 0, end;

   data[0] = 0;

   Slw->SetArrivalFunction(ReceiveFrame);

   if (receive)
   {
      fprintf(stderr, "Receiving session.\n");
      while (nReceived == 0)
      {
	 Slw->Poll();
	 usleep(1);
      }
      start = times(NULL);
   }

   for (i=0; i < 500 ; i++)
   {
      data[0] = i % 256;

      TransmitFrame(data, SLW_MAX_DATA);
      Slw->Poll();
   }

   end = times(NULL);

   for (;;)
   {
      i = nReceived;

      for (j=0; j < 400; j++)
      {
	 Slw->Poll();
	 usleep(10000);
      }

      if (nReceived == i)
	break;
      end = times(NULL);
   }

   fprintf(stderr, "Received %d frames totalling %d bytes in %ld clocks.\n",
	   nReceived, nSum, end - start);

   double bps = nSum;
   double tmp = (end - start);
   tmp = tmp / (double) CLOCKS_PER_SEC;

   bps /= tmp;

   fprintf(stderr, "%.0f bytes per second.\n", bps);

   fprintf(stderr, "%.0f bits per second.\n", 8*bps);


   char buf[2000];

   Slw->SlwError.Status(buf);

   fprintf(stderr, buf);
}


int main(int argc, char *argv[])
{
   if (argc < 2)
   {
      fprintf(stderr, "Usage:\n");
      fprintf(stderr, "   %s <device> [receiving]\n", argv[0]);
      exit(0);
   }

   Slw = new cSlw(NULL); // Log file name here.
   pLogFile->Log("Using device %s\n", argv[1]);

   if (Slw->Open(argv[1]) == -1)
   {
      pLogFile->Log("Can't open device.\n");
      exit(1);
   }

   Slw->Deactivate();

   test(argc >= 3);

   Slw->Close();

   delete Slw;

   return 0;
}
