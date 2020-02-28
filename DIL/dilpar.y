/* *********************************************************************** *
 * File   : dilpar.y                                  Part of Valhalla MUD *
 * Version: 3.10                                                           *
 * Author : bombman@diku.dk                                                *
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

/* 21-Aug-94 gnort: Added iscommand("foo")
 *                  Tiny bugfix in send
 * 26-Aug-94 gnort: Changed above iscommand to accept either "foo" or CMD_FOO
 *                  Killed old command and changed iscommand to command()
 * 28-Jan-95 HHS:   Reconstructed code generation entirely
 * 14-Apr-95 HHS:   Added for and foreach, break and continue
 * Wed Jan 22 14:57:30 PST 1997   HHS added paycheck dilfun DILSE_PCK/ DILE_PCK
 */

/*
 * Definitions and type associations.
 */

%{
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

 int verbose = 0; // MS2020
 
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
%}

%union {
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
}

%{
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
%}

/* general */
%token '(' ')' '{' '}' '[' ']' ',' ';' ':'
%token UNKNOWN
%token <str> STRING
%token <sym> SYMBOL
%token <num> PNUM

%type <exp> dilexp dilsexp dilterm dilfactor variable dilfun idx field
%type <num> cmp icmp pcmp scmp type ihold ahold iunhold
%type <ins> dilinstlist dilproc dilinst block dilcomposed dilcomplex 
%type <ins> label labelskip labellist coreexp corevar proccall 
%type <str_list> stringlist strings
%type <ins> dilass dilassrgt corefunc

/* DIL code */
%token DILSC_VAR DILSC_BEG DILSC_COD DILSC_END DILSC_EXT DILSC_REC DILSC_AWA

/* DIL functions */
%token DILSE_ATOI DILSE_RND  DILSE_FNDU DILSE_FNDRU DILSE_FNDR DILSE_LOAD
%token DILSE_GETW DILSE_ITOA DILSE_ISS  DILSE_IN   DILSE_ISA
%token DILSE_CMDS DILSE_FNDS DILSE_GETWS DILSE_LEN DILSE_PURS DILSE_TRMO
%token DILSE_DLD  DILSE_DLF  DILSE_LAND DILSE_LOR DILSE_VISI DILSE_OPPO
%token DILSE_RTI  DILSE_PCK

%token DILSE_AND DILSE_OR DILSE_NOT

/* DIL built-in variables */
%token DILTO_EQ DILTO_NEQ DILTO_PEQ DILTO_SEQ DILTO_LEQ DILTO_GEQ
%token DILTO_GT DILTO_LE DILTO_ASS

/* DIL built-in variables */
%token DILSE_NULL DILSE_SELF DILSE_ACTI DILSE_ARGM DILSE_HRT DILSE_CMST
%token DILSE_TDA  DILSE_THO  DILSE_TMD  DILSE_TYE DILSE_SKIP DILSE_WEAT
%token DILSE_MEDI DILSE_TARG DILSE_POWE DILSE_CST DILSE_MEL DILSE_EQPM
%token DILSE_OPRO DILSE_REST DILSE_FIT  DILSE_CARY DILSE_PATH DILSE_MONS
%token DILSE_SPLX DILSE_SPLI
%token DILSE_TXF  DILSE_AST

/* DIL procedures */
%token DILSI_LNK  DILSI_EXP DILSI_SET DILSI_UST DILSI_ADE DILSI_SUE
%token DILSI_DST  DILSI_ADL DILSI_SUL DILSI_SND DILSI_SNT
%token DILSI_SEC  DILSI_USE DILSI_ADA DILSI_SETF DILSI_CHAS
%token DILSI_SUA  DILSI_EQP DILSI_UEQ DILSI_OAC DILSI_STOR DILSI_SETE
%token DILSI_QUIT DILSI_LOG DILSI_SNTA DILSI_SNTADIL DILSI_DLC DILSE_INTR
%token DILSI_CLI  DILSI_SWT DILSI_SBT DILSE_ATSP DILSI_FOLO DILSI_LCRI

/* DIL instructions */
%token DILSI_WHI DILSI_IF  DILSI_WLK DILSI_EXE DILSI_WIT DILSI_ACT
%token DILSI_ELS DILSI_GOT DILSI_PRI DILSI_NPR DILSI_BLK DILSI_CNT
%token DILSI_PUP DILSI_FOR DILSI_FOE DILSI_BRK DILSI_RTS
%token DILSI_ON  DILSI_AMOD

/* DIL structure fields */
%token DILSF_ZOI DILSF_NMI DILSF_TYP DILSF_NXT DILSF_NMS DILSF_NAM DILSF_IDX
%token DILSF_TIT DILSF_EXT DILSF_OUT DILSF_INS DILSF_GNX DILSF_SPD
%token DILSF_GPR DILSF_LGT DILSF_BGT DILSF_MIV DILSF_ILL DILSF_FL 
%token DILSF_OFL DILSF_MHP DILSF_CHP DILSF_BWT DILSF_WGT DILSF_CAP
%token DILSF_ALG DILSF_SPL DILSF_FUN DILSF_ZON DILSF_MAS DILSF_FOL
%token DILSF_OTY DILSF_VAL DILSF_EFL DILSF_CST DILSF_RNT DILSF_EQP
%token DILSF_ONM DILSF_XNF DILSF_TOR DILSF_RFL DILSF_MOV DILSF_ACT
%token DILSF_SEX DILSF_RCE DILSF_ABL DILSF_EXP DILSF_LVL DILSF_HGT
%token DILSF_POS DILSF_ATY DILSF_MNA DILSF_END DILSF_WPN DILSF_MAN
%token DILSF_AFF DILSF_DRE DILSF_FGT DILSF_DEF DILSF_TIM DILSF_CRM
%token DILSF_FLL DILSF_THR DILSF_DRK DILSF_SPT DILSF_APT DILSF_GLD
%token DILSF_QST DILSF_SKL DILSF_BIR DILSF_PTI DILSF_PCF DILSF_HOME
%token DILSF_ODES DILSF_IDES DILSF_DES DILSF_LCN DILSF_ABAL DILSF_ATOT
%token DILSF_MMA DILSF_LSA DILSF_INFO DILSF_MED

/* DIL types */
%token DILST_UP DILST_INT DILST_SP DILST_SLP DILST_EDP

%left '+' '-' '*' '/' '%'
%right UMINUS UPLUS DILSE_NOT

%%

file     : program
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
		  if (verbose)
		    fprintf(stderr," prg : %-20s ", nBuf);

		  bwrite_dil(pBuf, &prg);
		  /* dumpdil(&prg); */
	       }
	       else
	       {
		  if (verbose)
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
	    if (verbose)
	      fprintf(stderr," (%5d bytes)\n", pBuf->GetLength());
            printf("Data in: %s\n", tmpfname);
	    if (errcon)
	      remove(tmpfname);
         }
         ;
         
program  : DILSC_BEG dilinit diloptions fundef dilrefs
                       dildecls DILSC_COD block DILSC_END
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
         ;


diloptions: DILSC_REC diloptions
          {
             SET_BIT(tmpl.flags, DILFL_RECALL);
             SET_BIT(prg.flags, DILFL_RECALL);
          }
          | DILSC_AWA diloptions
          {
             SET_BIT(tmpl.flags, DILFL_AWARE);
             SET_BIT(prg.flags, DILFL_AWARE);
          }
          | /* Naught */
          {
          }
          ;

