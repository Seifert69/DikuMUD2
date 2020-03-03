/* *********************************************************************** *
 * File   : dilexp.c                                  Part of Valhalla MUD *
 * Version: 2.00                                                           *
 * Author : bombman@diku.dk & seifert@diku.dk				   *
 *                                                                         *
 *                                                                         *
 * Purpose: DIL Expressions.		       	                           *
 *									   *
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

/* Wed Jan 22 14:57:30 PST 1997   HHS added paycheck dilfun */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h> /* For type_check */

#include "structs.h"
#include "handler.h"
#include "textutil.h"
#include "interpreter.h"
#include "comm.h"
#include "movement.h"
#include "dil.h"
#include "affect.h"
#include "utility.h"
#include "utils.h"
#include "db.h"
#include "limits.h"
#include "common.h"
#include "spells.h"
#include "db_file.h"
#include "unixshit.h"
#include "dilexp.h"
#include "dilrun.h"
#include "money.h"
#include "magic.h"
#include "fight.h"
#include "skills.h"

struct time_info_data mud_date(time_t t);

/* ************************************************************************ */
/* DIL-expressions							    */
/* ************************************************************************ */

void dilfe_illegal(register struct dilprg *p, register class dilval *v)
{
   szonelog(UNIT_FI_ZONE(p->sarg->owner),
	    "DIL %s@%s, Illegal Expression/Instruction Node.\n",
	    UNIT_FI_NAME(p->sarg->owner), UNIT_FI_ZONENAME(p->sarg->owner));
   p->waitcmd = WAITCMD_QUIT;
   if (v) v->type=DILV_ERR; /* error value */
}


void dilfe_atsp(register struct dilprg *p, register class dilval *v)
{
   class dilval v1, v2, v3, v4, v5;

   eval_dil_exp(p, &v1);	/* spell number */
   eval_dil_exp(p, &v2);	/* caster */
   eval_dil_exp(p, &v3);	/* medium */
   eval_dil_exp(p, &v4);	/* target */
   eval_dil_exp(p, &v5);	/* bonus */

   v->type=DILV_INT;

   switch (dil_getval(&v1))
   {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
   }

   switch (dil_getval(&v2))
   {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_UP:
      if (!v2.val.ptr || !IS_CHAR((struct unit_data *)v2.val.ptr))
	v->type=DILV_FAIL;
      break;
     default:
      v->type=DILV_ERR;
      return;
   }

   switch (dil_getval(&v3))
   {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_UP:
      if (!v3.val.ptr)
	v->type=DILV_FAIL;
      break;
     default:
      v->type=DILV_ERR;
      return;
   }

   switch (dil_getval(&v4))
   {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_UP:
      if (!v4.val.ptr)
	v->type=DILV_FAIL;
      break;
     default:
      v->type=DILV_ERR;
      return;
   }

   switch (dil_getval(&v5))
   {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_INT:
      break;
     default:
      v->type=DILV_ERR;
      return;
   }

   if (is_in(v1.val.num, SPL_GROUP_MAX, SPL_TREE_MAX-1) &&
       (spell_info[v1.val.num].spell_pointer ||
	spell_info[v1.val.num].tmpl) &&
       v->type == DILV_INT)
   {
      struct spell_args sa;

      set_spellargs(&sa,
		    (struct unit_data *) v2.val.ptr,
		    (struct unit_data *) v3.val.ptr,
		    (struct unit_data *) v4.val.ptr, NULL, 0);
      sa.pEffect = NULL;

      /* cast the spell */
      v->val.num = spell_offensive(&sa, v1.val.num, v5.val.num);
      dil_test_secure(p);
   }
}


void dilfe_cast2(register struct dilprg *p, register class dilval *v)
{
   class dilval v1, v2, v3, v4, v5;

   eval_dil_exp(p, &v1);	/* spell number */
   eval_dil_exp(p, &v2);	/* caster       */
   eval_dil_exp(p, &v3);	/* medium       */
   eval_dil_exp(p, &v4);	/* target       */
   eval_dil_exp(p, &v5);	/* Display?     */

   v->type=DILV_INT;

   switch (dil_getval(&v1))
   {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
   }

   switch (dil_getval(&v2))
   {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_UP:
      if (!v2.val.ptr || !IS_CHAR((struct unit_data *)v2.val.ptr))
	v->type=DILV_FAIL;
      break;
     default:
      v->type=DILV_ERR;
      return;
   }

   switch (dil_getval(&v3))
   {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_UP:
      if (!v3.val.ptr)
	v->type=DILV_FAIL;
      break;
     default:
      v->type=DILV_ERR;
      return;
   }

   switch (dil_getval(&v4))
   {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_UP:
      if (!v4.val.ptr)
	v->type=DILV_FAIL;
      break;
     default:
      v->type=DILV_ERR;
      return;
   }

   switch (dil_getval(&v5))
   {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_SP:
      break;
     default:
      v->type=DILV_ERR;
      return;
   }

   if (v->type == DILV_INT &&
       is_in(v1.val.num, SPL_GROUP_MAX, SPL_TREE_MAX-1) &&
       (spell_info[v1.val.num].spell_pointer ||
	spell_info[v1.val.num].tmpl))
   {
      /* cast the spell */
      char mbuf[MAX_INPUT_LENGTH] = {0};
      v->val.num = spell_perform(v1.val.num, MEDIA_SPELL,
				 (struct unit_data *) v2.val.ptr,
				 (struct unit_data *) v3.val.ptr,
				 (struct unit_data *) v4.val.ptr,
				 mbuf, (char *) v5.val.ptr);
      dil_test_secure(p);
   }
}

void dilfe_rest(register struct dilprg *p, register class dilval *v)
{
   class dilval v1, v2;

   struct unit_data *restore_unit(char *zonename, char *unitname);

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   v->type = DILV_UP;

   switch (dil_getval(&v1))
   {
     case DILV_SP:
      break;
     default:
      v->type=DILV_ERR;
      return;
   }

   switch (dil_getval(&v2))
   {
     case DILV_SP:
      break;
     default:
      v->type=DILV_ERR;
      return;
   }

   if (v->type == DILV_UP)
   {
      v->val.ptr = restore_unit((char*) v1.val.ptr, (char *) v2.val.ptr);
      if (v->val.ptr == NULL)
      {
	 v->type = DILV_NULL;
      }
      else
      {
	 unit_to_unit((struct unit_data *) v->val.ptr, p->owner);
      }
   }
}



void dilfe_opro(register struct dilprg *p, register class dilval *v)
{
   class dilval v1, v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   v->type = DILV_INT;

   switch (dil_getval(&v1))
   {
     case DILV_INT:
      break;
     default:
      v->type=DILV_ERR;
      return;
   }

   switch (dil_getval(&v2))
   {
     case DILV_INT:
      break;
     default:
      v->type=DILV_ERR;
      return;
   }

   if (v->type == DILV_INT)
   {
      if ((v1.val.num > 0) && (v2.val.num > 0) &&
	  (v2.val.num < v1.val.num/2 - 1))
	v->val.num = open_ended_roll(v1.val.num, v2.val.num);
      else
	v->val.num = 0;
   }
}



void dilfe_eqpm(register struct dilprg *p, register class dilval *v)
{
   class dilval v1, v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   v->type = DILV_UP;

   switch (dil_getval(&v1))
   {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_UP:
      if (!v1.val.ptr || !IS_CHAR((struct unit_data *)v1.val.ptr))
	v->type=DILV_FAIL;
      break;
   }

   switch (dil_getval(&v2))
   {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_INT:
      break;
     default:
      v->type=DILV_ERR;
      return;
   }

   if (v->type == DILV_UP)
   {
      v->val.ptr = equipment((struct unit_data *) v1.val.ptr, v2.val.num);
      if (v->val.ptr == NULL)
	v->type = DILV_NULL;
   }
}



/* int meleeAttack(unit, unit, int, int) */
void dilfe_mel(register struct dilprg *p, register class dilval *v)
{
   class dilval v1, v2, v3, v4;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   eval_dil_exp(p, &v3);
   eval_dil_exp(p, &v4);

   v->type = DILV_INT;

   switch (dil_getval(&v1))
   {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_UP:
      if (!v1.val.ptr || !IS_CHAR((struct unit_data *)v1.val.ptr))
	v->type=DILV_FAIL;
      break;
   }

   switch (dil_getval(&v2))
   {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_UP:
      if (!v2.val.ptr || !IS_CHAR((struct unit_data *)v2.val.ptr))
	v->type=DILV_FAIL;
      break;
     default:
      v->type=DILV_ERR;
      return;
   }

   switch (dil_getval(&v3))
   {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_INT:
      break;
     default:
      v->type=DILV_ERR;
      return;
   }

   switch (dil_getval(&v4))
   {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_INT:
      break;
     default:
      v->type=DILV_ERR;
      return;
   }

   if (v->type == DILV_INT)
   {
      v->val.num = one_hit((struct unit_data *) v1.val.ptr,
			   (struct unit_data *) v2.val.ptr,
			   v3.val.num, v4.val.num);
      dil_test_secure(p);
   }
}


