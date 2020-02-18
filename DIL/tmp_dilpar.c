/* A Bison parser, made by GNU Bison 3.4.1.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 37 "dilpar.y"

#include <stdio.h>
#include <unistd.h>

#include "structs.h"
#include "utils.h"
#include "dilpar.h"
#include "textutil.h"
#include "db_file.h"
#include "dil.h"

extern char *yytext; 
extern int yylex();
extern int linenum;
 
/*
 *  MUST be changed to
 *  `extern char yytext[]'
 *  if you use lex instead of flex.
 */

int sunlight = 0;
const sbit8 time_light[24] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

#define yyerror(dum) syntax(dum)

#define DSL_STA 0            /* static expression */
#define DSL_DYN 1            /* dynamic expression */
#define DSL_LFT 2            /* left expression */
#define DSL_FCT 3            /* functon/procedure */
#define DSL_SFT 4            /* symbolic functon/procedure */

char cur_filename[255];

const char *cur_zonename="NOZONE"; /* zone name from argument */
char cur_tmplref[8192];      /* generated template reference (if -t) */

char **var_names;       /* names of variables */
ubit8 in_foreach=0;     /* inside foreach */
 
ubit16 label_no;        /* number of labels */
char **label_names;     /* names of labels */
ubit32 *label_adr;      /* address of label */

ubit16 label_use_no;    /* number of used labels */
ubit32 *label_use_idx;  /* what label is used (label_adr) */
ubit32 *label_use_adr;  /* where a label is used */
ubit32 labelgen;        /* counter for label generation */

ubit16 break_no;		/* size of break stack */
ubit16 cont_no;			/* size of continue stack */
ubit16 *break_idx;		/* break stack (label idx) */
ubit16 *cont_idx;		/* continue stack (label idx) */

int errcon = 0;
int istemplate = 0;		/* is this a template program */

/* work structures */

struct dilref {
   char *zname;		/* func/proc name */
   char *name;          /* zone name */
   ubit8 rtnt;		/* return type */
   ubit8 argc;		/* number of arguments (min 1) */
   ubit8 *argt;		/* argument types */
   char **argv;		/* argument names */
};

struct dilprg prg;              /* current program building */
struct diltemplate tmpl;        /* current template building */
struct dilframe frm;		/* current frame building */
struct dilref ref;		/* current reference */

#define REFMAX 2048 		/* max # of references */
#define VARMAX 255		/* max # of variables */
#define ARGMAX 250		/* max # of arguments */

struct dilref refs[REFMAX];	/* references read  */

/* the registered names in 'name@zone' format */
char **ref_names;		/* for fast search */
char **ref_usednames;           /* used names, registered in tmpl */
int refcount = 0;		/* number of ext. ref. */

ubit8 *wcore;           /* writing pointer (in cur.core )  */
ubit8 *wtmp;            /* writing pointer (in cur.core )  */

char *str_list[300];     /* Data for string lists */
int str_top;            /* Number of strings */
char tmpfname[L_tmpnam] = "";

void add_ref(struct dilref *ref);
void add_var(char *name, ubit16 type);
int add_label(char *str, ubit32 adr);
ubit32 get_label(char *name, ubit32 adr);
void moredilcore(ubit32 size);
void update_labels(void);
void fatal(const char *str);
void warning(const char *str);
void syntax(const char *str);
void dumpdiltemplate(struct diltemplate *tmpl);
void dumpdil(struct dilprg *prg);
int dil_corecrc(ubit8 *core, int len);
int dil_headercrc(char **name, ubit8 *type);

#define INITEXP(v) \
   CREATE(v.code, ubit8, CODESIZE); \
   v.codep=v.code;v.num=0;v.typ=v.rtyp=DILV_NULL;v.dsl=DSL_STA;v.boolean=0;

#define FREEEXP(v) \
   free(v.code); v.code = NULL;

#define checkbool(s,v) do {if (v) fprintf(stderr,"Warning: %s is boolean, not integer\n",s);} while (0)

#line 185 "y.tab.c"

# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
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

#line 684 "y.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */

/* Second part of user prologue.  */
#line 166 "dilpar.y"

void add_ubit8(struct exptype *dest, ubit8 d);
void add_ubit32(struct exptype *dest, ubit32 d);
void add_sbit32(struct exptype *dest, sbit32 d);
void add_ubit16(struct exptype *dest, ubit16 d);
void add_string(struct exptype *dest, char *d);
void cat_string(struct exptype *dest, char *d);
void add_stringlist(struct exptype *dest, char **d);
void add_code(struct exptype *dest, struct exptype *src);
void copy_code(struct exptype *dest, struct exptype *src);
void make_code(struct exptype *dest);

#line 713 "y.tab.c"


#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2001

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  237
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  60
/* YYNRULES -- Number of rules.  */
#define YYNRULES  302
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  902

#define YYUNDEFTOK  2
#define YYMAXUTOK   476

/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  ((unsigned) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,   233,     2,     2,
       3,     4,   231,   229,     9,   230,   236,   232,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    11,    10,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     7,     2,     8,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     5,     2,     6,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   157,   158,   159,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     174,   175,   176,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,   190,   191,   192,   193,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   234,   235
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   255,   255,   331,   352,   357,   363,   368,   438,   439,
     446,   450,   453,   454,   458,   461,   466,   491,   518,   523,
     524,   527,   528,   531,   540,   599,   609,   613,   617,   621,
     625,   635,   682,   738,   745,   752,   759,   766,   773,   780,
     787,   794,   801,   808,   815,   822,   829,   848,   861,   876,
     884,   892,   900,   908,   916,   924,   932,   940,   948,   956,
     964,   972,   980,   988,   996,  1004,  1012,  1020,  1028,  1036,
    1044,  1052,  1060,  1068,  1076,  1084,  1092,  1100,  1108,  1116,
    1124,  1132,  1140,  1148,  1156,  1164,  1172,  1180,  1188,  1196,
    1205,  1213,  1226,  1234,  1244,  1252,  1260,  1268,  1281,  1291,
    1301,  1311,  1319,  1332,  1340,  1348,  1356,  1364,  1372,  1382,
    1390,  1398,  1406,  1414,  1422,  1430,  1438,  1446,  1454,  1462,
    1477,  1485,  1498,  1506,  1514,  1522,  1530,  1538,  1546,  1554,
    1562,  1570,  1586,  1592,  1598,  1602,  1608,  1612,  1616,  1620,
    1630,  1638,  1698,  1721,  1754,  1779,  1787,  1806,  1818,  1868,
    1895,  1919,  1945,  1953,  1981,  2008,  2031,  2054,  2081,  2122,
    2129,  2136,  2143,  2155,  2168,  2175,  2184,  2205,  2221,  2242,
    2263,  2279,  2295,  2343,  2364,  2387,  2409,  2442,  2470,  2486,
    2502,  2528,  2550,  2572,  2594,  2610,  2667,  2688,  2709,  2735,
    2757,  2784,  2800,  2843,  2886,  2917,  2943,  2969,  2995,  3033,
    3048,  3063,  3078,  3093,  3120,  3131,  3135,  3143,  3169,  3204,
    3211,  3217,  3223,  3233,  3237,  3247,  3253,  3261,  3297,  3308,
    3319,  3332,  3345,  3358,  3372,  3386,  3400,  3414,  3428,  3442,
    3456,  3471,  3486,  3504,  3517,  3532,  3545,  3559,  3593,  3604,
    3615,  3626,  3639,  3653,  3683,  3696,  3708,  3719,  3732,  3745,
    3758,  3771,  3782,  3789,  3796,  3803,  3810,  3821,  3835,  3853,
    3876,  3887,  3900,  3923,  3934,  3946,  3947,  3950,  3951,  3954,
    3958,  3960,  3964,  3966,  3968,  3972,  3985,  3998,  4009,  4014,
    4007,  4037,  4059,  4081,  4082,  4083,  4087,  4096,  4207,  4295,
    4312,  4331,  4335,  4339,  4342,  4344,  4346,  4348,  4365,  4378,
    4391,  4398,  4415
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "'('", "')'", "'{'", "'}'", "'['", "']'",
  "','", "';'", "':'", "UNKNOWN", "STRING", "SYMBOL", "PNUM", "DILSC_VAR",
  "DILSC_BEG", "DILSC_COD", "DILSC_END", "DILSC_EXT", "DILSC_REC",
  "DILSC_AWA", "DILSE_ATOI", "DILSE_RND", "DILSE_FNDU", "DILSE_FNDRU",
  "DILSE_FNDR", "DILSE_LOAD", "DILSE_GETW", "DILSE_ITOA", "DILSE_ISS",
  "DILSE_IN", "DILSE_ISA", "DILSE_CMDS", "DILSE_FNDS", "DILSE_GETWS",
  "DILSE_LEN", "DILSE_PURS", "DILSE_TRMO", "DILSE_DLD", "DILSE_DLF",
  "DILSE_LAND", "DILSE_LOR", "DILSE_VISI", "DILSE_OPPO", "DILSE_RTI",
  "DILSE_PCK", "DILSE_AND", "DILSE_OR", "DILSE_NOT", "DILTO_EQ",
  "DILTO_NEQ", "DILTO_PEQ", "DILTO_SEQ", "DILTO_LEQ", "DILTO_GEQ",
  "DILTO_GT", "DILTO_LE", "DILTO_ASS", "DILSE_NULL", "DILSE_SELF",
  "DILSE_ACTI", "DILSE_ARGM", "DILSE_HRT", "DILSE_CMST", "DILSE_TDA",
  "DILSE_THO", "DILSE_TMD", "DILSE_TYE", "DILSE_SKIP", "DILSE_WEAT",
  "DILSE_MEDI", "DILSE_TARG", "DILSE_POWE", "DILSE_CST", "DILSE_MEL",
  "DILSE_EQPM", "DILSE_OPRO", "DILSE_REST", "DILSE_FIT", "DILSE_CARY",
  "DILSE_PATH", "DILSE_MONS", "DILSE_SPLX", "DILSE_SPLI", "DILSE_TXF",
  "DILSE_AST", "DILSI_LNK", "DILSI_EXP", "DILSI_SET", "DILSI_UST",
  "DILSI_ADE", "DILSI_SUE", "DILSI_DST", "DILSI_ADL", "DILSI_SUL",
  "DILSI_SND", "DILSI_SNT", "DILSI_SEC", "DILSI_USE", "DILSI_ADA",
  "DILSI_SETF", "DILSI_CHAS", "DILSI_SUA", "DILSI_EQP", "DILSI_UEQ",
  "DILSI_OAC", "DILSI_STOR", "DILSI_SETE", "DILSI_QUIT", "DILSI_LOG",
  "DILSI_SNTA", "DILSI_SNTADIL", "DILSI_DLC", "DILSE_INTR", "DILSI_CLI",
  "DILSI_SWT", "DILSI_SBT", "DILSE_ATSP", "DILSI_FOLO", "DILSI_LCRI",
  "DILSI_WHI", "DILSI_IF", "DILSI_WLK", "DILSI_EXE", "DILSI_WIT",
  "DILSI_ACT", "DILSI_ELS", "DILSI_GOT", "DILSI_PRI", "DILSI_NPR",
  "DILSI_BLK", "DILSI_CNT", "DILSI_PUP", "DILSI_FOR", "DILSI_FOE",
  "DILSI_BRK", "DILSI_RTS", "DILSI_ON", "DILSI_AMOD", "DILSF_ZOI",
  "DILSF_NMI", "DILSF_TYP", "DILSF_NXT", "DILSF_NMS", "DILSF_NAM",
  "DILSF_IDX", "DILSF_TIT", "DILSF_EXT", "DILSF_OUT", "DILSF_INS",
  "DILSF_GNX", "DILSF_SPD", "DILSF_GPR", "DILSF_LGT", "DILSF_BGT",
  "DILSF_MIV", "DILSF_ILL", "DILSF_FL", "DILSF_OFL", "DILSF_MHP",
  "DILSF_CHP", "DILSF_BWT", "DILSF_WGT", "DILSF_CAP", "DILSF_ALG",
  "DILSF_SPL", "DILSF_FUN", "DILSF_ZON", "DILSF_MAS", "DILSF_FOL",
  "DILSF_OTY", "DILSF_VAL", "DILSF_EFL", "DILSF_CST", "DILSF_RNT",
  "DILSF_EQP", "DILSF_ONM", "DILSF_XNF", "DILSF_TOR", "DILSF_RFL",
  "DILSF_MOV", "DILSF_ACT", "DILSF_SEX", "DILSF_RCE", "DILSF_ABL",
  "DILSF_EXP", "DILSF_LVL", "DILSF_HGT", "DILSF_POS", "DILSF_ATY",
  "DILSF_MNA", "DILSF_END", "DILSF_WPN", "DILSF_MAN", "DILSF_AFF",
  "DILSF_DRE", "DILSF_FGT", "DILSF_DEF", "DILSF_TIM", "DILSF_CRM",
  "DILSF_FLL", "DILSF_THR", "DILSF_DRK", "DILSF_SPT", "DILSF_APT",
  "DILSF_GLD", "DILSF_QST", "DILSF_SKL", "DILSF_BIR", "DILSF_PTI",
  "DILSF_PCF", "DILSF_HOME", "DILSF_ODES", "DILSF_IDES", "DILSF_DES",
  "DILSF_LCN", "DILSF_ABAL", "DILSF_ATOT", "DILSF_MMA", "DILSF_LSA",
  "DILSF_INFO", "DILSF_MED", "DILST_UP", "DILST_INT", "DILST_SP",
  "DILST_SLP", "DILST_EDP", "'+'", "'-'", "'*'", "'/'", "'%'", "UMINUS",
  "UPLUS", "'.'", "$accept", "file", "program", "diloptions", "dilinit",
  "dildecls", "decls", "dilrefs", "refs", "ref", "arginit", "refarglist",
  "somerefargs", "refarg", "fundef", "type", "variable", "idx", "field",
  "scmp", "pcmp", "cmp", "icmp", "dilexp", "dilsexp", "dilterm",
  "dilfactor", "dilfun", "stringlist", "strings", "coreexp", "corevar",
  "ihold", "iunhold", "ahold", "label", "labelskip", "labellist",
  "dilproc", "pushbrk", "defbrk", "popbrk", "pushcnt", "defcnt", "popcnt",
  "semicolons", "optsemicolons", "block", "dilinstlist", "dilcomposed",
  "dilcomplex", "$@1", "$@2", "arglist", "someargs", "dilass", "proccall",
  "corefunc", "dilassrgt", "dilinst", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,    40,    41,   123,   125,    91,    93,    44,
      59,    58,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,   275,
     276,   277,   278,   279,   280,   281,   282,   283,   284,   285,
     286,   287,   288,   289,   290,   291,   292,   293,   294,   295,
     296,   297,   298,   299,   300,   301,   302,   303,   304,   305,
     306,   307,   308,   309,   310,   311,   312,   313,   314,   315,
     316,   317,   318,   319,   320,   321,   322,   323,   324,   325,
     326,   327,   328,   329,   330,   331,   332,   333,   334,   335,
     336,   337,   338,   339,   340,   341,   342,   343,   344,   345,
     346,   347,   348,   349,   350,   351,   352,   353,   354,   355,
     356,   357,   358,   359,   360,   361,   362,   363,   364,   365,
     366,   367,   368,   369,   370,   371,   372,   373,   374,   375,
     376,   377,   378,   379,   380,   381,   382,   383,   384,   385,
     386,   387,   388,   389,   390,   391,   392,   393,   394,   395,
     396,   397,   398,   399,   400,   401,   402,   403,   404,   405,
     406,   407,   408,   409,   410,   411,   412,   413,   414,   415,
     416,   417,   418,   419,   420,   421,   422,   423,   424,   425,
     426,   427,   428,   429,   430,   431,   432,   433,   434,   435,
     436,   437,   438,   439,   440,   441,   442,   443,   444,   445,
     446,   447,   448,   449,   450,   451,   452,   453,   454,   455,
     456,   457,   458,   459,   460,   461,   462,   463,   464,   465,
     466,   467,   468,   469,   470,   471,   472,   473,   474,    43,
      45,    42,    47,    37,   475,   476,    46
};
# endif

#define YYPACT_NINF -622

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-622)))