dilinit   : /* nothing */
         {
	    if (verbose)
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
         ;

dildecls : /* naught */
         | DILSC_VAR decls
         {
            tmpl.varcrc = dil_headercrc(var_names, tmpl.vart);
	    prg.varcrc = tmpl.varcrc;
         }
         ;

decls    : SYMBOL ':' type ';' decls
         {
            add_var($1,$3);
         }
         | /* nothing */
         ;
         
dilrefs  : DILSC_EXT refs
         | /* nothing */
         ;


refs     : ref refs
         {
         }
         | ref
         {
         }
         ;

ref      : SYMBOL '(' arginit refarglist ')' ';'
         {
            char zbuf[255];
            char nbuf[255];
            
            /* define procedure setup */
            split_fi_ref($1,zbuf,nbuf);

	    if ((strlen(zbuf) ? strlen(zbuf) : -1) + strlen(nbuf) + 1 !=
		strlen($1))
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
         | type SYMBOL '(' arginit refarglist ')' ';'
         {
            char zbuf[255];
            char nbuf[255];

            /* define function setup */
            split_fi_ref($2,zbuf,nbuf);

	    if ((strlen(zbuf) ? strlen(zbuf) : -1) + strlen(nbuf) + 1 !=
		strlen($2))
	    {
	       sprintf(zbuf, "Name of dilprogram '%s' was too long.",
		       nbuf);
	       fatal(zbuf);
	    }

            if (!*zbuf)
              strcpy(zbuf,cur_zonename);

            ref.name = str_dup(nbuf);
            ref.zname = str_dup(zbuf);
            ref.rtnt = $1;
	    add_ref(&ref);
         }
         ;

arginit  : /* naught */
         {
	    /* init arglist */
            ref.argc=0;
         }
         
refarglist : /* naught */
           | somerefargs
           ;

somerefargs : refarg ',' somerefargs
            | refarg
            ;
			
refarg   : SYMBOL ':' type
         {
            /* collect argument */
            ref.argt[ref.argc] = $3;
            ref.argv[ref.argc] = str_dup($1);
            if (++ref.argc > ARGMAX)
               fatal("Too many arguments");
         }
       
fundef   : ref
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
         | /* nothing */
         {
	    fatal("All DIL programs must have a name");
            *cur_tmplref='\0';
         }
         ;
         
/* ************************************************************* */
/*                      DIL types                                */
/* ************************************************************* */

type      : DILST_UP
         {
            $$ = DILV_UP;
         }
         | DILST_INT
         {
            $$ = DILV_INT;
         }
         | DILST_SP
         {
            $$ = DILV_SP;
         }
         | DILST_SLP
         {
            $$ = DILV_SLP;
         }
         | DILST_EDP
         {
            $$ = DILV_EDP;
         }
         ;

/* ************************************************************* */
/*                      DIL variables                            */
/* ************************************************************* */

variable : variable '.' field
         {
            INITEXP($$);
	    if ($1.dsl==DSL_FCT)
	       fatal("Attempting to derive fields of proc/func");

            switch ($3.rtyp)
	    {
               /* these structures have fields */
               case DILV_UP:
               case DILV_EDP:
               case DILV_SLP:
               case DILV_SP:
                  if ($1.typ != $3.rtyp)
                     fatal("Illegal type for field");
               break;

               case DILV_UEDP: /* shared fields */
                  if ($1.typ != DILV_UP && $1.typ != DILV_EDP)
                     fatal("Illegal type for field");
               break;
               default:
                  fatal("Illegal field");
               break;
            }

            /* create code */
            add_ubit8(&($$),DILE_FLD);
            add_ubit8(&($$),$3.num);
            add_code(&($$),&($1));

            if ($3.codep != $3.code) /* index optinal */
               add_code(&($$),&($3)); 

            if ($3.typ == DILV_UEDP)
               $$.typ = $1.typ; /* inherit type */
            else
               $$.typ = $3.typ; /* follow type */

            $$.dsl = $3.dsl;   /* static/dynamic/lvalue */

	    if ($1.typ == DILV_SLP && $1.dsl == DSL_LFT)
	      $$.dsl = DSL_LFT;

            FREEEXP($1);
            FREEEXP($3);
         }         
         | ihold SYMBOL iunhold
         {
            int i,refnum;
	    char nbuf[255],zbuf[255],buf[255];

            INITEXP($$);

            if ((i = search_block($2, (const char **) var_names, TRUE)) == -1)
	    {
	       /* not a variable */
	       /* check external function */

	       split_fi_ref($2,zbuf,nbuf);
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
	       $$.dsl = DSL_FCT;
	       $$.rtyp = refnum;
	       $$.typ = tmpl.xrefs[refnum].rtnt;
               /*fprintf(stderr, "Adding ubit16 reference %d %p.\n",
		       refnum, &($$));*/
               add_ubit16(&($$), refnum);
            }
	    else /* A variable */
	    {
               $$.typ = tmpl.vart[i];
               $$.dsl = DSL_LFT;
               add_ubit8(&($$),DILE_VAR);
               add_ubit16(&($$),i);
            }
         }
         | DILSE_SELF
         {
            INITEXP($$);
            $$.dsl = DSL_DYN;
            $$.typ = DILV_UP;
            add_ubit8(&($$),DILE_SELF);
         }
         | DILSE_ACTI
         {
            INITEXP($$);
            $$.dsl = DSL_DYN;
            $$.typ = DILV_UP;
            add_ubit8(&($$),DILE_ACTI);
         }
         | DILSE_MEDI
         {
            INITEXP($$);
            $$.dsl = DSL_DYN;
            $$.typ = DILV_UP;
            add_ubit8(&($$),DILE_MEDI);
         }
         | DILSE_TARG
         {
            INITEXP($$);
            $$.dsl = DSL_DYN;
            $$.typ = DILV_UP;
            add_ubit8(&($$),DILE_TARG);
         }
         | DILSE_POWE
         {
            INITEXP($$);
            $$.dsl = DSL_LFT;
            $$.typ = DILV_INT;
            add_ubit8(&($$),DILE_POWE);
         }
         | DILSE_CMST
         {
            INITEXP($$);
            $$.dsl = DSL_DYN;
            $$.typ = DILV_SP;
            add_ubit8(&($$),DILE_CMST);
         }
         | DILSE_ARGM
         {
            INITEXP($$);
            $$.dsl = DSL_DYN;
            $$.typ = DILV_SP;
            add_ubit8(&($$),DILE_ARGM);
         }
         | DILSE_HRT
         {
            INITEXP($$);
            $$.dsl = DSL_LFT;
            $$.typ = DILV_INT;
            add_ubit8(&($$),DILE_HRT);
         }
         | DILSE_WEAT
         {
            INITEXP($$);
            $$.dsl = DSL_DYN;
            $$.typ = DILV_INT;
            add_ubit8(&($$),DILE_WEAT);
	 }
         | DILSE_THO
         {
            INITEXP($$);
            $$.dsl = DSL_DYN;
            $$.typ = DILV_INT;
            add_ubit8(&($$),DILE_THO);
         }
         | DILSE_TDA
         {
            INITEXP($$);
            $$.dsl = DSL_DYN;
            $$.typ = DILV_INT;
            add_ubit8(&($$),DILE_TDA);
         }
         | DILSE_TMD
         {
            INITEXP($$);
            $$.dsl = DSL_DYN;
            $$.typ = DILV_INT;
            add_ubit8(&($$),DILE_TMD);
         }
         | DILSE_TYE
         {
            INITEXP($$);
            $$.dsl = DSL_DYN;
            $$.typ = DILV_INT;
            add_ubit8(&($$),DILE_TYE);
         }
         | DILSE_RTI
         {
            INITEXP($$);
            $$.dsl = DSL_DYN;
            $$.typ = DILV_INT;
            add_ubit8(&($$),DILE_RTI);
         }
         ;

/* ************************************************************* */
/*                      DIL fields                               */
/* ************************************************************* */
/* If index, returns it as dyn. code. Returns field type in num  */

/* ******************************** */
/* stringlist index :               */
/* ******************************** */
/* . [ exp ] */

idx      : '[' dilexp ']'
         {
            INITEXP($$);
	    checkbool("index in array",$2.boolean);
            if ($2.typ != DILV_INT)
               fatal("Integer expected for index");
            else 
               make_code(&($2));     /* make it dynamic */
            copy_code(&($$),&($2)); /* copy the code   */
            FREEEXP($2);
         }
         ;
         
field    : idx
         {
            INITEXP($$);
            copy_code(&($$),&($1));
            $$.rtyp = DILV_SLP;
            $$.typ = DILV_SP;
            $$.dsl = DSL_DYN;
            $$.num = DILF_IDX;
            FREEEXP($1);
         }

/* ******************************** */
/* unit fields :                    */
/* ******************************** */

         | DILSF_BIR       /* .birth */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_BIR;
         }
         | DILSF_PTI       /* .playtime */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_DYN;
            $$.num = DILF_PTI;
         }
         | DILSF_PCF       /* .pcflags */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_PCF;
         }
         | DILSF_HOME       /* .home */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_SP;
            $$.dsl = DSL_LFT;
            $$.num = DILF_HOME;
         }
         | DILSF_MAS       /* .master */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_UP;
            $$.dsl = DSL_DYN;
            $$.num = DILF_MAS;
         }
         | DILSF_FOL       /* .follower */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_UP;
            $$.dsl = DSL_DYN;
            $$.num = DILF_FOL;
         }
         | DILSF_IDX       /* .idx */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_DYN;
            $$.num = DILF_SID;
         }
         | DILSF_ZOI       /* .zoneidx */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_SP;
            $$.dsl = DSL_DYN;
            $$.num = DILF_ZOI;
         }
         | DILSF_NMI       /* .nameidx */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_SP;
            $$.dsl = DSL_DYN;
            $$.num = DILF_NMI;
         }
         | DILSF_TYP       /* .type */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_DYN;
            $$.num = DILF_TYP;
         }
         | DILSF_NXT       /* .next */
         {
            INITEXP($$);
            $$.rtyp = DILV_UEDP;
            $$.typ = DILV_UEDP;
            $$.dsl = DSL_DYN;
            $$.num = DILF_NXT;
         }
         | DILSF_NMS       /* .names */
         {
            INITEXP($$);
            $$.rtyp = DILV_UEDP;
            $$.typ = DILV_SLP;
            $$.dsl = DSL_LFT;
            $$.num = DILF_NMS;
         }
         | DILSF_NAM       /* .name */
         {
            INITEXP($$);
            $$.rtyp = DILV_UEDP;
            $$.typ = DILV_SP;
            $$.dsl = DSL_DYN;
            $$.num = DILF_NAM;
         }
         | DILSF_DES        /* .descr (extras only) */
         {
            INITEXP($$);
            $$.rtyp = DILV_EDP;
            $$.typ = DILV_SP;
            $$.dsl = DSL_LFT;
            $$.num = DILF_DES;
         }
         | DILSF_ODES       /* .outside_descr */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_SP;
            $$.dsl = DSL_LFT;
            $$.num = DILF_ODES;
         }
         | DILSF_IDES       /* .inside_descr */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_SP;
            $$.dsl = DSL_LFT;
            $$.num = DILF_IDES;
         }
         | DILSF_TIT       /* .title */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_SP;
            $$.dsl = DSL_LFT;
            $$.num = DILF_TIT;
         }
         | DILSF_EXT       /* .extra */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_EDP;
            $$.dsl = DSL_DYN;
            $$.num = DILF_EXT;
         }
         | DILSF_OUT       /* .outside */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_UP;
            $$.dsl = DSL_DYN;
            $$.num = DILF_OUT;
         }
         | DILSF_INS       /* .inside */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_UP;
            $$.dsl = DSL_DYN;
            $$.num = DILF_INS;
         }
         | DILSF_GNX       /* .gnext */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_UP;
            $$.dsl = DSL_DYN;
            $$.num = DILF_GNX;
         }
         | DILSF_LCN
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_DYN;
            $$.num = DILF_LCN;
         }
         | DILSF_GPR       /* .gprevious */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_UP;
            $$.dsl = DSL_DYN;
            $$.num = DILF_GPR;
         }
         | DILSF_LGT       /* .light */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_DYN;
            $$.num = DILF_LGT;
         }
         | DILSF_BGT       /* .bright */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_DYN;
            $$.num = DILF_BGT;
         }
         | DILSF_MIV       /* .minv */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_MIV;
         }
         | DILSF_ILL       /* .illum */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_DYN;
            $$.num = DILF_ILL;
         }
         | DILSF_FL        /* .flags */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_FL;
         }
         | DILSF_MAN       /* .manipulate */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_MAN;
         }
         | DILSF_OFL       /* .openflags */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_OFL;
         }
         | DILSF_MHP       /* .max_hp */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_MHP;
         }
         | DILSF_INFO       /* .info */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_EDP;
            $$.dsl = DSL_DYN;
	    $$.num = DILF_INFO;
         }
         | DILSF_LSA       /* .lifespan */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_LSA;
         }
         | DILSF_MED       /* .max_endurance */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_DYN;
            $$.num = DILF_MED;
         }
         | DILSF_MMA       /* .max_mana */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_DYN;
            $$.num = DILF_MMA;
         }
         | DILSF_CHP       /* .hp */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_CHP;
         }
         | DILSF_BWT       /* .baseweight */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_DYN;
            $$.num = DILF_BWT;
         }
         | DILSF_WGT       /* .weight */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_DYN;
            $$.num = DILF_WGT;
         }
         | DILSF_CAP       /* .capacity */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_CAP;
         }
         | DILSF_ALG       /* .alignment */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_ALG;
         }
         | DILSF_SPL idx   /* spells */
         {
            INITEXP($$);
            copy_code(&($$),&($2));
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_SPL;
         }
         | DILSF_FUN       /* .hasfunc */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_DYN;
            $$.num = DILF_FUN;
         }
         | DILSF_ZON       /* .zone */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_SP;
            $$.dsl = DSL_DYN;
            $$.num = DILF_ZON;
         }
         
/* ******************************** */
/* object fields :                  */
/* ******************************** */

         | DILSF_OTY       /* .objecttype */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_OTY;
         }
         | DILSF_VAL idx   /* .value */
         {
            INITEXP($$);
            copy_code(&($$),&($2));
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_VAL;
            FREEEXP($2);
         }
         | DILSF_EFL       /* .objectflags */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_EFL;
         }
         | DILSF_CST       /* .cost */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_CST;
         }
         | DILSF_RNT       /* .rent */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_RNT;
         }
         | DILSF_EQP       /* .equip */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_DYN;
            $$.num = DILF_EQP;
         }

/* ******************************** */
/* room fields :                    */
/* ******************************** */

         | DILSF_ONM idx   /* .exit_names */
         {
            INITEXP($$);
            copy_code(&($$),&($2));
            $$.rtyp = DILV_UP;
            $$.typ = DILV_SLP;
            $$.dsl = DSL_LFT;
            $$.num = DILF_ONM;
            FREEEXP($2);
         }
         | DILSF_XNF idx   /* .exit_info */
         {
            INITEXP($$);
            copy_code(&($$),&($2));
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_XNF;
            FREEEXP($2);
         }
         | DILSF_TOR idx   /* .exit_to */
         {
            INITEXP($$);
            copy_code(&($$),&($2));
            $$.rtyp = DILV_UP;
            $$.typ = DILV_UP;
            $$.dsl = DSL_DYN;
            $$.num = DILF_TOR;
            FREEEXP($2);
         }
         | DILSF_RFL       /* .roomflags */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_RFL;
         }
         | DILSF_MOV       /* .movement */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_MOV;
         }

/* ******************************** */
/* char fields :                    */
/* ******************************** */

         | DILSF_ABAL       /* .acc_balance */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_DYN;
            $$.num = DILF_ABAL;
         }
         | DILSF_ATOT       /* .acc_total */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_DYN;
            $$.num = DILF_ATOT;
         }
         | DILSF_SPD       /* .speed */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_DYN;
            $$.num = DILF_SPD;
         }
         | DILSF_SEX       /* .sex */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_SEX;
         }
         | DILSF_RCE       /* .race */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_RCE;
         }
         | DILSF_ABL idx   /* .abilities */
         {
            INITEXP($$);
            copy_code(&($$),&($2));
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_ABL;
            FREEEXP($2);
         }
         | DILSF_EXP       /* .exp */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_DYN;
            $$.num = DILF_EXP;
         }
         | DILSF_LVL       /* .level */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_DYN;
            $$.num = DILF_LVL;
         }
         | DILSF_HGT       /* .height */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_HGT;
         }
         | DILSF_POS       /* .position */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_POS;
         }
         | DILSF_ATY       /* .attack_type */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_ATY;
         }
         | DILSF_MNA       /* .mana */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_MNA;
         }
         | DILSF_END       /* .endurance */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_END;
         }
         | DILSF_AFF       /* .affected */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_AFF;
         }
         | DILSF_DRE       /* .dex_red */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_DYN;
            $$.num = DILF_DRE;
         }
         | DILSF_FGT       /* .fighting */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_UP;
            $$.dsl = DSL_DYN;
            $$.num = DILF_FGT;
         }
         | DILSF_WPN idx   /* .weapons */
         {
            INITEXP($$);
            copy_code(&($$),&($2));
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_WPN;
            FREEEXP($2);
         }

/* ******************************** */
/* NPC fields :                     */
/* ******************************** */

         | DILSF_DEF       /* .defaultpos */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_DEF;
         }
         | DILSF_ACT       /* .npcflags */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_ACT;
         }

/* ******************************** */
/* PC fields :                      */
/* ******************************** */

         | DILSF_TIM       /* .time */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_TIM;
         }
         | DILSF_CRM       /* .crimes */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_CRM;
         }
         | DILSF_FLL       /* .full */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_FLL;
         }
         | DILSF_THR       /* .thirst */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_THR;
         }
         | DILSF_DRK       /* .drunk */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_DRK;
         }
         | DILSF_SPT       /* .skill_points */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_DYN;
            $$.num = DILF_SPT;
         }
         | DILSF_APT       /* .ability_points */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_DYN;
            $$.num = DILF_APT;
         }
         | DILSF_GLD       /* .guild */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_SP;
            $$.dsl = DSL_LFT;
            $$.num = DILF_GLD;
         }
         | DILSF_QST       /* .quests */
         {
            INITEXP($$);
            $$.rtyp = DILV_UP;
            $$.typ = DILV_EDP;
            $$.dsl = DSL_DYN;
            $$.num = DILF_QST;
         }
         | DILSF_SKL idx   /* .skills */
         {
            INITEXP($$);
            copy_code(&($$),&($2));
            $$.rtyp = DILV_UP;
            $$.typ = DILV_INT;
            $$.dsl = DSL_LFT;
            $$.num = DILF_SKL;
            FREEEXP($2);
         }
         ;

/* ************************************************************* */
/*                        DIL comparisons                          */
/* ************************************************************* */

scmp      : DILTO_SEQ
         {
            $$ = DILE_SE;
         }
         ;

pcmp      : DILTO_PEQ
         {
            $$ = DILE_PE;
         }
         ;

