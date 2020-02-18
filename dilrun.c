/* *********************************************************************** *
 * File   : dilrun.c                                  Part of Valhalla MUD *
 * Version: 2.00                                                           *
 * Author : bombman@diku.dk & seifert@diku.dk				   *
 *                                                                         *
 *                                                                         *
 * Purpose: Runtime handling procedures for DIL.                           *
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
#include "db_file.h"
#include "limits.h"
#include "common.h"
#include "spells.h"
#include "db_file.h"
#include "unixshit.h"
#include "dilinst.h"
#include "dilexp.h"
#include "dilrun.h"

/* *********************************************************************** *
 * Implementation notes:
 *
 *   Internal command structure:
 *	The commands and expressions are all stored in a shared
 *	core. expressions are stored with a leading function id
 *	corresponding to the function array. This method is also
 *	used by instructions, where the return value is not used
 *	(must be NULL!). The core is also used to store static
 *	values of both integers, strings and stringlists. The
 *	values are read with bread_ functions.
 *
 *   The types and their meaning:
 *	unitptr (UP)	   : A pointer to a unit. units not secured,
 *	   are set to NULL after activation.
 *	extraptr (EDP)	   : A pointer to an extra description.
 *	   are set to NULL after activation.
 *	string (SP)	   : A string. When assigned from another
 *	   string, (also static) strdup'ed. (old free'd). Under
 *	   expression
 *	   evaluation strings are only copied if needed, including
 *	   reading from the core (assuming bread_ format is normal
 *	   string format)
 *	integer (INT)	   : A signed 32 bit integer.
 *	NEW IMPLEMENTATION:
 *	stringlist (SLP)   : A stringlist. When assigned from another
 *	   (also static) copied. (old free'd)
 *
 *   Local passed values in expression evaluation:
 *	Values are tagged with the type of allocation used:
 *	   NORM: normal malloc.
 *	   HASH: hashed class.
 *	   EXP:	 temporarily expression eval.
 *	   NONE: not allocated.
 *
 *
 *
 *	########################################################
 *	NOTE! you MUST! mark a field HASH if DIL is supposed to
 *	      use the hash deallocation if field is changed!
 *	########################################################
 *
 *
 *
 *	unitptr reference     : unitptr variable that can be assigned
 *	   new value. Unitptr are only references.
 *	string reference      : string variable that can be assigned
 *	   new value. the 'atyp' variable tells how the string is
 *	   allocated.
 *	stringlist reference  : stringlist that can be assigned a new
 *	   value. the 'atyp' variable tells how the stringlist is
 *	   allocated.
 *	extraptr reference    : extraptr variable that can be
 *	   assigned a new value. Extraptr are only references.
 *	integer reference     : Several types of integer references
 *	   exist to acommodate all the different types of integers.
 *
 *   On activation and combat.
 *	When encountered the boolean expression is passed, and
 *	assumed to have no side-effects. Should expressions have
 *	sideeffects, the commands should have this form:
 *	<cmd> <eadr> <exp> <xadr>
 *	where <eadr> the the end address of the command, so skipping
 *	it is possible without evaluating the expression.
 *
 *   Calls and stackframes.
 *      The dilprg structure contains the data needed for one DIL
 *      process. The process is marked eighter DILP_COPY for
 *      a process that is a direct copy of a previous registered
 *      template, or DILP_INLINE for an inline defined DIL body code.
 *      For backward compatability, the old DIL programs are marked
 *      DIL_INLINE uppon loading, and thus have a local template not
 *      registered in the zone. The structure also includes a stack,
 *      where each frame contains the needed data for the current
 *      proc/func. Among these are variables, secure structures, external
 *      references program counter and variables. When the process is
 *      saved, only some data in the first stackframe is saved.
 *      Only variables are saved, thus starting over the process when
 *      loaded next time.
 *
 * *********************************************************************** */


struct dilprg *dil_list = NULL;
struct dilprg *dil_list_nextdude = NULL;


void dil_intr_remove(struct dilprg *p, int idx)
{
   if ((idx >= 0) && (idx < p->sp->intrcount))
   {
      memmove(&(p->sp->intr[idx]),&(p->sp->intr[idx+1]),
	      sizeof(struct dilintr)*(p->sp->intrcount-idx-1));
      p->sp->intr[p->sp->intrcount-1].flags=0;
      p->sp->intr[p->sp->intrcount-1].lab=NULL;
   }
}



int dil_intr_insert(struct dilprg *p, ubit8 *lab, ubit16 flags)
{
   int intnum;


   /* find intnum */
   for (intnum=0; intnum < p->sp->intrcount; intnum++)
     if (p->sp->intr[intnum].lab == lab)
     {
	dil_intr_remove(p, intnum);
	break;
     }

   /* find intnum */
   for (intnum=0; intnum < p->sp->intrcount; intnum++)
     if (!p->sp->intr[intnum].flags)
       break;

   assert (intnum<p->sp->intrcount);

   p->sp->intr[intnum].flags = flags; 
   p->sp->intr[intnum].lab   = lab;     

   return intnum;
}