#define YYTABLE_NINF -284

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      47,  -622,    68,  -622,    75,  -622,    75,    75,    60,  -622,
    -622,    77,  -622,  -622,  -622,  -622,  -622,  -622,    67,   105,
    -622,    60,   116,   145,   146,  -622,    60,   147,   138,  -622,
     151,   170,  -622,   175,  -622,   182,  -622,   189,   146,   -74,
     188,   146,   -74,  1229,   177,   196,  -622,  -622,  -622,   191,
    1229,   198,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,
    -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,
    -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,
    -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,
    -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,
    -622,  -622,  -622,  1927,  -622,   230,  1176,  -622,  -622,  -622,
     229,  -622,   235,   147,   234,   237,   243,   247,   251,   252,
     253,   255,   259,   260,   264,   265,   267,   277,   279,   286,
     297,   307,   308,   313,   314,   316,  -622,   320,   353,  -622,
     354,   356,   357,   358,   359,   361,   362,   364,   365,   374,
    -622,   375,   376,   377,   378,   379,   369,  -622,  -622,  -622,
    -622,   381,   385,  -622,   390,   867,   391,  -622,  -622,  -622,
    -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,
    -622,   160,   338,   384,  -622,  -622,  -622,   229,  -622,  -622,
    -622,   229,  1229,   867,   867,   867,   867,   867,   867,   867,
     867,   867,   867,   867,   867,   867,   867,   867,   867,   867,
     867,   867,   867,  -622,   867,   867,   867,   867,   867,   867,
     867,   867,   867,   867,   867,   867,  -622,   867,   867,   867,
     867,   867,  -622,  -622,  -622,   867,   867,  -622,   867,   867,
     386,  -622,  -622,   415,   417,   418,   426,   428,   439,   440,
     445,   446,   449,   456,   457,   461,   462,   468,   469,   470,
     473,   474,   480,   481,  1497,  -622,   482,   483,   484,   487,
     488,   493,   496,   497,   498,   499,   500,   504,   505,   510,
     511,  1497,  1497,   160,    34,    23,    33,  -622,  -622,  -622,
    -622,   867,   957,   142,  -622,  -622,  -622,  -622,  -622,  -622,
     393,   506,   516,   517,   518,   519,   522,   515,   529,   530,
     533,   531,   554,   583,   579,   582,   587,   588,   589,   591,
     596,   598,   594,   600,   601,   602,   603,   604,   611,   607,
     608,   610,   612,  -622,   616,   619,   615,   617,   631,   621,
     636,   624,   318,  -622,    89,   867,   867,   867,   867,   867,
     867,   867,   867,   867,   867,   867,   867,   867,   867,   867,
     867,   867,   867,   867,   867,   867,  -622,   867,   867,   867,
     867,   867,   867,   867,   867,   867,   867,   867,   867,   867,
     867,   867,    33,    33,  -622,  -622,  -622,  -622,  -622,  -622,
     867,   867,  1497,  1497,  -622,  -622,  1497,  1497,   867,   867,
    1497,  1497,  1497,  1497,  1497,  1497,  -622,   637,   867,  -622,
    -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,
    -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,
    -622,  -622,  -622,  -622,  -622,   513,  -622,  -622,  -622,  -622,
    -622,   513,  -622,  -622,  -622,  -622,   513,   513,   513,  -622,
    -622,  -622,  -622,  -622,   513,  -622,  -622,  -622,  -622,  -622,
    -622,  -622,   513,  -622,  -622,  -622,  -622,  -622,  -622,  -622,
    -622,  -622,  -622,  -622,  -622,  -622,  -622,   513,  -622,  -622,
    -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,
    -622,  -622,  -622,  -622,    40,  -622,  -622,  -622,  -622,  -622,
     629,  -622,   867,   867,   867,   867,   867,   867,   867,  -622,
     867,   867,  -622,   867,   369,  -622,   867,   867,   867,   867,
     867,  -622,   867,  -622,   867,  -622,   867,   867,   867,   867,
    -622,   867,   867,   867,   867,   644,  -622,  -622,   867,   867,
     867,  -622,  -622,  -622,  -622,  -622,   593,   334,   349,   864,
    1375,   399,   411,   423,   437,  1384,  1494,   454,   186,   466,
     478,  1539,  1547,  1576,  1584,  1592,  1600,  1608,  1616,  1628,
    1636,  1644,  1652,  1660,  1668,  1680,  1688,   528,  1696,   578,
     586,  1704,  1712,    30,    30,    33,    33,    33,    33,    30,
      30,  -622,  -622,  -622,  -622,  -622,  -622,   463,   867,   269,
    -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,  -622,   642,
     652,   653,   655,   656,   668,   657,   658,   659,   679,   684,
     680,   690,   691,   692,   693,   694,   695,   698,   700,   703,
     704,   710,   711,   712,   708,   867,  1359,   715,   718,   716,
    -622,  -622,  -622,   867,   867,   867,  -622,  -622,  -622,  -622,
     867,   867,  -622,  -622,   867,  -622,  -622,   867,   867,   867,
     867,   867,   867,   867,   867,   867,   867,   867,   867,   867,
     867,   867,   867,  -622,   867,  -622,  -622,   867,   867,    70,
     730,  -622,   268,   867,  -622,  -622,  -622,  -622,   867,  -622,
    -622,  -622,  -622,  -622,   867,  -622,  -622,  -622,  -622,    70,
    -622,  -622,  -622,  -622,    70,  -622,  -622,  -622,   867,   731,
    1229,  -622,  -622,   229,  -622,  -622,   867,  -622,   597,  1720,
    1732,   614,   623,  1740,   635,  1748,   654,   672,   681,   717,
     727,  1756,  1764,   737,   757,   766,  1772,  1784,   776,   951,
    1792,  1800,  1808,  -622,  -622,   734,  -622,  -622,  -622,   736,
     735,   738,   741,   740,   746,   747,   748,  -622,   751,  -622,
     479,  -622,   744,  -622,  -622,   867,   867,  -622,  -622,   867,
    -622,   867,  -622,  -622,  -622,  -622,  -622,   867,   867,  -622,
    -622,  -622,   867,   867,  -622,  -622,   867,   867,   867,    70,
    -622,   268,   867,  -622,   867,  -622,  -622,  -622,  1229,    57,
     626,  -622,   867,  1927,  1816,   961,   983,   991,  1824,  1836,
    1006,  1033,  1844,  1852,  1860,  -622,  -622,   752,   749,  -622,
    1229,   632,   754,  -622,   867,  -622,  -622,  -622,   867,   867,
    -622,  -622,   867,   867,   867,  -622,   867,  -622,  -622,  1229,
     867,   755,  1180,  1868,  1190,  1876,  1888,  1896,   762,  -622,
    -622,   777,  -622,  -622,   867,  -622,   867,   867,   867,   867,
    -622,   867,  1229,  1198,  1904,  1344,  1912,   778,  -622,   760,
    -622,  -622,   867,  -622,   867,   867,  -622,  -622,  -622,  1920,
    1353,   781,  -622,   867,  -622,   867,  -622,  1928,   789,  -622,
     867,   867,  -622,  1367,   790,  -622,  -622,   867,   791,   867,
     797,  -622
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     7,     0,     2,     6,     1,     6,     6,    25,     4,
       5,     0,    26,    27,    28,    29,    30,    24,    13,     0,
      18,     0,     8,     0,    19,    12,    15,    11,     0,    18,
       0,     0,    20,    22,    14,     0,     9,     0,    19,     0,
       0,     0,     0,   209,     0,     0,    23,    16,    21,     0,
     209,     0,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   259,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   294,     0,   209,   274,   295,   296,
       0,     3,     0,    11,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   209,     0,     0,   252,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     262,     0,     0,     0,     0,     0,     0,   254,   255,   253,
     211,     0,     0,   211,   297,   209,     0,    46,    33,    34,
      39,    40,    38,    43,    42,    44,    45,    41,    35,    36,
      37,   208,   209,     0,   217,   269,   271,   265,   273,    17,
      10,   267,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   209,   209,   211,   209,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   263,   209,   209,   209,
     209,   209,   212,   300,   302,   209,   209,   301,   209,   209,
       0,   162,   161,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   209,   164,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   209,   209,   165,   207,   140,   145,   152,   160,   163,
     209,   209,     0,   209,   209,   210,   266,   268,   272,   282,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   209,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   205,     0,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   166,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   209,   147,   146,   134,   135,   139,   138,   136,   137,
     209,   209,   209,   209,   133,   132,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,     0,   209,    56,
      57,    58,    59,    60,    61,    55,    65,    66,    67,    68,
      69,   105,    71,    72,    73,    74,    75,    76,    78,    79,
      84,    85,    86,    87,    88,     0,    90,    91,    53,    54,
      92,     0,    94,    95,    96,    97,     0,     0,     0,   101,
     102,   121,   106,   107,     0,   109,   110,   111,   112,   113,
     114,   115,     0,    77,   116,   117,   118,   120,   122,   123,
     124,   125,   126,   127,   128,   129,   130,     0,    49,    50,
      51,    52,    63,    64,    62,    70,   103,   104,    83,    81,
      80,    82,    48,    31,   165,   291,   209,   292,   293,   287,
       0,    32,   209,   209,   209,   209,   209,   209,   209,   238,
     209,   209,   246,   209,     0,   251,   209,   209,   209,   209,
     209,   258,   209,   219,   209,   239,   209,   209,   209,   209,
     218,   209,   209,   209,   209,     0,   211,   256,   209,   209,
     209,   240,   259,   298,   159,   204,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   141,   142,   150,   151,   148,   149,   144,
     143,   158,   155,   156,   153,   154,   157,     0,   209,     0,
      89,    93,    98,    99,   100,   108,   119,   131,    18,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   209,   209,     0,     0,     0,
     262,   206,   167,   209,   209,   209,   178,   191,   201,   171,
     209,   209,   203,   179,   209,   202,   170,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   209,   209,   184,   209,   199,   200,   209,   209,     0,
       0,    47,   209,   209,   241,   242,   226,   229,   209,   234,
     233,   235,   247,   250,   209,   222,   223,   236,   257,     0,
     228,   248,   249,   227,     0,   225,   224,   221,   209,     0,
     209,   209,   277,   267,   244,   245,   209,   263,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   214,   213,   216,   299,   220,   286,     0,
     285,     0,     0,     0,     0,     0,     0,   211,     0,   211,
     273,   209,     0,   209,   173,   209,   209,   174,   175,   209,
     189,   209,   168,   169,   181,   183,   182,   209,   209,   197,
     196,   195,   209,   209,   187,   186,   209,   209,   209,     0,
     288,   209,   209,   230,   209,   290,   289,   231,   209,   267,
       0,   211,   209,   209,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   215,   284,     0,     0,   209,
     209,     0,     0,   211,   209,   177,   180,   190,   209,   209,
     194,   188,   209,   209,   209,   232,   209,   211,   275,   209,
     209,     0,     0,     0,     0,     0,     0,     0,     0,   260,
     276,     0,   278,   176,   209,   198,   209,   209,   209,   209,
     261,   209,   209,     0,     0,     0,     0,     0,   264,     0,
     279,   193,   209,   192,   209,   209,   281,   243,   209,     0,
       0,     0,   211,   209,   172,   209,   260,     0,     0,   261,
     209,   209,   264,     0,     0,   280,   185,   209,     0,   209,
       0,   237
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -622,  -622,  -622,   152,  -622,  -622,   689,  -622,   780,   758,
     -27,   772,   763,  -622,  -622,   104,  -102,   750,  -622,  -622,
    -622,  -622,  -622,   197,  -329,  -118,  -170,  -622,  -622,  -622,
     -94,  -287,   -43,  -622,   -75,  -148,  -621,    22,  -622,   274,
     -66,   -64,   190,   109,   -63,  -105,   123,   201,   -46,  -189,
    -622,  -622,  -622,    48,  -622,  -622,   545,   550,  -622,   208
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,     8,     4,    28,    36,    22,    25,    26,
      24,    31,    32,    33,    18,    19,   283,   492,   493,   398,
     399,   390,   391,   284,   285,   286,   287,   288,   289,   344,
     748,   182,   183,   501,   234,   744,   745,   746,   104,   150,
     860,   868,   226,   333,   876,   297,   298,    44,   105,   106,
     107,   862,   878,   749,   750,   108,   184,   109,   499,   110
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     103,   181,    38,   299,   114,   188,   496,   103,   233,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,  -208,   151,   152,   153,   154,   155,   156,
     157,   158,   159,   160,   161,   162,   163,   164,   165,   166,
     186,   583,   584,   103,     1,   400,   392,   187,     5,   589,
     590,   290,   393,   392,    11,   401,   394,   395,   754,   393,
      20,   402,   296,   755,   232,   384,   385,    21,   237,   386,
     387,   388,   389,   213,   366,   545,     6,     7,   546,   300,
     301,   302,   303,   304,   305,   306,   307,   308,   309,   310,
     311,   312,   313,   314,   315,   316,   317,   318,   319,    23,
     321,   322,   323,   324,   325,   326,   327,   328,   329,   330,
     331,   332,    27,   334,   335,   336,   337,   338,   320,   294,
     743,   339,   340,    46,   341,   239,    49,   240,    29,   103,
      12,    13,    14,    15,    16,   241,    37,   242,     9,    10,
      30,    35,    39,   382,   383,   243,   244,   245,   246,   247,
     248,   249,   250,   251,    40,   252,   253,   254,   255,   256,
     257,   258,   259,   260,    41,  -269,   261,   262,   167,   263,
     653,   494,   264,    42,    43,   654,   111,   407,    47,   495,
     112,   113,   265,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   115,   177,   178,   179,   180,   266,   267,   268,
     269,   270,   271,   272,   273,   274,   275,   276,   277,   278,
     591,   592,   593,   594,   595,   596,   185,   384,   385,   187,
     191,   386,   387,   388,   389,   189,   193,   406,   192,    72,
     194,   500,   396,   397,   195,   196,   197,    80,   198,   396,
     397,   279,   199,   200,   403,   404,   405,   201,   202,   280,
     203,   239,  -283,   240,   585,   586,   292,   681,   587,   588,
     204,   241,   205,   242,    12,    13,    14,    15,    16,   206,
     535,   243,   244,   245,   246,   247,   248,   249,   250,   251,
     207,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     208,   209,   261,   262,   167,   263,   210,   211,   264,   212,
     384,   385,   544,   214,   386,   387,   388,   389,   265,   168,
     169,   170,   171,   172,   173,   174,   175,   176,   642,   177,
     178,   179,   180,   266,   267,   268,   269,   270,   271,   272,
     273,   274,   275,   276,   277,   278,   215,   216,   643,   217,
     218,   219,   220,   597,   221,   222,   619,   223,   224,   384,
     385,   281,   282,   386,   387,   388,   389,   225,   227,   228,
     229,   230,   231,   232,   235,   384,   385,   279,   236,   386,
     387,   388,   389,   238,   291,   280,   292,   293,   295,   343,
     384,   385,   502,   646,   386,   387,   388,   389,   609,   610,
     611,   612,   613,   614,   615,   647,   616,   617,   345,   618,
     346,   347,   620,   621,   622,   623,   624,   648,   625,   348,
     626,   349,   627,   628,   629,   630,   342,   631,   632,   633,
     634,   649,   350,   351,   637,   638,   639,   712,   352,   353,
     384,   385,   354,   294,   386,   387,   388,   389,   652,   355,
     356,   636,   384,   385,   357,   358,   386,   387,   388,   389,
     655,   359,   360,   361,   384,   385,   362,   363,   386,   387,
     388,   389,   656,   364,   365,   367,   368,   369,   384,   385,
     370,   371,   386,   387,   388,   389,   372,   281,   282,   373,
     374,   375,   376,   377,   680,   384,   385,   378,   379,   386,
     387,   388,   389,   380,   381,   503,   823,   384,   385,   509,
     408,   386,   387,   388,   389,   504,   505,   506,   507,   384,
     385,   508,   673,   386,   387,   388,   389,   512,   510,   511,
     513,   709,   547,   548,   549,   550,   551,   552,   553,   554,
     555,   556,   557,   558,   559,   560,   561,   562,   563,   564,
     565,   566,   567,   514,   568,   569,   570,   571,   572,   573,
     574,   575,   576,   577,   578,   579,   580,   581,   582,   384,
     385,   682,   675,   386,   387,   388,   389,   515,   516,   751,
     676,   517,   679,   103,   752,   521,   518,   519,   520,   522,
     753,   764,   523,   524,   525,   599,   641,  -268,   760,   819,
     526,   527,   528,   529,   756,   530,   531,   532,   767,   533,
     536,   534,   762,   537,   538,   541,   539,   768,   543,   384,
     385,   838,   608,   386,   387,   388,   389,   384,   385,   770,
     540,   386,   387,   388,   389,   542,   598,   635,   384,   385,
     850,   683,   386,   387,   388,   389,   684,   685,   772,   686,
     687,   689,   690,   691,   758,   384,   385,   103,   759,   386,
     387,   388,   389,   870,   384,   385,   773,   688,   386,   387,
     388,   389,   798,   692,   800,   774,   384,   385,   693,   694,
     386,   387,   388,   389,   695,   696,   697,   698,   817,   700,
     818,   181,   701,   699,   702,   384,   385,   703,   822,   386,
     387,   388,   389,   704,   705,   706,   707,   708,   801,   714,
     803,   775,   715,   384,   385,   716,   821,   386,   387,   388,
     389,   776,   384,   385,   747,   757,   386,   387,   388,   389,
     790,   779,   848,   789,   791,   793,   851,   792,   841,   794,
     795,   796,   797,   802,   820,   103,   835,   799,   836,   852,
     839,   780,   849,   840,   877,   867,    17,   869,   384,   385,
     781,   859,   386,   387,   388,   389,   837,   103,   384,   385,
     784,   881,   386,   387,   388,   389,   861,   875,   384,   385,
     885,   888,   386,   387,   388,   389,   103,   894,   891,   897,
     899,   901,   190,   898,    48,   900,    34,   886,   384,   385,
      45,   815,   386,   387,   388,   389,   640,   384,   385,   103,
     889,   386,   387,   388,   389,   892,   763,   384,   385,   895,
     717,   386,   387,   388,   389,   882,   761,   711,   497,   816,
     718,   719,   720,   498,   713,     0,     0,   721,   722,     0,
       0,   723,     0,     0,   724,   725,   726,   727,   728,   729,
     730,   731,   732,   733,   734,   735,   736,   737,   738,   739,
     239,   740,   240,   644,   741,   742,     0,     0,     0,     0,
     241,     0,   242,     0,     0,     0,     0,     0,     0,     0,
     243,   244,   245,   246,   247,   248,   249,   250,   251,     0,
     252,   253,   254,   255,   256,   257,   258,   259,   260,     0,
       0,   261,   262,   167,   263,   384,   385,   264,     0,   386,
     387,   388,   389,     0,     0,     0,     0,   265,   168,   169,
     170,   171,   172,   173,   174,   175,   176,     0,   177,   178,
     179,   180,   266,   267,   268,   269,   270,   271,   272,   273,
     274,   275,   276,   277,   278,   785,     0,     0,     0,     0,
       0,     0,   804,   805,   408,   825,   806,     0,   807,     0,
       0,     0,     0,     0,   808,   809,     0,     0,     0,   810,
     811,     0,     0,   812,   813,   814,   279,   826,     0,     0,
       0,     0,     0,     0,   280,   827,     0,     0,     0,     0,
       0,     0,   384,   385,     0,     0,   386,   387,   388,   389,
     830,     0,   384,   385,     0,     0,   386,   387,   388,   389,
       0,   842,     0,     0,     0,   843,   844,     0,     0,   845,
     846,   847,     0,     0,   384,   385,     0,   831,   386,   387,
     388,   389,   384,   385,     0,     0,   386,   387,   388,   389,
       0,   863,     0,   864,   865,   866,     0,   384,   385,     0,
       0,   386,   387,   388,   389,     0,     0,     0,     0,   879,
       0,   880,     0,     0,     0,     0,     0,     0,     0,     0,
     887,     0,     0,     0,   384,   385,     0,   893,   386,   387,
     388,   389,     0,     0,     0,     0,   281,   282,   409,   410,
     411,   412,   413,   414,   415,   416,   417,   418,   419,   420,
     421,   422,   423,   424,   425,   426,   427,   428,   429,   430,
     431,   432,   433,   434,   435,   436,   437,   438,   439,   440,
     441,   442,   443,   444,   445,   446,   447,   448,   449,   450,
     451,   452,   453,   454,   455,   456,   457,   458,   459,   460,
     461,   462,   463,   464,   465,   466,   467,   468,   469,   470,
     471,   472,   473,   474,   475,   476,   477,   478,   479,   480,
     481,   482,   483,   484,   485,   486,   487,   488,   489,   490,
     491,    50,  -270,     0,   853,   600,     0,    51,     0,     0,
       0,   601,     0,     0,   855,     0,   602,   603,   604,     0,
       0,     0,   871,     0,   605,     0,     0,     0,     0,     0,
       0,     0,   606,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   607,     0,     0,
       0,   384,   385,     0,    50,   386,   387,   388,   389,     0,
      51,   384,   385,     0,     0,   386,   387,   388,   389,   384,
     385,    52,     0,   386,   387,   388,   389,     0,     0,     0,
       0,     0,     0,     0,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,     0,    84,    85,    86,    87,
      88,    89,    90,    91,    52,    92,    93,    94,    95,    96,
      97,     0,    98,    99,   100,   101,   102,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,   873,    84,
      85,    86,    87,    88,    89,    90,    91,   884,    92,    93,
      94,    95,    96,    97,   710,    98,    99,   100,   101,   102,
      51,   896,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   645,     0,     0,     0,     0,     0,
       0,     0,     0,   650,     0,   384,   385,     0,     0,   386,
     387,   388,   389,     0,   384,   385,     0,     0,   386,   387,
     388,   389,     0,     0,     0,     0,     0,     0,   384,   385,
       0,     0,   386,   387,   388,   389,   384,   385,     0,     0,
     386,   387,   388,   389,    52,   384,   385,     0,     0,   386,
     387,   388,   389,     0,     0,     0,     0,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,     0,    84,
      85,    86,    87,    88,    89,    90,    91,     0,    92,    93,
      94,    95,    96,    97,     0,    98,    99,   100,   101,   102,
     239,     0,   240,   651,     0,     0,     0,     0,     0,     0,
     241,     0,   242,     0,     0,     0,     0,     0,     0,     0,
     243,   244,   245,   246,   247,   248,   249,   250,   251,     0,
     252,   253,   254,   255,   256,   257,   258,   259,   260,     0,
       0,   261,   262,   167,   263,   384,   385,   264,   657,   386,
     387,   388,   389,     0,     0,     0,   658,   265,   168,   169,
     170,   171,   172,   173,   174,   175,   176,     0,   177,   178,
     179,   180,   266,   267,   268,   269,   270,   271,   272,   273,
     274,   275,   276,   277,   278,   659,     0,     0,     0,     0,
     384,   385,     0,   660,   386,   387,   388,   389,   384,   385,
       0,   661,   386,   387,   388,   389,     0,     0,     0,   662,
       0,     0,     0,     0,     0,     0,   279,   663,     0,     0,
       0,     0,     0,     0,   280,   664,     0,   384,   385,     0,
       0,   386,   387,   388,   389,   384,   385,   665,     0,   386,
     387,   388,   389,   384,   385,   666,     0,   386,   387,   388,
     389,   384,   385,   667,     0,   386,   387,   388,   389,   384,
     385,   668,     0,   386,   387,   388,   389,   384,   385,   669,
       0,   386,   387,   388,   389,     0,     0,   670,     0,   384,
     385,     0,     0,   386,   387,   388,   389,   384,   385,   671,
       0,   386,   387,   388,   389,   384,   385,   672,     0,   386,
     387,   388,   389,   384,   385,   674,     0,   386,   387,   388,
     389,   384,   385,   677,     0,   386,   387,   388,   389,   384,
     385,   678,     0,   386,   387,   388,   389,     0,     0,   765,
       0,   384,   385,     0,     0,   386,   387,   388,   389,   384,
     385,   766,     0,   386,   387,   388,   389,   384,   385,   769,
       0,   386,   387,   388,   389,   384,   385,   771,     0,   386,
     387,   388,   389,   384,   385,   777,     0,   386,   387,   388,
     389,   384,   385,   778,     0,   386,   387,   388,   389,     0,
       0,   782,     0,   384,   385,     0,     0,   386,   387,   388,
     389,   384,   385,   783,     0,   386,   387,   388,   389,   384,
     385,   786,     0,   386,   387,   388,   389,   384,   385,   787,
       0,   386,   387,   388,   389,   384,   385,   788,     0,   386,
     387,   388,   389,   384,   385,   824,     0,   386,   387,   388,
     389,     0,     0,   828,     0,   384,   385,     0,     0,   386,
     387,   388,   389,   384,   385,   829,     0,   386,   387,   388,
     389,   384,   385,   832,     0,   386,   387,   388,   389,   384,
     385,   833,     0,   386,   387,   388,   389,   384,   385,   834,
       0,   386,   387,   388,   389,   384,   385,   854,     0,   386,
     387,   388,   389,     0,     0,   856,     0,   384,   385,     0,
       0,   386,   387,   388,   389,   384,   385,   857,     0,   386,
     387,   388,   389,   384,   385,   858,     0,   386,   387,   388,
     389,   384,   385,   872,     0,   386,   387,   388,   389,   384,
     385,   874,     0,   386,   387,   388,   389,   384,   385,   883,
       0,   386,   387,   388,   389,     0,     0,   890,     0,   384,
     385,     0,     0,   386,   387,   388,   389,   384,   385,     0,
       0,   386,   387,   388,   389,   384,   385,     0,     0,   386,
     387,   388,   389,   384,   385,     0,     0,   386,   387,   388,
     389,   384,   385,   167,     0,   386,   387,   388,   389,   384,
     385,     0,     0,   386,   387,   388,   389,     0,   168,   169,
     170,   171,   172,   173,   174,   175,   176,     0,   177,   178,
     179,   180
};

