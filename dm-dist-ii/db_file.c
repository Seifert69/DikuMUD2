/* *********************************************************************** *
 * File   : db_file.c                                 Part of Valhalla MUD *
 * Version: 2.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Routines for writing and reading from/to buffers. Used by      *
 *          dmc and save char.                                             *
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

/* 23/08/93 jubal   : Set capacity to something reasonable if unset        
 * 28-Jan-95 HHS:   Reconstructed code generation entirely
 * 14-Apr-95 HHS:   Added for and foreach, break and continue
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "structs.h"
#include "utils.h"
#include "textutil.h"
#include "interpreter.h"
#include "dil.h"
#include "dilrun.h"
#include "utility.h"
#include "db_file.h"
#include "handler.h"
#include "unixshit.h"
#include "db.h"
#include "bytestring.h"


int g_nCorrupt = 0;                    /* > 0 when a unit is corrupt       */

CByteBuffer g_FileBuffer(16384);

//int filbuffer_length = 0;             /* The length of filbuffer         */
//ubit8 *filbuffer = 0;                 /* Buffer for read/write unit      */


#ifdef DMSERVER
  extern struct unit_function_array_type unit_function_array[];
#endif



int bread_extra(CByteBuffer *pBuf, class extra_descr_data **ppExtra)
{
   struct extra_descr_data *e, *te;
   ubit8 i;
   char *c;

   *ppExtra = NULL;
   te       = NULL;

   if (pBuf->Read8(&i))
     return 1;    /* No of extra descriptions */

   /* While description is non null, keep reading */
   for ( ; i > 0; i--)
   {
      e = new (struct extra_descr_data);

      if (pBuf->SkipString(&c))
	return 1;

      e->descr.Reassign(c);
      e->names.ReadBuffer(pBuf);
      e->next  = NULL;

      /* Insert at END of list to keep the ordering of the names */
      /* as specified in the binary file                         */
      if (!*ppExtra)
      {
	 *ppExtra = e;
	 te = e;
      }
      else
      {
	 te->next = e;
	 te = e;
      }
   }

   return 0;
}


int bread_swap(CByteBuffer *pBuf, struct unit_data *u)
{
   char *c;

   if (pBuf->SkipString(&c))
     return 1;

   u->title.Reassign(c);

   if (pBuf->SkipString(&c))
     return 1;

   u->out_descr.Reassign(c);

   if (pBuf->SkipString(&c))
     return 1;

   u->in_descr.Reassign(c);

   u->extra_descr = new extra_descr_data;

   if (bread_extra(pBuf, &u->extra_descr))
     return 1;

   return 0;
}


int bread_swap_skip(CByteBuffer *pBuf)
{
   int i;
   ubit8 t8;

   pBuf->SkipString();
   pBuf->SkipString();
   pBuf->SkipString();

   if (pBuf->Read8(&t8))
     return 1;

   i = t8;

   for ( ; i > 0; i--)
   {
      pBuf->SkipString();
      if (pBuf->SkipNames())
	return 1;
   }

   return 0;
}

/*
 * This function reads a DIL template
 */
struct diltemplate *bread_diltemplate(CByteBuffer *pBuf)
{
#ifdef DMSERVER
   extern int mud_bootzone;
   int valid;
#endif
   int i,j;
   struct diltemplate *tmpl;
     
   /* read a template */
   CREATE(tmpl, struct diltemplate, 1);

   tmpl->nActivations = 0;
	
   pBuf->ReadStringAlloc((char **) &tmpl->prgname); //MS2020
   pBuf->Read8(&tmpl->flags);
   pBuf->Read16(&tmpl->intrcount);

   REMOVE_BIT(tmpl->flags, DILFL_EXECUTING|DILFL_CMDBLOCK);

   pBuf->Read16(&tmpl->varcrc);
   pBuf->Read16(&tmpl->corecrc);
   pBuf->Read8(&tmpl->rtnt);
   pBuf->Read8(&tmpl->argc);
	
   if (tmpl->argc) {
      CREATE(tmpl->argt, ubit8, tmpl->argc);
      
      for (i=0;i<tmpl->argc;i++)
	pBuf->Read8(&tmpl->argt[i]);   /* argument types */
   } else
     tmpl->argt=NULL;
   
   pBuf->ReadBlock(&tmpl->core, &tmpl->coresz);
   
   pBuf->Read16(&tmpl->varc);      /* number of variables */
   
   if (tmpl->varc) {
      CREATE(tmpl->vart, ubit8, tmpl->varc);
      
      for (i=0;i<tmpl->varc;i++)
	pBuf->Read8(&tmpl->vart[i]);
   } else
     tmpl->vart=NULL;

   pBuf->Read16(&tmpl->xrefcount);

