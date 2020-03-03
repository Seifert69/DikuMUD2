/* *********************************************************************** *
 * File   : dilexp.h                                  Part of Valhalla MUD *
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

/* Wed Jan 22 14:57:30 PST 1997   HHS added paycheck dilfun (dilfe_pck) */

#ifndef _MUD_DILEXP_H
#define _MUD_DILEXP_H

#include "dil.h"

void dilfe_rti(struct dilprg *, struct dilval *);
void dilfe_txf(struct dilprg *, struct dilval *);
void dilfe_ast(struct dilprg *, struct dilval *);

void dilfe_splx(struct dilprg *, struct dilval *);
void dilfe_spli(struct dilprg *, struct dilval *);
void dilfe_mons(struct dilprg *, struct dilval *);
void dilfe_path(struct dilprg *, struct dilval *);
void dilfe_cary(struct dilprg *, struct dilval *);
void dilfe_fits(struct dilprg *, struct dilval *);
void dilfe_rest(struct dilprg *, struct dilval *);
void dilfe_opro(struct dilprg *, struct dilval *);
void dilfe_eqpm(struct dilprg *, struct dilval *);
void dilfe_mel(struct dilprg *, struct dilval *);
void dilfe_cast2(struct dilprg *, struct dilval *);
void dilfe_atsp(struct dilprg *, struct dilval *);
void dilfe_weat(register struct dilprg *p, register struct dilval *v);
void dilfe_illegal(struct dilprg *, struct dilval *);
void dilfe_plus(struct dilprg *, struct dilval *);
void dilfe_min(struct dilprg *, struct dilval *);
void dilfe_mul(struct dilprg *, struct dilval *);
void dilfe_div(struct dilprg *, struct dilval *);
void dilfe_mod(struct dilprg *, struct dilval *);
void dilfe_and(struct dilprg *, struct dilval *);
void dilfe_or(struct dilprg *, struct dilval *);
void dilfe_not(struct dilprg *, struct dilval *);
void dilfe_atoi(struct dilprg *, struct dilval *);
void dilfe_rnd(struct dilprg *, struct dilval *);
void dilfe_fndu(struct dilprg *, struct dilval *);
void dilfe_fndru(struct dilprg *, struct dilval *);
void dilfe_fndr(struct dilprg *, struct dilval *);
void dilfe_load(struct dilprg *, struct dilval *);
void dilfe_gt(struct dilprg *, struct dilval *);
void dilfe_lt(struct dilprg *, struct dilval *);
void dilfe_ge(struct dilprg *, struct dilval *);
void dilfe_le(struct dilprg *, struct dilval *);
void dilfe_eq(struct dilprg *, struct dilval *);
void dilfe_pe(struct dilprg *, struct dilval *);
void dilfe_ne(struct dilprg *, struct dilval *);
void dilfe_iss(struct dilprg *, struct dilval *);
void dilfe_in(struct dilprg *, struct dilval *);
void dilfe_getw(struct dilprg *, struct dilval *);
void dilfe_getws(struct dilprg *, struct dilval *);
void dilfe_fld(struct dilprg *, struct dilval *);
void dilfe_var(struct dilprg *, struct dilval *);
void dilfe_fs(struct dilprg *, struct dilval *);
void dilfe_int(struct dilprg *, struct dilval *);
void dilfe_umin(struct dilprg *, struct dilval *);
void dilfe_self(struct dilprg *, struct dilval *);
void dilfe_acti(struct dilprg *, struct dilval *);
void dilfe_argm(struct dilprg *, struct dilval *);
void dilfe_cmst(struct dilprg *, struct dilval *);
void dilfe_itoa(struct dilprg *, struct dilval *);
void dilfe_len(struct dilprg *, struct dilval *);
void dilfe_se(struct dilprg *, struct dilval *);
void dilfe_fsl(struct dilprg *, struct dilval *);
void dilfe_hrt(struct dilprg *, struct dilval *);
void dilfe_isa(struct dilprg *, struct dilval *);
void dilfe_tho(struct dilprg *, struct dilval *);
void dilfe_tda(struct dilprg *, struct dilval *);
void dilfe_tmd(struct dilprg *, struct dilval *);
void dilfe_tye(struct dilprg *, struct dilval *);
void dilfe_cmds(struct dilprg *, struct dilval *);
void dilfe_fnds(struct dilprg *, struct dilval *);
void dilfe_fnds2(struct dilprg *, struct dilval *);
void dilfe_sne(struct dilprg *, struct dilval *);
void dilfe_pne(struct dilprg *, struct dilval *);
void dilfe_null(struct dilprg *, struct dilval *);
void dilfe_dlf(struct dilprg *, struct dilval *);
void dilfe_dld(struct dilprg *, struct dilval *);
void dilfe_lor(struct dilprg *, struct dilval *);
void dilfe_land(struct dilprg *, struct dilval *);
void dilfe_intr(struct dilprg *, struct dilval *);
void dilfe_visi(struct dilprg *, struct dilval *);
void dilfe_oppo(struct dilprg *, struct dilval *);
void dilfe_purs(struct dilprg *, struct dilval *);
void dilfe_medi(struct dilprg *, struct dilval *);
void dilfe_targ(struct dilprg *, struct dilval *);
void dilfe_powe(struct dilprg *, struct dilval *);
void dilfe_trmo(struct dilprg *, struct dilval *);
void dilfe_pck(struct dilprg *, struct dilval *);
void dilfe_act(struct dilprg *, struct dilval *);

#endif