cmp      : DILTO_EQ
         {
            $$ = DILE_EQ;
         }
         | DILTO_NEQ
         {
            $$ = DILE_NE;
         }
         ;

icmp     : DILTO_GT
         {
            $$ = DILE_GT;
         }
         | DILTO_LE
         {
            $$ = DILE_LT;
         }
         | DILTO_GEQ
         {
            $$ = DILE_GE;
         }
         | DILTO_LEQ
         {
            $$ = DILE_LE;
         }
         ;

/* ************************************************************* */
/*                      DIL expression                           */
/* ************************************************************* */

dilexp   : dilsexp
         {
/*          fprintf(stderr,"dilsexp->dilexp\n");*/
            INITEXP($$);
	    $$.boolean=$1.boolean;
            copy_code(&($$),&($1));
            FREEEXP($1);
         }
         | dilexp cmp dilsexp
         {
            INITEXP($$);
	    $$.boolean=1;
            switch ($1.typ) {
               /*
                * Replaces integer function identifies with
                * appropiate pointer/string function identifier
                * where appropriate.
                */

               case DILV_INT:
                  if ($3.typ != DILV_INT)
                     fatal("Arg 2 of integer comparison not integer");
                  break;
               case DILV_NULL:
               case DILV_UP:
               case DILV_EDP:
               case DILV_SLP:
                  if (($1.typ != $3.typ) &&
                      ($1.typ != DILV_NULL) && ($3.typ != DILV_NULL))
                     fatal("Argument of pointer compare not of same type.");
                  if ($2 == DILE_NE)
                     $2 = DILE_PNEQ; /* snask */
                  else
                     $2 = DILE_PE; /* snask */
                  break;
               case DILV_SP:
                  if ($3.typ == DILV_NULL) {
                     if ($2 == DILE_NE)
                        $2 = DILE_PNEQ; /* snask */
                     else
                        $2 = DILE_PE; /* snask */
                     break;
                  }
                  if ($3.typ != DILV_SP)
                     fatal("Arg 2 of string comp. not a string");
                  else {
                     if ($2 == DILE_NE)
                        $2 = DILE_SNEQ; /* snask */
                     else
                        $2 = DILE_SE; /* snask */
                  }
                  break;
               default:
                  fatal("Arg 1 of compare invalid type");
               break;
            }
            /* Make nodes dynamic */
            /* Integer compares are not _yet_ static */
            make_code(&($1));
            make_code(&($3));
            add_ubit8(&($$),$2);
            add_code(&($$),&($1));
            add_code(&($$),&($3));
            $$.dsl = DSL_DYN;
            $$.typ = DILV_INT;
            FREEEXP($1);
            FREEEXP($3);
         }
         | dilexp icmp dilsexp
         {
            INITEXP($$);
	    $$.boolean=1;
            if ($1.typ != DILV_INT)
               fatal("Arg 1 of integer comparison not integer");
            else if ($3.typ != DILV_INT)
               fatal("Arg 2 of integer comparison not integer");
            else
            {
               /* Make nodes dynamic */
               /* Integer compares are not _yet_ static */
               make_code(&($1));
               make_code(&($3));
               add_ubit8(&($$),$2);
               add_code(&($$),&($1));
               add_code(&($$),&($3));
               $$.dsl = DSL_DYN;
               $$.typ = DILV_INT;
            }
            FREEEXP($1);
            FREEEXP($3);
         }
         | dilsexp pcmp dilsexp
         {
            INITEXP($$);
	    $$.boolean=1;
            switch ($1.typ)
            {
             case DILV_NULL:
             case DILV_UP:
             case DILV_SP:
             case DILV_EDP:
             case DILV_SLP:
               if (($1.typ != $3.typ) &&
               ($1.typ != DILV_NULL) && ($3.typ != DILV_NULL))
                  fatal("Argument of pointer compare not of same type.");
               else
               {
                  make_code(&($1));
                  make_code(&($3));
                  add_ubit8(&($$),$2); /* compare funct */
                  add_code(&($$),&($1));
                  add_code(&($$),&($3));
                  $$.dsl = DSL_DYN;
                  $$.typ = DILV_INT;
               }
               break;

             default:
               fatal("Arg 1 of pointer comp. not a pointer");
               break;
            }
            FREEEXP($1);
            FREEEXP($3);
         }
         | dilsexp scmp dilsexp
         {
            INITEXP($$);
	    $$.boolean=1;
            if ($1.typ != DILV_SP)
               fatal("Arg 1 of string comp. not a string");
            else if ($1.typ != DILV_SP)
               fatal("Arg 2 of string comp. not a string");
            else
            {
               /* Strings are not _yet_ static */
               /* String compare not _yet_ static */
               make_code(&($1));
               make_code(&($3));
               add_ubit8(&($$),$2); /* compare funct */
               add_code(&($$),&($1));
               add_code(&($$),&($3));
               $$.dsl = DSL_DYN;
               $$.typ = DILV_INT;
            }
            FREEEXP($1);
            FREEEXP($3);
         }
         ;

dilsexp   : dilterm
         {
/*          fprintf(stderr,"dilterm->dilsexp\n");*/
            INITEXP($$);
	    $$.boolean=$1.boolean;
            copy_code(&($$),&($1));
            FREEEXP($1);
         }
         | '-' dilterm   %prec UMINUS
         {
            INITEXP($$);
	    $$.boolean=$2.boolean;
            if ($2.typ != DILV_INT)
               fatal("Arg 1 of 'unary -' not integer");
            else {
               /* Type is ok */
               $$.typ = DILV_INT;
               $$.dsl = $2.dsl;
               if (!$2.dsl)
                  $$.num =   - $2.num; /* static */
               else {
                  add_ubit8(&($$), DILE_UMIN); 
                  add_code(&($$),&($2));
               }
            }
            FREEEXP($2);
         }
         | '+' dilterm   %prec UPLUS
         {
            INITEXP($$);
	    $$.boolean=$2.boolean;
            if ($2.typ != DILV_INT)
               fatal("Arg 1 of 'unary +' not integer");
            else {
               /* Type is ok */
               copy_code(&($$),&($2));
            }
            FREEEXP($2);
         }
         | dilsexp '+' dilterm
         {
            INITEXP($$);
	    $$.boolean=$1.boolean||$3.boolean;
            if ($1.typ != DILV_INT)
            {
               if ($1.typ != DILV_SP)
                  fatal("Arg 1 of '+' not integer or string");
               else if ($3.typ != DILV_SP)
                  fatal("Arg 2 of '+' not string");
               else {
                  /* Type is SP + SP */
                  $$.typ = DILV_SP;
                  if (!($1.dsl+$3.dsl)) {
                     /* both static */
                     copy_code(&($$),&($1));
                     cat_string(&($$),(char *) &($3.code[1]));
                     $$.dsl = DSL_STA;
                  } else {
                     /* one is dynamic */
                     make_code(&($1));
                     make_code(&($3));
                     add_ubit8(&($$),DILE_PLUS);
                     add_code(&($$),&($1));
                     add_code(&($$),&($3));
                     $$.dsl = DSL_DYN;
                  }
               }
            }
            else if ($3.typ != DILV_INT)
               fatal("Arg 2 of '+' not integer");
            else {
               /* Type is INT + INT */
               $$.typ = DILV_INT;
               if (!($1.dsl + $3.typ)) {
                  $$.num = $1.num + $3.num;
                  $$.dsl = DSL_STA;
               } else {
                  /* make nodes dynamic */
                  make_code(&($1));
                  make_code(&($3));
                  add_ubit8(&($$), DILE_PLUS);
                  add_code(&($$),&($1));
                  add_code(&($$),&($3));
                  $$.dsl = DSL_DYN;
               }
            }
            FREEEXP($1);
            FREEEXP($3);
         }
         | dilsexp '-' dilterm
         {
            INITEXP($$);
	    $$.boolean=$1.boolean||$3.boolean;
            if ($1.typ != DILV_INT)
               fatal("Arg 1 of '-' not integer");
            else if ($3.typ != DILV_INT)
               fatal("Arg 2 of '-' not integer");
            else {
               /* Type is ok */
               $$.typ = DILV_INT;
               if (!($1.dsl + $3.dsl)) {
                  $$.num = $1.num - $3.num;
                  $$.dsl = DSL_STA;
               } else {
                  /* make nodes dynamic */
                  make_code(&($1));
                  make_code(&($3));
                  add_ubit8(&($$), DILE_MIN);
                  add_code(&($$),&($1));
                  add_code(&($$),&($3));
                  $$.dsl = DSL_DYN;
               }
            }
            FREEEXP($1);
            FREEEXP($3);
         }
         | dilsexp DILSE_LOR dilterm
         {
            INITEXP($$);
	    $$.boolean=1;
            /* Type is ok */
            $$.typ = DILV_INT;
            if (!($1.dsl + $3.dsl) &&
                ($1.typ == DILV_INT) &&
                ($3.typ == DILV_INT)) {
               /* static integers */
               $$.num = $1.num || $3.num;
               $$.dsl = DSL_STA;
            } else {
               /* make nodes dynamic */
               make_code(&($1));
               make_code(&($3));
               add_ubit8(&($$), DILE_LOR);
               add_code(&($$),&($1));
               add_code(&($$),&($3));
               $$.dsl = DSL_DYN;
            }
            FREEEXP($1);
            FREEEXP($3);
         }
         | dilsexp DILSE_OR dilterm
         {
            INITEXP($$);
	    $$.boolean=$1.boolean||$3.boolean;
            /* Type is ok */
            $$.typ = DILV_INT;
            if (!($1.dsl + $3.dsl) &&
                ($1.typ == DILV_INT) &&
                ($3.typ == DILV_INT)) {
               /* static integers */
               $$.num = $1.num | $3.num;
               $$.dsl = DSL_STA;
            } else {
               /* make nodes dynamic */
               make_code(&($1));
               make_code(&($3));
               add_ubit8(&($$), DILE_OR);
               add_code(&($$),&($1));
               add_code(&($$),&($3));
               $$.dsl = DSL_DYN;
            }
            FREEEXP($1);
            FREEEXP($3);
         }
         ;
      
dilterm  : dilfactor
         {
/*          fprintf(stderr,"dilfactor->dilterm\n");*/
            INITEXP($$);
	    $$.boolean=$1.boolean;
            copy_code(&($$),&($1));
            FREEEXP($1);
         }
         | dilterm '*' dilfactor
         {
            INITEXP($$);
	    $$.boolean=$1.boolean||$3.boolean;
            if ($1.typ != DILV_INT)
               fatal("Arg 1 of '*' not integer");
            else if ($3.typ != DILV_INT)
               fatal("Arg 2 of '*' not integer");
            else {
               /* Type is ok */
               $$.typ = DILV_INT;
               if (!($1.dsl + $3.dsl)) {
                  /* Both values are static */
                  $$.dsl = DSL_STA;
                  $$.num = $1.num * $3.num;
               } else {
                  /* make nodes dynamic */
                  make_code(&($1));
                  make_code(&($3));
                  add_ubit8(&($$), DILE_MUL);
                  add_code(&($$),&($1));
                  add_code(&($$),&($3));
                  $$.dsl = DSL_DYN;
               }
            }
            FREEEXP($1);
            FREEEXP($3);
         }
         | dilterm '/' dilfactor
         {
            INITEXP($$);
	    $$.boolean=$1.boolean||$3.boolean;
            if ($1.typ != DILV_INT)
               fatal("Arg 1 of '/' not integer");
            else if ($3.typ != DILV_INT)
               fatal("Arg 2 of '/' not integer");
            else {
               /* Type is ok */
               $$.typ = DILV_INT;
               if (!($1.dsl + $3.dsl)) {
                  $$.dsl = DSL_STA;
                  $$.num = $1.num / $3.num;
               } else {
                  /* make nodes dynamic */
                  make_code(&($1));
                  make_code(&($3));
                  add_ubit8(&($$), DILE_DIV);
                  add_code(&($$),&($1));
                  add_code(&($$),&($3));
                  $$.dsl = DSL_DYN;
               }
            }
            FREEEXP($1);
            FREEEXP($3);
         }
         | dilterm DILSE_LAND dilfactor
         {
            INITEXP($$);
	    $$.boolean=1;
            /* Type is ok */
            $$.typ = DILV_INT;
            if (!($1.dsl + $3.dsl) &&
                ($1.typ == DILV_INT) &&
                ($3.typ == DILV_INT)) {
               $$.dsl = DSL_STA;
               $$.num = $1.num && $3.num;
            } else {
               /* make nodes dynamic */
               make_code(&($1));
               make_code(&($3));
               add_ubit8(&($$), DILE_LAND);
               add_code(&($$),&($1));
               add_code(&($$),&($3));
               $$.dsl = DSL_DYN;
            }
            FREEEXP($1);
            FREEEXP($3);
         }
         | dilterm DILSE_AND dilfactor
         {
            INITEXP($$);
	    $$.boolean=$1.boolean||$3.boolean;
            /* Type is ok */
            $$.typ = DILV_INT;
            if (!($1.dsl + $3.dsl) &&
                ($1.typ == DILV_INT) &&
                ($3.typ == DILV_INT)) {
               $$.dsl = DSL_STA;
               $$.num = $1.num & $3.num;
            } else {
               /* make nodes dynamic */
               make_code(&($1));
               make_code(&($3));
               add_ubit8(&($$), DILE_AND);
               add_code(&($$),&($1));
               add_code(&($$),&($3));
               $$.dsl = DSL_DYN;
            }
            FREEEXP($1);
            FREEEXP($3);
         }
         | dilterm '%' dilfactor
         {
            INITEXP($$);
	    $$.boolean=$1.boolean||$3.boolean;
            if ($1.typ != DILV_INT)
               fatal("Arg 1 of '%' not integer");
            else if ($3.typ != DILV_INT)
               fatal("Arg 2 of '%' not integer");
            else {
               /* Type is ok */
               $$.typ = DILV_INT;
               if (!($1.dsl + $3.dsl)) {
                  $$.dsl = DSL_STA;
                  $$.num = $1.num % $3.num;
               } else {
                  /* make nodes dynamic */
                  make_code(&($1));
                  make_code(&($3));
                  add_ubit8(&($$), DILE_MOD);
                  add_code(&($$),&($1));
                  add_code(&($$),&($3));
                  $$.dsl = DSL_DYN;
               }
            }
            FREEEXP($1);
            FREEEXP($3);
         }
         | dilterm DILSE_IN dilfactor
         {
            INITEXP($$);
	    $$.boolean=$1.boolean||$3.boolean;
            if ($1.typ != DILV_SP)
               fatal("Arg 1 of 'in' not string");
            else if (($3.typ != DILV_EDP) &&
                     ($3.typ != DILV_SLP) &&
                      ($3.typ != DILV_SP))
               fatal("Arg 2 of 'in' not string, string "
                     "list or extra description");
            else {
               /* Type is string in stringlist */
               /* Strings are not _yet_ static here */
               $$.dsl = DSL_DYN;
               switch ($3.typ) {
                  
                case DILV_SLP:
                  $$.typ = DILV_INT;
                  break;

                case DILV_EDP:
                  $$.typ = DILV_EDP;
                  break;

                case DILV_SP:
                  $$.typ = DILV_INT;
                  break;
               }
               make_code(&($1));
               make_code(&($3));
               add_ubit8(&($$), DILE_IN);
               add_code(&($$),&($1));
               add_code(&($$),&($3));
               $$.dsl = DSL_DYN;
            }
            FREEEXP($1);
            FREEEXP($3);
         }
         ;