   if (tmpl->xrefcount)
   {
      CREATE(tmpl->xrefs, struct dilxref, tmpl->xrefcount);

      /* read the symbolic references */      
      for (i=0;i<tmpl->xrefcount;i++)
      {
	 pBuf->ReadStringAlloc(&tmpl->xrefs[i].name);
	 pBuf->Read8(&tmpl->xrefs[i].rtnt);
	 pBuf->Read8(&tmpl->xrefs[i].argc);

	 if (tmpl->xrefs[i].argc)
	 {
	    CREATE(tmpl->xrefs[i].argt, ubit8, tmpl->xrefs[i].argc);
	    for (j=0;j<tmpl->xrefs[i].argc;j++)
	      pBuf->Read8(&tmpl->xrefs[i].argt[j]);
	 }
	 else
	   tmpl->xrefs[i].argt=NULL;
      }
   }
   else
     tmpl->xrefs=NULL;

#ifdef DMSERVER
   /* Resolve the external references runtime */

   if (tmpl->xrefcount)
     CREATE(tmpl->extprg, struct diltemplate *, tmpl->xrefcount);
   else
     tmpl->extprg=NULL;

   if (!mud_bootzone) {
      /*
       * This template not loaded boottime, so resolve
       * and typecheck the external references now
       */
      for (i=0;i<tmpl->xrefcount;i++) {
         tmpl->extprg[i] = find_dil_template( tmpl->xrefs[i].name );
	 valid=1;

	 if (tmpl->extprg[i]) {
	    /* check argument count and types */
	    if ((tmpl->xrefs[i].rtnt!=tmpl->extprg[i]->rtnt) ||
		(tmpl->xrefs[i].argc!=tmpl->extprg[i]->argc))
	      valid=0;
	    for (j=0;j<tmpl->xrefs[i].argc;j++)
	      if (tmpl->xrefs[i].argt[j]!=tmpl->extprg[i]->argt[j]) 
		valid=0;
	 } else {
	    /* ERROR MESSAGE HERE */
	    slog(LOG_ALL,0,"Cannot resolve external reference '%s'",
		     tmpl->xrefs[i].name);
	    valid = 0;
	 }
	 /* Typecheck error ! */
	 if (!valid) {
	    tmpl->extprg[i]=NULL;
	    /* ERROR MESSAGE HERE */
	    slog(LOG_ALL,0,"Error typechecking reference to '%s'",
		     tmpl->xrefs[i].name);
	 }
      }
   }
#else
   /* This is compiletime, no resolve done */
   tmpl->extprg = NULL;
#endif
   tmpl->next = NULL;
   return tmpl;
}	


/* Reads DIL interrupt list */
void bread_dilintr(CByteBuffer *pBuf, struct dilprg *prg)
{
   int i;
   ubit32 lab;

   /* read interrupts */
   pBuf->Read16(&prg->stack[0].intrcount);

   if (prg->stack[0].intrcount)
   {
      CREATE(prg->stack[0].intr,struct dilintr,prg->stack[0].intrcount);

      for (i=0;i<prg->stack[0].intrcount;i++)
      {
	 pBuf->Read16(&prg->stack[0].intr[i].flags);
	 pBuf->Read32(&lab);
	 prg->stack[0].intr[i].lab = &(prg->stack[0].tmpl->core[lab]);
      }
   }
   else
     prg->stack[0].intr=NULL;
}


void bwrite_dilintr(CByteBuffer *pBuf, struct dilprg *prg)
{
   ubit16 i;
   ubit32 lab;
   
   pBuf->Append16(prg->stack[0].intrcount);

   for (i=0;i<prg->stack[0].intrcount;i++) 
   {
      if (!IS_SET(prg->flags,DILFL_RECALL)) 
      {
	 /* non-recall program: clear all interrupts! */
	 pBuf->Append16(0);
	 pBuf->Append32(0);
      }
      else
      {
	 if (prg->stack[0].intr[i].lab)
	   lab = prg->stack[0].intr[i].lab - prg->stack[0].tmpl->core;
	 else
	   lab = 0; 

	 pBuf->Append16(prg->stack[0].intr[i].flags);
	 pBuf->Append32(lab);
      }
   }
}

/*
 * This function reads a DIL program
 *
 * Implementation notes:
 *
 * in DMC:
 *   The DIL programs to be copies (from the zones)
 *   are read as a function, that runtime tries
 *   to initialize the copied function (DILP_COPY).
 *
 *   The 'old' type DIL programs, are inline, and
 *   thus have a local template, saved along with
 *   the program specification. (DILP_INLINE).
 *
 *   The loaded templates from DIL compiler are not
 *   typechecked in DMC compiletime.
 * 
 * in DIL:
 *   The compiler saves eihter templates or
 *   dil programs with local templates on demand.
 *   The usages of external programs are typechecked.
 * 
 * in SERVER:
 *   Upon loading a template when first reading
 *   a zone, no check of xrefs are made, until
 *   all runtime zones have been loaded.
 * 
 *   Other runtime loading of templates, require
 *   lookup and typecheck of loaded template.
 *
 */
void *bread_dil(CByteBuffer *pBuf, struct unit_data *owner, ubit8 version,
		struct unit_fptr *fptr)
{
   struct dilprg *prg;
   struct diltemplate *tmpl = NULL;
   ubit32 recallpc=0;
   ubit16 t16;
   int i, novar;
#ifdef DMSERVER
   char buf[255];
   char name[255];
#endif
   
