/* *********************************************************************** *
 * File   : act_comm.c                                Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert, ??                                                    *
 *                                                                         *
 * Purpose: say, shout, etc.                                               *
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

/* 24 Aug 1993, gnort:  Introduced more interesting communication.         */
/*                      You can now continue writing on non-full notes     */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "handler.h"
#include "interpreter.h"
#include "db.h"
#include "protocol.h"

#include "spells.h"
#include "textutil.h"
#include "modify.h"

#include "utility.h"

ubit8 g_nShout = 0;

/* extern variables */

extern struct descriptor_data *descriptor_list;


int is_ignored(struct unit_data *ch, struct unit_data *victim)
{
   struct extra_descr_data *pexd;
   char tmp[128];

   if (!IS_PC(ch) || !IS_PC(victim))
     return FALSE;

   pexd = PC_INFO(ch)->find_raw("$ignore");

   if (pexd)
   {
      if (pexd->names.IsName(itoa(PC_ID(victim))))
	return TRUE;
   }

   return FALSE;
}

char *drunk_speech(struct unit_data *ch, const char *str)
{
   static char result[MAX_STRING_LENGTH];
   char *c, b;
   int drunk;

   if (!IS_PC(ch))
     return (char *) str;
   if ((drunk = PC_COND(ch, DRUNK)) <= 4)
     return (char *) str;

   for (c = result; *str; str++)
   {
      b = tolower(*str);

      *c++ = b;

      if (b == 's' && *(str+1) != 's')
      {
	 *c++ = 'c';
	 *c++ = 'h';
      }
      else if (drunk > 8)
      {
	 if (b == 'o' && *(str+1) != 'o')
	 {
	    *c++ = 'h';
	 }
	 else if (drunk > 16)
	 {
	    if (b == 't' && *(str+1) != 't')
	    {
	       *c++ = 'a';
	    }
	    else if (drunk > 22)
	    {
	       if (b == ' ' && *(str+1) != ' ' && drunk > 20 && !number(0,3))
	       {
		  *c++ = '*';
		  *c++ = 'H';
		  *c++ = 'I';
		  *c++ = 'C';
		  *c++ = '*';
		  *c++ = ' ';
	       }
	    }
	 }
      }
   }

   *c = 0;

   return result;
}


void do_emote(struct unit_data *ch, char *arg,
	      const struct command_info *cmd)
{
   if (IS_SET(UNIT_FLAGS(ch), UNIT_FL_BURIED))
   {
      send_to_char("You are buried alive!\n\r",ch);
      return;
   }

   if (str_is_empty(arg))
     send_to_char("What do you want to emote?\n\r", ch);
   else
   {
      if(IS_PC(ch) && !IS_SET(PC_FLAGS(ch), PC_ECHO))
      {
	 act("$1n $2t", A_SOMEONE, ch, arg, 0, TO_ROOM);
	 send_to_char("Ok.\n\r", ch);
      }
      else
	act("$1n $2t", A_SOMEONE, ch, arg, 0, TO_ALL);

      send_done(ch, NULL, NULL, 0, cmd, arg);
   }
}


void do_say(struct unit_data *ch,
	    char *argument,
	    const struct command_info *cmd)
{
   if (CHAR_HAS_FLAG(ch, CHAR_MUTE))
   {
      send_to_char("But, you are mute?!\n\r",ch);
      return;
   }

   if (IS_SET(UNIT_FLAGS(ch), UNIT_FL_BURIED))
   {
      send_to_char("You are buried alive!\n\r",ch);
      return;
   }

   if (str_is_empty(argument))
     send_to_char("What do you want to say?\n\r", ch);
   else
   {
      const char *mode = "say";
      switch(argument[strlen(argument) - 1])
      {
	case '?':
	 mode = "ask";
	 break;

	case '!':
	 mode = "exclaim";
	 break;
      }

      act(COLOUR_COMM "$1n $2ts '$3t'" COLOUR_NORMAL,
	  A_SOMEONE, ch, mode, drunk_speech(ch, argument), TO_ROOM);

      if(IS_PC(ch) && !IS_SET(PC_FLAGS(ch), PC_ECHO))
	send_to_char("Ok.\n\r", ch);
      else
	act("You $2t '$3t'", A_ALWAYS, ch, mode, argument, TO_CHAR);
      
      send_done(ch, NULL, NULL, 0, cmd, argument);
   }
}


