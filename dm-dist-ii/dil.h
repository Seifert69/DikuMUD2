/* *********************************************************************** *
 * File   : dil.h                                     Part of Valhalla MUD *
 * Version: 2.00                                                           *
 * Author : bombman@diku.dk                                                *
 *                                                                         *
 * Purpose: Prototypes, types and defines for the rest of dil              *
 *                                                                         *
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

/* Wed Jan 22 14:57:30 PST 1997   HHS added paycheck dilfun DILE_PCK */
	
#ifndef _MUD_DIL_H
#define _MUD_DIL_H
	
#ifndef L_tmpnam
	  #define   L_tmpnam       1024 /* Oh thank you Marcel! */
#endif
	
#define WAITCMD_MAXINST	   2000

#define SKIP     0xffffffff /* skip label/index defined */


/* The following "dilarg" structs are used only for DMC purposes to allow
   the user to enter the : dilcopy prg@zon(arg, arg, arg); instead of the
   stupid string format */

struct dilargtype
{
   ubit8 type;
   union
   {
     char *string;
     char **stringlist;
     int64_t num; //MS2020
   } data;
};

struct dilargstype
{
   ubit8 no;
   char *name;
   struct dilargtype dilarg[256];
};


/* 
 *  The expressions and instructions are as follows:
 *  All expressions are prefixed, and their type defined by the leading 
 *  DIL?_* value. Functions and instructions are followed by
 *  the argument expressions. Code pointers are 32-bit pointers point to
 *  other positions in the DIL intruction memory.
 */   

#define DILFL_COPY        0x01  /* Program is a --copy--                 */
#define DILFL_RECALL      0x02  /* Recall PC?                            */
#define DILFL_EXECUTING   0x04  /* Program is already executing?         */
#define DILFL_CMDBLOCK    0x08  /* Block the currently executed command? */
#define DILFL_AWARE       0x10  /* To determine when to use SFB_AWARE    */
#define DILFL_FREEME      0x20  /* If "copy" is just a faked copy....    */
#define DILFL_DEACTIVATED 0x40  /* True when dil may not be executed     */
	
/* DIL expressions */
#define DIL_ILL		0	/* Illegal (for debugging purpose) */
#define DILE_PLUS	1	/* # + # */
#define DILE_MIN	2	/* # - # */
#define DILE_MUL	3	/* # * # */
#define DILE_DIV	4	/* # / # */
#define DILE_MOD	5	/* # % # */
#define DILE_AND	6	/* # and # */
#define DILE_OR		7	/* # or # */
#define DILE_NOT	8	/* not # */
#define DILE_GT		9	/* # > # */
#define DILE_LT		10	/* # < # */
#define DILE_GE		11	/* # >= # */
#define DILE_LE		12	/* # <= # */
#define DILE_EQ		13	/* # == # */
#define DILE_PE		14	/* # #= # */
#define DILE_NE		15	/* # != # */
#define DILE_IN		16	/* # in # */
#define DILE_UMIN	17	/* - # */
#define DILE_SE		18	/* # ?= # */
#define DILE_NULL	19	/* null */
	
/* DIL functions */
#define DILE_FLD	20	/* get field + ubit8 DILF_? */
#define DILE_ATOI	21	/* atoi(#) */
#define DILE_ITOA	22	/* itoa(#) */
#define DILE_RND	23	/* rnd(#,#) */
#define DILE_FNDU	24	/* findunit(#,#,#) */
#define DILE_FNDR	25	/* findroom(#,#) */
#define DILE_LOAD	26	/* load(#) */
#define DILE_ISS	27	/* isset(#,#) */
#define DILE_GETW	28	/* getword(#) */
#define DILE_ISA	29	/* isaff(#,#) */
#define DILE_CMDS	30	/* command(#) */
#define DILE_FNDS	31	/* findsymbolic(#) */
	
/* DIL internal variables */
#define DILE_ACTI	32	/* activator */
#define DILE_ARGM	33	/* argument */
#define DILE_THO	34	/* mudhour */
#define DILE_TDA	35	/* mudday */
#define DILE_TMD	36	/* mudmonth */
#define DILE_TYE	37	/* mudyear */
#define DILE_HRT	38	/* heartbeat */
#define DILE_SELF	39	/* self */
	
