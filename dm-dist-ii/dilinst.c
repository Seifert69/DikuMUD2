/* *********************************************************************** *
 * File   : dilinst.c                                 Part of Valhalla MUD *
 * Version: 2.00                                                           *
 * Author : bombman@diku.dk & seifert@diku.dk				   *
 *                                                                         *
 *                                                                         *
 * Purpose: DIL Instructions.                                              *
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
#include "dilrun.h"
#include "magic.h"
#include "trie.h"
#include "main.h"
#include "account.h"
#include "justice.h"

/* report error in instruction */
void dil_insterr(struct dilprg *p, const char *where)
{
   /* instruction called as an expression! */
   /* This is serous! mess-up in the core.. stop the program */

   szonelog(UNIT_FI_ZONE(p->sarg->owner),
      "DIL %s@%s, Instruction error in %s\n",
      UNIT_FI_NAME(p->sarg->owner),
      UNIT_FI_ZONENAME(p->sarg->owner),
      where);
   p->waitcmd = WAITCMD_QUIT;
}


/* ************************************************************************ */
/* DIL-instructions							    */
/* ************************************************************************ */

/* foreach - clear / build */
void dilfi_foe(struct dilprg *p, class dilval *v)
{
   class dilval v1;
   int i;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"foreach - build");
      return;
   }
   
   eval_dil_exp(p,&v1);

   if (dil_type_check("foreach - build", p, 1,
		  &v1, TYPEFAIL_NULL, 1, DILV_INT))
     return;

   /* build environment into secures with NULL labels */

   dil_getval(&v1);

   if (v1.val.num)
   { 
      for (i=0;i<p->sp->securecount;i++)
	if (p->sp->secure[i].lab == NULL)
	{
	   dil_sub_secure(p->sp, p->sp->secure[i].sup, TRUE);
	   i--; // Shit
	}

      if (UNIT_IN(p->sarg->owner))
         scan4_unit(p->sarg->owner,v1.val.num);
      else
         scan4_unit_room(p->sarg->owner,v1.val.num);

      for(i = 0; i < unit_vector.top; i++)
	dil_add_secure(p,UVI(i), NULL);
      dil_add_secure(p, p->sarg->owner, NULL);
   }
}

/* foreach - next */
void dilfi_fon(struct dilprg *p, class dilval *v)
{
   class dilval v1;
   struct unit_data *u;
   ubit32 adr;
   int i;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"foreach - next");
      return;
   }
   
   eval_dil_exp(p,&v1);
   adr=bread_ubit32(&(p->sp->pc));
   p->waitcmd--;

   if (v1.type != DILV_UPR)
   {
      dil_typeerr(p,"foreach - next");
      return;
   }
   
   /* look for NULL references, remove first */
   u=NULL;
   for (i=0;i<p->sp->securecount;i++)
     if (!p->sp->secure[i].lab) {
        u=p->sp->secure[i].sup;
        break;
     }

   if (!u) {
      /* no new in environment found, exit loop */
      p->sp->pc = &(p->sp->tmpl->core[adr]);
   }
   else
   {
      /* assign variable the new value */
      dil_sub_secure(p->sp, u, TRUE);
      *((unit_data **) v1.ref) = u;
   }
}

/* store */
void dilfi_stor(struct dilprg *p, class dilval *v)
{
   class dilval v1;
   int dif;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"store");
      return;
   }
   
   eval_dil_exp(p, &v1);

   if (dil_type_check("store", p, 1,
		  &v1, TYPEFAIL_NULL, 1, DILV_UP) < 0)
     return;

   if (v1.val.ptr)
   {
      void store_unit(struct unit_data *u);

      if (!IS_ROOM((struct unit_data *) v1.val.ptr))
	store_unit((struct unit_data *) v1.val.ptr);
   }
}

/* set bright */
void dilfi_sbt(struct dilprg *p, class dilval *v)
{
   class dilval v1,v2;
   int dif;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"setbright");
      return;
   }
   
   eval_dil_exp(p,&v1);
   eval_dil_exp(p,&v2);

   if (dil_type_check("setbright", p, 2,
		  &v1, TYPEFAIL_NULL, 1, DILV_UP,
		  &v2, FAIL_NULL, 1, DILV_INT) < 0)
     return;

   if (v1.val.ptr)
   {
      dif = v2.val.num - UNIT_BRIGHT((struct unit_data *) v1.val.ptr);
      
      modify_bright( (struct unit_data *) v1.val.ptr, dif);
   }
}


/* acc_modify */
void dilfi_amod(struct dilprg *p, class dilval *v)
{
   class dilval v1, v2;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"acc_modify");
      return;
   }
   
   eval_dil_exp(p,&v1);
   eval_dil_exp(p,&v2);

   if (dil_type_check("acc_modify", p, 2,
		  &v1, TYPEFAIL_NULL, 1, DILV_UP,
		  &v2, FAIL_NULL, 1, DILV_INT) < 0)
     return;

   if (g_cServerConfig.m_bAccounting &&
       v1.val.ptr && IS_PC((struct unit_data *) v1.val.ptr))
   {
      if (p->stack[0].tmpl->zone->access != 0)
      {
	 szonelog(p->stack->tmpl->zone,
		  "DIL '%s' attempt to violate system access security (amod).",
		  p->stack->tmpl->prgname);
	 p->waitcmd = WAITCMD_QUIT;
      }
      else
      {

	 if (v2.val.num > 0)
	   account_insert(p->owner,
			  (struct unit_data *)v1.val.ptr, v2.val.num);
	 else if (v2.val.num < 0)
	   account_withdraw(p->owner,
			    (struct unit_data *)v1.val.ptr, -v2.val.num);
      }
   }
}


/* set weight */
void dilfi_swt(struct dilprg *p, class dilval *v)
{
   class dilval v1,v2;
   int dif;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"setweight");
      return;
   }
   
   eval_dil_exp(p,&v1);
   eval_dil_exp(p,&v2);

   if (dil_type_check("setweight", p, 2,
		  &v1, TYPEFAIL_NULL, 1, DILV_UP,
		  &v2, FAIL_NULL, 1, DILV_INT) < 0)
     return;

   if (v1.val.ptr)
   {
      dif = v2.val.num - UNIT_BASE_WEIGHT((struct unit_data *) v1.val.ptr);

      /* set new baseweight */
      UNIT_BASE_WEIGHT((struct unit_data *) v1.val.ptr) = v2.val.num;

      /* update weight */
      weight_change_unit((struct unit_data *) v1.val.ptr, dif);
   }
}


/* change_speed */
void dilfi_chas(struct dilprg *p, class dilval *v)
{
   class dilval v1,v2;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"change_speed");
      return;
   }
   
   eval_dil_exp(p,&v1);
   eval_dil_exp(p,&v2);

   if (dil_type_check("change_speed", p, 2,
		  &v1, TYPEFAIL_NULL, 1, DILV_UP,
		  &v2, FAIL_NULL, 1, DILV_INT) < 0)
     return;

   if (v1.val.ptr && IS_CHAR((struct unit_data *) v1.val.ptr) &&
       CHAR_COMBAT((struct unit_data *) v1.val.ptr))
     CHAR_COMBAT((struct unit_data *) v1.val.ptr)->changeSpeed(v2.val.num);
}


/* set_fighting */
void dilfi_setf(struct dilprg *p, class dilval *v)
{
   class dilval v1,v2;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"set_fighting");
      return;
   }
   
   eval_dil_exp(p,&v1);
   eval_dil_exp(p,&v2);

   if (dil_type_check("set_fighting", p, 2,
		  &v1, TYPEFAIL_NULL, 1, DILV_UP,
		  &v2, TYPEFAIL_NULL, 1, DILV_UP) < 0)
     return;

   if (v1.val.ptr && IS_CHAR((struct unit_data *) v1.val.ptr) &&
       v2.val.ptr && IS_CHAR((struct unit_data *) v2.val.ptr))
   {
      if (CHAR_FIGHTING((struct unit_data *) v1.val.ptr))
	set_fighting((struct unit_data *) v1.val.ptr,
		     (struct unit_data *) v2.val.ptr, FALSE);
      else
	set_fighting((struct unit_data *) v1.val.ptr,
		     (struct unit_data *) v2.val.ptr, TRUE);
   }
}