void do_shout(struct unit_data *ch, char *argument,
	      const struct command_info *cmd)
{
   struct descriptor_data *i;
   const char *me     = "You shout '$3t'";
   const char *others = COLOUR_COMM "$1n shouts '$2t'" COLOUR_NORMAL;
   int endcost;

   extern ubit8 g_nShout;

   if (IS_PC(ch) && (CHAR_LEVEL(ch) < g_nShout))
   {
      char buf[256];

      act("You can not shout until you are level $2d.",
	  A_ALWAYS, ch, &g_nShout, 0, TO_CHAR);
      return;
   }

   if (CHAR_HAS_FLAG(ch, CHAR_MUTE))
   {
      send_to_char("But, you're mute?!\n\r",ch);
      return;
   }

   if (IS_SET(UNIT_FLAGS(ch), UNIT_FL_BURIED))
   {
      send_to_char("You are buried alive!\n\r",ch);
      return;
   }

   if (IS_PC(ch) && IS_SET(PC_FLAGS(ch), PC_NOSHOUTING))
   {
      send_to_char("You can't shout, your throath is sore!!\n\r", ch);
      return;
   }

   endcost = CHAR_LEVEL(ch) <= START_LEVEL ? 50 : 10;

   if (CHAR_ENDURANCE(ch) < endcost)
   {
      send_to_char("You are too exhausted.\n\r", ch);
      return;
   }

   if (IS_MORTAL(ch))
     CHAR_ENDURANCE(ch) -= endcost;

   if (str_is_empty(argument))
   {
      send_to_char("Shout? Yes! Fine! Shout we must, but WHAT??\n\r", ch);
      return;
   }

   switch(argument[strlen(argument) - 1])
   {
     case '?':
      me     = "You shout asking '$3t'";
      others = COLOUR_COMM "$1n shouts asking '$2t'" COLOUR_NORMAL;
      break;

     case '!':
      me     = "You yell '$3t'";
      others = COLOUR_COMM "$1n yells '$2t'" COLOUR_NORMAL;
      break;
   }
   
   for (i = descriptor_list; i; i = i->next)
     if ((i->character != ch)  && descriptor_is_playing(i) &&
	 !(IS_PC(i->character) && IS_SET(PC_FLAGS(i->character),PC_NOSHOUT)))
       act(others, A_SOMEONE, ch,
	   drunk_speech(ch, argument), i->character, TO_VICT);

   if (IS_PC(ch) && IS_SET(PC_FLAGS(ch), PC_ECHO))
     act(me, A_ALWAYS, ch, 0, argument, TO_CHAR);
   else
     send_to_char("Ok.\n\r", ch);
   
   if (IS_PC(ch) && IS_SET(PC_FLAGS(ch), PC_NOSHOUT))
     send_to_char("You realize that you can't hear people shouting back.\n\r",
		  ch);

   send_done(ch, NULL, NULL, 0, cmd, argument);
}