dilfactor : '(' dilexp ')'
         {
            INITEXP($$);
	    $$.boolean=$2.boolean;
            copy_code(&($$),&($2));
	    FREEEXP($2); 
         }
         | dilfun
         {
            INITEXP($$);
	    $$.boolean=$1.boolean;
            copy_code(&($$),&($1));
	    FREEEXP($1);
         }
         | PNUM
         {
            INITEXP($$);
            $$.dsl = DSL_STA;
            $$.typ = DILV_INT;
            $$.num = $1;
         }
         | STRING
         {
            INITEXP($$);
/*            if (*$1) { */
               /* Strings are now static */
               $$.typ = DILV_SP;
               $$.dsl = DSL_STA;
               add_ubit8(&($$),DILE_FS);
               add_string(&($$),$1);
/* WHY NOT?           } else
               fatal("empty strings not allowed for now"); */
         }
         | stringlist
         {
            INITEXP($$);
            if (!$1[0])
               fatal("empty stringlists not allowed for now");
            else {
               /* write stringlist _NOT_ static */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_SLP;
               add_ubit8(&($$),DILE_FSL);
               add_stringlist(&($$),$1);
            }
         }
         | DILSE_NULL
         {
            INITEXP($$);
            $$.typ = DILV_NULL;
            $$.dsl = DSL_STA;
            $$.num = 0;
         }
         | variable
         {
	    if ($1.dsl==DSL_FCT)
	       fatal("Illegal use of proc/func");

            INITEXP($$);
            copy_code(&($$),&($1));
            FREEEXP($1);
         }
         | DILSE_NOT dilfactor %prec DILSE_NOT
         {
            INITEXP($$);
	    $$.boolean=1;
            /* Type is ok */
            $$.typ = DILV_INT;
            $$.dsl = $2.dsl;
            if (!$2.dsl && $2.typ == DILV_INT)
               $$.num = !$2.num;
            else {
               add_ubit8(&($$),DILE_NOT);
               add_code(&($$),&($2));
            }
            FREEEXP($2);
         }
         ;

/* ************************************************************* */
/*                        DIL functions                          */
/* ************************************************************* */