/* clear interrupt */
void dilfi_cli(struct dilprg *p, class dilval *v)
{
   class dilval v1;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"clear");
      return;
   }

   eval_dil_exp(p,&v1);

   if (dil_type_check("clear", p, 1,
		  &v1, FAIL_NULL, 1, DILV_INT) < 0)
     return;

   dil_intr_remove(p, v1.val.num);
}



/* Return from proc */
void dilfi_rts(struct dilprg *p, class dilval *v)
{
   int i;
   
   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"return");
      return;
   }

   p->waitcmd--;

   if (p->sp==p->stack)
   {
      /* just stop execution, never discard last stackframe! */
      p->waitcmd = WAITCMD_QUIT;
      return;
   }

   i = p->sp - p->stack - 1;

   dil_free_frame(p->sp);

   /* pop stack frame */
   p->sp=&p->stack[i];
}


void dilfi_rtf(struct dilprg *p, class dilval *v)
{
   struct dilframe *cfrm;
   class dilval v1;
   int i;
   
   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"return <exp>");
      return;
   }

   p->waitcmd--;

   eval_dil_exp(p, &v1);

   if (p->sp==p->stack)
   {
      /* just stop execution, never discard last stackframe! */
      p->waitcmd = WAITCMD_QUIT;
      return;
   }

   i = p->sp - p->stack -1;

   cfrm = p->sp;

   /* pop stack frame */
   p->sp=&p->stack[i];

   /* copy return variable */
   i = p->sp->ret;

   dil_free_var(&p->sp->vars[i]);

   switch (dil_getval(&v1))
   {
     case DILV_UP:
      p->sp->vars[i].val.unitptr = (struct unit_data *) v1.val.ptr;
      break;

     case DILV_SP:
      p->sp->vars[i].val.string=str_dup((char *)v1.val.ptr);
      break;

     case DILV_SLP:
      p->sp->vars[i].val.namelist = ((cNamelist *) v1.val.ptr)->
	Duplicate();
      break;

     case DILV_EDP:
      p->sp->vars[i].val.extraptr = (class extra_descr_data *) v1.val.ptr;
      break;

     case DILV_INT:
      p->sp->vars[i].val.integer=v1.val.num;
      break;

     default:
      p->sp->vars[i].type=DILV_ERR;
      dil_typeerr(p, "function call evaltuated to failed result.");
      break;
   }

   dil_free_frame(cfrm);
}


void dil_pop_frame(struct dilprg *p)
{
   dilfi_rts(p, NULL);  /* XXX Is this a potential memory leak rts / rtf? */
}


/*
   'p' is the program in which a new frame should be created.
   'rtmpl' is the template which the frame should be based upon.
   'av' are the arguments which are to be passed to the new frame.
*/

void dil_push_frame(struct dilprg *p, struct diltemplate *rtmpl,
		      class dilval *av)
{
   int i;
   struct dilframe *frm;

   i = p->sp - p->stack;

   if (i+2 > p->stacksz)
   {
      p->stacksz += DIL_STACKINC;
      RECREATE(p->stack, struct dilframe, p->stacksz);
   }

   frm   = &p->stack[i+1];
   p->sp = frm;

   frm->tmpl        = rtmpl;
   frm->pc          = rtmpl->core;
   frm->ret         = 0;
   frm->securecount = 0;
   frm->secure      = NULL;

   frm->intrcount = rtmpl->intrcount;

   if (rtmpl->intrcount)
     CREATE(frm->intr, struct dilintr, rtmpl->intrcount);
   else
     frm->intr=NULL;

   if (rtmpl->varc)
   {
      CREATE(frm->vars, struct dilvar, rtmpl->varc);

      for (i=0; i < rtmpl->varc; i++)
      {
	 frm->vars[i].type       = rtmpl->vart[i];
	 if (i >= rtmpl->argc)
	 {
	    if (frm->vars[i].type == DILV_SLP)
	      frm->vars[i].val.namelist = new cNamelist;
	    else
	      frm->vars[i].val.string = NULL;
	 }
	 else
	   frm->vars[i].val.string = NULL;
      }
   }
   else
     frm->vars = NULL;

   ubit8 tmp;

   for (i=0; i < rtmpl->argc; i++)
   {
      tmp = dil_getval(&av[i]);

      if (tmp != frm->vars[i].type) /* Can fail, err, be null, etc */
      {
	 if (tmp != DILV_NULL)
	 {
	    dil_pop_frame(p);
	    return;
	 }
      }

      switch (frm->vars[i].type)
      {
	case DILV_UP:
	 frm->vars[i].val.unitptr = (struct unit_data *) av[i].val.ptr;
	 break;

	case DILV_SP:
	 frm->vars[i].val.string     = str_dup((char *) av[i].val.ptr);
	 break;

	case DILV_SLP:
	 if (tmp != DILV_NULL)
	   frm->vars[i].val.namelist = ((cNamelist *) av[i].val.ptr)->
	     Duplicate();
	 break;

	case DILV_EDP:
	 frm->vars[i].val.extraptr   = (class extra_descr_data *)
	   av[i].val.ptr;
	 break;

	case DILV_INT:
	 frm->vars[i].val.integer    = av[i].val.num;
	 break;

	default:
	 error(HERE, "Impossible!");
      }
   }
}


/* Remote procedure call */
void dilfi_rproc(struct dilprg *p, class dilval *v)
{
   int xrefi,i;
   struct diltemplate *ctmpl;
   class dilval av[255];
   ubit16 argcnt;
   
   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"remote procedure call");
      return;
   }

   p->waitcmd--;

   /*
    * <rpcall> <funcnumber> <nargs> <arg> <arg> ...
    */

   ctmpl= p->sp->tmpl;
   xrefi = bread_ubit16(&(p->sp->pc)); /* <funcnumber> */
   argcnt = bread_ubit16(&(p->sp->pc)); /* <argc>, ignored */

   if (!ctmpl->extprg[xrefi])
   {
     /*
      * This function had a lost reference! 
      * stop the DIL program executing,
      * write error, but do not slime it.
      */
      szonelog(UNIT_FI_ZONE(p->sarg->owner),
         "DIL %s@%s, Error in remote call\n",
         UNIT_FI_NAME(p->sarg->owner),
         UNIT_FI_ZONENAME(p->sarg->owner));
     p->waitcmd = WAITCMD_STOP;
     return;
   }
 
   /* evaluate arguments */
   for (i=0;i<ctmpl->xrefs[xrefi].argc;i++) 
     eval_dil_exp(p, &av[i]);

   /* expand stack */

   dil_push_frame(p, ctmpl->extprg[xrefi], av);
}

/* Remote symbolic procedure call */
void dilfi_rsproc(struct dilprg *p, class dilval *v)
{
   int i;
   struct diltemplate *ctmpl,*ntmpl;
   class dilval av[255];
   class dilval v1;
   ubit8 argcnt;
   
   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"remote procedure call");
      return;
   }

   p->waitcmd--;

   /*
    * <rspcall> <funcname> <nargs> <arg> <arg> ...
    */

   ctmpl= p->sp->tmpl;
   eval_dil_exp(p, &v1);
   argcnt = (ubit8) bread_ubit16(&(p->sp->pc)); /* <argc> */

   /* evaluate arguments */
   for (i=0;i<argcnt;i++) 
     eval_dil_exp(p, &av[i]);

   if (dil_getval(&v1) != DILV_SP)
     return; /* error finding proc name, skip */
 
   ntmpl=find_dil_template((char *)v1.val.ptr);

   if (!ntmpl)
     return; /* error finding template, skip */

   if (ntmpl->argc!=argcnt)
     return; /* wrong number of arguments */

   for (i=0; i < argcnt; i++)
   {
      int t = dil_getval(&av[i]);

      switch (ntmpl->argt[i])
      {
	case DILV_INT:
	 if (t != DILV_INT)
	   return; /* type error, expect integer */
	 break;
 	case DILV_SP:
	 if ((t != DILV_NULL) && (t != DILV_SP))
	   return; /* type error, expect string */
	 break;
	case DILV_UP:
	 if ((t != DILV_NULL) && (t != DILV_UP))
	   return; /* type error, expect unitptr */
	 break;

	case DILV_EDP:
	 if ((t != DILV_NULL) && (t != DILV_EDP))
	   return; /* type error, expect extraptr */
	 break;

	case DILV_SLP:
	 if ((t != DILV_NULL) && (t != DILV_SLP))
	   return; /* type error, expect stringlist */
	 break;
      }
   }
	 
   /* expand stack */

   dil_push_frame(p, ntmpl, av);
}