   CREATE(prg, struct dilprg, 1);
   CREATE(prg->stack, struct dilframe, 1);
   prg->sp=prg->stack;
   prg->stacksz=1;
   prg->owner = owner;
#ifdef DMSERVER
   prg->next = dil_list;
   dil_list = prg;
#endif

      /* read new version */

      pBuf->Read8(&prg->flags);

      REMOVE_BIT(prg->flags, DILFL_EXECUTING|DILFL_CMDBLOCK);

      if (fptr && IS_SET(prg->flags, DILFL_AWARE))
	SET_BIT(fptr->flags, SFB_AWARE);

      if (!IS_SET(prg->flags, DILFL_COPY))
      {
      	 /* read local template */
      	 tmpl = bread_diltemplate(pBuf);
      }
      else
      {
#ifndef DMSERVER
      	 tmpl=NULL;
	 error(HERE, "Impossible");
#else

      	 /* lookup template, only runtime */
         pBuf->ReadStringCopy(name, sizeof(name));      /* prg/template name */
      	 tmpl = find_dil_template(name);

	 if (!tmpl)
	 {
	    /* Make static template containing just the name */
	    CREATE(tmpl, struct diltemplate, 1);
	    tmpl->nActivations = 0;
	    tmpl->prgname = str_dup(name);
	    tmpl->zone = NULL;
	    /* Prevent all execution */
	    SET_BIT(prg->flags, DILFL_EXECUTING);
	    /* slog(LOG_ALL,0,"Error resolving copy template '%s'",
		 name); */
	    tmpl->flags |= DILFL_FREEME;
	 }
#endif
      }
      
      prg->waitcmd=WAITCMD_MAXINST - 1;  /* Command countdown          */
      pBuf->Read16(&prg->varcrc);        /* variable crc from compiler */
      pBuf->Read16(&prg->corecrc);       /* core crc from compiler     */
      pBuf->Read32(&recallpc);           /* recalled PC                */

      if (!IS_SET(prg->flags,DILFL_RECALL))
	recallpc=0;

      /* read stackframe #0 */

      prg->sp->ret = 0;                 /* return variable # (ignored) */
      prg->sp->tmpl = tmpl;             /* template                    */

      pBuf->Read16(&t16);             /* the SAVED #vars             */
      novar = t16;

      if (novar)
	CREATE(prg->sp->vars,struct dilvar,novar); /* saved variables */
      else
	prg->sp->vars=NULL;

      for (i=0;i<novar;i++)
      {
	 pBuf->Read8(&prg->sp->vars[i].type);

         switch (prg->sp->vars[i].type) {
            case DILV_SLP:
	       prg->sp->vars[i].val.namelist = new cNamelist;
               prg->sp->vars[i].val.namelist->ReadBuffer(pBuf);
               break;
            case DILV_SP:
	       pBuf->ReadStringAlloc(&prg->sp->vars[i].val.string);
               break;
            case DILV_INT:
	       pBuf->Read32(&prg->sp->vars[i].val.integer);
               break;
            default:
               prg->sp->vars[i].val.integer = 0;
         }
      }

      /* read interrupt array */
      bread_dilintr(pBuf, prg);
      
      prg->stack[0].securecount=0; /* number of secures */
      prg->stack[0].secure=NULL;   /* secured vars */

      /* The static template ends here.... */
      if (tmpl->flags == DILFL_FREEME) {
	 tmpl->varc=novar;
	 tmpl->intrcount=prg->stack[0].intrcount; 
	 return prg;
      }
      
      if ((prg->varcrc != tmpl->varcrc)||(novar != tmpl->varc))
      {
#ifndef DMSERVER
	 error(HERE, "Impossible");
#else
	 /* state of variables have changed */
	 /* sprintf(buf,"Var CRC mismatch. prg:%d, tmpl:%d",
		 prg->varcrc,tmpl->varcrc);
	 slog(LOG_ALL,0,buf); */

         for (i=0;i<novar;i++)
	 {
	    dil_free_var(&prg->sp->vars[i]);

	    prg->sp->vars[i].val.string  = NULL;
	    prg->sp->vars[i].val.integer = 0;
	 }

	 if (prg->sp->vars)
	   free(prg->sp->vars);

	 if (tmpl->varc)
	   CREATE(prg->sp->vars,struct dilvar,tmpl->varc);
	 else
	   prg->sp->vars=NULL;
	 for (i=0;i<tmpl->varc;i++) {
	    prg->sp->vars[i].type=tmpl->vart[i];
	    if (tmpl->vart[i] == DILV_SLP)
	      prg->sp->vars[i].val.namelist = new cNamelist;
	    else
	      prg->sp->vars[i].val.string=NULL;
	 }
         prg->varcrc=tmpl->varcrc; /* variables updated! */
	 
	 /* free interrupts */
	 if(prg->sp->intr)
	   free(prg->sp->intr);

	 if (tmpl->intrcount)
	   CREATE(prg->sp->intr,struct dilintr,tmpl->intrcount);
	 else
	   prg->sp->intr=NULL;
	 prg->sp->intrcount=tmpl->intrcount;

	 /* clear interrupts */
	 for (i=0;i<prg->sp->intrcount;i++) {
	    prg->sp->intr[i].flags=0;
	    prg->sp->intr[i].lab=NULL;
	 }

	 recallpc=0;
#endif
      }