dilfun   : DILSE_ATOI '(' dilexp ')'
         {
            INITEXP($$);
            if ($3.typ != DILV_SP)
               fatal("Arg 1 of 'atoi' not string");
            else {
               /* Type is ok */
               /* Function is not _yet_ static */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_INT;
               make_code(&($3));
               add_ubit8(&($$),DILE_ATOI);
               add_code(&($$),&($3));
            }
            FREEEXP($3);
         }
         | DILSE_DLD '(' dilexp ',' dilexp ')'
         {
            INITEXP($$);
            if ($3.typ != DILV_SP)
               fatal("Arg 1 of 'dildestroy' not string");
            else if ($5.typ != DILV_UP)
               fatal("Arg 2 of 'dildestouy' not unitptr");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_INT;
               make_code(&($3));
               make_code(&($5));
               add_ubit8(&($$),DILE_DLD);
               add_code(&($$),&($3));
               add_code(&($$),&($5));
            }
            FREEEXP($3);
            FREEEXP($5);
	 }
         | DILSE_DLF '(' dilexp ',' dilexp ')'
         {
            INITEXP($$);
            if ($3.typ != DILV_SP)
               fatal("Arg 1 of 'dildestroy' not string");
            else if ($5.typ != DILV_UP)
               fatal("Arg 2 of 'dildestouy' not unitptr");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_INT;
               make_code(&($3));
               make_code(&($5));
               add_ubit8(&($$),DILE_DLF);
               add_code(&($$),&($3));
               add_code(&($$),&($5));
            }
            FREEEXP($3);
            FREEEXP($5);
	 }
         | DILSE_LEN '(' dilexp ')'
         {
	    INITEXP($$);
            if (($3.typ != DILV_SP) && ($3.typ != DILV_SLP))
               fatal("Arg 1 of 'length' not string or stringlist");
            else {
               /* Type is ok */
               /* Function is not _yet_ static */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_INT;
               make_code(&($3));
               add_ubit8(&($$),DILE_LEN);
               add_code(&($$),&($3));
            }
            FREEEXP($3);
         }
         | DILSE_ITOA '(' dilexp ')'
         {
            INITEXP($$);
            if ($3.typ != DILV_INT)
               fatal("Arg 1 of 'itoa' not integer");
            else {
               /* Type is ok */
               /* Function is not _yet_ static */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_SP;
               make_code(&($3));
               add_ubit8(&($$),DILE_ITOA);
               add_code(&($$),&($3));
            }
            FREEEXP($3);
         }
         | DILSI_ACT '(' dilexp ',' dilexp ',' dilexp ',' dilexp ',' dilexp ',' dilexp ')'
         {
	    /* Fri Jan 24 12:44:22 PST 1997   HHS:
	       This function writes act() as a function that
	       returns a string in an expression
	       */
	    INITEXP($$)
	    checkbool("argument 2 of act",$5.boolean);
	    checkbool("argument 6 of act",$13.boolean);
            if ($3.typ != DILV_SP)
               fatal("Arg 1 of 'act' not a string");
            else if ($5.typ != DILV_INT)
               fatal("Arg 2 of 'act' not an integer");
            else if ($7.typ != DILV_NULL &&
                $7.typ != DILV_SP &&
                $7.typ != DILV_UP)
               fatal("Arg 3 of 'act' not a unit or string");
            else if ($9.typ != DILV_NULL &&
                $9.typ != DILV_SP &&
                $9.typ != DILV_UP)
               fatal("Arg 4 of 'act' not a unit or string");
            else if ($11.typ != DILV_NULL &&
                $11.typ != DILV_SP &&
                $11.typ != DILV_UP)
               fatal("Arg 5 of 'act' not a unit or string");
            else if ($13.typ != DILV_INT)
               fatal("Arg 6 of 'act' not an integer");
            else {
	       /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_SP;

	       make_code(&($3));
	       make_code(&($5));
	       make_code(&($7));
	       make_code(&($9));
	       make_code(&($11));
	       make_code(&($13));

               add_ubit8(&($$),DILE_ACT);
	       add_code(&($$),&($3));
	       add_code(&($$),&($5));
	       add_code(&($$),&($7));
	       add_code(&($$),&($9));
	       add_code(&($$),&($11));
	       add_code(&($$),&($13));
            }
         }
         | DILSE_RND '(' dilexp ',' dilexp ')'
         {
            INITEXP($$);
            if ($3.typ != DILV_INT)
               fatal("Arg 1 of 'rnd' not integer");
            else if ($5.typ != DILV_INT)
               fatal("Arg 2 of 'rnd' not integer");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_INT;
               make_code(&($3));
               make_code(&($5));
               add_ubit8(&($$),DILE_RND);
               add_code(&($$),&($3));
               add_code(&($$),&($5));
            }
            FREEEXP($3);
            FREEEXP($5);
         }
         | DILSE_ISS '(' dilexp ',' dilexp ')'
         {
            INITEXP($$);
	    $$.boolean=1;
            if ($3.typ != DILV_INT)
               fatal("Arg 1 of 'isset' not integer");
            else if ($5.typ != DILV_INT)
               fatal("Arg 2 of 'isset' not integer");
            else {
               /* Type is ok */
               /* Function is not _yet_ static */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_INT;
               make_code(&($3));
               make_code(&($5));
               add_ubit8(&($$),DILE_ISS);
               add_code(&($$),&($3));
               add_code(&($$),&($5));
            }
            FREEEXP($3);
            FREEEXP($5);
         }
         | DILSE_ISA '(' dilexp ',' dilexp ')'
         {
            INITEXP($$);
	    $$.boolean=1;
            if ($3.typ != DILV_UP)
               fatal("Arg 1 of 'isaff' not a unit");
            else if ($5.typ != DILV_INT)
               fatal("Arg 2 of 'isaff' not integer");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_INT;
               make_code(&($3));
               make_code(&($5));
               add_ubit8(&($$),DILE_ISA);
               add_code(&($$),&($3));
               add_code(&($$),&($5));
            }
            FREEEXP($3);
            FREEEXP($5);
         }
         | DILSE_FNDU '(' dilexp ',' dilexp ',' dilexp ',' dilexp ')'
         {
            INITEXP($$);
	    checkbool("argument 3 of findunit",$7.boolean);
            if ($3.typ != DILV_UP)
               fatal("Arg 1 of 'findunit' not a unit");
            else if ($5.typ != DILV_SP)
               fatal("Arg 2 of 'findunit' not a string");
            else if ($7.typ != DILV_INT)
               fatal("Arg 3 of 'findunit' not integer");
            else if ($9.typ != DILV_UP &&
                $9.typ != DILV_NULL)
               fatal("Arg 4 of 'findunit' not unit");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_UP;
               make_code(&($3));
               make_code(&($5));
               make_code(&($7));
               make_code(&($9));
               add_ubit8(&($$),DILE_FNDU);
               add_code(&($$),&($3));
               add_code(&($$),&($5));
               add_code(&($$),&($7));
               add_code(&($$),&($9));
            }
            FREEEXP($3);
            FREEEXP($5);
            FREEEXP($7);
            FREEEXP($9);
         }
         | DILSE_FNDRU '(' dilexp ',' dilexp ',' dilexp ')'
         {
            INITEXP($$);

            if ($3.typ != DILV_UP)
               fatal("Arg 1 of 'findrndunit' not an unitptr.");
            else if ($5.typ != DILV_INT)
               fatal("Arg 2 of 'findrndunit' not an integer.");
            else if ($7.typ != DILV_INT)
               fatal("Arg 3 of 'findrndunit' not an integer");
            else
	    {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_UP;
               make_code(&($3));
               make_code(&($5));
               make_code(&($7));
               add_ubit8(&($$),DILE_FNDRU);
               add_code(&($$),&($3));
               add_code(&($$),&($5));
               add_code(&($$),&($7));
            }
            FREEEXP($3);
            FREEEXP($5);
            FREEEXP($7);
         }
         | DILSE_FNDR '(' dilexp ')'
         {   
            INITEXP($$);
            if ($3.typ != DILV_SP)
               fatal("Arg 1 of 'findroom' not string");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_UP;
               make_code(&($3));
               add_ubit8(&($$),DILE_FNDR);
               add_code(&($$),&($3));
            }
	    FREEEXP($3);
         }
         | DILSE_FNDS '(' dilexp ')'
         {   
            INITEXP($$);
            if ($3.typ != DILV_SP)
               fatal("Arg 1 of 'findsymbolic' not string");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_UP;
               make_code(&($3));
               add_ubit8(&($$),DILE_FNDS);
               add_code(&($$),&($3));
            }
            FREEEXP($3);
         }
         | DILSE_FNDS '(' dilexp ',' dilexp ',' dilexp ')'
         {   
            INITEXP($$);
            if ($3.typ != DILV_UP)
	      fatal("Arg 1 of 'findsymbolic' not a unitptr");
	    else if ($5.typ != DILV_SP)
	      fatal("Arg 2 of 'findsymbolic' not a string");
	    else if ($7.typ != DILV_INT)
	      fatal("Arg 3 of 'findsymbolic' not an integer");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_UP;
               make_code(&($3));
               make_code(&($5));
               make_code(&($7));
               add_ubit8(&($$),DILE_FNDS2);
               add_code(&($$),&($3));
               add_code(&($$),&($5));
               add_code(&($$),&($7));
            }
            FREEEXP($3);
            FREEEXP($5);
            FREEEXP($7);
         }
         | DILSE_VISI '(' dilexp ',' dilexp ')'
         {
            INITEXP($$);
	    $$.boolean=1;
            if ($3.typ != DILV_UP)
               fatal("Arg 1 of 'visible' not a unitptr");
            else if ($5.typ != DILV_UP)
               fatal("Arg 2 of 'visible' not a unitptr");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_INT;
               make_code(&($3));
               make_code(&($5));
               add_ubit8(&($$),DILE_VISI);
               add_code(&($$),&($3));
               add_code(&($$),&($5));
            }
            FREEEXP($3);
            FREEEXP($5);
	 }
         | DILSE_PCK '(' dilexp ',' dilexp ')'
         {
            INITEXP($$);
	    $$.boolean=1;
            if ($3.typ != DILV_UP)
               fatal("Arg 1 of 'paycheck' not a unitptr");
            else if ($5.typ != DILV_UP)
               fatal("Arg 2 of 'paycheck' not a unitptr");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_INT;
               make_code(&($3));
               make_code(&($5));
               add_ubit8(&($$),DILE_PCK);
               add_code(&($$),&($3));
               add_code(&($$),&($5));
            }
            FREEEXP($3);
            FREEEXP($5);
	 }
         | DILSE_OPPO '(' dilexp ',' dilexp ')'
         {
            INITEXP($$);
	    $$.boolean=1;
            if ($3.typ != DILV_UP)
               fatal("Arg 1 of 'opponent' not a unitptr");
            else if ($5.typ != DILV_UP)
               fatal("Arg 2 of 'opponent' not a unitptr");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_INT;
               make_code(&($3));
               make_code(&($5));
               add_ubit8(&($$), DILE_OPPO);
               add_code(&($$),&($3));
               add_code(&($$),&($5));
            }
            FREEEXP($3);
            FREEEXP($5);
	 }
         | DILSE_SPLX '(' dilexp ')' /* spellindex */
         {
            INITEXP($$);
            if ($3.typ != DILV_SP)
               fatal("Arg 1 of 'spellindex' not a string");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_INT;
               make_code(&($3));
               add_ubit8(&($$), DILE_SPLX);
               add_code(&($$),&($3));
            }
            FREEEXP($3);
         }
         | DILSE_SPLI '(' dilexp ',' dilexp ',' dilexp ',' dilexp ','
                          dilexp ',' dilexp ',' dilexp ',' dilexp')'
         {
            INITEXP($$);
	    $$.boolean=1;

            if ($3.typ != DILV_INT)
	      fatal("Arg 1 of 'spellinfo' not an integer");
            else if ($5.typ != DILV_INT || $5.dsl != DSL_LFT)
	      fatal("Arg 2 of 'spellinfo' not an integer variable");
            else if ($7.typ != DILV_INT || $7.dsl != DSL_LFT)
	      fatal("Arg 3 of 'spellinfo' not an integer variable");
            else if ($9.typ != DILV_INT || $9.dsl != DSL_LFT)
	      fatal("Arg 4 of 'spellinfo' not an integer variable");
            else if ($11.typ != DILV_INT || $11.dsl != DSL_LFT)
	      fatal("Arg 5 of 'spellinfo' not an integer variable");
            else if ($13.typ != DILV_INT || $13.dsl != DSL_LFT)
	      fatal("Arg 6 of 'spellinfo' not an integer variable");
            else if ($15.typ != DILV_INT || $15.dsl != DSL_LFT)
	      fatal("Arg 7 of 'spellinfo' not an integer variable");
            else if ($17.typ != DILV_INT || $17.dsl != DSL_LFT)
	      fatal("Arg 8 of 'spellinfo' not an integer variable");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_SP;

               make_code(&($3));
               make_code(&($5));
               make_code(&($7));
               make_code(&($9));
               make_code(&($11));
               make_code(&($13));
               make_code(&($15));
               make_code(&($17));

               add_ubit8(&($$), DILE_SPLI);

               add_code(&($$),&($3));
               add_code(&($$),&($5));
               add_code(&($$),&($7));
               add_code(&($$),&($9));
               add_code(&($$),&($11));
               add_code(&($$),&($13));
               add_code(&($$),&($15));
               add_code(&($$),&($17));
            }
            FREEEXP($3);
            FREEEXP($5);
            FREEEXP($7);
            FREEEXP($9);
            FREEEXP($11);
            FREEEXP($13);
            FREEEXP($15);
            FREEEXP($17);
         }
         | DILSE_MONS '(' dilexp ',' dilexp ')' /* moneystring */
         {
            INITEXP($$);
            if ($3.typ != DILV_INT)
               fatal("Arg 1 of 'money_string' not an integer");
            else if ($5.typ != DILV_INT)
               fatal("Arg 2 of 'money_string' not a boolean");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_SP;
               make_code(&($3));
               make_code(&($5));
               add_ubit8(&($$), DILE_MONS);
               add_code(&($$),&($3));
               add_code(&($$),&($5));
            }
            FREEEXP($3);
            FREEEXP($5);
	 }
         | DILSE_PATH '(' dilexp ',' dilexp ')'
         {
            INITEXP($$);
            if ($3.typ != DILV_UP)
               fatal("Arg 1 of 'pathto' not a unitptr");
            else if ($5.typ != DILV_UP)
               fatal("Arg 2 of 'pathto' not a unitptr");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_INT;
               make_code(&($3));
               make_code(&($5));
               add_ubit8(&($$), DILE_PATH);
               add_code(&($$),&($3));
               add_code(&($$),&($5));
            }
            FREEEXP($3);
            FREEEXP($5);
	 }
         | DILSE_CARY '(' dilexp ',' dilexp ',' dilexp ')'
         {
            INITEXP($$);

            if ($3.typ != DILV_UP)
               fatal("Arg 1 of 'can_carry' not a unitptr");
            else if ($5.typ != DILV_UP)
               fatal("Arg 2 of 'can_carry' not a unitptr");
            else if ($7.typ != DILV_INT)
 	       fatal("Arg 3 of 'can_carry' not an integer");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_INT;
               make_code(&($3));
               make_code(&($5));
               make_code(&($7));
               add_ubit8(&($$), DILE_CARY);
               add_code(&($$),&($3));
               add_code(&($$),&($5));
               add_code(&($$),&($7));
            }
            FREEEXP($3);
            FREEEXP($5);
	 }
         | DILSE_PURS '(' dilexp ',' dilexp ')'
         {
            INITEXP($$);

            if ($3.typ != DILV_UP)
               fatal("Arg 1 of 'purse' not a unitptr");
            else if ($5.typ != DILV_SP)
               fatal("Arg 2 of 'purse' not a string");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_INT;
               make_code(&($3));
               make_code(&($5));
               add_ubit8(&($$), DILE_PURS);
               add_code(&($$),&($3));
               add_code(&($$),&($5));
            }
            FREEEXP($3);
            FREEEXP($5);
	 }
         | DILSE_TRMO '(' dilexp ',' dilexp ',' dilexp ')'
         {
            INITEXP($$);

            if (($3.typ != DILV_UP) && ($3.typ != DILV_NULL))
               fatal("Arg 1 of 'transfermoney' not a unitptr");
            else if (($5.typ != DILV_UP) && ($5.typ != DILV_NULL))
               fatal("Arg 2 of 'transfermoney' not a unitptr");
            else if ($7.typ != DILV_INT)
               fatal("Arg 3 of 'transfermoney' not an integer");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_INT;
               make_code(&($3));
               make_code(&($5));
               make_code(&($7));
               add_ubit8(&($$), DILE_TRMO);
               add_code(&($$),&($3));
               add_code(&($$),&($5));
               add_code(&($$),&($7));
            }
            FREEEXP($3);
            FREEEXP($5);
            FREEEXP($7);
	 }
         | DILSE_LOAD '(' dilexp ')'
         {
            INITEXP($$);
            if ($3.typ != DILV_SP)
               fatal("Arg 1 of 'load' not name string");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_UP;
               make_code(&($3));
               add_ubit8(&($$),DILE_LOAD);
               add_code(&($$),&($3));
            }
            FREEEXP($3);
         }
         | DILSE_ATSP '(' dilexp ',' dilexp ',' dilexp ','
                          dilexp ',' dilexp ')'
         {
            INITEXP($$);
	    checkbool("argument 1 of attack_spell", $3.boolean);

            if ($3.typ != DILV_INT)
               fatal("Arg 1 of 'attack_spell' not a number");
            else if ($5.typ != DILV_UP)
               fatal("Arg 2 of 'attack_spell' not a unitptr");
            else if ($7.typ != DILV_UP)
               fatal("Arg 3 of 'attack_spell' not a unitptr");
            else if ($9.typ != DILV_UP)
               fatal("Arg 4 of 'attack_spell' not a unitptr");
            else if ($11.typ != DILV_INT)
               fatal("Arg 5 of 'attack_spell' not an integer");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_INT;

               make_code(&($3));
               make_code(&($5));
               make_code(&($7));
               make_code(&($9));
               make_code(&($11));

               add_ubit8(&($$), DILE_ATSP);

               add_code(&($$),&($3));
               add_code(&($$),&($5));
               add_code(&($$),&($7));
               add_code(&($$),&($9));
               add_code(&($$),&($11));
            }

            FREEEXP($3);
            FREEEXP($5);
            FREEEXP($7);
            FREEEXP($9);
            FREEEXP($11);
	 }
         | DILSE_CST '(' dilexp ',' dilexp ',' dilexp ','
                         dilexp ',' dilexp ')'
         {
            INITEXP($$);
	    checkbool("argument 1 of cast",$3.boolean);

            if ($3.typ != DILV_INT)
               fatal("Arg 1 of 'cast_spell' not a number");
            else if ($5.typ != DILV_UP)
               fatal("Arg 2 of 'cast_spell' not a unitptr");
            else if ($7.typ != DILV_UP)
               fatal("Arg 3 of 'cast_spell' not a unitptr");
            else if ($9.typ != DILV_UP)
               fatal("Arg 4 of 'cast_spell' not a unitptr");
            else if ($11.typ != DILV_SP)
               fatal("Arg 5 of 'cast_spell' not a string");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_INT;

               make_code(&($3));
               make_code(&($5));
               make_code(&($7));
               make_code(&($9));
               make_code(&($11));

               add_ubit8(&($$), DILE_CAST2);

               add_code(&($$),&($3));
               add_code(&($$),&($5));
               add_code(&($$),&($7));
               add_code(&($$),&($9));
               add_code(&($$),&($11));
            }

            FREEEXP($3);
            FREEEXP($5);
            FREEEXP($7);
            FREEEXP($9);
            FREEEXP($11);
	 }
         | DILSE_FIT '(' dilexp ',' dilexp ',' dilexp ')'
         {
            INITEXP($$);

            if ($3.typ != DILV_UP)
	      fatal("Arg 1 of 'fits' not an unitptr.");
            else if ($5.typ != DILV_UP)
	      fatal("Arg 2 of 'fits' not an unitptr.");
            else if ($7.typ != DILV_INT)
	      fatal("Arg 3 of 'fits' not an integer.");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_SP;

               make_code(&($3));
               make_code(&($5));
               make_code(&($7));

               add_ubit8(&($$), DILE_FIT);

               add_code(&($$),&($3));
               add_code(&($$),&($5));
               add_code(&($$),&($7));
            }

            FREEEXP($3);
            FREEEXP($5);
            FREEEXP($7);
         }
         | DILSE_REST '(' dilexp ',' dilexp ')'
         {
            INITEXP($$);

            if ($3.typ != DILV_SP)
	      fatal("Arg 1 of 'restore' not a string");
            else if ($5.typ != DILV_SP)
	      fatal("Arg 2 of 'restore' not a string");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_UP;

               make_code(&($3));
               make_code(&($5));

               add_ubit8(&($$), DILE_REST);

               add_code(&($$),&($3));
               add_code(&($$),&($5));
            }

            FREEEXP($3);
            FREEEXP($5);
	 }
         | DILSE_OPRO '(' dilexp ',' dilexp ')'
         {
            INITEXP($$);

            if ($3.typ != DILV_INT)
	      fatal("Arg 1 of 'openroll' not an integer");
            else if ($5.typ != DILV_INT)
	      fatal("Arg 2 of 'openroll' not an integer");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_INT;

               make_code(&($3));
               make_code(&($5));

               add_ubit8(&($$), DILE_OPRO);

               add_code(&($$),&($3));
               add_code(&($$),&($5));
            }

            FREEEXP($3);
            FREEEXP($5);
	 }
         | DILSE_EQPM '(' dilexp ',' dilexp ')'
         {
            INITEXP($$);

            if ($3.typ != DILV_UP)
	      fatal("Arg 1 of 'equipment' not a unitptr");
            else if ($5.typ != DILV_INT)
	      fatal("Arg 2 of 'equipment' not a unitptr");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_UP;

               make_code(&($3));
               make_code(&($5));

               add_ubit8(&($$), DILE_EQPM);

               add_code(&($$),&($3));
               add_code(&($$),&($5));
            }

            FREEEXP($3);
            FREEEXP($5);
	 }
         | DILSE_MEL '(' dilexp ',' dilexp ',' dilexp ',' dilexp ')'
         {
            INITEXP($$);
	    checkbool("argument 1 of MeleeAttack",$3.boolean);

            if ($3.typ != DILV_UP)
	      fatal("Arg 1 of 'MeleeAttack' not a unitptr");
            else if ($5.typ != DILV_UP)
	      fatal("Arg 2 of 'MeleeAttack' not a unitptr");
            else if ($7.typ != DILV_INT)
	      fatal("Arg 3 of 'MeleeAttack' not an integer");	      
            else if ($9.typ != DILV_INT)
	      fatal("Arg 4 of 'MeleeAttack' not an integer");	      
            else
	    {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_INT;

               make_code(&($3));
               make_code(&($5));
               make_code(&($7));
               make_code(&($9));

               add_ubit8(&($$), DILE_MEL);

               add_code(&($$),&($3));
               add_code(&($$),&($5));
               add_code(&($$),&($7));
               add_code(&($$),&($9));
            }

            FREEEXP($3);
            FREEEXP($5);
            FREEEXP($7);
            FREEEXP($9);
	 }
         | DILSE_TXF '(' dilexp ')'
         {
            INITEXP($$);
            if ($3.typ != DILV_SP)
               fatal("Arg 1 of 'textformat' not string variable");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_SP;
               add_ubit8(&($$),DILE_TXF);
               add_code(&($$),&($3));
            }
            FREEEXP($3);
         }
         | DILSE_AST '(' dilexp ')'
         {
            INITEXP($$);
            if ($3.typ != DILV_INT)
               fatal("Arg 1 of 'asctime' not integer");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_SP;
               add_ubit8(&($$),DILE_AST);
               add_code(&($$),&($3));
            }
            FREEEXP($3);
         }
         | DILSE_GETW '(' dilexp ')'
         {
            INITEXP($$);
            if ($3.typ != DILV_SP)
               fatal("Arg 1 of 'getword' not string variable");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_SP;
               add_ubit8(&($$),DILE_GETW);
               add_code(&($$),&($3));
            }
            FREEEXP($3);
         }
         | DILSE_GETWS '(' dilexp ')'
         {
            INITEXP($$);
            if ($3.typ != DILV_SP)
               fatal("Arg 1 of 'getwords' not string");
            else {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_SLP;
               add_ubit8(&($$),DILE_GETWS);
               add_code(&($$),&($3));
            }
            FREEEXP($3);
         }
         | DILSE_CMDS '(' dilexp ')'
         {
            INITEXP($$);
	    $$.boolean=1;
            if ($3.typ == DILV_SP)
            {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_INT;
               make_code(&($3));
               add_ubit8(&($$),DILE_CMDS);
               add_code(&($$),&($3));
            } else if ($3.typ == DILV_INT) {
               /* Type is ok */
               /* Make nodes dynamic */
               $$.dsl = DSL_DYN;
               $$.typ = DILV_INT;
               make_code(&($3));
               add_ubit8(&($$),DILE_CMDS);
               add_code(&($$),&($3));
            } else
                fatal("Arg 1 of 'command' not a string or an integer");
            FREEEXP($3);
         }
         ;