/* visible, some vs other */
void dilfe_visi(struct dilprg *p, class dilval *v)
{
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1)
   eval_dil_exp(p, &v2);

   v->type=DILV_INT;
   switch (dil_getval(&v1)) {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_UP:
      if (!v1.val.ptr || !IS_CHAR((struct unit_data *)v1.val.ptr))
	v->type=DILV_FAIL;
      break;
     default:
      v->type=DILV_ERR;
      return;
   }
   switch (dil_getval(&v2)) {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_UP: 
      if (!v2.val.ptr)
	v->type=DILV_FAIL;
     break;
     default:
      v->type=DILV_ERR;
      return;
   }

   if (v->type==DILV_INT)
     v->val.num = CHAR_CAN_SEE((struct unit_data *)v1.val.ptr,
			       (struct unit_data *)v2.val.ptr);
}

/* is unit opponent of other */
void dilfe_oppo(struct dilprg *p, class dilval *v)
{
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1)
   eval_dil_exp(p, &v2);

   v->type=DILV_INT;
   switch (dil_getval(&v1)) {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_UP:
      if (!v1.val.ptr || !IS_CHAR((struct unit_data *) v1.val.ptr))
	v->type=DILV_FAIL;
      break;
     default:
      v->type=DILV_ERR;
      return;
   }
   switch (dil_getval(&v2)) {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_UP: 
      if (!v2.val.ptr || !IS_CHAR((struct unit_data *) v2.val.ptr))
	v->type=DILV_FAIL;
     break;
     default:
      v->type=DILV_ERR;
      return;
   }

   if (v->type==DILV_INT)
     v->val.num = (CHAR_COMBAT((struct unit_data *) v1.val.ptr) ?
		   CHAR_COMBAT((struct unit_data *) v1.val.ptr)->
		   FindOpponent((struct unit_data *)v2.val.ptr) != NULL
                   : FALSE);
}


/* spellindex */
void dilfe_splx(struct dilprg *p, class dilval *v)
{
   class dilval v1;

   eval_dil_exp(p, &v1)

   v->type=DILV_INT;

   switch (dil_getval(&v1))
   {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_SP:
      if (!v1.val.ptr)
	v->type=DILV_FAIL;
      break;
     default:
      v->type=DILV_ERR;
      return;
   }

   if (v->type==DILV_INT)
   {
      int i;
      char *c = (char *) v1.val.ptr;

      if (str_is_empty(c))
	v->val.num = -1;
      else
	v->val.num = search_block_abbrevs(c, spl_text, (const char **) &c);
   }
}

/* spellinfo */
void dilfe_spli(struct dilprg *p, class dilval *v)
{
   class dilval v1, v2, v3, v4, v5, v6, v7, v8;

   v->type = DILV_FAIL;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   eval_dil_exp(p, &v3);
   eval_dil_exp(p, &v4);
   eval_dil_exp(p, &v5);
   eval_dil_exp(p, &v6);
   eval_dil_exp(p, &v7);
   eval_dil_exp(p, &v8);

   switch (dil_getval(&v1))
   {
     case DILV_FAIL:
     case DILV_NULL:
      return;

     case DILV_INT:
      if (!is_in(v1.val.num, SPL_ALL, SPL_TREE_MAX-1))
      {
	 v->type=DILV_FAIL;
	 return;
      }
      break;

     default:
      v->type=DILV_ERR;
      return;
   }

   switch (v2.type)
   {
     case DILV_FAIL:
     case DILV_NULL:
      return;

     case DILV_SINT4R:
     case DILV_UINT4R:
      break;

     default:
      v->type=DILV_ERR;
      return;
   }

   switch (v3.type)
   {
     case DILV_FAIL:
     case DILV_NULL:
      return;

     case DILV_SINT4R:
     case DILV_UINT4R:
      break;

     default:
      v->type=DILV_ERR;
      return;
   }

   switch (v4.type)
   {
     case DILV_FAIL:
     case DILV_NULL:
      return;

     case DILV_SINT4R:
     case DILV_UINT4R:
      break;

     default:
      v->type=DILV_ERR;
      return;
   }

   switch (v5.type)
   {
     case DILV_FAIL:
     case DILV_NULL:
      return;

     case DILV_SINT4R:
     case DILV_UINT4R:
      break;

     default:
      v->type=DILV_ERR;
      return;
   }

   switch (v6.type)
   {
     case DILV_FAIL:
     case DILV_NULL:
      return;

     case DILV_SINT4R:
     case DILV_UINT4R:
      break;

     default:
      v->type=DILV_ERR;
      return;
   }

   switch (v7.type)
   {
     case DILV_FAIL:
     case DILV_NULL:
      return;

     case DILV_SINT4R:
     case DILV_UINT4R:
      break;

     default:
      v->type=DILV_ERR;
      return;
   }

   switch (v8.type)
   {
     case DILV_FAIL:
     case DILV_NULL:
      return;

     case DILV_SINT4R:
     case DILV_UINT4R:
      break;

     default:
      v->type=DILV_ERR;
      return;
   }

   /* pName := spellinfo(SPL_LIGHTNING_3, nRealm, nSphere, nMana, bOffensive,
      nResistType, bvMedia, bvTargets); */

   /* We're home free... */

   *((ubit32 *) v2.ref) = spell_info[v1.val.num].realm;
   *((ubit32 *) v3.ref) = spl_tree[v1.val.num].parent;
   *((ubit32 *) v4.ref) = spell_info[v1.val.num].usesmana;
   *((ubit32 *) v5.ref) = spell_info[v1.val.num].offensive;
   *((ubit32 *) v6.ref) = spell_info[v1.val.num].cast_type;
   *((ubit32 *) v7.ref) = spell_info[v1.val.num].media;
   *((ubit32 *) v8.ref) = spell_info[v1.val.num].targets;

   v->type = DILV_SP;
   v->atyp = DILA_EXP;
   v->val.ptr = str_dup(spl_text[v1.val.num] == NULL ? "" :
			spl_text[v1.val.num]);
}



/* contents of purse */
void dilfe_purs(struct dilprg *p, class dilval *v)
{
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1)
   eval_dil_exp(p, &v2);

   v->type=DILV_INT;
   switch (dil_getval(&v1)) {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_UP:
      if (!v1.val.ptr)
	v->type=DILV_FAIL;
      break;
     default:
      v->type=DILV_ERR;
      return;
   }

   switch (dil_getval(&v2)) {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_SP: 
      if ((!v2.val.ptr)||(!*((char *)v2.val.ptr)))
	v->type=DILV_FAIL;
      break;

     default: 
      v->type=DILV_ERR;
      return;
   }

   if (v->type==DILV_INT)
   {
      int i;

      v->val.num = 0;

      for (i = 0; i <= MAX_MONEY; i++)
	if (strcmp((char *) v2.val.ptr, money_types[i].abbrev) == 0)
	  break;

      if (i <= MAX_MONEY)
      {
	 /* Note down money-objects in from, and their values */
	 for (struct unit_data *tmp =
	      UNIT_CONTAINS((struct unit_data *) v1.val.ptr);
	      tmp; tmp = tmp->next)
	   if (IS_MONEY(tmp) && MONEY_TYPE(tmp) == i)
	   {
	      v->val.num = MONEY_AMOUNT(tmp);
	      break;
	   }
      }
   }
}

/* money_string */
void dilfe_mons(struct dilprg *p, class dilval *v)
{
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1)
   eval_dil_exp(p, &v2);

   v->type=DILV_SP;

   switch (dil_getval(&v1))
   {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_INT:
      break;
     default:
      v->type=DILV_ERR;
      return;
   }

   switch (dil_getval(&v2))
   {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_INT: 
      break;
     default: 
      v->type=DILV_ERR;
      return;
   }

   if (v->type == DILV_SP)
   {
      v->atyp = DILA_EXP;
      v->val.ptr = str_dup(money_string(v1.val.num, DEF_CURRENCY, v2.val.num));
   }
}

/* pathto */
void dilfe_path(struct dilprg *p, class dilval *v)
{
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1)
   eval_dil_exp(p, &v2);

   v->type=DILV_INT;

   switch (dil_getval(&v1))
   {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_UP:
      if (!v1.val.ptr)
	v->type=DILV_FAIL;
      break;
     default:
      v->type=DILV_ERR;
      return;
   }

   switch (dil_getval(&v2))
   {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_UP: 
      if (!v2.val.ptr)
	v->type=DILV_FAIL;
      break;

     default: 
      v->type=DILV_ERR;
      return;
   }

   if (v->type == DILV_INT)
   {
      struct unit_data *u1, *u2;
      u1 = unit_room((struct unit_data *) v1.val.ptr);
      u2 = unit_room((struct unit_data *) v2.val.ptr);

      v->val.num = move_to(u1,u2);
   }
}