static const yytype_int16 yycheck[] =
{
      43,   103,    29,   192,    50,   110,   293,    50,   156,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,     3,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     106,   390,   391,   106,    17,    32,    43,    10,     0,   398,
     399,   165,    49,    43,    14,    42,    53,    54,   699,    49,
       3,    48,   187,   704,    14,    51,    52,    20,   163,    55,
      56,    57,    58,   136,   264,     6,    21,    22,     9,   193,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,    14,
     214,   215,   216,   217,   218,   219,   220,   221,   222,   223,
     224,   225,    16,   227,   228,   229,   230,   231,   213,   182,
      70,   235,   236,    39,   238,     3,    42,     5,     3,   192,
     224,   225,   226,   227,   228,    13,    18,    15,     6,     7,
      14,    14,    11,   281,   282,    23,    24,    25,    26,    27,
      28,    29,    30,    31,     4,    33,    34,    35,    36,    37,
      38,    39,    40,    41,     9,   128,    44,    45,    46,    47,
       4,   293,    50,    11,     5,     9,    19,   291,    10,   293,
       4,    10,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    14,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
     400,   401,   402,   403,   404,   405,     6,    51,    52,    10,
       6,    55,    56,    57,    58,    10,     3,   290,    11,   107,
       3,   294,   229,   230,     3,     3,     3,   115,     3,   229,
     230,   119,     3,     3,   231,   232,   233,     3,     3,   127,
       3,     3,     4,     5,   392,   393,   236,     8,   396,   397,
       3,    13,     3,    15,   224,   225,   226,   227,   228,     3,
     333,    23,    24,    25,    26,    27,    28,    29,    30,    31,
       3,    33,    34,    35,    36,    37,    38,    39,    40,    41,
       3,     3,    44,    45,    46,    47,     3,     3,    50,     3,
      51,    52,     4,     3,    55,    56,    57,    58,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,     4,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,     3,     3,     9,     3,
       3,     3,     3,   406,     3,     3,   514,     3,     3,    51,
      52,   229,   230,    55,    56,    57,    58,     3,     3,     3,
       3,     3,     3,    14,     3,    51,    52,   119,     3,    55,
      56,    57,    58,     3,     3,   127,   236,    59,    14,    13,
      51,    52,     9,     4,    55,    56,    57,    58,   502,   503,
     504,   505,   506,   507,   508,     4,   510,   511,     3,   513,
       3,     3,   516,   517,   518,   519,   520,     4,   522,     3,
     524,     3,   526,   527,   528,   529,   239,   531,   532,   533,
     534,     4,     3,     3,   538,   539,   540,   636,     3,     3,
      51,    52,     3,   496,    55,    56,    57,    58,     4,     3,
       3,   536,    51,    52,     3,     3,    55,    56,    57,    58,
       4,     3,     3,     3,    51,    52,     3,     3,    55,    56,
      57,    58,     4,     3,     3,     3,     3,     3,    51,    52,
       3,     3,    55,    56,    57,    58,     3,   229,   230,     3,
       3,     3,     3,     3,   598,    51,    52,     3,     3,    55,
      56,    57,    58,     3,     3,     9,   803,    51,    52,     4,
       7,    55,    56,    57,    58,     9,     9,     9,     9,    51,
      52,     9,     4,    55,    56,    57,    58,     4,     9,     9,
       9,   635,   345,   346,   347,   348,   349,   350,   351,   352,
     353,   354,   355,   356,   357,   358,   359,   360,   361,   362,
     363,   364,   365,     9,   367,   368,   369,   370,   371,   372,
     373,   374,   375,   376,   377,   378,   379,   380,   381,    51,
      52,   608,     4,    55,    56,    57,    58,     4,     9,   683,
       4,     9,   129,   636,   688,     4,     9,     9,     9,     3,
     694,     4,     4,     9,     4,   408,    13,   128,   713,   798,
       9,     9,     9,     9,   708,     4,     9,     9,     4,     9,
       4,     9,   716,     4,     9,     4,     9,     4,     4,    51,
      52,   820,     3,    55,    56,    57,    58,    51,    52,     4,
       9,    55,    56,    57,    58,     9,     9,     3,    51,    52,
     839,     9,    55,    56,    57,    58,     4,     4,     4,     4,
       4,     4,     4,     4,   710,    51,    52,   710,   711,    55,
      56,    57,    58,   862,    51,    52,     4,     9,    55,    56,
      57,    58,   757,     4,   759,     4,    51,    52,     4,     9,
      55,    56,    57,    58,     4,     4,     4,     4,   792,     4,
     794,   803,     4,     9,     4,    51,    52,     4,   802,    55,
      56,    57,    58,     9,     4,     4,     4,     9,   761,     4,
     763,     4,     4,    51,    52,     9,   801,    55,    56,    57,
      58,     4,    51,    52,     4,     4,    55,    56,    57,    58,
       4,     4,   836,     9,     9,     4,   840,     9,   823,     9,
       4,     4,     4,     9,   128,   798,     4,     6,     9,     4,
     128,     4,   837,     9,     4,   859,     8,   861,    51,    52,
       4,     9,    55,    56,    57,    58,   819,   820,    51,    52,
       4,   875,    55,    56,    57,    58,     9,     9,    51,    52,
       9,   885,    55,    56,    57,    58,   839,   891,     9,     9,
       9,     4,   113,   897,    41,   899,    26,   882,    51,    52,
      38,   789,    55,    56,    57,    58,   542,    51,    52,   862,
     886,    55,    56,    57,    58,   889,   717,    51,    52,   892,
     640,    55,    56,    57,    58,   878,   713,   636,   293,   791,
     643,   644,   645,   293,   636,    -1,    -1,   650,   651,    -1,
      -1,   654,    -1,    -1,   657,   658,   659,   660,   661,   662,
     663,   664,   665,   666,   667,   668,   669,   670,   671,   672,
       3,   674,     5,     9,   677,   678,    -1,    -1,    -1,    -1,
      13,    -1,    15,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    -1,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    -1,
      -1,    44,    45,    46,    47,    51,    52,    50,    -1,    55,
      56,    57,    58,    -1,    -1,    -1,    -1,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    -1,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,     4,    -1,    -1,    -1,    -1,
      -1,    -1,   765,   766,     7,     4,   769,    -1,   771,    -1,
      -1,    -1,    -1,    -1,   777,   778,    -1,    -1,    -1,   782,
     783,    -1,    -1,   786,   787,   788,   119,     4,    -1,    -1,
      -1,    -1,    -1,    -1,   127,     4,    -1,    -1,    -1,    -1,
      -1,    -1,    51,    52,    -1,    -1,    55,    56,    57,    58,
       4,    -1,    51,    52,    -1,    -1,    55,    56,    57,    58,
      -1,   824,    -1,    -1,    -1,   828,   829,    -1,    -1,   832,
     833,   834,    -1,    -1,    51,    52,    -1,     4,    55,    56,
      57,    58,    51,    52,    -1,    -1,    55,    56,    57,    58,
      -1,   854,    -1,   856,   857,   858,    -1,    51,    52,    -1,
      -1,    55,    56,    57,    58,    -1,    -1,    -1,    -1,   872,
      -1,   874,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     883,    -1,    -1,    -1,    51,    52,    -1,   890,    55,    56,
      57,    58,    -1,    -1,    -1,    -1,   229,   230,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   154,   155,   156,   157,   158,   159,   160,   161,   162,
     163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,   190,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,     5,     6,    -1,     4,   435,    -1,    11,    -1,    -1,
      -1,   441,    -1,    -1,     4,    -1,   446,   447,   448,    -1,
      -1,    -1,     4,    -1,   454,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   462,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   477,    -1,    -1,
      -1,    51,    52,    -1,     5,    55,    56,    57,    58,    -1,
      11,    51,    52,    -1,    -1,    55,    56,    57,    58,    51,
      52,    75,    -1,    55,    56,    57,    58,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,    -1,   120,   121,   122,   123,
     124,   125,   126,   127,    75,   129,   130,   131,   132,   133,
     134,    -1,   136,   137,   138,   139,   140,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,     4,   120,
     121,   122,   123,   124,   125,   126,   127,     4,   129,   130,
     131,   132,   133,   134,     5,   136,   137,   138,   139,   140,
      11,     4,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     9,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     9,    -1,    51,    52,    -1,    -1,    55,
      56,    57,    58,    -1,    51,    52,    -1,    -1,    55,    56,
      57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    51,    52,
      -1,    -1,    55,    56,    57,    58,    51,    52,    -1,    -1,
      55,    56,    57,    58,    75,    51,    52,    -1,    -1,    55,
      56,    57,    58,    -1,    -1,    -1,    -1,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,    -1,   120,
     121,   122,   123,   124,   125,   126,   127,    -1,   129,   130,
     131,   132,   133,   134,    -1,   136,   137,   138,   139,   140,
       3,    -1,     5,     9,    -1,    -1,    -1,    -1,    -1,    -1,
      13,    -1,    15,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    -1,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    -1,
      -1,    44,    45,    46,    47,    51,    52,    50,     9,    55,
      56,    57,    58,    -1,    -1,    -1,     9,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    -1,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,     9,    -1,    -1,    -1,    -1,
      51,    52,    -1,     9,    55,    56,    57,    58,    51,    52,
      -1,     9,    55,    56,    57,    58,    -1,    -1,    -1,     9,
      -1,    -1,    -1,    -1,    -1,    -1,   119,     9,    -1,    -1,
      -1,    -1,    -1,    -1,   127,     9,    -1,    51,    52,    -1,
      -1,    55,    56,    57,    58,    51,    52,     9,    -1,    55,
      56,    57,    58,    51,    52,     9,    -1,    55,    56,    57,
      58,    51,    52,     9,    -1,    55,    56,    57,    58,    51,
      52,     9,    -1,    55,    56,    57,    58,    51,    52,     9,
      -1,    55,    56,    57,    58,    -1,    -1,     9,    -1,    51,
      52,    -1,    -1,    55,    56,    57,    58,    51,    52,     9,
      -1,    55,    56,    57,    58,    51,    52,     9,    -1,    55,
      56,    57,    58,    51,    52,     9,    -1,    55,    56,    57,
      58,    51,    52,     9,    -1,    55,    56,    57,    58,    51,
      52,     9,    -1,    55,    56,    57,    58,    -1,    -1,     9,
      -1,    51,    52,    -1,    -1,    55,    56,    57,    58,    51,
      52,     9,    -1,    55,    56,    57,    58,    51,    52,     9,
      -1,    55,    56,    57,    58,    51,    52,     9,    -1,    55,
      56,    57,    58,    51,    52,     9,    -1,    55,    56,    57,
      58,    51,    52,     9,    -1,    55,    56,    57,    58,    -1,
      -1,     9,    -1,    51,    52,    -1,    -1,    55,    56,    57,
      58,    51,    52,     9,    -1,    55,    56,    57,    58,    51,
      52,     9,    -1,    55,    56,    57,    58,    51,    52,     9,
      -1,    55,    56,    57,    58,    51,    52,     9,    -1,    55,
      56,    57,    58,    51,    52,     9,    -1,    55,    56,    57,
      58,    -1,    -1,     9,    -1,    51,    52,    -1,    -1,    55,
      56,    57,    58,    51,    52,     9,    -1,    55,    56,    57,
      58,    51,    52,     9,    -1,    55,    56,    57,    58,    51,
      52,     9,    -1,    55,    56,    57,    58,    51,    52,     9,
      -1,    55,    56,    57,    58,    51,    52,     9,    -1,    55,
      56,    57,    58,    -1,    -1,     9,    -1,    51,    52,    -1,
      -1,    55,    56,    57,    58,    51,    52,     9,    -1,    55,
      56,    57,    58,    51,    52,     9,    -1,    55,    56,    57,
      58,    51,    52,     9,    -1,    55,    56,    57,    58,    51,
      52,     9,    -1,    55,    56,    57,    58,    51,    52,     9,
      -1,    55,    56,    57,    58,    -1,    -1,     9,    -1,    51,
      52,    -1,    -1,    55,    56,    57,    58,    51,    52,    -1,
      -1,    55,    56,    57,    58,    51,    52,    -1,    -1,    55,
      56,    57,    58,    51,    52,    -1,    -1,    55,    56,    57,
      58,    51,    52,    46,    -1,    55,    56,    57,    58,    51,
      52,    -1,    -1,    55,    56,    57,    58,    -1,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    -1,    71,    72,
      73,    74
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,    17,   238,   239,   241,     0,    21,    22,   240,   240,
     240,    14,   224,   225,   226,   227,   228,   246,   251,   252,
       3,    20,   244,    14,   247,   245,   246,    16,   242,     3,
      14,   248,   249,   250,   245,    14,   243,    18,   247,    11,
       4,     9,    11,     5,   284,   248,   252,    10,   249,   252,
       5,    11,    75,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   120,   121,   122,   123,   124,   125,
     126,   127,   129,   130,   131,   132,   133,   134,   136,   137,
     138,   139,   140,   269,   275,   285,   286,   287,   292,   294,
     296,    19,     4,    10,   285,    14,   269,   269,   269,   269,
     269,   269,   269,   269,   269,   269,   269,   269,   269,   269,
     269,   269,   269,   269,   269,   269,   269,   269,   269,   269,
     269,   269,   269,   269,   269,   269,   269,   269,   269,   269,
     276,   269,   269,   269,   269,   269,   269,   269,   269,   269,
     269,   269,   269,   269,   269,   269,   269,    46,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    71,    72,    73,
      74,   253,   268,   269,   293,     6,   285,    10,   282,    10,
     243,     6,    11,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,   269,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,   279,     3,     3,     3,
       3,     3,    14,   272,   271,     3,     3,   271,     3,     3,
       5,    13,    15,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    44,    45,    47,    50,    60,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,   119,
     127,   229,   230,   253,   260,   261,   262,   263,   264,   265,
     267,     3,   236,    59,   269,    14,   282,   282,   283,   286,
     267,   267,   267,   267,   267,   267,   267,   267,   267,   267,
     267,   267,   267,   267,   267,   267,   267,   267,   267,   267,
     271,   267,   267,   267,   267,   267,   267,   267,   267,   267,
     267,   267,   267,   280,   267,   267,   267,   267,   267,   267,
     267,   267,   260,    13,   266,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,   263,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,   262,   262,    51,    52,    55,    56,    57,    58,
     258,   259,    43,    49,    53,    54,   229,   230,   256,   257,
      32,    42,    48,   231,   232,   233,   269,   267,     7,   141,
     142,   143,   144,   145,   146,   147,   148,   149,   150,   151,
     152,   153,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   187,   188,   189,   190,   191,
     192,   193,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   254,   255,   253,   267,   268,   293,   294,   295,
     269,   270,     9,     9,     9,     9,     9,     9,     9,     4,
       9,     9,     4,     9,     9,     4,     9,     9,     9,     9,
       9,     4,     3,     4,     9,     4,     9,     9,     9,     9,
       4,     9,     9,     9,     9,   269,     4,     4,     9,     9,
       9,     4,     9,     4,     4,     6,     9,   260,   260,   260,
     260,   260,   260,   260,   260,   260,   260,   260,   260,   260,
     260,   260,   260,   260,   260,   260,   260,   260,   260,   260,
     260,   260,   260,   260,   260,   260,   260,   260,   260,   260,
     260,   260,   260,   261,   261,   262,   262,   262,   262,   261,
     261,   263,   263,   263,   263,   263,   263,   269,     9,   260,
     254,   254,   254,   254,   254,   254,   254,   254,     3,   267,
     267,   267,   267,   267,   267,   267,   267,   267,   267,   272,
     267,   267,   267,   267,   267,   267,   267,   267,   267,   267,
     267,   267,   267,   267,   267,     3,   271,   267,   267,   267,
     276,    13,     4,     9,     9,     9,     4,     4,     4,     4,
       9,     9,     4,     4,     9,     4,     4,     9,     9,     9,
       9,     9,     9,     9,     9,     9,     9,     9,     9,     9,
       9,     9,     9,     4,     9,     4,     4,     9,     9,   129,
     267,     8,   247,     9,     4,     4,     4,     4,     9,     4,
       4,     4,     4,     4,     9,     4,     4,     4,     4,     9,
       4,     4,     4,     4,     9,     4,     4,     4,     9,   267,
       5,   284,   286,   296,     4,     4,     9,   279,   260,   260,
     260,   260,   260,   260,   260,   260,   260,   260,   260,   260,
     260,   260,   260,   260,   260,   260,   260,   260,   260,   260,
     260,   260,   260,    70,   272,   273,   274,     4,   267,   290,
     291,   267,   267,   267,   273,   273,   267,     4,   285,   269,
     282,   283,   267,   280,     4,     9,     9,     4,     4,     9,
       4,     9,     4,     4,     4,     4,     4,     9,     9,     4,
       4,     4,     9,     9,     4,     4,     9,     9,     9,     9,
       4,     9,     9,     4,     9,     4,     4,     4,   271,     6,
     271,   269,     9,   269,   260,   260,   260,   260,   260,   260,
     260,   260,   260,   260,   260,   274,   290,   267,   267,   286,
     128,   271,   267,   268,     9,     4,     4,     4,     9,     9,
       4,     4,     9,     9,     9,     4,     9,   269,   286,   128,
       9,   271,   260,   260,   260,   260,   260,   260,   267,   271,
     286,   267,     4,     4,     9,     4,     9,     9,     9,     9,
     277,     9,   288,   260,   260,   260,   260,   267,   278,   267,
     286,     4,     9,     4,     9,     9,   281,     4,   289,   260,
     260,   267,   269,     9,     4,     9,   271,   260,   267,   277,
       9,     9,   278,   260,   267,   281,     4,     9,   267,     9,
     267,     4
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   237,   238,   239,   240,   240,   240,   241,   242,   242,
     243,   243,   244,   244,   245,   245,   246,   246,   247,   248,
     248,   249,   249,   250,   251,   251,   252,   252,   252,   252,
     252,   253,   253,   253,   253,   253,   253,   253,   253,   253,
     253,   253,   253,   253,   253,   253,   253,   254,   255,   255,
     255,   255,   255,   255,   255,   255,   255,   255,   255,   255,
     255,   255,   255,   255,   255,   255,   255,   255,   255,   255,
     255,   255,   255,   255,   255,   255,   255,   255,   255,   255,
     255,   255,   255,   255,   255,   255,   255,   255,   255,   255,
     255,   255,   255,   255,   255,   255,   255,   255,   255,   255,
     255,   255,   255,   255,   255,   255,   255,   255,   255,   255,
     255,   255,   255,   255,   255,   255,   255,   255,   255,   255,
     255,   255,   255,   255,   255,   255,   255,   255,   255,   255,
     255,   255,   256,   257,   258,   258,   259,   259,   259,   259,
     260,   260,   260,   260,   260,   261,   261,   261,   261,   261,
     261,   261,   262,   262,   262,   262,   262,   262,   262,   263,
     263,   263,   263,   263,   263,   263,   263,   264,   264,   264,
     264,   264,   264,   264,   264,   264,   264,   264,   264,   264,
     264,   264,   264,   264,   264,   264,   264,   264,   264,   264,
     264,   264,   264,   264,   264,   264,   264,   264,   264,   264,
     264,   264,   264,   264,   265,   266,   266,   267,   268,   269,
     270,   271,   272,   273,   273,   274,   274,   275,   275,   275,
     275,   275,   275,   275,   275,   275,   275,   275,   275,   275,
     275,   275,   275,   275,   275,   275,   275,   275,   275,   275,
     275,   275,   275,   275,   275,   275,   275,   275,   275,   275,
     275,   275,   275,   275,   275,   275,   275,   275,   275,   276,
     277,   278,   279,   280,   281,   282,   282,   283,   283,   284,
     285,   285,   286,   286,   286,   287,   287,   287,   288,   289,
     287,   287,   287,   290,   290,   290,   291,   292,   293,   294,
     294,   295,   295,   295,   296,   296,   296,   296,   296,   296,
     296,   296,   296
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     9,     2,     2,     0,     0,     0,     2,
       5,     0,     2,     0,     2,     1,     6,     7,     0,     0,
       1,     3,     1,     3,     1,     0,     1,     1,     1,     1,
       1,     3,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     2,     1,     1,     1,     1,     2,     2,
       2,     1,     1,     1,     1,     1,     1,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     3,     3,     1,     2,     2,     3,     3,
       3,     3,     1,     3,     3,     3,     3,     3,     3,     3,
       1,     1,     1,     1,     1,     1,     2,     4,     6,     6,
       4,     4,    14,     6,     6,     6,    10,     8,     4,     4,
       8,     6,     6,     6,     4,    18,     6,     6,     8,     6,
       8,     4,    12,    12,     8,     6,     6,     6,    10,     4,
       4,     4,     4,     4,     3,     1,     3,     1,     1,     0,
       0,     0,     1,     1,     1,     3,     1,     2,     5,     5,
       7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
       9,     9,    11,     7,     7,     7,     7,    25,     5,     5,
       5,     7,     7,    15,     7,     7,     5,     7,     7,     7,
       7,     5,     2,     2,     2,     2,     5,     7,     5,     0,
       0,     0,     0,     0,     0,     1,     2,     0,     1,     3,
       1,     2,     4,     2,     1,    11,    12,     7,     0,     0,
      20,    15,     4,     0,     3,     1,     1,     4,     7,     9,
       9,     1,     1,     1,     1,     1,     1,     2,     5,     7,
       3,     3,     3
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return (YYSIZE_T) (yystpcpy (yyres, yystr) - yyres);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yynewstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  *yyssp = (yytype_int16) yystate;

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = (YYSIZE_T) (yyssp - yyss + 1);

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2:
#line 256 "dilpar.y"
    {
            ubit8 *b;
            FILE *f;

#ifdef MARCEL
            sprintf(tmpfname, "/tmp/dil.XX");
            if (tmpfname != mktemp(tmpfname)) {
               perror("mktemp");
               exit(1);
            }
#else
            tmpnam(tmpfname);
#endif

            if ((f = fopen(tmpfname, "wb")) == NULL)
               fatal("Couldn't write final code.");

            /* truncate core to used size ! */
            tmpl.coresz = wcore - tmpl.core + 1;

#ifndef DEMO_VERSION
	    CByteBuffer *pBuf = &g_FileBuffer;
	    pBuf->Clear();

	    {
	       char zBuf[500], nBuf[500];

	       split_fi_ref(prg.sp->tmpl->prgname, zBuf, nBuf);

	       if (!istemplate)
	       {
		  fprintf(stderr," prg : %-20s ", nBuf);

		  bwrite_dil(pBuf, &prg);
		  /* dumpdil(&prg); */
	       }
	       else
	       {
		  fprintf(stderr," tmpl: %-20s ", nBuf);

		  bwrite_diltemplate(pBuf, &tmpl);
		  /* dumpdiltemplate(&tmpl); */
	       }
	    }

            pBuf->FileWrite(f);
#endif
            fclose(f);

#ifndef DEMO_VERSION

	    if (istemplate) {
	       FILE *f;
	       char fname[255];

               sprintf(fname,"%s.dh",cur_zonename);
               if ((f = fopen(fname, "a")) == NULL) {
                 fatal("Couldn't append to .dh file");
               }
               fprintf(f,"%s\n",cur_tmplref);
	       fclose(f);               
            }
#endif       

            free_namelist(var_names);
            free_namelist(label_names);
            if (label_no)
               free(label_adr);
            fprintf(stderr," (%5d bytes)\n", pBuf->GetLength());
            printf("Data in: %s\n", tmpfname);
	    if (errcon)
	      remove(tmpfname);
         }
#line 2720 "y.tab.c"
    break;

  case 3:
#line 333 "dilpar.y"
    {
            /* start at the top again */
            moredilcore(5);
            bwrite_ubit8(&wcore, DILI_GOTO);
            bwrite_ubit32(&wcore, 0);
            /* truncate surplus core space */
            tmpl.coresz = wcore - tmpl.core + 1;
            update_labels();
            tmpl.corecrc = dil_corecrc(tmpl.core, tmpl.coresz);
	    prg.corecrc = tmpl.corecrc;
	    /* create clean interrupts */
	    if (frm.intrcount)
	      CREATE(frm.intr,struct dilintr,frm.intrcount);
	    else
	      frm.intr = NULL;
         }
#line 2741 "y.tab.c"
    break;

  case 4:
#line 353 "dilpar.y"
    {
             SET_BIT(tmpl.flags, DILFL_RECALL);
             SET_BIT(prg.flags, DILFL_RECALL);
          }
#line 2750 "y.tab.c"
    break;

  case 5:
#line 358 "dilpar.y"
    {
             SET_BIT(tmpl.flags, DILFL_AWARE);
             SET_BIT(prg.flags, DILFL_AWARE);
          }
#line 2759 "y.tab.c"
    break;

  case 6:
#line 363 "dilpar.y"
    {
          }
#line 2766 "y.tab.c"
    break;

  case 7:
#line 368 "dilpar.y"
    {
	    fprintf(stderr, "DIL (line %5d)",linenum);

            /* Set up template  */
            CREATE(tmpl.argt, ubit8, ARGMAX);
            CREATE(tmpl.core, ubit8, CODESIZE);
            CREATE(tmpl.vart, ubit8, VARMAX);
            tmpl.prgname = "NONAME";
            tmpl.varc = 0;
            tmpl.coresz = CODESIZE;
            tmpl.flags = 0;
            tmpl.argc = 0;
            tmpl.xrefcount = 0;
            CREATE(tmpl.xrefs, struct dilxref, REFMAX);
            tmpl.extprg = NULL;
            tmpl.varcrc = 0;
            tmpl.corecrc = 0;
	    tmpl.intrcount = 0;

            wcore = tmpl.core;

            /* setup tmp. reference */
            CREATE(ref.argt, ubit8, ARGMAX);
            CREATE(ref.argv, char *, ARGMAX);
            ref.name= NULL;
            ref.rtnt= DILV_NULL;
            ref.argc= 0;
            
            /* setup program (not used with -t) */
            prg.flags = 0;
            prg.varcrc = 0;
            prg.corecrc = 0;
            prg.stacksz = 1;
            prg.stack = &frm;
            prg.sp = prg.stack;

            /* set up frame (not used with -t) */  
            frm.tmpl = &tmpl;
            frm.ret = 0;
            CREATE(frm.vars, struct dilvar, VARMAX);
            /* not used: */
            frm.pc = tmpl.core;
            frm.securecount = 0;
            frm.secure=NULL;
	    frm.intrcount = 0;

            /* compiler variables */
            var_names = create_namelist();
            ref_names = create_namelist();
	    refcount=0;
            ref_usednames = create_namelist();
            /* labels */
            labelgen = 0;
            label_names = create_namelist();
            label_adr = NULL;
            label_no = 0;
            /* label uses */
            label_use_no = 0;
            label_use_adr = NULL;
            label_use_idx = NULL;
            /* break and continue */
            break_no = 0;
            cont_no = 0;
            break_idx = NULL;
            cont_idx = NULL;
	    *cur_tmplref='\0';

         }
#line 2839 "y.tab.c"
    break;

  case 9:
#line 440 "dilpar.y"
    {
            tmpl.varcrc = dil_headercrc(var_names, tmpl.vart);
	    prg.varcrc = tmpl.varcrc;
         }
#line 2848 "y.tab.c"
    break;

  case 10:
#line 447 "dilpar.y"
    {
            add_var((yyvsp[-4].sym),(yyvsp[-2].num));
         }
#line 2856 "y.tab.c"
    break;

  case 14:
#line 459 "dilpar.y"
    {
         }
#line 2863 "y.tab.c"
    break;

  case 15:
#line 462 "dilpar.y"
    {
         }
#line 2870 "y.tab.c"
    break;

  case 16:
#line 467 "dilpar.y"
    {
            char zbuf[255];
            char nbuf[255];
            
            /* define procedure setup */
            split_fi_ref((yyvsp[-5].sym),zbuf,nbuf);

	    if ((strlen(zbuf) ? strlen(zbuf) : -1) + strlen(nbuf) + 1 !=
		strlen((yyvsp[-5].sym)))
	    {
	       sprintf(zbuf, "Name of dilprogram '%s' was too long.",
		       nbuf);
	       fatal(zbuf);
	    }

            if (!*zbuf)
              strcpy(zbuf,cur_zonename);

            ref.name = str_dup(nbuf);
            ref.zname = str_dup(zbuf);
            ref.rtnt = DILV_NULL;

	    add_ref(&ref);
         }
#line 2899 "y.tab.c"
    break;

  case 17:
#line 492 "dilpar.y"
    {
            char zbuf[255];
            char nbuf[255];

            /* define function setup */
            split_fi_ref((yyvsp[-5].sym),zbuf,nbuf);

	    if ((strlen(zbuf) ? strlen(zbuf) : -1) + strlen(nbuf) + 1 !=
		strlen((yyvsp[-5].sym)))
	    {
	       sprintf(zbuf, "Name of dilprogram '%s' was too long.",
		       nbuf);
	       fatal(zbuf);
	    }

            if (!*zbuf)
              strcpy(zbuf,cur_zonename);

            ref.name = str_dup(nbuf);
            ref.zname = str_dup(zbuf);
            ref.rtnt = (yyvsp[-6].num);
	    add_ref(&ref);
         }
#line 2927 "y.tab.c"
    break;

  case 18:
#line 518 "dilpar.y"
    {
	    /* init arglist */
            ref.argc=0;
         }
#line 2936 "y.tab.c"
    break;

  case 23:
#line 532 "dilpar.y"
    {
            /* collect argument */
            ref.argt[ref.argc] = (yyvsp[0].num);
            ref.argv[ref.argc] = str_dup((yyvsp[-2].sym));
            if (++ref.argc > ARGMAX)
               fatal("Too many arguments");
         }
#line 2948 "y.tab.c"
    break;

  case 24:
#line 541 "dilpar.y"
    {
            static const char *typname[] = {
               "void",
               "unitptr",
               "string",
               "stringlist",
               "extraptr",
               "integer"};
            char buf[255],nbuf[255],zbuf[255];
            int i;

            /* define this procedure */
	    split_fi_ref(ref.name,zbuf,nbuf);
	    if (!*zbuf)
	      strcpy(zbuf,cur_zonename);
	    sprintf(buf,"%s@%s",nbuf,zbuf);
            tmpl.prgname = str_dup(buf);
            tmpl.rtnt = ref.rtnt;
            tmpl.argc = ref.argc;

            if (tmpl.argc && !istemplate)
	      fatal("Arguments not allowed on inline DIL programs, make "
		    "it into a template instead.");

	    if (tmpl.argc) {
	       CREATE(tmpl.argt, ubit8, ref.argc);
	       memcpy(tmpl.argt,ref.argt,ref.argc);
	    } else 
	      tmpl.argt=NULL;

            /* create template reference line for .dh file */
            if (ref.rtnt == DILV_NULL)
              sprintf(cur_tmplref,"%s (", tmpl.prgname);
            else
              sprintf(cur_tmplref,"%s %s (", typname[tmpl.rtnt], tmpl.prgname);
              
            for (i=0;i<ref.argc;i++)
	    {
	       /* add arguments as variables */
	       add_var(ref.argv[i], ref.argt[i]);

	       /* generate template reference line */
	       strcat(cur_tmplref, ref.argv[i]);
	       strcat(cur_tmplref, " : ");
	       strcat(cur_tmplref, typname[ref.argt[i]]);
	       if (i<ref.argc-1)
		 strcat(cur_tmplref, ", ");
	    }
	    strcat(cur_tmplref, ");");


	    /* clear used names ! 
	    ref_usednames[0]=NULL;
	    ref_names[0]=NULL;
	    refcount=0;
	    */
         }
#line 3010 "y.tab.c"
    break;

  case 25:
#line 599 "dilpar.y"
    {
	    fatal("All DIL programs must have a name");
            *cur_tmplref='\0';
         }
#line 3019 "y.tab.c"
    break;

  case 26:
#line 610 "dilpar.y"
    {
            (yyval.num) = DILV_UP;
         }
#line 3027 "y.tab.c"
    break;

  case 27:
#line 614 "dilpar.y"
    {
            (yyval.num) = DILV_INT;
         }
#line 3035 "y.tab.c"
    break;

  case 28:
#line 618 "dilpar.y"
    {
            (yyval.num) = DILV_SP;
         }
#line 3043 "y.tab.c"
    break;

  case 29:
#line 622 "dilpar.y"
    {
            (yyval.num) = DILV_SLP;
         }
#line 3051 "y.tab.c"
    break;

  case 30:
#line 626 "dilpar.y"
    {
            (yyval.num) = DILV_EDP;
         }
#line 3059 "y.tab.c"
    break;

  case 31:
#line 636 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    if ((yyvsp[-2].exp).dsl==DSL_FCT)
	       fatal("Attempting to derive fields of proc/func");

            switch ((yyvsp[0].exp).rtyp)
	    {
               /* these structures have fields */
               case DILV_UP:
               case DILV_EDP:
               case DILV_SLP:
               case DILV_SP:
                  if ((yyvsp[-2].exp).typ != (yyvsp[0].exp).rtyp)
                     fatal("Illegal type for field");
               break;

               case DILV_UEDP: /* shared fields */
                  if ((yyvsp[-2].exp).typ != DILV_UP && (yyvsp[-2].exp).typ != DILV_EDP)
                     fatal("Illegal type for field");
               break;
               default:
                  fatal("Illegal field");
               break;
            }

            /* create code */
            add_ubit8(&((yyval.exp)),DILE_FLD);
            add_ubit8(&((yyval.exp)),(yyvsp[0].exp).num);
            add_code(&((yyval.exp)),&((yyvsp[-2].exp)));

            if ((yyvsp[0].exp).codep != (yyvsp[0].exp).code) /* index optinal */
               add_code(&((yyval.exp)),&((yyvsp[0].exp))); 

            if ((yyvsp[0].exp).typ == DILV_UEDP)
               (yyval.exp).typ = (yyvsp[-2].exp).typ; /* inherit type */
            else
               (yyval.exp).typ = (yyvsp[0].exp).typ; /* follow type */

            (yyval.exp).dsl = (yyvsp[0].exp).dsl;   /* static/dynamic/lvalue */

	    if ((yyvsp[-2].exp).typ == DILV_SLP && (yyvsp[-2].exp).dsl == DSL_LFT)
	      (yyval.exp).dsl = DSL_LFT;

            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 3110 "y.tab.c"
    break;

  case 32:
#line 683 "dilpar.y"
    {
            int i,refnum;
	    char nbuf[255],zbuf[255],buf[255];

            INITEXP((yyval.exp));

            if ((i = search_block((yyvsp[-1].sym), (const char **) var_names, TRUE)) == -1)
	    {
	       /* not a variable */
	       /* check external function */

	       split_fi_ref((yyvsp[-1].sym),zbuf,nbuf);
	       if (!*zbuf)
		 strcpy(zbuf,cur_zonename);
	       sprintf(buf,"%s@%s",nbuf,zbuf);

	       /* mark reference as used */
	       if ((refnum = (search_block(buf, (const char **) ref_usednames, TRUE))) == -1)
	       {
		  /* not used before, add to tmpl */
		  if ((i = (search_block(buf, (const char **) ref_names, TRUE))) == -1)
		  {
		     char tmpbuf[256];
		     sprintf(tmpbuf, "No such variable or reference: %s", buf);
		     fatal(tmpbuf);
		  }
	       
		  /* they share the arrays !! */
		  sprintf(buf,"%s@%s",refs[i].name,refs[i].zname);
		  tmpl.xrefs[tmpl.xrefcount].name=str_dup(buf);
		  tmpl.xrefs[tmpl.xrefcount].rtnt=refs[i].rtnt;
		  tmpl.xrefs[tmpl.xrefcount].argc=refs[i].argc;
		  tmpl.xrefs[tmpl.xrefcount].argt=refs[i].argt;

		  refnum=tmpl.xrefcount++; /* this is the reference location */
		  
		  /* it would be wierd if this uccurs, but better safe... */
		  if (tmpl.xrefcount > REFMAX)
		    fatal("Too many references");
	       }
	       (yyval.exp).dsl = DSL_FCT;
	       (yyval.exp).rtyp = refnum;
	       (yyval.exp).typ = tmpl.xrefs[refnum].rtnt;
               /*fprintf(stderr, "Adding ubit16 reference %d %p.\n",
		       refnum, &($$));*/
               add_ubit16(&((yyval.exp)), refnum);
            }
	    else /* A variable */
	    {
               (yyval.exp).typ = tmpl.vart[i];
               (yyval.exp).dsl = DSL_LFT;
               add_ubit8(&((yyval.exp)),DILE_VAR);
               add_ubit16(&((yyval.exp)),i);
            }
         }
#line 3170 "y.tab.c"
    break;

  case 33:
#line 739 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).typ = DILV_UP;
            add_ubit8(&((yyval.exp)),DILE_SELF);
         }