/* free generated temporary values */
dilval::~dilval(void)
{
   switch (type)
   {
     case DILV_SP:
      /* Only free if temporary allocated expression */
      if (val.ptr == NULL)
	slog(LOG_ALL, 0, "DIL: NULL string pointer to free().");
      else if (atyp == DILA_EXP)
      {
	 free(val.ptr);
	 val.ptr = NULL;
      }
      break;

     case DILV_SLP:
      /* Only free if temporary allocated expression */
      if (val.ptr == NULL)
	slog(LOG_ALL, 0, "DIL: NULL string list pointer to free().");
      else if (atyp == DILA_EXP)
      {
	 delete ((class cNamelist *) val.ptr);
	 val.ptr = NULL;
      }
      break;
   }
}


void dil_free_var(struct dilvar *v)
{
   switch (v->type)
   {
     case DILV_SP:
      if (v->val.string)
      {
	 free(v->val.string);
	 v->val.string = NULL;
      }
      break;
      
     case DILV_SLP:
      if (v->val.namelist)
      {
	 delete v->val.namelist;
	 v->val.namelist = NULL;
      }
      break;
   }
}


void dil_free_frame(struct dilframe *frame)
{
   int j;

   /* free variables */
   for (j = 0; j < frame->tmpl->varc; j++)
     dil_free_var(&frame->vars[j]);

   if (frame->vars)
   {
      free(frame->vars);
      frame->vars = NULL;
   }

   /* discard secures */
   if (frame->secure)
   {
      free(frame->secure);
      frame->secure = NULL;
   }

   /* discard intr */
   if (frame->intr) 
   {
      free(frame->intr);
      frame->intr=NULL;
   }
}


void dil_free_template(struct diltemplate *tmpl, int copy)
{
   int i;

   if (tmpl->flags == DILFL_FREEME)
   {
      /* free dummy template */
      free(tmpl->prgname);
      free(tmpl);
   }
   else if (!copy)
   {
      /* free entire inline template */
      free(tmpl->prgname);

      if (tmpl->argt)
	free(tmpl->argt);

      if (tmpl->vart)
	free(tmpl->vart);

      if (tmpl->core)
	free(tmpl->core);

      if (tmpl->extprg)
	free(tmpl->extprg);

      for (i=0;i<tmpl->xrefcount;i++)
      {
	 if (tmpl->xrefs[i].name)
	   free(tmpl->xrefs[i].name);

	 if (tmpl->xrefs[i].argt)
	   free(tmpl->xrefs[i].argt);
      }

      if (tmpl->xrefs)
	free(tmpl->xrefs);

      free(tmpl);
   }
}

/* returns boolean value of DIL value */
char dil_getbool(class dilval *v)
{
   /*
      Please! no 'smart' stuff here, the older NICE version
      did NOT work correctly on leftvalue pointers, as there
      is a difference between a pointer and the address of
      the pointer variable! The new value type is very
      simplified, and thus more effective.
      */
   switch (v->type)
   {
     case DILV_UP:
     case DILV_SP:
     case DILV_EDP:
      return (v->val.ptr != NULL); /* return Rvalue */

     case DILV_UPR:
     case DILV_SPR:
     case DILV_EDPR:
      return (*((void **) v->ref) != NULL); /* return Lvalue */

     case DILV_SLP:
      return ((cNamelist *) v->val.ptr)->Length() != 0;

     case DILV_SLPR: 
      return ((cNamelist *) v->ref)->Length() != 0;

     case DILV_HASHSTR:
      /* return Lvalue */
      return ((class cStringInstance *) (v->ref))->String() != NULL;

     case DILV_INT:
      return (v->val.num != 0); /* return Rvalue */

     case DILV_SINT1R:
      return (*((sbit8 *) v->ref) != 0); /* return Lvalue */

     case DILV_SINT2R:
      return (*((sbit16 *) v->ref) != 0); /* return Lvalue */

     case DILV_SINT4R:
      return (*((sbit32 *) v->ref) != 0); /* return Lvalue */

     case DILV_UINT1R:
      return (*((ubit8 *) v->ref) != 0); /* return Lvalue */

     case DILV_UINT2R:
      return (*((ubit16 *) v->ref) != 0); /* return Lvalue */

     case DILV_UINT4R:
      return (*((ubit32 *) v->ref) != 0); /* return Lvalue */

     case DILV_FAIL:
     case DILV_NULL:
      return FALSE;

     default:
      slog(LOG_ALL,0,"DIL getbool error.");
      return FALSE;
   }
}