stringlist : '{' strings '}'
         {
            int i;

            CREATE($$, char *, (str_top + 2));
            for (i = 0; i <= str_top; i++)
               $$[i] = str_list[i];
            $$[i] = 0;
         }
         ;
         
strings   : STRING
         {
            str_list[str_top = 0] = $1;
         }
         | strings ',' STRING
         {
            str_list[++str_top] = $3;
	    if (str_top > sizeof(str_list) - 3)
	      fatal("Static stringlist has too many elements.");
         }
         ;

coreexp   : dilexp
/*
   This rule makes code for the expression in the core
   instead of the stack element.
*/
         {
            int i;

/*          fprintf(stderr,"COREEXP\n");*/
	    if (($1.typ==DILV_PROC)||($1.typ==DILV_FUNC))
	      fatal("Illegal use of proc/func");

            make_code(&($1));
	    $$.boolean=$1.boolean;
            /* write dynamic expression in core */
            $$.fst = wcore - tmpl.core;
            moredilcore($1.codep - $1.code);
            for(i=0;i<$1.codep-$1.code;i++,wcore++)
               *wcore=$1.code[i];
            $$.lst = wcore - tmpl.core;
            $$.dsl = $1.dsl;
            $$.typ = $1.typ;
            FREEEXP($1);
         }
         ; 

corevar   : variable
/*
   This rule makes code for the variable or func/proc in the core
   instead of the stack element!
*/
         {
            int i;
/*          fprintf(stderr,"COREVAR\n");*/
	    if (($1.dsl != DSL_LFT)&&
		($1.dsl != DSL_FCT)&&
		($1.dsl != DSL_SFT))
	      fatal("Illegal lvalue");
            make_code(&($1)); /* does nothing!? */
	    $$.boolean=$1.boolean;
            /* write dynamic expression in core */
            $$.fst = wcore - tmpl.core;
            moredilcore($1.codep-$1.code);
            for(i=0;i<$1.codep-$1.code;i++,wcore++)
               *wcore=$1.code[i];
            $$.lst = wcore - tmpl.core;
	    /* fprintf(stderr, "Refnum supposedly : %d, %d == %d, %d\n",
		    $1.code[0], $1.code[1], *(wcore-2), *(wcore-1));
	    fprintf(stderr, "Corevar : fst %p, lst %p.\n",
		    $$.fst, $$.lst);*/
            $$.dsl = $1.dsl;
            $$.typ = $1.typ;
            FREEEXP($1);
         }
         ; 
         
/* ************************************************************* */
/*                      DIL procedures                               */
/* ************************************************************* */

ihold      : /* instruction core placeholder */
         {
            $$ = wcore - tmpl.core;
            wcore++;      /* ubit8 */
         }
         ;

iunhold  :
	 {
            wcore--;      /* ubit8 */
            $$ = wcore - tmpl.core;
	 }    

ahold    : /* address core placeholder */
         {
            $$ = wcore - tmpl.core;
            wcore+=4;   /* ubit32 */
         }
         ;

label    : SYMBOL
         {
            /* lable reference */
            $$.fst = wcore - tmpl.core;
            moredilcore(4);
            bwrite_ubit32(&wcore,get_label($1,wcore-tmpl.core)); /* here */
            $$.lst = wcore - tmpl.core;
         }
         ;
         
labelskip : label
         {
            $$ = $1;
         }
         | DILSE_SKIP
         {
            /* 'skip' reference */
            $$.fst = wcore - tmpl.core;
            moredilcore(4);
            bwrite_ubit32(&wcore,SKIP);  /* address null value */
            $$.lst = wcore - tmpl.core;
         }
         ;

labellist : labelskip ',' labellist
         {
           $$.typ=$3.typ+1;
	   $$.fst=$1.fst;
	   $$.lst=$3.lst;
         }
         | labelskip
         {
           $$.typ=1;
	   $$.fst=$1.fst;
	   $$.lst=$1.lst;
         }
         ;