#line 3181 "y.tab.c"
    break;

  case 34:
#line 746 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).typ = DILV_UP;
            add_ubit8(&((yyval.exp)),DILE_ACTI);
         }
#line 3192 "y.tab.c"
    break;

  case 35:
#line 753 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).typ = DILV_UP;
            add_ubit8(&((yyval.exp)),DILE_MEDI);
         }
#line 3203 "y.tab.c"
    break;

  case 36:
#line 760 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).typ = DILV_UP;
            add_ubit8(&((yyval.exp)),DILE_TARG);
         }
#line 3214 "y.tab.c"
    break;

  case 37:
#line 767 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).typ = DILV_INT;
            add_ubit8(&((yyval.exp)),DILE_POWE);
         }
#line 3225 "y.tab.c"
    break;

  case 38:
#line 774 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).typ = DILV_SP;
            add_ubit8(&((yyval.exp)),DILE_CMST);
         }
#line 3236 "y.tab.c"
    break;

  case 39:
#line 781 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).typ = DILV_SP;
            add_ubit8(&((yyval.exp)),DILE_ARGM);
         }
#line 3247 "y.tab.c"
    break;

  case 40:
#line 788 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).typ = DILV_INT;
            add_ubit8(&((yyval.exp)),DILE_HRT);
         }
#line 3258 "y.tab.c"
    break;

  case 41:
#line 795 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).typ = DILV_INT;
            add_ubit8(&((yyval.exp)),DILE_WEAT);
	 }
#line 3269 "y.tab.c"
    break;

  case 42:
#line 802 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).typ = DILV_INT;
            add_ubit8(&((yyval.exp)),DILE_THO);
         }
#line 3280 "y.tab.c"
    break;

  case 43:
#line 809 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).typ = DILV_INT;
            add_ubit8(&((yyval.exp)),DILE_TDA);
         }
#line 3291 "y.tab.c"
    break;

  case 44:
#line 816 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).typ = DILV_INT;
            add_ubit8(&((yyval.exp)),DILE_TMD);
         }
#line 3302 "y.tab.c"
    break;

  case 45:
#line 823 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).typ = DILV_INT;
            add_ubit8(&((yyval.exp)),DILE_TYE);
         }
#line 3313 "y.tab.c"
    break;

  case 46:
#line 830 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).typ = DILV_INT;
            add_ubit8(&((yyval.exp)),DILE_RTI);
         }
#line 3324 "y.tab.c"
    break;

  case 47:
#line 849 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    checkbool("index in array",(yyvsp[-1].exp).boolean);
            if ((yyvsp[-1].exp).typ != DILV_INT)
               fatal("Integer expected for index");
            else 
               make_code(&((yyvsp[-1].exp)));     /* make it dynamic */
            copy_code(&((yyval.exp)),&((yyvsp[-1].exp))); /* copy the code   */
            FREEEXP((yyvsp[-1].exp));
         }
#line 3339 "y.tab.c"
    break;

  case 48:
#line 862 "dilpar.y"
    {
            INITEXP((yyval.exp));
            copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
            (yyval.exp).rtyp = DILV_SLP;
            (yyval.exp).typ = DILV_SP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_IDX;
            FREEEXP((yyvsp[0].exp));
         }
#line 3353 "y.tab.c"
    break;

  case 49:
#line 877 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_BIR;
         }
#line 3365 "y.tab.c"
    break;

  case 50:
#line 885 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_PTI;
         }
#line 3377 "y.tab.c"
    break;

  case 51:
#line 893 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_PCF;
         }
#line 3389 "y.tab.c"
    break;

  case 52:
#line 901 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_SP;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_HOME;
         }
#line 3401 "y.tab.c"
    break;

  case 53:
#line 909 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_UP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_MAS;
         }
#line 3413 "y.tab.c"
    break;

  case 54:
#line 917 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_UP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_FOL;
         }
#line 3425 "y.tab.c"
    break;

  case 55:
#line 925 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_SID;
         }
#line 3437 "y.tab.c"
    break;

  case 56:
#line 933 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_SP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_ZOI;
         }
#line 3449 "y.tab.c"
    break;

  case 57:
#line 941 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_SP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_NMI;
         }
#line 3461 "y.tab.c"
    break;

  case 58:
#line 949 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_TYP;
         }
#line 3473 "y.tab.c"
    break;

  case 59:
#line 957 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UEDP;
            (yyval.exp).typ = DILV_UEDP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_NXT;
         }
#line 3485 "y.tab.c"
    break;

  case 60:
#line 965 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UEDP;
            (yyval.exp).typ = DILV_SLP;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_NMS;
         }
#line 3497 "y.tab.c"
    break;

  case 61:
#line 973 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UEDP;
            (yyval.exp).typ = DILV_SP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_NAM;
         }
#line 3509 "y.tab.c"
    break;

  case 62:
#line 981 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_EDP;
            (yyval.exp).typ = DILV_SP;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_DES;
         }
#line 3521 "y.tab.c"
    break;

  case 63:
#line 989 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_SP;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_ODES;
         }
#line 3533 "y.tab.c"
    break;

  case 64:
#line 997 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_SP;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_IDES;
         }
#line 3545 "y.tab.c"
    break;

  case 65:
#line 1005 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_SP;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_TIT;
         }
#line 3557 "y.tab.c"
    break;

  case 66:
#line 1013 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_EDP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_EXT;
         }
#line 3569 "y.tab.c"
    break;

  case 67:
#line 1021 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_UP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_OUT;
         }
#line 3581 "y.tab.c"
    break;

  case 68:
#line 1029 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_UP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_INS;
         }
#line 3593 "y.tab.c"
    break;

  case 69:
#line 1037 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_UP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_GNX;
         }
#line 3605 "y.tab.c"
    break;

  case 70:
#line 1045 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_LCN;
         }
#line 3617 "y.tab.c"
    break;

  case 71:
#line 1053 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_UP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_GPR;
         }
#line 3629 "y.tab.c"
    break;

  case 72:
#line 1061 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_LGT;
         }
#line 3641 "y.tab.c"
    break;

  case 73:
#line 1069 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_BGT;
         }
#line 3653 "y.tab.c"
    break;

  case 74:
#line 1077 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_MIV;
         }
#line 3665 "y.tab.c"
    break;

  case 75:
#line 1085 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_ILL;
         }
#line 3677 "y.tab.c"
    break;

  case 76:
#line 1093 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_FL;
         }
#line 3689 "y.tab.c"
    break;

  case 77:
#line 1101 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_MAN;
         }
#line 3701 "y.tab.c"
    break;

  case 78:
#line 1109 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_OFL;
         }
#line 3713 "y.tab.c"
    break;

  case 79:
#line 1117 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_MHP;
         }
#line 3725 "y.tab.c"
    break;

  case 80:
#line 1125 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_EDP;
            (yyval.exp).dsl = DSL_DYN;
	    (yyval.exp).num = DILF_INFO;
         }
#line 3737 "y.tab.c"
    break;

  case 81:
#line 1133 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_LSA;
         }
#line 3749 "y.tab.c"
    break;

  case 82:
#line 1141 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_MED;
         }
#line 3761 "y.tab.c"
    break;

  case 83:
#line 1149 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_MMA;
         }
#line 3773 "y.tab.c"
    break;

  case 84:
#line 1157 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_CHP;
         }
#line 3785 "y.tab.c"
    break;

  case 85:
#line 1165 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_BWT;
         }
#line 3797 "y.tab.c"
    break;

  case 86:
#line 1173 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_WGT;
         }
#line 3809 "y.tab.c"
    break;

  case 87:
#line 1181 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_CAP;
         }
#line 3821 "y.tab.c"
    break;

  case 88:
#line 1189 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_ALG;
         }
#line 3833 "y.tab.c"
    break;

  case 89:
#line 1197 "dilpar.y"
    {
            INITEXP((yyval.exp));
            copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_SPL;
         }
#line 3846 "y.tab.c"
    break;

  case 90:
#line 1206 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_FUN;
         }
#line 3858 "y.tab.c"
    break;

  case 91:
#line 1214 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_SP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_ZON;
         }
#line 3870 "y.tab.c"
    break;

  case 92:
#line 1227 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_OTY;
         }
#line 3882 "y.tab.c"
    break;

  case 93:
#line 1235 "dilpar.y"
    {
            INITEXP((yyval.exp));
            copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_VAL;
            FREEEXP((yyvsp[0].exp));
         }
#line 3896 "y.tab.c"
    break;

  case 94:
#line 1245 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_EFL;
         }
#line 3908 "y.tab.c"
    break;

  case 95:
#line 1253 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_CST;
         }
#line 3920 "y.tab.c"
    break;

  case 96:
#line 1261 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_RNT;
         }
#line 3932 "y.tab.c"
    break;

  case 97:
#line 1269 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_EQP;
         }
#line 3944 "y.tab.c"
    break;

  case 98:
#line 1282 "dilpar.y"
    {
            INITEXP((yyval.exp));
            copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_SLP;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_ONM;
            FREEEXP((yyvsp[0].exp));
         }
#line 3958 "y.tab.c"
    break;

  case 99:
#line 1292 "dilpar.y"
    {
            INITEXP((yyval.exp));
            copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_XNF;
            FREEEXP((yyvsp[0].exp));
         }
#line 3972 "y.tab.c"
    break;

  case 100:
#line 1302 "dilpar.y"
    {
            INITEXP((yyval.exp));
            copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_UP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_TOR;
            FREEEXP((yyvsp[0].exp));
         }
#line 3986 "y.tab.c"
    break;

  case 101:
#line 1312 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_RFL;
         }
#line 3998 "y.tab.c"
    break;

  case 102:
#line 1320 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_MOV;
         }
#line 4010 "y.tab.c"
    break;

  case 103:
#line 1333 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_ABAL;
         }
#line 4022 "y.tab.c"
    break;

  case 104:
#line 1341 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_ATOT;
         }
#line 4034 "y.tab.c"
    break;

  case 105:
#line 1349 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_SPD;
         }
#line 4046 "y.tab.c"
    break;

  case 106:
#line 1357 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_SEX;
         }
#line 4058 "y.tab.c"
    break;

  case 107:
#line 1365 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_RCE;
         }
#line 4070 "y.tab.c"
    break;

  case 108:
#line 1373 "dilpar.y"
    {
            INITEXP((yyval.exp));
            copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_ABL;
            FREEEXP((yyvsp[0].exp));
         }
#line 4084 "y.tab.c"
    break;

  case 109:
#line 1383 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_EXP;
         }
#line 4096 "y.tab.c"
    break;

  case 110:
#line 1391 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_LVL;
         }
#line 4108 "y.tab.c"
    break;

  case 111:
#line 1399 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_HGT;
         }
#line 4120 "y.tab.c"
    break;

  case 112:
#line 1407 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_POS;
         }
#line 4132 "y.tab.c"
    break;

  case 113:
#line 1415 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_ATY;
         }
#line 4144 "y.tab.c"
    break;

  case 114:
#line 1423 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_MNA;
         }
#line 4156 "y.tab.c"
    break;

  case 115:
#line 1431 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_END;
         }
#line 4168 "y.tab.c"
    break;

  case 116:
#line 1439 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_AFF;
         }
#line 4180 "y.tab.c"
    break;

  case 117:
#line 1447 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_DRE;
         }
#line 4192 "y.tab.c"
    break;

  case 118:
#line 1455 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_UP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_FGT;
         }
#line 4204 "y.tab.c"
    break;

  case 119:
#line 1463 "dilpar.y"
    {
            INITEXP((yyval.exp));
            copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_WPN;
            FREEEXP((yyvsp[0].exp));
         }
#line 4218 "y.tab.c"
    break;

  case 120:
#line 1478 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_DEF;
         }
#line 4230 "y.tab.c"
    break;

  case 121:
#line 1486 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_ACT;
         }
#line 4242 "y.tab.c"
    break;

  case 122:
#line 1499 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_TIM;
         }
#line 4254 "y.tab.c"
    break;

  case 123:
#line 1507 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_CRM;
         }
#line 4266 "y.tab.c"
    break;

  case 124:
#line 1515 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_FLL;
         }
#line 4278 "y.tab.c"
    break;

  case 125:
#line 1523 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_THR;
         }
#line 4290 "y.tab.c"
    break;

  case 126:
#line 1531 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_DRK;
         }
#line 4302 "y.tab.c"
    break;

  case 127:
#line 1539 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_SPT;
         }
#line 4314 "y.tab.c"
    break;

  case 128:
#line 1547 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_APT;
         }
#line 4326 "y.tab.c"
    break;

  case 129:
#line 1555 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_SP;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_GLD;
         }
#line 4338 "y.tab.c"
    break;

  case 130:
#line 1563 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_EDP;
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).num = DILF_QST;
         }
#line 4350 "y.tab.c"
    break;

  case 131:
#line 1571 "dilpar.y"
    {
            INITEXP((yyval.exp));
            copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
            (yyval.exp).rtyp = DILV_UP;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = DSL_LFT;
            (yyval.exp).num = DILF_SKL;
            FREEEXP((yyvsp[0].exp));
         }
#line 4364 "y.tab.c"
    break;

  case 132:
#line 1587 "dilpar.y"
    {
            (yyval.num) = DILE_SE;
         }
#line 4372 "y.tab.c"
    break;

  case 133:
#line 1593 "dilpar.y"
    {
            (yyval.num) = DILE_PE;
         }
#line 4380 "y.tab.c"
    break;

  case 134:
#line 1599 "dilpar.y"
    {
            (yyval.num) = DILE_EQ;
         }
#line 4388 "y.tab.c"
    break;

  case 135:
#line 1603 "dilpar.y"
    {
            (yyval.num) = DILE_NE;
         }
#line 4396 "y.tab.c"
    break;

  case 136:
#line 1609 "dilpar.y"
    {
            (yyval.num) = DILE_GT;
         }
#line 4404 "y.tab.c"
    break;

  case 137:
#line 1613 "dilpar.y"
    {
            (yyval.num) = DILE_LT;
         }
#line 4412 "y.tab.c"
    break;

  case 138:
#line 1617 "dilpar.y"
    {
            (yyval.num) = DILE_GE;
         }
#line 4420 "y.tab.c"
    break;

  case 139:
#line 1621 "dilpar.y"
    {
            (yyval.num) = DILE_LE;
         }
#line 4428 "y.tab.c"
    break;

  case 140:
#line 1631 "dilpar.y"
    {
/*          fprintf(stderr,"dilsexp->dilexp\n");*/
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[0].exp).boolean;
            copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
            FREEEXP((yyvsp[0].exp));
         }
#line 4440 "y.tab.c"
    break;

  case 141:
#line 1639 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=1;
            switch ((yyvsp[-2].exp).typ) {
               /*
                * Replaces integer function identifies with
                * appropiate pointer/string function identifier
                * where appropriate.
                */

               case DILV_INT:
                  if ((yyvsp[0].exp).typ != DILV_INT)
                     fatal("Arg 2 of integer comparison not integer");
                  break;
               case DILV_NULL:
               case DILV_UP:
               case DILV_EDP:
               case DILV_SLP:
                  if (((yyvsp[-2].exp).typ != (yyvsp[0].exp).typ) &&
                      ((yyvsp[-2].exp).typ != DILV_NULL) && ((yyvsp[0].exp).typ != DILV_NULL))
                     fatal("Argument of pointer compare not of same type.");
                  if ((yyvsp[-1].num) == DILE_NE)
                     (yyvsp[-1].num) = DILE_PNEQ; /* snask */
                  else
                     (yyvsp[-1].num) = DILE_PE; /* snask */
                  break;
               case DILV_SP:
                  if ((yyvsp[0].exp).typ == DILV_NULL) {
                     if ((yyvsp[-1].num) == DILE_NE)
                        (yyvsp[-1].num) = DILE_PNEQ; /* snask */
                     else
                        (yyvsp[-1].num) = DILE_PE; /* snask */
                     break;
                  }
                  if ((yyvsp[0].exp).typ != DILV_SP)
                     fatal("Arg 2 of string comp. not a string");
                  else {
                     if ((yyvsp[-1].num) == DILE_NE)
                        (yyvsp[-1].num) = DILE_SNEQ; /* snask */
                     else
                        (yyvsp[-1].num) = DILE_SE; /* snask */
                  }
                  break;
               default:
                  fatal("Arg 1 of compare invalid type");
               break;
            }
            /* Make nodes dynamic */
            /* Integer compares are not _yet_ static */
            make_code(&((yyvsp[-2].exp)));
            make_code(&((yyvsp[0].exp)));
            add_ubit8(&((yyval.exp)),(yyvsp[-1].num));
            add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
            add_code(&((yyval.exp)),&((yyvsp[0].exp)));
            (yyval.exp).dsl = DSL_DYN;
            (yyval.exp).typ = DILV_INT;
            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 4504 "y.tab.c"
    break;

  case 142:
#line 1699 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=1;
            if ((yyvsp[-2].exp).typ != DILV_INT)
               fatal("Arg 1 of integer comparison not integer");
            else if ((yyvsp[0].exp).typ != DILV_INT)
               fatal("Arg 2 of integer comparison not integer");
            else
            {
               /* Make nodes dynamic */
               /* Integer compares are not _yet_ static */
               make_code(&((yyvsp[-2].exp)));
               make_code(&((yyvsp[0].exp)));
               add_ubit8(&((yyval.exp)),(yyvsp[-1].num));
               add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
               add_code(&((yyval.exp)),&((yyvsp[0].exp)));
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
            }
            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 4531 "y.tab.c"
    break;

  case 143:
#line 1722 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=1;
            switch ((yyvsp[-2].exp).typ)
            {
             case DILV_NULL:
             case DILV_UP:
             case DILV_SP:
             case DILV_EDP:
             case DILV_SLP:
               if (((yyvsp[-2].exp).typ != (yyvsp[0].exp).typ) &&
               ((yyvsp[-2].exp).typ != DILV_NULL) && ((yyvsp[0].exp).typ != DILV_NULL))
                  fatal("Argument of pointer compare not of same type.");
               else
               {
                  make_code(&((yyvsp[-2].exp)));
                  make_code(&((yyvsp[0].exp)));
                  add_ubit8(&((yyval.exp)),(yyvsp[-1].num)); /* compare funct */
                  add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
                  add_code(&((yyval.exp)),&((yyvsp[0].exp)));
                  (yyval.exp).dsl = DSL_DYN;
                  (yyval.exp).typ = DILV_INT;
               }
               break;

             default:
               fatal("Arg 1 of pointer comp. not a pointer");
               break;
            }
            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 4568 "y.tab.c"
    break;

  case 144:
#line 1755 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=1;
            if ((yyvsp[-2].exp).typ != DILV_SP)
               fatal("Arg 1 of string comp. not a string");
            else if ((yyvsp[-2].exp).typ != DILV_SP)
               fatal("Arg 2 of string comp. not a string");
            else
            {
               /* Strings are not _yet_ static */
               /* String compare not _yet_ static */
               make_code(&((yyvsp[-2].exp)));
               make_code(&((yyvsp[0].exp)));
               add_ubit8(&((yyval.exp)),(yyvsp[-1].num)); /* compare funct */
               add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
               add_code(&((yyval.exp)),&((yyvsp[0].exp)));
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
            }
            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 4595 "y.tab.c"
    break;

  case 145:
#line 1780 "dilpar.y"
    {
/*          fprintf(stderr,"dilterm->dilsexp\n");*/
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[0].exp).boolean;
            copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
            FREEEXP((yyvsp[0].exp));
         }
#line 4607 "y.tab.c"
    break;

  case 146:
#line 1788 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[0].exp).boolean;
            if ((yyvsp[0].exp).typ != DILV_INT)
               fatal("Arg 1 of 'unary -' not integer");
            else {
               /* Type is ok */
               (yyval.exp).typ = DILV_INT;
               (yyval.exp).dsl = (yyvsp[0].exp).dsl;
               if (!(yyvsp[0].exp).dsl)
                  (yyval.exp).num =   - (yyvsp[0].exp).num; /* static */
               else {
                  add_ubit8(&((yyval.exp)), DILE_UMIN); 
                  add_code(&((yyval.exp)),&((yyvsp[0].exp)));
               }
            }
            FREEEXP((yyvsp[0].exp));
         }
#line 4630 "y.tab.c"
    break;

  case 147:
#line 1807 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[0].exp).boolean;
            if ((yyvsp[0].exp).typ != DILV_INT)
               fatal("Arg 1 of 'unary +' not integer");
            else {
               /* Type is ok */
               copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
            }
            FREEEXP((yyvsp[0].exp));
         }
#line 4646 "y.tab.c"
    break;

  case 148:
#line 1819 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[-2].exp).boolean||(yyvsp[0].exp).boolean;
            if ((yyvsp[-2].exp).typ != DILV_INT)
            {
               if ((yyvsp[-2].exp).typ != DILV_SP)
                  fatal("Arg 1 of '+' not integer or string");
               else if ((yyvsp[0].exp).typ != DILV_SP)
                  fatal("Arg 2 of '+' not string");
               else {
                  /* Type is SP + SP */
                  (yyval.exp).typ = DILV_SP;
                  if (!((yyvsp[-2].exp).dsl+(yyvsp[0].exp).dsl)) {
                     /* both static */
                     copy_code(&((yyval.exp)),&((yyvsp[-2].exp)));
                     cat_string(&((yyval.exp)),(char *) &((yyvsp[0].exp).code[1]));
                     (yyval.exp).dsl = DSL_STA;
                  } else {
                     /* one is dynamic */
                     make_code(&((yyvsp[-2].exp)));
                     make_code(&((yyvsp[0].exp)));
                     add_ubit8(&((yyval.exp)),DILE_PLUS);
                     add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
                     add_code(&((yyval.exp)),&((yyvsp[0].exp)));
                     (yyval.exp).dsl = DSL_DYN;
                  }
               }
            }
            else if ((yyvsp[0].exp).typ != DILV_INT)
               fatal("Arg 2 of '+' not integer");
            else {
               /* Type is INT + INT */
               (yyval.exp).typ = DILV_INT;
               if (!((yyvsp[-2].exp).dsl + (yyvsp[0].exp).typ)) {
                  (yyval.exp).num = (yyvsp[-2].exp).num + (yyvsp[0].exp).num;
                  (yyval.exp).dsl = DSL_STA;
               } else {
                  /* make nodes dynamic */
                  make_code(&((yyvsp[-2].exp)));
                  make_code(&((yyvsp[0].exp)));
                  add_ubit8(&((yyval.exp)), DILE_PLUS);
                  add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
                  add_code(&((yyval.exp)),&((yyvsp[0].exp)));
                  (yyval.exp).dsl = DSL_DYN;
               }
            }
            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 4700 "y.tab.c"
    break;

  case 149:
#line 1869 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[-2].exp).boolean||(yyvsp[0].exp).boolean;
            if ((yyvsp[-2].exp).typ != DILV_INT)
               fatal("Arg 1 of '-' not integer");
            else if ((yyvsp[0].exp).typ != DILV_INT)
               fatal("Arg 2 of '-' not integer");
            else {
               /* Type is ok */
               (yyval.exp).typ = DILV_INT;
               if (!((yyvsp[-2].exp).dsl + (yyvsp[0].exp).dsl)) {
                  (yyval.exp).num = (yyvsp[-2].exp).num - (yyvsp[0].exp).num;
                  (yyval.exp).dsl = DSL_STA;
               } else {
                  /* make nodes dynamic */
                  make_code(&((yyvsp[-2].exp)));
                  make_code(&((yyvsp[0].exp)));
                  add_ubit8(&((yyval.exp)), DILE_MIN);
                  add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
                  add_code(&((yyval.exp)),&((yyvsp[0].exp)));
                  (yyval.exp).dsl = DSL_DYN;
               }
            }
            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 4731 "y.tab.c"
    break;

  case 150:
#line 1896 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=1;
            /* Type is ok */
            (yyval.exp).typ = DILV_INT;
            if (!((yyvsp[-2].exp).dsl + (yyvsp[0].exp).dsl) &&
                ((yyvsp[-2].exp).typ == DILV_INT) &&
                ((yyvsp[0].exp).typ == DILV_INT)) {
               /* static integers */
               (yyval.exp).num = (yyvsp[-2].exp).num || (yyvsp[0].exp).num;
               (yyval.exp).dsl = DSL_STA;
            } else {
               /* make nodes dynamic */
               make_code(&((yyvsp[-2].exp)));
               make_code(&((yyvsp[0].exp)));
               add_ubit8(&((yyval.exp)), DILE_LOR);
               add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
               add_code(&((yyval.exp)),&((yyvsp[0].exp)));
               (yyval.exp).dsl = DSL_DYN;
            }
            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 4759 "y.tab.c"
    break;

  case 151:
#line 1920 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[-2].exp).boolean||(yyvsp[0].exp).boolean;
            /* Type is ok */
            (yyval.exp).typ = DILV_INT;
            if (!((yyvsp[-2].exp).dsl + (yyvsp[0].exp).dsl) &&
                ((yyvsp[-2].exp).typ == DILV_INT) &&
                ((yyvsp[0].exp).typ == DILV_INT)) {
               /* static integers */
               (yyval.exp).num = (yyvsp[-2].exp).num | (yyvsp[0].exp).num;
               (yyval.exp).dsl = DSL_STA;
            } else {
               /* make nodes dynamic */
               make_code(&((yyvsp[-2].exp)));
               make_code(&((yyvsp[0].exp)));
               add_ubit8(&((yyval.exp)), DILE_OR);
               add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
               add_code(&((yyval.exp)),&((yyvsp[0].exp)));
               (yyval.exp).dsl = DSL_DYN;
            }
            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 4787 "y.tab.c"
    break;

  case 152:
#line 1946 "dilpar.y"
    {
/*          fprintf(stderr,"dilfactor->dilterm\n");*/
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[0].exp).boolean;
            copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
            FREEEXP((yyvsp[0].exp));
         }
#line 4799 "y.tab.c"
    break;

  case 153:
#line 1954 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[-2].exp).boolean||(yyvsp[0].exp).boolean;
            if ((yyvsp[-2].exp).typ != DILV_INT)
               fatal("Arg 1 of '*' not integer");
            else if ((yyvsp[0].exp).typ != DILV_INT)
               fatal("Arg 2 of '*' not integer");
            else {
               /* Type is ok */
               (yyval.exp).typ = DILV_INT;
               if (!((yyvsp[-2].exp).dsl + (yyvsp[0].exp).dsl)) {
                  /* Both values are static */
                  (yyval.exp).dsl = DSL_STA;
                  (yyval.exp).num = (yyvsp[-2].exp).num * (yyvsp[0].exp).num;
               } else {
                  /* make nodes dynamic */
                  make_code(&((yyvsp[-2].exp)));
                  make_code(&((yyvsp[0].exp)));
                  add_ubit8(&((yyval.exp)), DILE_MUL);
                  add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
                  add_code(&((yyval.exp)),&((yyvsp[0].exp)));
                  (yyval.exp).dsl = DSL_DYN;
               }
            }
            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 4831 "y.tab.c"
    break;

  case 154:
#line 1982 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[-2].exp).boolean||(yyvsp[0].exp).boolean;
            if ((yyvsp[-2].exp).typ != DILV_INT)
               fatal("Arg 1 of '/' not integer");
            else if ((yyvsp[0].exp).typ != DILV_INT)
               fatal("Arg 2 of '/' not integer");
            else {
               /* Type is ok */
               (yyval.exp).typ = DILV_INT;
               if (!((yyvsp[-2].exp).dsl + (yyvsp[0].exp).dsl)) {
                  (yyval.exp).dsl = DSL_STA;
                  (yyval.exp).num = (yyvsp[-2].exp).num / (yyvsp[0].exp).num;
               } else {
                  /* make nodes dynamic */
                  make_code(&((yyvsp[-2].exp)));
                  make_code(&((yyvsp[0].exp)));
                  add_ubit8(&((yyval.exp)), DILE_DIV);
                  add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
                  add_code(&((yyval.exp)),&((yyvsp[0].exp)));
                  (yyval.exp).dsl = DSL_DYN;
               }
            }
            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 4862 "y.tab.c"
    break;

  case 155:
#line 2009 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=1;
            /* Type is ok */
            (yyval.exp).typ = DILV_INT;
            if (!((yyvsp[-2].exp).dsl + (yyvsp[0].exp).dsl) &&
                ((yyvsp[-2].exp).typ == DILV_INT) &&
                ((yyvsp[0].exp).typ == DILV_INT)) {
               (yyval.exp).dsl = DSL_STA;
               (yyval.exp).num = (yyvsp[-2].exp).num && (yyvsp[0].exp).num;
            } else {
               /* make nodes dynamic */
               make_code(&((yyvsp[-2].exp)));
               make_code(&((yyvsp[0].exp)));
               add_ubit8(&((yyval.exp)), DILE_LAND);
               add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
               add_code(&((yyval.exp)),&((yyvsp[0].exp)));
               (yyval.exp).dsl = DSL_DYN;
            }
            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 4889 "y.tab.c"
    break;

  case 156:
#line 2032 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[-2].exp).boolean||(yyvsp[0].exp).boolean;
            /* Type is ok */
            (yyval.exp).typ = DILV_INT;
            if (!((yyvsp[-2].exp).dsl + (yyvsp[0].exp).dsl) &&
                ((yyvsp[-2].exp).typ == DILV_INT) &&
                ((yyvsp[0].exp).typ == DILV_INT)) {
               (yyval.exp).dsl = DSL_STA;
               (yyval.exp).num = (yyvsp[-2].exp).num & (yyvsp[0].exp).num;
            } else {
               /* make nodes dynamic */
               make_code(&((yyvsp[-2].exp)));
               make_code(&((yyvsp[0].exp)));
               add_ubit8(&((yyval.exp)), DILE_AND);
               add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
               add_code(&((yyval.exp)),&((yyvsp[0].exp)));
               (yyval.exp).dsl = DSL_DYN;
            }
            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 4916 "y.tab.c"
    break;

  case 157:
#line 2055 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[-2].exp).boolean||(yyvsp[0].exp).boolean;
            if ((yyvsp[-2].exp).typ != DILV_INT)
               fatal("Arg 1 of '%' not integer");
            else if ((yyvsp[0].exp).typ != DILV_INT)
               fatal("Arg 2 of '%' not integer");
            else {
               /* Type is ok */
               (yyval.exp).typ = DILV_INT;
               if (!((yyvsp[-2].exp).dsl + (yyvsp[0].exp).dsl)) {
                  (yyval.exp).dsl = DSL_STA;
                  (yyval.exp).num = (yyvsp[-2].exp).num % (yyvsp[0].exp).num;
               } else {
                  /* make nodes dynamic */
                  make_code(&((yyvsp[-2].exp)));
                  make_code(&((yyvsp[0].exp)));
                  add_ubit8(&((yyval.exp)), DILE_MOD);
                  add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
                  add_code(&((yyval.exp)),&((yyvsp[0].exp)));
                  (yyval.exp).dsl = DSL_DYN;
               }
            }
            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 4947 "y.tab.c"
    break;

  case 158:
#line 2082 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[-2].exp).boolean||(yyvsp[0].exp).boolean;
            if ((yyvsp[-2].exp).typ != DILV_SP)
               fatal("Arg 1 of 'in' not string");
            else if (((yyvsp[0].exp).typ != DILV_EDP) &&
                     ((yyvsp[0].exp).typ != DILV_SLP) &&
                      ((yyvsp[0].exp).typ != DILV_SP))
               fatal("Arg 2 of 'in' not string, string "
                     "list or extra description");
            else {
               /* Type is string in stringlist */
               /* Strings are not _yet_ static here */
               (yyval.exp).dsl = DSL_DYN;
               switch ((yyvsp[0].exp).typ) {
                  
                case DILV_SLP:
                  (yyval.exp).typ = DILV_INT;
                  break;

                case DILV_EDP:
                  (yyval.exp).typ = DILV_EDP;
                  break;

                case DILV_SP:
                  (yyval.exp).typ = DILV_INT;
                  break;
               }
               make_code(&((yyvsp[-2].exp)));
               make_code(&((yyvsp[0].exp)));
               add_ubit8(&((yyval.exp)), DILE_IN);
               add_code(&((yyval.exp)),&((yyvsp[-2].exp)));
               add_code(&((yyval.exp)),&((yyvsp[0].exp)));
               (yyval.exp).dsl = DSL_DYN;
            }
            FREEEXP((yyvsp[-2].exp));
            FREEEXP((yyvsp[0].exp));
         }
#line 4990 "y.tab.c"
    break;

  case 159:
#line 2123 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[-1].exp).boolean;
            copy_code(&((yyval.exp)),&((yyvsp[-1].exp)));
	    FREEEXP((yyvsp[-1].exp)); 
         }
#line 5001 "y.tab.c"
    break;

  case 160:
#line 2130 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=(yyvsp[0].exp).boolean;
            copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
	    FREEEXP((yyvsp[0].exp));
         }
#line 5012 "y.tab.c"
    break;

  case 161:
#line 2137 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).dsl = DSL_STA;
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).num = (yyvsp[0].num);
         }
#line 5023 "y.tab.c"
    break;

  case 162:
#line 2144 "dilpar.y"
    {
            INITEXP((yyval.exp));
/*            if (*$1) { */
               /* Strings are now static */
               (yyval.exp).typ = DILV_SP;
               (yyval.exp).dsl = DSL_STA;
               add_ubit8(&((yyval.exp)),DILE_FS);
               add_string(&((yyval.exp)),(yyvsp[0].str));
/* WHY NOT?           } else
               fatal("empty strings not allowed for now"); */
         }
#line 5039 "y.tab.c"
    break;

  case 163:
#line 2156 "dilpar.y"
    {
            INITEXP((yyval.exp));
            if (!(yyvsp[0].str_list)[0])
               fatal("empty stringlists not allowed for now");
            else {
               /* write stringlist _NOT_ static */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_SLP;
               add_ubit8(&((yyval.exp)),DILE_FSL);
               add_stringlist(&((yyval.exp)),(yyvsp[0].str_list));
            }
         }
#line 5056 "y.tab.c"
    break;

  case 164:
#line 2169 "dilpar.y"
    {
            INITEXP((yyval.exp));
            (yyval.exp).typ = DILV_NULL;
            (yyval.exp).dsl = DSL_STA;
            (yyval.exp).num = 0;
         }
#line 5067 "y.tab.c"
    break;

  case 165:
#line 2176 "dilpar.y"
    {
	    if ((yyvsp[0].exp).dsl==DSL_FCT)
	       fatal("Illegal use of proc/func");

            INITEXP((yyval.exp));
            copy_code(&((yyval.exp)),&((yyvsp[0].exp)));
            FREEEXP((yyvsp[0].exp));
         }
#line 5080 "y.tab.c"
    break;

  case 166:
#line 2185 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=1;
            /* Type is ok */
            (yyval.exp).typ = DILV_INT;
            (yyval.exp).dsl = (yyvsp[0].exp).dsl;
            if (!(yyvsp[0].exp).dsl && (yyvsp[0].exp).typ == DILV_INT)
               (yyval.exp).num = !(yyvsp[0].exp).num;
            else {
               add_ubit8(&((yyval.exp)),DILE_NOT);
               add_code(&((yyval.exp)),&((yyvsp[0].exp)));
            }
            FREEEXP((yyvsp[0].exp));
         }
#line 5099 "y.tab.c"
    break;

  case 167:
#line 2206 "dilpar.y"
    {
            INITEXP((yyval.exp));
            if ((yyvsp[-1].exp).typ != DILV_SP)
               fatal("Arg 1 of 'atoi' not string");
            else {
               /* Type is ok */
               /* Function is not _yet_ static */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_ATOI);
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-1].exp));
         }
#line 5119 "y.tab.c"
    break;

  case 168:
#line 2222 "dilpar.y"
    {
            INITEXP((yyval.exp));
            if ((yyvsp[-3].exp).typ != DILV_SP)
               fatal("Arg 1 of 'dildestroy' not string");
            else if ((yyvsp[-1].exp).typ != DILV_UP)
               fatal("Arg 2 of 'dildestouy' not unitptr");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_DLD);
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 5144 "y.tab.c"
    break;

  case 169:
#line 2243 "dilpar.y"
    {
            INITEXP((yyval.exp));
            if ((yyvsp[-3].exp).typ != DILV_SP)
               fatal("Arg 1 of 'dildestroy' not string");
            else if ((yyvsp[-1].exp).typ != DILV_UP)
               fatal("Arg 2 of 'dildestouy' not unitptr");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_DLF);
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 5169 "y.tab.c"
    break;

  case 170:
#line 2264 "dilpar.y"
    {
	    INITEXP((yyval.exp));
            if (((yyvsp[-1].exp).typ != DILV_SP) && ((yyvsp[-1].exp).typ != DILV_SLP))
               fatal("Arg 1 of 'length' not string or stringlist");
            else {
               /* Type is ok */
               /* Function is not _yet_ static */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_LEN);
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-1].exp));
         }
#line 5189 "y.tab.c"
    break;

  case 171:
#line 2280 "dilpar.y"
    {
            INITEXP((yyval.exp));
            if ((yyvsp[-1].exp).typ != DILV_INT)
               fatal("Arg 1 of 'itoa' not integer");
            else {
               /* Type is ok */
               /* Function is not _yet_ static */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_SP;
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_ITOA);
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-1].exp));
         }
#line 5209 "y.tab.c"
    break;

  case 172:
#line 2296 "dilpar.y"
    {
	    /* Fri Jan 24 12:44:22 PST 1997   HHS:
	       This function writes act() as a function that
	       returns a string in an expression
	       */
	    INITEXP((yyval.exp))
	    checkbool("argument 2 of act",(yyvsp[-9].exp).boolean);
	    checkbool("argument 6 of act",(yyvsp[-1].exp).boolean);
            if ((yyvsp[-11].exp).typ != DILV_SP)
               fatal("Arg 1 of 'act' not a string");
            else if ((yyvsp[-9].exp).typ != DILV_INT)
               fatal("Arg 2 of 'act' not an integer");
            else if ((yyvsp[-7].exp).typ != DILV_NULL &&
                (yyvsp[-7].exp).typ != DILV_SP &&
                (yyvsp[-7].exp).typ != DILV_UP)
               fatal("Arg 3 of 'act' not a unit or string");
            else if ((yyvsp[-5].exp).typ != DILV_NULL &&
                (yyvsp[-5].exp).typ != DILV_SP &&
                (yyvsp[-5].exp).typ != DILV_UP)
               fatal("Arg 4 of 'act' not a unit or string");
            else if ((yyvsp[-3].exp).typ != DILV_NULL &&
                (yyvsp[-3].exp).typ != DILV_SP &&
                (yyvsp[-3].exp).typ != DILV_UP)
               fatal("Arg 5 of 'act' not a unit or string");
            else if ((yyvsp[-1].exp).typ != DILV_INT)
               fatal("Arg 6 of 'act' not an integer");
            else {
	       /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_SP;

	       make_code(&((yyvsp[-11].exp)));
	       make_code(&((yyvsp[-9].exp)));
	       make_code(&((yyvsp[-7].exp)));
	       make_code(&((yyvsp[-5].exp)));
	       make_code(&((yyvsp[-3].exp)));
	       make_code(&((yyvsp[-1].exp)));

               add_ubit8(&((yyval.exp)),DILE_ACT);
	       add_code(&((yyval.exp)),&((yyvsp[-11].exp)));
	       add_code(&((yyval.exp)),&((yyvsp[-9].exp)));
	       add_code(&((yyval.exp)),&((yyvsp[-7].exp)));
	       add_code(&((yyval.exp)),&((yyvsp[-5].exp)));
	       add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
	       add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
         }
#line 5261 "y.tab.c"
    break;

  case 173:
#line 2344 "dilpar.y"
    {
            INITEXP((yyval.exp));
            if ((yyvsp[-3].exp).typ != DILV_INT)
               fatal("Arg 1 of 'rnd' not integer");
            else if ((yyvsp[-1].exp).typ != DILV_INT)
               fatal("Arg 2 of 'rnd' not integer");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_RND);
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
         }
#line 5286 "y.tab.c"
    break;

  case 174:
#line 2365 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=1;
            if ((yyvsp[-3].exp).typ != DILV_INT)
               fatal("Arg 1 of 'isset' not integer");
            else if ((yyvsp[-1].exp).typ != DILV_INT)
               fatal("Arg 2 of 'isset' not integer");
            else {
               /* Type is ok */
               /* Function is not _yet_ static */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_ISS);
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
         }
#line 5313 "y.tab.c"
    break;

  case 175:
#line 2388 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=1;
            if ((yyvsp[-3].exp).typ != DILV_UP)
               fatal("Arg 1 of 'isaff' not a unit");
            else if ((yyvsp[-1].exp).typ != DILV_INT)
               fatal("Arg 2 of 'isaff' not integer");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_ISA);
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
         }
#line 5339 "y.tab.c"
    break;

  case 176:
#line 2410 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    checkbool("argument 3 of findunit",(yyvsp[-3].exp).boolean);
            if ((yyvsp[-7].exp).typ != DILV_UP)
               fatal("Arg 1 of 'findunit' not a unit");
            else if ((yyvsp[-5].exp).typ != DILV_SP)
               fatal("Arg 2 of 'findunit' not a string");
            else if ((yyvsp[-3].exp).typ != DILV_INT)
               fatal("Arg 3 of 'findunit' not integer");
            else if ((yyvsp[-1].exp).typ != DILV_UP &&
                (yyvsp[-1].exp).typ != DILV_NULL)
               fatal("Arg 4 of 'findunit' not unit");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_UP;
               make_code(&((yyvsp[-7].exp)));
               make_code(&((yyvsp[-5].exp)));
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_FNDU);
               add_code(&((yyval.exp)),&((yyvsp[-7].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-5].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-7].exp));
            FREEEXP((yyvsp[-5].exp));
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
         }
#line 5376 "y.tab.c"
    break;

  case 177:
#line 2443 "dilpar.y"
    {
            INITEXP((yyval.exp));

            if ((yyvsp[-5].exp).typ != DILV_UP)
               fatal("Arg 1 of 'findrndunit' not an unitptr.");
            else if ((yyvsp[-3].exp).typ != DILV_INT)
               fatal("Arg 2 of 'findrndunit' not an integer.");
            else if ((yyvsp[-1].exp).typ != DILV_INT)
               fatal("Arg 3 of 'findrndunit' not an integer");
            else
	    {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_UP;
               make_code(&((yyvsp[-5].exp)));
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_FNDRU);
               add_code(&((yyval.exp)),&((yyvsp[-5].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-5].exp));
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
         }
#line 5408 "y.tab.c"
    break;

  case 178:
#line 2471 "dilpar.y"
    {   
            INITEXP((yyval.exp));
            if ((yyvsp[-1].exp).typ != DILV_SP)
               fatal("Arg 1 of 'findroom' not string");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_UP;
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_FNDR);
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
	    FREEEXP((yyvsp[-1].exp));
         }
#line 5428 "y.tab.c"
    break;

  case 179:
#line 2487 "dilpar.y"
    {   
            INITEXP((yyval.exp));
            if ((yyvsp[-1].exp).typ != DILV_SP)
               fatal("Arg 1 of 'findsymbolic' not string");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_UP;
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_FNDS);
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-1].exp));
         }
#line 5448 "y.tab.c"
    break;

  case 180:
#line 2503 "dilpar.y"
    {   
            INITEXP((yyval.exp));
            if ((yyvsp[-5].exp).typ != DILV_UP)
	      fatal("Arg 1 of 'findsymbolic' not a unitptr");
	    else if ((yyvsp[-3].exp).typ != DILV_SP)
	      fatal("Arg 2 of 'findsymbolic' not a string");
	    else if ((yyvsp[-1].exp).typ != DILV_INT)
	      fatal("Arg 3 of 'findsymbolic' not an integer");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_UP;
               make_code(&((yyvsp[-5].exp)));
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_FNDS2);
               add_code(&((yyval.exp)),&((yyvsp[-5].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-5].exp));
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
         }
#line 5478 "y.tab.c"
    break;

  case 181:
#line 2529 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=1;
            if ((yyvsp[-3].exp).typ != DILV_UP)
               fatal("Arg 1 of 'visible' not a unitptr");
            else if ((yyvsp[-1].exp).typ != DILV_UP)
               fatal("Arg 2 of 'visible' not a unitptr");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_VISI);
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 5504 "y.tab.c"
    break;

  case 182:
#line 2551 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=1;
            if ((yyvsp[-3].exp).typ != DILV_UP)
               fatal("Arg 1 of 'paycheck' not a unitptr");
            else if ((yyvsp[-1].exp).typ != DILV_UP)
               fatal("Arg 2 of 'paycheck' not a unitptr");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_PCK);
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 5530 "y.tab.c"
    break;

  case 183:
#line 2573 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=1;
            if ((yyvsp[-3].exp).typ != DILV_UP)
               fatal("Arg 1 of 'opponent' not a unitptr");
            else if ((yyvsp[-1].exp).typ != DILV_UP)
               fatal("Arg 2 of 'opponent' not a unitptr");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)), DILE_OPPO);
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 5556 "y.tab.c"
    break;

  case 184:
#line 2595 "dilpar.y"
    {
            INITEXP((yyval.exp));
            if ((yyvsp[-1].exp).typ != DILV_SP)
               fatal("Arg 1 of 'spellindex' not a string");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)), DILE_SPLX);
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-1].exp));
         }
#line 5576 "y.tab.c"
    break;

  case 185:
#line 2612 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=1;

            if ((yyvsp[-15].exp).typ != DILV_INT)
	      fatal("Arg 1 of 'spellinfo' not an integer");
            else if ((yyvsp[-13].exp).typ != DILV_INT || (yyvsp[-13].exp).dsl != DSL_LFT)
	      fatal("Arg 2 of 'spellinfo' not an integer variable");
            else if ((yyvsp[-11].exp).typ != DILV_INT || (yyvsp[-11].exp).dsl != DSL_LFT)
	      fatal("Arg 3 of 'spellinfo' not an integer variable");
            else if ((yyvsp[-9].exp).typ != DILV_INT || (yyvsp[-9].exp).dsl != DSL_LFT)
	      fatal("Arg 4 of 'spellinfo' not an integer variable");
            else if ((yyvsp[-7].exp).typ != DILV_INT || (yyvsp[-7].exp).dsl != DSL_LFT)
	      fatal("Arg 5 of 'spellinfo' not an integer variable");
            else if ((yyvsp[-5].exp).typ != DILV_INT || (yyvsp[-5].exp).dsl != DSL_LFT)
	      fatal("Arg 6 of 'spellinfo' not an integer variable");
            else if ((yyvsp[-3].exp).typ != DILV_INT || (yyvsp[-3].exp).dsl != DSL_LFT)
	      fatal("Arg 7 of 'spellinfo' not an integer variable");
            else if ((yyvsp[-1].exp).typ != DILV_INT || (yyvsp[-1].exp).dsl != DSL_LFT)
	      fatal("Arg 8 of 'spellinfo' not an integer variable");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_SP;

               make_code(&((yyvsp[-15].exp)));
               make_code(&((yyvsp[-13].exp)));
               make_code(&((yyvsp[-11].exp)));
               make_code(&((yyvsp[-9].exp)));
               make_code(&((yyvsp[-7].exp)));
               make_code(&((yyvsp[-5].exp)));
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));

               add_ubit8(&((yyval.exp)), DILE_SPLI);

               add_code(&((yyval.exp)),&((yyvsp[-15].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-13].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-11].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-9].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-7].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-5].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-15].exp));
            FREEEXP((yyvsp[-13].exp));
            FREEEXP((yyvsp[-11].exp));
            FREEEXP((yyvsp[-9].exp));
            FREEEXP((yyvsp[-7].exp));
            FREEEXP((yyvsp[-5].exp));
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
         }