/* DIL static references */
#define DILE_VAR	40	/* variable (ubit16) */
#define DILE_FS	        41	/* fixed string (char[]) */
#define DILE_FSL	42	/* fixed stringlist (char[][]) */
#define DILE_INT	43	/* fixed integer (sbit32) */
#define DILE_LEN	44	/* length(#) */
	
/* DIL instructions */
#define DILI_ASS	45	/* # = # */
#define DILI_LNK	46	/* link(#,#) */
#define DILI_EXP	47	/* experience(#) */
#define DILI_CAST	48	/* cast_spell */
#define DILI_IF         49	/* if */
#define DILI_SET	50	/* set (#,#) */
#define DILI_USET	51	/* unset (#,#) */
#define DILI_ADL	52	/* addlist (#,#) */
#define DILI_SUL	53	/* sublist (#,#) */
#define DILI_ADE	54	/* addextra (#,#,#) */
#define DILI_SUE	55	/* subextra (#,#) */
#define DILI_DST	56	/* destroy (#) */
#define DILI_WALK	57	/* walkto (#) */
#define DILI_EXEC	58	/* exec (#,#) */
#define DILI_WIT	59	/* wait (#,#) */
#define DILI_ACT	60	/* act (#,#,#,#,#,#) */
#define DILI_GOTO	61	/* goto label */
#define DILI_SUA	62	/* subaff (#,#) */
#define DILI_ADA	63	/* addaff (#,#,#) */
#define DILI_PRI	64	/* priority */
#define DILI_NPR	65	/* nopriority */
#define DILI_SND	66	/* send (#) */
#define DILI_SNT	67	/* sendto (#,#) */
#define DILI_SEC	68	/* secure (#,@) */
#define DILI_USE	69	/* unsecure (#) */
#define DILI_FOE	70	/* foreach - clear / build list */
#define DILI_FON	71	/* foreach - get next in environment */
#define DILI_EQP	72	/* addequip (#,#) */
#define DILI_UEQ	73	/* unequip (#) */
#define DILE_WEAT	74	/* weather */
#define DILE_OPPO	75	/* opponent(#,#) */
#define DILI_QUIT	76	/* quit */
#define DILI_BLK	77	/* block */
#define DILI_PUP	78	/* position_update */

/* Extensions */
#define DILE_GETWS	79	/* getwords(#) */
#define DILI_SNTA	80	/* sendtoall */
#define DILI_LOG	81	/* log */
#define DILE_PNEQ	82	/* not # #= # */
#define DILE_SNEQ	83	/* not # $= # */
#define DILI_RPC        84      /* remote procedure call */
#define DILI_RFC        85      /* remote function call */
#define DILI_RTS        86      /* return from subroutine */
#define DILI_RTF        87      /* return from function */
#define DILE_DLD        88      /* dil destroy (#,#) */
#define DILE_DLF        89      /* dil find (#,#) */
#define DILI_DLC        90      /* dil copy (#,#) */
#define DILE_LOR        91      /* logical or */
#define DILE_LAND       92      /* logical and */
#define DILI_ON 	93	/* on # @ @ @ @ */
#define DILI_SPC	94	/* symbolic procedure call */
#define DILI_SFC	95	/* symbolic function call */
#define DILE_INTR	96	/* interrupt */
#define DILI_CLI        97      /* clear interrupt */
#define DILI_SBT	98	/* setbright (#,#) */
#define DILI_SWT	99	/* setweight (#,#) */
#define DILE_FNDRU     100	/* findunit(#,#,#) */
#define DILE_VISI      101	/* visible(#,#)   */
#define DILE_ATSP      102	/* attack_spell(#,#,#,#,#) */
#define DILE_PURS      103	/* purse(#,#) */
#define DILI_CHAS      104	/* change_speed(#,#) */
#define DILI_SETF      105	/* set_fighting(#,#) */
#define DILE_MEDI      106	/* medium */
#define DILE_TARG      107	/* target */
#define DILE_POWE      108	/* power */
#define DILE_TRMO      109	/* transfermoney(#,#,#) */
#define DILI_SNTADIL   110	/* sendtoalldil(#,#,#) */
#define DILE_CAST2     111	/* int cast_spell(#,#,#,#,#) */
#define DILE_MEL       112      /* int meleeattack(#,#,#) */
#define DILE_EQPM      113      /* unitptr equipment(#,#) */
#define DILE_CMST      114	/* cmdstr */
#define DILE_OPRO      115	/* openroll(#,#) */