      if ((prg->corecrc)!=(tmpl->corecrc))
      {
	/* slog(LOG_ALL, 0, "Core CRC mismatch. prg:%d, tmpl:%d",
	      prg->corecrc, tmpl->corecrc); */
	 recallpc=0;
	 prg->corecrc=tmpl->corecrc;

	 /* free interrupts */
	 if (prg->sp->intr)
	   free(prg->sp->intr);

	 if (tmpl->intrcount)
	   CREATE(prg->sp->intr,struct dilintr,tmpl->intrcount);
	 else
	   prg->sp->intr=NULL;
	 prg->sp->intrcount=tmpl->intrcount;

	 /* clear interrupts */
	 for (i=0;i<prg->sp->intrcount;i++) {
	    prg->sp->intr[i].flags=0;
	    prg->sp->intr[i].lab=NULL;
	 }
      }

      prg->stack[0].pc = &(tmpl->core[recallpc]); /* program counter */


   return prg;
}

struct unit_fptr *bread_func(CByteBuffer *pBuf, ubit8 version,
			     struct unit_data *owner)
{
   struct unit_fptr *fptr, *head;
   int cnt, i;
   ubit8 t8;
   ubit16 t16;
   ubit32 t32;

   fptr = 0;
   head = 0;
   cnt = 0;

   g_nCorrupt += pBuf->Read8(&t8);
   i = t8;

   for (; i > 0; i--)
   {
      if (fptr)
      {
	 CREATE(fptr->next, struct unit_fptr, 1);
	 fptr = fptr->next;
      }
      else
      {
	 CREATE(head, struct unit_fptr, 1);
	 fptr = head;
      }

      g_nCorrupt += pBuf->Read16(&fptr->index);
      if (fptr->index > SFUN_TOP_IDX)
      {
	 slog(LOG_ALL, 0, "Illegal func index in bread_func index - corrupt.");
	 g_nCorrupt = TRUE;
	 return NULL;
      }

      g_nCorrupt += pBuf->Read16(&fptr->heart_beat);
      if ((fptr->heart_beat) && (fptr->heart_beat < WAIT_SEC))
      {
	 slog(LOG_ALL, 0, "WARNING: HEARTBEAT LOW (%d)\n", fptr->heart_beat);
      }
      g_nCorrupt += pBuf->Read16(&fptr->flags);

      if (fptr->index == SFUN_DIL_INTERNAL)
	fptr->data       = bread_dil(pBuf, owner, version, fptr);
      else if (fptr->index == SFUN_DILCOPY_INTERNAL)
      {
	 char *zname;
	 char *uname;
	 char name[256], *c;

	 struct dilargstype *dilargs;

	 CREATE(dilargs, struct dilargstype, 1);

	 pBuf->ReadStringCopy(name, sizeof(name));
	 strcat(name, "/");
	 pBuf->SkipString(&c);
	 strcat(name, c);

	 dilargs->name = str_dup(name);

	 pBuf->Read8(&dilargs->no);

	 for (int j=0; j < dilargs->no; j++)
	 {
	    pBuf->Read8(&dilargs->dilarg[j].type);

	    switch (dilargs->dilarg[j].type)
	    {
	      case DILV_SLP:
	       pBuf->ReadNames(&dilargs->dilarg[j].data.stringlist);
	       break;

	      case DILV_SP:
	       pBuf->ReadStringAlloc(&dilargs->dilarg[j].data.string);
	       break;

	      case DILV_INT:
		ubit32 i;
		pBuf->Read32(&i);
		dilargs->dilarg[j].data.num = i;
		break;

	      default:
	       error(HERE, "Uhadada");
	    }
	 }

	 fptr->data = dilargs;
      }
      else
      {
#ifdef DMSERVER
	 REMOVE_BIT(fptr->flags, SFB_ALL);
	 SET_BIT(fptr->flags, unit_function_array[fptr->index].sfb);
#endif
	 pBuf->ReadStringAlloc((char **) &fptr->data);
      }

      fptr->next         = NULL;
   }

   return head;
}


void bread_block(FILE *datafile, long file_pos, int length, void *buffer)
{
  /* Search from beginning of file (binary file) */
  if (fseek(datafile, file_pos, SEEK_SET))
     assert(FALSE);

  if (!((int) fread(buffer, sizeof(ubit8), length, datafile)==length))
     assert(FALSE);
}


void bwrite_swap(CByteBuffer *pBuf, struct unit_data *u)
{
   pBuf->AppendString(UNIT_TITLE_STRING(u));
   pBuf->AppendString(UNIT_OUT_DESCR_STRING(u));
   pBuf->AppendString(UNIT_IN_DESCR_STRING(u));
   UNIT_EXTRA_DESCR(u)->AppendBuffer(pBuf);
}