/* Remote function call */
void dilfi_rfunc(register struct dilprg *p, register class dilval *v)
{
   int xrefi,vari,i;
   struct diltemplate *ctmpl;
   class dilval av[255];
   ubit16 argcnt;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"remote func call");
      return;
   }

   p->waitcmd--;

   /*
    * <rfcall> <var> <var#> <funcnumber> <nargs> <arg> <arg> ...
    */

   ctmpl= p->sp->tmpl;
   p->sp->pc++; /* skip <var> */
   vari = bread_ubit16(&(p->sp->pc)); /* <var#> */
   xrefi = bread_ubit16(&(p->sp->pc)); /* <funcnumber> */
   argcnt = bread_ubit16(&(p->sp->pc)); /* <nargs>, ignored */
   if (!ctmpl->extprg[xrefi]) {
     /*
      * This function had a lost reference! 
      * stop the DIL program executing,
      * write error, but do not slime it.
      */
      szonelog(UNIT_FI_ZONE(p->sarg->owner),
         "DIL %s@%s, Error in remote call\n",
         UNIT_FI_NAME(p->sarg->owner),
         UNIT_FI_ZONENAME(p->sarg->owner));
     p->waitcmd = WAITCMD_STOP;
     return;
   }
   
   /* evaluate arguments */
   for (i=0;i<ctmpl->xrefs[xrefi].argc;i++)
     eval_dil_exp(p, &av[i]);

   /* RTF expects destination variable to be in parental frame */
   p->sp->ret = vari;

   /* expand stack */

   dil_push_frame(p, ctmpl->extprg[xrefi], av);
}

/* Remote symbolic function call */
void dilfi_rsfunc(register struct dilprg *p, register class dilval *v)
{
   int vari,i;
   struct diltemplate *ctmpl,*ntmpl;
   class dilval av[255];
   class dilval v1;
   ubit8 argcnt;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"remote func call");
      return;
   }

   p->waitcmd--;

   /*
    * <rsfcall> <var> <var#> <funcname> <nargs> <arg> <arg> ...
    */

   ctmpl= p->sp->tmpl;
   p->sp->pc++; /* skip <var> */
   vari = bread_ubit16(&(p->sp->pc)); /* <var#> */
   eval_dil_exp(p, &v1);               /* funcname */
   argcnt = bread_ubit16(&(p->sp->pc)); /* <nargs>, ignored */

   /* evaluate arguments */
   for (i=0;i<argcnt;i++) 
   {
      eval_dil_exp(p, &av[i]);
      dil_getval(&av[i]); /* Call by values only! */
   }

   if (dil_getval(&v1) != DILV_SP)
     return; /* error finding proc name, skip */
 
   ntmpl=find_dil_template((char *)v1.val.ptr);

   if (!ntmpl)
     return; /* error finding template, skip */

   if (ntmpl->argc!=argcnt)
     return; /* wrong number of arguments */

   for(i=0;i<argcnt;i++)
   {
      int t  = dil_getval(&av[i]);

      switch (ntmpl->argt[i])
      {
	case DILV_INT:
	 if (t != DILV_INT)
	   return; /* type error, expect integer */
	 break;
 	case DILV_SP:
	 if ((t != DILV_NULL) &&
	     (t != DILV_SP))
	   return; /* type error, expect string */
	 break;
	case DILV_UP:
	 if ((t != DILV_NULL) &&
	     (t != DILV_UP))
	   return; /* type error, expect unitptr */
	 break;
	case DILV_EDP:
	 if ((t != DILV_NULL) &&
	     (t != DILV_EDP))
	   return; /* type error, expect extraptr */
	 break;
	case DILV_SLP:
	 if ((t != DILV_NULL) &&
	     (t != DILV_SLP))
	   return; /* type error, expect stringlist */
	 break;
      }
   }
	 

   /* RTF expects destination variable to be in parental frame */
   p->sp->ret = vari;

   /* expand stack */

   dil_push_frame(p, ntmpl, av);
}



/* Assignment of value to reference */
void dilfi_ass(register struct dilprg *p, register class dilval *v)
{
   class dilval v1, v2;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"assignment");
      return;
   }

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   p->waitcmd--;

   switch (v1.type)
   {
     case DILV_NULL:
     case DILV_FAIL:		/* just ignore */
      break;

      /* unit pointer assignment */
     case DILV_UPR:
      switch (dil_getval(&v2))
      {
	case DILV_FAIL:
	 break;
	case DILV_NULL:
	 *((void **) v1.ref) = NULL;
	 break;

	case DILV_UP:
	 *((struct unit_data **) v1.ref) =
	   (struct unit_data *) v2.val.ptr;
	 break;

	default:
	 /* ERROR incompatible types */
	 dil_typeerr(p,"unit assignemt");
	 break;
      }
      break;
      
     case DILV_HASHSTR:
      switch (dil_getval(&v2))
      {
	case DILV_FAIL:
	 break;
	 
	case DILV_NULL:
	 ((cStringInstance *) v1.ref)->Reassign(NULL);
	 break;
	 
	case DILV_SP:
	 ((cStringInstance *) v1.ref)->Reassign((char *) v2.val.ptr);
	 break;
 
	case DILV_HASHSTR:
	 ((cStringInstance *) v1.ref)->
	   Reassign(((cStringInstance *) v2.ref)->StringPtr());
	 break;
	 
	default:
	 dil_typeerr(p,"hash-string assignment");
	 break;
      }
      break;

      /* string assignment */
     case DILV_SPR:
      /* free potential old string */
      if (v1.atyp == DILA_NORM)
      {
	 if (*((char **) v1.ref))
	   free(*((char **) v1.ref));
      }
      else
	dil_typeerr(p, "ordinary string assignment <- hash");
      
      switch (dil_getval(&v2))
      {
	case DILV_FAIL:
	 break;

	case DILV_NULL:
	 *((void **) v1.ref) = NULL;
	 break;

	case DILV_HASHSTR:
	 *((char **) v1.ref) = str_dup(((cStringInstance *)
					v2.ref)->StringPtr());
	 break;

	case DILV_SP:
	 *((char **) v1.ref) = str_dup((char *) v2.val.ptr);
	 break;

	default:
	 /* ERROR incompatible types */
	 dil_typeerr(p,"string assignemt");
	 break;
      }
      break;
      
     case DILV_SLPR:
      /* String list assignment. The old stringlist is */
      /* discarded, and replaced with the new */
      switch (dil_getval(&v2))
      {
	case DILV_FAIL:
	 break;

	case DILV_NULL:
	 /* assign empty list */
	 assert((cNamelist *) v1.ref);
	 ((cNamelist *) v1.ref)->Free();
	 break;

	case DILV_SLP:
         /* string list assignment */
         assert((cNamelist *) v1.ref);

	 ((cNamelist *) v1.ref)->Free();
	    
	 if (v2.val.ptr)
	   ((cNamelist *) v1.ref)->
	     CopyList((cNamelist *) v2.val.ptr);
         break;

	default:
	 /* ERROR incompatible types */
	 dil_typeerr(p,"namelist SPLR assignemt");
	 break;
      }
      break;

     case DILV_EDPR:
      /* assignment of extra decr pointer variable */
      switch (dil_getval(&v2))
      {
	case DILV_FAIL:
	 break;

	case DILV_NULL:
	 /* assign empty list */
	 *((void **) v1.ref) = NULL;
	 break;

	case DILV_EDP:
	 *((class extra_descr_data **) v1.ref) =
	   (class extra_descr_data *) v2.val.ptr;
	 break;

	default:
	 /* ERROR incompatible types */
	 dil_typeerr(p,"exdp assignemt");
	 break;
      }
      break;

     case DILV_SINT1R:
      /* assignment of signed 8bit integer */
      switch (dil_getval(&v2))
      {
	case DILV_FAIL:
	 break;
	case DILV_INT:
	 *((sbit8 *) v1.ref) = v2.val.num;
	 break;
	default:
	 /* ERROR incompatible types */
	 dil_typeerr(p,"integer assignemt");
	 break;
      }
      break;

     case DILV_SINT2R:
      /* assignment of signed 16bit integer */
      switch (dil_getval(&v2))
      {
	case DILV_FAIL:
	 break;
	case DILV_INT:
	 *((sbit16 *) v1.ref) = v2.val.num;
	 break;
	default:
	 /* ERROR incompatible types */
	 dil_typeerr(p,"integer assignemt");
	 break;
      }
      break;

     case DILV_SINT4R:
      /* assignment of signed 32bit integer */
      switch (dil_getval(&v2))
      {
	case DILV_FAIL:
	 break;
	case DILV_INT:
	 *((sbit32 *) v1.ref) = v2.val.num;
	 break;
	default:
	 /* ERROR incompatible types */
	 dil_typeerr(p,"integer assignemt");
	 break;
      }
      break;

     case DILV_UINT1R:
      /* assignment of unsigned 8bit integer */
      switch (dil_getval(&v2))
      {
	case DILV_FAIL:
	 break;
	case DILV_INT:
	 *((ubit8 *) v1.ref) = v2.val.num;
	 break;
	default:
	 /* ERROR incompatible types */
	 dil_typeerr(p,"integer assignemt");
	 break;
      }
      break;

     case DILV_UINT2R:
      /* assignment of unsigned 16bit integer */
      switch (dil_getval(&v2))
      {
	case DILV_FAIL:
	 break;
	case DILV_INT:
	 *((ubit16 *) v1.ref) = v2.val.num;
	 break;
	default:
	 /* ERROR incompatible types */
	 dil_typeerr(p,"integer assignemt");
	 break;
      }
      break;

     case DILV_UINT4R:
      /* assignment of unsigned 32bit integer */
      switch (dil_getval(&v2))
      {
	case DILV_FAIL:
	 break;
	case DILV_INT:
	 *((ubit32 *) v1.ref) = v2.val.num;
	 break;
	default:
	 /* ERROR incompatible types */
	 dil_typeerr(p,"integer assignemt");
	 break;
      }
      break;

     default:
      /* not an lvalue! */
      dil_typeerr(p,"lvalue assignemt");
      break;
   }
}