#define DILE_REST      116	/* restore(#,#) */
#define DILI_STOR      117	/* store(#)     */
#define DILI_AMOD      118	/* acc_modify(#,#)  */
#define DILI_SETE      119	/* sendtext(#,#)  */

#define DILI_FOLO      120	/* follow(#,#)       */
#define DILI_LCRI      121	/* logcrime(#,#,#)   */
#define DILE_FIT       122	/* fits(#,#,#)       */
#define DILE_CARY      123	/* can_carry(#,#)    */
#define DILE_FNDS2     124	/* findsymbolic(#,#) */
#define DILE_PATH      125	/* pathto(#,#)       */
#define DILE_MONS      126	/* moneystring(#,#)  */
#define DILE_SPLX      127	/* spellindex(#)     */
#define DILE_SPLI      128	/* spellinfo(#,#)    */

#define DILE_RTI       129	/* realtime()        */
#define DILE_TXF       130      /* textformat(#)     */
#define DILE_AST       131      /* asctime(#)        */
#define DILE_PCK       132	/* paycheck(#,#)   */
#define DILE_ACT       133      /* act(...) expression */

#define DILI_MAX       133	/* The maximum node number */
	
/* DIL Field references */
#define DILF_NMS	0	/* .names */
#define DILF_ODES	1	/* .descr */
#define DILF_NXT	2	/* .next */
#define DILF_NAM	3	/* .name */
#define DILF_TIT	4	/* .title */
#define DILF_EXT	5	/* .extra */
#define DILF_OUT	6	/* .outside */
#define DILF_INS	7	/* .inside */
#define DILF_GNX	8	/* .gnext */
#define DILF_GPR	9	/* .gprevious */
#define DILF_FUN	10	/* .hasfun */
#define DILF_MHP	11	/* .max_hp */
#define DILF_CHP	12	/* .hp */
#define DILF_MAN	13	/* .manipulate */
#define DILF_FL		14	/* .flags */
#define DILF_BWT	15	/* .baseweight */
#define DILF_WGT	16	/* .weight */
#define DILF_CAP	17	/* .capacity */
#define DILF_ALG	18	/* .alignment */
#define DILF_FGT	19	/* .fighting */
#define DILF_OFL	20	/* .openflags */
#define DILF_LGT	21	/* .light */
#define DILF_BGT	22	/* .bright */
#define DILF_ILL	23	/* .illum */
#define DILF_SPL	24	/* .spell */
#define DILF_VAL	25	/* .values */
#define DILF_EFL	26	/* .objectflags */
#define DILF_CST	27	/* .cost */
#define DILF_RNT	28	/* .rent */
#define DILF_OTY	29	/* .objecttype */
#define DILF_EQP	30	/* .equip */
#define DILF_MOV	31	/* .movement */
#define DILF_ONM	32	/* .exit_name */
#define DILF_XNF	33	/* .exit_info */
#define DILF_TOR	34	/* .exit_to */
#define DILF_RFL	35	/* .roomflags */
#define DILF_ZON	36	/* .zone */
	
#define DILF_ACT	37	/* .behavior */
#define DILF_TIM	38	/* .time */
#define DILF_EXP	39	/* .exp */
#define DILF_AFF	40	/* .affected */
#define DILF_MNA	41	/* .mana */
#define DILF_END	42	/* .endurance */
#define DILF_ATY	43	/* .attack_type */
#define DILF_DEF	44	/* .defaultpos */
#define DILF_HGT	45	/* .height */
#define DILF_RCE	46	/* .race */
#define DILF_DRE	47	/* .dex_red */
#define DILF_SEX	48	/* .sex */
#define DILF_LVL	49	/* .level */
#define DILF_POS	50	/* .position */
#define DILF_ABL	51	/* .abilities */
#define DILF_WPN	52	/* .weapons */

