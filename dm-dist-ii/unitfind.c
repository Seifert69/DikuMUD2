/* *********************************************************************** *
 * File   : unitfind.c                                Part of Valhalla MUD *
 * Version: 2.24                                                           *
 * Author : Var.                                                           *
 *                                                                         *
 * Purpose: Basic routines for finding units.                              *
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
#include <time.h>

#include "utils.h"
#include "db.h"
#include "unitfind.h"
#include "handler.h"
#include "textutil.h"
#include "utility.h"
#include "common.h"

/* Assumes UNIT_IN(room) == NULL */
static ubit1 same_surroundings_room(struct unit_data *room,
				    struct unit_data *u2)
{
   if (!UNIT_IN(u2))
     return FALSE;

   if (UNIT_IN(u2) == room)
     return TRUE;

   if (UNIT_IS_TRANSPARENT(UNIT_IN(u2)) &&
       UNIT_IN(UNIT_IN(u2)) == room)
     return TRUE;

   return FALSE;
}

ubit1 same_surroundings(struct unit_data *u1, struct unit_data *u2)
{
   if (!UNIT_IN(u1))
     return same_surroundings_room(u1, u2);
   else if (!UNIT_IN(u2))
     return same_surroundings_room(u2, u1);

   if (UNIT_IN(u1) == UNIT_IN(u2))
     return TRUE;

   if (UNIT_IS_TRANSPARENT(UNIT_IN(u1)) && UNIT_IN(UNIT_IN(u1)) == UNIT_IN(u2))
     return TRUE;
  
   if (UNIT_IS_TRANSPARENT(UNIT_IN(u2)) && UNIT_IN(UNIT_IN(u2)) == UNIT_IN(u1))
     return TRUE;

   if (UNIT_IS_TRANSPARENT(UNIT_IN(u1)) && UNIT_IS_TRANSPARENT(UNIT_IN(u2)) &&
       UNIT_IN(UNIT_IN(u1)) == UNIT_IN(UNIT_IN(u2)))
     return TRUE;
  
   return FALSE;
}


/* Does not accept multiple find flags as argument! */
/* Call several times if that is required           */
/* Fri Jan 24 06:04:47 PST 1997  HHS: 
   added FIND_UNIT_PAY and FIND_UNIT_NOPAY
       To locate paying/ nonpaying players OR 
          locate rooms in pay/nonpay zones
   When using FIND_UNIT_PAY/FIND_UNIT_NOPAY, you will
   only get interresting results when seaching for rooms or
   pc's. Thus searching _EQUIP would not give any units.
   _SURRO is fixed, so it finds a vild unit in the generated list,
     seaching first forward then backward from a random position,
     looking for a valig unit.
   The general findunit will ignore these flags!  
*/
/* Fri Jan 24 10:22:06 PST 1997  HHS:
   I fixed the selection of one unit, so it will not have to run through
   the list two times. And without using an array (like scan4unit)
   
   I 've analyzed the problem, each time you find a kandidate unit,
   you count (like before), but you choose a unit by a chance of
     p(i)= 1/(i+1) 
   where i is the number of units found so far... thus only one run :-).
   The result is that the likelihood of selecing the each is 1/maxcount. 
   (i've even tested it experimentally)
*/
   
/* returns if PC is pay/no pay !0/0 */
static inline int pcpay(struct unit_data *u)
{
   return ((PC_ACCOUNT(u).credit>0.0) ||
	   (PC_ACCOUNT(u).discount==100) ||
	   (PC_ACCOUNT(u).flatrate > (ubit32) time(0)) ||
           ( CHAR_DESCRIPTOR(u) ?
               g_cServerConfig.FromLAN(CHAR_DESCRIPTOR(u)->host) : 0));
}

/* returns if ROOM is pay/no pay !0/0 */
static inline int roompay(struct unit_data *u)
{
   return (UNIT_FI_ZONE(u)->payonly);
}

/* These functions determine if the units are candidates in find */
static inline int findcheck(struct unit_data *u, int pset, int tflags)
{
   if (IS_SET(UNIT_TYPE(u), tflags))
   {
      if (pset == FIND_UNIT_PAY)
      {
	 if (IS_PC(u) && pcpay(u))
	   return 1;

	 if (IS_ROOM(u) && roompay(u))
	   return 1;

	 return 0;
      }
      else if (pset == FIND_UNIT_NOPAY)
      {
	 if (IS_PC(u) && !pcpay(u))
	   return 1;

	 if (IS_ROOM(u) && !roompay(u))
	   return 1;

	 return 0;
      }
      else
	return 1;
   }

   return 0;
}