/* Link unit into other unit */
void dilfi_lnk(register struct dilprg *p, register class dilval *v)
{
   class dilval v1, v2;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"link");
      return;
   }

   eval_dil_exp(p, &v1);	/* unit to link */
   eval_dil_exp(p, &v2);	/* unit to link into */

   p->waitcmd--;

   if (dil_type_check("link", p, 2,
		  &v1, FAIL_NULL, 1, DILV_UP,
		  &v2, FAIL_NULL, 1, DILV_UP) < 0)
     return;

   if (v1.val.ptr && v2.val.ptr)
   {
      if (IS_OBJ((struct unit_data *) v1.val.ptr) &&
	  OBJ_EQP_POS((struct unit_data *) v1.val.ptr))
      {
	 unequip_object((struct unit_data *) v1.val.ptr);
      }
      if (!unit_recursive((struct unit_data *) v1.val.ptr,
			  (struct unit_data *) v2.val.ptr) &&
	  (!IS_ROOM((struct unit_data *) v1.val.ptr) ||
	   IS_ROOM((struct unit_data *) v2.val.ptr)))
      {
	 unit_from_unit((struct unit_data *) v1.val.ptr);
	 unit_to_unit((struct unit_data *) v1.val.ptr,
		      (struct unit_data *) v2.val.ptr);
      }
   }
}

/* dilcopy */
void dilfi_dlc(register struct dilprg *p, register class dilval *v)
{
   class dilval v1, v2;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"dilcopy");
      return;
   }

   eval_dil_exp(p, &v1);	/* name of prg */
   eval_dil_exp(p, &v2);	/* unit to link into */

   p->waitcmd--;

   if (dil_type_check("dilcopy", p, 2,
		  &v1, TYPEFAIL_NULL, 1, DILV_SP,
		  &v2, TYPEFAIL_NULL, 1, DILV_UP) < 0)
     return;

   if (v1.val.ptr && v2.val.ptr)
     dil_copy((char *) v1.val.ptr, (struct unit_data *) v2.val.ptr);
}

/* sendtext */
void dilfi_sete(register struct dilprg *p, register class dilval *v)
{
   class dilval v1, v2;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"sendtext");
      return;
   }

   eval_dil_exp(p, &v1);	/* name of prg */
   eval_dil_exp(p, &v2);	/* unit to link into */

   p->waitcmd--;

   if (dil_type_check("sendtext", p, 2,
		  &v1, TYPEFAIL_NULL, 1, DILV_SP,
		  &v2, TYPEFAIL_NULL, 1, DILV_UP) < 0)
     return;

   if (v1.val.ptr && v2.val.ptr && IS_CHAR((struct unit_data*)v2.val.ptr))
     send_to_char((char *) v1.val.ptr, (struct unit_data *) v2.val.ptr);
}


/* Set one char to follow another unconditionally */
void dilfi_folo(register struct dilprg *p, register class dilval *v)
{
   class dilval v1, v2;
   sbit32 value = 0;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"folo");
      return;
   }

   eval_dil_exp(p, &v1);	/* follower */
   eval_dil_exp(p, &v2);	/* master   */

   p->waitcmd--;

   if (dil_type_check("follow", p, 2,
		  &v1, TYPEFAIL_NULL, 1, DILV_UP,
		  &v2, TYPEFAIL_NULL, 1, DILV_UP) < 0)
     return;

   if (v1.val.ptr && v2.val.ptr &&
       IS_CHAR((struct unit_data *) v1.val.ptr) &&
       IS_CHAR((struct unit_data *) v2.val.ptr))
   {
      if (CHAR_MASTER((struct unit_data *) v1.val.ptr))
	stop_following((struct unit_data *) v1.val.ptr);

      start_following((struct unit_data *) v1.val.ptr,
		      (struct unit_data *) v2.val.ptr);
   }
}

/* logcrime */
void dilfi_lcri(register struct dilprg *p, register class dilval *v)
{
   class dilval v1, v2, v3;
   sbit32 value = 0;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"lcri");
      return;
   }

   eval_dil_exp(p, &v1);	/* criminal  */
   eval_dil_exp(p, &v2);	/* victim    */
   eval_dil_exp(p, &v3);	/* crimetype */

   p->waitcmd--;

   if (dil_type_check("logcrime", p, 3,
		  &v1, TYPEFAIL_NULL, 1, DILV_UP,
		  &v2, TYPEFAIL_NULL, 1, DILV_UP,
		  &v3, TYPEFAIL_NULL, 1, DILV_INT) < 0)
     return;

   if (v1.val.ptr && v2.val.ptr &&
       IS_CHAR((struct unit_data *) v1.val.ptr) &&
       IS_CHAR((struct unit_data *) v2.val.ptr))
   {
      
      log_crime((struct unit_data *) v1.val.ptr,
		(struct unit_data *) v2.val.ptr,
		v3.val.num);
   }
}