dilproc  : ihold proccall
         {
            char nbuf[255],zbuf[255],buf[255];
            int i;

            /* ignore returning variable ... */
	    if ($2.typ == DILV_SP) {

	       /*
		* Write remote symbolic procedure call
		*
		* <rspcall> <var exp> <nargs> <arg> <arg> ...
		*/

	       wtmp = &tmpl.core[$1];
	       bwrite_ubit8(&wtmp,DILI_SPC);
	       $$.fst = $1;
	       $$.lst = wcore - tmpl.core;
	    } else {
	       /*
		* Write remote normal procedure call
		* 
		* <rpcall> <funcnumber> <arg> <arg> ...
		*/

	       if (($2.dsl != DSL_FCT) ||
		   ($2.typ != DILV_NULL))
		 fatal ("refernce not a procedure");

	       wtmp = &tmpl.core[$1];
	       bwrite_ubit8(&wtmp,DILI_RPC);

	       $$.fst = $1;
	       $$.lst = wcore - tmpl.core;
	    }
         }
         | DILSI_CLI ihold '(' coreexp ')'
         {
            if ($4.typ != DILV_INT)
               fatal("Arg 1 of 'clear' not an integer");
            else {
               $$.fst = $2;
               $$.lst = $4.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_CLI);
            }
         }
         | DILSI_STOR ihold '(' coreexp ')'
         {
            if ($4.typ != DILV_UP)
               fatal("Arg 1 of 'store' not an unitptr");
            else {
               $$.fst = $2;
               $$.lst = $4.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_STOR);
            }
         }
         | DILSI_AMOD ihold '(' coreexp ',' coreexp ')'
         {
            if ($4.typ != DILV_UP)
               fatal("Arg 1 of 'acc_modify' not an unitptr");
            else if ($6.typ != DILV_INT)
               fatal("Arg 2 of 'acc_modify' not an integer");
            else {
               $$.fst = $2;
               $$.lst = $6.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_AMOD);
            }
         }
         | DILSI_FOLO ihold '(' coreexp ',' coreexp ')'
         {
            if ($4.typ != DILV_UP)
               fatal("Arg 1 of 'follow' not an unitptr");
            else if ($6.typ != DILV_UP)
               fatal("Arg 2 of 'follow' not an unitptr");
            else {
               $$.fst = $2;
               $$.lst = $6.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_FOLO);
            }
         }
         | DILSI_SETF ihold '(' coreexp ',' coreexp ')'
         {
            if ($4.typ != DILV_UP)
               fatal("Arg 1 of 'set_fighting' not an unitptr");
            else if ($6.typ != DILV_UP)
               fatal("Arg 2 of 'set_fighting' not an unitptr");
            else {
               $$.fst = $2;
               $$.lst = $6.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_SETF);
            }
         }
         | DILSI_CHAS ihold '(' coreexp ',' coreexp ')'
         {
	    checkbool("argument 2 of change_speed",$6.boolean);
            if ($4.typ != DILV_UP)
               fatal("Arg 1 of 'change_speed' not an unitptr");
            else if ($6.typ != DILV_INT)
               fatal("Arg 2 of 'change_speed' not an integer");
            else {
               $$.fst = $2;
               $$.lst = $6.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_CHAS);
            }
         }
         | DILSI_SBT ihold '(' coreexp ',' coreexp ')'
         {
	    checkbool("argument 2 of setbright",$6.boolean);
            if ($4.typ != DILV_UP)
               fatal("Arg 1 of 'setbright' not an unitptr");
            else if ($6.typ != DILV_INT)
               fatal("Arg 2 of 'setbright' not an integer");
            else {
               $$.fst = $2;
               $$.lst = $6.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_SBT);
            }
         }
         | DILSI_SWT ihold '(' coreexp ',' coreexp ')'
         {
	    checkbool("argument 2 of setweight",$6.boolean);
            if ($4.typ != DILV_UP)
               fatal("Arg 1 of 'setweight' not an unitptr");
            else if ($6.typ != DILV_INT)
               fatal("Arg 2 of 'setweight' not an integer");
            else {
               $$.fst = $2;
               $$.lst = $6.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_SWT);
            }
         }
         | DILSI_SET ihold '(' coreexp ',' coreexp ')'
         {
	    checkbool("argument 2 of set",$6.boolean);
            if ($4.typ != DILV_INT || $4.dsl != DSL_LFT)
               fatal("Arg 1 of 'set' not an integer variable");
            else if ($6.typ != DILV_INT)
               fatal("Arg 2 of 'set' not an integer");
            else {
               $$.fst = $2;
               $$.lst = $6.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_SET);
            }
         }
         | DILSI_DLC ihold '(' coreexp ',' coreexp ')'
         {
            if ($4.typ != DILV_SP)
               fatal("Arg 1 of 'dilcopy' not a string");
            else if ($6.typ != DILV_UP)
               fatal("Arg 2 of 'dilcopy' not a unitptr");
            else
	    {
               $$.fst = $2;
               $$.lst = $6.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_DLC);
            }
         }
         | DILSI_SETE ihold '(' coreexp ',' coreexp ')'
         {
            if ($4.typ != DILV_SP)
               fatal("Arg 1 of 'sendtext' not a string");
            else if ($6.typ != DILV_UP)
               fatal("Arg 2 of 'sendtext' not a unitptr");
            else
	    {
               $$.fst = $2;
               $$.lst = $6.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_SETE);
            }
         }
         | DILSI_UST ihold '(' coreexp ',' coreexp ')'
         {
	    checkbool("argument 2 of unset",$6.boolean);
            if ($4.typ != DILV_INT || $4.dsl != DSL_LFT)
               fatal("Arg 1 of 'unset' not an integer variable");
            else if ($6.typ != DILV_INT)
               fatal("Arg 2 of 'unset' not an integer");
            else {
               $$.fst = $2;
               $$.lst = $6.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_USET);
            }
         }
         | DILSI_ADE ihold '(' coreexp ',' coreexp ',' coreexp ')'
         {
	    if (($4.typ != DILV_EDP) || ($4.dsl != DSL_DYN))
               fatal("Arg 1 of 'addextra' not a unit extra description");
            else if ($6.typ != DILV_SLP)
               fatal("Arg 2 of 'addextra' not a stringlist");
            else if ($8.typ != DILV_SP)
               fatal("Arg 3 of 'addextra' not a string");
            else {
               $$.fst = $2;
               $$.lst = $8.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_ADE);
            }
         }
         | DILSI_LCRI ihold '(' coreexp ',' coreexp ',' coreexp ')'
         {
            if ($4.typ != DILV_UP)
               fatal("Arg 1 of 'logcrime' not an unitptr");
            else if ($6.typ != DILV_UP)
               fatal("Arg 2 of 'logcrime' not an unitptr");
            else if ($8.typ != DILV_INT)
               fatal("Arg 3 of 'logcrime' not an integer");
            else {
               $$.fst = $2;
               $$.lst = $8.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_LCRI);
            }
         }
         | DILSE_CST ihold '(' coreexp ',' coreexp ',' coreexp ',' coreexp ')'
         {
	    checkbool("argument 1 of cast",$4.boolean);
            if ($4.typ != DILV_INT)
               fatal("Arg 1 of 'cast_spell' not a number");
            else if ($6.typ != DILV_UP)
               fatal("Arg 2 of 'cast_spell' not a unitptr");
            else if ($8.typ != DILV_UP)
               fatal("Arg 3 of 'cast_spell' not a unitptr");
            else if ($10.typ != DILV_UP)
               fatal("Arg 4 of 'cast_spell' not a unitptr");
            else {
               $$.fst = $2;
               $$.lst = $10.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_CAST);
            }
         }
         | DILSI_ADL ihold '(' coreexp ',' coreexp ')'
         {
            if (($4.typ != DILV_SLP) || ($4.dsl != DSL_LFT))
               fatal("Arg 1 of 'addlist' not a stringlist variable");
            else if ($6.typ != DILV_SP)
               fatal("Arg 2 of 'addlist' not a string");
            else {
               $$.fst = $2;
               $$.lst = $6.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_ADL);
            }
         }
         | DILSI_SUE ihold '(' coreexp ',' coreexp ')'
         {
	   /*fprintf(stderr, "SUE DSL %d TYP %d\n", $4.dsl, $4.typ);*/

	    if (($4.typ != DILV_EDP) || ($4.dsl != DSL_DYN))
               fatal("Arg 1 of 'subextra' not a unit extra description");
            else if ($6.typ != DILV_SP)
               fatal("Arg 2 of 'subextra' not a string");
            else {
               $$.fst = $2;
               $$.lst = $6.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_SUE);
            }
         }
         | DILSI_SUL ihold '(' coreexp ',' coreexp ')'
         {
            if ($4.typ != DILV_SLP || $4.dsl != DSL_LFT)
               fatal("Arg 1 of 'sublist' not a stringlist variable");
            else if ($6.typ != DILV_SP)
               fatal("Arg 2 of 'sublist' not a string");
            else {
               $$.fst = $2;
               $$.lst = $6.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_SUL);
            }
         }
         | DILSI_SUA ihold '(' coreexp ',' coreexp ')'
         {
	    checkbool("argument 2 of subaff",$6.boolean);
            if ($4.typ != DILV_UP)
               fatal("Arg 1 of 'subaff' not a unit");
            else if ($6.typ != DILV_INT)
               fatal("Arg 2 of 'subaff' not an integer");
            else {
               $$.fst = $2;
               $$.lst = $6.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_SUA);
            }
         }
         | DILSI_ADA ihold '(' coreexp ',' coreexp ',' coreexp ','
                               coreexp ',' coreexp ',' coreexp ','
                               coreexp ',' coreexp ',' coreexp ','
                               coreexp ',' coreexp ')'
         {
            if ($4.typ != DILV_UP)
               fatal("Arg 1 of 'addaff' not a unit");
            else if ($6.typ != DILV_INT)
               fatal("Arg 2 of 'addaff' not an integer");
            else if ($8.typ != DILV_INT)
               fatal("Arg 3 of 'addaff' not an integer");
            else if ($10.typ != DILV_INT)
               fatal("Arg 4 of 'addaff' not an integer");
            else if ($12.typ != DILV_INT)
               fatal("Arg 5 of 'addaff' not an integer");
            else if ($14.typ != DILV_INT)
               fatal("Arg 6 of 'addaff' not an integer");
            else if ($16.typ != DILV_INT)
               fatal("Arg 7 of 'addaff' not an integer");
            else if ($18.typ != DILV_INT)
               fatal("Arg 8 of 'addaff' not an integer");
            else if ($20.typ != DILV_INT)
               fatal("Arg 9 of 'addaff' not an integer");
            else if ($22.typ != DILV_INT)
               fatal("Arg 10 of 'addaff' not an integer");
            else if ($24.typ != DILV_INT)
               fatal("Arg 11 of 'addaff' not an integer");
            else   {
               $$.fst = $2;
               $$.lst = $24.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_ADA);
            }
         }
         | DILSI_DST ihold '(' coreexp ')'
         {
            if ($4.typ != DILV_UP)
               fatal("Arg 1 of 'destruct' not a unit");
            else {
               $$.fst = $2;
               $$.lst = $4.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_DST);
            }
         }
         | DILSI_LOG ihold '(' coreexp ')'
         {
            if ($4.typ != DILV_SP)
               fatal("Arg 1 of 'log' not a string.");
            else {
               $$.fst = $2;
               $$.lst = $4.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_LOG);
            }
         }
         | DILSI_PUP ihold '(' coreexp ')'
         {
            if ($4.typ != DILV_UP)
               fatal("Arg 1 of 'position_update' not a unit");
            else {
               $$.fst = $2;
               $$.lst = $4.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_PUP);
            }
         }
         | DILSI_LNK ihold '(' coreexp ',' coreexp ')'
         {
            if ($4.typ != DILV_UP)
               fatal("Arg 1 of 'link' not a unit");
            else if ($6.typ != DILV_UP)
               fatal("Arg 2 of 'link' not a unit");
            else {
               $$.fst = $2;
               $$.lst = $6.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_LNK);
            }
         }
         | DILSI_EXP ihold '(' coreexp ',' coreexp ')'
         {
	    checkbool("argument 1 of experience",$4.boolean);
            if ($4.typ != DILV_INT)
               fatal("Arg 1 of 'experience' not an integer");
            else if ($6.typ != DILV_UP)
               fatal("Arg 2 of 'experience' not a unitptr");
            else {
               $$.fst = $2;
               $$.lst = $6.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_EXP);
            }
         }
         | DILSI_ACT ihold '(' coreexp ',' coreexp ',' coreexp ',' coreexp ',' coreexp ',' coreexp ')'
         {
	    checkbool("argument 2 of act",$6.boolean);
	    checkbool("argument 6 of act",$14.boolean);
            if ($4.typ != DILV_SP)
               fatal("Arg 1 of 'act' not a string");
            else if ($6.typ != DILV_INT)
               fatal("Arg 2 of 'act' not an integer");
            else if ($8.typ != DILV_NULL &&
                $8.typ != DILV_SP &&
                $8.typ != DILV_UP)
               fatal("Arg 3 of 'act' not a unit or string");
            else if ($10.typ != DILV_NULL &&
                $10.typ != DILV_SP &&
                $10.typ != DILV_UP)
               fatal("Arg 4 of 'act' not a unit or string");
            else if ($12.typ != DILV_NULL &&
                $12.typ != DILV_SP &&
                $12.typ != DILV_UP)
               fatal("Arg 5 of 'act' not a unit or string");
            else if ($14.typ != DILV_INT)
               fatal("Arg 6 of 'act' not an integer");
            else {
               /* make code */
               $$.fst = $2;
               $$.lst = $14.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_ACT);
            }
         }
         | DILSI_EXE ihold '(' coreexp ',' coreexp ')'
         {
            if ($4.typ != DILV_SP)
               fatal("Arg 1 of 'exec' not a string");
            else if ($6.typ != DILV_UP)
               fatal("Arg 2 of 'exec' not a unit");
            else {
               $$.fst = $2;
               $$.lst = $6.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_EXEC);
            }
         }
         | DILSI_WIT ihold '(' coreexp ',' coreexp ')'
         {
	    checkbool("argument 1 of wait",$4.boolean);
            if ($4.typ != DILV_INT)
               fatal("Arg 1 of 'wait' not an integer");
            else {
               $$.fst = $2;
               $$.lst = $6.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_WIT);
            }
         }
         | DILSI_SND ihold '(' coreexp ')'
         {
            if ($4.typ != DILV_SP)
               fatal("Arg 1 of 'send' not a string");
            else {
               $$.fst = $2;
               $$.lst = $4.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_SND);
            }
         }
         | DILSI_SNT ihold '(' coreexp ',' coreexp ')'
         {
            if ($4.typ != DILV_SP)
               fatal("Arg 1 of 'sendto' not a string");
            else if ($6.typ != DILV_UP)
               fatal("Arg 2 of 'sendto' not a unit");
            else {
               $$.fst = $2;
               $$.lst = $6.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_SNT);
            }
         }
         | DILSI_SNTA ihold '(' coreexp ',' coreexp ')'
         {
            if ($4.typ != DILV_SP)
               fatal("Arg 1 of 'sendtoall' not a string");
            else if ($6.typ != DILV_SP)
               fatal("Arg 2 of 'sendtoall' not a string");
            else {
               $$.fst = $2;
               $$.lst = $6.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_SNTA);
            }
         }
         | DILSI_SNTADIL ihold '(' coreexp ',' coreexp ')'
         {
            if ($4.typ != DILV_SP)
               fatal("Arg 1 of 'sendtoalldil' not a string");
            else if ($6.typ != DILV_SP)
               fatal("Arg 2 of 'sendtoalldil' not a string");
            else {
               $$.fst = $2;
               $$.lst = $6.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_SNTADIL);
            }
         }
         | DILSI_SEC ihold '(' coreexp ',' label ')'
         {
            if ($4.dsl != DSL_LFT)
               fatal("Arg 1 of 'secure' not a user-variable.");
            else if ($4.typ != DILV_UP)
               fatal("Arg 1 of 'secure' not a unit");
            else {
               $$.fst = $2;
               $$.lst = $6.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_SEC);
            }
         }
         | DILSI_USE ihold '(' coreexp ')'
         {
            if ($4.typ != DILV_UP)
               fatal("Arg 1 of 'unsecure' not a unit");
            else { 
               $$.fst = $2;
               $$.lst = $4.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_USE);
            }
         }
         | DILSI_QUIT ihold
         {
            $$.fst = $2;
            $$.lst = $2+1;
            wtmp = &tmpl.core[$2];
            bwrite_ubit8(&wtmp,DILI_QUIT);
         }
         | DILSI_BLK ihold
         {
            $$.fst = $2;
            $$.lst = $2+1;
            wtmp = &tmpl.core[$2];
            bwrite_ubit8(&wtmp,DILI_BLK);
         }
         | DILSI_PRI ihold
         {
            $$.fst = $2;
            $$.lst = $2+1;
            wtmp = &tmpl.core[$2];
            bwrite_ubit8(&wtmp,DILI_PRI);
         }
         | DILSI_NPR ihold
         {
            $$.fst = $2;
            $$.lst = $2+1;
            wtmp = &tmpl.core[$2];
            bwrite_ubit8(&wtmp,DILI_NPR);
         }
         | DILSI_WLK ihold '(' coreexp ')'
         {
            if ($4.typ != DILV_UP)
               fatal("Arg 1 of 'walkto' not a unit");
            else {
               $$.fst = $2;
               $$.lst = $4.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_WALK);
            }
         }
         | DILSI_EQP ihold '(' coreexp ',' coreexp ')'
         {
	    checkbool("argument 1 of addequip",$4.boolean);
            if ($4.typ != DILV_UP)
               fatal("Arg 1 of 'addequip' not a unit");
            else if ($6.typ != DILV_INT)
               fatal("Arg 2 of 'addequip' not an integer");
            else {
               $$.fst = $2;
               $$.lst = $6.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_EQP);
            }
         }
         | DILSI_UEQ ihold '(' coreexp ')'
         {
            if ($4.typ != DILV_UP)
               fatal("Arg 1 of 'unequip' not a unit");
            else {
               $$.fst = $2;
               $$.lst = $4.lst;
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_UEQ);
            }
         }
         ;

/* ************************************************************* */
/* DIL instructions */
/* ************************************************************* */

pushbrk  : /* naught */
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
			 
defbrk   : /* naught */
         {
             /*
              * Defines address of break label on top of stack.
              */
             if (!break_no)
                fatal("INTERNAL ERROR (break stack def)");
             else
                label_adr[break_idx[break_no-1]] = wcore-tmpl.core;
         }
			
popbrk   : /* naught */
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

pushcnt  : /* naught */
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
			 
defcnt   : /* naught */
         {
            /*
             * Defines address of cont label on top of stack.
             */
            if (!cont_no)
               fatal("INTERNAL ERROR (cont stack def)");
            else
               label_adr[cont_idx[cont_no-1]] = wcore-tmpl.core;
         }
			
popcnt   : /* naught */
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