struct unit_data *random_unit(struct unit_data *ref, int sflags, int tflags)
{
   register struct unit_data *u, *selected=NULL;
   int count = 0;
   int pset  = 0;

   pset      = sflags & (FIND_UNIT_NOPAY | FIND_UNIT_PAY);
   sflags   &= FIND_UNIT_LOCATION_MASK;

   /* Needs work, there must be a better way to do this... */

   if (sflags == FIND_UNIT_WORLD)
   {
      for (u = unit_list; u; u = u->gnext)
        if ((u != ref) && findcheck(u, pset, tflags))
	{
	   count++;
	   if (number(1, count) == 1)
	     selected = u;
	}

      return selected;
   }
   else if (sflags == FIND_UNIT_ZONE)
   {
      struct zone_type *z;

      if (UNIT_FI_ZONE(ref))
        z = UNIT_FI_ZONE(ref);
      else
        z = unit_zone(ref);

      for (u = unit_list; u; u = u->gnext)
        if ((u != ref) &&
	    ((IS_PC(u) && unit_zone(u) == z) ||
	     (UNIT_FI_ZONE(u) == z)) &&
	    findcheck(u,pset,tflags)) {
	   count++;
	   if (number(1, count) == 1)
	     selected = u;
	}	  

      return selected;
   }
   else if (sflags == FIND_UNIT_INVEN)
   {
      for (u = UNIT_CONTAINS(ref); u; u = u->next)
	if ((!IS_OBJ(u) || (OBJ_EQP_POS(u) == 0)) &&
	    findcheck(u,pset,tflags))
	{
	   count++;
	   if (number(1, count) == count)
	     selected=u;
	}

      return selected;

   }
   else if (sflags == FIND_UNIT_EQUIP)
   {
      /* Should this return ONLY? objects? HHS */
      if (IS_SET(UNIT_ST_OBJ, tflags))
      {
         for (u = UNIT_CONTAINS(ref); u; u = u->next)
	   if (IS_OBJ(u) && OBJ_EQP_POS(u)) 
	     if (number(1, count) == count)
	       selected=u;

	 return selected;
      }
   }
   else if (sflags == FIND_UNIT_SURRO)
   {
      int i,p;
      scan4_unit(ref, tflags);
      if (UVITOP > 0) {
	 p=number(0, UVITOP-1);
	 /* find nearest that fulfill findcheck() */
	 for (i = p;i<UVITOP;i++)
	   if (findcheck(UVI(i),pset,tflags)) return UVI(i);
	 /* try lookin back then */
	 for (i = p-1;i>=0;i--)
	   if (findcheck(UVI(i),pset,tflags)) return UVI(i);
	 /* then give up (UV not containing any usefull
	    units (findcheck()) */
      }
   }

   return NULL;
}

/* As find_unit below, except visibility is relative to
   viewer with respect to CHAR_CAN_SEE */