void bwrite_affect(CByteBuffer *pBuf, struct unit_affected_type *af,
		   ubit8 version)
{
   int i = 0;
   ubit32 nPos, nOrgPos = pBuf->GetLength();

   if (version <= 56)
     pBuf->Append8(0); /* Assume no affects by default */
   else
     pBuf->Append16(0); /* Assume no affects by default */

   for (; af; af = af->next)
   {
      i++;
      pBuf->Append16(af->duration);
      pBuf->Append16(af->id);
      pBuf->Append16(af->beat);

      pBuf->Append32(af->data[0]);
      pBuf->Append32(af->data[1]);
      pBuf->Append32(af->data[2]);

      pBuf->Append16(af->firstf_i);
      pBuf->Append16(af->tickf_i);
      pBuf->Append16(af->lastf_i);
      pBuf->Append16(af->applyf_i);
   }

   assert(i <= 5000);

   if (i > 0)
   {
      nPos = pBuf->GetLength();
      pBuf->SetLength(nOrgPos);

      if (version <= 56)
	pBuf->Append8(i);
      else
	pBuf->Append16(i);

      pBuf->SetLength(nPos);
   }
}

/*
 * This function writes a DIL template
 */
void bwrite_diltemplate(CByteBuffer *pBuf, struct diltemplate *tmpl)
{
   int i,j;
	
   /* write a template */

   /* slog(LOG_ALL, 0, "Write Template: Var %d, Core %d.",
      tmpl->varcrc, tmpl->corecrc); */

   if (tmpl->zone)
   {
      char buf[512];

      sprintf(buf, "%s@%s", tmpl->prgname, tmpl->zone->name);
      pBuf->AppendString(buf);
   }
   else
     pBuf->AppendString(tmpl->prgname);      /* program name @ zone */

   pBuf->Append8(tmpl->flags);         /* recall, etc */
   pBuf->Append16(tmpl->intrcount);    /* max number of intr */
   pBuf->Append16(tmpl->varcrc);       /* variable crc from compiler */
   pBuf->Append16(tmpl->corecrc);      /* core crc from compiler */
   pBuf->Append8(tmpl->rtnt);          /* return type */
   pBuf->Append8(tmpl->argc);          /* number of arguments */

   for (i=0;i<tmpl->argc;i++)
	pBuf->Append8(tmpl->argt[i]);  /* argument types */

   /* fprintf(stderr,"Writing core: %d\n",tmpl->coresz); */

   pBuf->AppendBlock(tmpl->core, tmpl->coresz); /* core */
 
   pBuf->Append16(tmpl->varc);         /* number of variables */
   for (i=0;i<tmpl->varc;i++)
      pBuf->Append8(tmpl->vart[i]);     /* variable types */

   pBuf->Append16(tmpl->xrefcount);    /* number of external references */

   /* read the symbolic references */      
   for (i=0;i<tmpl->xrefcount;i++)
   {
      pBuf->AppendString(tmpl->xrefs[i].name);
      pBuf->Append8(tmpl->xrefs[i].rtnt);
      pBuf->Append8(tmpl->xrefs[i].argc);

      for (j=0;j<tmpl->xrefs[i].argc;j++)
	pBuf->Append8(tmpl->xrefs[i].argt[j]);         
   }
   /* resolved references are runtime */
}	

/*
 * This function writes a DIL program
 */
void bwrite_dil(CByteBuffer *pBuf, struct dilprg *prg)
{
   int i;
   struct diltemplate *tmpl;
   
   /* write new version */
   pBuf->Append8(prg->flags);           /* from other template? */

   tmpl=prg->stack[0].tmpl;

   if (!IS_SET(prg->flags, DILFL_COPY))
      /* write local template */
      bwrite_diltemplate(pBuf, tmpl);
   else
   {
      if (tmpl->zone)
      {
	 char buf[512];
	 
	 sprintf(buf, "%s@%s", tmpl->prgname, tmpl->zone->name);
	 pBuf->AppendString(buf);
      }
      else
	pBuf->AppendString(tmpl->prgname);          
	
   }
   
   /* slog(LOG_ALL, 0, "Write Program: Var %d, Core %d.",
      prg->varcrc, prg->corecrc); */

   /* variable crc from compiler */
   pBuf->Append16(prg->varcrc);        

   /* core crc from compiler */
   pBuf->Append16(prg->corecrc);        

   /* save state of frame 0 ONLY!!!!! */
   pBuf->Append32(prg->stack[0].pc - prg->stack[0].tmpl->core);

   /* write stackframe #0 */
   pBuf->Append16(tmpl->varc);     /* the SAVED #vars */

   for (i=0;i<tmpl->varc;i++)
   {
     pBuf->Append8(prg->stack[0].vars[i].type);
     
     switch (prg->stack[0].vars[i].type)
     {
       case DILV_SLP:
	 if (prg->stack[0].vars[i].val.namelist)
	   prg->stack[0].vars[i].val.namelist->AppendBuffer(pBuf);
	 else
	   pBuf->AppendNames(NULL);
	 break;
	 
        case DILV_SP:
	  pBuf->AppendString(prg->stack[0].vars[i].val.string);
	  break;

        case DILV_INT:
	  pBuf->Append32(prg->stack[0].vars[i].val.integer);
	  break;
      }
   }
   
   /* write intr */
   bwrite_dilintr(pBuf, prg);
}