#line 5636 "y.tab.c"
    break;

  case 186:
#line 2668 "dilpar.y"
    {
            INITEXP((yyval.exp));
            if ((yyvsp[-3].exp).typ != DILV_INT)
               fatal("Arg 1 of 'money_string' not an integer");
            else if ((yyvsp[-1].exp).typ != DILV_INT)
               fatal("Arg 2 of 'money_string' not a boolean");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_SP;
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)), DILE_MONS);
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 5661 "y.tab.c"
    break;

  case 187:
#line 2689 "dilpar.y"
    {
            INITEXP((yyval.exp));
            if ((yyvsp[-3].exp).typ != DILV_UP)
               fatal("Arg 1 of 'pathto' not a unitptr");
            else if ((yyvsp[-1].exp).typ != DILV_UP)
               fatal("Arg 2 of 'pathto' not a unitptr");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)), DILE_PATH);
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 5686 "y.tab.c"
    break;

  case 188:
#line 2710 "dilpar.y"
    {
            INITEXP((yyval.exp));

            if ((yyvsp[-5].exp).typ != DILV_UP)
               fatal("Arg 1 of 'can_carry' not a unitptr");
            else if ((yyvsp[-3].exp).typ != DILV_UP)
               fatal("Arg 2 of 'can_carry' not a unitptr");
            else if ((yyvsp[-1].exp).typ != DILV_INT)
 	       fatal("Arg 3 of 'can_carry' not an integer");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-5].exp)));
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)), DILE_CARY);
               add_code(&((yyval.exp)),&((yyvsp[-5].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-5].exp));
            FREEEXP((yyvsp[-3].exp));
	 }
#line 5716 "y.tab.c"
    break;

  case 189:
#line 2736 "dilpar.y"
    {
            INITEXP((yyval.exp));

            if ((yyvsp[-3].exp).typ != DILV_UP)
               fatal("Arg 1 of 'purse' not a unitptr");
            else if ((yyvsp[-1].exp).typ != DILV_SP)
               fatal("Arg 2 of 'purse' not a string");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)), DILE_PURS);
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 5742 "y.tab.c"
    break;

  case 190:
#line 2758 "dilpar.y"
    {
            INITEXP((yyval.exp));

            if (((yyvsp[-5].exp).typ != DILV_UP) && ((yyvsp[-5].exp).typ != DILV_NULL))
               fatal("Arg 1 of 'transfermoney' not a unitptr");
            else if (((yyvsp[-3].exp).typ != DILV_UP) && ((yyvsp[-3].exp).typ != DILV_NULL))
               fatal("Arg 2 of 'transfermoney' not a unitptr");
            else if ((yyvsp[-1].exp).typ != DILV_INT)
               fatal("Arg 3 of 'transfermoney' not an integer");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-5].exp)));
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)), DILE_TRMO);
               add_code(&((yyval.exp)),&((yyvsp[-5].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-5].exp));
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 5773 "y.tab.c"
    break;

  case 191:
#line 2785 "dilpar.y"
    {
            INITEXP((yyval.exp));
            if ((yyvsp[-1].exp).typ != DILV_SP)
               fatal("Arg 1 of 'load' not name string");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_UP;
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_LOAD);
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-1].exp));
         }
#line 5793 "y.tab.c"
    break;

  case 192:
#line 2802 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    checkbool("argument 1 of attack_spell", (yyvsp[-9].exp).boolean);

            if ((yyvsp[-9].exp).typ != DILV_INT)
               fatal("Arg 1 of 'attack_spell' not a number");
            else if ((yyvsp[-7].exp).typ != DILV_UP)
               fatal("Arg 2 of 'attack_spell' not a unitptr");
            else if ((yyvsp[-5].exp).typ != DILV_UP)
               fatal("Arg 3 of 'attack_spell' not a unitptr");
            else if ((yyvsp[-3].exp).typ != DILV_UP)
               fatal("Arg 4 of 'attack_spell' not a unitptr");
            else if ((yyvsp[-1].exp).typ != DILV_INT)
               fatal("Arg 5 of 'attack_spell' not an integer");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;

               make_code(&((yyvsp[-9].exp)));
               make_code(&((yyvsp[-7].exp)));
               make_code(&((yyvsp[-5].exp)));
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));

               add_ubit8(&((yyval.exp)), DILE_ATSP);

               add_code(&((yyval.exp)),&((yyvsp[-9].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-7].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-5].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }

            FREEEXP((yyvsp[-9].exp));
            FREEEXP((yyvsp[-7].exp));
            FREEEXP((yyvsp[-5].exp));
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 5839 "y.tab.c"
    break;

  case 193:
#line 2845 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    checkbool("argument 1 of cast",(yyvsp[-9].exp).boolean);

            if ((yyvsp[-9].exp).typ != DILV_INT)
               fatal("Arg 1 of 'cast_spell' not a number");
            else if ((yyvsp[-7].exp).typ != DILV_UP)
               fatal("Arg 2 of 'cast_spell' not a unitptr");
            else if ((yyvsp[-5].exp).typ != DILV_UP)
               fatal("Arg 3 of 'cast_spell' not a unitptr");
            else if ((yyvsp[-3].exp).typ != DILV_UP)
               fatal("Arg 4 of 'cast_spell' not a unitptr");
            else if ((yyvsp[-1].exp).typ != DILV_SP)
               fatal("Arg 5 of 'cast_spell' not a string");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;

               make_code(&((yyvsp[-9].exp)));
               make_code(&((yyvsp[-7].exp)));
               make_code(&((yyvsp[-5].exp)));
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));

               add_ubit8(&((yyval.exp)), DILE_CAST2);

               add_code(&((yyval.exp)),&((yyvsp[-9].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-7].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-5].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }

            FREEEXP((yyvsp[-9].exp));
            FREEEXP((yyvsp[-7].exp));
            FREEEXP((yyvsp[-5].exp));
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 5885 "y.tab.c"
    break;

  case 194:
#line 2887 "dilpar.y"
    {
            INITEXP((yyval.exp));

            if ((yyvsp[-5].exp).typ != DILV_UP)
	      fatal("Arg 1 of 'fits' not an unitptr.");
            else if ((yyvsp[-3].exp).typ != DILV_UP)
	      fatal("Arg 2 of 'fits' not an unitptr.");
            else if ((yyvsp[-1].exp).typ != DILV_INT)
	      fatal("Arg 3 of 'fits' not an integer.");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_SP;

               make_code(&((yyvsp[-5].exp)));
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));

               add_ubit8(&((yyval.exp)), DILE_FIT);

               add_code(&((yyval.exp)),&((yyvsp[-5].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }

            FREEEXP((yyvsp[-5].exp));
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
         }
#line 5920 "y.tab.c"
    break;

  case 195:
#line 2918 "dilpar.y"
    {
            INITEXP((yyval.exp));

            if ((yyvsp[-3].exp).typ != DILV_SP)
	      fatal("Arg 1 of 'restore' not a string");
            else if ((yyvsp[-1].exp).typ != DILV_SP)
	      fatal("Arg 2 of 'restore' not a string");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_UP;

               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));

               add_ubit8(&((yyval.exp)), DILE_REST);

               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }

            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 5950 "y.tab.c"
    break;

  case 196:
#line 2944 "dilpar.y"
    {
            INITEXP((yyval.exp));

            if ((yyvsp[-3].exp).typ != DILV_INT)
	      fatal("Arg 1 of 'openroll' not an integer");
            else if ((yyvsp[-1].exp).typ != DILV_INT)
	      fatal("Arg 2 of 'openroll' not an integer");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;

               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));

               add_ubit8(&((yyval.exp)), DILE_OPRO);

               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }

            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 5980 "y.tab.c"
    break;

  case 197:
#line 2970 "dilpar.y"
    {
            INITEXP((yyval.exp));

            if ((yyvsp[-3].exp).typ != DILV_UP)
	      fatal("Arg 1 of 'equipment' not a unitptr");
            else if ((yyvsp[-1].exp).typ != DILV_INT)
	      fatal("Arg 2 of 'equipment' not a unitptr");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_UP;

               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));

               add_ubit8(&((yyval.exp)), DILE_EQPM);

               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }

            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 6010 "y.tab.c"
    break;

  case 198:
#line 2996 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    checkbool("argument 1 of MeleeAttack",(yyvsp[-7].exp).boolean);

            if ((yyvsp[-7].exp).typ != DILV_UP)
	      fatal("Arg 1 of 'MeleeAttack' not a unitptr");
            else if ((yyvsp[-5].exp).typ != DILV_UP)
	      fatal("Arg 2 of 'MeleeAttack' not a unitptr");
            else if ((yyvsp[-3].exp).typ != DILV_INT)
	      fatal("Arg 3 of 'MeleeAttack' not an integer");	      
            else if ((yyvsp[-1].exp).typ != DILV_INT)
	      fatal("Arg 4 of 'MeleeAttack' not an integer");	      
            else
	    {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;

               make_code(&((yyvsp[-7].exp)));
               make_code(&((yyvsp[-5].exp)));
               make_code(&((yyvsp[-3].exp)));
               make_code(&((yyvsp[-1].exp)));

               add_ubit8(&((yyval.exp)), DILE_MEL);

               add_code(&((yyval.exp)),&((yyvsp[-7].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-5].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-3].exp)));
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }

            FREEEXP((yyvsp[-7].exp));
            FREEEXP((yyvsp[-5].exp));
            FREEEXP((yyvsp[-3].exp));
            FREEEXP((yyvsp[-1].exp));
	 }
#line 6052 "y.tab.c"
    break;

  case 199:
#line 3034 "dilpar.y"
    {
            INITEXP((yyval.exp));
            if ((yyvsp[-1].exp).typ != DILV_SP)
               fatal("Arg 1 of 'textformat' not string variable");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_SP;
               add_ubit8(&((yyval.exp)),DILE_TXF);
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-1].exp));
         }
#line 6071 "y.tab.c"
    break;

  case 200:
#line 3049 "dilpar.y"
    {
            INITEXP((yyval.exp));
            if ((yyvsp[-1].exp).typ != DILV_INT)
               fatal("Arg 1 of 'asctime' not integer");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_SP;
               add_ubit8(&((yyval.exp)),DILE_AST);
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-1].exp));
         }
#line 6090 "y.tab.c"
    break;

  case 201:
#line 3064 "dilpar.y"
    {
            INITEXP((yyval.exp));
            if ((yyvsp[-1].exp).typ != DILV_SP)
               fatal("Arg 1 of 'getword' not string variable");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_SP;
               add_ubit8(&((yyval.exp)),DILE_GETW);
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-1].exp));
         }
#line 6109 "y.tab.c"
    break;

  case 202:
#line 3079 "dilpar.y"
    {
            INITEXP((yyval.exp));
            if ((yyvsp[-1].exp).typ != DILV_SP)
               fatal("Arg 1 of 'getwords' not string");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_SLP;
               add_ubit8(&((yyval.exp)),DILE_GETWS);
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            }
            FREEEXP((yyvsp[-1].exp));
         }
#line 6128 "y.tab.c"
    break;

  case 203:
#line 3094 "dilpar.y"
    {
            INITEXP((yyval.exp));
	    (yyval.exp).boolean=1;
            if ((yyvsp[-1].exp).typ == DILV_SP)
            {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_CMDS);
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            } else if ((yyvsp[-1].exp).typ == DILV_INT) {
               /* Type is ok */
               /* Make nodes dynamic */
               (yyval.exp).dsl = DSL_DYN;
               (yyval.exp).typ = DILV_INT;
               make_code(&((yyvsp[-1].exp)));
               add_ubit8(&((yyval.exp)),DILE_CMDS);
               add_code(&((yyval.exp)),&((yyvsp[-1].exp)));
            } else
                fatal("Arg 1 of 'command' not a string or an integer");
            FREEEXP((yyvsp[-1].exp));
         }
#line 6157 "y.tab.c"
    break;

  case 204:
#line 3121 "dilpar.y"
    {
            int i;

            CREATE((yyval.str_list), char *, (str_top + 2));
            for (i = 0; i <= str_top; i++)
               (yyval.str_list)[i] = str_list[i];
            (yyval.str_list)[i] = 0;
         }
#line 6170 "y.tab.c"
    break;

  case 205:
#line 3132 "dilpar.y"
    {
            str_list[str_top = 0] = (yyvsp[0].str);
         }
#line 6178 "y.tab.c"
    break;

  case 206:
#line 3136 "dilpar.y"
    {
            str_list[++str_top] = (yyvsp[0].str);
	    if (str_top > sizeof(str_list) - 3)
	      fatal("Static stringlist has too many elements.");
         }
#line 6188 "y.tab.c"
    break;

  case 207:
#line 3148 "dilpar.y"
    {
            int i;

/*          fprintf(stderr,"COREEXP\n");*/
	    if (((yyvsp[0].exp).typ==DILV_PROC)||((yyvsp[0].exp).typ==DILV_FUNC))
	      fatal("Illegal use of proc/func");

            make_code(&((yyvsp[0].exp)));
	    (yyval.ins).boolean=(yyvsp[0].exp).boolean;
            /* write dynamic expression in core */
            (yyval.ins).fst = wcore - tmpl.core;
            moredilcore((yyvsp[0].exp).codep - (yyvsp[0].exp).code);
            for(i=0;i<(yyvsp[0].exp).codep-(yyvsp[0].exp).code;i++,wcore++)
               *wcore=(yyvsp[0].exp).code[i];
            (yyval.ins).lst = wcore - tmpl.core;
            (yyval.ins).dsl = (yyvsp[0].exp).dsl;
            (yyval.ins).typ = (yyvsp[0].exp).typ;
            FREEEXP((yyvsp[0].exp));
         }
#line 6212 "y.tab.c"
    break;

  case 208:
#line 3174 "dilpar.y"
    {
            int i;
/*          fprintf(stderr,"COREVAR\n");*/
	    if (((yyvsp[0].exp).dsl != DSL_LFT)&&
		((yyvsp[0].exp).dsl != DSL_FCT)&&
		((yyvsp[0].exp).dsl != DSL_SFT))
	      fatal("Illegal lvalue");
            make_code(&((yyvsp[0].exp))); /* does nothing!? */
	    (yyval.ins).boolean=(yyvsp[0].exp).boolean;
            /* write dynamic expression in core */
            (yyval.ins).fst = wcore - tmpl.core;
            moredilcore((yyvsp[0].exp).codep-(yyvsp[0].exp).code);
            for(i=0;i<(yyvsp[0].exp).codep-(yyvsp[0].exp).code;i++,wcore++)
               *wcore=(yyvsp[0].exp).code[i];
            (yyval.ins).lst = wcore - tmpl.core;
	    /* fprintf(stderr, "Refnum supposedly : %d, %d == %d, %d\n",
		    $1.code[0], $1.code[1], *(wcore-2), *(wcore-1));
	    fprintf(stderr, "Corevar : fst %p, lst %p.\n",
		    $$.fst, $$.lst);*/
            (yyval.ins).dsl = (yyvsp[0].exp).dsl;
            (yyval.ins).typ = (yyvsp[0].exp).typ;
            FREEEXP((yyvsp[0].exp));
         }
#line 6240 "y.tab.c"
    break;

  case 209:
#line 3204 "dilpar.y"
    {
            (yyval.num) = wcore - tmpl.core;
            wcore++;      /* ubit8 */
         }
#line 6249 "y.tab.c"
    break;

  case 210:
#line 3211 "dilpar.y"
    {
            wcore--;      /* ubit8 */
            (yyval.num) = wcore - tmpl.core;
	 }
#line 6258 "y.tab.c"
    break;

  case 211:
#line 3217 "dilpar.y"
    {
            (yyval.num) = wcore - tmpl.core;
            wcore+=4;   /* ubit32 */
         }
#line 6267 "y.tab.c"
    break;

  case 212:
#line 3224 "dilpar.y"
    {
            /* lable reference */
            (yyval.ins).fst = wcore - tmpl.core;
            moredilcore(4);
            bwrite_ubit32(&wcore,get_label((yyvsp[0].sym),wcore-tmpl.core)); /* here */
            (yyval.ins).lst = wcore - tmpl.core;
         }
#line 6279 "y.tab.c"
    break;

  case 213:
#line 3234 "dilpar.y"
    {
            (yyval.ins) = (yyvsp[0].ins);
         }
#line 6287 "y.tab.c"
    break;

  case 214:
#line 3238 "dilpar.y"
    {
            /* 'skip' reference */
            (yyval.ins).fst = wcore - tmpl.core;
            moredilcore(4);
            bwrite_ubit32(&wcore,SKIP);  /* address null value */
            (yyval.ins).lst = wcore - tmpl.core;
         }
#line 6299 "y.tab.c"
    break;

  case 215:
#line 3248 "dilpar.y"
    {
           (yyval.ins).typ=(yyvsp[0].ins).typ+1;
	   (yyval.ins).fst=(yyvsp[-2].ins).fst;
	   (yyval.ins).lst=(yyvsp[0].ins).lst;
         }
#line 6309 "y.tab.c"
    break;

  case 216:
#line 3254 "dilpar.y"
    {
           (yyval.ins).typ=1;
	   (yyval.ins).fst=(yyvsp[0].ins).fst;
	   (yyval.ins).lst=(yyvsp[0].ins).lst;
         }
#line 6319 "y.tab.c"
    break;

  case 217:
#line 3262 "dilpar.y"
    {
            char nbuf[255],zbuf[255],buf[255];
            int i;

            /* ignore returning variable ... */
	    if ((yyvsp[0].ins).typ == DILV_SP) {

	       /*
		* Write remote symbolic procedure call
		*
		* <rspcall> <var exp> <nargs> <arg> <arg> ...
		*/

	       wtmp = &tmpl.core[(yyvsp[-1].num)];
	       bwrite_ubit8(&wtmp,DILI_SPC);
	       (yyval.ins).fst = (yyvsp[-1].num);
	       (yyval.ins).lst = wcore - tmpl.core;
	    } else {
	       /*
		* Write remote normal procedure call
		* 
		* <rpcall> <funcnumber> <arg> <arg> ...
		*/

	       if (((yyvsp[0].ins).dsl != DSL_FCT) ||
		   ((yyvsp[0].ins).typ != DILV_NULL))
		 fatal ("refernce not a procedure");

	       wtmp = &tmpl.core[(yyvsp[-1].num)];
	       bwrite_ubit8(&wtmp,DILI_RPC);

	       (yyval.ins).fst = (yyvsp[-1].num);
	       (yyval.ins).lst = wcore - tmpl.core;
	    }
         }
#line 6359 "y.tab.c"
    break;

  case 218:
#line 3298 "dilpar.y"
    {
            if ((yyvsp[-1].ins).typ != DILV_INT)
               fatal("Arg 1 of 'clear' not an integer");
            else {
               (yyval.ins).fst = (yyvsp[-3].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-3].num)];
               bwrite_ubit8(&wtmp,DILI_CLI);
            }
         }
#line 6374 "y.tab.c"
    break;

  case 219:
#line 3309 "dilpar.y"
    {
            if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 1 of 'store' not an unitptr");
            else {
               (yyval.ins).fst = (yyvsp[-3].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-3].num)];
               bwrite_ubit8(&wtmp,DILI_STOR);
            }
         }
#line 6389 "y.tab.c"
    break;

  case 220:
#line 3320 "dilpar.y"
    {
            if ((yyvsp[-3].ins).typ != DILV_UP)
               fatal("Arg 1 of 'acc_modify' not an unitptr");
            else if ((yyvsp[-1].ins).typ != DILV_INT)
               fatal("Arg 2 of 'acc_modify' not an integer");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_AMOD);
            }
         }
#line 6406 "y.tab.c"
    break;

  case 221:
#line 3333 "dilpar.y"
    {
            if ((yyvsp[-3].ins).typ != DILV_UP)
               fatal("Arg 1 of 'follow' not an unitptr");
            else if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 2 of 'follow' not an unitptr");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_FOLO);
            }
         }
#line 6423 "y.tab.c"
    break;

  case 222:
#line 3346 "dilpar.y"
    {
            if ((yyvsp[-3].ins).typ != DILV_UP)
               fatal("Arg 1 of 'set_fighting' not an unitptr");
            else if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 2 of 'set_fighting' not an unitptr");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_SETF);
            }
         }
#line 6440 "y.tab.c"
    break;

  case 223:
#line 3359 "dilpar.y"
    {
	    checkbool("argument 2 of change_speed",(yyvsp[-1].ins).boolean);
            if ((yyvsp[-3].ins).typ != DILV_UP)
               fatal("Arg 1 of 'change_speed' not an unitptr");
            else if ((yyvsp[-1].ins).typ != DILV_INT)
               fatal("Arg 2 of 'change_speed' not an integer");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_CHAS);
            }
         }
#line 6458 "y.tab.c"
    break;

  case 224:
#line 3373 "dilpar.y"
    {
	    checkbool("argument 2 of setbright",(yyvsp[-1].ins).boolean);
            if ((yyvsp[-3].ins).typ != DILV_UP)
               fatal("Arg 1 of 'setbright' not an unitptr");
            else if ((yyvsp[-1].ins).typ != DILV_INT)
               fatal("Arg 2 of 'setbright' not an integer");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_SBT);
            }
         }
#line 6476 "y.tab.c"
    break;

  case 225:
#line 3387 "dilpar.y"
    {
	    checkbool("argument 2 of setweight",(yyvsp[-1].ins).boolean);
            if ((yyvsp[-3].ins).typ != DILV_UP)
               fatal("Arg 1 of 'setweight' not an unitptr");
            else if ((yyvsp[-1].ins).typ != DILV_INT)
               fatal("Arg 2 of 'setweight' not an integer");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_SWT);
            }
         }
#line 6494 "y.tab.c"
    break;

  case 226:
#line 3401 "dilpar.y"
    {
	    checkbool("argument 2 of set",(yyvsp[-1].ins).boolean);
            if ((yyvsp[-3].ins).typ != DILV_INT || (yyvsp[-3].ins).dsl != DSL_LFT)
               fatal("Arg 1 of 'set' not an integer variable");
            else if ((yyvsp[-1].ins).typ != DILV_INT)
               fatal("Arg 2 of 'set' not an integer");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_SET);
            }
         }
#line 6512 "y.tab.c"
    break;

  case 227:
#line 3415 "dilpar.y"
    {
            if ((yyvsp[-3].ins).typ != DILV_SP)
               fatal("Arg 1 of 'dilcopy' not a string");
            else if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 2 of 'dilcopy' not a unitptr");
            else
	    {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_DLC);
            }
         }
#line 6530 "y.tab.c"
    break;

  case 228:
#line 3429 "dilpar.y"
    {
            if ((yyvsp[-3].ins).typ != DILV_SP)
               fatal("Arg 1 of 'sendtext' not a string");
            else if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 2 of 'sendtext' not a unitptr");
            else
	    {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_SETE);
            }
         }
#line 6548 "y.tab.c"
    break;

  case 229:
#line 3443 "dilpar.y"
    {
	    checkbool("argument 2 of unset",(yyvsp[-1].ins).boolean);
            if ((yyvsp[-3].ins).typ != DILV_INT || (yyvsp[-3].ins).dsl != DSL_LFT)
               fatal("Arg 1 of 'unset' not an integer variable");
            else if ((yyvsp[-1].ins).typ != DILV_INT)
               fatal("Arg 2 of 'unset' not an integer");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_USET);
            }
         }
#line 6566 "y.tab.c"
    break;

  case 230:
#line 3457 "dilpar.y"
    {
	    if (((yyvsp[-5].ins).typ != DILV_EDP) || ((yyvsp[-5].ins).dsl != DSL_DYN))
               fatal("Arg 1 of 'addextra' not a unit extra description");
            else if ((yyvsp[-3].ins).typ != DILV_SLP)
               fatal("Arg 2 of 'addextra' not a stringlist");
            else if ((yyvsp[-1].ins).typ != DILV_SP)
               fatal("Arg 3 of 'addextra' not a string");
            else {
               (yyval.ins).fst = (yyvsp[-7].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-7].num)];
               bwrite_ubit8(&wtmp,DILI_ADE);
            }
         }
#line 6585 "y.tab.c"
    break;

  case 231:
#line 3472 "dilpar.y"
    {
            if ((yyvsp[-5].ins).typ != DILV_UP)
               fatal("Arg 1 of 'logcrime' not an unitptr");
            else if ((yyvsp[-3].ins).typ != DILV_UP)
               fatal("Arg 2 of 'logcrime' not an unitptr");
            else if ((yyvsp[-1].ins).typ != DILV_INT)
               fatal("Arg 3 of 'logcrime' not an integer");
            else {
               (yyval.ins).fst = (yyvsp[-7].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-7].num)];
               bwrite_ubit8(&wtmp,DILI_LCRI);
            }
         }
#line 6604 "y.tab.c"
    break;

  case 232:
#line 3487 "dilpar.y"
    {
	    checkbool("argument 1 of cast",(yyvsp[-7].ins).boolean);
            if ((yyvsp[-7].ins).typ != DILV_INT)
               fatal("Arg 1 of 'cast_spell' not a number");
            else if ((yyvsp[-5].ins).typ != DILV_UP)
               fatal("Arg 2 of 'cast_spell' not a unitptr");
            else if ((yyvsp[-3].ins).typ != DILV_UP)
               fatal("Arg 3 of 'cast_spell' not a unitptr");
            else if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 4 of 'cast_spell' not a unitptr");
            else {
               (yyval.ins).fst = (yyvsp[-9].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-9].num)];
               bwrite_ubit8(&wtmp,DILI_CAST);
            }
         }
#line 6626 "y.tab.c"
    break;

  case 233:
#line 3505 "dilpar.y"
    {
            if (((yyvsp[-3].ins).typ != DILV_SLP) || ((yyvsp[-3].ins).dsl != DSL_LFT))
               fatal("Arg 1 of 'addlist' not a stringlist variable");
            else if ((yyvsp[-1].ins).typ != DILV_SP)
               fatal("Arg 2 of 'addlist' not a string");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_ADL);
            }
         }
#line 6643 "y.tab.c"
    break;

  case 234:
#line 3518 "dilpar.y"
    {
	   /*fprintf(stderr, "SUE DSL %d TYP %d\n", $4.dsl, $4.typ);*/

	    if (((yyvsp[-3].ins).typ != DILV_EDP) || ((yyvsp[-3].ins).dsl != DSL_DYN))
               fatal("Arg 1 of 'subextra' not a unit extra description");
            else if ((yyvsp[-1].ins).typ != DILV_SP)
               fatal("Arg 2 of 'subextra' not a string");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_SUE);
            }
         }
#line 6662 "y.tab.c"
    break;

  case 235:
#line 3533 "dilpar.y"
    {
            if ((yyvsp[-3].ins).typ != DILV_SLP || (yyvsp[-3].ins).dsl != DSL_LFT)
               fatal("Arg 1 of 'sublist' not a stringlist variable");
            else if ((yyvsp[-1].ins).typ != DILV_SP)
               fatal("Arg 2 of 'sublist' not a string");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_SUL);
            }
         }
#line 6679 "y.tab.c"
    break;

  case 236:
#line 3546 "dilpar.y"
    {
	    checkbool("argument 2 of subaff",(yyvsp[-1].ins).boolean);
            if ((yyvsp[-3].ins).typ != DILV_UP)
               fatal("Arg 1 of 'subaff' not a unit");
            else if ((yyvsp[-1].ins).typ != DILV_INT)
               fatal("Arg 2 of 'subaff' not an integer");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_SUA);
            }
         }
#line 6697 "y.tab.c"
    break;

  case 237:
#line 3563 "dilpar.y"
    {
            if ((yyvsp[-21].ins).typ != DILV_UP)
               fatal("Arg 1 of 'addaff' not a unit");
            else if ((yyvsp[-19].ins).typ != DILV_INT)
               fatal("Arg 2 of 'addaff' not an integer");
            else if ((yyvsp[-17].ins).typ != DILV_INT)
               fatal("Arg 3 of 'addaff' not an integer");
            else if ((yyvsp[-15].ins).typ != DILV_INT)
               fatal("Arg 4 of 'addaff' not an integer");
            else if ((yyvsp[-13].ins).typ != DILV_INT)
               fatal("Arg 5 of 'addaff' not an integer");
            else if ((yyvsp[-11].ins).typ != DILV_INT)
               fatal("Arg 6 of 'addaff' not an integer");
            else if ((yyvsp[-9].ins).typ != DILV_INT)
               fatal("Arg 7 of 'addaff' not an integer");
            else if ((yyvsp[-7].ins).typ != DILV_INT)
               fatal("Arg 8 of 'addaff' not an integer");
            else if ((yyvsp[-5].ins).typ != DILV_INT)
               fatal("Arg 9 of 'addaff' not an integer");
            else if ((yyvsp[-3].ins).typ != DILV_INT)
               fatal("Arg 10 of 'addaff' not an integer");
            else if ((yyvsp[-1].ins).typ != DILV_INT)
               fatal("Arg 11 of 'addaff' not an integer");
            else   {
               (yyval.ins).fst = (yyvsp[-23].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-23].num)];
               bwrite_ubit8(&wtmp,DILI_ADA);
            }
         }
#line 6732 "y.tab.c"
    break;

  case 238:
#line 3594 "dilpar.y"
    {
            if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 1 of 'destruct' not a unit");
            else {
               (yyval.ins).fst = (yyvsp[-3].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-3].num)];
               bwrite_ubit8(&wtmp,DILI_DST);
            }
         }
#line 6747 "y.tab.c"
    break;

  case 239:
#line 3605 "dilpar.y"
    {
            if ((yyvsp[-1].ins).typ != DILV_SP)
               fatal("Arg 1 of 'log' not a string.");
            else {
               (yyval.ins).fst = (yyvsp[-3].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-3].num)];
               bwrite_ubit8(&wtmp,DILI_LOG);
            }
         }
#line 6762 "y.tab.c"
    break;

  case 240:
#line 3616 "dilpar.y"
    {
            if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 1 of 'position_update' not a unit");
            else {
               (yyval.ins).fst = (yyvsp[-3].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-3].num)];
               bwrite_ubit8(&wtmp,DILI_PUP);
            }
         }
#line 6777 "y.tab.c"
    break;

  case 241:
#line 3627 "dilpar.y"
    {
            if ((yyvsp[-3].ins).typ != DILV_UP)
               fatal("Arg 1 of 'link' not a unit");
            else if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 2 of 'link' not a unit");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_LNK);
            }
         }
#line 6794 "y.tab.c"
    break;

  case 242:
#line 3640 "dilpar.y"
    {
	    checkbool("argument 1 of experience",(yyvsp[-3].ins).boolean);
            if ((yyvsp[-3].ins).typ != DILV_INT)
               fatal("Arg 1 of 'experience' not an integer");
            else if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 2 of 'experience' not a unitptr");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_EXP);
            }
         }
#line 6812 "y.tab.c"
    break;

  case 243:
#line 3654 "dilpar.y"
    {
	    checkbool("argument 2 of act",(yyvsp[-9].ins).boolean);
	    checkbool("argument 6 of act",(yyvsp[-1].ins).boolean);
            if ((yyvsp[-11].ins).typ != DILV_SP)
               fatal("Arg 1 of 'act' not a string");
            else if ((yyvsp[-9].ins).typ != DILV_INT)
               fatal("Arg 2 of 'act' not an integer");
            else if ((yyvsp[-7].ins).typ != DILV_NULL &&
                (yyvsp[-7].ins).typ != DILV_SP &&
                (yyvsp[-7].ins).typ != DILV_UP)
               fatal("Arg 3 of 'act' not a unit or string");
            else if ((yyvsp[-5].ins).typ != DILV_NULL &&
                (yyvsp[-5].ins).typ != DILV_SP &&
                (yyvsp[-5].ins).typ != DILV_UP)
               fatal("Arg 4 of 'act' not a unit or string");
            else if ((yyvsp[-3].ins).typ != DILV_NULL &&
                (yyvsp[-3].ins).typ != DILV_SP &&
                (yyvsp[-3].ins).typ != DILV_UP)
               fatal("Arg 5 of 'act' not a unit or string");
            else if ((yyvsp[-1].ins).typ != DILV_INT)
               fatal("Arg 6 of 'act' not an integer");
            else {
               /* make code */
               (yyval.ins).fst = (yyvsp[-13].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-13].num)];
               bwrite_ubit8(&wtmp,DILI_ACT);
            }
         }
#line 6846 "y.tab.c"
    break;

  case 244:
#line 3684 "dilpar.y"
    {
            if ((yyvsp[-3].ins).typ != DILV_SP)
               fatal("Arg 1 of 'exec' not a string");
            else if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 2 of 'exec' not a unit");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_EXEC);
            }
         }
#line 6863 "y.tab.c"
    break;

  case 245:
#line 3697 "dilpar.y"
    {
	    checkbool("argument 1 of wait",(yyvsp[-3].ins).boolean);
            if ((yyvsp[-3].ins).typ != DILV_INT)
               fatal("Arg 1 of 'wait' not an integer");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_WIT);
            }
         }
#line 6879 "y.tab.c"
    break;

  case 246:
#line 3709 "dilpar.y"
    {
            if ((yyvsp[-1].ins).typ != DILV_SP)
               fatal("Arg 1 of 'send' not a string");
            else {
               (yyval.ins).fst = (yyvsp[-3].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-3].num)];
               bwrite_ubit8(&wtmp,DILI_SND);
            }
         }
#line 6894 "y.tab.c"
    break;

  case 247:
#line 3720 "dilpar.y"
    {
            if ((yyvsp[-3].ins).typ != DILV_SP)
               fatal("Arg 1 of 'sendto' not a string");
            else if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 2 of 'sendto' not a unit");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_SNT);
            }
         }
#line 6911 "y.tab.c"
    break;

  case 248:
#line 3733 "dilpar.y"
    {
            if ((yyvsp[-3].ins).typ != DILV_SP)
               fatal("Arg 1 of 'sendtoall' not a string");
            else if ((yyvsp[-1].ins).typ != DILV_SP)
               fatal("Arg 2 of 'sendtoall' not a string");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_SNTA);
            }
         }
#line 6928 "y.tab.c"
    break;

  case 249:
#line 3746 "dilpar.y"
    {
            if ((yyvsp[-3].ins).typ != DILV_SP)
               fatal("Arg 1 of 'sendtoalldil' not a string");
            else if ((yyvsp[-1].ins).typ != DILV_SP)
               fatal("Arg 2 of 'sendtoalldil' not a string");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_SNTADIL);
            }
         }
#line 6945 "y.tab.c"
    break;

  case 250:
#line 3759 "dilpar.y"
    {
            if ((yyvsp[-3].ins).dsl != DSL_LFT)
               fatal("Arg 1 of 'secure' not a user-variable.");
            else if ((yyvsp[-3].ins).typ != DILV_UP)
               fatal("Arg 1 of 'secure' not a unit");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_SEC);
            }
         }
#line 6962 "y.tab.c"
    break;

  case 251:
#line 3772 "dilpar.y"
    {
            if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 1 of 'unsecure' not a unit");
            else { 
               (yyval.ins).fst = (yyvsp[-3].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-3].num)];
               bwrite_ubit8(&wtmp,DILI_USE);
            }
         }
#line 6977 "y.tab.c"
    break;

  case 252:
#line 3783 "dilpar.y"
    {
            (yyval.ins).fst = (yyvsp[0].num);
            (yyval.ins).lst = (yyvsp[0].num)+1;
            wtmp = &tmpl.core[(yyvsp[0].num)];
            bwrite_ubit8(&wtmp,DILI_QUIT);
         }
#line 6988 "y.tab.c"
    break;

  case 253:
#line 3790 "dilpar.y"
    {
            (yyval.ins).fst = (yyvsp[0].num);
            (yyval.ins).lst = (yyvsp[0].num)+1;
            wtmp = &tmpl.core[(yyvsp[0].num)];
            bwrite_ubit8(&wtmp,DILI_BLK);
         }
#line 6999 "y.tab.c"
    break;

  case 254:
#line 3797 "dilpar.y"
    {
            (yyval.ins).fst = (yyvsp[0].num);
            (yyval.ins).lst = (yyvsp[0].num)+1;
            wtmp = &tmpl.core[(yyvsp[0].num)];
            bwrite_ubit8(&wtmp,DILI_PRI);
         }
#line 7010 "y.tab.c"
    break;

  case 255:
#line 3804 "dilpar.y"
    {
            (yyval.ins).fst = (yyvsp[0].num);
            (yyval.ins).lst = (yyvsp[0].num)+1;
            wtmp = &tmpl.core[(yyvsp[0].num)];
            bwrite_ubit8(&wtmp,DILI_NPR);
         }
#line 7021 "y.tab.c"
    break;

  case 256:
#line 3811 "dilpar.y"
    {
            if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 1 of 'walkto' not a unit");
            else {
               (yyval.ins).fst = (yyvsp[-3].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-3].num)];
               bwrite_ubit8(&wtmp,DILI_WALK);
            }
         }
#line 7036 "y.tab.c"
    break;

  case 257:
#line 3822 "dilpar.y"
    {
	    checkbool("argument 1 of addequip",(yyvsp[-3].ins).boolean);
            if ((yyvsp[-3].ins).typ != DILV_UP)
               fatal("Arg 1 of 'addequip' not a unit");
            else if ((yyvsp[-1].ins).typ != DILV_INT)
               fatal("Arg 2 of 'addequip' not an integer");
            else {
               (yyval.ins).fst = (yyvsp[-5].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-5].num)];
               bwrite_ubit8(&wtmp,DILI_EQP);
            }
         }
#line 7054 "y.tab.c"
    break;

  case 258:
#line 3836 "dilpar.y"
    {
            if ((yyvsp[-1].ins).typ != DILV_UP)
               fatal("Arg 1 of 'unequip' not a unit");
            else {
               (yyval.ins).fst = (yyvsp[-3].num);
               (yyval.ins).lst = (yyvsp[-1].ins).lst;
               wtmp = &tmpl.core[(yyvsp[-3].num)];
               bwrite_ubit8(&wtmp,DILI_UEQ);
            }
         }
#line 7069 "y.tab.c"
    break;

  case 259:
#line 3853 "dilpar.y"
    {
            /*
             * Defines name and pushes
             * break label on break stack
             * Inserted in structure from point
             * where break is allowed.
             */
             char buf[255];
			 	
             /* create label */
             sprintf(buf,"__break%d",labelgen++);
			 	
             if (break_no) {
                /* reallocate break stack */
                RECREATE(break_idx,ubit16,break_no+1);
             } else {
                /* allocate new break stack */
                CREATE(break_idx,ubit16,break_no+1);
             }
             break_idx[break_no++]=add_label(buf,SKIP);
         }
#line 7095 "y.tab.c"
    break;

  case 260:
#line 3876 "dilpar.y"
    {
             /*
              * Defines address of break label on top of stack.
              */
             if (!break_no)
                fatal("INTERNAL ERROR (break stack def)");
             else
                label_adr[break_idx[break_no-1]] = wcore-tmpl.core;
         }
#line 7109 "y.tab.c"
    break;

  case 261:
#line 3887 "dilpar.y"
    {
             /*
              * Pops the current break label from the break stack.
              * Inserted in structure from point where
              * break is no longer alowed
              */
             if (!break_no)
                fatal("INTERNAL ERROR (break stack def)");
             else
                break_no--;
         }
#line 7125 "y.tab.c"
    break;

  case 262:
#line 3900 "dilpar.y"
    {
             /*
              * Defines name and pushes
              * cont label on cont stack
              * Inserted in structure from point
              * where cont is allowed.
              */
             char buf[255];
			 	
             /* create label */
             sprintf(buf,"__cont%d",labelgen++);
			 	
             if (cont_no) {
                /* reallocate cont stack */
                RECREATE(cont_idx,ubit16,cont_no+1);
             } else {
                /* allocate new cont stack */
                CREATE(cont_idx,ubit16,cont_no+1);
             }
             cont_idx[cont_no++]=add_label(buf,SKIP);		 		
         }
#line 7151 "y.tab.c"
    break;

  case 263:
#line 3923 "dilpar.y"
    {
            /*
             * Defines address of cont label on top of stack.
             */
            if (!cont_no)
               fatal("INTERNAL ERROR (cont stack def)");
            else
               label_adr[cont_idx[cont_no-1]] = wcore-tmpl.core;
         }
#line 7165 "y.tab.c"
    break;

  case 264:
#line 3934 "dilpar.y"
    {
            /*
             * Pops the current cont label from the cont stack.
             * Inserted in structure from point where
             * cont is no longer alowed
             */
            if (!cont_no)
               fatal("INTERNAL ERROR (cont stack def)");
            else 
               cont_no--;
         }
#line 7181 "y.tab.c"
    break;

  case 269:
#line 3955 "dilpar.y"
    { (yyval.ins) = (yyvsp[-1].ins); }
#line 7187 "y.tab.c"
    break;

  case 270:
#line 3959 "dilpar.y"
    { (yyval.ins) = (yyvsp[0].ins); }
#line 7193 "y.tab.c"
    break;

  case 271:
#line 3961 "dilpar.y"
    { (yyval.ins).fst = (yyvsp[-1].ins).fst; (yyval.ins).lst = (yyvsp[0].ins).lst; }
#line 7199 "y.tab.c"
    break;

  case 272:
#line 3965 "dilpar.y"
    { (yyval.ins) = (yyvsp[-2].ins); }
#line 7205 "y.tab.c"
    break;

  case 273:
#line 3967 "dilpar.y"
    { (yyval.ins) = (yyvsp[-1].ins); }
#line 7211 "y.tab.c"
    break;

  case 274:
#line 3969 "dilpar.y"
    { (yyval.ins) = (yyvsp[0].ins); }
#line 7217 "y.tab.c"
    break;

  case 275:
#line 3973 "dilpar.y"
    {
            wtmp = &tmpl.core[(yyvsp[-9].num)];
            bwrite_ubit8(&wtmp,DILI_IF); /* the instruction */
            wtmp = &tmpl.core[(yyvsp[-5].num)];
            bwrite_ubit32(&wtmp,(yyvsp[0].ins).fst); /* address of else */
            wtmp = &tmpl.core[(yyvsp[-3].num)];
            bwrite_ubit8(&wtmp,DILI_GOTO); /* skip else */
            wtmp = &tmpl.core[(yyvsp[-2].num)];
            bwrite_ubit32(&wtmp,(yyvsp[0].ins).lst); /* end of else */
            (yyval.ins).fst = (yyvsp[-9].num);
            (yyval.ins).lst = (yyvsp[0].ins).lst;
         }
#line 7234 "y.tab.c"
    break;

  case 276:
#line 3986 "dilpar.y"
    {
            wtmp = &tmpl.core[(yyvsp[-10].num)];
            bwrite_ubit8(&wtmp,DILI_IF); /* the instruction */
            wtmp = &tmpl.core[(yyvsp[-6].num)];
            bwrite_ubit32(&wtmp,(yyvsp[0].ins).fst); /* address of else */
            wtmp = &tmpl.core[(yyvsp[-3].num)];
            bwrite_ubit8(&wtmp,DILI_GOTO); /* skip else */
            wtmp = &tmpl.core[(yyvsp[-2].num)];
            bwrite_ubit32(&wtmp,(yyvsp[0].ins).lst); /* end of else */
            (yyval.ins).fst = (yyvsp[-10].num);
            (yyval.ins).lst = (yyvsp[0].ins).lst;
         }
#line 7251 "y.tab.c"
    break;

  case 277:
#line 3999 "dilpar.y"
    {
            wtmp = &tmpl.core[(yyvsp[-5].num)];
            bwrite_ubit8(&wtmp,DILI_IF); /* the instruction */
            wtmp = &tmpl.core[(yyvsp[-1].num)];
            bwrite_ubit32(&wtmp,(yyvsp[0].ins).lst); /* address of else */
            (yyval.ins).fst = (yyvsp[-5].num);
            (yyval.ins).lst = (yyvsp[0].ins).lst;
         }
#line 7264 "y.tab.c"
    break;

  case 278:
#line 4009 "dilpar.y"
    { in_foreach++;
            if (in_foreach>1)
	      fatal("'foreach' may not be nested!");	      
	   }
#line 7273 "y.tab.c"
    break;

  case 279:
#line 4014 "dilpar.y"
    { in_foreach--; }
#line 7279 "y.tab.c"
    break;

  case 280:
#line 4016 "dilpar.y"
    {
            /*
             * allows both break and continue inside the composed
             * statement. continue starts at the next element
             * break starts with the instruction after loop.
             */

            if ((yyvsp[-16].ins).typ != DILV_INT)
	      fatal("Arg 1 of 'foreach' not an integer");
            wtmp = &tmpl.core[(yyvsp[-18].num)];
            bwrite_ubit8(&wtmp,DILI_FOE); /* foreach - clear / build */
            wtmp = &tmpl.core[(yyvsp[-11].num)];
            bwrite_ubit8(&wtmp,DILI_FON); /* foreach - next */
            wtmp = &tmpl.core[(yyvsp[-9].num)];
            bwrite_ubit32(&wtmp,wcore-tmpl.core);
            wtmp = &tmpl.core[(yyvsp[-4].num)];
            bwrite_ubit8(&wtmp,DILI_GOTO); /* loop */
            bwrite_ubit32(&wtmp,(yyvsp[-11].num));
            (yyval.ins).fst=(yyvsp[-18].num);
            (yyval.ins).lst=wcore-tmpl.core;
         }
#line 7305 "y.tab.c"
    break;

  case 281:
#line 4040 "dilpar.y"
    {
            /*
             * allows both break and continue inside the composed
             * statement. continue starts at the test statement.
             * break starts with the instruction after loop.
             */
         	 
            /* made with 'if' and 'goto' */
            wtmp = &tmpl.core[(yyvsp[-10].num)];
            bwrite_ubit8(&wtmp,DILI_IF); /* the instruction */
            wtmp = &tmpl.core[(yyvsp[-6].num)];
            bwrite_ubit32(&wtmp,wcore - tmpl.core); /* address of break */
            wtmp = &tmpl.core[(yyvsp[-4].num)];
            bwrite_ubit8(&wtmp,DILI_GOTO); /* test again */
            wtmp = &tmpl.core[(yyvsp[-3].num)];
            bwrite_ubit32(&wtmp,(yyvsp[-10].num)); /* address of start */
            (yyval.ins).fst = (yyvsp[-10].num);
            (yyval.ins).lst = wcore-tmpl.core;
         }
#line 7329 "y.tab.c"
    break;

  case 282:
#line 4060 "dilpar.y"
    {
	    int i;

            /* register label */
            if ((i = search_block((yyvsp[-2].sym), (const char **) label_names, TRUE)) != -1)
	    {
	       if (label_adr[i] != SKIP)
		 fatal("Redefinition of label");
	       else
	       {
		  /* update label */
		  label_adr[i]= (yyvsp[0].ins).fst;
		  /* fprintf(stderr,"LABEL %s had adress %d\n",
			  label_names[i], $4.fst); */
	       }
            } else
	      add_label((yyvsp[-2].sym), (yyvsp[0].ins).fst);
            (yyval.ins) = (yyvsp[0].ins);
         }
#line 7353 "y.tab.c"
    break;

  case 286:
#line 4088 "dilpar.y"
    {
            /* collect argument types */
            ref.argt[ref.argc] = (yyvsp[0].ins).typ;
            if (++ref.argc > ARGMAX)
               fatal("Too many arguments");
         }
#line 7364 "y.tab.c"
    break;

  case 287:
#line 4097 "dilpar.y"
    {
	    if ((yyvsp[0].ins).dsl==DSL_FCT)
	    {
	       /* this is an assignment from a normal function */
	       int i, varnum, refnum;
	       char buf[255],nbuf[255],zbuf[255];

	       /*
		* For function calls:
		* <rfcall> <var> <var#> <funcnumber> <nargs> <arg> <arg> ...
		*/
             
	       /* fprintf(stderr,"Function call\n"); */

	       /* check if corevar is true variable */
	       if (tmpl.core[(yyvsp[-2].ins).fst]!=DILE_VAR)
		 fatal("Variable must be true local variable");
              
	       /* read variable number from core */
	       wtmp = &tmpl.core[1+((yyvsp[-2].ins).fst)];
	       varnum=bread_ubit16(&wtmp);

	       wtmp = &tmpl.core[(yyvsp[0].ins).fst];
	       refnum=bread_ubit16(&wtmp);

	       /* check func/var types */
	       switch (tmpl.vart[varnum])
	       {
		 case DILV_SP:
		 case DILV_EDP:
		 case DILV_SLP:
		 case DILV_UP:
		  //if ((tmpl.vart[varnum] != tmpl.xrefs[refnum].rtnt) &&
		  //	(tmpl.vart[varnum] != DILV_NULL))
		  if (tmpl.vart[varnum] != tmpl.xrefs[refnum].rtnt)
		    fatal("Assigning incompatible types (A)");
		  break;
		 case DILV_INT:
                  if (tmpl.vart[varnum] != tmpl.xrefs[refnum].rtnt)
		    fatal("Assigning incompatible types (B)");
		  break;
		 default:
                  fatal("Type error in function assign");
		  break;
	       }
	       wtmp = &tmpl.core[(yyvsp[-3].num)];
	       bwrite_ubit8(&wtmp,DILI_RFC);
	       (yyval.ins).fst = (yyvsp[-3].num);
	       (yyval.ins).lst = (yyvsp[0].ins).lst;
	       
	    } else if ((yyvsp[0].ins).dsl==DSL_SFT) {

	       /* this is an assignment from a symbolic function */
	       int i, varnum;
	       char buf[255],nbuf[255],zbuf[255];

	       /*
		* For symbolic function calls:
		* <rsfcall> <var> <var#> <funcnumber> <nargs> <arg> <arg> ...
		*/

	       /* fprintf(stderr,"Symbolic function call\n"); */

	       /* check if corevar is true variable */
	       if (tmpl.core[(yyvsp[-2].ins).fst]!=DILE_VAR)
		 fatal("Variable must be true local variable");
              
	       /* check func/var types impossible! */

	       wtmp = &tmpl.core[(yyvsp[-3].num)];
	       bwrite_ubit8(&wtmp,DILI_SFC);
	       (yyval.ins).fst = (yyvsp[-3].num);
	       (yyval.ins).lst = (yyvsp[0].ins).lst;	       
	       
	    } else {
	       /* this is an assignment to an expression */	 

	       /* fprintf(stderr,"Assignment\n"); */
	       switch ((yyvsp[-2].ins).typ) {
		 case DILV_SP:
		 case DILV_EDP:
		 case DILV_SLP:
		 case DILV_UP:
                  if (((yyvsp[-2].ins).typ != (yyvsp[0].ins).typ) && ((yyvsp[0].ins).typ != DILV_NULL))
		    fatal("Assigning incompatible types (C)");
                  else {
                     wtmp = &tmpl.core[(yyvsp[-3].num)];
                     bwrite_ubit8(&wtmp,DILI_ASS);
                     (yyval.ins).fst = (yyvsp[-3].num);
                     (yyval.ins).lst = (yyvsp[0].ins).lst;
                  }
		  break;
		 case DILV_INT:
                  if ((yyvsp[-2].ins).typ != (yyvsp[0].ins).typ)
		    fatal("Assigning incompatible types (D)");
                  else {
                     wtmp = &tmpl.core[(yyvsp[-3].num)];
                     bwrite_ubit8(&wtmp,DILI_ASS);
                     (yyval.ins).fst = (yyvsp[-3].num);
                     (yyval.ins).lst = (yyvsp[0].ins).lst;
                  }
		  break;
		 default:
                  fatal("Type error in assign");
		  break;
	       }
	    }
	 }