/* Assign EXP to player */
void dilfi_exp(register struct dilprg *p, register class dilval *v)
{
   class dilval v1, v2;
   sbit32 value = 0;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"exp");
      return;
   }

   eval_dil_exp(p, &v1);	/* value */
   eval_dil_exp(p, &v2);	/* pc */

   p->waitcmd--;

   if (dil_type_check("exp", p, 2,
		  &v1, TYPEFAIL_NULL, 1, DILV_INT,
		  &v2, FAIL_NULL, 1, DILV_UP) < 0)
     return;

   if (dil_getval(&v1) == DILV_INT)
     value = v1.val.num;

   if (v2.val.ptr && IS_PC((struct unit_data *) v2.val.ptr))
   {
      value = MAX(-level_xp(CHAR_LEVEL((struct unit_data *) v2.val.ptr)),
		  value);

      value = MIN(level_xp(CHAR_LEVEL((struct unit_data *) v2.val.ptr)),
		  value);

      slog(LOG_ALL, 0, "%s gained %d experience from unit %s@%s.",
	   UNIT_NAME((struct unit_data *) v2.val.ptr), value,
	   UNIT_FI_NAME(p->sarg->owner),
	   UNIT_FI_ZONENAME(p->sarg->owner));
      gain_exp((struct unit_data *) v2.val.ptr, value);
   }
}

/* Branch on expression */
void dilfi_if(register struct dilprg *p, register class dilval *v)
{
   class dilval v1;
   ubit32 coreptr;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"if");
      return;
   }

   eval_dil_exp(p, &v1);
   coreptr = bread_ubit32(&(p->sp->pc)); /* else branch */

   p->waitcmd--;
   if (!dil_getbool(&v1))		/* might be pointer, but ok! */
     p->sp->pc = &(p->sp->tmpl->core[coreptr]); /* choose else branch */
}

/* Set bits in integer */
void dilfi_set(register struct dilprg *p, register class dilval *v)
{
   class dilval v1, v2;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"set");
      return;
   }

   eval_dil_exp(p, &v1);	/* integer reference to set */
   eval_dil_exp(p, &v2);	/* bits to set */

   p->waitcmd--;

   if (dil_type_check("set", p, 1,
		  &v2, TYPEFAIL_NULL, 1, DILV_INT) < 0)
     return;

   switch (v1.type)
   {
     case DILV_FAIL:
      return;

     case DILV_SINT1R:
      *((sbit8 *) v1.ref) |= v2.val.num;
      break;
     case DILV_SINT2R:
      *((sbit16 *) v1.ref) |= v2.val.num;
      break;
     case DILV_SINT4R:
      *((sbit32 *) v1.ref) |= v2.val.num;
      break;
     case DILV_UINT1R:
      *((ubit8 *) v1.ref) |= v2.val.num;
      break;
     case DILV_UINT2R:
      *((ubit16 *) v1.ref) |= v2.val.num;
      break;
     case DILV_UINT4R:
      *((ubit32 *) v1.ref) |= v2.val.num;
      break;
     default:
      /* not an lvalue! */
      dil_typeerr(p, "lvalue set");
      break;
   }
}

/* Unset bits in integer */
void dilfi_uset(register struct dilprg *p, register class dilval *v)
{
   class dilval v1, v2;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"unset");
      return;
   }

   eval_dil_exp(p, &v1);	/* integer reference to set */
   eval_dil_exp(p, &v2);	/* bits to set */

   p->waitcmd--;

   if (dil_type_check("unset", p, 1,
		  &v2, TYPEFAIL_NULL, 1, DILV_INT) < 0)
      return;

   switch (v1.type)
   {
     case DILV_FAIL:
      return;

     case DILV_SINT1R:
      REMOVE_BIT(*((sbit8 *) v1.ref), v2.val.num);
      break;
     case DILV_SINT2R:
      REMOVE_BIT(*((sbit16 *) v1.ref), v2.val.num);
      break;
     case DILV_SINT4R:
      REMOVE_BIT(*((sbit32 *) v1.ref), v2.val.num);
      break;
     case DILV_UINT1R:
      REMOVE_BIT(*((ubit8 *) v1.ref), v2.val.num);
      break;
     case DILV_UINT2R:
      REMOVE_BIT(*((ubit16 *) v1.ref), v2.val.num);
      break;
     case DILV_UINT4R:
      REMOVE_BIT(*((ubit32 *) v1.ref), v2.val.num);
      break;

     default:
      /* not an lvalue! */
      dil_typeerr(p, "lvalue unset");
      break;
   }
}

/* Add element to string list (addstring) */
void dilfi_adl(register struct dilprg *p, register class dilval *v)
{
   class dilval v1, v2;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"addstring");
      return;
   }

   eval_dil_exp(p, &v1);	/* string list */
   eval_dil_exp(p, &v2);	/* string */

   p->waitcmd--;

   switch (v1.type)
   {
     case DILV_FAIL:
      return;

     case DILV_SLPR:
      break;

     default:
      /* ERROR not right lvalue */
      dil_typeerr(p,"lvalue addstring");
      return;
   }

   if (dil_type_check("addstring", p, 1,
		  &v2, FAIL_NULL, 1, DILV_SP) < 0)
      return;

   if (v2.val.ptr)
     ((cNamelist *) v1.ref)->AppendName(skip_spaces((char *) v2.val.ptr));
}

/* Substract element from stringlist */
void dilfi_sul(register struct dilprg *p, register class dilval *v)
{
   class dilval v1, v2;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"substring");
      return;
   }

   eval_dil_exp(p, &v1);	/* string list */
   eval_dil_exp(p, &v2);	/* string */

   p->waitcmd--;

   switch (v1.type)
   {
     case DILV_FAIL:
      return;

     case DILV_SLPR:
      break;

     default:
      /* ERROR not right lvalue */
      dil_typeerr(p,"lvalue substring");
      return;
   }


   if (dil_type_check("substring", p, 1,
		      &v2, TYPEFAIL_NULL, 1, DILV_SP) < 0)
      return;

   if (v2.val.ptr)
     ((cNamelist *) v1.ref)->RemoveName(skip_spaces((char *) v2.val.ptr));
}

/* add element to extra description */
void dilfi_ade(register struct dilprg *p, register class dilval *v)
{
   /* add entry to extradescription */
   class dilval v1, v2, v3;

   /* three arguments */
   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"addextra");
      return;
   }

   eval_dil_exp(p, &v1);	/* extra ptr */
   eval_dil_exp(p, &v2);	/* slp */
   eval_dil_exp(p, &v3);	/* sp  */

   p->waitcmd--;

   switch (v1.type)
   {
     case DILV_FAIL:
     case DILV_EDP:
      return;

     case DILV_EDPR:
      break;

     default:
      /* ERROR not right lvalue */
      dil_typeerr(p,"lvalue addextra");
      return;
   }

   if (dil_type_check("addextra", p, 2,
		  &v2, TYPEFAIL_NULL, 1, DILV_SLP,
		  &v3, TYPEFAIL_NULL, 1, DILV_SP) < 0)
      return;

   if (v1.ref && v2.val.ptr && v3.val.ptr)
   {
      *((class extra_descr_data **) v1.ref) =
	(*((class extra_descr_data **) v1.ref))->
	  add((char *) NULL, (char *) v3.val.ptr);
      (*((class extra_descr_data **) v1.ref))->
	names.CopyList((cNamelist *) v2.val.ptr);
   }
}

/* Substract elemnt from extra description */
void dilfi_sue(register struct dilprg *p, register class dilval *v)
{
   class dilval v1, v2;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"subextra");
      return;
   }

   eval_dil_exp(p, &v1);	/* edp lvalue */
   eval_dil_exp(p, &v2);	/* sp */

   p->waitcmd--;

   switch (v1.type)
   {
     case DILV_FAIL:
     case DILV_EDP:
      return;

     case DILV_EDPR:
      break;

     default:
      /* ERROR not right lvalue */
      dil_typeerr(p,"lvalue subextra");
      return;
   }

   if (dil_type_check("subextra", p, 1,
		  &v2, TYPEFAIL_NULL, 1, DILV_SP) < 0)
     return;

   if (v2.val.ptr && v1.ref)
     *((class extra_descr_data **) v1.ref) =
       (*((class extra_descr_data **) v1.ref))->remove((char *) v2.val.ptr);
}

