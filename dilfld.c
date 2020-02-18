/* *********************************************************************** *
 * File   : dilfld.c                                  Part of Valhalla MUD *
 * Version: 2.00                                                           *
 * Author : bombman@diku.dk & seifert@diku.dk				   *
 *                                                                         *
 *                                                                         *
 * Purpose: DIL Fields.			       	                           *
 *									   *
 * Bugs   : self.extra := activator.extra makes it crash.                  *
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

#include "structs.h"
#include "utils.h"
#include "dil.h"
#include "dilexp.h"
#include "dilrun.h"
#include "textutil.h"
#include "db_file.h"
#include "handler.h"
#include "interpreter.h"
#include "utility.h"
#include "common.h"
#include "main.h"
#include "limits.h"
#include "config.h"


void dilfe_fld(register struct dilprg *p, register class dilval *v)
{
   /* Get a structure field */
   class dilval v1, v2;
   int fldno;

   fldno = bread_ubit8(&(p->sp->pc));
   eval_dil_exp(p, &v1);	/* what to get index from */
   v2.type = DILV_NULL;

   switch (fldno)
   {
      /* *********************************** */
      /* . [ index ]				*/
     case DILF_IDX:
      eval_dil_exp(p, &v2);	/* evaluate index */

      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;
	 break;
	case DILV_SLP:
	 if (v1.val.ptr)
	   v->type = DILV_SP;
	 else
	   v->type = DILV_FAIL; /* NULL list */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }

      switch (dil_getval(&v2))
      {
	case DILV_FAIL:
	 if (v->type != DILV_ERR)
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	case DILV_INT:
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
      }

      if (v->type == DILV_SP)
      {
	 v->atyp = DILA_EXP;

	 if (((ubit32) v2.val.num <
	      ((class cNamelist *) v1.val.ptr)->Length()) &&
	      (v2.val.num >= 0))
	 {
	    v->atyp = DILA_NONE;  // Dont dealloc!
	    v->type = DILV_HASHSTR;
	    v->ref  = ((cNamelist *) v1.val.ptr)->InstanceName(v2.val.num);
	 }
	 else
	   v->type = DILV_FAIL; /* illegal index */
      }
      break;

      /* *********************************** */
      /* .idx   			     */
     case DILF_SID:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    v->atyp = DILA_NORM;
	    v->type = DILV_INT;
            if (IS_PC((struct unit_data *) v1.val.ptr))
	      v->val.num = (int64_t) PC_ID((struct unit_data *) v1.val.ptr);
	    else
	      v->val.num = (int64_t) UNIT_FILE_INDEX((struct unit_data *)
						     v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .speed   			     */
     case DILF_SPD:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if ((v1.val.ptr && IS_CHAR((struct unit_data *) v1.val.ptr)))
	 {
	    v->atyp = DILA_NORM;
	    v->type = DILV_INT;
	    v->val.num = CHAR_SPEED((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .acc_total  			     */
     case DILF_ATOT:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if ((v1.val.ptr && IS_PC((struct unit_data *) v1.val.ptr)))
	 {
	    v->atyp = DILA_NORM;
	    v->type = DILV_INT;
	    if (g_cServerConfig.m_bAccounting)
	      v->val.num = PC_ACCOUNT((struct unit_data *) v1.val.ptr).
		total_credit;
	    else
	      v->val.num = 0;
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .acc_balance  			     */
     case DILF_ABAL:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if ((v1.val.ptr && IS_PC((struct unit_data *) v1.val.ptr)))
	 {
	    v->atyp = DILA_NORM;
	    v->type = DILV_INT;
	    if (g_cServerConfig.m_bAccounting)
	      v->val.num = (int) PC_ACCOUNT((struct unit_data *) v1.val.ptr).credit;
	    else
	      v->val.num = 0;
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .zoneidx				*/
     case DILF_ZOI:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    v->atyp = DILA_NORM;
	    v->type = DILV_SP;
	    v->val.ptr = (void *) UNIT_FI_ZONENAME((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .nameidx				*/
     case DILF_NMI:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    v->atyp = DILA_NORM;
	    v->type = DILV_SP;
	    v->val.ptr = (void *) UNIT_FI_NAME((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .type					*/
     case DILF_TYP:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if(v1.val.ptr)
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_INT;
	    v->val.num = UNIT_TYPE((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .next				       */
     case DILF_NXT:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_EDP:
	 v->atyp = DILA_NORM;
	 v->type = DILV_EDP;
	 if (v1.val.ptr)
	   v->val.ptr = ((struct extra_descr_data *) v1.val.ptr)->next;
	 else
	   v->val.ptr = NULL;
	 break;
	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    v->atyp = DILA_NORM;
	    v->type = DILV_UP;
	    v->val.ptr = ((struct unit_data *) v1.val.ptr)->next;
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .names				     */
     case DILF_NMS:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_EDP:
	 if (v1.val.ptr)
	 {
	    v->atyp = DILA_NORM;
	    v->type = DILV_SLPR;
	    v->ref  = &(((struct extra_descr_data *) v1.val.ptr)->names);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    v->atyp = DILA_NORM;
	    v->type = DILV_SLPR;
	    v->ref  = &UNIT_NAMES((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .name                               */
     case DILF_NAM:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;

	case DILV_EDP:
	 if (v1.val.ptr)
	 {
	    v->atyp = DILA_NORM;
	    v->type = DILV_SP;
	    v->val.ptr = (void *)
	      IF_STR(((struct extra_descr_data *) v1.val.ptr)->names.Name());
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;

	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    v->atyp = DILA_NORM;
	    v->type = DILV_SP;
	    v->val.ptr = (void *)
	      IF_STR(UNIT_NAME((struct unit_data *) v1.val.ptr));
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .descr (extra's)	             */
     case DILF_DES:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;

	case DILV_EDP:
	 if (v1.val.ptr)
	 {
	    v->atyp = DILA_NONE; // Dont dealloc!
	    v->type = DILV_HASHSTR;
	    v->ref = &(((struct extra_descr_data *) v1.val.ptr)->descr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .outside_descr		             */
     case DILF_ODES:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;

	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    v->atyp = DILA_NONE;  // Dont dealloc!
	    v->type = DILV_HASHSTR;
	    v->ref  = &UNIT_OUT_DESCR((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .inside_descr		             */
     case DILF_IDES:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;

	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    v->atyp = DILA_NONE;  // Dont dealloc!
	    v->type = DILV_HASHSTR;
	    v->ref  = &UNIT_IN_DESCR((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .title				     */
     case DILF_TIT:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    v->atyp = DILA_NONE;  // Dont dealloc!
	    v->type = DILV_HASHSTR;
	    v->ref  = &UNIT_TITLE((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .hometown			     */
     case DILF_HOME:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_PC((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NORM;
	    v->type = DILV_SPR;
	    v->ref = &PC_HOME((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .extra				     */
     case DILF_EXT:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    v->atyp = DILA_NORM;
	    v->type = DILV_EDPR;
	    v->ref  = &UNIT_EXTRA_DESCR((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .outside			       */
     case DILF_OUT:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    v->atyp = DILA_NORM;
	    v->type = DILV_UP;
	    v->val.ptr = UNIT_IN((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .inside			     */
     case DILF_INS:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    v->atyp = DILA_NORM;
	    v->type = DILV_UP;
	    v->val.ptr = UNIT_CONTAINS((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .gnext				     */
     case DILF_GNX:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    v->atyp = DILA_NORM;
	    v->type = DILV_UP;
	    v->val.ptr = ((struct unit_data *) v1.val.ptr)->gnext;
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .loadcount			     */
     case DILF_LCN:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    v->atyp = DILA_NORM;
	    v->type = DILV_INT;
	    v->val.num = UNIT_FILE_INDEX((struct unit_data *) v1.val.ptr)?
	      UNIT_FILE_INDEX((struct unit_data *) v1.val.ptr)->no_in_mem:
	      1;
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .gprevious			     */
     case DILF_GPR:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    v->atyp = DILA_NORM;
	    v->type = DILV_UP;
	    v->val.ptr = ((struct unit_data *) v1.val.ptr)->gprevious;
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .light				     */
     case DILF_LGT:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_INT;
	    v->val.num = UNIT_LIGHTS((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
	 break;
      }
      break;

      /* *********************************** */
      /* .bright			     */
     case DILF_BGT:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_INT;
	    v->val.num = UNIT_BRIGHT((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .illum				     */
     case DILF_ILL:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_INT;
	    v->val.num = UNIT_ILLUM((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .flags				     */
     case DILF_FL:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    /* ubit16 flags; */
	    v->atyp = DILA_NONE;
	    v->type = DILV_UINT2R;
	    v->ref = &UNIT_FLAGS((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .manipulate			     */
     case DILF_MAN:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_UINT4R;
	    v->ref = &UNIT_MANIPULATE((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .openflags			     */
     case DILF_OFL:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    /* ubit8 openflags */
	    v->atyp = DILA_NONE;
	    v->type = DILV_UINT1R;
	    v->ref = &UNIT_OPEN_FLAGS((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .lifespan			     */
     case DILF_LSA:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_PC((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp    = DILA_NONE;

	    if (p->stack[0].tmpl->zone->access != 0)
	    {
	       v->type    = DILV_INT;
	       v->val.num = PC_LIFESPAN((struct unit_data *) v1.val.ptr);
	    }
	    else
	    {
	       v->type    = DILV_UINT2R;
	       v->ref     = &PC_LIFESPAN((struct unit_data *) v1.val.ptr);
	    }
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .max_mana			     */
     case DILF_MMA:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_CHAR((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp    = DILA_NONE;
	    v->type    = DILV_INT;
	    v->val.num = mana_limit((struct unit_data *)v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .max_endurance			     */
     case DILF_MED:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_CHAR((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp    = DILA_NONE;
	    v->type    = DILV_INT;
	    v->val.num = move_limit((struct unit_data *)v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .max_hp			     */
     case DILF_MHP:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    v->atyp    = DILA_NONE;

	    if (IS_PC((struct unit_data *) v1.val.ptr))
	    {
	       v->type    = DILV_INT;
	       v->val.num = UNIT_MAX_HIT((struct unit_data *)v1.val.ptr);
	    }
	    else
	    {
	       v->type = DILV_SINT4R;
	       v->ref = &UNIT_MAX_HIT((struct unit_data *)v1.val.ptr);
	    }
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .hp				     */
     case DILF_CHP:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    /* sbit32 hp; */
	    v->atyp = DILA_NONE;
	    v->type = DILV_SINT4R;
	    v->ref = &UNIT_HIT((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .baseweight			     */
     case DILF_BWT:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_INT;
	    v->val.num = UNIT_BASE_WEIGHT((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .weight			     */
     case DILF_WGT:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_INT;
	    v->val.num = UNIT_WEIGHT((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .capacity			     */
     case DILF_CAP:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    /* sbit16 capacity */
	    v->atyp = DILA_NONE;
	    v->type = DILV_SINT2R;
	    v->ref = &UNIT_CAPACITY((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .alignment			    */
     case DILF_ALG:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    /* sbit16 capacity */
	    v->atyp = DILA_NONE;
	    v->type = DILV_SINT2R;
	    v->ref = &UNIT_ALIGNMENT((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .spells [ index ]		     */
     case DILF_SPL:
      eval_dil_exp(p, &v2);	/* evaluate index */
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 v->type = DILV_INT;
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }

      switch (dil_getval(&v2))
      {
	case DILV_FAIL:
	 if (v->type != DILV_ERR)
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	case DILV_INT:
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }

      if (v->type == DILV_INT)
      {
	 if (v1.val.ptr)
	 {
	    switch (UNIT_TYPE((struct unit_data *) v1.val.ptr))
	    {
	      case UNIT_ST_ROOM:
	       v->atyp = DILA_NONE;
	       v->type = DILV_UINT1R;
	       v->ref = &ROOM_RESISTANCE((struct unit_data *) v1.val.ptr);
	       break;

	      case UNIT_ST_OBJ:
	       v->atyp = DILA_NONE;
	       v->type = DILV_UINT1R;
	       v->ref = &OBJ_RESISTANCE((struct unit_data *) v1.val.ptr);
	       break;

	      case UNIT_ST_NPC:
	       if (is_in(v2.val.num, 0, SPL_GROUP_MAX-1))
	       {
		  v->atyp = DILA_NONE;
		  v->type = DILV_UINT1R;
		  v->ref = &NPC_SPL_SKILL((struct unit_data *) v1.val.ptr,
					  v2.val.num);
	       }
	       else
		 v->type = DILV_FAIL; /* illegal index */
	       break;

	      case UNIT_ST_PC:
	       if (is_in(v2.val.num, 0, SPL_TREE_MAX-1))
	       {
		  if (p->stack[0].tmpl->zone->access == 0)
		  {
		     v->atyp = DILA_NONE;
		     v->type = DILV_UINT1R;
		     v->ref  = &PC_SPL_SKILL((struct unit_data *) v1.val.ptr,
					     v2.val.num);
		  }
		  else
		  {
		     v->atyp = DILA_NONE;
		     v->type = DILV_INT;
		     v->val.num = PC_SPL_SKILL((struct unit_data *) v1.val.ptr,
					       v2.val.num);
		  }
	       }
	       else
		 v->type = DILV_FAIL; /* illegal index */
	       break;
	      default:
	       v->type = DILV_ERR; /* illegal type */
	       break;
	    }
	 }
	 else
	   v->type = DILV_FAIL; /* illegal type */
      }
      break;

      /* *********************************** */
      /* .hasfunc			   */
     case DILF_FUN:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_INT;
	    v->val.num = (UNIT_FUNC((struct unit_data *) v1.val.ptr) != NULL);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .zone				   */
     case DILF_ZON:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    v->atyp = DILA_NORM;
	    v->type = DILV_SP;
	    v->val.ptr = unit_zone((struct unit_data *) v1.val.ptr)->name;
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .objecttype			   */
     case DILF_OTY:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_OBJ((struct unit_data *) v1.val.ptr))
	 {
	    v->type = DILV_UINT1R;
	    v->atyp = DILA_NONE;
	    v->ref  = &OBJ_TYPE((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .value [ index ]		    */
     case DILF_VAL:
      eval_dil_exp(p, &v2);	/* evaluate index */
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 v->type = DILV_UINT4R;
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }

      switch (dil_getval(&v2))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 if (v->type != DILV_ERR)
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	case DILV_INT:
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }

      if (v->type == DILV_UINT4R)
      {
	 if ((v1.val.ptr) && IS_OBJ((struct unit_data *) v1.val.ptr) &&
	     (v2.val.num >= 0) && (v2.val.num < 5))
	 {
	    v->atyp = DILA_NONE;
	    v->ref = &OBJ_VALUE((struct unit_data *) v1.val.ptr, v2.val.num);
	 }
	 else
	   v->type = DILV_FAIL;
      }
      break;

      /* *********************************** */
      /* .objectflags			     */
     case DILF_EFL:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_OBJ((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_UINT1R;
	    v->ref = &OBJ_FLAGS((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .cost				   */
     case DILF_CST:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_OBJ((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_UINT4R;
	    v->ref = &OBJ_PRICE((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .rent				   */
     case DILF_RNT:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_OBJ((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_UINT4R;
	    v->ref = &OBJ_PRICE_DAY((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .equip				     */
     case DILF_EQP:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_OBJ((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_INT;
	    v->val.num = OBJ_EQP_POS((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .exit_names [ index ]		     */
     case DILF_ONM:
      eval_dil_exp(p, &v2);	/* evaluate index */

      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 v->type = DILV_SLPR;
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }

      switch (dil_getval(&v2))
      {
	case DILV_FAIL:
	 if (v->type != DILV_ERR)
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	case DILV_INT:
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }

      if (v->type == DILV_SLPR)
      {
	 if (v1.val.ptr && IS_ROOM((struct unit_data *) v1.val.ptr) &&
	     is_in(v2.val.num, 0, 5) &&
	     ROOM_EXIT((struct unit_data *) v1.val.ptr, v2.val.num))
	 {
	    v->atyp = DILA_NORM;
	    v->ref  = &(ROOM_EXIT((struct unit_data *) v1.val.ptr,
				  v2.val.num)->open_name);
	 }
	 else
	   v->type = DILV_FAIL;
      }

      break;

      /* *********************************** */
      /* .exit_info [ index ]		     */
     case DILF_XNF:
      eval_dil_exp(p, &v2);	/* evaluate index */
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 v->type = DILV_UINT1R;
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }

      switch (dil_getval(&v2))
      {
	case DILV_FAIL:
	 if (v->type != DILV_ERR)
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	case DILV_INT:
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }

      if (v->type == DILV_UINT1R)
      {
	 if (v1.val.ptr && IS_ROOM((struct unit_data *) v1.val.ptr) &&
	     is_in(v2.val.num, 0, 5) &&
	     ROOM_EXIT((struct unit_data *) v1.val.ptr, v2.val.num))
	 {
	    v->atyp = DILA_NONE;
	    v->ref = &(ROOM_EXIT((struct unit_data *) v1.val.ptr,
				 v2.val.num)->exit_info);
	 }
	 else
	   v->type = DILV_FAIL;
      }
      break;

      /* *********************************** */
      /* .exit_to [ index ]		     */
     case DILF_TOR:
      eval_dil_exp(p, &v2);
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 v->type = DILV_UP;
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      switch (dil_getval(&v2))
      {
	case DILV_FAIL:
	 if (v->type != DILV_ERR)
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	case DILV_INT:
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }

      if (v->type == DILV_UP)
      {
	 if (v1.val.ptr && IS_ROOM((struct unit_data *) v1.val.ptr) &&
	     is_in(v2.val.num, 0, 5) &&
	     ROOM_EXIT((struct unit_data *) v1.val.ptr, v2.val.num))
	 {
	    v->atyp = DILA_NORM;
	    v->val.ptr = ROOM_EXIT((struct unit_data *) v1.val.ptr,
				   v2.val.num)->to_room;
	 }
	 else
	   v->type = DILV_FAIL;
      }
      break;

      /* *********************************** */
      /* .roomflags			     */
     case DILF_RFL:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_ROOM((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_UINT1R;
	    v->ref = &ROOM_FLAGS((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .movement			     */
     case DILF_MOV:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_ROOM((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_UINT1R;
	    v->ref = &ROOM_LANDSCAPE((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .sex				     */
     case DILF_SEX:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_CHAR((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_SINT1R;
	    v->ref = &CHAR_SEX((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .race				     */
     case DILF_RCE:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_CHAR((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_UINT2R;
	    v->ref = &CHAR_RACE((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .abilities [ index ]		     */
     case DILF_ABL:
      eval_dil_exp(p, &v2);	/* evaluate index */
      switch (dil_getval(&v2))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;

	case DILV_INT:
	 v->type = DILV_INT;
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }

      switch (dil_getval(&v1))
      {
	case DILV_FAIL:
	 if (v->type != DILV_ERR)
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	case DILV_UP:
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }

      if (v->type == DILV_INT)
      {
	 if (v1.val.ptr && IS_CHAR((struct unit_data *) v1.val.ptr) &&
	     is_in(v2.val.num, 0, ABIL_TREE_MAX-1))
	 {
	    if (IS_PC((struct unit_data *) v1.val.ptr))
	    {
	       if (p->stack[0].tmpl->zone->access == 0)
	       {
		  v->atyp = DILA_NONE;
		  v->type = DILV_UINT1R;
		  v->ref  = &CHAR_ABILITY((struct unit_data *)
					  v1.val.ptr, v2.val.num);
	       }
	       else
	       {
		  v->atyp = DILA_NONE;
		  v->type = DILV_INT;
		  v->val.num = CHAR_ABILITY((struct unit_data *)
					    v1.val.ptr, v2.val.num);
	       }
	    }
	    else
	    {
	       v->atyp = DILA_NONE;
	       v->type = DILV_UINT1R;
	       v->ref = &CHAR_ABILITY((struct unit_data *)
				      v1.val.ptr, v2.val.num);
	    }
	 }
	 else
	   v->type = DILV_FAIL;
      }
      break;

      /* *********************************** */
      /* .exp				     */
     case DILF_EXP:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_CHAR((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_INT;
	    v->val.num = CHAR_EXP((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .level				     */
     case DILF_LVL:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_CHAR((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_INT;
	    v->val.num = CHAR_LEVEL((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .size 	                             */
     case DILF_HGT:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_UINT2R;
	    v->ref = &UNIT_SIZE((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .position			     */
     case DILF_POS:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;

	case DILV_UP:
	 if (v1.val.ptr && IS_CHAR((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_SINT1R;
	    v->ref = &CHAR_POS((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .attack_type			     */
     case DILF_ATY:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_CHAR((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_UINT1R;
	    v->ref = &CHAR_ATTACK_TYPE((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .mana				     */
     case DILF_MNA:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_CHAR((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_SINT2R;
	    v->ref = &CHAR_MANA((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .endurance			     */
     case DILF_END:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_CHAR((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_SINT2R;
	    v->ref  = &CHAR_ENDURANCE((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .charflags			     */
     case DILF_AFF:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_CHAR((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_UINT4R;
	    v->ref  = &CHAR_FLAGS((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .fighting			     */
     case DILF_FGT:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_CHAR((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_UP;
	    v->val.ptr = CHAR_FIGHTING((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .weapons [ index ]		     */
     case DILF_WPN:
      eval_dil_exp(p, &v2);	/* evaluate index */
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 v->type = DILV_INT;
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }

      switch (dil_getval(&v2))
      {
	case DILV_FAIL:
	 if (v->type != DILV_ERR)
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	case DILV_INT:
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }

      if (v->type == DILV_INT)
      {
	 if (v1.val.ptr)
	 {
	    switch (UNIT_TYPE((struct unit_data *) v1.val.ptr))
	    {
	      case UNIT_ST_NPC:
	       if (is_in(v2.val.num, 0, WPN_GROUP_MAX-1))
	       {
		  v->atyp = DILA_NONE;
		  v->type = DILV_UINT1R;
		  v->ref  = &NPC_WPN_SKILL((struct unit_data *)
					   v1.val.ptr, v2.val.num);
	       }
	       else
		 v->type = DILV_FAIL;
	       break;
	      case UNIT_ST_PC:
	       if (is_in(v2.val.num, 0, WPN_TREE_MAX-1))
	       {
		  if (p->stack[0].tmpl->zone->access == 0)
		  {
		     v->atyp = DILA_NONE;
		     v->type = DILV_UINT1R;
		     v->ref  = &PC_WPN_SKILL((struct unit_data *)
					     v1.val.ptr, v2.val.num);
		  }
		  else
		  {
		     v->atyp = DILA_NONE;
		     v->type = DILV_INT;
		     v->val.num = PC_WPN_SKILL((struct unit_data *)
					       v1.val.ptr, v2.val.num);
		  }
	       }
	       else
		 v->type = DILV_FAIL;
	       break;

	      default:
	       v->type = DILV_ERR; /* illegal type */
	       break;
	    }
	 }
	 else
	   v->type = DILV_FAIL; /* illegal type */
      }
      break;

      /* *********************************** */
      /* .defaultpos			     */
     case DILF_DEF:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_NPC((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_UINT1R;
	    v->ref = &NPC_DEFAULT((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .npcflags			     */
     case DILF_ACT:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_NPC((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_UINT1R;
	    v->ref = &NPC_FLAGS((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .pcflags			     */
     case DILF_PCF:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_PC((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_UINT2R;
	    v->ref = &PC_FLAGS((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .birth				     */
     case DILF_BIR:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_PC((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_UINT4R;
	    v->ref  = &PC_TIME((struct unit_data *) v1.val.ptr).birth;
	 }
	 else
	   v->type = DILV_FAIL;
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .playtime			     */
     case DILF_PTI:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_PC((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp    = DILA_NONE;
	    v->type    = DILV_INT;
	    v->val.num = PC_TIME((struct unit_data *) v1.val.ptr).played;
	 }
	 else
	   v->type = DILV_FAIL;
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .full				     */
     case DILF_FLL:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_PC((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_SINT1R;
	    v->ref = &PC_COND((struct unit_data *) v1.val.ptr, FULL);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .thirst			     */
     case DILF_THR:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_PC((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_SINT1R;
	    v->ref = &PC_COND((struct unit_data *) v1.val.ptr, THIRST);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .drunk				     */
     case DILF_DRK:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_PC((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_SINT1R;
	    v->ref = &PC_COND((struct unit_data *) v1.val.ptr, DRUNK);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .skill_points			     */
     case DILF_SPT:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_PC((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_INT;
	    v->val.num = PC_SKILL_POINTS((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .ability_points		     */
     case DILF_APT:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_PC((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_INT;
	    v->val.num = PC_ABILITY_POINTS((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .guild				     */
     case DILF_GLD:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_PC((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NORM;
	    v->type = DILV_SPR;
	    v->ref = &PC_GUILD((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .crimes			     */
     case DILF_CRM:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_PC((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_UINT2R;
	    v->ref = &PC_CRIMES((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .quests			     */
     case DILF_QST:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_PC((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NORM;
	    v->type = DILV_EDPR;
	    v->ref = &PC_QUEST((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .info           		     */
     case DILF_INFO:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_PC((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NORM;
	    v->type = DILV_EDPR;
	    v->ref = &PC_INFO((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .skills [ index ]		    */
     case DILF_SKL:
      eval_dil_exp(p, &v2);	/* evaluate index */
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 v->type = DILV_INT;
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }

      switch (dil_getval(&v2))
      {
	case DILV_FAIL:
	 if (v->type != DILV_ERR)
	   v->type = DILV_FAIL; /* not applicable */
	 break;
	case DILV_INT:
	 break;
	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }

      if (v->type == DILV_INT)
      {
	 if (v1.val.ptr && IS_PC((struct unit_data *) v1.val.ptr) &&
	     is_in(v2.val.num, 0, SKI_TREE_MAX-1))
	 {
	    if (p->stack[0].tmpl->zone->access == 0)
	    {
	       v->atyp = DILA_NONE;
	       v->type = DILV_UINT1R;
	       v->ref  = &PC_SKI_SKILL((struct unit_data *) v1.val.ptr,
				       v2.val.num);
	    }
	    else
	    {
	       v->atyp = DILA_NONE;
	       v->type = DILV_INT;
	       v->val.num = PC_SKI_SKILL((struct unit_data *) v1.val.ptr,
					 v2.val.num);
	    }
	 }
	 else
	   v->type = DILV_FAIL;
      }
      break;

      /* *********************************** */
      /* .master			     */
     case DILF_MAS:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_CHAR((struct unit_data *) v1.val.ptr))
	 {
	    v->atyp = DILA_NORM;
	    v->type = DILV_UPR;
	    v->ref = &CHAR_MASTER((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;


      /* *********************************** */
      /* .follower			     */
     case DILF_FOL:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr && IS_CHAR((struct unit_data *) v1.val.ptr))
	 {
	    if (CHAR_FOLLOWERS((struct unit_data *) v1.val.ptr))
	    {
	       v->atyp = DILA_NORM;
	       v->type = DILV_UPR;
	       v->ref = &CHAR_FOLLOWERS((struct unit_data *) v1.val.ptr)->
		 follower;
	    }
	    else
	    {
	       v->atyp = DILA_NORM;
	       v->type = DILV_NULL;
	       v->val.ptr = NULL;
	    }
	 }
	 else
	   v->type = DILV_FAIL;
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;

      /* *********************************** */
      /* .minv				     */
     case DILF_MIV:
      switch (dil_getval(&v1))
      {
	case DILV_NULL:		/* not applicable */
	case DILV_FAIL:
	 v->type = DILV_FAIL;	/* not applicable */
	 break;
	case DILV_UP:
	 if (v1.val.ptr)
	 {
	    v->atyp = DILA_NONE;
	    v->type = DILV_UINT1R;
	    v->ref = &UNIT_MINV((struct unit_data *) v1.val.ptr);
	 }
	 else
	   v->type = DILV_FAIL;
	 break;

	default:
	 v->type = DILV_ERR;	/* wrong type */
	 break;
      }
      break;


     default:
      szonelog(UNIT_FI_ZONE(p->sarg->owner),
	       "DIL %s@%s, unknown field type: %d\n",
	       UNIT_FI_NAME(p->sarg->owner), UNIT_FI_ZONENAME(p->sarg->owner),
	       fldno);
      p->waitcmd = WAITCMD_QUIT;
      v->type = DILV_ERR;
      break;
   }
}