/* can_carry */
void dilfe_cary(struct dilprg *p, class dilval *v)
{
   class dilval v1;
   class dilval v2;
   class dilval v3;

   eval_dil_exp(p, &v1)
   eval_dil_exp(p, &v2);
   eval_dil_exp(p, &v3);

   v->type=DILV_INT;

   switch (dil_getval(&v1))
   {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_UP:
      if (!v1.val.ptr || !IS_CHAR((struct unit_data *) v1.val.ptr))
	v->type=DILV_FAIL;
      break;
     default:
      v->type=DILV_ERR;
      return;
   }

   switch (dil_getval(&v2))
   {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_UP: 
      if (!v2.val.ptr)
	v->type=DILV_FAIL;
      break;

     default: 
      v->type=DILV_ERR;
      return;
   }

   switch (dil_getval(&v3))
   {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_INT: 
      if (v3.val.num < 1)
	v->type=DILV_FAIL;
      break;

     default: 
      v->type=DILV_ERR;
      return;
   }

   if (v->type==DILV_INT)
   {
      if (!char_can_carry_n((struct unit_data *) v1.val.ptr, v3.val.num))
	v->val.num = 1;
      else if (!char_can_carry_w((struct unit_data *) v1.val.ptr,
				 v3.val.num *
				 UNIT_WEIGHT((struct unit_data *) v2.val.ptr)))
	v->val.num = 2;
      else
	v->val.num = 0;
   }
}

/* transfermoney */
void dilfe_trmo(struct dilprg *p, class dilval *v)
{
   class dilval v1, v2, v3;

   eval_dil_exp(p, &v1)
   eval_dil_exp(p, &v2);
   eval_dil_exp(p, &v3);

   v->type = DILV_INT;
 
   switch (dil_getval(&v1)) {
     case DILV_FAIL:
      v->type=DILV_FAIL;
      break;
     case DILV_UP:
      if (v1.val.ptr && !IS_CHAR((struct unit_data *) v1.val.ptr))
	v->type=DILV_FAIL;
      break;

     case DILV_NULL:
      break;

     default:
      v->type = DILV_ERR;
      return;
   }

   switch (dil_getval(&v2)) {
     case DILV_FAIL:
      v->type=DILV_FAIL;
      break;
     case DILV_UP: 
      if (v2.val.ptr && !IS_CHAR((struct unit_data *) v2.val.ptr))
	v->type=DILV_FAIL;
     break;
     case DILV_NULL:
      break;
     default:
      v->type = DILV_ERR;
      return;
   }

   switch (dil_getval(&v3)) {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_INT: 
      break;
     default:
      v->type = DILV_ERR;
      return;
   }

   v->val.num = 0;

   if (v->type==DILV_INT && (v1.val.ptr || v2.val.ptr))
   {
      if (v1.val.ptr == NULL)
      {
	 slog(LOG_ALL, 0, "%s was given %d old gold pieces by DIL %s@%s.",
	      UNIT_NAME((struct unit_data *) v2.val.ptr), v3.val.num,
	      UNIT_FI_NAME(p->sarg->owner),
	      UNIT_FI_ZONENAME(p->sarg->owner));
	 money_transfer(NULL,
			(struct unit_data *) v2.val.ptr,
			v3.val.num,
			local_currency((struct unit_data *) v2.val.ptr));
	 v->val.num = 1;
      }
      else if (v2.val.ptr == NULL)
      {
	 if (char_can_afford((struct unit_data *) v1.val.ptr, v3.val.num,
			     local_currency((struct unit_data *) v1.val.ptr)))
	 {
	    money_transfer((struct unit_data *) v1.val.ptr, NULL,
			   v3.val.num,
			   local_currency((struct unit_data *) v1.val.ptr));
	    v->val.num = 1;
	 }
      }
      else
      {
	 if (char_can_afford((struct unit_data *) v1.val.ptr, v3.val.num,
			     local_currency((struct unit_data *) v2.val.ptr)))
	 {
	    money_transfer((struct unit_data *) v1.val.ptr,
			   (struct unit_data *) v2.val.ptr,
			   v3.val.num,
			   local_currency((struct unit_data *) v2.val.ptr));
	    v->val.num = 1;
	 }
      }
   }
}

/* transfermoney */
void dilfe_fits(struct dilprg *p, class dilval *v)
{
   class dilval v1, v2, v3;

   eval_dil_exp(p, &v1)
   eval_dil_exp(p, &v2);
   eval_dil_exp(p, &v3);

   v->type = DILV_SP;
 
   switch (dil_getval(&v1)) {
     case DILV_FAIL:
      v->type=DILV_FAIL;
      break;
     case DILV_UP:
      if (!v1.val.ptr || !IS_CHAR((struct unit_data *) v1.val.ptr))
	v->type=DILV_FAIL;
      break;

     case DILV_NULL:
      break;

     default:
      v->type = DILV_ERR;
      return;
   }

   switch (dil_getval(&v2)) {
     case DILV_FAIL:
      v->type=DILV_FAIL;
      break;
     case DILV_UP: 
      if (!v2.val.ptr || !IS_OBJ((struct unit_data *) v2.val.ptr))
	v->type=DILV_FAIL;
     break;
     case DILV_NULL:
      break;
     default:
      v->type = DILV_ERR;
      return;
   }

   switch (dil_getval(&v3)) {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_INT: 
      break;
     default:
      v->type = DILV_ERR;
      return;
   }

   if (v->type==DILV_SP)
   {
      char *obj_wear_size(struct unit_data *ch, struct unit_data *obj,
			  int keyword);

      char *c = obj_wear_size((struct unit_data *) v1.val.ptr,
					 (struct unit_data *) v2.val.ptr,
					 v3.val.num);

      v->atyp = DILA_EXP;
      v->val.ptr = str_dup(c == NULL ? "" : c);
   }
}

void dilfe_intr(struct dilprg *p, class dilval *v)
{
   /* add interrupt to current frame */
   ubit16 intnum;
   ubit8 *lab;

   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1);
   lab=p->sp->pc;
   eval_dil_exp(p, &v2); /* ignore */

   intnum = dil_intr_insert(p, lab, v1.val.num);

   if (v)
   {
      v->val.num=intnum;
      v->type = DILV_INT;
      v->atyp = DILA_NONE;
   }

   bread_ubit32(&(p->sp->pc)); /* skip label */
}


void dilfe_not(register struct dilprg *p, register class dilval *v)
{
   /* Negation of integers (and booleans, etc.) */
   class dilval v1;

   eval_dil_exp(p, &v1);
   v->type = DILV_INT;
   v->atyp = DILA_NONE;
   v->val.num = ! dil_getbool(&v1);
}

void dilfe_umin(register struct dilprg *p, register class dilval *v)
{
   /* Unary minus */
   class dilval v1;

   eval_dil_exp(p, &v1);

   switch (dil_getval(&v1))
   {
     case DILV_INT:
      v->atyp = DILA_NONE;
      v->type = DILV_INT;
      v->val.num = - v1.val.num;
      break;
     case DILV_FAIL:
      v->type = DILV_FAIL;
      break;
     default:
      v->type = DILV_ERR; /* wrong type */
      return;
   }
}

void dilfe_itoa(register struct dilprg *p, register class dilval *v)
{
   /* Conversion of integers to strings */
   class dilval v1;

   eval_dil_exp(p, &v1);
   switch (dil_getval(&v1))
   {
     case DILV_FAIL:
      v->type = DILV_FAIL;
      break;
     case DILV_INT:
      v->type = DILV_SP;
      v->atyp = DILA_EXP;
      v->val.ptr = str_dup(itoa(v1.val.num));
      break;
     default:
      v->type = DILV_ERR; /* wrong type */
      return;
   }
}

void dilfe_atoi(register struct dilprg *p, register class dilval *v)
{
   /* Conversion of strings to integers */
   class dilval v1;

   eval_dil_exp(p, &v1);
   switch (dil_getval(&v1))
   {
     case DILV_NULL:
     case DILV_FAIL:
      v->type = DILV_FAIL; /* failed */
      break;
     case DILV_SP:
      if (v1.val.ptr)
      {
	 v->atyp = DILA_NONE;
	 v->type = DILV_INT;
	 v->val.num = atoi((char *) v1.val.ptr);
      }
      else
	v->type = DILV_FAIL; /* failed */
      break;
     default:
      v->type = DILV_ERR; /* Wrong type */
      return;
   }
}

void dilfe_len(register struct dilprg *p, register class dilval *v)
{
   /* length of strings or stringlists */
   class dilval v1;

   eval_dil_exp(p, &v1);
   switch (dil_getval(&v1))
   {
      case DILV_NULL:
      case DILV_FAIL:
	 v->type = DILV_FAIL; /* failed */
	 break;
      case DILV_SP:
	 v->atyp = DILA_NONE;
	 v->type = DILV_INT;
	 if (v1.val.ptr)
	   v->val.num = strlen((char *) v1.val.ptr);
	 else
	   v->val.num = 0;
	 break;
      case DILV_SLP:
	 v->atyp = DILA_NONE;
	 v->type = DILV_INT;
	 if (v1.val.ptr)
	   v->val.num = ((cNamelist *) v1.val.ptr)->Length();
	 else
	   v->val.num = 0;
	 break;
      default:
	 v->type = DILV_ERR; /* Wrong type */
	 return;
   }
}

/* textformat */

void dilfe_txf(register struct dilprg *p, register class dilval *v)
{
   class dilval v1;

   eval_dil_exp(p, &v1); /* string */

   switch (dil_getval(&v1))
   {
     case DILV_NULL:
     case DILV_FAIL:
      v->type = DILV_FAIL; /* failed */
      break;

     case DILV_SP:
      if (v1.val.ptr)
      {
	 char dest[4096];

	 v->atyp = DILA_EXP;
	 v->type = DILV_SP;
	 str_escape_format((char *) v1.val.ptr, dest, sizeof(dest), TRUE);

	 v->val.ptr = str_dup(dest);
      }
      else
	v->type = DILV_FAIL; /* NULL string */
      break;

     default:
      v->type = DILV_ERR; /* wrong type */
      return;
   }
}


