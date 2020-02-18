/* *********************************************************************** *
 * File   : interpreter.h                             Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : ???                                                            *
 *                                                                         *
 * Purpose: Prototypes and types for the command interpreter module.       *
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

#ifndef _MUD_INTERPRETER_H
#define _MUD_INTERPRETER_H

#include "dil.h"

int char_is_playing(struct unit_data *u);
int descriptor_is_playing(struct descriptor_data *d);
void set_descriptor_fptr(struct descriptor_data *d,
			 void (*fptr)(struct descriptor_data *, char*),
			 ubit1 call);
void descriptor_interpreter(struct descriptor_data *d, char *arg);
void interpreter_string_add(struct descriptor_data *d, char *str);

struct spec_arg
{
   struct unit_data *owner;     /* Who is this?                       */
   struct unit_data *activator; /* Who performed the operation        */
   struct unit_data *medium;    /* Possibly what is used in operation */
   struct unit_data *target;    /* Possible target of operation       */

   struct command_info *cmd;
   struct unit_fptr *fptr; /* The fptr is allowed to be modified, destroyed */

   int *pInt;  /* Potential int to modify */

   const char *arg;
   ubit16 mflags; /* Would like to make constant, but then can't define.. */
};

struct command_info
{
   ubit8 combat_speed;  /* The speed of a combat command         */
   ubit8 combat_buffer; /* Use the combat speed / buffer system? */

   const char *cmd_str;

   sbit32 no;

   ubit8 minimum_position;

   void (*cmd_fptr) (struct unit_data *ch, char *arg,
		     const struct command_info *c);

   ubit8 minimum_level;
   ubit8 log_level;  /* For logging certain immortal commands */

   struct diltemplate *tmpl;/* Perhaps a DIL template...         */

   /* Also put noise/provokation and impact values here            */

   /* Example: if ((cmd->noise > 50) || (cmd->impact > 7))         */
   /*          { wake(dragon); .... }                              */
   /* Example: if (cmd->provokation < 0) { do_kiss(maid, ch); .. } */
};

/* Bitmasks to determine what kind of messages is to be send
   to a special function. */

struct unit_function_array_type
{
   const char *name;
   int (*func) (struct spec_arg *sarg);
   int save_w_d;  /* May it be saved if it has data? True/false */
   ubit16 sfb;    /* what kind of messages should be send */
   sbit16 tick;   /* Default tick count */
};


extern struct command_info cmd_auto_enter;
extern struct command_info cmd_auto_tick;
extern struct command_info cmd_auto_extract;
extern struct command_info cmd_auto_death;
extern struct command_info cmd_auto_combat;
extern struct command_info cmd_auto_unknown;
extern struct command_info cmd_auto_save;
extern struct command_info cmd_auto_msg;
extern struct command_info cmd_auto_damage;
extern struct command_info cmd_a_social;

/* To check for commands by string */
ubit1 is_command(const struct command_info *cmd, const char *str);

/* Check to see if typed command is abbreviated */
ubit1 cmd_is_abbrev(struct unit_data *ch, const struct command_info *cmd);

/* Interpreter routines */
void wrong_position(struct unit_data *ch);
void command_interpreter(struct unit_data *ch, char *argument);
void argument_interpreter(const char *argument,
			  char *first_arg, char *second_arg );
void half_chop(char *string, char *arg1, char *arg2);

/* The routine to check for special routines */

int unit_function_scan(struct unit_data *u, struct spec_arg *sarg);
int function_activate(struct unit_data *u, struct spec_arg *sarg);
#ifdef DMSERVER
int basic_special(struct unit_data *ch, struct spec_arg *sarg, ubit16 mflt,
		  struct unit_data *extra_target = NULL);
#endif
int send_preprocess(struct unit_data *ch, const struct command_info *cmd,
		    char *arg);
void send_done(struct unit_data *activator, struct unit_data *medium,
	       struct unit_data *target, int i, const struct command_info *cmd,
	       const char *arg, struct unit_data *extra_target = NULL);
int send_ack(struct unit_data *activator,
	     struct unit_data *medium,
	     struct unit_data *target,
	     int *i,
	     const struct command_info *cmd,
	     const char *arg,
	     struct unit_data *extra_target);
int send_message(struct unit_data *ch, char *arg);
int send_death(struct unit_data *ch);
int send_combat(struct unit_data *ch);
int send_save_to(struct unit_data *from, struct unit_data *to);

#include "spec_assign.h"