/* transform references to values, and returns original type */
int dil_getval(class dilval *v)
{
   /* original type */
  static ubit8 orig_type[DILV_MAX+1] = //MS2020 was missing var type def?! odd. Ubit8 added
   {
      DILV_ERR,
      DILV_UP,  DILV_SP,   DILV_SLP,   DILV_EDP, DILV_INT,
      DILV_UP,  DILV_SP,   DILV_SLP,   DILV_EDP, DILV_INT,
      DILV_INT, DILV_INT,  DILV_INT,   DILV_INT, DILV_INT,
      DILV_ERR, DILV_NULL, DILV_FAIL,
      DILV_ERR, DILV_ERR,  DILV_ERR,   DILV_SP
   };

   switch (v->type)
   {
     case DILV_UP:
     case DILV_SP:
     case DILV_SLP:
     case DILV_EDP:
     case DILV_INT:
      v->ref=NULL;		/* this is NOT a reference */
      break;
     case DILV_SPR:
      v->val.ptr = * ((char **) v->ref); /* get value of ref */
      break;
     case DILV_HASHSTR:
      /* Important! Remember that the HASHSTR may NEVER EVER BE CHANGED! */
      v->val.ptr = (char *) ((class cStringInstance *) v->ref)->String();
      break;

     case DILV_SLPR:
       v->val.ptr = (void *) v->ref; //MS2020
      break;

     case DILV_UPR:
     case DILV_EDPR:
      v->val.ptr = * ((void **) v->ref); /* get value of ref */
      break;

     case DILV_SINT1R:
      v->val.num = * ((sbit8 *) v->ref);
      break;
     case DILV_SINT2R:
      v->val.num = * ((sbit16 *) v->ref);
      break;
     case DILV_SINT4R:
      v->val.num = * ((sbit32 *) v->ref);
      break;
     case DILV_UINT1R:
      v->val.num = * ((ubit8 *) v->ref);
      break;
     case DILV_UINT2R:
      v->val.num = * ((ubit16 *) v->ref);
      break;
     case DILV_UINT4R:
      v->val.num = * ((ubit32 *) v->ref);
      break;
     case DILV_NULL:
     case DILV_FAIL:
      v->val.ptr = NULL;
      v->ref = NULL;
      break;

     default:
      v->type = DILV_ERR;	/* illegal type! */
      v->val.num = 0;
      v->ref = NULL;
      break;
   }
   return orig_type[v->type];
}

/* adds exp node to exp, returns node number */
void dil_add_secure(struct dilprg *prg, struct unit_data *sup, ubit8 *lab)
{
   if (sup == NULL)
     return;

   if (prg->sp->securecount)
     RECREATE(prg->sp->secure, struct dilsecure, prg->sp->securecount+1);
   else
     CREATE(prg->sp->secure, struct dilsecure, prg->sp->securecount+1);

   prg->sp->secure[prg->sp->securecount].sup  = sup;
   prg->sp->secure[prg->sp->securecount].lab  = lab;
   
   prg->sp->securecount++;
}

/* adds exp node to exp, returns node number */
void dil_sub_secure(struct dilframe *frm, struct unit_data *sup,
		    int bForeach)
{
   int i;

   for (i=0; i < frm->securecount; i++)
     if (frm->secure[i].sup == sup)
     {
	if (bForeach && frm->secure[i].lab)
	  continue;

	if (!bForeach && !frm->secure[i].lab)
	  continue;

	frm->secure[i]=frm->secure[--(frm->securecount)];

	if (frm->securecount)
	  RECREATE(frm->secure, struct dilsecure, frm->securecount);
	else
	{
	   free(frm->secure);
	   frm->secure = NULL;
	}
     }
}

/* *********************************************************************
   This function tests if secures if DIL's are triggered.
   The function must be called just BEFORE a dil program is activated,
   or reactivated after external calls such as addequip(), unequip(),
   exec(), send() and sendto().
   ********************************************************************* */

/* Clears all variables that are not secured! Called at every activation */
/* of a DIL program (after secures are tested for!).			 */
void dil_clear_non_secured(register struct dilprg *prg)
{
   register int i, j;
   struct dilframe *frm;

   for (frm=prg->stack;frm<=prg->sp;frm++) {
      for (i=0; i < frm->tmpl->varc; i++)
      {
	 if (frm->vars[i].type == DILV_EDP)
	   frm->vars[i].val.extraptr = NULL;
	 else if (frm->vars[i].type == DILV_UP)
	 {
	    for (j=0; j < frm->securecount; j++)
	      if (frm->secure[j].sup == frm->vars[i].val.unitptr)
		break;
	    if (j >= frm->securecount)
	      frm->vars[i].val.unitptr = NULL;
	 }
      }
   }
}

/* If a secure is violated, this routine will clear all local DIL variables */
/* that refer to this secure.						    */
void dil_clear_lost_reference(register struct dilframe *frm, void *ptr)
{
   int i;

   for (i=0; i < frm->tmpl->varc; i++)
     if (frm->vars[i].val.unitptr == ptr)
       frm->vars[i].val.unitptr = NULL;
}

void dil_test_secure(register struct dilprg *prg)
{
   int i;
   struct dilframe *frm;

   if (prg->waitcmd <= WAITCMD_STOP)
     return;

   for (frm=prg->stack;frm<=prg->sp;frm++)
     for (i=0; i < frm->securecount; i++)
       if (scan4_ref(prg->sarg->owner, frm->secure[i].sup) == NULL)
       {
	  if (frm->secure[i].lab)
	  {
	     /* This is REALLY important! Imagine a broken secure in a
		pause; the execution then continues at the label point,
		however, to get the expected "wait" and execution behaviour,
		waitcmd must be less than MAXINST (see dilfi_wit) */
	     
	     prg->waitcmd--;
	     frm->pc = frm->secure[i].lab;
	  }

	  dil_clear_lost_reference(frm, frm->secure[i].sup);
	  dil_sub_secure(frm, frm->secure[i].sup);

	  /* Do not return until all have been tested! */
       }
}