/* asctime */
void dilfe_ast(register struct dilprg *p, register class dilval *v)
{
   class dilval v1;
   char *c;

   eval_dil_exp(p, &v1); /* integer */

   switch (dil_getval(&v1))
   {
     case DILV_NULL:
     case DILV_FAIL:
      v->type = DILV_FAIL; /* failed */
      break;

     case DILV_INT:
      c = ctime((time_t *) &v1.val.num);
      assert(strlen(c) >= 1);

      v->atyp = DILA_EXP;
      v->type = DILV_SP;
      c[strlen(c)-1] = 0;
      v->val.ptr = str_dup(c);
      break;

     default:
      v->type = DILV_ERR; /* wrong type */
      return;
   }
}

void dilfe_getw(register struct dilprg *p, register class dilval *v)
{
   /* Get first word of a string */
   class dilval v1;
   char *c;
   char buf1[MAX_STRING_LENGTH];

   eval_dil_exp(p, &v1); /* string */

   switch (dil_getval(&v1))
   {
      case DILV_NULL:
      case DILV_FAIL:
	 v->type = DILV_FAIL; /* failed */
	 break;
      case DILV_SP:
	 if (v1.val.ptr)
	 {
	    char *str_next_word_copy(const char *argument, char *first_arg);

	    v->atyp = DILA_EXP;
	    v->type = DILV_SP;
	    c = str_next_word_copy((char *) v1.val.ptr, buf1);
	    c = skip_spaces(c);
	    v->val.ptr = str_dup(buf1);

	    if (v1.atyp == DILA_NORM && v1.type == DILV_SPR)
	      memmove(*(char **) v1.ref, c, strlen(c)+1);
	    else if (p->sarg->arg == v1.val.ptr)
	      p->sarg->arg = c; /* Update argument as in findunit :) */

	 }
	 else
	   v->type = DILV_FAIL; /* NULL string */
	 break;
      default:
	 v->type = DILV_ERR; /* wrong type */
	 return;
   }
}

void dilfe_getws(register struct dilprg *p, register class dilval *v)
{
   /* Get first word of a string */
   class dilval v1;
   char *tmp, *c;

   eval_dil_exp(p, &v1); /* string */

   switch (dil_getval(&v1))
   {
      case DILV_NULL:
      case DILV_FAIL:
	 v->type = DILV_FAIL; /* failed */
	 break;

      case DILV_SP:
	 if (v1.val.ptr)
	 {
	    cNamelist *words = new cNamelist;

	    v->atyp = DILA_EXP;
	    v->type = DILV_SLP;

	    tmp = str_dup((char *) v1.val.ptr);
	    for (c = strtok(tmp," "); c && *c ; c = strtok(NULL," "))
	      words->AppendName(c);
	    free(tmp);

	    v->val.ptr = words;
	 }
	 else
	    v->type = DILV_FAIL; /* NULL string */
	 break;

      default:
	 v->type = DILV_ERR; /* wrong type */
	 break;
   }
}

void dilfe_load(register struct dilprg *p, register class dilval *v)
{
   /* Load a unit from database */
   class dilval v1;

   eval_dil_exp(p, &v1); /* unit name */

   switch (dil_getval(&v1))
   {
      case DILV_NULL:
      case DILV_FAIL:
	 v->type = DILV_FAIL;
	 break;
      case DILV_SP:
	 if (v1.val.ptr)
	 {
	    v->val.ptr = read_unit(str_to_file_index((char *) v1.val.ptr));
	    if (v->val.ptr)
	    {
	       if (IS_MONEY((struct unit_data *) v->val.ptr))
		 set_money((struct unit_data *) v->val.ptr,
			   MONEY_AMOUNT((struct unit_data *) v->val.ptr));

	       unit_to_unit((struct unit_data *) v->val.ptr, p->sarg->owner);
	       v->atyp = DILA_NORM;
	       v->type = DILV_UP;
	    }
	    else
	       v->type = DILV_NULL; /* Unit not found */
	 }
	 else
	    v->type = DILV_FAIL; /* NULL string */
	 break;
      default:
	 v->type = DILV_ERR; /* wrong type */
	 break;
   }
}

void dilfe_plus(register struct dilprg *p, register class dilval *v)
{
   /* Addition of strings or integers */

   class dilval v1, v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   switch (dil_getval(&v2))
   {
     case DILV_NULL:
     case DILV_FAIL:
      v->type = DILV_FAIL;
      return;

     case DILV_INT:
      v->type = DILV_INT;
      break;

     case DILV_SP:
      v->type = DILV_SP;
      break;

     default:
      v->type = DILV_ERR; /* wrong type */
      return;
   }

   switch (dil_getval(&v1))
   {
     case DILV_FAIL:
      v->type = DILV_FAIL;
      return;

     case DILV_SP:
      if (v->type != DILV_SP)
	v->type = DILV_ERR; /* wrong type */
      break;

     case DILV_INT:
      if (v->type != DILV_INT)
	v->type = DILV_ERR; /* wrong type */
      break;

     case DILV_NULL:
      if (v->type != DILV_SP)
	v->type = DILV_ERR; /* wrong type */
      break;

     default:
      v->type = DILV_ERR; /* wrong type */
      return;
   }

   switch (v->type)
   {
     case DILV_SP:
      v->atyp = DILA_EXP;
      v->val.ptr = str_dup(str_cc(STR((char *) v1.val.ptr),
				  STR((char *) v2.val.ptr)));
      break;

     case DILV_INT:
      v->atyp = DILA_NONE;
      v->val.num = v1.val.num + v2.val.num;
      break;
   }
}


void dilfe_dld(register struct dilprg *p, register class dilval *v)
{
   /* Destruction of DIL programs */

   class dilval v1, v2;

   eval_dil_exp(p, &v1); /* string  */
   eval_dil_exp(p, &v2); /* unitptr */

   v->type = DILV_INT;

   switch (dil_getval(&v1))
   {
     case DILV_NULL:
     case DILV_FAIL:
      v->type = DILV_FAIL;
      return;

     case DILV_SP:
      break;

     default:
      v->type = DILV_ERR; /* wrong type */
      return;
   }

   switch (dil_getval(&v2))
   {
     case DILV_NULL:
     case DILV_FAIL:
      v->type = DILV_FAIL;
      return;

     case DILV_UP:
      break;

     default:
      v->type = DILV_ERR; /* wrong type */
      return;
   }

   if (v->type == DILV_INT)
   {
      if (v1.val.ptr && v2.val.ptr)
      {
	 v->atyp= DILA_NONE;
	 v->val.num = dil_destroy((char *) v1.val.ptr, 
				  (struct unit_data *) v2.val.ptr);
      }
   }
}


void dilfe_dlf(register struct dilprg *p, register class dilval *v)
{
   /* Detection of DIL programs (TRUE/FALSE) */

   class dilval v1, v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   switch (dil_getval(&v2))
   {
     case DILV_NULL:
     case DILV_FAIL:
      v->type = DILV_FAIL;
      return;

     case DILV_UP:
      v->type = DILV_INT;
      break;

     default:
      v->type = DILV_ERR; /* wrong type */
      return;
   }

   switch (dil_getval(&v1))
   {
     case DILV_NULL:
     case DILV_FAIL:
      v->type = DILV_FAIL;
      return;

     case DILV_SP:
      v->type = DILV_INT; /* wrong type */
      break;

     default:
      v->type = DILV_ERR; /* wrong type */
      return;
   }

   if (v1.val.ptr && v2.val.ptr) {
      v->atyp= DILA_NONE;
      if (dil_find((char *) v1.val.ptr, 
		   (struct unit_data *) v2.val.ptr))
	v->val.num = TRUE;
      else
	v->val.num = FALSE;
   }
}


void dilfe_min(register struct dilprg *p, register class dilval *v)
{
   /* Subtraction of integers */
   class dilval v1, v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   switch (dil_getval(&v2))
   {
     case DILV_FAIL:
      v->type = DILV_FAIL;
      break;
     case DILV_INT:
      v->type = DILV_INT;
      break;
     default:
      v->type = DILV_ERR; /* wrong type */
      return;
   }

   switch (dil_getval(&v1))
   {
     case DILV_FAIL:
      v->type = DILV_FAIL;
      break;

     case DILV_INT:
      break;

     default:
      v->type = DILV_ERR; /* wrong type */
      break;
   }

   if (v->type == DILV_INT)
   {
      v->atyp = DILA_NONE;
      v->val.num = v1.val.num - v2.val.num;
   }
}

void dilfe_mul(register struct dilprg *p, register class dilval *v)
{
   /* Multiplication of integers */
   class dilval v1, v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   switch (dil_getval(&v2))
   {
     case DILV_FAIL:
      v->type = DILV_FAIL;
      break;
     case DILV_INT:
      v->type = DILV_INT;
      break;
     default:
      v->type = DILV_ERR; /* wrong type */
      return;
   }

   switch (dil_getval(&v1))
   {
     case DILV_FAIL:
      v->type = DILV_FAIL;
      break;
     case DILV_INT:
      break;
     default:
      v->type = DILV_ERR; /* wrong type */
      return;
   }

   if (v->type == DILV_INT)
   {
      v->atyp = DILA_NONE;
      v->val.num = v1.val.num * v2.val.num;
   }
}

