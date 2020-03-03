/* *********************************************************************** *
 * File   : extra.c                                   Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert                                                        *
 *                                                                         *
 * Purpose: Extra description handling                                     *
 * Bugs   : None known                                                     *
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

#include "extra.h"
#include "utils.h"
#include "utility.h"
#include "textutil.h"

extra_descr_data::extra_descr_data(void)
{
   next = NULL;
}

extra_descr_data::~extra_descr_data(void)
{
   next = NULL;
}

void extra_descr_data::AppendBuffer(CByteBuffer *pBuf)
{
   ubit8 u;
   int i = 0;
   ubit32 nOrgPos, nPos;

   nOrgPos = pBuf->GetLength();
   
   pBuf->Append8(0);  /* Assume no extra description */

   struct extra_descr_data *e = this;

   /* While description is non null, keep writing */
   for (; e; e = e->next)
   {
      i++;
      pBuf->AppendString(e->descr.StringPtr());
      e->names.AppendBuffer(pBuf);
   }

   assert(i <= 255);

   if (i > 0)
   {
      nPos = pBuf->GetLength();
      pBuf->SetLength(nOrgPos);
      u = i;
      pBuf->Append8(u);  /* Assume no extra description */
      pBuf->SetLength(nPos);
   }
}



void extra_descr_data::free_list(void)
{
   class extra_descr_data *tmp_descr, *ex;

   ex = this;

   while (ex)
   {
      tmp_descr = ex;
      ex = ex->next;
      delete tmp_descr;
   }
}


/*                                                            */
/* Only matches on complete letter by letter match!           */
/* The empty word "" or NULL matches an empty namelist.       */
/*                                                            */

class extra_descr_data *extra_descr_data::find_raw(const char *word)
{
   ubit32 i;

   // MS2020 warning. if (this == 0) return NULL;

   for (class extra_descr_data *exd = this; exd; exd = exd->next)
   {
      if (exd->names.Length() < 1)
      {
	 if (str_is_empty(word))
	   return exd;
      }
      else
      {
	 for (i = 0; i < exd->names.Length(); i++)
	   if (str_ccmp(word, exd->names.Name(i)) == 0)
	     return exd;
      }
   }

   return NULL;
}


/*                                                            */
/* Adds an extra description just before the one given (this) */
/*                                                            */

class extra_descr_data * extra_descr_data::add(const char **names,
					       const char *descr)
{
   class extra_descr_data *new_ex;

   new_ex = new(class extra_descr_data);

   if (names)
     new_ex->names.CopyList(names);

   if (!str_is_empty(descr))
     new_ex->descr.Reassign(descr);

   new_ex->next  = this;

   return new_ex;
}


/*                                                            */
/* Adds an extra description just before the one given (this) */
/*                                                            */

class extra_descr_data * extra_descr_data::add(const char *name, const char *descr)
{
   const char *names[2];
   
   names[0] = name;
   names[1] = NULL;

   return add(names, descr);
}


class extra_descr_data *extra_descr_data::remove(class extra_descr_data *exd)
{
   class extra_descr_data *list = this;

   assert(list && exd);

   if (list == exd)
   {
      list = exd->next;
      delete exd;

      return list;
   }
   else
   {
      class extra_descr_data *pex;
      
      for (pex = list; pex && pex->next; pex = pex->next)
	if (pex->next == exd)
	{
	   pex->next = exd->next;
	   delete exd;
	   return list;
	}
   }

   /* This should not be possible */

   error(HERE, "Remove extra descr got unmatching list and exd.\n");
   return NULL;
}


class extra_descr_data *extra_descr_data::remove(const char *name)
{
   // MS2020 warning if (this == NULL)     return NULL;

   class extra_descr_data *tex = find_raw(name);

   if (tex)
     return this->remove(tex);
   else
     return this;
}


/* ===================================================================== */
/*   The following should really be in the unit_data class, but we dont  */
/*   have it yet...                                                      */


/*  We don't want people to be able to see $-prefixed extras
 *  so check for that...
 */
class extra_descr_data *unit_find_extra(const char *word,
					class unit_data *unit)
{
   word = skip_spaces(word);

   if (unit && *word != '$')
   {
      class extra_descr_data *i;

      word = skip_spaces(word);

      for (i = UNIT_EXTRA_DESCR(unit); i; i = i->next)
      {
	 if (i->names.Name())
	 {
	    if (i->names.Name(0)[0] == '$')
	      continue;

	    if (i->names.IsName((char *) word))
	      return i;
	 }
	 else if (UNIT_NAMES(unit).IsName((char *) word))
	   return i;
      }
   }

   return NULL;
}


class extra_descr_data *char_unit_find_extra(class unit_data *ch,
					     class unit_data **target,
					     char *word,
					     class unit_data *list)
{
   class extra_descr_data *exd = NULL;

   for (; list; list = list->next)
     if (CHAR_CAN_SEE(ch, list) && (exd = unit_find_extra(word, list)))
     {
	if (target)
	  *target = list;
	return exd;
     }

   if (target)
     *target = NULL;

   return NULL;
}

const char *unit_find_extra_string(class unit_data *ch,
				   char *word, class unit_data *list)
{
   class extra_descr_data *exd =
     char_unit_find_extra(ch, NULL, word, list);

   if (exd)
     return exd->descr.String();

   return NULL;
}