/* report error in passed type to function */
void dil_typeerr(struct dilprg *p, const char *where)
{
   /* returned value not of expected type */
   /* This is serous! mess-up in the core.. stop the program */

   szonelog(UNIT_FI_ZONE(p->sarg->owner),
      "DIL %s@%s, Type error in %s\n",
      UNIT_FI_NAME(p->sarg->owner),
      UNIT_FI_ZONENAME(p->sarg->owner),
      where);
   p->waitcmd = WAITCMD_QUIT;
}

/*  Within the program p, and within function f,
 *  check the list of variables of the form { v, flag, n, {type} }
 *  where tot is the amount of variables to be checked, 
 *  v is a dilval *, flag is one of the macros above,
 *  n is the number of accepted types and type is an int of the form
 *  DILV_xxx.  Returns 0 for success, -1 for failure.
 */
int dil_type_check(const char *f, struct dilprg *p, int tot, ...)
{
   va_list args;
   class dilval *v[20];
   int cnt, val, flag, idx = 0;
   bool any, ok_sofar = TRUE;

   va_start(args, tot);

   while (tot--)
   {
      v[idx] = va_arg(args, class dilval *);

      flag = va_arg(args, int);
      cnt = va_arg(args, int);

      /* Gnort made a big bug here! Caused nasty crashes... be careful
	 with those dreaded va_args... */

      val = dil_getval(v[idx]);

      any = FALSE;
      while (cnt--)
	if (val == va_arg(args, int))
	  any =TRUE;

      if (!any)
      {
	 /* Don't write type error if dil_getval failed or
	  * returned DILV_NULL (provided we fail on nulls)
	  */
	 if ((val != DILV_FAIL) && ((val != DILV_NULL) ||
				    (flag != FAIL_NULL)))
	 {
	    dil_typeerr(p, f);
	    ok_sofar = FALSE;
	 }
      }
      
      ++idx;
   }
   
   va_end(args);
   
   if (ok_sofar)
     return 0;
   
   /* Type error, clean up  They clean themselves up now... :-/
   while (0 < idx--)
     delete v[idx]; */
   
   p->waitcmd = WAITCMD_DESTROYED;
   
   return -1;
}

/* ************************************************************************ */
/* Evaluating DIL-expressions/instructions				    */
/* ************************************************************************ */

struct dil_func_type dilfe_func[DILI_MAX+1] =
{
 {dilfe_illegal},		/* 0 */
 {dilfe_plus},
 {dilfe_min},
 {dilfe_mul},
 {dilfe_div},
 {dilfe_mod},
 {dilfe_and},
 {dilfe_or},
 {dilfe_not},
 {dilfe_gt},
 {dilfe_lt},			/* 10 */
 {dilfe_ge},
 {dilfe_le},
 {dilfe_eq},
 {dilfe_pe},
 {dilfe_ne},
 {dilfe_in},
 {dilfe_umin},
 {dilfe_se},
 {dilfe_null},

 {dilfe_fld},			/* 20 */
 {dilfe_atoi},
 {dilfe_itoa},
 {dilfe_rnd},
 {dilfe_fndu},
 {dilfe_fndr},			/* 25 */
 {dilfe_load},
 {dilfe_iss},
 {dilfe_getw},
 {dilfe_isa},
 {dilfe_cmds},			/* 30 */
 {dilfe_fnds},

 {dilfe_acti},
 {dilfe_argm},
 {dilfe_tho},
 {dilfe_tda},
 {dilfe_tmd},
 {dilfe_tye},
 {dilfe_hrt},
 {dilfe_self},

 {dilfe_var},			/* 40 */
 {dilfe_fs},
 {dilfe_fsl},
 {dilfe_int},
 {dilfe_len},

 {dilfi_ass},
 {dilfi_lnk},
 {dilfi_exp},
 {dilfi_cast},
 {dilfi_if},
 {dilfi_set},			/* 50 */
 {dilfi_uset},
 {dilfi_adl},
 {dilfi_sul},
 {dilfi_ade},
 {dilfi_sue},
 {dilfi_dst},
 {dilfi_walk},
 {dilfi_exec},
 {dilfi_wit},
 {dilfi_act},			/* 60 */
 {dilfi_goto},
 {dilfi_sua},
 {dilfi_ada},
 {dilfi_pri},
 {dilfi_npr},
 {dilfi_snd},
 {dilfi_snt},
 {dilfi_sec},
 {dilfi_use},
 {dilfi_foe},			/* 70 */
 {dilfi_fon},
 {dilfi_eqp},
 {dilfi_ueq},
 {dilfe_weat},
 {dilfe_oppo},
 {dilfi_quit},

 {dilfi_blk},
 {dilfi_pup},