#line 7477 "y.tab.c"
    break;

  case 288:
#line 4208 "dilpar.y"
    {
	    int i, refnum;
	    char buf[256];

	    if ((yyvsp[-6].ins).dsl != DSL_FCT)
	    {
	       if ((yyvsp[-6].ins).typ == DILV_SP)
	       {
		  /* this is a symbolic proc/func call */
		  (yyval.ins).dsl=DSL_SFT;

		  (yyval.ins).fst = (yyvsp[-6].ins).fst;
		  (yyval.ins).typ = (yyvsp[-6].ins).typ;
		  wtmp=&tmpl.core[(yyvsp[-5].num)];
		  bwrite_ubit16(&wtmp,ref.argc);
		  (yyval.ins).lst = wcore-tmpl.core;
	       }
	       else
		 fatal("Report to Papi.");
	    }
	    else
	    {
	       /* this is a normal proc/func call */
	       (yyval.ins).dsl=DSL_FCT;

	       if ((yyvsp[-6].ins).dsl!=DSL_FCT)
		 fatal("reference not a procedure or function");

	       /* fprintf(stderr, "Func : fst %p, lst %p.\n",
		       $1.fst, $1.lst); */

	       /* read refnum from core */
	       wtmp = &tmpl.core[(yyvsp[-6].ins).fst];

               refnum = bread_ubit16(&wtmp);
	       /*fprintf(stderr, "refnum read %d, %p.\n",
		       refnum, &wcore[$1.fst]);*/


	       if (tmpl.xrefs[refnum].argc != ref.argc)
	       {
		  char buf[256];
		  sprintf(buf, "Illegal number of arguments %d, %d expected."
			  " Refnum %d.",
			  ref.argc, tmpl.xrefs[refnum].argc, refnum);
		  fatal(buf);
	       }

	       /* check argument types/count */
	       for (i=0;i<tmpl.xrefs[refnum].argc;i++)
	       {
		  /* argument types collected in ref */
		  switch (tmpl.xrefs[refnum].argt[i])
		  {
		    case DILV_SP:
		    case DILV_EDP:
		    case DILV_SLP:
		    case DILV_UP:
                     if ((tmpl.xrefs[refnum].argt[i] != ref.argt[i]) &&
                         (ref.argt[i] != DILV_NULL)) {
                        sprintf(buf, "Incompatible argument "
                                "types in argument %d (A)", i+1);
                        fatal(buf);
                     }
		     break;
		    case DILV_INT:
                     if (tmpl.xrefs[refnum].argt[i] != ref.argt[i]) {
                        sprintf(buf, "Incompatible argument "
                                "types in argument %d (B)", i+1);
                        fatal(buf);
		     }
		     break;
		    default:
                     fatal("Type error in argument");
		     break;
		  }
	       }

	       (yyval.ins).fst = (yyvsp[-6].ins).fst;
	       (yyval.ins).typ = (yyvsp[-6].ins).typ;
	       wtmp=&tmpl.core[(yyvsp[-5].num)];
	       bwrite_ubit16(&wtmp,ref.argc);
	       (yyval.ins).lst = wcore-tmpl.core;
	    }
	 }
#line 7567 "y.tab.c"
    break;

  case 289:
#line 4296 "dilpar.y"
    {
	    checkbool("argument 1 in 'interrupt'",(yyvsp[-5].ins).boolean);
            if ((yyvsp[-5].ins).typ != DILV_INT)
               fatal("Arg 1 of 'interrupt' not number");
            else {
	       (yyval.ins).fst=(yyvsp[-7].num);
	       (yyval.ins).lst=(yyvsp[-1].ins).lst;
               /* Type is ok */
	       wtmp=&tmpl.core[(yyvsp[-7].num)];
               bwrite_ubit8(&wtmp,DILE_INTR);
	       tmpl.intrcount++;
	       frm.intrcount++;
            }
	    (yyval.ins).dsl=DSL_DYN;
	    (yyval.ins).typ=DILV_INT;
	 }
#line 7588 "y.tab.c"
    break;

  case 290:
#line 4313 "dilpar.y"
    {
	    ubit32 flags;

            (yyval.ins).fst = (yyvsp[-7].num);
            (yyval.ins).lst = (yyvsp[-1].ins).lst;
            /* write an interrupt instead! */
            wtmp = &tmpl.core[(yyvsp[-7].num)];
            bwrite_ubit8(&wtmp,DILE_INTR);
	    bwrite_ubit8(&wtmp,DILE_INT);
	    flags = SFB_ACTIVATE;
            bwrite_ubit32(&wtmp,flags);
	    tmpl.intrcount++;
	    frm.intrcount++;
	    (yyval.ins).dsl=DSL_DYN;
	    (yyval.ins).typ=DILV_INT;
         }
#line 7609 "y.tab.c"
    break;

  case 291:
#line 4332 "dilpar.y"
    {
	    (yyval.ins)=(yyvsp[0].ins);
	 }
#line 7617 "y.tab.c"
    break;

  case 292:
#line 4336 "dilpar.y"
    {
	    (yyval.ins)=(yyvsp[0].ins);
         }
#line 7625 "y.tab.c"
    break;

  case 294:
#line 4343 "dilpar.y"
    { (yyval.ins) = (yyvsp[0].ins); }
#line 7631 "y.tab.c"
    break;

  case 295:
#line 4345 "dilpar.y"
    {  (yyval.ins) = (yyvsp[0].ins); }
#line 7637 "y.tab.c"
    break;

  case 296:
#line 4347 "dilpar.y"
    { (yyval.ins) = (yyvsp[0].ins); }
#line 7643 "y.tab.c"
    break;

  case 297:
#line 4349 "dilpar.y"
    {
            if (!istemplate) {
              fatal("return only allowed in templates");
            }
            if (tmpl.rtnt != DILV_NULL) {
              fatal("no return expression expected");
	    }            	

            /* WAS: bwrite_ubit8(&wcore,DILI_RTS); */ /* the instruction */

            wtmp = &tmpl.core[(yyvsp[0].num)];
            bwrite_ubit8(&wtmp,DILI_RTS); /* the instruction */

	    (yyval.ins).fst = (yyvsp[0].num);
	    (yyval.ins).lst = (yyvsp[0].num)+1;
         }
#line 7664 "y.tab.c"
    break;

  case 298:
#line 4366 "dilpar.y"
    {
            if (!istemplate) {
              fatal("return only allowed in templates");
            }
            if (tmpl.rtnt != (yyvsp[-1].ins).typ) {
              fatal("return expression not of correct type");
	    }            	
            wtmp = &tmpl.core[(yyvsp[-3].num)];
            bwrite_ubit8(&wtmp,DILI_RTF); /* the instruction */
	    (yyval.ins).fst = (yyvsp[-3].num); 
	    (yyval.ins).lst = (yyvsp[-1].ins).lst;
         }
#line 7681 "y.tab.c"
    break;

  case 299:
#line 4379 "dilpar.y"
    {
	    if ((yyvsp[-4].ins).typ != DILV_INT)
	      fatal("Expression after 'on' does not return integer");
	    else {
	       (yyval.ins).fst=(yyvsp[-5].num);
	       (yyval.ins).lst=(yyvsp[0].ins).lst;
	       wtmp=&tmpl.core[(yyvsp[-5].num)];
	       bwrite_ubit8(&wtmp, DILI_ON);
	       wtmp=&tmpl.core[(yyvsp[-3].num)];
	       bwrite_ubit16(&wtmp, (yyvsp[0].ins).typ);
	    }
         }
#line 7698 "y.tab.c"
    break;

  case 300:
#line 4392 "dilpar.y"
    {
            wtmp = &tmpl.core[(yyvsp[-1].num)];
            bwrite_ubit8(&wtmp,DILI_GOTO);
            (yyval.ins).fst = (yyvsp[-1].num);
            (yyval.ins).lst = (yyvsp[0].ins).lst;
         }
#line 7709 "y.tab.c"
    break;

  case 301:
#line 4399 "dilpar.y"
    {
            /* break statement */
            if (!break_no) {
               /* not in loop */
               fatal("Using break outside loop");
            } else {
               wtmp = &tmpl.core[(yyvsp[-1].num)];
               bwrite_ubit8(&wtmp,DILI_GOTO);
               wtmp = &tmpl.core[(yyvsp[0].num)];
               /* register use or find break label */
               bwrite_ubit32(&wtmp,
                  get_label(label_names[break_idx[break_no-1]],(yyvsp[0].num))); 
            }
            (yyval.ins).fst = (yyvsp[-1].num);
            (yyval.ins).lst = (yyvsp[0].num)+4;
         }
#line 7730 "y.tab.c"
    break;

  case 302:
#line 4416 "dilpar.y"
    {
            /* continue statement */
            if (!cont_no) {
               /* not in loop */
               fatal("Using continue outside loop");
            } else {
               wtmp = &tmpl.core[(yyvsp[-1].num)];
               bwrite_ubit8(&wtmp,DILI_GOTO);
               wtmp = &tmpl.core[(yyvsp[0].num)];
               /* register use or find continue label */
               bwrite_ubit32(&wtmp,
                  get_label(label_names[cont_idx[break_no-1]],(yyvsp[0].num))); 
            }
            (yyval.ins).fst = (yyvsp[-1].num);
            (yyval.ins).lst = (yyvsp[0].num)+4;
         }
#line 7751 "y.tab.c"
    break;


#line 7755 "y.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 4433 "dilpar.y"


void add_var(char *name, ubit16 type)
{            
   str_lower(name);

   if (search_block(name, (const char **) var_names, TRUE) != -1)
     fatal("Duplicate variable identifier");

   var_names = add_name(name, var_names);
   tmpl.vart[tmpl.varc]=type;
   frm.vars[tmpl.varc].val.integer=0;
   frm.vars[tmpl.varc].type=type;
   
   if (++tmpl.varc > VARMAX)
     fatal("Too many variables");
}

int add_label(char *name, ubit32 adr)
{
   str_lower(name);

   /* add new label here */
   label_names = add_name(name, label_names);

   if (label_no == 0)
      CREATE(label_adr, ubit32, 1);
   else
      RECREATE(label_adr, ubit32, label_no+1);

   label_adr[label_no] = adr; /* index adress */

   /*fprintf(stderr, "ADD LABEL %s (%d)\n", name, adr);*/

   return (label_no++);
}

ubit32 get_label(char *name, ubit32 adr)
{
   /* get number of referenced label */
   int i;

   if ((i = search_block(name, (const char **) label_names, TRUE)) == -1) {
      /* does not exist, register label */
     /*fprintf(stderr,"GET LABEL UNKNOWN: %s\n", name);*/
      i = add_label(name, SKIP);
   }

   if (label_adr[i] == SKIP) {
      /* register use of label before definition */
      if (label_use_no == 0) {
         CREATE(label_use_adr, ubit32, 1);
         CREATE(label_use_idx, ubit32, 1);
      } else {
         RECREATE(label_use_adr, ubit32, label_use_no+1);
         RECREATE(label_use_idx, ubit32, label_use_no+1);
      }
      /*fprintf(stderr,"GET LABEL REGISTERED LOCATION: %s %d\n", name, adr);*/
      label_use_adr[label_use_no] = adr; /* update here */
      label_use_idx[label_use_no] = i;
      label_use_no++;
      return SKIP; /* address is updated later */
   } else {
      /* just return address */
     /*fprintf(stderr,
       "GET LABEL WROTE LOCATION: %s %d\n", name, label_adr[i]);*/
      return label_adr[i];
   }
}

void update_labels(void)
{
   int i;
   char buf[255];
   ubit8 *wtmp;
   
   for(i = 0; i < label_use_no; i++)
   {
      /* Make sure ALL functions with labels have
       * the NOLAB number replaced with the label
       * address.
       */

       if (label_use_adr[i]!=SKIP)
       {
	  /*fprintf(stderr,"UPDATE LABEL %s: location %d to adress %d\n",
		  label_names[label_use_idx[i]],
		  label_use_adr[i], label_adr[label_use_idx[i]]);*/
          wtmp=&tmpl.core[label_use_adr[i]];

          if (label_adr[label_use_idx[i]] == SKIP)
	  {
             sprintf(buf,"Undefined label: %s",label_names[label_use_idx[i]]);
             fatal(buf);
          } 
          bwrite_ubit32(&wtmp, label_adr[label_use_idx[i]]);
	  if (label_adr[label_use_idx[i]] > (wcore - tmpl.core))
	  {
	     sprintf(buf, "Internal compiler error when resolving label %s.",
		   label_names[label_use_idx[i]]);
	     fatal(buf);
	  }
      }
   }
}

void syntax(const char *str)
{
   fprintf(stderr, "\n%d: %s\n    Token: '%s'\n", linenum, str, yytext);
   errcon = 1;
}

void fatal(const char *str)
{
   fprintf(stderr, "\n%d: %s\n    Token: '%s'\n", linenum, str, yytext);
   if (*tmpfname)
     remove(tmpfname);
   exit(1);
}


void warning(const char *str)
{
   fprintf(stderr, "\n%d: %s\n    Token: '%s'\n", linenum, str, yytext);
}


/* Read a string of arbitrary length, allocate space for and return it */
char *str_dup_file(FILE *fl)
{
   char tmp[MAX_STRING_LENGTH], *res;

   CREATE(res, char, 1);

   while (fgets(tmp, MAX_STRING_LENGTH, fl))
   {
      RECREATE(res, char, strlen(tmp) + strlen(res) + 1);
      strcat(res, tmp);
   }
   return res;
}

int strlstlen(char **strlst)
{
   char **slp=strlst;
   int len = 0;
   while (*slp) 
      len+=strlen(*(slp++))+1;
   len++;
   return len;
}

/* code manipulation */

/* increase core size of tmpl.core if need be, and updates wcore */
void moredilcore(ubit32 size)
{
   ubit32 p1,p2,pos;
   p1 = tmpl.coresz;
   p2 = (wcore - tmpl.core) + size;
   
   if ( p1 < p2 ) {
      pos = wcore - tmpl.core;
      RECREATE(tmpl.core, ubit8, tmpl.coresz+CODESIZE);
      tmpl.coresz+=CODESIZE;
      wcore = &tmpl.core[pos];
    }
}



/* expression manipulation */

void add_ubit8(struct exptype *dest, ubit8 d)
{
/*   fprintf(stderr, "UBIT8\n");*/
   if (dest->codep - dest->code + sizeof(ubit8) >= CODESIZE) 
      fatal("U8: Expression too large");
   bwrite_ubit8(&(dest->codep), d);
}

void add_ubit32(struct exptype *dest, ubit32 d)
{
/*   fprintf(stderr, "UBIT32\n");*/
   if (dest->codep - dest->code + sizeof(ubit32) >= CODESIZE) 
      fatal("U32: Expression too large");
   bwrite_ubit32(&(dest->codep), d);
}

void add_sbit32(struct exptype *dest, sbit32 d)
{
/*   fprintf(stderr, "SBIT32\n");*/
   if (dest->codep - dest->code + sizeof(sbit32) >= CODESIZE) 
      fatal("S32: Expression too large");
   bwrite_ubit32(&(dest->codep), (ubit32) d);
}

void add_ubit16(struct exptype *dest, ubit16 d)
{
/*   fprintf(stderr, "UBIT16\n");*/
   if (dest->codep - dest->code + sizeof(ubit16) >= CODESIZE) 
      fatal("U16: Expression too large");
   bwrite_ubit16(&(dest->codep), d);
}

void add_string(struct exptype *dest, char *d)
{
/*   fprintf(stderr, "STRING %d , %d\n",
         dest->codep - dest->code,
         strlen(d));
*/
   if (dest->codep - dest->code + strlen(d) + 1 >= CODESIZE) 
      fatal("AS: Expression too large");
   bwrite_string(&(dest->codep), d);
}

void cat_string(struct exptype *dest, char *d)
{
/*   fprintf(stderr, "STRING %d , %d\n",
         dest->codep - dest->code,
         strlen(d));
*/
   dest->codep--;  /* Overwrite trailing \0 char */
   if (dest->codep - dest->code + strlen(d) + 1 >= CODESIZE) 
      fatal("CS: Expression too large");
   bwrite_string(&(dest->codep), d);
}

void add_stringlist(struct exptype *dest, char **d)
{
/*   fprintf(stderr, "STRINGLIST\n");*/
   if (dest->codep - dest->code + strlstlen(d) + 1 >= CODESIZE) 
      fatal("ASL: Expression too large.");
   bwrite_nameblock(&(dest->codep), d);
}

void add_code(struct exptype *dest, struct exptype *src)
{
   sbit32 len = src->codep - src->code;

/*   fprintf(stderr, "ADD CODE\n");*/

   /* adds code in src to dest */
   if ((dest->codep - dest->code + len) >= CODESIZE)
      fatal("ACO: expression too large");
   memcpy(dest->codep, src->code, len);
   dest->codep += len;
}

void copy_code(struct exptype *dest, struct exptype *src)
{
   sbit32 len = src->codep - src->code;

/*   fprintf(stderr, "COPY CODE %d\n", len);*/

   /* copy code in src to dest */
   dest->codep = dest->code;
   memcpy(dest->code, src->code,len);
   dest->codep = &(dest->code[len]);
   dest->typ = src->typ;
   dest->dsl = src->dsl;
   dest->num = src->num;
}



ubit16 UpdateCRC(register ubit8 c, register ubit16 crc)
{
/*

  The UpdateCRC is as follows in 8088 assembler:

  AL = Char to update with (8  bit)
  BX = CRC value to update (16 bit) (BH is MSB, BL is LSB of CRC).

  At the start of a packet, CRC should be set to 0.
  Always at the end of a packet before transmitting the CRC, do:

     CRC = UpdateCRC(0, UpdateCRC(0, CRC))

  Then transmit the CRC in MSB, LSB order.

  When receiving: The two CRC value characters should be calculated
  as part of the package. Then CRC will be zero if no error occurred.

         MOV CX, 8      ; The generator is X^16+X^12+X^5+1
LOOP1:                  ; as recommended by CCITT. An alternate
         ROL AL, 1      ; generator often used in synchronous
         RCL BX, 1      ; communication is X^16+X^15+X^2+1
         JNB SKIP       ; This may be used by substituting XOR 8005H
         XOR BX, 8005H  ; for XOR 1021H in the adjacent code.
SKIP:
         LOOP LOOP1

*/

   register int i;
   register int rcl_cf;
    
   for (i=0; i < 8; i++)
   {
      c = (c << 1) | ((c & 128) >> 7);
      rcl_cf = crc & 0x8000;
      crc = (crc << 1) | (c & 1);
      if (rcl_cf)     
	crc ^= 0x8005;
   }

   return crc;
}
      


int dil_headercrc(char **name, ubit8 *type)
{
   int i, j;
   ubit16 crc = 0;

   for (i=0; name[i]; i++)
   {
      crc = UpdateCRC(type[i], crc);

      for (j=0; name[i][j]; j++)
	crc = UpdateCRC(name[i][j], crc);
   }

   return crc;
}


int dil_corecrc(ubit8 *core, int len)
{
   int i, j;
   ubit16 crc = 0;

   for (i=0; i < len; i++)
     crc = UpdateCRC(core[i], crc);

   return crc;
}


void make_code(struct exptype *dest)
{
/*   fprintf(stderr, "MAKE CODE\n");*/

   if (dest->dsl == DSL_STA) {
      switch (dest->typ) {
         case DILV_INT: /* static integer */
             dest->codep = dest->code;
             dest->dsl = DSL_DYN;
             bwrite_ubit8(&(dest->codep), DILE_INT);
             bwrite_ubit32(&(dest->codep),(ubit32) dest->num);
         break;
          case DILV_NULL: /* null pointer */
             dest->codep = dest->code;
             dest->dsl = DSL_DYN;
             bwrite_ubit8(&(dest->codep),DILE_NULL);
         break;
         default: /* static other */
             dest->dsl = DSL_DYN; /* then its allready there */
         break;
      }
   }
}


void add_ref(struct dilref *ref)
{
   int i;
   char nbuf[255];


   /* register reference */
   refs[refcount] = *ref;


   if (ref->argc)
   {
      CREATE(refs[refcount].argv, char *, ref->argc);
      memcpy(refs[refcount].argv, ref->argv, ref->argc*sizeof(char *));
      CREATE(refs[refcount].argt, ubit8, ref->argc);
      memcpy(refs[refcount].argt, ref->argt, ref->argc*sizeof(ubit8));
   }
   else
   {
      refs[refcount].argv=NULL;
      refs[refcount].argt=NULL;
   }

   sprintf(nbuf,"%s@%s", ref->name, ref->zname);

   if (is_name(nbuf, (const char **) ref_names))
     fatal("Redefinition of external reference.");

   ref_names = add_name(nbuf, ref_names);
   
   if (++refcount > REFMAX)
     fatal("Too many references");
}


void dumpdiltemplate(struct diltemplate *tmpl) {

  int i;
  fprintf(stderr,"Name: '%s'\n",tmpl->prgname);
  fprintf(stderr,"Crc: %d\n",tmpl->varcrc);
  fprintf(stderr,"Return type: %d\n",tmpl->rtnt);
  fprintf(stderr,"# Arguments: %d\n",tmpl->argc);
  for (i=0;i<tmpl->argc;i++)
    fprintf(stderr,"Arg %d type: %d\n",i,tmpl->argt[i]);
  fprintf(stderr,"Coresize: %d\n",tmpl->coresz);
  fprintf(stderr,"Core: %p\n",tmpl->core);
  fprintf(stderr,"# Vars: %d\n",tmpl->varc);
  for (i=0;i<tmpl->varc;i++) 
    fprintf(stderr,"Var %d type: %d\n",i,tmpl->vart[i]);
  fprintf(stderr,"# References: %d\n",tmpl->xrefcount);
  for (i=0;i<tmpl->xrefcount;i++) {
     fprintf(stderr,"Xref %d: '%s'\n",i,tmpl->xrefs[i].name);
     fprintf(stderr,"     Return type %d\n",tmpl->xrefs[i].rtnt);
     fprintf(stderr,"     # Arguments: %d\n",tmpl->xrefs[i].argc);
  }
}

void dumpdil(struct dilprg *prg) {
  int i;

  fprintf(stderr,"Flags: %d\n",prg->flags);
  fprintf(stderr,"Varcrc: %d\n",prg->varcrc);
  fprintf(stderr,"Stacksz: %d\n",prg->stacksz);
  fprintf(stderr,"Stackptr: %d\n",(int) (1+(prg->sp - prg->stack)));
  for (i=0;i<=(prg->sp - prg->stack);i++) {
    fprintf(stderr,"Stackframe %d:\n",i);
    fprintf(stderr,"Return var# %d:\n",prg->stack[i].ret);
    dumpdiltemplate(prg->stack[i].tmpl);
  }
}

int main(int argc, char **argv)
{
   int result, argn;

   extern int linenum;

   int yyparse(void);

#ifndef DOS
   alarm(60*15);  /* If not done in five minutes, abort! */
#endif

   if ((argc < 2) || (argc > 6))
   {
      fprintf(stderr, "Usage: %s [lineno] [-v] [-t] [-z zonename] (%d)\n",
	      argv[0],argc);
      exit(1);
   }

   for (argn=1;argn<argc;argn++) {
      if (!strcmp(argv[argn],"-t"))
         istemplate = TRUE;
      else if (!strcmp(argv[argn],"-v"))
	fprintf(stderr,
		"DIL " DIL_VERSION " Copyright (C) 1995 - 1996 by "
		"Valhalla [" __DATE__ "]\n");
      else if (!strcmp(argv[argn],"-z")) {
         argn++;
         cur_zonename=argv[argn];
      }
      else
	linenum = atoi(argv[argn]);
   }

   result = yyparse();

   return(0);
}
