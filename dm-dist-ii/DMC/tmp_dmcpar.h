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
    FLAGS = 258,
    END = 259,
    UNKNOWN = 260,
    OBJECTS = 261,
    ZONE = 262,
    RESET = 263,
    ROOMS = 264,
    MOBILES = 265,
    ENDMARK = 266,
    DIL = 267,
    DILPRG = 268,
    STRING = 269,
    SYMBOL = 270,
    PNUM = 271,
    NAMES = 272,
    TITLE = 273,
    DESCR = 274,
    OUT_DESCR = 275,
    IN_DESCR = 276,
    EXTRA = 277,
    KEY = 278,
    OPEN = 279,
    MANIPULATE = 280,
    UFLAGS = 281,
    WEIGHT = 282,
    CAPACITY = 283,
    SPECIAL = 284,
    LIGHT = 285,
    BRIGHT = 286,
    MINV = 287,
    RANDOM = 288,
    DILCOPY = 289,
    ID = 290,
    DATA = 291,
    DURATION = 292,
    FIRSTF = 293,
    TICKF = 294,
    LASTF = 295,
    APPLYF = 296,
    TIME = 297,
    BITS = 298,
    STRINGT = 299,
    EXIT = 300,
    MOVEMENT = 301,
    IN = 302,
    SPELL = 303,
    LINK = 304,
    TO = 305,
    KEYWORD = 306,
    VALUE = 307,
    COST = 308,
    RENT = 309,
    TYPE = 310,
    AFFECT = 311,
    MANA = 312,
    HIT = 313,
    MONEY = 314,
    EXP = 315,
    ALIGNMENT = 316,
    SEX = 317,
    LEVEL = 318,
    HEIGHT = 319,
    RACE = 320,
    POSITION = 321,
    ABILITY = 322,
    WEAPON = 323,
    ATTACK = 324,
    ARMOUR = 325,
    DEFENSIVE = 326,
    OFFENSIVE = 327,
    SPEED = 328,
    DEFAULT = 329,
    ACT = 330,
    RESET_F = 331,
    LIFESPAN = 332,
    CREATORS = 333,
    NOTES = 334,
    HELP = 335,
    WEATHER = 336,
    LOAD = 337,
    FOLLOW = 338,
    MAX = 339,
    ZONEMAX = 340,
    LOCAL = 341,
    INTO = 342,
    NOP = 343,
    EQUIP = 344,
    DOOR = 345,
    PURGE = 346,
    REMOVE = 347,
    COMPLETE = 348,
    SPLUS = 349,
    UMINUS = 350,
    UPLUS = 351
  };
#endif
/* Tokens.  */
#define FLAGS 258
#define END 259
#define UNKNOWN 260
#define OBJECTS 261
#define ZONE 262
#define RESET 263
#define ROOMS 264
#define MOBILES 265
#define ENDMARK 266
#define DIL 267
#define DILPRG 268
#define STRING 269
#define SYMBOL 270
#define PNUM 271
#define NAMES 272
#define TITLE 273
#define DESCR 274
#define OUT_DESCR 275
#define IN_DESCR 276
#define EXTRA 277
#define KEY 278
#define OPEN 279
#define MANIPULATE 280
#define UFLAGS 281
#define WEIGHT 282
#define CAPACITY 283
#define SPECIAL 284
#define LIGHT 285
#define BRIGHT 286
#define MINV 287
#define RANDOM 288
#define DILCOPY 289
#define ID 290
#define DATA 291
#define DURATION 292
#define FIRSTF 293
#define TICKF 294
#define LASTF 295
#define APPLYF 296
#define TIME 297
#define BITS 298
#define STRINGT 299
#define EXIT 300
#define MOVEMENT 301
#define IN 302
#define SPELL 303
#define LINK 304
#define TO 305
#define KEYWORD 306
#define VALUE 307
#define COST 308
#define RENT 309
#define TYPE 310
#define AFFECT 311
#define MANA 312
#define HIT 313
#define MONEY 314
#define EXP 315
#define ALIGNMENT 316
#define SEX 317
#define LEVEL 318
#define HEIGHT 319
#define RACE 320
#define POSITION 321
#define ABILITY 322
#define WEAPON 323
#define ATTACK 324
#define ARMOUR 325
#define DEFENSIVE 326
#define OFFENSIVE 327
#define SPEED 328
#define DEFAULT 329
#define ACT 330
#define RESET_F 331
#define LIFESPAN 332
#define CREATORS 333
#define NOTES 334
#define HELP 335
#define WEATHER 336
#define LOAD 337
#define FOLLOW 338
#define MAX 339
#define ZONEMAX 340
#define LOCAL 341
#define INTO 342
#define NOP 343
#define EQUIP 344
#define DOOR 345
#define PURGE 346
#define REMOVE 347
#define COMPLETE 348
#define SPLUS 349
#define UMINUS 350
#define UPLUS 351

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 102 "dmcpar.y"

  char *str;
  char sym[SYMSIZE + 1];
  char **str_list;
  struct dilargtype *dilarg;
  struct dilargstype *dilargs;
  int num;

#line 258 "y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