 {dilfe_getws},
 {dilfi_snta},			/* 80 */
 {dilfi_log},
 {dilfe_pne},
 {dilfe_sne},
 {dilfi_rproc},
 {dilfi_rfunc},
 {dilfi_rts},
 {dilfi_rtf},
 {dilfe_dld},
 {dilfe_dlf},
 {dilfi_dlc},			/* 90 */
 {dilfe_lor},
 {dilfe_land},
 {dilfi_on},
 {dilfi_rsproc},
 {dilfi_rsfunc},
 {dilfe_intr},
 {dilfi_cli},
 {dilfi_sbt},
 {dilfi_swt},
 {dilfe_fndru},			/* 100 */
 {dilfe_visi},
 {dilfe_atsp},
 {dilfe_purs},
 {dilfi_chas},
 {dilfi_setf},                  /* 105 */
 {dilfe_medi},
 {dilfe_targ},
 {dilfe_powe},
 {dilfe_trmo},
 {dilfi_sntadil},               /* 110 */
 {dilfe_cast2},
 {dilfe_mel},
 {dilfe_eqpm},
 {dilfe_cmst},
 {dilfe_opro},                  /* 115 */
 {dilfe_rest},
 {dilfi_stor},
 {dilfi_amod},
 {dilfi_sete},
 {dilfi_folo},                  /* 120 */
 {dilfi_lcri},
 {dilfe_fits},
 {dilfe_cary},
 {dilfe_fnds2},
 {dilfe_path},                 /* 125  */
 {dilfe_mons},
 {dilfe_splx},
 {dilfe_spli},
 {dilfe_rti},
 {dilfe_txf},                  /* 130  */
 {dilfe_ast},
 {dilfe_visi},
 {dilfe_act}
};


void dil_free_prg(struct dilprg *prg)
{
   struct diltemplate *tmpl;
   struct dilframe *frm;
   struct dilprg *tp;

   assert(dil_list);

   if (prg == dil_list)
   {
      if (dil_list_nextdude == dil_list)
	dil_list_nextdude = prg->next;
      dil_list = prg->next;
   }
   else
   {
      int ok = FALSE;
      for (tp = dil_list; tp->next; tp = tp->next)
	if (tp->next == prg)
	{
	   if (dil_list_nextdude == tp->next)
	     dil_list_nextdude = prg->next;
	   tp->next = prg->next;
	   ok = TRUE;
	   break;
	}

      assert(ok);
   }
   prg->next = NULL;
 
   tmpl = prg->stack[0].tmpl;

   for (frm=prg->stack; frm <= (prg->sp); frm++) 
     dil_free_frame(frm);

   free(prg->stack);

   dil_free_template(tmpl, IS_SET(prg->flags,DILFL_COPY));

   free(prg);
}


static int check_interrupt(struct dilprg *prg)
{
   int i;

   for (i=0; i < prg->sp->intrcount; i++)
   {
      if (IS_SET(prg->sp->intr[i].flags, prg->sarg->mflags | SFB_ACTIVATE))
      {
	 class dilval v1;
	 ubit32 adr;
	 int oldwaitcmd=prg->waitcmd;
	 ubit8 *oldpc = prg->sp->pc;
      
	 prg->sp->pc = prg->sp->intr[i].lab;

	 eval_dil_exp(prg, &v1);

	 adr = bread_ubit32(&(prg->sp->pc));

	 prg->waitcmd = oldwaitcmd;

	 if (dil_getbool(&v1))
	 {
	    if (adr == SKIP)
	    {
	       prg->sp->pc = oldpc;
	       return 1;
	    }

	    prg->waitcmd--;
	    prg->sp->pc = &(prg->sp->tmpl->core[adr]);

	    if (IS_SET(prg->sp->intr[i].flags, SFB_ACTIVATE))
	    {
	       prg->sp->intr[i].flags = 0;
	       prg->sp->intr[i].lab = NULL;
	    }

	    return 0;
	 }
	 else
	 {
	    prg->sp->pc = oldpc;
	 }
      }
   }

   return 0;
}



/* ************************************************************************ */
/* Running a DIL-program */
/* ************************************************************************ */

void ActivateDil(struct unit_data *pc)
{
   struct unit_fptr *fptr;   
   struct dilprg *prg;

   for (fptr = UNIT_FUNC(pc); fptr; fptr = fptr->next)
   {
      if (fptr->index == SFUN_DIL_INTERNAL && fptr->data)
      {
	 prg = (struct dilprg *) fptr->data;
	 REMOVE_BIT(prg->flags, DILFL_DEACTIVATED);
      }
   }
}


void DeactivateDil(struct unit_data *pc)
{
   struct unit_fptr *fptr;   
   struct dilprg *prg;

   for (fptr = UNIT_FUNC(pc); fptr; fptr = fptr->next)
   {
      if (fptr->index == SFUN_DIL_INTERNAL && fptr->data)
      {
	 prg = (struct dilprg *) fptr->data;
	 SET_BIT(prg->flags, DILFL_DEACTIVATED);
      }
   }
}