void bwrite_func(CByteBuffer *pBuf, struct unit_fptr *fptr)
{
   char *data;
   int cnt;
   int i = 0;
   ubit32 nPos, nOrgPos = pBuf->GetLength();
   pBuf->Append8(0); /* Assume no affects by default */

   cnt = 0;

   for (; fptr; fptr = fptr->next)
   {
      assert(fptr->index <= SFUN_TOP_IDX);

      data = (char *) fptr->data;

#ifdef DMSERVER
      if (unit_function_array[fptr->index].save_w_d == SD_NEVER)
	continue; /* DONT SAVE THIS FROM INSIDE THE GAME! */

      if (fptr->data && unit_function_array[fptr->index].save_w_d == SD_NULL)
	data = 0;

      /* Else this is SD_ASCII and we can save anything we like ... :-) */
#endif
      i++;
      pBuf->Append16(fptr->index);

      if (fptr->heart_beat && fptr->heart_beat < WAIT_SEC)
	slog(LOG_ALL, 0, "WARNING: HEARTBEAT LOW (%d)\n", fptr->heart_beat);

      pBuf->Append16(fptr->heart_beat);
      pBuf->Append16(fptr->flags);

      if (fptr->index == SFUN_DIL_INTERNAL)
      {
	 assert(fptr->data);
	 bwrite_dil(pBuf, (struct dilprg *) fptr->data);
      }
      else if (fptr->index == SFUN_DILCOPY_INTERNAL)
      {
#ifdef DMC_SRC
	 assert(fptr->data);
	 struct dilargstype *dilargs = (struct dilargstype *) fptr->data;

	 pBuf->AppendDoubleString(dilargs->name);

	 pBuf->Append8(dilargs->no);

	 for (int j=0; j < dilargs->no; j++)
	 {
	    pBuf->Append8(dilargs->dilarg[j].type);

	    switch (dilargs->dilarg[j].type)
	    {
	      case DILV_SLP:
	       pBuf->AppendNames((const char**) dilargs->dilarg[j].data.stringlist);
	       break;
	      case DILV_SP:
	       pBuf->AppendString(dilargs->dilarg[j].data.string);
	       break;
	      case DILV_INT:
	       pBuf->Append32(dilargs->dilarg[j].data.num);
	       break;
	      default:
	       error(HERE, "Not possible");
	    }
	 }
#else
	 error(HERE, "Not possible!");
#endif
      }
      else
      {
	 if (data)
	   pBuf->AppendString((char *) data);
	 else
	   pBuf->AppendString("");
      }
   }

   assert(i <= 255);

   if (i > 0)
   {
      nPos = pBuf->GetLength();
      pBuf->SetLength(nOrgPos);
      pBuf->Append8(i);
      pBuf->SetLength(nPos);
   }
}


void bwrite_block(FILE *datafile, int length, void *buffer)
{
   if (fwrite(buffer, sizeof(ubit8), length, datafile) != (size_t) length)
    assert(FALSE);
}