struct unit_data *find_unit_general(const struct unit_data *viewer, 
			    const struct unit_data *ch, 
			    char **arg,
			    const struct unit_data *list,
			    const ubit32 bitvector)
{
   struct unit_data *best = NULL;
   int best_len = 0;
   ubit32 bitvectorm;

   int i, number, original_number;
   const char *ct = NULL;
   char name[256], *c;
   ubit1 is_fillword = TRUE;
   struct unit_data *u, *uu;

   /* Eliminate the 'pay' bits */
   bitvectorm = bitvector & FIND_UNIT_LOCATION_MASK;

   for (c = *arg; isaspace(*c); c++)
     ;

   /* Eliminate spaces and all "ignore" words */
   while (is_fillword)
   {
      for(i = 0; (name[i] = c[i]) && name[i] != ' '; i++)
	;
      name[i] = 0;

      if (search_block(name, fillwords, TRUE) < 0)
	is_fillword = FALSE;
      else
	c += i;

      for(; *c == ' '; c++)
	;
   }

   if (!*c)
     return NULL;

   str_remspc(c);

   for(i = 0; isdigit(name[i] = c[i]); i++)
     ;
   name[i] = 0;

   if (c[i] == '.')
   {
      number = original_number = atoi(name);
      c += i + 1;
   }
   else
     number = original_number = 1;

   if (IS_CHAR(ch))		/* Only check bitvector if ch IS a char! */
   {
      /* Equipment can only be objects. */
      if (IS_SET(bitvectorm, FIND_UNIT_EQUIP))
	for (u = UNIT_CONTAINS(ch); u; u = u->next)
	  if (IS_OBJ(u) && OBJ_EQP_POS(u) &&
	      ((viewer == ch) || CHAR_CAN_SEE(viewer, u)) &&
	      (CHAR_LEVEL(viewer) >= UNIT_MINV(u)) &&
	      (ct = UNIT_NAMES(u).IsNameRaw(c)) && (ct - c >= best_len))
	  {
	     if (ct - c > best_len)
	     {
		number = original_number;
		best_len = ct - c;
	     }

	     if (--number == 0)
	       best = u;
	  }

      if (IS_SET(bitvectorm, FIND_UNIT_INVEN))
	for (u = UNIT_CONTAINS(ch); u; u = u->next)
	  if ((ct = UNIT_NAMES(u).IsNameRaw(c)) &&
	      ((viewer == ch) || CHAR_CAN_SEE(viewer, u)) &&
	      (CHAR_LEVEL(viewer) >= UNIT_MINV(u)) &&
	      !(IS_OBJ(u) && OBJ_EQP_POS(u)) && (ct - c >= best_len))
	  {
	     if (ct - c > best_len)
	     {
		number = original_number;
		best_len = ct - c;
	     }

	     if (--number == 0)
	       best = u;
	  }

      /* This is the ugly one, modified for transparance */
      if (IS_SET(bitvectorm, FIND_UNIT_SURRO))
      {
	 const char *tmp_self[] = { "self", NULL };

	 if ((ct = is_name_raw(c, tmp_self)))
	 {
	    *arg = (char *) ct;
	    return (struct unit_data *) ch;
	 }

	 /* MS: Removed !IS_ROOM(UNIT_IN(ch)) because you must be able to
	        open rooms from the inside... */
	 if ((ct = UNIT_NAMES(UNIT_IN(ch)).IsNameRaw(c))
	     && CHAR_CAN_SEE(viewer, UNIT_IN(ch)) && (ct - c >= best_len))
	 {
	    if (ct - c > best_len)
	    {
	       number = original_number;
	       best_len = ct - c;
	    }

	    if (--number == 0)
	      best = UNIT_IN(ch);
	 }

	 /* Run through units in local environment */
	 for (u = UNIT_CONTAINS(UNIT_IN(ch)); u; u = u->next)
	 {
	    if (IS_ROOM(u) || CHAR_CAN_SEE(viewer,u)) /* Cansee room in dark */
	    {
	       if ((ct = UNIT_NAMES(u).IsNameRaw(c)) &&
		   (ct - c >= best_len))
	       {
		  if (ct - c > best_len)
		  {
		     number = original_number;
		     best_len = ct - c;
		  }
		 
		  if (--number == 0)
		    best = u;
	       }

	       /* check tranparancy */
	       if (UNIT_CHARS(u) && UNIT_IS_TRANSPARENT(u))
		 for (uu = UNIT_CONTAINS(u); uu; uu = uu->next)
		   if (IS_CHAR(uu) && (ct = UNIT_NAMES(uu).IsNameRaw(c)) &&
		       CHAR_CAN_SEE(viewer,uu) && (ct - c >= best_len))
		   {
		      if (ct - c > best_len)
		      {
			 number = original_number;
			 best_len = ct - c;
		      }
		 
		      if (--number == 0)
			best = uu;
		   }
	    }
	    

	 } /* End for */

	 /* Run through units in local environment if upwards transparent */
	 if ((u = UNIT_IN(UNIT_IN(ch))) && UNIT_IS_TRANSPARENT(UNIT_IN(ch)))
	 {
	    for (u = UNIT_CONTAINS(u); u; u = u->next)
	      if (u != UNIT_IN(ch) && CHAR_CAN_SEE(viewer,u))
	      {
		 if ((ct = UNIT_NAMES(u).IsNameRaw(c)) &&
		     (ct - c >= best_len))
		 {
		    if (ct - c > best_len)
		    {
		       number = original_number;
		       best_len = ct - c;
		    }
		 
		    if (--number == 0)
		      best = u;
		 }

		 /* check down into transparent unit */
		 if (UNIT_CHARS(u) && UNIT_IS_TRANSPARENT(u))
		   for (uu = UNIT_CONTAINS(u); uu; uu = uu->next)
		     if (IS_CHAR(uu) &&
			 (ct = UNIT_NAMES(uu).IsNameRaw(c)) &&
			 CHAR_CAN_SEE(viewer,uu) && (ct - c >= best_len))
		     {
			if (ct - c > best_len)
			{
			   number = original_number;
			   best_len = ct - c;
			}
			
			if (--number == 0)
			  best = uu;
		     }
	      }
	 }
      }

      if (IS_SET(bitvectorm, FIND_UNIT_ZONE))
	for (u = unit_list; u; u = u->gnext)
	  if ((ct = UNIT_NAMES(u).IsNameRaw(c)) && CHAR_CAN_SEE(viewer,u) &&
	      unit_zone(u) == unit_zone(ch) &&  (ct - c >= best_len))
	  {
	     if (ct - c > best_len)
	     {
		number = original_number;
		best_len = ct - c;
	     }
	     
	     if (--number == 0)
	       best = u;
	  }

      if (IS_SET(bitvectorm, FIND_UNIT_WORLD))
	for (u = unit_list; u; u = u->gnext)
	  if ((ct = UNIT_NAMES(u).IsNameRaw(c)) &&
	      CHAR_CAN_SEE(viewer, u) && (ct - c >= best_len))
	  {
	     if (ct - c > best_len)
	     {
		number = original_number;
		best_len = ct - c;
	     }
		 
	     if (--number == 0)
	       best = u;
	  }
   }

   for (; list; list = list->next)
     if ((ct = UNIT_NAMES((struct unit_data *) list).IsNameRaw(c))
	 && (ct - c >= best_len))
     {
	if (ct - c > best_len)
	{
	   number = original_number;
	   best_len = ct - c;
	}
		 
	if (--number == 0)
	  best = (struct unit_data *) list;
     }

   *arg = (c + best_len);

   return best;
}