/* Destroy unit */
void dilfi_dst(register struct dilprg *p, register class dilval *v)
{
   class dilval v1;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"destroy");
      return;
   }

   eval_dil_exp(p, &v1);	/* unit */

   p->waitcmd--;

   if (dil_type_check("destroy", p, 1,
		  &v1, TYPEFAIL_NULL, 1, DILV_UP) < 0)
     return;

   if (v1.val.ptr && !IS_ROOM((struct unit_data *) v1.val.ptr))
   {
      if (v1.val.ptr == p->sarg->owner)
      {
	 extract_unit((struct unit_data *) v1.val.ptr);
	 p->waitcmd = WAITCMD_DESTROYED;
      }
      else
      {
	 extract_unit((struct unit_data *) v1.val.ptr);
	 dil_test_secure(p);
      }
   }
}

/* Make NPC walk to room */
void dilfi_walk(register struct dilprg *p, register class dilval *v)
{
   class dilval v1;
   ubit16 i;
   ubit8 *oldpc;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"walk");
      return;
   }

   oldpc = p->sp->pc - 1;

   eval_dil_exp(p, &v1);	/* room */

   p->waitcmd = WAITCMD_FINISH;

   if (dil_type_check("walkto", p, 1,
		  &v1, FAIL_NULL, 1, DILV_UP) < 0)
     return;

   if (v1.val.ptr)
   {
      v1.val.ptr = unit_room((struct unit_data *) v1.val.ptr);

      /* Walks happens ONLY on TICKS! */
      REMOVE_BIT(p->sarg->fptr->flags, SFB_CMD|SFB_TICK|SFB_DEAD|SFB_DONE|
		 SFB_PRE|SFB_COM|SFB_MSG|SFB_SAVE|SFB_ACTIVATE);

      SET_BIT(p->sarg->fptr->flags, SFB_TICK);

      i = npc_move(p->sarg->owner, (struct unit_data *) v1.val.ptr);

      switch (i)
      {
	case MOVE_GOAL:
	case MOVE_FAILED:
	 break;			/* just continue */

	case MOVE_DEAD:
	 p->waitcmd = WAITCMD_DESTROYED;
	 break;

	default:
	 /* still moving */
	 p->sp->pc = oldpc; /* rewind pc to just before command */
	 break;
      }
      dil_test_secure(p);
   }
}

/* Execute command */
void dilfi_exec(register struct dilprg *p, register class dilval *v)
{
   class dilval v1, v2;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"exec");
      return;
   }

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   p->waitcmd--;

   if (dil_type_check("exec", p, 2,
		  &v1, TYPEFAIL_NULL, 1, DILV_SP,
		  &v2, TYPEFAIL_NULL, 1, DILV_UP) < 0)
     return;

   if (v1.val.ptr && v2.val.ptr && IS_CHAR((struct unit_data *) v2.val.ptr))
   {
      char cmd[MAX_INPUT_LENGTH + 1];

      strncpy(cmd, (char *) v1.val.ptr, MAX_INPUT_LENGTH);
      cmd[MAX_INPUT_LENGTH] = 0;

      if (strlen((char *) v1.val.ptr) > MAX_INPUT_LENGTH)
      {
	 slog(LOG_ALL, 0, "DIL %s@%s issued command which was too long: %s",
	      UNIT_FI_NAME(p->sarg->owner),
	      UNIT_FI_ZONENAME(p->sarg->owner), cmd);
      }

      if (IS_IMMORTAL((struct unit_data *) v2.val.ptr))
      {
	 char buf[MAX_INPUT_LENGTH];
	 struct command_info *cmd_ptr;

	 extern struct trie_type *intr_trie;

	 str_next_word(cmd, buf);

	 if ((cmd_ptr = (struct command_info *) search_trie(buf, intr_trie)))
	 {
	    if (cmd_ptr->minimum_level >= IMMORTAL_LEVEL)
	    {
	       slog(LOG_EXTENSIVE, 0, "DIL %s on %s tried "
		    "to make %s do: %s",
		    p->sp->tmpl->prgname,
		    STR(UNIT_NAME(p->sarg->owner)),
		    UNIT_NAME((struct unit_data *) v2.val.ptr),
		    cmd);
	    }
	    else
	    {
	       command_interpreter((struct unit_data *) v2.val.ptr, cmd);
	       dil_test_secure(p);
	    }
	 }
	 else
	 {
	    command_interpreter((struct unit_data *) v2.val.ptr, cmd);
	    dil_test_secure(p);
	 }
      }
      else
      {
	 command_interpreter((struct unit_data *) v2.val.ptr, cmd);
	 dil_test_secure(p);
      }
   }
}

/* Wait */
void dilfi_wit(register struct dilprg *p, register class dilval *v)
{
   class dilval v1, v2;
   ubit8 *oldpc;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"wait");
      return;
   }

   oldpc = p->sp->pc - 1;

   eval_dil_exp(p, &v1);	/* bits */
   eval_dil_exp(p, &v2);	/* boolean expr */

   if (dil_type_check("wait", p, 1,
		  &v1, TYPEFAIL_NULL, 1, DILV_INT) < 0)
     return;

   if (p->waitcmd != WAITCMD_MAXINST)
   {
      REMOVE_BIT(v1.val.num,
		 ~(SFB_DONE|SFB_PRE|SFB_CMD|SFB_TICK|SFB_RANTIME|
		   SFB_DEAD|SFB_COM|SFB_MSG));

      REMOVE_BIT(p->sarg->fptr->flags, SFB_CMD|SFB_TICK|SFB_DEAD|SFB_DONE|
		 SFB_PRE|SFB_COM|SFB_MSG|SFB_SAVE|SFB_ACTIVATE);

      SET_BIT(p->sarg->fptr->flags, v1.val.num);

      p->waitcmd = WAITCMD_FINISH;
      p->sp->pc = oldpc; /* rewind pc to just before wait command */
   }
   else
   {
      if (IS_SET(v1.val.num, p->sarg->mflags) && dil_getbool(&v2))
	p->waitcmd--;
      else
      {
	 p->sp->pc = oldpc; /* rewind pc to just before command */
	 p->waitcmd = WAITCMD_FINISH;
      }
   }
}

/* Act call */
void dilfi_act(register struct dilprg *p, register class dilval *v)
{
   /* Act call */

   class dilval v1, v2, v3, v4, v5, v6;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"act");
      return;
   }

   /* 6 arguments */
   eval_dil_exp(p, &v1);	/* format string */
   eval_dil_exp(p, &v2);	/* visuality */
   eval_dil_exp(p, &v3);	/* arg 1 */
   eval_dil_exp(p, &v4);	/* arg 2 */
   eval_dil_exp(p, &v5);	/* arg 3 */
   eval_dil_exp(p, &v6);	/* to whom */

   p->waitcmd--;

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
	act((char *) v1.val.ptr, v2.val.num, v3.val.ptr, v4.val.ptr, 
	    v5.val.ptr, v6.val.num);
      break;

     case TO_VICT:
     case TO_NOTVICT:
      if (v5.val.ptr)
	act((char *) v1.val.ptr, v2.val.num, v3.val.ptr, v4.val.ptr, 
	    v5.val.ptr, v6.val.num);
   }
}

/* Goto new command */
void dilfi_goto(register struct dilprg *p, register class dilval *v)
{
   ubit32 adr;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"goto");
      return;
   }

   adr=bread_ubit32(&(p->sp->pc));
   p->sp->pc = &(p->sp->tmpl->core[adr]);
   p->waitcmd--;
}

/* Goto new command */
void dilfi_on(register struct dilprg *p, register class dilval *v)
{
   class dilval v1;
   ubit32 adr;
   ubit16 maxlab;
   ubit8 *brkptr;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"on");
      return;
   }

   eval_dil_exp(p, &v1);
   p->waitcmd--;

   maxlab=bread_ubit16(&(p->sp->pc));
   brkptr = p->sp->pc + sizeof(ubit32)*(maxlab);

   if (dil_getval(&v1)!=DILV_INT) {
      p->sp->pc = brkptr;
      return;
   }

   if ((v1.val.num>=maxlab)||(v1.val.num<0)) 
   {
      p->sp->pc = brkptr;
   }
   else
   {
      p->sp->pc += sizeof(ubit32)*(v1.val.num);
      adr = bread_ubit32(&(p->sp->pc));
      if (adr==SKIP)
	p->sp->pc = brkptr;
      else
	p->sp->pc = &(p->sp->tmpl->core[adr]);
   }
}

