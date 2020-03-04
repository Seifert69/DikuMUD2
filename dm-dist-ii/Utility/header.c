/* *********************************************************************** *
 * File   : header.c                                  Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Maing nice headers.                                            *
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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define MAX_SIZE 8192

float version      = 1.0;
char file[1024]    = "UNKNOWN";
char author[1024]  =
" * Author : Unknown.                                                       *";
char purpose[1024] = 
" * Purpose: Unknown.                                                       *";
char bugs[1024]    =
" * Bugs   : Unknown.                                                       *";
char status[1024]    =
" * Status : Unpublished.                                                   *";

const char *notice1 =
" * Copyright (C) Valhalla (This work is unpublished).                      *"
"\n"
" *                                                                         *"
"\n"
" * This work is a property of:                                             *"
"\n"
" *                                                                         *"
"\n"
" *        Valhalla I/S                                                     *"
"\n"
" *        Noerre Soegade 37A, 4th floor                                    *"
"\n"
" *        1370 Copenhagen K.                                               *"
"\n"
" *        Denmark                                                          *"
"\n"
" *                                                                         *"
"\n"
" * This is an unpublished work containing Valhalla confidential and        *"
"\n"
" * proprietary information. Disclosure, use or reproduction without        *"
"\n"
" * authorization of Valhalla is prohobited.                                *";


const char *notice2 =
" * Copyright (C) 1994 - 1996 by Valhalla (This work is published).         *"
"\n"
" *                                                                         *"
"\n"
" * This work is a property of:                                             *"
"\n"
" *                                                                         *"
"\n"
" *        Valhalla I/S                                                     *"
"\n"
" *        Noerre Soegade 37A, 4th floor                                    *"
"\n"
" *        1370 Copenhagen K.                                               *"
"\n"
" *        Denmark                                                          *"
"\n"
" *                                                                         *"
"\n"
" * This work is copyrighted. No part of this work may be copied,           *"
"\n"
" * reproduced, translated or reduced to any medium without the prior       *"
"\n"
" * written consent of Valhalla.                                            *";

char notice[4096];

 void write_header(FILE *output)
{
   fprintf(output,
"/* *********************************************************************** *"
"\n"
" * File   : %-40s  Part of Valhalla MUD *"
"\n"
" * Version: %1.2f                                                           *"
"\n"
"%s"
"\n"
" *                                                                         *"
"\n"
"%s"
"\n"
"%s"
"\n"
"%s"
"\n"
" *                                                                         *"
"\n"
"%s"
"\n"
" * *********************************************************************** */"
"\n", file, version, author, purpose, bugs, status, notice);

}

int main(int argc, char *argv[])
{
   char buf[MAX_SIZE];
   char *start, *end;
   char c;
   int n;
   FILE *input, *output;

   strcpy(notice, notice1);

   input = stdin;
   output = stdout;

   if (argc == 2)
   {
      input = fopen(argv[1], "rb");
      if (input == NULL)
      {
	 printf("Error: No such file '%s'.\n", argv[1]);
	 exit(1);
      }

      output = fopen("tmp_hdr", "wb");
      if (input == NULL)
      {
	 printf("Error: Can't make temp file 'tmpheader'.\n");
	 exit(1);
      }

      strcpy(file, argv[1]);
   }

   n = fread(buf, sizeof(char), MAX_SIZE, input);
   c = buf[n-1];
   buf[n-1] = 0;
   start = strstr(buf, "/*");
   end = strstr(buf, "*/");
   if ((start == NULL) || (end == NULL) || (start[40] != '*'))
   {
      buf[n-1] = c;
      version = 1.0;
      write_header(output);
      fwrite(buf, sizeof(char), n, output);
   }
   else
   {
      char *pstr, *estr;

      pstr = strstr(buf, "Version:");
      if (pstr)
      {
	 sscanf(pstr + 9, " %f ", &version);
      }

      pstr = strstr(buf, " * Author : ");
      if (pstr)
      {
	 estr = strstr(pstr + 11, "*\n");
	 if (estr)
	 {
	    strncpy(author, pstr, estr-pstr);
	    author[estr - pstr+1] = 0;
	 }

	 author[75] = '*';
	 author[76] = 0;
      }

      pstr = strstr(buf, " * Status : ");
      if (pstr)
      {
	 if (strstr(pstr + 11, "Unpublished"))
	   strcpy(notice, notice1);
	 else if (strstr(pstr + 11, "Published"))
	   strcpy(notice, notice2);

	 estr = strstr(pstr + 11, "*\n");
	 if (estr)
	 {
	    strncpy(status, pstr, estr-pstr);
	    status[estr - pstr+1] = 0;
	 }

	 status[75] = '*';
	 status[76] = 0;
      }

      pstr = strstr(buf, " * Purpose:");
      if (pstr)
      {
	 estr = strstr(pstr + 11, "\n * Bugs");
	 if (estr)
	 {
	    strncpy(purpose, pstr, estr-pstr);
	    purpose[estr - pstr+1] = 0;
	 }
      }

      pstr = strstr(buf, " * Bugs   :");
      if (pstr)
      {
	 estr = strstr(pstr + 11, "\n * Valhalla ");
	 if (estr)
	 {
	    strncpy(bugs, pstr, estr-pstr);
	    bugs[estr - pstr+1] = 0;
	 }
      }
	

      buf[n-1] = c;
      write_header(output);
      fwrite(end+3, sizeof(char), n - (end-buf) - 3, output);
   }

   while (!feof(input))
   {
      n = fread(buf, sizeof(char), MAX_SIZE, input);
      if (n > 0)
	fwrite(buf, sizeof(char), n, output);
   }

   n = rename("tmp_hdr", file);
   assert(n == 0);

   return 0;
}