semicolons   : ';'
         | ';' semicolons 
         ;

optsemicolons   :
         | semicolons
         ;

block    : '{' dilinstlist '}'
         { $$ = $2; }
         ;

dilinstlist   : dilcomposed
         { $$ = $1; }
         | dilcomposed dilinstlist
         { $$.fst = $1.fst; $$.lst = $2.lst; }
         ;
         
dilcomposed   : '{' dilinstlist '}' optsemicolons
         { $$ = $2; }
         | dilinst semicolons
         { $$ = $1; }
         | dilcomplex
         { $$ = $1; }
         ;

dilcomplex : DILSI_IF ihold '(' coreexp ')' ahold block ihold ahold DILSI_ELS dilcomposed
         {
            wtmp = &tmpl.core[$2];
            bwrite_ubit8(&wtmp,DILI_IF); /* the instruction */
            wtmp = &tmpl.core[$6];
            bwrite_ubit32(&wtmp,$11.fst); /* address of else */
            wtmp = &tmpl.core[$8];
            bwrite_ubit8(&wtmp,DILI_GOTO); /* skip else */
            wtmp = &tmpl.core[$9];
            bwrite_ubit32(&wtmp,$11.lst); /* end of else */
            $$.fst = $2;
            $$.lst = $11.lst;
         }
         | DILSI_IF ihold '(' coreexp ')' ahold dilinst optsemicolons ihold ahold DILSI_ELS dilcomposed
         {
            wtmp = &tmpl.core[$2];
            bwrite_ubit8(&wtmp,DILI_IF); /* the instruction */
            wtmp = &tmpl.core[$6];
            bwrite_ubit32(&wtmp,$12.fst); /* address of else */
            wtmp = &tmpl.core[$9];
            bwrite_ubit8(&wtmp,DILI_GOTO); /* skip else */
            wtmp = &tmpl.core[$10];
            bwrite_ubit32(&wtmp,$12.lst); /* end of else */
            $$.fst = $2;
            $$.lst = $12.lst;
         }
         | DILSI_IF ihold '(' coreexp ')' ahold dilcomposed
         {
            wtmp = &tmpl.core[$2];
            bwrite_ubit8(&wtmp,DILI_IF); /* the instruction */
            wtmp = &tmpl.core[$6];
            bwrite_ubit32(&wtmp,$7.lst); /* address of else */
            $$.fst = $2;
            $$.lst = $7.lst;
         }
         | DILSI_FOE ihold '(' coreexp ','
           pushbrk pushcnt defcnt ihold corevar ahold ')'
           { in_foreach++;
            if (in_foreach>1)
	      fatal("'foreach' may not be nested!");	      
	   }
           dilcomposed 
           { in_foreach--; }
           ihold ahold defbrk popbrk popcnt
         {
            /*
             * allows both break and continue inside the composed
             * statement. continue starts at the next element
             * break starts with the instruction after loop.
             */

            if ($4.typ != DILV_INT)
	      fatal("Arg 1 of 'foreach' not an integer");
            wtmp = &tmpl.core[$2];
            bwrite_ubit8(&wtmp,DILI_FOE); /* foreach - clear / build */
            wtmp = &tmpl.core[$9];
            bwrite_ubit8(&wtmp,DILI_FON); /* foreach - next */
            wtmp = &tmpl.core[$11];
            bwrite_ubit32(&wtmp,wcore-tmpl.core);
            wtmp = &tmpl.core[$16];
            bwrite_ubit8(&wtmp,DILI_GOTO); /* loop */
            bwrite_ubit32(&wtmp,$9);
            $$.fst=$2;
            $$.lst=wcore-tmpl.core;
         }
         | DILSI_WHI pushbrk pushcnt defcnt
            ihold '(' coreexp ')' ahold dilcomposed ihold ahold
            defbrk popbrk popcnt
         {
            /*
             * allows both break and continue inside the composed
             * statement. continue starts at the test statement.
             * break starts with the instruction after loop.
             */
         	 
            /* made with 'if' and 'goto' */
            wtmp = &tmpl.core[$5];
            bwrite_ubit8(&wtmp,DILI_IF); /* the instruction */
            wtmp = &tmpl.core[$9];
            bwrite_ubit32(&wtmp,wcore - tmpl.core); /* address of break */
            wtmp = &tmpl.core[$11];
            bwrite_ubit8(&wtmp,DILI_GOTO); /* test again */
            wtmp = &tmpl.core[$12];
            bwrite_ubit32(&wtmp,$5); /* address of start */
            $$.fst = $5;
            $$.lst = wcore-tmpl.core;
         }
         | ':' SYMBOL ':' dilcomposed
         {
	    int i;

            /* register label */
            if ((i = search_block($2, (const char **) label_names, TRUE)) != -1)
	    {
	       if (label_adr[i] != SKIP)
		 fatal("Redefinition of label");
	       else
	       {
		  /* update label */
		  label_adr[i]= $4.fst;
		  /* fprintf(stderr,"LABEL %s had adress %d\n",
			  label_names[i], $4.fst); */
	       }
            } else
	      add_label($2, $4.fst);
            $$ = $4;
         }
         ;

arglist  : /* naught */
         | someargs ',' arglist
         | someargs
         ;


someargs : coreexp
         {
            /* collect argument types */
            ref.argt[ref.argc] = $1.typ;
            if (++ref.argc > ARGMAX)
               fatal("Too many arguments");
         }
         ;

dilass   : ihold corevar DILTO_ASS dilassrgt
         {
	    if ($4.dsl==DSL_FCT)
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
	       if (tmpl.core[$2.fst]!=DILE_VAR)
		 fatal("Variable must be true local variable");
              
	       /* read variable number from core */
	       wtmp = &tmpl.core[1+($2.fst)];
	       varnum=bread_ubit16(&wtmp);

	       wtmp = &tmpl.core[$4.fst];
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
	       wtmp = &tmpl.core[$1];
	       bwrite_ubit8(&wtmp,DILI_RFC);
	       $$.fst = $1;
	       $$.lst = $4.lst;
	       
	    } else if ($4.dsl==DSL_SFT) {

	       /* this is an assignment from a symbolic function */
	       int i, varnum;
	       char buf[255],nbuf[255],zbuf[255];

	       /*
		* For symbolic function calls:
		* <rsfcall> <var> <var#> <funcnumber> <nargs> <arg> <arg> ...
		*/

	       /* fprintf(stderr,"Symbolic function call\n"); */

	       /* check if corevar is true variable */
	       if (tmpl.core[$2.fst]!=DILE_VAR)
		 fatal("Variable must be true local variable");
              
	       /* check func/var types impossible! */

	       wtmp = &tmpl.core[$1];
	       bwrite_ubit8(&wtmp,DILI_SFC);
	       $$.fst = $1;
	       $$.lst = $4.lst;	       
	       
	    } else {
	       /* this is an assignment to an expression */	 

	       /* fprintf(stderr,"Assignment\n"); */
	       switch ($2.typ) {
		 case DILV_SP:
		 case DILV_EDP:
		 case DILV_SLP:
		 case DILV_UP:
                  if (($2.typ != $4.typ) && ($4.typ != DILV_NULL))
		    fatal("Assigning incompatible types (C)");
                  else {
                     wtmp = &tmpl.core[$1];
                     bwrite_ubit8(&wtmp,DILI_ASS);
                     $$.fst = $1;
                     $$.lst = $4.lst;
                  }
		  break;
		 case DILV_INT:
                  if ($2.typ != $4.typ)
		    fatal("Assigning incompatible types (D)");
                  else {
                     wtmp = &tmpl.core[$1];
                     bwrite_ubit8(&wtmp,DILI_ASS);
                     $$.fst = $1;
                     $$.lst = $4.lst;
                  }
		  break;
		 default:
                  fatal("Type error in assign");
		  break;
	       }
	    }
	 }
         ;

proccall : corevar ihold ihold '(' arginit arglist ')'
         {
	    int i, refnum;
	    char buf[256];

	    if ($1.dsl != DSL_FCT)
	    {
	       if ($1.typ == DILV_SP)
	       {
		  /* this is a symbolic proc/func call */
		  $$.dsl=DSL_SFT;

		  $$.fst = $1.fst;
		  $$.typ = $1.typ;
		  wtmp=&tmpl.core[$2];
		  bwrite_ubit16(&wtmp,ref.argc);
		  $$.lst = wcore-tmpl.core;
	       }
	       else
		 fatal("Report to Papi.");
	    }
	    else
	    {
	       /* this is a normal proc/func call */
	       $$.dsl=DSL_FCT;

	       if ($1.dsl!=DSL_FCT)
		 fatal("reference not a procedure or function");

	       /* fprintf(stderr, "Func : fst %p, lst %p.\n",
		       $1.fst, $1.lst); */

	       /* read refnum from core */
	       wtmp = &tmpl.core[$1.fst];

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

	       $$.fst = $1.fst;
	       $$.typ = $1.typ;
	       wtmp=&tmpl.core[$2];
	       bwrite_ubit16(&wtmp,ref.argc);
	       $$.lst = wcore-tmpl.core;
	    }
	 }
         ;

corefunc : DILSE_INTR ihold '(' coreexp ',' coreexp ',' labelskip ')'
         {
	    checkbool("argument 1 in 'interrupt'",$4.boolean);
            if ($4.typ != DILV_INT)
               fatal("Arg 1 of 'interrupt' not number");
            else {
	       $$.fst=$2;
	       $$.lst=$8.lst;
               /* Type is ok */
	       wtmp=&tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILE_INTR);
	       tmpl.intrcount++;
	       frm.intrcount++;
            }
	    $$.dsl=DSL_DYN;
	    $$.typ=DILV_INT;
	 }
         | DILSI_OAC ihold ihold ahold '(' coreexp ',' labelskip ')'
         {
	    ubit32 flags;

            $$.fst = $2;
            $$.lst = $8.lst;
            /* write an interrupt instead! */
            wtmp = &tmpl.core[$2];
            bwrite_ubit8(&wtmp,DILE_INTR);
	    bwrite_ubit8(&wtmp,DILE_INT);
	    flags = SFB_ACTIVATE;
            bwrite_ubit32(&wtmp,flags);
	    tmpl.intrcount++;
	    frm.intrcount++;
	    $$.dsl=DSL_DYN;
	    $$.typ=DILV_INT;
         }
         ; 

dilassrgt: coreexp
         {
	    $$=$1;
	 }
         | proccall
         {
	    $$=$1;
         }
         | corefunc
         ;

dilinst  : dilproc
         { $$ = $1; }
         | dilass
         {  $$ = $1; }
         | corefunc
         { $$ = $1; }
         | DILSI_RTS ihold
         {
            if (!istemplate) {
              fatal("return only allowed in templates");
            }
            if (tmpl.rtnt != DILV_NULL) {
              fatal("no return expression expected");
	    }            	

            /* WAS: bwrite_ubit8(&wcore,DILI_RTS); */ /* the instruction */

            wtmp = &tmpl.core[$2];
            bwrite_ubit8(&wtmp,DILI_RTS); /* the instruction */

	    $$.fst = $2;
	    $$.lst = $2+1;
         }
         | DILSI_RTS ihold '(' coreexp ')'
         {
            if (!istemplate) {
              fatal("return only allowed in templates");
            }
            if (tmpl.rtnt != $4.typ) {
              fatal("return expression not of correct type");
	    }            	
            wtmp = &tmpl.core[$2];
            bwrite_ubit8(&wtmp,DILI_RTF); /* the instruction */
	    $$.fst = $2; 
	    $$.lst = $4.lst;
         }
         | DILSI_ON ihold coreexp ihold ihold DILSI_GOT labellist
         {
	    if ($3.typ != DILV_INT)
	      fatal("Expression after 'on' does not return integer");
	    else {
	       $$.fst=$2;
	       $$.lst=$7.lst;
	       wtmp=&tmpl.core[$2];
	       bwrite_ubit8(&wtmp, DILI_ON);
	       wtmp=&tmpl.core[$4];
	       bwrite_ubit16(&wtmp, $7.typ);
	    }
         }
         | DILSI_GOT ihold label
         {
            wtmp = &tmpl.core[$2];
            bwrite_ubit8(&wtmp,DILI_GOTO);
            $$.fst = $2;
            $$.lst = $3.lst;
         }
         | DILSI_BRK ihold ahold
         {
            /* break statement */
            if (!break_no) {
               /* not in loop */
               fatal("Using break outside loop");
            } else {
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_GOTO);
               wtmp = &tmpl.core[$3];
               /* register use or find break label */
               bwrite_ubit32(&wtmp,
                  get_label(label_names[break_idx[break_no-1]],$3)); 
            }
            $$.fst = $2;
            $$.lst = $3+4;
         }
         | DILSI_CNT ihold ahold
         {
            /* continue statement */
            if (!cont_no) {
               /* not in loop */
               fatal("Using continue outside loop");
            } else {
               wtmp = &tmpl.core[$2];
               bwrite_ubit8(&wtmp,DILI_GOTO);
               wtmp = &tmpl.core[$3];
               /* register use or find continue label */
               bwrite_ubit32(&wtmp,
                  get_label(label_names[cont_idx[break_no-1]],$3)); 
            }
            $$.fst = $2;
            $$.lst = $3+4;
         }
         ;
%%

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
