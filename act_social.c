/* *********************************************************************** *
 * File   : act_social.c                              Part of Valhalla MUD *
 * Version: 3.00                                                           *
 * Author : gnort@daimi.aau.dk                                             *
 *                                                                         *
 * Purpose: Handling the social commands                                   *
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
/* Tue  6-Jul-93   HHS: added exchangable lib dir
 * Sun 12-Mar-95 gnort: changed from binary searched array to trie
 *			because old method was too CPU intensive
 *			(as always, this meant I had to *think* to write
 *			sprint_social!!!1!)
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "handler.h"
#include "db.h"
#include "textutil.h"
#include "utility.h"
#include "interpreter.h"
#include "trie.h"
#include "movement.h"

/* externs */
extern char libdir[];    /* from dikumud.c */


struct social_msg
{
   char *cmd_str;     /* "hug" "kiss" etc */
   ubit8 hide_flag;   /* Should we hideinvis? */
   ubit8 min_pos;     /* Minimum position of char */
   ubit8 vic_min_pos; /* Minimum position of victim */
   ubit8 level;       /* Is this a restricted social? (silly concept) */

   /* No argument was supplied */
   char *char_no_arg;
   char *others_no_arg;

   /* An argument was there, and a victim was found */
   char *char_found;   /* if NULL, read no further, ignore args */
   char *others_found;
   char *vict_found;

   /* An argument was there, but no victim was found */
   char *not_found;

   /* The victim turned out to be the character */
   char *char_auto;
   char *others_auto;
};


static struct trie_type *soc_trie;


static char *fread_action(FILE *fl)
{
   char buf[512];

   char *m_tmp = fgets(buf, sizeof buf, fl);

   if (feof(fl))
   {
      perror("fread_action");
      exit(1);
   }

   if (*buf == '#')
     return NULL;

   buf[strlen(buf) - 1] = '\0';

   return str_dup(buf);
}


static int str_to_min_pos(char *str)
{
   int pos = 0;

   if (str_ccmp(str, "dead") == 0)
     pos = POSITION_DEAD;
   else if (str_ccmp(str, "sleep") == 0)
     pos = POSITION_SLEEPING;
   else if (str_ccmp(str, "rest") == 0)
     pos = POSITION_RESTING;
   else if (str_ccmp(str, "sit") == 0)
     pos = POSITION_SITTING;
   else if (str_ccmp(str, "fight") == 0)
     pos = POSITION_FIGHTING;
   else if (str_ccmp(str, "stand") == 0)
     pos = POSITION_STANDING;
   else
     error(HERE, "Illegal position in actions-file: %s", str);

   return pos;
}


static int str_to_hide_flag(char *str)
{
   int flag = 0;

   if (str_ccmp(str, "someone") == 0)
     flag = A_SOMEONE;
   else if (str_ccmp(str, "hideinv") == 0)
     flag = A_HIDEINV;
   else if (str_ccmp(str, "always") == 0)
     flag = A_ALWAYS;
   else
     error(HERE, "Illegal hide-flag in actions-file: %s", str);

   return flag;
}


static int soc_sort_cmp(struct social_msg *dat1, struct social_msg *dat2)
{
   return strcmp(dat1->cmd_str, dat2->cmd_str);
}


/*  Boot builds a array of the socials first, so we can sort them before
 *  putting them into the trie.
 */