void dilfe_div(register struct dilprg *p, register class dilval *v)
{
   /* Division of integers */
   class dilval v1, v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   switch (dil_getval(&v2))
   {
     case DILV_FAIL:
      v->type = DILV_FAIL;
      break;
     case DILV_INT:
      v->type = DILV_INT;
      break;
     default:
      v->type = DILV_ERR; /* wrong type */
      return;
   }

   switch (dil_getval(&v1))
   {
      case DILV_FAIL:
      v->type = DILV_FAIL;
      break;
     case DILV_INT:
      break;
     default:
      v->type = DILV_ERR; /* wrong type */
      return;
   }

   if (v->type == DILV_INT)
   {
      v->atyp = DILA_NONE;
      if (v2.val.num == 0)
	 v->type = DILV_FAIL;
      else
	v->val.num = v1.val.num / v2.val.num;
   }
}

void dilfe_mod(register struct dilprg *p, register class dilval *v)
{
   /* Modulo of integers */
   class dilval v1, v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   switch (dil_getval(&v2))
   {
     case DILV_FAIL:
      v->type = DILV_FAIL;
      break;
     case DILV_INT:
      v->type = DILV_INT;
      break;
     default:
      v->type = DILV_ERR; /* wrong type */
      return;
   }

   switch (dil_getval(&v1))
   {
     case DILV_FAIL:
      v->type = DILV_FAIL;
      break;
     case DILV_INT:
      break;
     default:
      v->type = DILV_ERR; /* wrong type */
      return;
   }

   if (v->type == DILV_INT)
   {
      v->atyp = DILA_NONE;
      if (v2.val.num == 0)
	 v->type = DILV_NULL;
      else
	v->val.num = v1.val.num % v2.val.num;
   }
}

void dilfe_and(register struct dilprg *p, register class dilval *v)
{
   /* And two integers (or booleans) */
   class dilval v1, v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   v->type = DILV_INT;
   v->atyp = DILA_NONE;

   if ((dil_getval(&v1) == DILV_INT) && (dil_getval(&v2) == DILV_INT))
     v->val.num = v1.val.num & v2.val.num;
   else
     v->type = DILV_FAIL;
}

void dilfe_land(register struct dilprg *p, register class dilval *v)
{
   /* And two integers (or booleans) */
   class dilval v1, v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   v->type = DILV_INT;
   v->atyp = DILA_NONE;
   v->val.num = dil_getbool(&v1) && dil_getbool(&v2);
}


void dilfe_or(register struct dilprg *p,	  register class dilval *v)
{
   /* Or two integers (or booleans) */
   class dilval v1, v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   v->type = DILV_INT;
   v->atyp = DILA_NONE;

   if ((dil_getval(&v1) == DILV_INT) && (dil_getval(&v2) == DILV_INT))
     v->val.num = v1.val.num | v2.val.num;
   else
     v->type = DILV_FAIL;
}

void dilfe_lor(register struct dilprg *p,	  register class dilval *v)
{
   /* Or two integers (or booleans) */
   class dilval v1, v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   v->type = DILV_INT;
   v->atyp = DILA_NONE;
   v->val.num = dil_getbool(&v1) || dil_getbool(&v2);
}


void dilfe_isa(register struct dilprg *p, register class dilval *v)
{
   /* Test if unit is affected by affect */
   class dilval v1, v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   switch (dil_getval(&v1))
   {
      case DILV_NULL:
      case DILV_FAIL:
	 v->type = DILV_FAIL;
	 break;
      case DILV_UP:
	 if (v1.val.ptr)
	   v->type=DILV_INT;
	 else
	   v->type=DILV_FAIL;
	 break;
      default:
	 v->type=DILV_ERR; /* wrong type */
	 break;
   }

   switch (dil_getval(&v2))
   {
      case DILV_FAIL:
	 if (v->type != DILV_ERR)
	    v->type = DILV_FAIL;
	 break;
      case DILV_INT:
	 break;
      default:
	 v->type = DILV_ERR; /* wrong type */
	 break;
   }

   if (v->type == DILV_INT)
   {
      v->atyp = DILA_NONE;
      v->val.num = (affected_by_spell((struct unit_data *) v1.val.ptr,
				      v2.val.num) != NULL);
   }
}

void dilfe_rnd(register struct dilprg *p, register class dilval *v)
{
   /* Random in an integer range */
   class dilval v1, v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   switch (dil_getval(&v1))
   {
      case DILV_FAIL:
	 v->type = DILV_FAIL;
	 break;
      case DILV_INT:
	 v->type=DILV_INT;
	 break;
      default:
	 v->type=DILV_ERR; /* wrong type */
	 break;
   }

   switch (dil_getval(&v2))
   {
      case DILV_FAIL:
	 if (v->type != DILV_ERR)
	    v->type = DILV_FAIL;
	 break;
      case DILV_INT:
	 break;
      default:
	 v->type = DILV_ERR; /* wrong type */
	 break;
   }

   if (v->type == DILV_INT)
   {
      v->atyp = DILA_NONE;
      v->val.num = number(v1.val.num, v2.val.num);
   }
}

void dilfe_fndr(register struct dilprg *p, register class dilval *v)
{
   /* Find a room */
   class dilval v1;
   char buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];

   eval_dil_exp(p, &v1);
   switch (dil_getval(&v1))
   {
      case DILV_FAIL:
	 v->type = DILV_FAIL;
	 break;
      case DILV_SP:
	 v->atyp = DILA_NORM;
	 v->type = DILV_UP;
	 *buf1='\0';
	 *buf2='\0';
	 if (v1.val.ptr)
	 {
	   split_fi_ref((char *) v1.val.ptr,buf1,buf2);
	   v->val.ptr = world_room(buf1, buf2);
	   if (v->val.ptr == NULL)
	     v->type = DILV_NULL; /* not found */
	 }
	 else
	   v->type = DILV_NULL; /* not found */

	 break;
      case DILV_NULL:
	 v->type = DILV_NULL; /* not found */
	 break;
      default:
	 v->type = DILV_ERR; /* wrong type */
	 break;
   }
}

void dilfe_fnds2(register struct dilprg *p, register class dilval *v)
{
   /* Find a symbolic unit */
   class dilval v1, v2, v3;

   char buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   eval_dil_exp(p, &v3);

   switch (dil_getval(&v1))
   {
     case DILV_FAIL:
      if (v->type != DILV_ERR)
	v->type = DILV_FAIL;
      break;

     case DILV_UP:
      if (v1.val.ptr)
	v->type = DILV_UP;
      else
	v->type = DILV_FAIL;
      break;

     case DILV_NULL:
      v->type = DILV_NULL; /* not found */
      break;

     default:
      v->type = DILV_ERR; /* wrong type */
      break;
   }

   switch (dil_getval(&v2))
   {
     case DILV_FAIL:
      if (v->type != DILV_ERR)
	v->type = DILV_FAIL;
      break;

     case DILV_SP:
      if (v2.val.ptr == NULL)
	v->type = DILV_FAIL;
      break;

     case DILV_NULL:
      v->type = DILV_NULL; /* not found */
      break;

     default:
      v->type = DILV_ERR; /* wrong type */
      break;
   }

   switch (dil_getval(&v3))
   {
     case DILV_FAIL:
      if (v->type != DILV_ERR)
	v->type = DILV_FAIL;
      break;

     case DILV_INT:
      break;

     case DILV_NULL:
      v->type = DILV_NULL; /* not found */
      break;

     default:
      v->type = DILV_ERR; /* wrong type */
      break;
   }

   if (v->type == DILV_UP)
   {
      v->atyp = DILA_NORM;
      *buf1='\0';
      *buf2='\0';
      split_fi_ref((char *) v2.val.ptr, buf1, buf2);
      v->val.ptr = find_symbolic_instance_ref((struct unit_data *) v1.val.ptr,
					      find_file_index(buf1, buf2),
					      v3.val.num);
      if (v->val.ptr == NULL)
	v->type = DILV_NULL; /* not found */
   }
}

void dilfe_fnds(register struct dilprg *p, register class dilval *v)
{
   /* Find a symbolic unit */
   class dilval v1;
   char buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];

   eval_dil_exp(p, &v1);
   switch (dil_getval(&v1))
   {
      case DILV_FAIL:
	 if (v->type != DILV_ERR)
	    v->type = DILV_FAIL;
	 break;
      case DILV_SP:
	 v->atyp = DILA_NORM;
	 v->type = DILV_UP;
	 *buf1='\0';
	 *buf2='\0';
	 split_fi_ref((char *) v1.val.ptr,buf1,buf2);
	 v->val.ptr = find_symbolic(buf1, buf2);
	 if (v->val.ptr == NULL)
	    v->type = DILV_NULL; /* not found */
	 break;
      case DILV_NULL:
	 v->type = DILV_NULL; /* not found */
	 break;
      default:
	 v->type = DILV_ERR; /* wrong type */
	 break;
   }
}