void do_ignore(struct unit_data *,char *, const struct command_info *);
void do_reply(struct unit_data *,char *, const struct command_info *);
void do_expert(struct unit_data *,char *, const struct command_info *);
void do_manifest(struct unit_data *,char *, const struct command_info *);
void do_sacrifice(struct unit_data *,char *, const struct command_info *);
void do_pray(struct unit_data *,char *, const struct command_info *);
void do_exit(struct unit_data *,char *, const struct command_info *);
void do_look(struct unit_data *,char *, const struct command_info *);
void do_read(struct unit_data *,char *, const struct command_info *);
void do_say(struct unit_data *,char *, const struct command_info *);
void do_snoop(struct unit_data *,char *, const struct command_info *);
void do_delete(struct unit_data *,char *, const struct command_info *);
void do_insult(struct unit_data *,char *, const struct command_info *);
void do_quit(struct unit_data *,char *, const struct command_info *);
void do_help(struct unit_data *,char *, const struct command_info *);
void do_who(struct unit_data *,char *, const struct command_info *);
void do_emote(struct unit_data *,char *, const struct command_info *);
void do_echo(struct unit_data *,char *, const struct command_info *);
void do_trans(struct unit_data *,char *, const struct command_info *);
void do_reset(struct unit_data *,char *, const struct command_info *);
void do_kill(struct unit_data *,char *, const struct command_info *);
void do_stand(struct unit_data *,char *, const struct command_info *);
void do_sit(struct unit_data *,char *, const struct command_info *);
void do_rest(struct unit_data *,char *, const struct command_info *);
void do_sleep(struct unit_data *,char *, const struct command_info *);
void do_wake(struct unit_data *,char *, const struct command_info *);
void do_force(struct unit_data *,char *, const struct command_info *);
void do_get(struct unit_data *,char *, const struct command_info *);
void do_drop(struct unit_data *,char *, const struct command_info *);
void do_news(struct unit_data *,char *, const struct command_info *);
void do_score(struct unit_data *,char *, const struct command_info *);
void do_guild(struct unit_data *,char *, const struct command_info *);
void do_status(struct unit_data *,char *, const struct command_info *);
void do_inventory(struct unit_data *,char *, const struct command_info *);
void do_equipment(struct unit_data *,char *, const struct command_info *);
void do_shout(struct unit_data *,char *, const struct command_info *);
void do_not_here(struct unit_data *,char *, const struct command_info *);
void do_tell(struct unit_data *,char *, const struct command_info *);
void do_wear(struct unit_data *,char *, const struct command_info *);
void do_wield(struct unit_data *,char *, const struct command_info *);
void do_grab(struct unit_data *,char *, const struct command_info *);
void do_remove(struct unit_data *,char *, const struct command_info *);
void do_put(struct unit_data *,char *, const struct command_info *);
void do_shutdown(struct unit_data *,char *, const struct command_info *);
void do_reboot(struct unit_data *,char *, const struct command_info *);
void do_execute(struct unit_data *,char *, const struct command_info *);
void do_save(struct unit_data *,char *, const struct command_info *);
void do_hit(struct unit_data *,char *, const struct command_info *);
void do_set(struct unit_data *,char *, const struct command_info *);
void do_setskill(struct unit_data *,char *, const struct command_info *);
void do_give(struct unit_data *,char *, const struct command_info *);
void do_wstat(struct unit_data *,char *, const struct command_info *);
void do_setskill(struct unit_data *,char *, const struct command_info *);
void do_time(struct unit_data *,char *, const struct command_info *);
void do_weather(struct unit_data *,char *, const struct command_info *);
void do_load(struct unit_data *,char *, const struct command_info *);
void do_purge(struct unit_data *,char *, const struct command_info *);
void do_ideatypobug(struct unit_data *,char *, const struct command_info *);
void do_whisper(struct unit_data *,char *, const struct command_info *);
void do_cast(struct unit_data *,char *, const struct command_info *);
void do_at(struct unit_data *,char *, const struct command_info *);
void do_goto(struct unit_data *,char *, const struct command_info *);
void do_ask(struct unit_data *,char *, const struct command_info *);
void do_drink(struct unit_data *,char *, const struct command_info *);
void do_eat(struct unit_data *,char *, const struct command_info *);
void do_pour(struct unit_data *,char *, const struct command_info *);
void do_sip(struct unit_data *,char *, const struct command_info *);
void do_taste(struct unit_data *,char *, const struct command_info *);
void do_order(struct unit_data *,char *, const struct command_info *);
void do_follow(struct unit_data *,char *, const struct command_info *);
void do_rent(struct unit_data *,char *, const struct command_info *);
void do_offer(struct unit_data *,char *, const struct command_info *);
void do_advance(struct unit_data *,char *, const struct command_info *);
void do_close(struct unit_data *,char *, const struct command_info *);
void do_open(struct unit_data *,char *, const struct command_info *);
void do_lock(struct unit_data *,char *, const struct command_info *);
void do_unlock(struct unit_data *,char *, const struct command_info *);
void do_exits(struct unit_data *,char *, const struct command_info *);
void do_enter(struct unit_data *,char *, const struct command_info *);
void do_leave(struct unit_data *,char *, const struct command_info *);
void do_write(struct unit_data *,char *, const struct command_info *);
void do_flee(struct unit_data *,char *, const struct command_info *);
void do_sneak(struct unit_data *,char *, const struct command_info *);
void do_hide(struct unit_data *,char *, const struct command_info *);
void do_backstab(struct unit_data *,char *, const struct command_info *);
void do_pick(struct unit_data *,char *, const struct command_info *);
void do_steal(struct unit_data *,char *, const struct command_info *);
void do_bash(struct unit_data *,char *, const struct command_info *);
void do_rescue(struct unit_data *,char *, const struct command_info *);
void do_kick(struct unit_data *,char *, const struct command_info *);
void do_search(struct unit_data *,char *, const struct command_info *);
void do_examine(struct unit_data *,char *, const struct command_info *);
void do_info(struct unit_data *,char *, const struct command_info *);
void do_users(struct unit_data *,char *, const struct command_info *);
void do_where(struct unit_data *,char *, const struct command_info *);
void do_level(struct unit_data *,char *, const struct command_info *);
void do_reroll(struct unit_data *,char *, const struct command_info *);
void do_brief(struct unit_data *,char *, const struct command_info *);
void do_wizlist(struct unit_data *,char *, const struct command_info *);
void do_consider(struct unit_data *,char *, const struct command_info *);
void do_group(struct unit_data *,char *, const struct command_info *);
void do_restore(struct unit_data *,char *, const struct command_info *);
void do_ban(struct unit_data *,char *, const struct command_info *);
void do_switch(struct unit_data *,char *, const struct command_info *);
void do_quaff(struct unit_data *,char *, const struct command_info *);
void do_recite(struct unit_data *,char *, const struct command_info *);
void do_use(struct unit_data *,char *, const struct command_info *);
void do_pose(struct unit_data *,char *, const struct command_info *);
void do_noshout(struct unit_data *,char *, const struct command_info *);
void do_change(struct unit_data *,char *, const struct command_info *);
void do_prompt(struct unit_data *,char *, const struct command_info *);
void do_echosay(struct unit_data *,char *, const struct command_info *);
void do_notell(struct unit_data *,char *, const struct command_info *);
void do_wizhelp(struct unit_data *,char *, const struct command_info *);
void do_credits(struct unit_data *,char *, const struct command_info *);
void do_compact(struct unit_data *,char *, const struct command_info *);
void do_dig(struct unit_data *,char *, const struct command_info *);
void do_bury(struct unit_data *,char *, const struct command_info *);
void do_turn(struct unit_data *,char *, const struct command_info *);
void do_diagnose(struct unit_data *,char *, const struct command_info *);
void do_appraise(struct unit_data *,char *, const struct command_info *);
void do_ventriloquate(struct unit_data *,char *, const struct command_info *);
void do_aid(struct unit_data *,char *, const struct command_info *);
void do_light(struct unit_data *,char *, const struct command_info *);
void do_extinguish(struct unit_data *,char *, const struct command_info *);
void do_wimpy(struct unit_data *,char *, const struct command_info *);
void do_peaceful(struct unit_data *,char *, const struct command_info *);
void do_wizinv(struct unit_data *,char *, const struct command_info *);
void do_drag(struct unit_data *,char *, const struct command_info *);
void do_path(struct unit_data *,char *, const struct command_info *);
void do_freeze(struct unit_data *,char *, const struct command_info *);
void do_file(struct unit_data *,char *, const struct command_info *);
void do_knock(struct unit_data *,char *, const struct command_info *);
void do_message(struct unit_data *,char *, const struct command_info *);
void do_broadcast(struct unit_data *,char *, const struct command_info *);
void do_wiz(struct unit_data *,char *, const struct command_info *);
void do_title(struct unit_data *,char *, const struct command_info *);
void do_split(struct unit_data *,char *, const struct command_info *);