/*
  Find Unit - General procedure to locate a unit.
  Ch        - The person looking for something
  Arg       - Address of a pointer to a string. Upon return the
  pointer to the string will point to the location
  after the name of the found object.
  List      - A pointer to a unit. This is used when for example
  a get xxx from bag is issued. List must then be the
  first object in the bag, and the list of objects in
  the bag will be searched.
  Bitvector - A bitvector (structs.h) designating where to look
  for a unit relative to Ch.

  Returns   - The unit which was found.

  Example:
  ch = Rainbird, arg -> "the 2.small bottle and the laugh".

  find_unit(ch, &arg, 0, FIND_UNIT_SURRO | FIND_UNIT_INVEN);
  Will try to find the second small bottle in the room or
  in the inventory.
  arg -> "and the laugh" if found.

  Example:
  ch = Rainbird, arg -> "the 2.small bottle from the bag",
  bag = UNIT_CONTAINS(bag)

  find_unit(ch, &arg, bag, 0);
  Will try to find the second small bottle from one of the
  elements in the "list" (bag).


  */

struct unit_data *find_unit(const struct unit_data *ch, char **arg,
			    const struct unit_data *list,
			    const ubit32 bitvector)
{
   return find_unit_general(ch, ch, arg, list, bitvector);
}


struct unit_data *find_symbolic_instance_ref(struct unit_data *ref,
					     struct file_index_type *fi,
					     ubit16 bitvector)
{
   register struct unit_data *u, *uu;

   if ((fi == NULL) || (ref == NULL))
     return NULL;
   
   if (IS_SET(bitvector, FIND_UNIT_EQUIP))
   {
      for (u = UNIT_CONTAINS(ref); u; u = u->next)
	if ((UNIT_FILE_INDEX(u) == fi) && UNIT_IS_EQUIPPED(u))
	  return u;
   }

   if (IS_SET(bitvector, FIND_UNIT_INVEN))
   {
      for (u = UNIT_CONTAINS(ref); u; u = u->next)
	if ((UNIT_FILE_INDEX(u) == fi) && !UNIT_IS_EQUIPPED(u))
	  return u;
   }

   if (IS_SET(bitvector, FIND_UNIT_SURRO) && UNIT_IN(ref))
   {
      if (fi == UNIT_FILE_INDEX(UNIT_IN(ref)))
	return UNIT_IN(ref);

      /* Run through units in local environment */
      for (u = UNIT_CONTAINS(UNIT_IN(ref)); u; u = u->next)
      {
	 if (UNIT_FILE_INDEX(u) == fi)
	   return u;

	 /* check tranparancy */
	 if (UNIT_CHARS(u) && UNIT_IS_TRANSPARENT(u))
	   for (uu = UNIT_CONTAINS(u); uu; uu = uu->next)
	     if (UNIT_FILE_INDEX(uu) == fi)
	       return uu;
      }

      /* Run through units in local environment if upwards transparent */
      if ((u = UNIT_IN(UNIT_IN(ref))) && UNIT_IS_TRANSPARENT(UNIT_IN(ref)))
      {
	 for (u = UNIT_CONTAINS(u); u; u = u->next)
	   if (u != UNIT_IN(ref))
	   {
	      if (fi == UNIT_FILE_INDEX(u))
		return u;
	      
	      /* check down into transparent unit */
	      if (UNIT_CHARS(u) && UNIT_IS_TRANSPARENT(u))
		for (uu = UNIT_CONTAINS(u); uu; uu = uu->next)
		  if (fi == UNIT_FILE_INDEX(uu))
		    return uu;
	   }
      }
   }

   if (IS_SET(bitvector, FIND_UNIT_ZONE))
   {
      for (u = unit_list; u; u = u->gnext)
	if ((unit_zone(u) == fi->zone) && (UNIT_FILE_INDEX(u) == fi))
	  return u;
   }

   if (IS_SET(bitvector, FIND_UNIT_WORLD))
   {
      for (u = unit_list; u; u = u->gnext)
	if ((UNIT_FILE_INDEX(u) == fi))
	  return u;
   }

   return NULL;
}