void dilfe_gt(register struct dilprg *p,	  register class dilval *v)
{
   /* Greater Than operator */
   class dilval v1, v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   switch (dil_getval(&v2))
   {
      case DILV_FAIL:
	 v->type = DILV_FAIL;
	 break;
      case DILV_INT:
	 v->type = DILV_INT;
	 break;
      default:
	 v->type = DILV_ERR; /* wrong type */
	 break;
   }

   switch (dil_getval(&v1))
   {
      case DILV_FAIL:
	 if (v->type != DILV_ERR)
	    v->type = DILV_FAIL;
	 break;
      case DILV_INT:
	 break;
      default:
	 v->type = DILV_ERR; /* wrong type */
	 break;
   }

   if (v->type == DILV_INT)
   {
      v->atyp = DILA_NONE;
      v->val.num = (v1.val.num > v2.val.num);
   }
}

void dilfe_lt(register struct dilprg *p,	  register class dilval *v)
{
   /* Less Than operator */
   class dilval v1, v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   switch (dil_getval(&v2))
   {
      case DILV_FAIL:
	 v->type = DILV_FAIL;
	 break;
      case DILV_INT:
	 v->type = DILV_INT;
	 break;
      default:
	 v->type = DILV_ERR; /* wrong type */
	 break;
   }

   switch (dil_getval(&v1))
   {
      case DILV_FAIL:
	 if (v->type != DILV_ERR)
	    v->type = DILV_FAIL;
	 break;
      case DILV_INT:
	 break;
      default:
	 v->type = DILV_ERR; /* wrong type */
	 break;
   }

   if (v->type == DILV_INT)
   {
      v->atyp = DILA_NONE;
      v->val.num = (v1.val.num < v2.val.num);
   }
}

void dilfe_ge(register struct dilprg *p,	  register class dilval *v)
{
   /* Greater or Equal operator */
   class dilval v1, v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   switch (dil_getval(&v2))
   {
      case DILV_FAIL:
	 v->type = DILV_FAIL;
	 break;
      case DILV_INT:
	 v->type = DILV_INT;
	 break;
      default:
	 v->type = DILV_ERR; /* wrong type */
	 break;
   }

   switch (dil_getval(&v1))
   {
      case DILV_FAIL:
	 if (v->type != DILV_ERR)
	    v->type = DILV_FAIL;
	 break;
      case DILV_INT:
	 break;
      default:
	 v->type = DILV_ERR; /* wrong type */
	 break;
   }

   if (v->type == DILV_INT)
   {
      v->atyp = DILA_NONE;
      v->val.num = (v1.val.num >= v2.val.num);
   }
}

void dilfe_le(register struct dilprg *p, register class dilval *v)
{
   /* Less or Equal operator */
   class dilval v1, v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   switch (dil_getval(&v2))
   {
      case DILV_FAIL:
	 v->type = DILV_FAIL;
	 break;
      case DILV_INT:
	 v->type = DILV_INT;
	 break;
      default:
	 v->type = DILV_ERR; /* wrong type */
	 break;
   }

   switch (dil_getval(&v1))
   {
      case DILV_FAIL:
	 if (v->type != DILV_ERR)
	    v->type = DILV_FAIL;
	 break;
      case DILV_INT:
	 break;
      default:
	 v->type = DILV_ERR; /* wrong type */
	 break;
   }

   if (v->type == DILV_INT)
   {
      v->atyp = DILA_NONE;
      v->val.num = (v1.val.num <= v2.val.num);
   }
}

void dilfe_eq(register struct dilprg *p, register class dilval *v)
{
   /* Equal operator */
   class dilval v1, v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   switch (dil_getval(&v2))
   {
      case DILV_FAIL:
	 v->type = DILV_FAIL;
	 break;
      case DILV_INT:
	 v->type = DILV_INT;
	 break;
      default:
	 v->type = DILV_ERR; /* wrong type */
	 break;
   }

   switch (dil_getval(&v1))
   {
      case DILV_FAIL:
	 if (v->type != DILV_ERR)
	    v->type = DILV_FAIL;
	 break;
      case DILV_INT:
	 break;
      default:
	 v->type = DILV_ERR; /* wrong type */
	 break;
   }

   if (v->type == DILV_INT)
   {
      v->atyp = DILA_NONE;
      v->val.num = (v1.val.num == v2.val.num);
   }
}

void dilfe_se(register struct dilprg *p, register class dilval *v)
{
   /* String equal operator */
   class dilval v1, v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   switch (dil_getval(&v2))
   {
      case DILV_FAIL:
	 v->type = DILV_FAIL;
	 break;
      case DILV_SP:
	 v->type = DILV_INT;
	 break;
      case DILV_NULL:
	 v->type = DILV_NULL; /* null pointer */
	 break;
      default:
	 v->type = DILV_ERR; /* wrong type */
	 break;
   }

   switch (dil_getval(&v1))
   {
      case DILV_FAIL:
	 if (v->type != DILV_ERR)
	    v->type = DILV_FAIL;
	 break;
      case DILV_SP:
	 break;
      case DILV_NULL:
	 if ((v->type != DILV_ERR) && (v->type != DILV_FAIL))
	    v->type = DILV_NULL; /* null pointer */
	 break;
      default:
	 v->type = DILV_ERR;
	 break;
   }

   switch (v->type)
   {
      case DILV_INT:
	 v->atyp = DILA_NONE;
	 if (str_is_empty((char *) v1.val.ptr) ||
	     str_is_empty((char *) v2.val.ptr))
	   v->val.num = (str_is_empty((char *) v1.val.ptr) &&
			 str_is_empty((char *) v2.val.ptr));
	 else
	   v->val.num = !str_ccmp((char *) v1.val.ptr, (char *) v2.val.ptr);
	 break;
      case DILV_NULL:
	 v->atyp = DILA_NONE;
	 v->type = DILV_INT;
	 v->val.num = FALSE;
	 break;
   }

}

void dilfe_sne(register struct dilprg *p, register class dilval *v)
{
   /* String not equal operator */
   class dilval v1, v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   switch (dil_getval(&v2))
   {
      case DILV_FAIL:
	 v->type = DILV_FAIL;
	 break;

      case DILV_NULL:
      case DILV_SP:
	 v->type = DILV_INT;
	 break;

      default:
	 v->type = DILV_ERR; /* wrong type */
	 break;
   }

   switch (dil_getval(&v1))
   {
      case DILV_FAIL:
	 if (v->type != DILV_ERR)
	    v->type = DILV_FAIL;
	 break;

      case DILV_NULL:
      case DILV_SP:
	 break;

      default:
	 v->type = DILV_ERR;
	 break;
   }

   switch (v->type)
   {
      case DILV_INT:
	 v->atyp = DILA_NONE;
	 if (str_is_empty((char *) v1.val.ptr) ||
	     str_is_empty((char *) v2.val.ptr))
	   v->val.num = (!str_is_empty((char *) v1.val.ptr) ||
			 !str_is_empty((char *) v2.val.ptr));
	 else
	   v->val.num = (str_ccmp((char *) v1.val.ptr, (char *) v2.val.ptr)
			 != 0);
	 break;
   }

}


void dilfe_pe(register struct dilprg *p, register class dilval *v)
{
   /* Pointer Equality operator */
   class dilval v1, v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   switch (dil_getval(&v2))
   {
     case DILV_FAIL:
      v->type = DILV_FAIL;
      break;

     case DILV_NULL:
     case DILV_SLP:
     case DILV_EDP:
     case DILV_UP:
     case DILV_SP:
      v->type = DILV_INT;
      break;

     default:
      v->type = DILV_ERR;	/* wrong type */
      return;
   }

   switch (dil_getval(&v1))
   {
     case DILV_FAIL:
      v->type = DILV_FAIL;
      break;

     case DILV_NULL:
     case DILV_SP:
     case DILV_SLP:
     case DILV_EDP:
     case DILV_UP:
      v->type = DILV_INT;
      break;

     default:
      v->type = DILV_ERR;	/* wrong type */
      return;
   }

   if (v->type == DILV_INT)
   {
      v->atyp = DILA_NONE;
      v->val.num = (v1.val.ptr == v2.val.ptr);
   }
}

void dilfe_pne(register struct dilprg *p, register class dilval *v)
{
   /* Pointer Equality operator */
   class dilval v1, v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   switch (dil_getval(&v2))
   {
     case DILV_FAIL:
      v->type = DILV_FAIL;
      break;

     case DILV_NULL:
     case DILV_SLP:
     case DILV_EDP:
     case DILV_UP:
     case DILV_SP:
      v->type = DILV_INT;
      break;

     default:
      v->type = DILV_ERR;	/* wrong type */
      return;
   }

   switch (dil_getval(&v1))
   {
     case DILV_FAIL:
      v->type = DILV_FAIL;
      break;

     case DILV_NULL:
     case DILV_SP:
     case DILV_SLP:
     case DILV_EDP:
     case DILV_UP:
      v->type = DILV_INT;
      break;

     default:
      v->type = DILV_ERR;	/* wrong type */
      return;
   }

   if (v->type == DILV_INT)
   {
      v->atyp = DILA_NONE;
      v->val.num = (v1.val.ptr != v2.val.ptr);
   }
}

