/* *********************************************************************** *
 * File   : act_change.c                              Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : gnort@daimi.aau.dk                                             *
 *                                                                         *
 * Purpose: Letting the player change all the things that were in the      *
 *          menu and other commands before.                                *
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

#include <stdlib.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "textutil.h"
#include "interpreter.h"


static void chg_wimpy(struct unit_data *ch)
{
   if (IS_SET(CHAR_FLAGS(ch), CHAR_WIMPY))
     send_to_char("You feel brave again.\n\r", ch);
   else
     send_to_char("Ok, you'll flee when death is near.\n\r", ch);

   TOGGLE_BIT(CHAR_FLAGS(ch), CHAR_WIMPY);
}


static void chg_expert(struct unit_data *ch)
{
   if (IS_SET(PC_FLAGS(ch), PC_EXPERT))
     send_to_char("You are now in normal mode.\n\r", ch);
   else
     send_to_char("You are now in expert mode.\n\r", ch);
   
   TOGGLE_BIT(PC_FLAGS(ch), PC_EXPERT);
}


static void chg_brief(struct unit_data *ch)
{
  if (IS_SET(PC_FLAGS(ch), PC_BRIEF))
    send_to_char("Brief mode off.\n\r", ch);
  else
    send_to_char("Brief mode on.\n\r", ch);

  TOGGLE_BIT(PC_FLAGS(ch), PC_BRIEF);
}


static void chg_compact(struct unit_data *ch)
{
  if (IS_SET(PC_FLAGS(ch), PC_COMPACT))
    send_to_char("You are now in the uncompacted mode.\n\r", ch);
  else
    send_to_char("You are now in compact mode.\n\r", ch);

  TOGGLE_BIT(PC_FLAGS(ch), PC_COMPACT);
}


static void chg_peaceful(struct unit_data *ch)
{
   if (IS_SET(CHAR_FLAGS(ch), CHAR_PEACEFUL))
     send_to_char("They will come in peace and leave in pieces.\n\r", ch);
   else
     send_to_char("You will no longer attack aggressors.\n\r", ch);

   TOGGLE_BIT(CHAR_FLAGS(ch), CHAR_PEACEFUL);
}


static void chg_prompt(struct unit_data *ch)
{
   TOGGLE_BIT(PC_FLAGS(ch), PC_PROMPT);
   send_to_char("Prompt changed.\n\r", ch);
}


static void chg_inform(struct unit_data *ch)
{
   TOGGLE_BIT(PC_FLAGS(ch), PC_INFORM);

   if (IS_SET(PC_FLAGS(ch), PC_INFORM))
     send_to_char("You will now get more information.\n\r", ch);
   else
     send_to_char("You will now get less information.\n\r", ch);
}

static void chg_shout(struct unit_data *ch)
{
  if (IS_SET(PC_FLAGS(ch), PC_NOSHOUT))
    send_to_char("You can now hear shouts again.\n\r", ch);
  else
    send_to_char("From now on, you won't hear shouts.\n\r", ch);
     
  TOGGLE_BIT(PC_FLAGS(ch), PC_NOSHOUT);
}


static void chg_tell(struct unit_data *ch)
{
  if (IS_SET(PC_FLAGS(ch), PC_NOTELL))
    send_to_char("You can now hear tells again.\n\r", ch);
  else
    send_to_char("From now on, you won't hear tells.\n\r", ch);
      
  TOGGLE_BIT(PC_FLAGS(ch), PC_NOTELL);
}


static void chg_exits(struct unit_data *ch)
{
  if (IS_SET(PC_FLAGS(ch), PC_EXITS))
    send_to_char("Exit information disabled.\n\r", ch);
  else
    send_to_char("Exit information enabled.\n\r", ch);
      
  TOGGLE_BIT(PC_FLAGS(ch), PC_EXITS);
}

static void chg_columns(struct unit_data *ch, const char *arg)
{
  if (str_is_empty(arg) || !str_is_number(arg))
  {
    send_to_char("You must enter a column number between 40 and 160.\n\r", ch);
    return;
  }

  int width = atoi(arg);

  if ((width < 40) || (width > 160))
  {
     send_to_char("You must enter a column number between 40 and 160.\n\r", ch);
     return;
  }

  act("Your screen width is now $2d columns.", 
      A_ALWAYS, ch, &width, 0, TO_CHAR);

  PC_SETUP_WIDTH(ch) = (ubit8) width;

  MplexSendSetup(CHAR_DESCRIPTOR(ch));
}


static void chg_rows(struct unit_data *ch, const char *arg)
{
  if (str_is_empty(arg) || !str_is_number(arg))
  {
    send_to_char("You must enter a row number between 15 and 60.\n\r", ch);
    return;
  }

  int height = atoi(arg);

  if ((height < 15) || (height > 60))
  {
     send_to_char("You must enter a row number between 15 and 60.\n\r", ch);
     return;
  }

  PC_SETUP_HEIGHT(ch) = (ubit8) height;

  act("Your screen height is $2d rows.", 
      A_ALWAYS, ch, &height, 0, TO_CHAR);

  MplexSendSetup(CHAR_DESCRIPTOR(ch));
}



static void chg_terminal(struct unit_data *ch, const char *arg)
{
  const char *Terminals[] =
  {
    "dumb",
    "tty",
    "ansi",
    NULL
  };

  char buf[1024];
  int n;

  if (PC_SETUP_EMULATION(ch) == TERM_INTERNAL)
  {
     send_to_char("You can not change terminal mode at this time.\n\r", ch);
     return;
  }

  arg = one_argument(arg, buf);

  n = search_block(buf, Terminals, 0);

  switch (n)
  {
    case 0:
     PC_SETUP_EMULATION(ch) = TERM_DUMB;
     send_to_char("Now using no emulation (dumb).\n\r", ch);
     break;

    case 1:
     PC_SETUP_EMULATION(ch) = TERM_TTY;
     send_to_char("Now using TTY emulation.\n\r", ch);
     break;

    case 2:
     PC_SETUP_EMULATION(ch) = TERM_ANSI;
     send_to_char("Now using ANSI emulation.\n\r", ch);
     break;

    default:
     send_to_char("You must enter a terminal type: 'dumb', 'tty' or 'ansi'.\n\r", ch);
     return;
  }

  MplexSendSetup(CHAR_DESCRIPTOR(ch));
}


static void chg_telnet(struct unit_data *ch)
{
  if (PC_SETUP_EMULATION(ch) == TERM_INTERNAL)
  {
     send_to_char("You can not change telnet mode at this time.\n\r", ch);
     return;
  }

  PC_SETUP_TELNET(ch) = !PC_SETUP_TELNET(ch);

  if (PC_SETUP_TELNET(ch))
    act("You are now assumed to be using telnet.", 
	A_ALWAYS, ch, 0, 0, TO_CHAR);
  else
    act("You are now assumed not to be using telnet.", 
	A_ALWAYS, ch, 0, 0, TO_CHAR);

  MplexSendSetup(CHAR_DESCRIPTOR(ch));
}


static void chg_character_echo(struct unit_data *ch)
{
  if (PC_SETUP_EMULATION(ch) == TERM_INTERNAL)
  {
     send_to_char("You can not change echo mode at this time.\n\r", ch);
     return;
  }

  PC_SETUP_ECHO(ch) = !PC_SETUP_ECHO(ch);

  if (PC_SETUP_ECHO(ch))
    act("You will now get all typed characters echoed.", 
	A_ALWAYS, ch, 0, 0, TO_CHAR);
  else
    act("You will now receive no echo characters.", 
	A_ALWAYS, ch, 0, 0, TO_CHAR);

  MplexSendSetup(CHAR_DESCRIPTOR(ch));
}




static void chg_redraw_prompt(struct unit_data *ch)
{
  if (PC_SETUP_EMULATION(ch) == TERM_INTERNAL)
  {
     send_to_char("You can not change redraw mode at this time.\n\r", ch);
     return;
  }

  PC_SETUP_REDRAW(ch) = !PC_SETUP_REDRAW(ch);

  if (PC_SETUP_REDRAW(ch))
    act("You will now get your prompt redrawn.", 
	A_ALWAYS, ch, 0, 0, TO_CHAR);
  else
    act("Your prompt will no longer get redrawn.", 
	A_ALWAYS, ch, 0, 0, TO_CHAR);

  MplexSendSetup(CHAR_DESCRIPTOR(ch));
}


static void chg_echo_say(struct unit_data *ch)
{
  TOGGLE_BIT(PC_FLAGS(ch), PC_ECHO);

  if(IS_SET(PC_FLAGS(ch), PC_ECHO))
    act("You will now get your communications echoed.", A_ALWAYS, ch,
	0, 0, TO_CHAR);
  else
    act("You will no longer get your communications echoed.", A_ALWAYS, ch,
	0, 0, TO_CHAR);
}


void do_change(struct unit_data *ch,char *arg,
	       const struct command_info *cmd)
{
  static const char *args[] =
  { 
    "brief",
    "compact",
    "expert",
    "inform",
    "shout",
    "tell",
    "communications",

    "wimpy",
    "peaceful",

    "prompt",
    "exits",

    "columns",
    "rows",
    "terminal",
    "telnet",
    "character echo",
    "redraw prompt",
    NULL
  };

  char buf[MAX_INPUT_LENGTH];

  if (!IS_PC(ch))
  {
     send_to_char("You don't want to do that.  Trust me.\n\r", ch);
     return;
  }

  if (!CHAR_DESCRIPTOR(ch))
    return;

  if (str_is_empty(arg))
  {
    send_to_char("Usage: change <type> [arguments]\n\r"
		 "<type> being one of:\n\r", ch);

    for (const char **p = args; *p; p++)
    {
       sprintf(buf, "   %s\n\r", *p);
       send_to_char(buf, ch);
    }

    return;
  }

  arg = one_argument(arg, buf);

  switch (search_block(buf, args, 0))
  {
    case 0:
     chg_brief(ch);
     break;

    case 1:
     chg_compact(ch);
     break;

    case 2:
     chg_expert(ch);
     break;

    case 3:
     chg_inform(ch);
     break;

    case 4:
     chg_shout(ch);
     break;

    case 5:
     chg_tell(ch);
     break;

    case 6:
     chg_echo_say(ch);
     break;

    case 7:
     chg_wimpy(ch);
     break;

    case 8:
     chg_peaceful(ch);
     break;

    case 9:
     chg_prompt(ch);
     break;

    case 10:
     chg_exits(ch);
     break;

    case 11:
     chg_columns(ch, arg);
     break;

    case 12:
     chg_rows(ch, arg);
     break;

    case 13:
     chg_terminal(ch, arg);
     break;

    case 14:
     chg_telnet(ch);
     break;

    case 15:
     chg_character_echo(ch);
     break;

    case 16:
     chg_redraw_prompt(ch);
     break;

/*
   case 0:
    send_to_char("Enter a new password:\n\r", ch);
    echo_off(CHAR_DESCRIPTOR(ch));
    snoop_off(ch);
    push_inputfun(ch, new_pwd);
    break;

   case 1:
    send_to_char("Enter a text you'd like others to see when they look "
                 "at you.\n\r", ch);
    send_to_char("Terminate with a '@'.\n\r\n\r", ch);
    push_inputfun(ch, new_description);
    break;

   case 2:
    send_to_char("Enter your email-adress, in the form name@host.\n\r", ch);
    push_inputfun(ch, new_email);
    break;

*/
   default:
    send_to_char("You can't change that, I'm afraid.\n\r", ch);
    break;
  }
}