struct unit_data *find_symbolic_instance(struct file_index_type *fi)
{
   struct unit_data *u;

   if (fi == NULL)
     return NULL;

   for (u = unit_list; u; u = u->gnext)
     if ((UNIT_FILE_INDEX(u) == fi))
       return u;

   return NULL;
}


struct unit_data *find_symbolic(char *zone, char *name)
{
   return find_symbolic_instance(find_file_index(zone, name));
}


struct unit_vector_data unit_vector;

/* Init the unit_vector for FIRST use */
static void init_unit_vector(void)
{
  unit_vector.size = 10;

  CREATE(unit_vector.units, struct unit_data *, unit_vector.size);
}

/* If things get too cramped, double size of unit_vector */
static void double_unit_vector(void)
{
   unit_vector.size *= 2;

   RECREATE(unit_vector.units, struct unit_data *, unit_vector.size);
}


/* Scan the chars surroundings and all transparent surroundings for all  */
/* units of types which match 'flags' in the 'room' specified.           */
/* Difference to scan4_unit is that a room is searched for contents,     */
/* but not outside room.                                                 */
void scan4_unit_room(struct unit_data *room, ubit8 type)
{
   struct unit_data *u, *uu;
   
   unit_vector.top = 0;
   
   if (unit_vector.size == 0)
     init_unit_vector();
   
   for (u = UNIT_CONTAINS(room); u; u = u->next)
   {
      if (IS_SET(UNIT_TYPE(u), type))
      {
	 unit_vector.units[unit_vector.top++] = u;
	 if (unit_vector.size == unit_vector.top)
	   double_unit_vector();
      }

      /* down into transparent unit */
      if (UNIT_IS_TRANSPARENT(u))
	for (uu = UNIT_CONTAINS(u); uu; uu = uu->next)
	  if (IS_SET(UNIT_TYPE(uu), type))
	  {
	     unit_vector.units[unit_vector.top++] = uu;
	     if (unit_vector.size == unit_vector.top)
	       double_unit_vector();
	  }
   }
}


/* MS: Fixed bug in scan4_XXX, only assigned 'u' in last               */
/*     check. Now assigns 'uu' instead                                 */