void do_tell(struct unit_data *ch, char *aaa,
	     const struct command_info *cmd)
{
   struct unit_data *vict = 0;
   struct char_follow_type *f;
   char type;
   const char *others, *me;
   char *c;
   int switched = FALSE;
   char *argument = (char *) aaa;

   if (IS_PC(ch) && IS_SET(PC_FLAGS(ch), PC_NOTELLING))
   {
      send_to_char("Your telepatic ability is lost!\n\r", ch);
      return;
   }

   if (str_is_empty(argument))
   {
      send_to_char("Who do you wish to tell what??\n\r", ch);
      return;
   }

   if (IS_SET(UNIT_FLAGS(ch), UNIT_FL_BURIED))
   {
      send_to_char("You are buried alive!\n\r",ch);
      return;
   }

   argument = skip_spaces(argument);

   if ( (c = strchr(argument, ',')) )
     *c = 0;

   if (strncmp(argument, "group", 5) &&
       !(vict = find_unit(ch, &argument, 0, FIND_UNIT_WORLD)))
   {
      send_to_char("No-one by that name here..\n\r", ch);
      if (c)
	*c = ',';
      return;
   }
   if (c)
   {
      *c = ',';
      argument++;
   }

   if (vict && !IS_CHAR(vict))
   {
      send_to_char("It can't hear you! (It's not alive!)\n\r", ch);
      return;
   }

   if (ch == vict)
   {
      send_to_char("You try to tell yourself something.\n\r", ch);
      return;
   }

   if(!vict)  /* group */
     argument += 5;

   argument = skip_spaces(argument);

   if (str_is_empty(argument))
   {
      send_to_char("What?\n\r", ch);
      return;
   }

   type = argument[strlen(argument) - 1];

   if (vict)
   {
      if (IS_PC(vict) && CHAR_DESCRIPTOR(vict) == NULL)
      {
	 struct descriptor_data *d;
	 
	 for (d = descriptor_list; d; d = d->next)
	 {
	    if (descriptor_is_playing(d) && d->original == vict)
	    {
	       /* AHA! He is switched! */
	       vict = d->character;
	       switched = TRUE;
	       break;
	    }
	 }
      }

      if (CHAR_POS(vict) <= POSITION_SLEEPING ||
	  (IS_PC(vict) && IS_SET(PC_FLAGS(vict), PC_NOTELL)) ||
	  (IS_PC(vict) && (CHAR_DESCRIPTOR(vict) == NULL)))
	act("$3e can't hear you.", A_ALWAYS, ch, 0, vict, TO_CHAR);

      else if (CHAR_DESCRIPTOR(vict) && CHAR_DESCRIPTOR(vict)->editing)
      {
	 act("$3n is busy writing a message, $3e can't hear you!",
	     A_ALWAYS, ch, 0, vict, TO_CHAR);
	 return;
      }
      else if (is_ignored(vict, ch))
      {
	 act("$3n ignores you.",
	     A_ALWAYS, ch, 0, vict, TO_CHAR);
	 return;
      }
      else
      {
	 /* single tell */
	 switch(type)
	 {
	   case '!':
	    me     = "You tell $3n exclaiming '$2t'";
	    others = COLOUR_COMM "$1n tells you exclaiming '$2t'"
	      COLOUR_NORMAL;
	    break;
	   case '?':
	    me     = "You tell $3n asking '$2t'";
	    others = COLOUR_COMM "$1n tells you asking '$2t'"
	      COLOUR_NORMAL;
	    break;
	   default:
	    me     = "You tell $3n '$2t'";
	    others = COLOUR_COMM "$1n tells you '$2t'" COLOUR_NORMAL;
	    break;
	 }

	 if(IS_PC(ch) && !IS_SET(PC_FLAGS(ch), PC_ECHO))
	   send_to_char("Ok.\n\r", ch);
	 else
	   act(me, A_ALWAYS, ch, argument, CHAR_ORIGINAL(vict), TO_CHAR);

	 if (switched)
	   act("SWITCHED TELL TRANSFER to you via $2n!",
	       A_ALWAYS, vict, CHAR_ORIGINAL(vict), 0, TO_CHAR);

	 act(others, A_SOMEONE, ch, drunk_speech(ch, argument), vict, TO_VICT);

	 if (IS_PC(ch) && IS_SET(PC_FLAGS(ch), PC_NOTELL))
	   send_to_char("You realize that you can't hear people telling "
			"back.\n\r", ch);

	 send_done(ch, NULL, vict, 0, cmd, argument, vict);
      }
      return;
   }

   /* group tell */
   if (!IS_SET(CHAR_FLAGS(ch),CHAR_GROUP))
   {
      send_to_char("What group?\n\r", ch);
      return;
   }

   switch(type)
   {
     case '!':
      me     = "You exclaim to the group '$2t'";
      others = COLOUR_COMM "$1n exclaims to the group '$2t'"
	COLOUR_NORMAL;
      break;
     case '?':
      me     = "You ask the group '$2t'";
      others = COLOUR_COMM "$1n asks the group '$2t'" COLOUR_NORMAL;
      break;
     default:
      me     = "You tell the group '$2t'";
      others = COLOUR_COMM "$1n tells the group '$2t'" COLOUR_NORMAL;
      break;
   }

   if (IS_PC(ch) && !IS_SET(PC_FLAGS(ch), PC_ECHO))
     send_to_char("Ok.\n\r", ch);
   else
     act(me, A_ALWAYS, ch, argument, NULL, TO_CHAR);

   /* who's head of group? */
   vict = CHAR_MASTER(ch) ? CHAR_MASTER(ch) : ch;

   if ((ch != vict) && IS_SET(CHAR_FLAGS(vict), CHAR_GROUP))
   {
      act(others, A_SOMEONE, ch,
	  drunk_speech(ch, argument), vict, TO_VICT);
      send_done(ch, NULL, vict, 0, cmd, argument, vict);
   }

   for (f = CHAR_FOLLOWERS(vict); f; f = f->next)
     if (ch != f->follower && IS_SET(CHAR_FLAGS(f->follower), CHAR_GROUP))
     {
	act(others, A_SOMEONE, ch,
	    drunk_speech(ch, argument), f->follower, TO_VICT);
	send_done(ch, NULL, vict, 0, cmd, argument, vict);
     }
}


void do_reply(struct unit_data *ch, char *aaa,
	      const struct command_info *cmd)
{
   do_not_here(ch, aaa, cmd);
   return;
}