/* Write unit to string. */
int write_unit_string(CByteBuffer *pBuf, struct unit_data *u)
{
   int i;
   ubit8 nVersion;

   nVersion = 57;

   ubit32 nPos = pBuf->GetLength();

   pBuf->Append8(nVersion);  /* Version Number! */

   UNIT_NAMES(u).AppendBuffer(pBuf);

   bwrite_swap(pBuf, u);

   pBuf->AppendDoubleString((char *) UNIT_KEY(u));

   pBuf->Append32(UNIT_MANIPULATE(u));
   pBuf->Append16(UNIT_FLAGS(u));
   pBuf->Append16(UNIT_BASE_WEIGHT(u));
   pBuf->Append16(UNIT_WEIGHT(u));
   pBuf->Append16(UNIT_CAPACITY(u));

   pBuf->Append32((ubit32) UNIT_MAX_HIT(u));
   pBuf->Append32((ubit32) UNIT_HIT(u));

   pBuf->Append16((ubit16) UNIT_ALIGNMENT(u));

   pBuf->Append8(UNIT_OPEN_FLAGS(u));
   pBuf->Append8(UNIT_LIGHTS(u));
   pBuf->Append8(UNIT_BRIGHT(u));
   pBuf->Append8(UNIT_ILLUM(u));
   pBuf->Append8(UNIT_CHARS(u));
   pBuf->Append8(UNIT_MINV(u));

   pBuf->Append16(UNIT_SIZE(u));

   if (UNIT_TYPE(u) == UNIT_ST_ROOM)
   {
      /* See if room is to be placed inside another room! */
      pBuf->AppendDoubleString((char *) UNIT_IN(u));
   }
   else
   {
      struct unit_data *inu = NULL;

      if (IS_PC(u))
	inu = CHAR_LAST_ROOM(u);
      else if (UNIT_IN(u))
#ifdef DMSERVER
	inu = unit_room(u);
#else
        assert(inu == NULL);
#endif

      if (inu && UNIT_FILE_INDEX(inu))
      {
	 pBuf->AppendString(UNIT_FI_ZONENAME(inu));
	 pBuf->AppendString(UNIT_FI_NAME(inu));
      }
      else
      {
	 pBuf->AppendString("");
	 pBuf->AppendString("");
      }
   }

   switch (UNIT_TYPE(u))
   {
      case UNIT_ST_NPC:
         pBuf->AppendString(CHAR_MONEY(u) ? CHAR_MONEY(u) : "");
	 /* fallthru */

      case UNIT_ST_PC:
	 pBuf->Append32(CHAR_EXP(u));
	 pBuf->Append32(CHAR_FLAGS(u));

	 pBuf->Append16((ubit16) CHAR_MANA(u));
	 pBuf->Append16((ubit16) CHAR_ENDURANCE(u));

	 pBuf->Append8(CHAR_NATURAL_ARMOUR(u));
	 pBuf->Append8(CHAR_SPEED(u));

	 pBuf->Append16(CHAR_ATTACK_TYPE(u));
	 pBuf->Append16(CHAR_RACE(u));

	 pBuf->Append16(CHAR_OFFENSIVE(u));
	 pBuf->Append16(CHAR_DEFENSIVE(u));

	 pBuf->Append8(CHAR_SEX(u));
	 pBuf->Append8(CHAR_LEVEL(u));
	 pBuf->Append8(CHAR_POS(u));

	 pBuf->Append8(ABIL_TREE_MAX);
	 for (i=0; i < ABIL_TREE_MAX; i++)
	 {
	    pBuf->Append8(CHAR_ABILITY(u,i));
	    if (IS_PC(u))
	    {
	       pBuf->Append8(PC_ABI_LVL(u,i));
	       pBuf->Append8(PC_ABI_COST(u,i));
	    }
	 }

	 if (IS_PC(u))
	 {
	    pBuf->AppendFloat(PC_ACCOUNT(u).credit);
	    pBuf->Append32(PC_ACCOUNT(u).credit_limit);
	    pBuf->Append32(PC_ACCOUNT(u).total_credit);
	    pBuf->Append16(PC_ACCOUNT(u).last4);
	    pBuf->Append8(PC_ACCOUNT(u).discount);
	    pBuf->Append32(PC_ACCOUNT(u).flatrate);
	    pBuf->Append8(PC_ACCOUNT(u).cracks);

	    pBuf->Append16(PC_LIFESPAN(u));

	    pBuf->Append8(PC_SETUP_ECHO(u));
	    pBuf->Append8(PC_SETUP_REDRAW(u));
	    pBuf->Append8(PC_SETUP_WIDTH(u));
	    pBuf->Append8(PC_SETUP_HEIGHT(u));
	    pBuf->Append8(PC_SETUP_EMULATION(u));
	    pBuf->Append8(PC_SETUP_TELNET(u));
	    pBuf->Append8(PC_SETUP_COLOUR(u));

	    pBuf->AppendString(PC_FILENAME(u));
	    pBuf->AppendString(PC_PWD(u));

	    for (i=0; i < 5; i++)
	      pBuf->Append32(PC_LASTHOST(u)[i]);

	    pBuf->Append32((ubit32) PC_ID(u));
	    pBuf->Append16(PC_CRACK_ATTEMPTS(u));

	    pBuf->AppendString(PC_HOME(u));
	    pBuf->AppendString(PC_GUILD(u));

	    pBuf->Append32((ubit32) PC_GUILD_TIME(u));
	    pBuf->Append16(PC_VIRTUAL_LEVEL(u));
	    
	    pBuf->Append32((ubit32) PC_TIME(u).creation);
	    pBuf->Append32((ubit32) PC_TIME(u).connect);
	    pBuf->Append32((ubit32) PC_TIME(u).birth);
	    pBuf->Append32(PC_TIME(u).played);

	    pBuf->AppendString(PC_BANK(u));
	    pBuf->Append32(PC_SKILL_POINTS(u));
	    pBuf->Append32(PC_ABILITY_POINTS(u));
	    pBuf->Append16(PC_FLAGS(u));

	    pBuf->Append8(SPL_TREE_MAX);
	    for (i=0; i < SPL_TREE_MAX; i++)
	    {
	       pBuf->Append8(PC_SPL_SKILL(u, i));
	       pBuf->Append8(PC_SPL_LVL(u, i));
	       pBuf->Append8(PC_SPL_COST(u, i));
	    }

	    pBuf->Append8(SKI_TREE_MAX);
	    for (i=0; i < SKI_TREE_MAX; i++)
	    {
	       pBuf->Append8(PC_SKI_SKILL(u, i));
	       pBuf->Append8(PC_SKI_LVL(u, i));
	       pBuf->Append8(PC_SKI_COST(u, i));
	    }

	    pBuf->Append8(WPN_TREE_MAX);
	    for (i=0; i < WPN_TREE_MAX; i++)
	    {
	       pBuf->Append8(PC_WPN_SKILL(u, i));
	       pBuf->Append8(PC_WPN_LVL(u, i));
	       pBuf->Append8(PC_WPN_COST(u, i));
	    }

	    pBuf->Append16(PC_CRIMES(u));

	    pBuf->Append8(3);
	    for (i=0; i < 3; i++)
	      pBuf->Append8(PC_COND(u, i));

	    pBuf->Append8(PC_ACCESS_LEVEL(u));

	    PC_QUEST(u)->AppendBuffer(pBuf);
	    PC_INFO(u)->AppendBuffer(pBuf);
	 }
	 else
	 {
	    for (i=0; i < WPN_GROUP_MAX; i++)
	       pBuf->Append8(NPC_WPN_SKILL(u,i));

	    for (i=0; i < SPL_GROUP_MAX; i++)
	       pBuf->Append8(NPC_SPL_SKILL(u,i));

	    pBuf->Append8(NPC_DEFAULT(u));
	    pBuf->Append8(NPC_FLAGS(u));
	 }
	 break;


      case UNIT_ST_OBJ:
	 pBuf->Append32(OBJ_VALUE(u, 0));
	 pBuf->Append32(OBJ_VALUE(u, 1));
	 pBuf->Append32(OBJ_VALUE(u, 2));
	 pBuf->Append32(OBJ_VALUE(u, 3));
	 pBuf->Append32(OBJ_VALUE(u, 4));
	 pBuf->Append32(OBJ_FLAGS(u));
	 pBuf->Append32(OBJ_PRICE(u));
	 pBuf->Append32(OBJ_PRICE_DAY(u));

	 pBuf->Append8(OBJ_TYPE(u));
	 pBuf->Append8(OBJ_RESISTANCE(u));
	 break;

      case UNIT_ST_ROOM:
	 /* Read N,S,E,W,U and D directions */
	 for (i=0; i<6; i++)
	 {
	    const char *c1 = "";
            const char *c2 = "";
 
            if (ROOM_EXIT(u,i) && ROOM_EXIT(u, i)->to_room)
            {
               c1 = (char *) ROOM_EXIT(u, i)->to_room;
               c2 = c1;
               TAIL(c2);
               c2++;
            }
            if (ROOM_EXIT(u,i) && *c1 && *c2)             
	    {
	       pBuf->AppendDoubleString((char *) ROOM_EXIT(u, i)->to_room);
	       ROOM_EXIT(u,i)->open_name.AppendBuffer(pBuf);
	       pBuf->Append16(ROOM_EXIT(u,i)->exit_info);
	       pBuf->AppendDoubleString((char *) ROOM_EXIT(u,i)->key);
	    }
	    else
	    {
	       pBuf->AppendString("");   /* Null Zone name */
	       pBuf->AppendString("");   /* Null name      */
	    }
	 }
	 pBuf->Append16(ROOM_FLAGS(u));
	 pBuf->Append8(ROOM_LANDSCAPE(u));
	 pBuf->Append8(ROOM_RESISTANCE(u));
	 break;

      default:
	 assert(FALSE);
	 break;
   }

   bwrite_affect(pBuf, UNIT_AFFECTED(u), nVersion);

   bwrite_func(pBuf, UNIT_FUNC(u));

   return pBuf->GetLength() - nPos;
}