/* Scan the chars surroundings and all transparent surroundsings for all */
/* units of types which match 'flags'. Updates the 'unit_vector' for     */
/* use in local routines.                                                */
void scan4_unit(struct unit_data *ch, ubit8 type)
{
   struct unit_data *u, *uu;

   if (!UNIT_IN(ch))
   {
      scan4_unit_room(ch, type);
      return;
   }

   unit_vector.top = 0;

   if (unit_vector.size == 0)
     init_unit_vector();

   for (u = UNIT_CONTAINS(UNIT_IN(ch)); u; u = u->next)
   {
      if (u != ch && IS_SET(UNIT_TYPE(u), type))
      {
	 unit_vector.units[unit_vector.top++] = u;
	 if (unit_vector.size == unit_vector.top)
	   double_unit_vector();
      }
     
      /* down into transparent unit */
      if (UNIT_IS_TRANSPARENT(u))
	for (uu = UNIT_CONTAINS(u); uu; uu = uu->next)
	  if (IS_SET(UNIT_TYPE(uu), type))
	  {
	     unit_vector.units[unit_vector.top++] = uu;
	     if (unit_vector.size == unit_vector.top)
	       double_unit_vector();
	  }
   }

   /* up through transparent unit */
   if (UNIT_IS_TRANSPARENT(UNIT_IN(ch)) &&
       UNIT_IN(UNIT_IN(ch)))
     for (u = UNIT_CONTAINS(UNIT_IN(UNIT_IN(ch))); u; u = u->next)
     {
	if (IS_SET(UNIT_TYPE(u), type))
	{
	   unit_vector.units[unit_vector.top++] = u;
	   if (unit_vector.size == unit_vector.top)
	     double_unit_vector();
	}

	/* down into transparent unit */
	if (UNIT_IS_TRANSPARENT(u) && u != UNIT_IN(ch))
	  for (uu = UNIT_CONTAINS(u); uu; uu = uu->next)
	    if (IS_SET(UNIT_TYPE(uu), type))
	    {
	       unit_vector.units[unit_vector.top++] = uu; /* MS FIX */
	       if (unit_vector.size == unit_vector.top)
		 double_unit_vector();
	    }
     }
}


static struct unit_data *scan4_ref_room(struct unit_data *room,
					struct unit_data *fu)
{
   struct unit_data *u, *uu;
   
   for (u = UNIT_CONTAINS(room); u; u = u->next)
   {
      if (u == fu)
	return fu;

      /* down into transparent unit */
      if (UNIT_IS_TRANSPARENT(u))
	for (uu = UNIT_CONTAINS(u); uu; uu = uu->next)
	  if (uu == fu)
	    return fu;
   }

   return NULL;
}


/* Scan the chars surroundings and all transparent surroundings for any   */
/* unit pointer occurance matching *fu. Return NULL or fu. Used only when */
/* you do not know wheter or not *fu has been extracted from the game. If */
/* you know that *fu exists, then a much simpler test is possible using   */
/* the 'same_surroundings()' function.                                     */
/* No checks for invisibility and the like                                */
struct unit_data *scan4_ref(struct unit_data *ch, struct unit_data *fu)
{
   struct unit_data *u, *uu;

   if (!UNIT_IN(ch))
     return scan4_ref_room(ch, fu);

   /* Scan ch's contents and transparent contents */
   if (scan4_ref_room(ch, fu))
     return fu;

   if (UNIT_IN(ch) == fu)
     return fu;

   /* up through transparent unit */
   for (u = UNIT_CONTAINS(UNIT_IN(ch)); u; u = u->next)
   {
      if (u == fu)
	return fu;

      /* down into transparent unit */
      if (UNIT_IS_TRANSPARENT(u))
	for (uu = UNIT_CONTAINS(u); uu; uu = uu->next)
	  if (uu == fu)
	    return fu;
   }
   
   /* up through transparent unit */
   if (UNIT_IS_TRANSPARENT(UNIT_IN(ch)) &&
       UNIT_IN(UNIT_IN(ch)))
     for (u = UNIT_CONTAINS(UNIT_IN(UNIT_IN(ch))); u; u = u->next)
     {
	if (u == fu)
	  return fu;

	/* down into transparent unit */
	if (UNIT_IS_TRANSPARENT(u) && u != UNIT_IN(ch))
	  for (uu = UNIT_CONTAINS(u); uu; uu = uu->next)
	    if (uu == fu)
	      return fu;
     }
   
   return NULL;
}


/* Return a random direction that 'unit' can go or -1 */
/* Tests for death rooms and water                    */
/* No longer tests for death rooms.                   */
int random_direction(struct unit_data *ch)
{
  int i, dirs[6], top;

  if (!IS_ROOM(UNIT_IN(ch)))
    return -1;

  for (top = i = 0; i < 6; i++)
    if (ROOM_EXIT(UNIT_IN(ch), i)  &&
	ROOM_EXIT(UNIT_IN(ch), i)->to_room  &&
	!IS_SET(ROOM_EXIT(UNIT_IN(ch), i)->exit_info, EX_CLOSED)  &&
	ROOM_LANDSCAPE(ROOM_EXIT(UNIT_IN(ch), i)->to_room) != SECT_WATER_SAIL)
      dirs[top++] = i;

  if (top > 0)
    return dirs[number(0, top - 1)];
  else
    return -1;
}
