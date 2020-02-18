/* *********************************************************************** *
 * File   : dilinst.h                                 Part of Valhalla MUD *
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
#ifndef _MUD_DILINST_H
#define _MUD_DILINST_H

void dil_insterr(struct dilprg *p, char *where);

void dilfi_folo(struct dilprg *, struct dilval *);
void dilfi_lcri(struct dilprg *, struct dilval *);
void dilfi_sete(struct dilprg *, struct dilval *);
void dilfi_amod(struct dilprg *, struct dilval *);
void dilfi_stor(struct dilprg *, struct dilval *);
void dilfi_chas(struct dilprg *, struct dilval *);
void dilfi_setf(struct dilprg *, struct dilval *);
void dilfi_ass(struct dilprg *, struct dilval *);
void dilfi_lnk(struct dilprg *, struct dilval *);
void dilfi_exp(struct dilprg *, struct dilval *);
void dilfi_if(struct dilprg *, struct dilval *);
void dilfi_set(struct dilprg *, struct dilval *);
void dilfi_uset(struct dilprg *, struct dilval *);
void dilfi_adl(struct dilprg *, struct dilval *);
void dilfi_sul(struct dilprg *, struct dilval *);
void dilfi_ade(struct dilprg *, struct dilval *);
void dilfi_sue(struct dilprg *, struct dilval *);
void dilfi_dst(struct dilprg *, struct dilval *);
void dilfi_walk(struct dilprg *, struct dilval *);
void dilfi_exec(struct dilprg *, struct dilval *);
void dilfi_wit(struct dilprg *, struct dilval *);
void dilfi_act(struct dilprg *, struct dilval *);
void dilfi_goto(struct dilprg *, struct dilval *);
void dilfi_sua(struct dilprg *, struct dilval *);
void dilfi_ada(struct dilprg *, struct dilval *);
void dilfi_pri(struct dilprg *, struct dilval *);
void dilfi_npr(struct dilprg *, struct dilval *);
void dilfi_snd(struct dilprg *, struct dilval *);
void dilfi_snt(struct dilprg *, struct dilval *);
void dilfi_snta(struct dilprg *, struct dilval *);
void dilfi_log(struct dilprg *, struct dilval *);
void dilfi_sec(struct dilprg *, struct dilval *);
void dilfi_use(struct dilprg *, struct dilval *);
void dilfi_eqp(struct dilprg *, struct dilval *);
void dilfi_ueq(struct dilprg *, struct dilval *);
void dilfi_quit(struct dilprg *, struct dilval *);
void dilfi_blk(struct dilprg *, struct dilval *);
void dilfi_pup(struct dilprg *, struct dilval *);
void dilfi_cast(struct dilprg *, struct dilval *);
void dilfi_rproc(struct dilprg *, struct dilval *);
void dilfi_rfunc(struct dilprg *, struct dilval *);
void dilfi_rts(struct dilprg *, struct dilval *);
void dilfi_rtf(struct dilprg *, struct dilval *);
void dilfi_dlc(struct dilprg *, struct dilval *);
void dilfi_on(struct dilprg *, struct dilval *);
void dilfi_rsproc(struct dilprg *, struct dilval *);
void dilfi_rsfunc(struct dilprg *, struct dilval *);
void dilfi_swt(struct dilprg *, struct dilval *);
void dilfi_sbt(struct dilprg *, struct dilval *);
void dilfi_cli(struct dilprg *, struct dilval *);
void dilfi_foe(struct dilprg *, struct dilval *);
void dilfi_fon(struct dilprg *, struct dilval *);
void dilfi_sntadil(struct dilprg *, struct dilval *);

#endif