void dilfe_ne(register struct dilprg *p, register class dilval *v)
{
   /* Not Equal operator */
   class dilval v1, v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   switch (dil_getval(&v2))
   {
      case DILV_FAIL:
	 v->type = DILV_FAIL;
	 break;
      case DILV_INT:
	 v->type = DILV_INT;
	 break;
      default:
	 v->type = DILV_ERR; /* wrong type */
	 break;
   }

   switch (dil_getval(&v1))
   {
      case DILV_FAIL:
	 if (v->type != DILV_ERR)
	    v->type = DILV_FAIL;
	 break;
      case DILV_INT:
	 break;
      default:
	 v->type = DILV_ERR; /* wrong type */
	 break;
   }

   if (v->type == DILV_INT)
   {
      v->atyp = DILA_NONE;
      v->val.num = (v1.val.num != v2.val.num);
   }
}

void dilfe_iss(register struct dilprg *p, register class dilval *v)
{
   /* Test if bits is set in bitfield */
   class dilval v1, v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   switch (dil_getval(&v2))
   {
      case DILV_FAIL:
	 v->type = DILV_FAIL;
	 break;
      case DILV_INT:
	 v->type = DILV_INT;
	 break;
      default:
	 v->type = DILV_ERR; /* wrong type */
	 break;
   }

   switch (dil_getval(&v1))
   {
      case DILV_FAIL:
	 if (v->type != DILV_ERR)
	    v->type = DILV_FAIL;
	 break;
      case DILV_INT:
	 break;
      default:
	 v->type = DILV_ERR; /* wrong type */
	 break;
   }

   if (v->type == DILV_INT)
   {
      v->atyp = DILA_NONE;
      v->val.num = IS_SET(v1.val.num, v2.val.num);
   }
}

void dilfe_in(register struct dilprg *p, register class dilval *v)
{
   /* Test if string in string, stringlist or extra description */
   class dilval v1, v2;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   switch (dil_getval(&v2))
   {
     case DILV_FAIL:
      v->type = DILV_FAIL;
      break;

     case DILV_SLP:
      if (v2.val.ptr)
	v->type = DILV_SLP;
      else
	v->type = DILV_NULL; /* not found */
      break;

     case DILV_EDP:
      if (v2.val.ptr)
	v->type = DILV_EDP;
      else
	v->type = DILV_NULL; /* not found */
      break;

     case DILV_SP:
      if (v2.val.ptr)
	v->type = DILV_INT;
      else
	v->type = DILV_NULL; /* not found */
      break;

     case DILV_NULL:
      v->type = DILV_NULL; /* not found */
      break;

     default:
      v->type = DILV_ERR;
      return;
   }

   switch (dil_getval(&v1))
   {
     case DILV_FAIL:
      v->type = DILV_FAIL;
      break;

     case DILV_SP:
      if (v1.val.ptr)
	break;
      else
	v->type = DILV_FAIL;

     case DILV_NULL:
      v->type = DILV_NULL;
      v->val.num = 0;
      break;

     default:
      v->type = DILV_ERR;
      return;
   }

   switch (v->type)
   {
     case DILV_SLP:
      v->atyp = DILA_NONE;
      v->type = DILV_INT;
      v->val.num = 1 + (((cNamelist *) v2.val.ptr)->
			IsNameIdx(skip_spaces((char *) v1.val.ptr)));
      break;

     case DILV_EDP:
      v->atyp = DILA_NORM;
      v->type = DILV_EDP;
      v->val.ptr = ((class extra_descr_data *) v2.val.ptr)->
	find_raw(skip_spaces((char *) v1.val.ptr));
      break;

     case DILV_INT:
      v->atyp = DILA_NONE;
      v->type = DILV_INT;
      v->val.num = (str_cstr((char *)v2.val.ptr, (char *) v1.val.ptr) != NULL);
      break;
   }
}

void dilfe_fndu(register struct dilprg *p, register class dilval *v)
{
   /* Find a unit */
   class dilval v1, v2, v3, v4;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   eval_dil_exp(p, &v3);
   eval_dil_exp(p, &v4);

   switch (dil_getval(&v1))
   {
     case DILV_FAIL:
      v->type = DILV_FAIL;
      break;

     case DILV_UP:
      if (v1.val.ptr)
	v->type = DILV_UP;
      else
	v->type = DILV_NULL; /* not applicable UP */
      break;

     case DILV_NULL:
      v->type = DILV_NULL;
      break;

     default:
      v->type = DILV_ERR;
      break;
   }

   switch (dil_getval(&v2))
   {
     case DILV_FAIL:
      if (v->type != DILV_ERR)
	v->type = DILV_FAIL;
      break;

     case DILV_SP:
      if (v->type != DILV_ERR)
      {
	 if (!v2.val.ptr)
	   v->type = DILV_NULL;   /* not applicable SP */
	 else if (!*((char *) v2.val.ptr))
	   v->type = DILV_NULL;   /* not applicable SP */
      }
      break;

     case DILV_NULL:
      if ((v->type != DILV_ERR) && (v->type != DILV_FAIL))
	v->type = DILV_NULL; /* not applicable */
      break;

     default:
      v->type = DILV_ERR; /* wrong type */
      break;
   }

   switch (dil_getval(&v3))
   {
     case DILV_FAIL:
      if (v->type != DILV_ERR)
	v->type = DILV_FAIL;
      break;

     case DILV_INT:
      break;

     default:
      v->type = DILV_ERR; /* wrong type */
      break;
   }

   switch (dil_getval(&v4))
   {
     case DILV_FAIL:
      if (v->type != DILV_ERR)
	v->type = DILV_FAIL;
      break;

     case DILV_UP:
      break;

     case DILV_NULL: /* null pointer */
      if ((v->type != DILV_ERR) && (v->type != DILV_FAIL))
	v4.val.ptr = NULL;
      break;

     default:
      v->type = DILV_ERR; /* wrong type */
      break;
   }

   if ((v->type == DILV_UP) && IS_CHAR((struct unit_data *) v1.val.ptr))
   {
      char *c = (char *) v2.val.ptr;

      v->atyp = DILA_NORM;
      v->val.ptr = find_unit((struct unit_data *) v1.val.ptr,
			     &c, (struct unit_data *) v4.val.ptr,
			     v3.val.num);

      if (v2.atyp == DILA_NORM && v2.type == DILV_SPR)
	memmove(v2.val.ptr, c, strlen(c)+1);
      else if (p->sarg->arg == v2.val.ptr)
	p->sarg->arg = c; /* Update argument if argument is argument! :) */

      if (!v->val.ptr)
	v->type = DILV_NULL;
   }
   else
     v->type = DILV_FAIL;
}


void dilfe_fndru(register struct dilprg *p, register class dilval *v)
{
   /* Find a unit */
   class dilval v1, v2, v3;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   eval_dil_exp(p, &v3);

   switch (dil_getval(&v1))
   {
      case DILV_FAIL:
	 v->type = DILV_FAIL;
	 break;
      case DILV_UP:
	 if (v1.val.ptr)
	    v->type = DILV_UP;
	 else
	    v->type = DILV_NULL; /* not applicable UP */
	 break;
      case DILV_NULL:
	 v->type = DILV_NULL;
	 break;
      default:
	 v->type = DILV_ERR;
	 break;
   }

   switch (dil_getval(&v2))
   {
      case DILV_FAIL:
	 if (v->type != DILV_ERR)
	    v->type = DILV_FAIL;
	 break;
      case DILV_INT:
	 break;

      case DILV_NULL:
	 if ((v->type != DILV_ERR) && (v->type != DILV_FAIL))
	    v->type = DILV_NULL; /* not applicable */
	 break;
      default:
	 v->type = DILV_ERR; /* wrong type */
	 break;
   }

   switch (dil_getval(&v3))
   {
      case DILV_FAIL:
	 if (v->type != DILV_ERR)
	    v->type = DILV_FAIL;
	 break;
      case DILV_INT:
	 break;
      default:
	 v->type = DILV_ERR; /* wrong type */
	 break;
   }

   if (v->type == DILV_UP)
   {
      v->atyp = DILA_NORM;
      v->val.ptr = random_unit((struct unit_data *) v1.val.ptr,
			       v2.val.num, v3.val.num);


      if (!v->val.ptr)
	 v->type = DILV_NULL;
   }
   else
     v->type = DILV_FAIL;

}

void dilfe_fs(register struct dilprg *p, register class dilval *v)
{
   /* A Fixed String */
   v->type = DILV_SP;
   v->atyp = DILA_NORM;

   /* Reference directly into the core */
   /* assumes bwrite_string is same format */
   /* saves LOTS of time! */
   v->val.ptr = p->sp->pc;

   /* just skip single string */
   TAIL(p->sp->pc);

   (p->sp->pc)++; /* ready for next */
}

void dilfe_fsl(register struct dilprg *p, register class dilval *v)
{
   cNamelist *namelist = new cNamelist;
   /* A Fixed String list */

   v->atyp = DILA_EXP; /* no other way, copy namelist from core */
   v->type = DILV_SLP;

   namelist->bread(&(p->sp->pc));
   v->val.ptr = namelist;
}