void do_crash(struct unit_data *,char *, const struct command_info *);
void do_wizlock(struct unit_data *,char *, const struct command_info *);

void do_practice(struct unit_data *,char *, const struct command_info *);
void do_move(struct unit_data *, char *, const struct command_info *);

void do_ride(struct unit_data *, char *, const struct command_info *);
void do_sail(struct unit_data *, char *, const struct command_info *);

void do_quests(struct unit_data *, char *, const struct command_info *);
void do_decapitate(struct unit_data *,char *, const struct command_info *);

void do_purse(struct unit_data *,char *, const struct command_info *);
void do_makemoney(struct unit_data *,char *, const struct command_info *);
void do_verify(struct unit_data *,char *, const struct command_info *);

void do_commands(struct unit_data *,char *, const struct command_info *);
void do_socials(struct unit_data *,char *, const struct command_info *);

void do_boards(struct unit_data *,char *, const struct command_info *);
void do_reimb(struct unit_data *,char *, const struct command_info *);
void do_finger(struct unit_data *,char *, const struct command_info *);
void do_account(struct unit_data *,char *, const struct command_info *);
void do_kickit(struct unit_data *,char *, const struct command_info *);
void do_corpses(struct unit_data *,char *, const struct command_info *);
void do_inform(struct unit_data *,char *, const struct command_info *);
void do_areas(struct unit_data *,char *, const struct command_info *);

#endif /* _MUD_INTERPRETER_H */