int run_dil(struct spec_arg *sarg)
{
   register struct dilprg *prg = (struct dilprg *) sarg->fptr->data;
   char *orgarg = (char *) sarg->arg; /* Because fndu may mess with it!!! */
   int i;
   static int activates = 0;

   if (prg == NULL)
     return SFR_SHARE;

   if (sarg->cmd->no == CMD_AUTO_EXTRACT)
   {
      if (IS_SET(prg->flags, DILFL_EXECUTING))
      {
	 /* Set so as to notify command loop! Extracted recursively! */
	 prg->waitcmd = WAITCMD_DESTROYED;

	 /* The very very very dirty static template hack... :-) */
	 if (IS_SET(prg->sp->tmpl->flags, DILFL_FREEME))
	   dil_free_prg(prg);

	 sarg->fptr->data   = NULL;
      }
      else
      {
	 sarg->fptr->data   = NULL;
	 dil_free_prg(prg);
      }
      return SFR_BLOCK;
   }

   if (IS_SET(prg->flags, DILFL_EXECUTING | DILFL_DEACTIVATED))
     return SFR_SHARE;

   SET_BIT(prg->flags, DILFL_EXECUTING);
   REMOVE_BIT(prg->flags, DILFL_CMDBLOCK);

   /* For evaluating expressions */
   prg->sarg = sarg;

   /* A MEGA HACK! The DIL activated spells will not be tested for
      secures on first call only, to avoid loosing global pointers */
   if (prg->waitcmd == WAITCMD_MAXINST)
     dil_test_secure(prg);

   dil_clear_non_secured(prg);

   /* Test if the ON_ACTIVATION should be triggered */
   if (check_interrupt(prg) == 1)
   {
      REMOVE_BIT(prg->flags, DILFL_EXECUTING);
      sarg->arg = orgarg;
      return SFR_SHARE;
   }

   if (activates+1 > 50)
   {
      slog(LOG_ALL, 0, "RECURSIVE DIL ERROR IN %s", prg->sp->tmpl->prgname);
      return SFR_SHARE;
   }

   /* For optimization purposes */
   ubit16 OrgHeartBeat = sarg->fptr->heart_beat;

   activates++;

   while (prg->waitcmd > 0)
   {
      (prg)->sp->pc++;

#ifdef MUD_DEBUG
      (prg)->sp->tmpl->nActivations++;
#endif

      assert(prg->sp->pc <= &(prg->sp->tmpl->core[prg->sp->tmpl->coresz]));

      (dilfe_func[*(prg->sp->pc-1)].func ((prg), NULL));
   }

   activates--;

   sarg->arg = orgarg;

   if (prg->waitcmd <= WAITCMD_DESTROYED)
   {				/* Was it destroyed?? */
      sarg->fptr->data = NULL;
      destroy_fptr(sarg->owner, sarg->fptr);

      if (IS_SET(prg->flags, DILFL_CMDBLOCK))
      {
	 dil_free_prg(prg);
	 return SFR_BLOCK;
      }
      else
      {
	 dil_free_prg(prg);
	 return SFR_SHARE;
      }
   }
   else if (prg->waitcmd <= WAITCMD_QUIT)
   {
      destroy_fptr(sarg->owner, sarg->fptr);

      if (IS_SET(prg->flags, DILFL_CMDBLOCK))
      {
	 dil_free_prg(prg);
	 return SFR_BLOCK;
      }
      else
      {
	 dil_free_prg(prg);
	 return SFR_SHARE;
      }
   }
   else if (prg->waitcmd <= WAITCMD_STOP)
   {
      /* Just return and let the EXECUTING bit stay turned on, so all
	 execution is blocked */
      return SFR_SHARE;      
   }
   else if (prg->waitcmd > WAITCMD_FINISH)
   {
      szonelog(UNIT_FI_ZONE(sarg->owner), "DIL %s in unit %s@%s had "
	       "endless loop.",
	       prg->sp->tmpl->prgname,
	       UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_SHARE;
   }

   for (i=0; i < prg->sp->intrcount; i++)
     SET_BIT(prg->sarg->fptr->flags,
	     prg->sp->intr[i].flags);

   /* Okay this is the problem:
        Imagine this:

	heartbeat := 30 * PULSE_SEC;
	wait(SFB_CMD | SFB_TICK, TRUE);
	heartbeat := PULSE_SEC;
	pause;
	goto loop;
	
      It will only work, if the "SFB_TICK" is what triggers the execution.
      Therefore, I am unfortunately forced to do the follow dequeue and
      enqueue.
      */
   void ResetFptrTimer(struct unit_data *u, struct unit_fptr *fptr);

   sarg->fptr->heart_beat = MAX(PULSE_SEC*1, sarg->fptr->heart_beat);

   /* Purely for optimization purposes! Enqueue / dequeue are HUGE! */
   if ((OrgHeartBeat != sarg->fptr->heart_beat) &&
       (sarg->cmd->no != CMD_AUTO_TICK))
     ResetFptrTimer(sarg->owner, sarg->fptr);

   prg->waitcmd	      = WAITCMD_MAXINST;

   REMOVE_BIT(prg->flags, DILFL_EXECUTING);

   if (IS_SET(prg->flags, DILFL_CMDBLOCK))
     return SFR_BLOCK;
   else
     return SFR_SHARE;
}


int dil_init(struct spec_arg *sarg)
{
   if (sarg->cmd->no != CMD_AUTO_EXTRACT)
   {
      if (sarg->fptr->data)
	dil_copy((char *) sarg->fptr->data, sarg->owner);
      else
      {
	 szonelog(UNIT_FI_ZONE(sarg->owner),
		  "Template '(null)' not found: %s@%s",
		  UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      }
      destroy_fptr(sarg->owner,sarg->fptr);
   }

   return SFR_SHARE;
}

static void dil_free_dilargs(struct dilargstype *dilargs)
{
   free(dilargs->name);

   for (int i = 0; i < dilargs->no; i++)
     switch (dilargs->dilarg[i].type)
     {
       case DILV_SP:
	if (dilargs->dilarg[i].data.string)
	  free(dilargs->dilarg[i].data.string);
	break;

       case DILV_SLP:
	if (dilargs->dilarg[i].data.stringlist)
	  free_namelist(dilargs->dilarg[i].data.stringlist);
	break;
     }

   free(dilargs);
}

int dil_direct_init(struct spec_arg *sarg)
{
   struct dilargstype *dilargs = (struct dilargstype *) sarg->fptr->data;

   assert(dilargs);

   if (sarg->cmd->no != CMD_AUTO_EXTRACT)
   {
      struct dilprg *prg;
      struct diltemplate *tmpl;

      tmpl=find_dil_template(dilargs->name);

      if (tmpl == NULL)
      {
	 szonelog(UNIT_FI_ZONE(sarg->owner),
		  "Template '%s' not found: %s@%s",
		  dilargs->name,
		  UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      }
      else if (tmpl->argc != dilargs->no)
      {
	 szonelog(UNIT_FI_ZONE(sarg->owner),
		  "Template '%s' had mismatching argument count %d: %s@%s",
		  dilargs->name,
		  dilargs->no,
		  UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      }
      else
      {
	 int i;
	 for (i=0; i < dilargs->no; i++)
	   if (tmpl->argt[i] != dilargs->dilarg[i].type)
	     break;

	 if (i < dilargs->no)
	 {
	    szonelog(UNIT_FI_ZONE(sarg->owner),
		     "Template '%s' had argument "
		     "mismatch for argument %d: %s@%s",
		     dilargs->name, i, UNIT_FI_NAME(sarg->owner),
		     UNIT_FI_ZONENAME(sarg->owner));
	 }
	 else
	 {
	    prg = dil_copy_template(tmpl, sarg->owner, NULL);

	    if (prg && dilargs->no > 0)
	    {
	       for (i=0; i < dilargs->no; i++)
	       {
		  if (tmpl->argt[i] == DILV_SP)
		  {
		     prg->sp->vars[i].val.string = 
		       dilargs->dilarg[i].data.string;
		     dilargs->dilarg[i].data.string = NULL;
		  }
		  else if (tmpl->argt[i] == DILV_SLP)
		  {
		     prg->sp->vars[i].val.namelist = 
		       new cNamelist((const char **)
				     dilargs->dilarg[i].data.stringlist);
		  }
		  else if (tmpl->argt[i] == DILV_INT)
		  {
		     prg->sp->vars[i].val.integer = 
		       dilargs->dilarg[i].data.num;
		  }
	       }
	    }
	 }
      }

      destroy_fptr(sarg->owner,sarg->fptr);
   }
   else
     dil_free_dilargs(dilargs);

   sarg->fptr->data = NULL;

   return SFR_SHARE;
}


int dil_destroy(char *name, struct unit_data *u)
{
   struct unit_fptr *fptr;
   struct dilprg *prg;

   fptr=dil_find(name,u);
   if (fptr) {
      assert(fptr->data);	/* MUST or ged! */
      prg=((struct dilprg *)fptr->data);
      prg->waitcmd=WAITCMD_QUIT;
      dil_activate(prg);
      return TRUE;
   }
   return FALSE;
} 

void dil_init_vars(int varc, struct dilframe *frm)
{
   for (int i = 0; i < varc; i++)
   {
      switch (frm->vars[i].type)
      {
	case DILV_SLP:
	 frm->vars[i].val.namelist = new cNamelist;
	 break;

	case DILV_SP:
	 frm->vars[i].val.string = NULL;
	 break;

	default:
	 frm->vars[i].val.integer = 0;
      }
   }
}

struct dilprg *dil_copy_template(struct diltemplate *tmpl,
				 struct unit_data *u,
				 struct unit_fptr **pfptr)
{
   struct dilprg *prg;
   struct dilframe *frm;
   struct unit_fptr *fptr;

   CREATE(prg,struct dilprg,1);
   CREATE(frm,struct dilframe,1);

   /* GENERATE PRG STRUCTURE, LINK IN STACK; AND TEMPLATE, ETC */
   prg->owner = u;
   prg->next = dil_list;
   dil_list  = prg;
   prg->stack=frm;
   prg->sp=frm;
   prg->sp->tmpl=tmpl;
   prg->varcrc=tmpl->varcrc;
   prg->corecrc=tmpl->corecrc;
   prg->stacksz=1;
   prg->flags = DILFL_COPY | REMOVE_BIT(tmpl->flags, DILFL_EXECUTING |
					DILFL_CMDBLOCK);
   prg->waitcmd=WAITCMD_MAXINST-1;
   frm->pc=tmpl->core;

   frm->ret=DILV_NULL;		/* ignored, 1.frame */

   if (tmpl->varc)
     CREATE(frm->vars, struct dilvar, tmpl->varc);
   else
     frm->vars=NULL;

   for (int i = 0; i < tmpl->varc; i++)
     frm->vars[i].type = tmpl->vart[i];

   dil_init_vars(tmpl->varc, frm);

   if (tmpl->intrcount)
     CREATE(frm->intr,struct dilintr,tmpl->intrcount);
   else
     frm->intr=NULL;
   frm->intrcount = tmpl->intrcount;

   frm->securecount=0;
   frm->secure=NULL;

   /* activate on tick SOON! */
   if (IS_SET(tmpl->flags,DILFL_AWARE))
     fptr = create_fptr(u,SFUN_DIL_INTERNAL,1, SFB_ALL|SFB_AWARE, prg);
   else
     fptr = create_fptr(u,SFUN_DIL_INTERNAL,1, SFB_ALL, prg);

   if (pfptr)
     *pfptr = fptr;

   return prg;
}

void dil_activate(struct dilprg *prg)
{
   struct spec_arg sarg;
   struct unit_fptr *fptr;

   assert(prg);

   for (fptr = UNIT_FUNC(prg->owner); fptr; fptr = fptr->next)
     if (fptr->data == prg)
       break;

   assert(fptr);

#ifdef DEBUG_HISTORY
   void add_func_history(struct unit_data *u, ubit16, ubit16);
   add_func_history(prg->owner, SFUN_DIL_INTERNAL, SFB_TICK);
#endif

   sarg.owner     = prg->owner;
   sarg.activator = NULL;
   sarg.fptr      = fptr;
   sarg.cmd       = &cmd_auto_tick;
   sarg.arg       = "";
   sarg.mflags    = SFB_TICK;
   sarg.medium    = NULL;
   sarg.target    = NULL;
   sarg.pInt      = NULL;
   
   run_dil(&sarg);
}

void dil_loadtime_activate(struct unit_data *u)
{
   struct unit_fptr *f, *fnext;

   for (f = UNIT_FUNC(u); f; f = fnext)
   {
      fnext = f->next;

      if (f->index == SFUN_DIL_INTERNAL && f->data)
      {
	 dil_activate((struct dilprg *) f->data);
      }
   }
}

struct dilprg *dil_copy(char *name, struct unit_data *u)
{
   char buf[MAX_STRING_LENGTH];
   struct dilprg *prg;
   struct diltemplate *tmpl;
   char *tmplname,*farg;
   int narg, i;
   char *args[256];

   assert(name);
   assert(strlen(name) < sizeof(buf));

   strcpy(buf, name);

   tmplname=strtok(buf," (");
   farg = strtok(NULL, ")");

   narg = 0;

   if (farg)
   {
      /* To avoid "(,25) giving no arg as 1 - do -1 and set to ' '. */

      farg--;
      *farg = ' ';
      farg = str_dup(farg);

      args[0] = strtok(farg, ",");

      if (args[0])
      {
	 args[0]++; /* Skip the space we just generated! */

	 for (narg = 1; narg < (int) sizeof(args); narg ++)
	 {
	    args[narg] = strtok(NULL,",");

	    if (args[narg] == NULL)
	      break;
	 }
      }
   }

   tmpl=find_dil_template(tmplname);

   if (!tmpl)
   {
      szonelog(UNIT_FI_ZONE(u),
	       "Template '%s' not found: %s@%s",
	       tmplname,UNIT_FI_NAME(u), UNIT_FI_ZONENAME(u));
      if (farg)
	free(farg);
      return NULL;
   }

   /* check argument count and types */
   if (tmpl->argc!=narg)
   {
      szonelog(UNIT_FI_ZONE(u),
	       "Template '%s' had mismatching argument count %d: %s@%s",
	       tmplname, narg, UNIT_FI_NAME(u), UNIT_FI_ZONENAME(u));
      if (farg)
	free(farg);
      return NULL;
   }

   for (i=0; i < narg; i++)
   {
      if (tmpl->argt[i] == DILV_SP)
      {
	 continue;
      }
      else if (tmpl->argt[i] == DILV_INT)
      {
	 args[i] = skip_spaces(args[i]);
	 strip_trailing_spaces(args[i]);

	 if (str_is_number(args[i]))
	   continue;
      }

      szonelog(UNIT_FI_ZONE(u),
	       "Template '%s' had mismatching argument mismatch for %d: %s@%s",
	       tmplname, i, UNIT_FI_NAME(u), UNIT_FI_ZONENAME(u));      
      if (farg)
	free(farg);
      return NULL;
   }

   prg = dil_copy_template(tmpl, u, NULL);

   if (prg && narg > 0)
   {
      for (i=0; i < narg; i++)
      {
	 if (tmpl->argt[i] == DILV_SP)
	 {
	    prg->sp->vars[i].val.string=str_dup(args[i]);
	 }
	 else if (tmpl->argt[i] == DILV_INT)
	 {
	    prg->sp->vars[i].val.integer=atoi(args[i]);
	 }
	 else
	   error(HERE, "DIL COPY Arg parse");
      }
   }

   if (farg)
     free(farg);

   return prg;
}

struct unit_fptr *dil_find(const char *name, struct unit_data *u)
{
   struct unit_fptr *fptr;
   struct diltemplate *tmpl;

   if ((tmpl = find_dil_template(name)))
   {
      for (fptr=UNIT_FUNC(u);fptr;fptr=fptr->next) 
	if (fptr->index == SFUN_DIL_INTERNAL)
	  if (((struct dilprg *)fptr->data)->stack[0].tmpl == tmpl)
	    return fptr;
   }
   return NULL;
}