void boot_social_messages(void)
{
   FILE *fl;
   char cmd[80], hide[80], min_pos[80], vic_min_pos[80], buf[256];
   int level;

   struct social_msg *list = NULL;
   int list_elms = 0, list_size = 0;

   if ((fl = fopen(str_cc(libdir, SOCMESS_FILE), "r")) == NULL)
   {
      perror("boot_social_messages");
      exit(1);
   }

   for (;;)
   {
      do
      {
	 if (fgets(buf, sizeof buf, fl) == NULL)
	 {
	    if (feof(fl))
	    {
	       int i;

	       fclose(fl);

	       /* Release mem not used. */
	       RECREATE(list, struct social_msg, list_elms);

	       qsort(list, list_elms, sizeof(struct social_msg),
		     (int(*)(const void *, const void *)) soc_sort_cmp);

	       soc_trie = NULL;

	       for (i = 0; i < list_elms; i++)
		 soc_trie = add_trienode(list[i].cmd_str, soc_trie);

	       qsort_triedata(soc_trie);

	       for (i = 0; i < list_elms; i++)
		 set_triedata(list[i].cmd_str, soc_trie, &list[i], FALSE);

	       return; /* list is not freed, as it it used for the trie */
	    }

	    perror("boot_social_messages");
	    exit(1);
	 }
      }
      /* Skip empty lines & comments */
      while (buf[0] == '\n' || buf[0] == '#');

      sscanf(buf, "%s %s %s %s %d", cmd, hide, min_pos, vic_min_pos, &level);

      /* alloc new cells */
      if (list == NULL)
      {
	 list_size = 10;
	 CREATE(list, struct social_msg, list_size);
      }
      else if (list_size == list_elms)
      {
	 list_size *= 2;
	 RECREATE(list, struct social_msg, list_size);
      }

      list[list_elms].cmd_str     = str_dup(cmd);
      list[list_elms].hide_flag   = str_to_hide_flag(hide);
      list[list_elms].min_pos     = str_to_min_pos(min_pos);
      list[list_elms].vic_min_pos = str_to_min_pos(vic_min_pos);
      list[list_elms].level       = MIN(255, level);

      list[list_elms].char_no_arg   = fread_action(fl);
      list[list_elms].others_no_arg = fread_action(fl);

      list[list_elms].char_found    = fread_action(fl);

      /* if no char_found, the rest is to be ignored */
      if (list[list_elms].char_found)
      {
	 list[list_elms].others_found  = fread_action(fl);
	 list[list_elms].vict_found    = fread_action(fl);
	 list[list_elms].not_found     = fread_action(fl);
	 list[list_elms].char_auto     = fread_action(fl);
	 list[list_elms].others_auto   = fread_action(fl);
      }

      list_elms++;
   }
}


/* is cmd an nonabbreviated social-string? */
bool cmd_is_a_social(char *cmd, int complete)
{
   struct social_msg *action;

   if (complete)
     return ((action = (struct social_msg *) search_trie(cmd, soc_trie))
	     && str_ccmp(action->cmd_str, cmd) == 0);
   else
     return search_trie(cmd, soc_trie) != NULL;
}


bool perform_social(struct unit_data *ch, char *arg, const command_info *cmd)
{
   struct social_msg *action;
   char *oarg = arg;

   action = (struct social_msg *) search_trie(cmd->cmd_str, soc_trie);

   if ((action == NULL) || (action->level > CHAR_LEVEL(ch)))
     return FALSE;
   else if (CHAR_POS(ch) < action->min_pos)
     wrong_position(ch);
   else if (str_is_empty(arg) || !action->char_found)
   {
      act(action->char_no_arg,   A_SOMEONE,         ch, 0, 0, TO_CHAR);
      act(action->others_no_arg, action->hide_flag, ch, 0, 0, TO_ROOM);
      send_done(ch, NULL, NULL, 0, cmd, oarg);
   }
   else
   {
      struct unit_data *vict = find_unit(ch, &arg, 0, FIND_UNIT_SURRO);

      if (vict == NULL || !IS_CHAR(vict))
	act(action->not_found, A_SOMEONE, ch, 0, 0, TO_CHAR);
      else if (vict == ch)
      {
	 act(action->char_auto,   A_SOMEONE,         ch, 0, 0, TO_CHAR);
	 act(action->others_auto, action->hide_flag, ch, 0, 0, TO_ROOM);
	 send_done(ch, NULL, NULL, 0, cmd, oarg);
      }
      else if (CHAR_POS(vict) < action->vic_min_pos)
	act("$2n is not in a proper position for that.", 
	    A_SOMEONE, ch, vict, 0, TO_CHAR);
      else
      {
	 act(action->char_found,   A_SOMEONE,         ch, 0, vict, TO_CHAR);
	 act(action->others_found, action->hide_flag, ch, 0, vict, TO_NOTVICT);
	 act(action->vict_found,   action->hide_flag, ch, 0, vict, TO_VICT);
	 send_done(ch, NULL, vict, 0, cmd, oarg);
      }
   }
   return TRUE;
}


/* Eeek, not trivial!
 *
 * b:   The array to hold the generated string
 * t:   The trie to search
 * no:  Pointer to the integer to control newlines
 * cur: The currently examined string
 * idx: Index to the permutable char of cur
 */
static int sprint_social(char *b, struct trie_type *t, int *no,
			char *cur, int idx)
{
   struct social_msg *sm;
   struct trie_type *t2;
   int i, count = 0;

   if (t)
   {
      cur[idx + 1] = '\0'; /* Make sure cur is correctly nil terminated */

      for (i = 0; i < t->size; i++)
      {
	 t2       = t->nexts[i].t;
	 cur[idx] = t->nexts[i].c; /* extend the current string */

	 if ((sm = (struct social_msg *) t2->data)
	     /* also make sure it is an unabbreviated social-string! */
	     && strcmp(sm->cmd_str, cur) == 0)
	 {
	    sprintf(b, "%-15s", sm->cmd_str);
	    if (++*no % 5 == 0)
	      strcat(b, "\n\r");
	    TAIL(b);

	    count++;
	 }
	 count += sprint_social(b, t2, no, cur, idx + 1);
	 TAIL(b);
      }
   }

   return count;
}


