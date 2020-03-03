/* *********************************************************************** *
 * File   : namelist.c                                Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: C++ Class of the popular Namelist                              *
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

#include <ctype.h>
#include <string.h>

#include "namelist.h"
#include "essential.h"
#include "textutil.h"
#include "db_file.h"

cNamelist::cNamelist(void)
{
   namelist = NULL;
   length = 0;
}

void cNamelist::CopyList(const char * const * const list)
{
   if (list)
     for (int i = 0; list[i]; i++)
       AppendName(list[i]);
}

void cNamelist::CopyList(class cNamelist *cn)
{
   for (ubit32 i = 0; i < cn->Length(); i++)
     AppendName(cn->Name(i));
}


void cNamelist::AppendBuffer(CByteBuffer *pBuf)
{
   for (ubit32 i = 0; i < length; i++)
     if (!str_is_empty(Name(i)))
       pBuf->AppendString(Name(i));
   
   pBuf->AppendString("");
}


int cNamelist::ReadBuffer(CByteBuffer *pBuf)
{
   char *c;

   for (;;)
   {
      if (pBuf->SkipString(&c))
        return 1;

      if (*c)
	AppendName(c);
      else
	break;
   }

   return 0;
}


void cNamelist::bread(ubit8 **b)
{
   char *c;

   for (;;)
   {
      c = bread_str_skip(b);

      if (*c)
        AppendName(c);
      else
        break;
   }
}


void cNamelist::catnames(char *s)
{
   ubit32 i;
   ubit1 ok = FALSE;

   strcpy(s, "{");
   TAIL(s);
   if (Length() > 0)
   {
      for (i = 0; i < Length(); i++)
      {
	 ok = TRUE;
	 sprintf(s, "\"%s\",", Name(i));
	 TAIL(s);
      }
      if (ok)
	s--;			/* remove the comma */
   }
   strcpy(s, "}");
}

void cNamelist::Remove(ubit32 idx)
{
   if (length > idx)
   {
      delete namelist[idx];
      namelist[idx] = namelist[length-1];
      length--;
      if (length == 0)
      {
	 free(namelist);
	 namelist = NULL;
      }
   }
}

void cNamelist::RemoveName(const char *name)
{
   register ubit32 i, j;

   for (i=0; i < length; i++)
   {
      for (j=0; namelist[i]->String()[j]; j++)
	if ( tolower(name[j]) != tolower(namelist[i]->String()[j]) )
	  break;

      if (!namelist[i]->String()[j])
	if ( !name[j] || isaspace(name[j]) )
	{
	   Remove(i);
	   return;
	}
   }
}

void cNamelist::Substitute(ubit32 idx, const char *newname)
{
   if (length > idx)
   {
      delete namelist[idx];
      namelist[idx] = new cStringInstance(newname);
   }
}

cNamelist::cNamelist(const char **list)
{
   namelist = NULL;
   length   = 0;

   CopyList(list);
}


void cNamelist::Free(void)
{
   ubit32 i;

   for (i=0; i < length; i++)
     delete namelist[i];

   if (namelist)
     free(namelist);

   namelist = NULL;
   length   = 0;
}


cNamelist::~cNamelist(void)
{
   Free();
}


cNamelist *cNamelist::Duplicate(void)
{
   cNamelist *pNl = new cNamelist;
   ubit32 i = 0;

   for (i = 0; i < length; i++)
     pNl->AppendName((char *) namelist[i]->String());

   return pNl;
}


const char *cNamelist::IsNameRaw(const char *name)
{
   register ubit32 i, j;

   for (i=0; i < length; i++)
   {
      for (j=0; namelist[i]->String()[j]; j++)
	if ( tolower(name[j]) != tolower(namelist[i]->String()[j]) )
	  break;

      if (!namelist[i]->String()[j])
	if ( !name[j] || isaspace(name[j]) )
	  return ((char *) name)+j;
   }

   return NULL;
}


const char *cNamelist::IsName(const char *name)
{
   char buf[MAX_STRING_LENGTH];

   name = skip_spaces(name);

   if (str_is_empty(name))
     return 0;

   strcpy(buf, name);
   str_remspc(buf);

   return IsNameRaw(buf);
}

/* Returns -1 if no name matches, or 0.. for the index in the namelist */
const int cNamelist::IsNameRawIdx(const char *name)
{
   register ubit32 i, j;

   for (i=0; i < length; i++)
   {
      for (j=0; namelist[i]->String()[j]; j++)
	if ( tolower(name[j]) != tolower(namelist[i]->String()[j]) )
	  break;

      if (!namelist[i]->String()[j])
	if ( !name[j] || isaspace(name[j]) )
	  return i;
   }

   return -1;
}

/* As IsNameRawIdx */
const int cNamelist::IsNameIdx(const char *name)
{
   char buf[MAX_STRING_LENGTH];

   name = skip_spaces(name);

   if (str_is_empty(name))
     return -1;

   strcpy(buf, name);
   str_remspc(buf);

   return IsNameRawIdx(buf);
}


const char *cNamelist::Name(ubit32 idx)
{
   if (idx < length)
     return namelist[idx]->String();

   return NULL;
}

cStringInstance *cNamelist::InstanceName(ubit32 idx)
{
   if (idx < length)
     return namelist[idx];

   return NULL;
}

void cNamelist::AppendName(const char *name)
{
   if (!str_is_empty(name))
   {
      length++;

      if (namelist == NULL)
	CREATE(namelist, class cStringInstance *, length);
      else
	RECREATE(namelist, class cStringInstance *, length);

      namelist[length - 1]     = new cStringInstance(name);
   }
}

void cNamelist::PrependName(const char *name)
{
   if (!str_is_empty(name))
   {
      length++;

      if (namelist == NULL)
	CREATE(namelist, class cStringInstance *, length);
      else
	RECREATE(namelist, class cStringInstance *, length);

      if (length > 1)
	memmove(&namelist[1], &namelist[0],
		sizeof(class cStringInstance *) * (length - 1));

      namelist[0]     = new cStringInstance(name);
   }
}

