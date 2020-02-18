/* *********************************************************************** *
 * File   : dilrun.h                                  Part of Valhalla MUD *
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
#ifndef _MUD_DILRUN_H
#define _MUD_DILRUN_H

struct dil_func_type
{
   void (*func) (struct dilprg *, struct dilval *);
};

extern struct dil_func_type dilfe_func[];

/* Maximum number of consecutive instructions allowed in one tick */
#define WAITCMD_FINISH	  -100	/* Stop program execution until next event */
#define WAITCMD_STOP      -150  /* Suspend execution                       */
#define WAITCMD_QUIT	  -200	/* Quit the DIL program entirely	   */
#define WAITCMD_DESTROYED -500	/* The DIL program was destroyed	   */

/* MS: The ONLY reason for having TWO almost identical evals, is that I
   want to initialize the result variables as error by default! */

#define eval_dil_exp(prg, v) \
   { (prg)->sp->pc++; \
   (v)->type = DILV_ERR; \
   (dilfe_func[*(prg->sp->pc-1)].func ((prg), (v))); \
   assert((prg)->sp->pc <= \
     &((prg)->sp->tmpl->core[(prg)->sp->tmpl->coresz])); }


void DeactivateDil(struct unit_data *pc);
void ActivateDil(struct unit_data *pc);

int dil_direct_init(struct spec_arg *sarg);
int dil_init(struct spec_arg *sarg);

void dil_activate(struct dilprg *prg);
void dil_init_vars(int varc, struct dilframe *frm);
void dil_free_var(struct dilvar *var);

void dil_free_frame(struct dilframe *frame);

char dil_getbool(struct dilval *v);
int dil_getval(struct dilval *v);
void dil_add_secure(struct dilprg *prg, struct unit_data *sup, ubit8 *lab);
#ifdef __cplusplus
void dil_sub_secure(struct dilframe *frm, struct unit_data *sup,
		    int bForeach = FALSE);
#endif
int same_environment(struct unit_data *u1, struct unit_data *u2);
void unhash_str(char **s);
void dil_clear_non_secured(register struct dilprg *prg);
void dil_clear_lost_reference(register struct dilframe *frm, void *ptr);
void dil_test_secure(register struct dilprg *prg);
int dil_destroy(char *name, struct unit_data *u);

/* NULL fptr creates one...  */
struct dilprg *dil_copy_template(struct diltemplate *tmpl,
				 struct unit_data *u,
				 struct unit_fptr **pfptr);
struct dilprg *dil_copy(char *name, struct unit_data *u);


struct unit_fptr *dil_find(const char *name, struct unit_data *u);

void dil_typeerr(struct dilprg *p, const char *where);

#define FAIL_NULL   1
#define TYPEFAIL_NULL 2

int dil_type_check(const char *f, struct dilprg *p, int tot, ...);
void dil_free_prg(struct dilprg *prg);

void dil_intr_remove(struct dilprg *p, int idx);
int dil_intr_insert(struct dilprg *p, ubit8 *lab, ubit16 flags);



#endif