/* Substract affect from unit */
void dilfi_sua(register struct dilprg *p, register class dilval *v)
{
   class dilval v1, v2;
   struct unit_affected_type *af;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"subaff");
      return;
   }

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   p->waitcmd--;

   if (dil_type_check("subaff", p, 2,
		  &v1, TYPEFAIL_NULL, 1, DILV_UP,
		  &v2, TYPEFAIL_NULL, 1, DILV_INT) < 0)
     return;

   if (v1.val.ptr)
   {
      af = affected_by_spell((struct unit_data *) v1.val.ptr, v2.val.num);
      if (af)
	destroy_affect(af);
   }
}

/* Add affect */
void dilfi_ada(register struct dilprg *p, register class dilval *v)
{
   class dilval v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"addaff");
      return;
   }

   p->waitcmd--;

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);
   eval_dil_exp(p, &v3);
   eval_dil_exp(p, &v4);
   eval_dil_exp(p, &v5);
   eval_dil_exp(p, &v6);
   eval_dil_exp(p, &v7);
   eval_dil_exp(p, &v8);
   eval_dil_exp(p, &v9);
   eval_dil_exp(p, &v10);
   eval_dil_exp(p, &v11);

   if (dil_type_check("addaff", p, 11,
		  &v1, TYPEFAIL_NULL, 1, DILV_UP,
		  &v2, TYPEFAIL_NULL, 1, DILV_INT,
		  &v3, TYPEFAIL_NULL, 1, DILV_INT,
		  &v4, TYPEFAIL_NULL, 1, DILV_INT,
		  &v5, TYPEFAIL_NULL, 1, DILV_INT,
		  &v6, TYPEFAIL_NULL, 1, DILV_INT,
		  &v7, TYPEFAIL_NULL, 1, DILV_INT,
		  &v8, TYPEFAIL_NULL, 1, DILV_INT,
		  &v9, TYPEFAIL_NULL, 1, DILV_INT,
		  &v10, TYPEFAIL_NULL, 1, DILV_INT,
		  &v11, TYPEFAIL_NULL, 1, DILV_INT) < 0)
     return;

   if ((v1.val.ptr) && 
       is_in(v2.val.num, 1, ID_TOP_IDX) &&
       is_in(v8.val.num, TIF_NONE, TIF_MAX) &&
       is_in(v9.val.num, TIF_NONE, TIF_MAX) &&
       is_in(v10.val.num, TIF_NONE, TIF_MAX) &&
       is_in(v11.val.num, APF_NONE, APF_MAX))
   {
      if (p->stack[0].tmpl->zone->access != 0)
      {
	 szonelog(p->stack->tmpl->zone,
		  "DIL '%s' attempt to violate system access security (ada).",
		  p->stack->tmpl->prgname);
	 p->waitcmd = WAITCMD_QUIT;
      }
      else
      {
	 struct unit_affected_type af;

	 af.id       = v2.val.num;
	 af.duration = v3.val.num;
	 af.beat     = v4.val.num;
      
	 af.data[0]  = v5.val.num;
	 af.data[1]  = v6.val.num;
	 af.data[2]  = v7.val.num;

	 af.firstf_i = v8.val.num;
	 af.tickf_i  = v9.val.num;
	 af.lastf_i  = v10.val.num;
	 af.applyf_i = v11.val.num;
	 create_affect((struct unit_data *) v1.val.num, &af);
      }
   }
}

/* Priority */
void dilfi_pri(register struct dilprg *p, register class dilval *v)
{
   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"priority");
      return;
   }

   p->waitcmd--;
   SET_BIT(p->sarg->fptr->flags, SFB_PRIORITY);
}

/* No Priority */
void dilfi_npr(register struct dilprg *p, register class dilval *v)
{
   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"nopriority");
      return;
   }

   p->waitcmd--;
   REMOVE_BIT(p->sarg->fptr->flags, SFB_PRIORITY);
}

/* Send message to DIL programs in local environment */
void dilfi_snd(register struct dilprg *p, register class dilval *v)
{
   class dilval v1;		/* message */

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"send");
      return;
   }

   eval_dil_exp(p, &v1);

   p->waitcmd--;

   if (dil_type_check("send", p, 1,
		  &v1, TYPEFAIL_NULL, 1, DILV_SP) < 0)
     return;

   if (v1.val.ptr)
   {
      send_message(p->sarg->owner, (char *) v1.val.ptr);
      dil_test_secure(p);
   }
}

/* Send message to DIL programs in the specified unit */
void dilfi_snt(register struct dilprg *p, register class dilval *v)
{
   /* sendto (string, uptr) */
   class dilval v1, v2;

   extern struct command_info cmd_auto_msg;

   if (v)
   {
      v->type=DILV_ERR; /* instructions do not return values */
      dil_insterr(p,"sendto");
      return;
   }

   eval_dil_exp(p, &v1); /* string */
   eval_dil_exp(p, &v2); /* unit */

   p->waitcmd--;

   if (dil_type_check("sendto", p, 2,
		  &v1, TYPEFAIL_NULL, 1, DILV_SP,
		  &v2, FAIL_NULL, 1, DILV_UP) < 0)
     return;

   if (v1.val.ptr && v2.val.ptr)
   {
      struct spec_arg sarg;

      sarg.activator = p->sarg->owner;
      sarg.medium    = NULL;
      sarg.target    = NULL;
      sarg.pInt      = NULL;
      sarg.fptr      = NULL; /* Set by unit_function_scan */
      sarg.cmd       = &cmd_auto_msg;
      sarg.arg       = (char *) v1.val.ptr;
      sarg.mflags    = SFB_MSG | SFB_AWARE;

      unit_function_scan((struct unit_data *) v2.val.ptr, &sarg);

      dil_test_secure(p);
   }
}


void dilfi_snta(register struct dilprg *p, register class dilval *v)
{
   /* Send message to DIL programs in all units of type specified */
   /* sendtoall (string, string) */

   class dilval v1, v2;

   extern struct command_info cmd_auto_msg;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"sendtoall");
      return;
   }

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   p->waitcmd--;

   if (dil_type_check("sendtoall", p, 2,
		  &v1, TYPEFAIL_NULL, 1, DILV_SP,
		  &v2, TYPEFAIL_NULL, 1, DILV_SP) < 0)
     return;

   if (v1.val.ptr && v2.val.ptr)
   {
      extern struct unit_data *unit_list;

      struct unit_data *u;
      struct file_index_type *fi;

      if ((fi = str_to_file_index((char *) v2.val.ptr)))
      {
	 struct spec_arg sarg;

	 sarg.activator = p->sarg->owner;
	 sarg.medium    = p->sarg->owner;
	 sarg.target    = NULL;
	 sarg.pInt      = NULL;
	 sarg.fptr      = NULL; /* Set by unit_function_scan */

	 sarg.fptr      = NULL;
	 sarg.cmd       = &cmd_auto_msg;
	 sarg.arg       = (char *) v1.val.ptr;
	 sarg.mflags    = SFB_MSG;

	 for (u = unit_list; u; u = u->gnext)
	   if (UNIT_FILE_INDEX(u) == fi)
	     unit_function_scan(u, &sarg);

	 dil_test_secure(p);
      }
   }
}