void ignore_toggle(struct unit_data *ch,
		   struct unit_data *victim)
{
   struct extra_descr_data *pexd;

   assert(IS_PC(ch));

   if (!IS_PC(victim))
   {
      act("You can't ignore $3n.", A_SOMEONE, ch, NULL, victim, TO_CHAR);
      return;
   }

   pexd = PC_INFO(ch)->find_raw("$ignore");

   if (pexd == NULL)
   {
      PC_INFO(ch) = PC_INFO(ch)->add("$ignore", "");
      pexd = PC_INFO(ch);
   }

   if (pexd->names.IsName(itoa(PC_ID(victim))))
   {
      act("You no longer ignore $2n.", A_ALWAYS, ch, victim, NULL, TO_CHAR);
      pexd->names.RemoveName(itoa(PC_ID(victim)));
   }
   else
   {
      act("You now ignore $2n.", A_ALWAYS, ch, victim, NULL, TO_CHAR);
      pexd->names.AppendName(itoa(PC_ID(victim)));
   }
}


void do_ignore(struct unit_data *ch, char *argument,
	       const struct command_info *cmd)
{
   char tmp[MAX_INPUT_LENGTH];
   class unit_data *victim;

   if (!IS_PC(ch))
   {
      send_to_char("You can't do that\n\r", ch);
      return;
   }

   if (str_is_empty(argument))
   {
      send_to_char("Who do you wish to ignore?\n\r", ch);
      return;
   }

   victim = find_unit(ch, &argument, 0, FIND_UNIT_WORLD);

   if (victim)
     ignore_toggle(ch, victim);
   else
     send_to_char("No such player to ignore.\n\r", ch);
}


void do_whisper(struct unit_data *ch, char *aaa,
		const struct command_info *cmd)
{
   struct unit_data *vict;
   char *arg = (char *) aaa;
   char *c;

   if (CHAR_HAS_FLAG(ch,CHAR_MUTE))
   {
      send_to_char("But, you're mute?!\n\r",ch);
      return;
   }

   if (IS_SET(UNIT_FLAGS(ch), UNIT_FL_BURIED))
   {
      send_to_char("You are buried alive!\n\r",ch);
      return;
   }

   if (str_is_empty(arg))
   {
      send_to_char("Who do you want to whisper to.. and what??\n\r", ch);
      return;
   }

   if ( (c = strchr(arg, ',')) )
     *c = 0;

   if (!(vict = find_unit(ch, &arg, 0, FIND_UNIT_SURRO)))
   {
      send_to_char("No-one by that name here..\n\r", ch);
      if (c)
	*c = ',';
      return;
   }

   if (c)
   {
      *c = ',';
      arg++;
   }

   if (vict == ch)
   {
      act("$1n whispers quietly to $1mself.",A_SOMEONE,ch,0,0,TO_ROOM);
      send_to_char("You can't seem to get your mouth"
		   " close enough to your ear...\n\r",ch);
   }
   else
   {
      arg = skip_spaces(arg);
      if(str_is_empty(arg))
	send_to_char("Whisper what?\n\r", ch);
      else
      {
	 act(COLOUR_COMM "$1n whispers to you, '$2t'" COLOUR_NORMAL
	     , A_SOMEONE, ch, drunk_speech(ch, arg), vict, TO_VICT);

	 if(IS_PC(ch) && !IS_SET(PC_FLAGS(ch), PC_ECHO))
	   send_to_char("Ok.\n\r", ch);
	 else
	   act("You whisper to $3n '$2t'", A_ALWAYS, ch, arg, vict, TO_CHAR);

	 act("$1n whispers something to $3n.",
	     A_SOMEONE, ch, 0, vict, TO_NOTVICT);
	 send_done(ch, NULL, vict, 0, cmd, arg);
      }
   }
}