#ifndef DMSERVER
void swap_in(struct unit_data *u)
{
}
#endif

/* Appends unit 'u' to file 'f'. Name is the unique name */
/* Used only by dmc.                                     */
void write_unit(FILE *f, struct unit_data *u, char *fname)
{
   CByteBuffer *pBuf;
   ubit32 nSizeStart, nStart, nPos;
   ubit32 length;
   ubit32 crc;

   pBuf = &g_FileBuffer;
   pBuf->Clear();

   pBuf->AppendString(fname);          /* Write unique name  */
   pBuf->Append8(UNIT_TYPE(u));    /* Write unit type    */

   nSizeStart = pBuf->GetLength();
   pBuf->Append32(0);              /* Write dummy length */
   pBuf->Append32(0);              /* Write dummy CRC    */

   nStart = pBuf->GetLength();

   length = write_unit_string(pBuf, u);

   /* Calculate the CRC */
   crc = length;

   for (ubit32 i=0; i < length; i++)
     crc += (pBuf->GetData()[nStart + i] << (i % 16));

   nPos = pBuf->GetLength();
   pBuf->SetLength(nSizeStart);

   /* Lets write the calculated length of the unit itself */
   pBuf->Append32(length);

   /* Lets write the calculated length of the unit itself */
   pBuf->Append32(crc);

   pBuf->SetLength(nPos);

   /* Lets write the entire block, including name, type and length info */
   pBuf->FileWrite(f);
}

/* Append template 'tmpl' to file 'f'                    */
/* Used only by dmc. for writing zones                   */
void write_diltemplate(FILE *f, struct diltemplate *tmpl)
{
   CByteBuffer *pBuf;
   ubit8 *b, *bstart;
   ubit32 length, nStart, nPos;

   pBuf = &g_FileBuffer;
   pBuf->Clear();

   pBuf->Append32(0);              /* Write dummy length */
   nStart = pBuf->GetLength();

   bwrite_diltemplate(pBuf, tmpl);

   /* We are now finished, and are positioned just beyond last data byte */
   length = pBuf->GetLength() - nStart;

   nPos = pBuf->GetLength();

   pBuf->SetLength(0);
   pBuf->Append32(length);
   pBuf->SetLength(nPos);

   pBuf->FileWrite(f);
}  

