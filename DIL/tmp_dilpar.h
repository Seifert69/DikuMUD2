/* A Bison parser, made by GNU Bison 3.4.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2019 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    UNKNOWN = 258,
    STRING = 259,
    SYMBOL = 260,
    PNUM = 261,
    DILSC_VAR = 262,
    DILSC_BEG = 263,
    DILSC_COD = 264,
    DILSC_END = 265,
    DILSC_EXT = 266,
    DILSC_REC = 267,
    DILSC_AWA = 268,
    DILSE_ATOI = 269,
    DILSE_RND = 270,
    DILSE_FNDU = 271,
    DILSE_FNDRU = 272,
    DILSE_FNDR = 273,
    DILSE_LOAD = 274,
    DILSE_GETW = 275,
    DILSE_ITOA = 276,
    DILSE_ISS = 277,
    DILSE_IN = 278,
    DILSE_ISA = 279,
    DILSE_CMDS = 280,
    DILSE_FNDS = 281,
    DILSE_GETWS = 282,
    DILSE_LEN = 283,
    DILSE_PURS = 284,
    DILSE_TRMO = 285,
    DILSE_DLD = 286,
    DILSE_DLF = 287,
    DILSE_LAND = 288,
    DILSE_LOR = 289,
    DILSE_VISI = 290,
    DILSE_OPPO = 291,
    DILSE_RTI = 292,
    DILSE_PCK = 293,
    DILSE_AND = 294,
    DILSE_OR = 295,
    DILSE_NOT = 296,
    DILTO_EQ = 297,
    DILTO_NEQ = 298,
    DILTO_PEQ = 299,
    DILTO_SEQ = 300,
    DILTO_LEQ = 301,
    DILTO_GEQ = 302,
    DILTO_GT = 303,
    DILTO_LE = 304,
    DILTO_ASS = 305,
    DILSE_NULL = 306,
    DILSE_SELF = 307,
    DILSE_ACTI = 308,
    DILSE_ARGM = 309,
    DILSE_HRT = 310,
    DILSE_CMST = 311,
    DILSE_TDA = 312,
    DILSE_THO = 313,
    DILSE_TMD = 314,
    DILSE_TYE = 315,
    DILSE_SKIP = 316,
    DILSE_WEAT = 317,
    DILSE_MEDI = 318,
    DILSE_TARG = 319,
    DILSE_POWE = 320,
    DILSE_CST = 321,
    DILSE_MEL = 322,
    DILSE_EQPM = 323,
    DILSE_OPRO = 324,
    DILSE_REST = 325,
    DILSE_FIT = 326,
    DILSE_CARY = 327,
    DILSE_PATH = 328,
    DILSE_MONS = 329,
    DILSE_SPLX = 330,
    DILSE_SPLI = 331,
    DILSE_TXF = 332,
    DILSE_AST = 333,
    DILSI_LNK = 334,
    DILSI_EXP = 335,
    DILSI_SET = 336,
    DILSI_UST = 337,
    DILSI_ADE = 338,
    DILSI_SUE = 339,
    DILSI_DST = 340,
    DILSI_ADL = 341,
    DILSI_SUL = 342,
    DILSI_SND = 343,
    DILSI_SNT = 344,
    DILSI_SEC = 345,
    DILSI_USE = 346,
    DILSI_ADA = 347,
    DILSI_SETF = 348,
    DILSI_CHAS = 349,
    DILSI_SUA = 350,
    DILSI_EQP = 351,
    DILSI_UEQ = 352,
    DILSI_OAC = 353,
    DILSI_STOR = 354,
    DILSI_SETE = 355,
    DILSI_QUIT = 356,
    DILSI_LOG = 357,
    DILSI_SNTA = 358,
    DILSI_SNTADIL = 359,
    DILSI_DLC = 360,
    DILSE_INTR = 361,
    DILSI_CLI = 362,
    DILSI_SWT = 363,
    DILSI_SBT = 364,
    DILSE_ATSP = 365,
    DILSI_FOLO = 366,
    DILSI_LCRI = 367,
    DILSI_WHI = 368,
    DILSI_IF = 369,
    DILSI_WLK = 370,
    DILSI_EXE = 371,
    DILSI_WIT = 372,
    DILSI_ACT = 373,
    DILSI_ELS = 374,
    DILSI_GOT = 375,
    DILSI_PRI = 376,
    DILSI_NPR = 377,
    DILSI_BLK = 378,
    DILSI_CNT = 379,
    DILSI_PUP = 380,
    DILSI_FOR = 381,
    DILSI_FOE = 382,
    DILSI_BRK = 383,
    DILSI_RTS = 384,
    DILSI_ON = 385,
    DILSI_AMOD = 386,
    DILSF_ZOI = 387,
    DILSF_NMI = 388,
    DILSF_TYP = 389,
    DILSF_NXT = 390,
    DILSF_NMS = 391,
    DILSF_NAM = 392,
    DILSF_IDX = 393,
    DILSF_TIT = 394,
    DILSF_EXT = 395,
    DILSF_OUT = 396,
    DILSF_INS = 397,
    DILSF_GNX = 398,
    DILSF_SPD = 399,
    DILSF_GPR = 400,
    DILSF_LGT = 401,
    DILSF_BGT = 402,
    DILSF_MIV = 403,
    DILSF_ILL = 404,
    DILSF_FL = 405,
    DILSF_OFL = 406,
    DILSF_MHP = 407,
    DILSF_CHP = 408,
    DILSF_BWT = 409,
    DILSF_WGT = 410,
    DILSF_CAP = 411,
    DILSF_ALG = 412,
    DILSF_SPL = 413,
    DILSF_FUN = 414,
    DILSF_ZON = 415,
    DILSF_MAS = 416,
    DILSF_FOL = 417,
    DILSF_OTY = 418,
    DILSF_VAL = 419,
    DILSF_EFL = 420,
    DILSF_CST = 421,
    DILSF_RNT = 422,
    DILSF_EQP = 423,
    DILSF_ONM = 424,
    DILSF_XNF = 425,
    DILSF_TOR = 426,
    DILSF_RFL = 427,
    DILSF_MOV = 428,
    DILSF_ACT = 429,
    DILSF_SEX = 430,
    DILSF_RCE = 431,
    DILSF_ABL = 432,
    DILSF_EXP = 433,
    DILSF_LVL = 434,
    DILSF_HGT = 435,
    DILSF_POS = 436,
    DILSF_ATY = 437,
    DILSF_MNA = 438,
    DILSF_END = 439,
    DILSF_WPN = 440,
    DILSF_MAN = 441,
    DILSF_AFF = 442,
    DILSF_DRE = 443,
    DILSF_FGT = 444,
    DILSF_DEF = 445,
    DILSF_TIM = 446,
    DILSF_CRM = 447,
    DILSF_FLL = 448,
    DILSF_THR = 449,
    DILSF_DRK = 450,
    DILSF_SPT = 451,
    DILSF_APT = 452,
    DILSF_GLD = 453,
    DILSF_QST = 454,
    DILSF_SKL = 455,
    DILSF_BIR = 456,
    DILSF_PTI = 457,
    DILSF_PCF = 458,
    DILSF_HOME = 459,
    DILSF_ODES = 460,
    DILSF_IDES = 461,
    DILSF_DES = 462,
    DILSF_LCN = 463,
    DILSF_ABAL = 464,
    DILSF_ATOT = 465,
    DILSF_MMA = 466,
    DILSF_LSA = 467,
    DILSF_INFO = 468,
    DILSF_MED = 469,
    DILST_UP = 470,
    DILST_INT = 471,
    DILST_SP = 472,
    DILST_SLP = 473,
    DILST_EDP = 474,
    UMINUS = 475,
    UPLUS = 476
  };
#endif
/* Tokens.  */
#define UNKNOWN 258
#define STRING 259
#define SYMBOL 260
#define PNUM 261
#define DILSC_VAR 262
#define DILSC_BEG 263
#define DILSC_COD 264
#define DILSC_END 265
#define DILSC_EXT 266
#define DILSC_REC 267
#define DILSC_AWA 268
#define DILSE_ATOI 269
#define DILSE_RND 270
#define DILSE_FNDU 271
#define DILSE_FNDRU 272
#define DILSE_FNDR 273
#define DILSE_LOAD 274
#define DILSE_GETW 275
#define DILSE_ITOA 276
#define DILSE_ISS 277
#define DILSE_IN 278
#define DILSE_ISA 279
#define DILSE_CMDS 280
#define DILSE_FNDS 281
#define DILSE_GETWS 282
#define DILSE_LEN 283
#define DILSE_PURS 284
#define DILSE_TRMO 285
#define DILSE_DLD 286
#define DILSE_DLF 287
#define DILSE_LAND 288
#define DILSE_LOR 289
#define DILSE_VISI 290
#define DILSE_OPPO 291
#define DILSE_RTI 292
#define DILSE_PCK 293
#define DILSE_AND 294
#define DILSE_OR 295
#define DILSE_NOT 296
#define DILTO_EQ 297
#define DILTO_NEQ 298
#define DILTO_PEQ 299
#define DILTO_SEQ 300
#define DILTO_LEQ 301
#define DILTO_GEQ 302
#define DILTO_GT 303
#define DILTO_LE 304
#define DILTO_ASS 305
#define DILSE_NULL 306
#define DILSE_SELF 307
#define DILSE_ACTI 308
#define DILSE_ARGM 309
#define DILSE_HRT 310
#define DILSE_CMST 311
#define DILSE_TDA 312
#define DILSE_THO 313
#define DILSE_TMD 314
#define DILSE_TYE 315
#define DILSE_SKIP 316
#define DILSE_WEAT 317
#define DILSE_MEDI 318
#define DILSE_TARG 319
#define DILSE_POWE 320
#define DILSE_CST 321
#define DILSE_MEL 322
#define DILSE_EQPM 323
#define DILSE_OPRO 324
#define DILSE_REST 325
#define DILSE_FIT 326
#define DILSE_CARY 327
#define DILSE_PATH 328
#define DILSE_MONS 329
#define DILSE_SPLX 330
#define DILSE_SPLI 331
#define DILSE_TXF 332
#define DILSE_AST 333
#define DILSI_LNK 334
#define DILSI_EXP 335
#define DILSI_SET 336
#define DILSI_UST 337
#define DILSI_ADE 338
#define DILSI_SUE 339
#define DILSI_DST 340
#define DILSI_ADL 341
#define DILSI_SUL 342
#define DILSI_SND 343
#define DILSI_SNT 344
#define DILSI_SEC 345
#define DILSI_USE 346
#define DILSI_ADA 347
#define DILSI_SETF 348
#define DILSI_CHAS 349
#define DILSI_SUA 350
#define DILSI_EQP 351
#define DILSI_UEQ 352
#define DILSI_OAC 353
#define DILSI_STOR 354
#define DILSI_SETE 355
#define DILSI_QUIT 356
#define DILSI_LOG 357
#define DILSI_SNTA 358
#define DILSI_SNTADIL 359
#define DILSI_DLC 360
#define DILSE_INTR 361
#define DILSI_CLI 362
#define DILSI_SWT 363
#define DILSI_SBT 364
#define DILSE_ATSP 365
#define DILSI_FOLO 366
#define DILSI_LCRI 367
#define DILSI_WHI 368
#define DILSI_IF 369
#define DILSI_WLK 370
#define DILSI_EXE 371
#define DILSI_WIT 372
#define DILSI_ACT 373
#define DILSI_ELS 374
#define DILSI_GOT 375
#define DILSI_PRI 376
#define DILSI_NPR 377
#define DILSI_BLK 378
#define DILSI_CNT 379
#define DILSI_PUP 380
#define DILSI_FOR 381
#define DILSI_FOE 382
#define DILSI_BRK 383
#define DILSI_RTS 384
#define DILSI_ON 385
#define DILSI_AMOD 386
#define DILSF_ZOI 387
#define DILSF_NMI 388
#define DILSF_TYP 389
#define DILSF_NXT 390
#define DILSF_NMS 391
#define DILSF_NAM 392
#define DILSF_IDX 393
#define DILSF_TIT 394
#define DILSF_EXT 395
#define DILSF_OUT 396
#define DILSF_INS 397
#define DILSF_GNX 398
#define DILSF_SPD 399
#define DILSF_GPR 400
#define DILSF_LGT 401
#define DILSF_BGT 402
#define DILSF_MIV 403
#define DILSF_ILL 404
#define DILSF_FL 405
#define DILSF_OFL 406
#define DILSF_MHP 407
#define DILSF_CHP 408
#define DILSF_BWT 409
#define DILSF_WGT 410
#define DILSF_CAP 411
#define DILSF_ALG 412
#define DILSF_SPL 413
#define DILSF_FUN 414
#define DILSF_ZON 415
#define DILSF_MAS 416
#define DILSF_FOL 417
#define DILSF_OTY 418
#define DILSF_VAL 419
#define DILSF_EFL 420
#define DILSF_CST 421
#define DILSF_RNT 422
#define DILSF_EQP 423
#define DILSF_ONM 424
#define DILSF_XNF 425
#define DILSF_TOR 426
#define DILSF_RFL 427
#define DILSF_MOV 428
#define DILSF_ACT 429
#define DILSF_SEX 430
#define DILSF_RCE 431
#define DILSF_ABL 432
#define DILSF_EXP 433
#define DILSF_LVL 434
#define DILSF_HGT 435
#define DILSF_POS 436
#define DILSF_ATY 437
#define DILSF_MNA 438
#define DILSF_END 439
#define DILSF_WPN 440
#define DILSF_MAN 441
#define DILSF_AFF 442
#define DILSF_DRE 443
#define DILSF_FGT 444
#define DILSF_DEF 445
#define DILSF_TIM 446
#define DILSF_CRM 447
#define DILSF_FLL 448
#define DILSF_THR 449
#define DILSF_DRK 450
#define DILSF_SPT 451
#define DILSF_APT 452
#define DILSF_GLD 453
#define DILSF_QST 454
#define DILSF_SKL 455
#define DILSF_BIR 456
#define DILSF_PTI 457
#define DILSF_PCF 458
#define DILSF_HOME 459
#define DILSF_ODES 460
#define DILSF_IDES 461
#define DILSF_DES 462
#define DILSF_LCN 463
#define DILSF_ABAL 464
#define DILSF_ATOT 465
#define DILSF_MMA 466
#define DILSF_LSA 467
#define DILSF_INFO 468
#define DILSF_MED 469
#define DILST_UP 470
#define DILST_INT 471
#define DILST_SP 472
#define DILST_SLP 473
#define DILST_EDP 474
#define UMINUS 475
#define UPLUS 476

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 152 "dilpar.y"

   sbit32 num;
   char *str;
   char sym[SYMSIZE + 1];
   char **str_list;
   struct exptype exp;
   struct {
      ubit32 fst,lst; /* first, last addr in core */
      ubit8 dsl,typ;      /* if expression: leftvalue, type */
      ubit8 boolean;
   } ins;
   struct dilxref xref;

#line 513 "y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