void dilfi_sntadil(register struct dilprg *p, register class dilval *v)
{
   /* Send message to DIL programs in all units of type specified */
   /* sendtoall (string<message>, string<template> ) */

   class dilval v1, v2;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"sendtoalldil");
      return;
   }

   eval_dil_exp(p, &v1);
   eval_dil_exp(p, &v2);

   p->waitcmd--;

   if (dil_type_check("sendtoalldil", p, 2,
		  &v1, TYPEFAIL_NULL, 1, DILV_SP,
		  &v2, TYPEFAIL_NULL, 1, DILV_SP) < 0)
     return;

   if (v1.val.ptr && !str_is_empty((char *) v2.val.ptr))
   {
      struct diltemplate *tmpl;

      tmpl=find_dil_template((char *) v2.val.ptr);

      if (tmpl)
      {
	 struct dilprg *tp;

	 for (tp = dil_list; tp; tp = dil_list_nextdude)
	 {
	    dil_list_nextdude = tp->next;

	    if (tp->sp && tp->sp->tmpl == tmpl && tp != p)
	    {
	       struct unit_fptr *fptr;

	       /* If it is destructed, then it cant be found because data
		  will be null */

	       for (fptr = UNIT_FUNC(tp->owner); fptr; fptr = fptr->next)
		 if (fptr->index == SFUN_DIL_INTERNAL && fptr->data &&
		     ((struct dilprg *) fptr->data)->sp &&
		     ((struct dilprg *) fptr->data)->sp->tmpl == tmpl)
		   break;

	       if (!fptr)
		 continue;

	       struct spec_arg sarg;

	       sarg.owner     = tp->owner;
	       sarg.activator = p->owner;
	       sarg.medium    = p->owner;
	       sarg.target    = tp->owner;
	       sarg.pInt      = NULL;
	       sarg.fptr      = fptr;

	       sarg.cmd       = &cmd_auto_msg;
	       sarg.arg       = (char *) v1.val.ptr;
	       sarg.mflags    = SFB_MSG;

	       function_activate(tp->owner, &sarg);
	    }
	 } /* for */
	 dil_test_secure(p);
      }
   }
}


void dilfi_log(register struct dilprg *p, register class dilval *v)
{
   /* Log a string to the logs. */

   class dilval v1;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"log");
      return;
   }

   eval_dil_exp(p, &v1);

   p->waitcmd--;

   if (dil_type_check("log", p, 1,
		  &v1, TYPEFAIL_NULL, 1, DILV_SP) < 0)
     return;

   if (v1.val.ptr)
     szonelog(UNIT_FI_ZONE(p->owner), "%s", (char *) v1.val.ptr);
}


/* Secure */
void dilfi_sec(register struct dilprg *p, register class dilval *v)
{
   class dilval v1;		/* unit */
   ubit32 adr;			/* address */

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"secure");
      return;
   }

   eval_dil_exp(p, &v1);
   adr = bread_ubit32(&(p->sp->pc));

   p->waitcmd--;

   if (dil_type_check("secure", p, 1,
		  &v1, TYPEFAIL_NULL, 1, DILV_UP) < 0)
     return;

   if (v1.val.ptr)
   {
      dil_sub_secure(p->sp, (struct unit_data *) v1.val.ptr);
      dil_add_secure(p, (struct unit_data *) v1.val.ptr, &(p->sp->tmpl->core[adr]));
   }
}

/* Unsecure */
void dilfi_use(register struct dilprg *p, register class dilval *v)
{
   class dilval v1;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"unsecure");
      return;
   }

   eval_dil_exp(p, &v1);

   p->waitcmd--;

   if (dil_type_check("unsecure", p, 1,
		  &v1, TYPEFAIL_NULL, 1, DILV_UP) < 0)
     return;

   if (v1.val.ptr)
     dil_sub_secure(p->sp, (struct unit_data *) v1.val.ptr);
}


/* Equip unit in inventory of PC/NPC */
void dilfi_eqp(register struct dilprg *p, register class dilval *v)
{
   class dilval v1, v2;

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"equip");
      return;
   }

   eval_dil_exp(p, &v1);	/* unit */
   eval_dil_exp(p, &v2);	/* position */

   p->waitcmd--;

   if (dil_type_check("equip", p, 2,
		  &v1, TYPEFAIL_NULL, 1, DILV_UP,
		  &v2, TYPEFAIL_NULL, 1, DILV_INT) < 0)
     return;

   if (v1.val.ptr && UNIT_IN((struct unit_data *) v1.val.ptr) &&
       IS_CHAR(UNIT_IN((struct unit_data *) v1.val.ptr)) &&
       IS_OBJ((struct unit_data *) v1.val.ptr) &&
       ! equipment(UNIT_IN((struct unit_data *) v1.val.ptr), v2.val.num))
   {
      /* Then equip char */
      equip_char(UNIT_IN((struct unit_data *) v1.val.ptr),
		 (struct unit_data *) v1.val.ptr, v2.val.num);
   }
}

/* Unequip unit in inventory of PC/NPC */
void dilfi_ueq(register struct dilprg *p, register class dilval *v)
{
   class dilval v1;

   if (v)
   {
      v->type=DILV_ERR; /* instructions do not return values */
      dil_insterr(p,"unequip");
      return;
   }

   eval_dil_exp(p, &v1); /* unit */

   p->waitcmd--;

   if (dil_type_check("unequip", p, 1,
		      &v1, FAIL_NULL, 1, DILV_UP) < 0)
     return;

   if (v1.val.ptr && IS_OBJ((struct unit_data *) v1.val.ptr) &&
       OBJ_EQP_POS((struct unit_data *) v1.val.ptr))
   {
      unequip_object((struct unit_data *) v1.val.ptr);
   }
}


void dilfi_quit(register struct dilprg *p, register class dilval *v)
{
   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"quit");
      return;
   }

   p->waitcmd = WAITCMD_QUIT;
}

/* Block */
void dilfi_blk(register struct dilprg *p, register class dilval *v)
{
   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"block");
      return;
   }

   p->waitcmd--;
   SET_BIT(p->flags,DILFL_CMDBLOCK);
}

void dilfi_pup(register struct dilprg *p, register class dilval *v)
{
   class dilval v1;

   void update_pos( struct unit_data *victim );
   void die(struct unit_data *ch);

   if (v)
   {
      v->type=DILV_ERR; /* instructions do not return values */
      dil_insterr(p,"updatepos");
      return;
   }

   eval_dil_exp(p, &v1);

   p->waitcmd--;

   if (dil_type_check("updatepos", p, 1,
		  &v1, TYPEFAIL_NULL, 1, DILV_UP) < 0)
     return;

   if (v1.val.ptr && IS_CHAR((struct unit_data *) v1.val.ptr))
   {
      update_pos((struct unit_data *) v1.val.ptr);
      if (CHAR_POS((struct unit_data *)v1.val.ptr) == POSITION_DEAD)
      {
	 die((struct unit_data *) v1.val.ptr);
	 dil_test_secure(p);
      }
   }
}

void dilfi_cast(register struct dilprg *p, register class dilval *v)
{
   class dilval v1, v2, v3, v4;
   struct unit_data *caster = NULL, *medium = NULL, *target = NULL;
   extern struct spell_info_type spell_info[SPL_TREE_MAX];

   if (v)
   {
      v->type=DILV_ERR;		/* instructions do not return values */
      dil_insterr(p,"cast");
      return;
   }

   eval_dil_exp(p, &v1);	/* spell number */
   eval_dil_exp(p, &v2);	/* caster */
   eval_dil_exp(p, &v3);	/* medium */
   eval_dil_exp(p, &v4);	/* target */

   p->waitcmd--;

   if (dil_type_check("cast", p, 4,
		  &v1, TYPEFAIL_NULL, 1, DILV_INT,
		  &v2, TYPEFAIL_NULL, 1, DILV_UP,
		  &v3, TYPEFAIL_NULL, 1, DILV_UP,
		  &v4, TYPEFAIL_NULL, 1, DILV_UP) < 0)
     return;

   caster = (struct unit_data *) v2.val.ptr;
   medium = (struct unit_data *) v3.val.ptr;
   target = (struct unit_data *) v4.val.ptr;

   if (is_in(v1.val.num, SPL_GROUP_MAX, SPL_TREE_MAX-1) &&
       caster && IS_CHAR(caster) && medium &&
       (spell_info[v1.val.num].spell_pointer ||
	spell_info[v1.val.num].tmpl))
   {
      /* cast the spell */
      char mbuf[MAX_INPUT_LENGTH] = {0};
      spell_perform(v1.val.num, MEDIA_SPELL,
		    caster, medium, target, mbuf);
      dil_test_secure(p);
   }
}