void do_socials(struct unit_data *ch, char *arg,
		const struct command_info *cmd)
{
   char buf[MAX_STRING_LENGTH], cur[50];
   int no = 0;

   if (!IS_PC(ch))
     return;

   send_to_char("The following social commands are available:\n\r\n\r", ch);
 
   if (sprint_social(buf, soc_trie, &no, cur, 0))
   {
      strcat(buf, "\n\r");
      page_string(CHAR_DESCRIPTOR(ch), buf);
   }
   else
     send_to_char("  None!\n\r", ch);
}


void do_insult(struct unit_data *ch, char *arg,
	       const struct command_info *cmd)
{
   const char *insult;
   struct unit_data *victim;

   if (str_is_empty(arg))
   {
      send_to_char("Surely you don't want to insult everybody.\n\r", ch);
      return;
   }

   victim = find_unit(ch, &arg, 0, FIND_UNIT_SURRO);

   if (victim == NULL || !IS_CHAR(victim))
   {
      send_to_char("No one by that name here.\n\r", ch);
      return;
   }

   if (victim == ch)
   {
      backdoor(ch, arg, cmd);

      send_to_char("You feel insulted.\n\r", ch);
   }
   else
   {
      /* Add new cases ad libitum/nauseum */
      switch (number(0, 2))
      {
	case 0:
	 if (CHAR_SEX(ch) == SEX_MALE)
	 {
	    if (CHAR_SEX(victim) == SEX_MALE)
	      insult = "$1n accuses you of fighting like a woman!";
	    else
	      insult = "$1n says that women can't fight.";
	 }
	 else /* female (or neutral!) */
	 {
	    if (CHAR_SEX(victim) == SEX_MALE)
	      insult = "$1n accuses you of having the smallest.... (brain?)";
	    else
	      insult = "$1n tells you that you'd loose a beauty contest "
		       "against a troll!";
	 }
	 break;

	case 1:
	 insult = "$1n calls your mother a bitch!";
	 break;

	default:
	 insult = "$1n tells you to get lost!";
	 break;
      }

      act("You insult $3n.",  A_SOMEONE, ch, 0, victim, TO_CHAR);
      act("$1n insults $3n.", A_SOMEONE, ch, 0, victim, TO_NOTVICT);
      act(insult,             A_SOMEONE, ch, 0, victim, TO_VICT);
   }
}


struct pose_type
{
   int level;           /* minimum level for poser */
   char *poser_msg[4];  /* message to poser        */
   char *room_msg[4];   /* message to room         */
};

#define MAX_POSES 60

static struct pose_type pose_messages[MAX_POSES];


void boot_pose_messages(void)
{
   FILE *fl;
   sbit16 counter, cls;

   return; /* SUSPEKT  no reason to boot these, eh? */

   if (!(fl = fopen(str_cc(libdir, POSEMESS_FILE), "r")))
   {
      perror("boot_pose_messages");
      exit(0);
   }

   for (counter = 0; ;counter++)
   {
      int m_tmp = fscanf(fl, " %d ", &pose_messages[counter].level);

      if (pose_messages[counter].level < 0)
	break;
      for (cls = 0; cls < 4; cls++)
      {
	 pose_messages[counter].poser_msg[cls] = fread_action(fl);
	 pose_messages[counter].room_msg[cls]  = fread_action(fl);
      }
   }

   fclose(fl);
}


void do_pose(struct unit_data *ch, char *argument,
	     const struct command_info *cmd)
{
   send_to_char("Sorry Buggy command.\n\r", ch);
   return;

#ifdef SUSPEKT
   struct pose_type *to_pose;
   int counter = 0;

   if (CHAR_LEVEL(ch) < pose_messages[0].level || IS_NPC(ch))
   {
      send_to_char("You can't do that.\n\r", ch);
      return;
   }

   while (pose_messages[counter].level < CHAR_LEVEL(ch)
	  && 0 < pose_messages[counter].level)
     counter++;

   to_pose = &pose_messages[number(0, counter - 1)];
     
   act(to_pose->poser_msg[0], A_SOMEONE, ch, 0, 0, TO_CHAR);
   act(to_pose->room_msg[0],  A_SOMEONE, ch, 0, 0, TO_ROOM);
#endif
}