void dilfe_var(register struct dilprg *p, register class dilval *v)
{
   /* A variable */
   int varno;

   varno = bread_ubit16(&(p->sp->pc));

   switch (p->sp->vars[varno].type)
   {
      /* normal variable */
      case DILV_UP:	   /* unit pointer */
	 v->atyp = DILA_NORM;
	 v->type = DILV_UPR;
	 v->ref = &(p->sp->vars[varno].val.unitptr);
	 break;
      case DILV_SP:	   /* string pointer */
	 v->atyp = DILA_NORM;
	 v->type = DILV_SPR;
	 v->ref = &(p->sp->vars[varno].val.string);
	 break;
      case DILV_SLP:	   /* string list pointer */
	 v->atyp    = DILA_NORM;
	 v->type    = DILV_SLPR;
 	 v->ref     = p->sp->vars[varno].val.namelist;
	 break;
      case DILV_EDP:	   /* extra description pointer */
	 v->atyp = DILA_NORM;
	 v->type = DILV_EDPR;
	 v->ref = &(p->sp->vars[varno].val.extraptr);
	 break;
      case DILV_INT:	   /* integer */
	 v->atyp = DILA_NONE;
	 v->type = DILV_SINT4R;
	 v->ref = &(p->sp->vars[varno].val.integer);
	 break;
      default:
	 assert(FALSE);
   }
}

void dilfe_weat(register struct dilprg *p, register class dilval *v)
{
   /* Self */

   v->type = DILV_INT;
   v->atyp = DILA_NORM;
   v->val.num = unit_zone(p->sarg->owner)->weather.sky;
}

void dilfe_self(register struct dilprg *p, register class dilval *v)
{
   /* Self */

   v->type = DILV_UP;
   v->atyp = DILA_NORM;
   v->val.ptr = p->sarg->owner;
}

void dilfe_hrt(register struct dilprg *p, register class dilval *v)
{
   /* Heartbeat */

   v->type = DILV_SINT2R;
   v->atyp = DILA_NONE;
   v->ref = (sbit16 *) &(p->sarg->fptr->heart_beat);
}

void dilfe_tho(register struct dilprg *p, register class dilval *v)
{
   /* MudHour */

   v->type = DILV_INT;
   v->atyp = DILA_NONE;
   v->val.num = mud_date(time(0)).hours;
}

void dilfe_tda(register struct dilprg *p, register class dilval *v)
{
   /* MudDay */

   v->type = DILV_INT;
   v->atyp = DILA_NONE;
   v->val.num = mud_date(time(0)).day;
}

void dilfe_tmd(register struct dilprg *p, register class dilval *v)
{
   /* MudMonth */

   v->type = DILV_INT;
   v->atyp = DILA_NONE;
   v->val.num = mud_date(time(0)).month;
}

void dilfe_tye(register struct dilprg *p, register class dilval *v)
{
   /* MudYear */

   v->type = DILV_INT;
   v->atyp = DILA_NONE;
   v->val.num = mud_date(time(0)).year;
}

void dilfe_rti(register struct dilprg *p, register class dilval *v)
{
   /* RealTime */

   v->type = DILV_INT;
   v->atyp = DILA_NONE;
   v->val.num = time(0);
}

void dilfe_acti(register struct dilprg *p, register class dilval *v)
{
   /* Activator */

   v->type = DILV_UP;
   v->atyp = DILA_NORM;
   v->val.ptr = p->sarg->activator;
}

void dilfe_medi(register struct dilprg *p, register class dilval *v)
{
   /* Medium */

   v->type = DILV_UP;
   v->atyp = DILA_NORM;
   v->val.ptr = p->sarg->medium;
}

void dilfe_targ(register struct dilprg *p, register class dilval *v)
{
   /* Target */

   v->type = DILV_UP;
   v->atyp = DILA_NORM;
   v->val.ptr = p->sarg->target;
}

void dilfe_powe(register struct dilprg *p, register class dilval *v)
{
   /* Power */

   static int dummy = 0;

   v->type = DILV_SINT4R;
   v->atyp = DILA_NONE;

   if (p->sarg->pInt)
     v->ref = (sbit32 *) p->sarg->pInt;
   else
     v->ref = (int *) &dummy;
}

void dilfe_cmst(register struct dilprg *p, register class dilval *v)
{
   /* cmdstr */

   v->type = DILV_SP;
   v->atyp = DILA_NORM;

   if (p->sarg->cmd->cmd_str)
     v->val.ptr = (char *) p->sarg->cmd->cmd_str;
   else
   {
      v->type = DILV_NULL;
      v->val.ptr = NULL;
   }
}

void dilfe_argm(register struct dilprg *p, register class dilval *v)
{
   /* Argument */

   v->type = DILV_SP;
   v->atyp = DILA_NORM;
   if (p->sarg->arg)
     v->val.ptr = (char *) p->sarg->arg;
   else
   {
      v->type = DILV_NULL;
      v->val.ptr = NULL;
   }
}

void dilfe_null(register struct dilprg *p, register class dilval *v)
{
   /* Pointer value null */

   v->type = DILV_NULL;
   v->atyp = DILA_NONE;
   v->val.ptr = NULL;
}

void dilfe_int(register struct dilprg *p, register class dilval *v)
{
   /* Fixed integer */

   v->type = DILV_INT;
   v->atyp = DILA_NONE;
   v->val.num = (sbit32) bread_ubit32(&(p->sp->pc));
}

void dilfe_cmds(register struct dilprg *p, register class dilval *v)
{
   /* Check if the input command might the supplied argument */
   class dilval v1;

   eval_dil_exp(p, &v1);

   switch (dil_getval(&v1))
   {
      case DILV_FAIL:
      case DILV_NULL:
	 v->type = DILV_FAIL;
	 break;
      case DILV_SP:
	 if (v1.val.ptr)
	 {
	    v->type = DILV_INT;
	    v->atyp = DILA_NONE;
	    v->val.num = is_command(p->sarg->cmd, (char *) v1.val.ptr);
	 }
	 else
	    v->type = DILV_FAIL;
	 break;

      case DILV_INT:
	 v->type = DILV_INT;
	 v->val.num = (p->sarg->cmd->no == v1.val.num);
	 break;

      default:
	 v->type = DILV_ERR; /* wrong type */
	 break;
   }
}
 

/* visible, some vs other */
void dilfe_pck(struct dilprg *p, class dilval *v)
{
   extern int pay_point_charlie(struct unit_data *ch,
			 struct unit_data *to); /* from act_movement.c */
   class dilval v1;
   class dilval v2;

   eval_dil_exp(p, &v1)
   eval_dil_exp(p, &v2);

   v->type=DILV_INT;
   switch (dil_getval(&v1)) {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_UP:
      if (!v1.val.ptr || !IS_CHAR((struct unit_data *)v1.val.ptr))
	v->type=DILV_FAIL;
      break;
     default:
      v->type=DILV_ERR;
      return;
   }
   switch (dil_getval(&v2)) {
     case DILV_FAIL:
     case DILV_NULL:
      v->type=DILV_FAIL;
      break;
     case DILV_UP: 
      if (!v2.val.ptr)
	v->type=DILV_FAIL;
     break;
     default:
      v->type=DILV_ERR;
      return;
   }

   if (v->type==DILV_INT)
     v->val.num = pay_point_charlie((struct unit_data *)v1.val.ptr,
				    (struct unit_data *)v2.val.ptr);
}

void dilfe_act(register struct dilprg *p, register class dilval *v)
{
   char buf[1024];
   /* Conversion of integers to strings */
   class dilval v1,v2,v3,v4,v5,v6;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   eval_dil_exp(p, &v3);
   eval_dil_exp(p, &v4);
   eval_dil_exp(p, &v5);
   eval_dil_exp(p, &v6);

   v->type = DILV_FAIL;
   
   if (dil_type_check("act", p, 6,
		  &v1, TYPEFAIL_NULL, 1, DILV_SP,
		  &v2, TYPEFAIL_NULL, 1, DILV_INT,
		  &v3, TYPEFAIL_NULL, 3, DILV_UP, DILV_SP, DILV_NULL,
		  &v4, TYPEFAIL_NULL, 3, DILV_UP, DILV_SP, DILV_NULL,
		  &v5, TYPEFAIL_NULL, 3, DILV_UP, DILV_SP, DILV_NULL,
		  &v6, TYPEFAIL_NULL, 1, DILV_INT) < 0)
     return;

   switch (v6.val.num)
   {
     case TO_CHAR:
     case TO_ROOM:
     case TO_ALL:
     case TO_REST:
      /* these require 1st argument */
      if (v3.val.ptr)
	act_generate(buf, (char *) v1.val.ptr, v2.val.num, v3.val.ptr, v4.val.ptr,
		     v5.val.ptr, v6.val.num, (struct unit_data *) v3.val.ptr);
      v->type = DILV_SP;
      v->atyp = DILA_EXP;
      v->val.ptr = str_dup(buf);
      break;

     case TO_VICT:
     case TO_NOTVICT:
      if (v5.val.ptr)
	act_generate(buf,(char *) v1.val.ptr, v2.val.num, v3.val.ptr, v4.val.ptr,
			  v5.val.ptr, v6.val.num, (struct unit_data *) v5.val.ptr);
      v->type = DILV_SP;
      v->atyp = DILA_EXP;
      v->val.ptr = str_dup(buf);
      break;
   }
}