#define DILF_MAS	53	/* .master */
#define DILF_FOL	54	/* .follower */
#define DILF_MIV	55	/* .minv */
#define DILF_SPT	56	/* .skill_points */
#define DILF_APT	57	/* .ability_points */
#define DILF_SKL	58	/* .skills */
#define DILF_GLD	59	/* .guild */
#define DILF_CRM	60	/* .crime */
#define DILF_FLL	61	/* .full */
#define DILF_THR	62	/* .thirst */
#define DILF_DRK	63	/* .drunk */
#define DILF_QST	64	/* .quests */
#define DILF_IDX	65	/* .[] */
#define DILF_NMI	66	/* .nameidx */
#define DILF_ZOI	67	/* .zoneidx */
#define DILF_TYP	68	/* .objecttype */
	
#define DILF_BIR        69      /* .birth */
#define DILF_PTI        70      /* .playtime */
#define DILF_PCF        71      /* .pcflags */
#define DILF_HOME       72      /* .home */
#define DILF_IDES	73	/* .inside_descr */
#define DILF_DES	74      /* .descr */
#define DILF_LCN        75      /* .loadcount */
#define DILF_SID        76      /* .idx (symbolic index as integer) */
#define DILF_SPD        77      /* .speed */
#define DILF_ABAL       78      /* .acc_balance */
#define DILF_ATOT       79      /* .acc_total   */
#define DILF_MMA	80	/* .max_mana */
#define DILF_LSA	81	/* .lifespan */
#define DILF_INFO	82	/* .info */
#define DILF_MED	83	/* .max_endurance */

#define DILF_MAX 	83      /* The maximum field number */
	
/* Legal variable values */
#define DILV_UP         1       /* unit pointer Rexpr Var */
#define DILV_SP         2       /* string pointer Rexpr Var */
#define DILV_SLP        3       /* string list pointer Var */
#define DILV_EDP        4       /* extra description pointer Rexpr Var */
#define DILV_INT        5       /* integer Rexpr Var */
	
/* Other return values */
#define DILV_UPR        6       /* unit pointer ref Lexpr */
#define DILV_SPR        7       /* string pointer Rexpr */
#define DILV_SLPR       8       /* string list pointer */
#define DILV_EDPR       9       /* extra descr pointer ref Lexpr */
	
/* Different pointers for different size fields */
#define DILV_SINT1R     10      /* integer ref, 8 bit, sgn Lexpr */
#define DILV_SINT2R     11      /* integer ref, 16 bit, sgn Lexpr */
#define DILV_SINT4R     12      /* integer ref, 32 bit, sgn Lexpr */
#define DILV_UINT1R     13      /* integer ref, 8 bit, usgn Lexpr */
#define DILV_UINT2R     14      /* integer ref, 16 bit, usgn Lexpr */
#define DILV_UINT4R     15      /* integer ref, 32 bit, usgn Lexpr */

#define DILV_ERR	16      /* fatal error value */
#define DILV_NULL       17      /* value is null */
#define DILV_FAIL       18      /* value is failed */
	
#define DILV_UEDP       19      /* FOR COMPILER ONLY! (shared fields) */
#define DILV_FUNC       20      /* FOR COMPILER ONLY! functions */
#define DILV_PROC       21      /* FOR COMPILER ONLY! procedures */
#define DILV_HASHSTR    22      /* Hashed String */
#define DILV_MAX        22      /* Max number */

/* DIL variable structure */
struct dilvar
{
   ubit8 type;		/* variable type */
   union {
      struct unit_data*          unitptr;
      sbit32                     integer;
      struct extra_descr_data*   extraptr;
      char                       *string;
      class cStringInstance      *pHash;
      class cNamelist            *namelist;
   } val;
};

/* allocation strategy */
#define DILA_NONE	0	/* not malloc (int) */
#define DILA_NORM	1	/* normal malloc */
#define DILA_EXP	2	/* temp. expression malloc */