void do_ask(struct unit_data *ch, char *aaa,
	    const struct command_info *cmd)
{
   struct unit_data *vict;
   char *argument = (char *) aaa;
   char *c;

   if (CHAR_HAS_FLAG(ch, CHAR_MUTE))
   {
      send_to_char("But, you're mute?!\n\r",ch);
      return;
   }

   if (IS_SET(UNIT_FLAGS(ch), UNIT_FL_BURIED))
   {
      send_to_char("You are buried alive!\n\r",ch);
      return;
   }

   if (str_is_empty(argument))
   {
      send_to_char("Who or what do you want to ask??\n\r", ch);
      return;
   }

   if ( (c = strchr(argument, ',')) )
     *c = 0;

   if (!(vict = find_unit(ch, &argument, 0, FIND_UNIT_SURRO)))
   {
      act(COLOUR_COMM "$1n asks '$3t'" COLOUR_NORMAL,
	  A_SOMEONE, ch, 0, drunk_speech(ch, argument), TO_ROOM);
      if(IS_PC(ch) && !IS_SET(PC_FLAGS(ch), PC_ECHO))
	send_to_char("Ok.\n\r", ch);
      else
	act("You ask '$3t'", A_ALWAYS, ch, 0, argument, TO_CHAR);

      send_done(ch, NULL, vict, 0, cmd, argument);

      if ( c )
	*c = ',';

      return;
   }

   if ( c )
   {
      *c = ',';
      argument++;
   }

   if (vict == ch)
   {
      act("$1n quietly asks $1mself a question.",A_SOMEONE,ch,0,0,TO_ROOM);
      send_to_char("You think about it for a while...\n\r", ch);
   }
   else
   {
      if (str_is_empty(argument))
	send_to_char("What?\n\r", ch);
      else
      {
	 argument = skip_spaces(argument);

	 act(COLOUR_COMM "$1n asks you '$2t'" COLOUR_NORMAL,
	     A_SOMEONE, ch, drunk_speech(ch, argument), vict, TO_VICT);

	 if(IS_PC(ch) && !IS_SET(PC_FLAGS(ch), PC_ECHO))
	   send_to_char("Ok.\n\r", ch);
	 else
	   act("You ask $2n '$3t'", A_ALWAYS, ch, vict, argument, TO_CHAR);

	 act(COLOUR_COMM "$1n asks $3n '$2t'" COLOUR_NORMAL,
	     A_SOMEONE, ch, drunk_speech(ch, argument), vict, TO_NOTVICT);

	 send_done(ch, NULL, vict, 0, cmd, argument);
      }
   }
}


#define MAX_NOTE_LENGTH 2000      /* arbitrary */

void do_write(struct unit_data *ch, char *aaa,
	      const struct command_info *cmd)
{
   struct extra_descr_data *exd;
   struct unit_data *paper = NULL;
   char *argument = (char *) aaa;

   void interpreter_string_add(struct descriptor_data *d, char *str);

   if (!CHAR_DESCRIPTOR(ch))
     return;
  
   if (str_is_empty(argument))
   {
      send_to_char("Write on what?\n\r", ch);
      return;
   }

   if ((paper = find_unit(ch, &argument, 0, FIND_UNIT_IN_ME)) == NULL)
   {
      send_to_char("You've got no such thing to write on.\n\r", ch);
      return;
   }

#ifdef SUSPEKT
   /* Too big a hassle! People cant figure it out... could improve it, or
      just leave it as is. */

   if (IS_MORTAL(ch))		/* Immortals needn't use pens ;-) */
   {
      if (str_is_empty(argument))
      {
	 send_to_char("What do you wish to write with?\n\r", ch);
	 return;
      }
    
      if ((pen = find_unit(ch, &argument, 0, FIND_UNIT_IN_ME)) == NULL)
      {
	 send_to_char("No such thing to write with.\n\r", ch);
	 return;
      }

      if (!IS_OBJ(pen) || OBJ_TYPE(pen) != ITEM_PEN)
      {
	 send_to_char("You can't write with that.\n\r", ch);
	 return;
      }
   }
#endif

   if (!IS_OBJ(paper) || OBJ_TYPE(paper) != ITEM_NOTE)
   {
      send_to_char("You can't write on that.\n\r", ch);
      return;
   }

   if ((exd = unit_find_extra(UNIT_NAME(paper), paper)))
   {
      if (exd->descr.Length() > MAX_NOTE_LENGTH - 10)
      {
	 act("There's simply no room for anything more on the note!",
	     A_ALWAYS, ch, 0, 0, TO_CHAR);
	 return;
      }
      send_to_char("There's something written on it already:\n\r\n\r", ch);
      send_to_char(exd->descr.String(), ch);
      return;
   }

   act("You begin to jot down a note on $2n.",
       A_ALWAYS, ch, paper, 0, TO_CHAR);
   act("$1n begins to jot down a note.",
       A_HIDEINV, ch, 0,0,TO_ROOM);

   UNIT_EXTRA_DESCR(paper) = UNIT_EXTRA_DESCR(paper)->add((char *) NULL, NULL);

   CHAR_DESCRIPTOR(ch)->editref  = UNIT_EXTRA_DESCR(paper);
   CHAR_DESCRIPTOR(ch)->postedit = edit_extra;
   CHAR_DESCRIPTOR(ch)->editing  = paper;

   set_descriptor_fptr(CHAR_DESCRIPTOR(ch), interpreter_string_add, TRUE);
}
