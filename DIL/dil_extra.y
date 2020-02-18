/* *********************************************************************** *
 * File   : dil_extra.y                               Part of Valhalla MUD *
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


forlist  : block
         { $$ = $1; }
         | dilinst
         { $$ = $1; }
         | dilinst ',' forlist
         { $$.fst = $1.fst; $$.lst = $3.lst; }
         | block ',' forlist
         { $$.fst = $1.fst; $$.lst = $3.lst; }
         

         | DILSI_FOR '(' forlist ';'
             pushcnt ihold coreexp ahold ';'
             pushbrk defcnt ihold ahold forlist ihold ahold ')'
             dilcomposed ihold ahold
             defbrk popbrk popcnt
         {
            /*
             * allows both break and continue inside the composed
             * and induction statment list.
             * continue starts at the induction statement.
             * break starts with the instruction after loop.
             * The order of execution is reordered using goto.
             *
             * <forlist1>
             * <if> <exp> <break-adr>
             * <goto> <composed-adr>
             * <forlist2> <goto> <if-adr>
             * <composed> <goto> <forlist2-adr>
             */
				 
            wtmp = &tmpl.core[$6];
            bwrite_ubit8(&wtmp,DILI_IF);
            wtmp = &tmpl.core[$8];
            bwrite_ubit32(&wtmp,wcore-tmpl.core);
            wtmp = &tmpl.core[$12];
            bwrite_ubit8(&wtmp,DILI_GOTO);
            bwrite_ubit32(&wtmp,$18.fst);
            wtmp = &tmpl.core[$15];
            bwrite_ubit8(&wtmp,DILI_GOTO);
            bwrite_ubit32(&wtmp,$6);
            wtmp = &tmpl.core[$19];
            bwrite_ubit8(&wtmp,DILI_GOTO);
            bwrite_ubit32(&wtmp,$14.fst);				
         }
         | DILSI_FOE SYMBOL ihold ihold ihold ihold '(' coreexp ')'
             pushbrk pushcnt ihold ihold ihold ihold ahold
             dilcomposed
             defcnt ihold ihold ihold ihold ihold ihold ihold
             ihold ahold
             defbrk popbrk popcnt
         {
            /*
             * allows both break and continue inside the composed
             * statment. continue starts at the induction statement.
             * break starts with the instruction after loop.
             * The order of execution is ordered effectively.
             *
             * <ass> <var> <var#> <coreexp>
             * <if> <var> <var#> <break-adr>
             * <composed>
             * <ass> <var> <var#> <fld> <var> <var#> <.next>
             * <goto> <if-adr>
             */
            int i;

            if ((i = search_block($2, var_names, TRUE)) == -1)
               fatal("Unknown variable");
            else if (tmpl.vart[i]!=DILV_UP) 
               fatal("Foreach variable must be unitptr");
            else if ($8.typ != DILV_UP)
               fatal("Foreach expression must be unitptr");
            else {
               /* assign variable to expression initially */
               wtmp = &tmpl.core[$3];
               bwrite_ubit8(&wtmp,DILI_ASS);
               bwrite_ubit8(&wtmp,DILE_VAR);
               bwrite_ubit16(&wtmp,i);
               wtmp = &tmpl.core[$12];
               bwrite_ubit8(&wtmp,DILI_IF);
               bwrite_ubit8(&wtmp,DILE_VAR);
               bwrite_ubit16(&wtmp,i);
               bwrite_ubit32(&wtmp,wcore-tmpl.core);
               wtmp = &tmpl.core[$19];
               bwrite_ubit8(&wtmp,DILI_ASS);
               bwrite_ubit8(&wtmp,DILE_VAR);
               bwrite_ubit16(&wtmp,i);
               bwrite_ubit8(&wtmp,DILE_FLD);
               bwrite_ubit8(&wtmp,DILE_VAR);
               bwrite_ubit16(&wtmp,i);
               bwrite_ubit8(&wtmp,DILF_NXT);
               bwrite_ubit8(&wtmp,DILI_GOTO);
               bwrite_ubit32(&wtmp,$12);					
            }
         }