/* DIL evaluation result. */
class dilval
{
  public:
   dilval(void) { type = DILV_FAIL; }
   ~dilval(void);

   ubit8 type;	                /* result type     */
   ubit8 atyp;		        /* allocation type */
   union {
      void *ptr;	        /* result pointer  */
      int64_t num;       	/* result integer  MS 2020 (32->64 bit)*/
   } val;
   void *ref;		        /* result reference (NULL=Rexpr) */
};

/* structure for securing unit pointers */
struct dilsecure
{
   struct unit_data *sup;       /* A direct reference to the variabel! */
   ubit8  *lab;			/* address to jump to, NULL=foreach */
};

/*
 *  An external reference.
 *  For each external reference, the name and cooresponding
 *  argument and return types will be saved
 */	
struct dilxref
{
   char *name; 			/* func/proc name [@ zone] */
   ubit8 rtnt;			/* return type */
   ubit8 argc;			/* number of arguments (min 1) */
   ubit8 *argt;                 /* argument types */
};
	
/*
 *  A DIL template for registering DIL programs/functions/procedures.
 *  Inline code is registered as local instances.
 *  Uppon loading old dil programs, an unlinked template is created.	
 */
struct diltemplate
{
   const char *prgname;		/* program name @ zone */
   struct zone_type *zone;      /* Pointer to owner of structure    */

   ubit8 flags;			/* recall, etc. */
   ubit16 intrcount;            /* max number of interrupts */
   ubit16 varcrc;		/* variable crc from compiler */
   ubit16 corecrc;		/* core crc from compiler */
   ubit8 rtnt;			/* return type */
   ubit8 argc;			/* number of arguments */
   ubit8 *argt;			/* argument types */

   ubit32 coresz;               /* size of coreblock */
   ubit8 *core;                 /* instructions, expressions and statics */
   
   ubit16 varc;                 /* number of variables */
   ubit8 *vart;                 /* variable types */

   ubit16 xrefcount;		/* number of external references   */
   struct diltemplate **extprg; /* external programs (SERVER only) */
   struct dilxref *xrefs;	/* external references (DMC only)  */

   ubit32 nActivations;         /* Number of activations           */

   struct diltemplate *next;    /* for zone templates              */
};

struct dilintr
{
   ubit16 flags;                /* what message types to react on 0=off */
   ubit8 *lab;                  /* where to perform check */
};
   
/*
 *  A stack frame for a DIL call of function or procedure.
 *  The frame contains runtime values for proc/func execution,
 *  including the return variable number for calling proc/func.
 *  Uppon call, a new stackframe is created from the called
 *  template. The needed memory is allocated in one chunk.
 */
struct dilframe
{
   ubit16 ret;                  /* return variable # (not saved) */ 
   struct diltemplate *tmpl;    /* current template */
   struct dilvar *vars;         /* variables */

   ubit8 *pc;                   /* program counter */

   ubit16 securecount;          /* number of secures (not saved) */
   struct dilsecure *secure;    /* secured vars (not saved) */

   ubit16 intrcount;            /* number of interrupts */
   struct dilintr *intr;        /* interrupts */
};

/*
 *   A dil process.
 *   The program is the runtime values of the process execution.
 */

#define DIL_STACKINC    8       /* # of stackframes to inc stack with */

struct dilprg
{
   ubit8 flags;                 /* Recall, copy, etc. */
   ubit16 varcrc;               /* variable crc from compiler (saved) */
   ubit16 corecrc;		/* core crc from compiler (saved) */

   ubit16 stacksz;		/* stack size */
   struct dilframe *sp;         /* stack and pointer */
   struct dilframe *stack;      /* stack frames, #0 saved */

   struct spec_arg *sarg;
   struct unit_data *owner;

   sbit16 waitcmd;              /* Command countdown */

   struct dilprg *next;         /* For global dilprg list (sendtoalldil) */
};

extern struct dilprg *dil_list;
extern struct dilprg *dil_list_nextdude;

/* Function prototypes */
void free_prg(struct dilprg *prg);
void clear_prg(struct dilprg *prg);
	
#endif /* _MUD_DIL_H */
