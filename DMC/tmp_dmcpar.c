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
#line 39 "dmcpar.y"

#include <stdio.h>
#include <ctype.h>

#include "dmc.h"
#include "dil.h"
#include "utils.h"
#include "textutil.h"
#include "money.h"
#include "db_file.h"

extern char *yytext; /*  MUST be changed to
		      *  `extern char yytext[]'
		      *  if you use lex instead of flex.
		      */
int yylex(void);


#define ZON_DIR_CONT 0
#define ZON_DIR_NEST 1
#define ZON_DIR_UNNEST 2

#define yyerror(dum) syntax(dum)

extern char cur_filename[];
extern int linenum;
extern struct zone_info zone;
extern int nooutput;
struct unit_data *cur;
struct extra_descr_data *cur_extra;
struct reset_command *cur_cmd;
struct unit_affected_type *cur_aff;
struct unit_fptr *cur_func;
struct diltemplate *cur_tmpl;
struct dilprg *cur_prg;

int myi, cur_ex, errcon = 0;

/* Temporary data for stringlists */
int str_top;
char *str_list[50];
char empty_ref[] = {'\0', '\0'};
int istemplate;
char **tmplnames=NULL;

struct dilargstype dilargs;
int dilarg_top;

/* Temporary data for moneylists */
int mon_top, mon_list[50][2];

struct unit_fptr *mcreate_func(void);
void dumpdiltemplate(struct diltemplate *tmpl);
void dumpdil(struct dilprg *prg);

 void syntax(const char *str);
void fatal(const char *str);
void warning(const char *str);


#line 131 "y.tab.c"

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

#line 375 "y.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */



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
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   479

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  110
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  83
/* YYNRULES -- Number of rules.  */
#define YYNRULES  218
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  355

#define YYUNDEFTOK  2
#define YYMAXUTOK   351

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
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     107,   108,   102,   100,    18,   101,     2,   103,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    19,
       2,     2,     2,     2,   109,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    16,     2,    17,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    14,     2,    15,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   104,
     105,   106
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   166,   166,   166,   168,   172,   173,   174,   182,   181,
     192,   195,   196,   199,   258,   258,   259,   262,   264,   263,
     279,   284,   285,   287,   288,   289,   291,   295,   299,   303,
     307,   315,   320,   321,   323,   328,   333,   338,   342,   369,
     372,   374,   373,   389,   394,   395,   397,   398,   399,   401,
     405,   409,   413,   417,   421,   430,   433,   435,   434,   450,
     455,   456,   458,   459,   460,   462,   466,   470,   487,   491,
     495,   499,   503,   507,   511,   515,   519,   523,   527,   531,
     535,   539,   543,   553,   559,   633,   657,   661,   665,   678,
     693,   697,   716,   720,   724,   728,   732,   736,   740,   744,
     752,   756,   760,   765,   778,   764,   806,   805,   820,   821,
     823,   831,   837,   841,   846,   847,   849,   853,   857,   861,
     865,   869,   873,   877,   887,   891,   897,   905,   915,   925,
     937,   940,   941,   943,   947,   951,   955,   959,   963,   967,
     971,   975,   982,   981,   986,   988,   989,   991,   996,   995,
    1001,  1018,  1038,  1047,  1056,  1063,  1070,  1075,  1081,  1088,
    1093,  1094,  1096,  1100,  1101,  1102,  1103,  1108,  1109,  1111,
    1112,  1113,  1114,  1119,  1120,  1122,  1123,  1127,  1132,  1137,
    1142,  1147,  1148,  1157,  1165,  1176,  1193,  1197,  1205,  1212,
    1218,  1222,  1229,  1233,  1244,  1259,  1276,  1280,  1285,  1289,
    1296,  1301,  1305,  1309,  1313,  1317,  1322,  1326,  1330,  1341,
    1345,  1351,  1357,  1367,  1373,  1383,  1398,  1404,  1411
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "FLAGS", "END", "UNKNOWN", "OBJECTS",
  "ZONE", "RESET", "ROOMS", "MOBILES", "ENDMARK", "DIL", "DILPRG", "'{'",
  "'}'", "'['", "']'", "','", "';'", "STRING", "SYMBOL", "PNUM", "NAMES",
  "TITLE", "DESCR", "OUT_DESCR", "IN_DESCR", "EXTRA", "KEY", "OPEN",
  "MANIPULATE", "UFLAGS", "WEIGHT", "CAPACITY", "SPECIAL", "LIGHT",
  "BRIGHT", "MINV", "RANDOM", "DILCOPY", "ID", "DATA", "DURATION",
  "FIRSTF", "TICKF", "LASTF", "APPLYF", "TIME", "BITS", "STRINGT", "EXIT",
  "MOVEMENT", "IN", "SPELL", "LINK", "TO", "KEYWORD", "VALUE", "COST",
  "RENT", "TYPE", "AFFECT", "MANA", "HIT", "MONEY", "EXP", "ALIGNMENT",
  "SEX", "LEVEL", "HEIGHT", "RACE", "POSITION", "ABILITY", "WEAPON",
  "ATTACK", "ARMOUR", "DEFENSIVE", "OFFENSIVE", "SPEED", "DEFAULT", "ACT",
  "RESET_F", "LIFESPAN", "CREATORS", "NOTES", "HELP", "WEATHER", "LOAD",
  "FOLLOW", "MAX", "ZONEMAX", "LOCAL", "INTO", "NOP", "EQUIP", "DOOR",
  "PURGE", "REMOVE", "COMPLETE", "'+'", "'-'", "'*'", "'/'", "SPLUS",
  "UMINUS", "UPLUS", "'('", "')'", "'@'", "$accept", "file", "$@1",
  "sections", "uglykludge", "dil_section", "$@2", "dils", "dil",
  "room_section", "$@3", "rooms", "$@4", "room", "room_fields",
  "room_field", "oroom_field", "exitindex", "exit_fields", "exit_field",
  "object_section", "objects", "$@5", "object", "object_fields",
  "object_field", "oobject_field", "mobile_section", "mobiles", "$@6",
  "mobile", "mobile_fields", "mobile_field", "omobile_field", "unit_field",
  "$@7", "$@8", "$@9", "optfuncargs", "optfuncarg", "affect_fields",
  "affect_field", "dilargs", "dilarg", "zone_section", "zone_fields",
  "zone_field", "reset_section", "$@10", "reset_commands", "reset_command",
  "$@11", "block", "alloc", "command", "loadfields", "loadfield",
  "followfields", "followfield", "equipfields", "equipfield", "local",
  "zonemax", "max", "position", "moneylist", "moneytype", "stringlist",
  "hardstringlist", "strings", "stringcomp", "flags", "numbers", "number",
  "expr", "term", "factor", "zonename", "czonename", "unitname",
  "cunitname", "reference", "index", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   123,   125,    91,    93,    44,    59,
     269,   270,   271,   272,   273,   274,   275,   276,   277,   278,
     279,   280,   281,   282,   283,   284,   285,   286,   287,   288,
     289,   290,   291,   292,   293,   294,   295,   296,   297,   298,
     299,   300,   301,   302,   303,   304,   305,   306,   307,   308,
     309,   310,   311,   312,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,   327,   328,
     329,   330,   331,   332,   333,   334,   335,   336,   337,   338,
     339,   340,   341,   342,   343,   344,   345,   346,   347,   348,
      43,    45,    42,    47,   349,   350,   351,    40,    41,    64
};
# endif

#define YYPACT_NINF -248

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-248)))

#define YYTABLE_NINF -213

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -248,    21,    29,  -248,  -248,    42,    39,   176,  -248,  -248,
      83,  -248,  -248,    75,    67,    67,    20,    94,    96,    67,
    -248,  -248,  -248,  -248,  -248,    63,  -248,  -248,     5,     5,
      67,  -248,   -42,    18,  -248,  -248,    17,  -248,  -248,  -248,
    -248,  -248,  -248,   111,  -248,  -248,  -248,   120,    63,    63,
      18,    18,   -15,     5,     5,     5,     5,  -248,  -248,    72,
    -248,  -248,   115,   117,   135,  -248,  -248,  -248,  -248,  -248,
      18,    18,  -248,  -248,  -248,   142,   143,   143,   143,  -248,
    -248,  -248,  -248,  -248,  -248,  -248,  -248,  -248,  -248,   160,
    -248,   -17,   409,   357,   276,  -248,    67,   151,   151,  -248,
     151,   151,   151,   151,   159,  -248,    54,  -248,  -248,    20,
     155,   156,   163,   169,    20,   151,    54,    54,    54,    67,
      67,  -248,    67,    67,    67,   151,   183,   171,   151,    67,
    -248,    67,    67,    67,  -248,  -248,  -248,  -248,  -248,    54,
    -248,    67,   183,    67,    67,    67,  -248,  -248,  -248,  -248,
      54,  -248,   183,    67,    67,    67,    67,    67,    67,    67,
     183,   183,    67,    67,    67,    67,    67,    67,    54,  -248,
    -248,  -248,  -248,    98,    99,    95,  -248,  -248,  -248,    67,
    -248,   150,   191,    54,  -248,  -248,  -248,  -248,  -248,  -248,
    -248,   186,  -248,  -248,  -248,  -248,  -248,  -248,   187,  -248,
    -248,  -248,   101,   188,  -248,  -248,  -248,  -248,    56,  -248,
    -248,  -248,    27,  -248,  -248,    54,   193,   192,   193,  -248,
    -248,    67,  -248,   193,  -248,  -248,  -248,  -248,  -248,    67,
      67,  -248,  -248,  -248,  -248,  -248,  -248,  -248,   143,   196,
     -29,   -25,    91,    54,   151,  -248,  -248,  -248,    76,  -248,
    -248,    25,   209,    67,   183,    67,    67,    67,    67,    67,
      67,   177,  -248,  -248,   207,   151,    54,   151,    20,  -248,
    -248,    67,  -248,  -248,  -248,  -248,  -248,  -248,  -248,    67,
      67,    67,   151,  -248,  -248,  -248,  -248,  -248,  -248,  -248,
    -248,  -248,  -248,  -248,  -248,  -248,  -248,    67,  -248,  -248,
     198,  -248,    54,  -248,  -248,   -12,  -248,  -248,   128,  -248,
    -248,  -248,    67,  -248,  -248,  -248,  -248,  -248,  -248,  -248,
    -248,  -248,  -248,  -248,  -248,  -248,   192,  -248,  -248,  -248,
    -248,  -248,  -248,  -248,     0,    25,   210,     4,  -248,  -248,
      67,    54,   211,  -248,   128,  -248,  -248,  -248,  -248,    67,
    -248,  -248,  -248,   122,  -248
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     0,     1,   131,     0,    10,     0,     3,     8,
      16,   141,   212,     0,     0,     0,     0,     0,     0,     0,
     132,   140,   211,    11,    14,     7,   138,   210,     0,     0,
       0,   135,   200,   201,   206,   133,     0,   187,   136,   186,
     137,   139,   134,     9,    17,    40,    56,   144,     7,     7,
     203,   202,     0,     0,     0,     0,     0,   189,   190,     0,
      13,    12,    15,    39,    55,   142,     4,     5,     6,   209,
     204,   205,   207,   208,   188,     0,     0,     0,     0,   151,
     191,   214,    19,    21,   213,    42,    44,    58,    60,   151,
     145,     0,     0,     0,     0,   146,     0,     0,     0,   159,
       0,     0,     0,     0,   147,    25,     0,    20,    84,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   103,     0,     0,     0,     0,     0,     0,     0,     0,
     106,     0,     0,     0,    22,    24,    32,    23,    48,     0,
      43,     0,     0,     0,     0,     0,    45,    47,    46,    64,
       0,    59,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    61,
      63,    62,   158,   214,     0,   215,   160,   167,   173,     0,
     156,     0,     0,     0,    27,   196,    83,    86,    89,    87,
      88,     0,    92,    93,    94,    96,    97,    98,     0,   100,
     101,   102,     0,     0,    31,    26,    28,    29,     0,    95,
      90,    99,     0,    53,    54,     0,    50,     0,    51,    52,
      75,     0,    65,    67,    68,    69,    70,    71,    73,     0,
       0,    72,    76,    78,    79,    77,    74,    66,     0,     0,
     152,   153,   154,     0,     0,   151,   149,   198,     0,    91,
     104,   124,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   114,    30,     0,     0,     0,     0,     0,    33,
      49,     0,   185,   183,    82,    80,    81,   217,   216,     0,
       0,     0,     0,   166,   161,   165,   164,   163,   172,   168,
     171,   170,   169,   182,   177,   174,   175,     0,   155,   157,
     151,   197,     0,   108,   192,     0,   125,   127,   128,   129,
     218,   116,     0,   117,   119,   120,   121,   122,   123,   107,
     115,    38,    35,    37,    34,    36,     0,   180,   179,   178,
     162,   176,   150,   199,   105,     0,     0,     0,   118,   184,
       0,     0,     0,   109,   111,   126,    85,   193,   195,     0,
     112,   113,   110,     0,   194
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -248,  -248,  -248,  -248,    74,  -248,  -248,  -248,  -248,  -248,
    -248,  -248,  -248,  -248,  -248,  -248,  -248,  -248,  -248,  -248,
    -248,  -248,  -248,  -248,  -248,  -248,  -248,  -248,  -248,  -248,
    -248,  -248,  -248,  -248,    40,  -248,  -248,  -248,  -248,  -248,
    -248,   -26,  -248,  -103,  -248,  -248,  -248,  -248,  -248,    -9,
     -86,  -248,  -248,  -248,  -248,  -248,  -248,  -248,  -248,  -248,
    -248,    -3,    -1,   -95,  -248,  -121,   -79,  -102,  -247,  -248,
     -85,  -108,  -248,   -14,   218,     1,   104,  -248,    -5,    92,
     -59,   -87,  -117
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     5,    47,    10,    23,    43,    61,    25,
      44,    62,    76,    82,    92,   134,   135,   136,   212,   269,
      48,    63,    77,    85,    93,   146,   147,    49,    64,    78,
      87,    94,   169,   170,   137,   198,   303,   208,   334,   343,
     261,   262,   305,   306,     6,     7,    20,    66,    79,    89,
      90,   182,   246,    91,   104,   240,   284,   241,   289,   242,
     295,   285,   286,   287,   297,   216,   273,    38,    39,    59,
     308,   184,   248,   185,    32,    33,    34,    21,   174,    83,
     175,   176,   204
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      31,    35,    22,    95,   307,    42,   335,   186,   193,   194,
     195,   177,   191,   178,   179,   180,   181,    84,    84,    84,
     304,     3,    96,   218,   347,   215,   348,    27,   192,    50,
      51,   213,    57,   223,    36,   221,     4,    58,   202,    36,
      37,   206,   220,   229,   230,   304,   263,    27,   340,   341,
     342,     9,   264,     8,    70,    71,   265,   266,    53,    54,
     237,   279,   280,   281,   282,   279,   280,   281,   183,    45,
     283,    97,    98,    46,   288,   247,    27,    99,   100,   101,
     102,   103,   172,   267,   268,    53,    54,    74,   307,    27,
      75,   301,    24,    69,   302,    26,   336,   253,   254,   255,
     256,   257,   258,   259,   260,   196,   197,   270,   199,   200,
     201,   349,    30,  -181,    40,   207,    41,   209,   210,   211,
      55,    56,    67,    68,    60,    28,    29,   214,    65,   217,
     217,   219,    30,   148,   171,   298,   -18,   312,   -41,   222,
     217,   224,   225,   226,   227,   228,   292,   296,   231,   232,
     233,   234,   235,   236,    28,    29,   -57,   299,   323,    72,
      73,    30,    80,   293,    81,   243,   325,    28,    29,    86,
      88,  -143,   173,  -148,    30,   187,   188,    11,   322,   277,
     324,   279,  -130,   189,  -130,  -130,  -130,  -130,  -130,   190,
     294,  -181,  -181,   205,   333,   330,   319,    12,  -181,   203,
      13,  -212,   238,   244,   239,   245,   249,   274,   251,   250,
     252,   271,   272,   332,    95,   275,   276,    12,   253,   254,
     255,   256,   257,   258,   259,   260,   310,   321,   337,   346,
     354,   352,   345,   351,   278,   320,   300,   309,   290,   311,
     291,   313,   314,   315,   316,   317,   318,   339,    52,   344,
       0,     0,     0,     0,     0,     0,     0,   326,    14,    15,
      16,    17,    18,    19,     0,   327,   328,   329,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   149,     0,   150,
     151,     0,     0,   331,     0,     0,     0,     0,     0,   108,
       0,     0,     0,     0,     0,     0,     0,     0,   338,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,     0,   125,     0,     0,     0,
       0,   309,     0,     0,     0,     0,   350,     0,     0,     0,
     152,     0,     0,     0,     0,   353,     0,     0,   130,   153,
     131,   154,   155,   132,   156,   157,   133,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   138,     0,
     139,   140,     0,     0,     0,     0,     0,     0,     0,     0,
     108,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,     0,   125,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     105,   141,   106,   107,     0,   142,   143,   144,   145,   130,
       0,   131,   108,     0,   132,     0,     0,   133,     0,     0,
       0,     0,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,     0,   125,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     126,   127,   128,   129,     0,     0,     0,     0,     0,     0,
       0,   130,     0,   131,     0,     0,   132,     0,     0,   133
};

static const yytype_int16 yycheck[] =
{
      14,    15,     7,    89,   251,    19,    18,   109,   116,   117,
     118,    98,   114,   100,   101,   102,   103,    76,    77,    78,
      20,     0,    39,   144,    20,   142,    22,    22,   115,    28,
      29,   139,    15,   154,    14,   152,     7,    20,   125,    14,
      20,   128,   150,   160,   161,    20,    19,    22,    48,    49,
      50,    12,    25,    11,    53,    54,    29,    30,   100,   101,
     168,    90,    91,    92,    93,    90,    91,    92,    14,     6,
      99,    88,    89,    10,    99,   183,    22,    94,    95,    96,
      97,    98,    96,    56,    57,   100,   101,    15,   335,    22,
      18,    15,     9,   108,    18,    20,   108,    41,    42,    43,
      44,    45,    46,    47,    48,   119,   120,   215,   122,   123,
     124,   107,   107,    22,    20,   129,    20,   131,   132,   133,
     102,   103,    48,    49,    13,   100,   101,   141,     8,   143,
     144,   145,   107,    93,    94,   243,    21,   254,    21,   153,
     154,   155,   156,   157,   158,   159,   241,   242,   162,   163,
     164,   165,   166,   167,   100,   101,    21,   244,   266,    55,
      56,   107,    20,    72,    21,   179,   268,   100,   101,    77,
      78,    11,    21,    14,   107,    20,    20,     1,   265,   238,
     267,    90,     6,    20,     8,     9,    10,    11,    12,    20,
      99,   100,   101,    22,   302,   282,    19,    21,   107,    16,
      24,   103,   103,    53,   109,    14,    20,   221,   107,    22,
      22,    18,    20,    15,   300,   229,   230,    21,    41,    42,
      43,    44,    45,    46,    47,    48,    17,    20,   100,    19,
     108,    20,   335,   341,   239,   261,   245,   251,   241,   253,
     241,   255,   256,   257,   258,   259,   260,   326,    30,   334,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   271,    82,    83,
      84,    85,    86,    87,    -1,   279,   280,   281,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     1,    -1,     3,
       4,    -1,    -1,   297,    -1,    -1,    -1,    -1,    -1,    13,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   312,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    -1,    40,    -1,    -1,    -1,
      -1,   335,    -1,    -1,    -1,    -1,   340,    -1,    -1,    -1,
      54,    -1,    -1,    -1,    -1,   349,    -1,    -1,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,     1,    -1,
       3,     4,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      13,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    -1,    40,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       1,    54,     3,     4,    -1,    58,    59,    60,    61,    62,
      -1,    64,    13,    -1,    67,    -1,    -1,    70,    -1,    -1,
      -1,    -1,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    -1,    40,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      51,    52,    53,    54,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    62,    -1,    64,    -1,    -1,    67,    -1,    -1,    70
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,   111,   112,     0,     7,   113,   154,   155,    11,    12,
     115,     1,    21,    24,    82,    83,    84,    85,    86,    87,
     156,   187,   188,   116,     9,   119,    20,    22,   100,   101,
     107,   183,   184,   185,   186,   183,    14,    20,   177,   178,
      20,    20,   183,   117,   120,     6,    10,   114,   130,   137,
     185,   185,   184,   100,   101,   102,   103,    15,    20,   179,
      13,   118,   121,   131,   138,     8,   157,   114,   114,   108,
     185,   185,   186,   186,    15,    18,   122,   132,   139,   158,
      20,    21,   123,   189,   190,   133,   189,   140,   189,   159,
     160,   163,   124,   134,   141,   160,    39,    88,    89,    94,
      95,    96,    97,    98,   164,     1,     3,     4,    13,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    40,    51,    52,    53,    54,
      62,    64,    67,    70,   125,   126,   127,   144,     1,     3,
       4,    54,    58,    59,    60,    61,   135,   136,   144,     1,
       3,     4,    54,    63,    65,    66,    68,    69,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,   142,
     143,   144,   183,    21,   188,   190,   191,   191,   191,   191,
     191,   191,   161,    14,   181,   183,   177,    20,    20,    20,
      20,   177,   191,   181,   181,   181,   183,   183,   145,   183,
     183,   183,   191,    16,   192,    22,   191,   183,   147,   183,
     183,   183,   128,   181,   183,   192,   175,   183,   175,   183,
     181,   192,   183,   175,   183,   183,   183,   183,   183,   192,
     192,   183,   183,   183,   183,   183,   183,   181,   103,   109,
     165,   167,   169,   183,    53,    14,   162,   181,   182,    20,
      22,   107,    22,    41,    42,    43,    44,    45,    46,    47,
      48,   150,   151,    19,    25,    29,    30,    56,    57,   129,
     181,    18,    20,   176,   183,   183,   183,   190,   188,    90,
      91,    92,    93,    99,   166,   171,   172,   173,    99,   168,
     171,   172,   173,    72,    99,   170,   173,   174,   181,   191,
     159,    15,    18,   146,    20,   152,   153,   178,   180,   183,
      17,   183,   192,   183,   183,   183,   183,   183,   183,    19,
     151,    20,   191,   181,   191,   177,   183,   183,   183,   183,
     191,   183,    15,   181,   148,    18,   108,   100,   183,   176,
      48,    49,    50,   149,   180,   153,    19,    20,    22,   107,
     183,   181,    20,   183,   108
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   110,   112,   111,   113,   114,   114,   114,   116,   115,
     115,   117,   117,   118,   120,   119,   119,   121,   122,   121,
     123,   124,   124,   125,   125,   125,   126,   126,   126,   126,
     126,   127,   128,   128,   129,   129,   129,   129,   129,   130,
     131,   132,   131,   133,   134,   134,   135,   135,   135,   136,
     136,   136,   136,   136,   136,   137,   138,   139,   138,   140,
     141,   141,   142,   142,   142,   143,   143,   143,   143,   143,
     143,   143,   143,   143,   143,   143,   143,   143,   143,   143,
     143,   143,   143,   144,   144,   144,   144,   144,   144,   144,
     144,   144,   144,   144,   144,   144,   144,   144,   144,   144,
     144,   144,   144,   145,   146,   144,   147,   144,   148,   148,
     149,   149,   149,   149,   150,   150,   151,   151,   151,   151,
     151,   151,   151,   151,   152,   152,   152,   153,   153,   153,
     154,   155,   155,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   158,   157,   157,   159,   159,   160,   161,   160,
     162,   163,   164,   164,   164,   164,   164,   164,   164,   164,
     165,   165,   166,   166,   166,   166,   166,   167,   167,   168,
     168,   168,   168,   169,   169,   170,   170,   170,   171,   172,
     173,   174,   174,   175,   175,   176,   177,   177,   178,   178,
     179,   179,   180,   180,   180,   180,   181,   181,   182,   182,
     183,   184,   184,   184,   184,   184,   185,   185,   185,   186,
     186,   187,   188,   189,   190,   191,   191,   191,   192
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     3,     5,     2,     2,     0,     0,     3,
       0,     0,     2,     1,     0,     3,     0,     0,     0,     3,
       3,     0,     2,     1,     1,     1,     2,     2,     2,     2,
       3,     2,     0,     2,     2,     2,     2,     2,     2,     2,
       0,     0,     3,     3,     0,     2,     1,     1,     1,     3,
       2,     2,     2,     2,     2,     2,     0,     0,     3,     3,
       0,     2,     1,     1,     1,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       3,     3,     3,     2,     1,     6,     2,     2,     2,     2,
       2,     3,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     0,     0,     5,     0,     4,     0,     2,
       2,     1,     2,     2,     1,     2,     2,     2,     3,     2,
       2,     2,     2,     2,     0,     1,     3,     1,     1,     1,
       2,     0,     2,     2,     2,     2,     2,     2,     2,     2,
       1,     1,     0,     3,     0,     1,     2,     2,     0,     4,
       3,     0,     3,     3,     3,     4,     2,     4,     2,     1,
       0,     2,     2,     1,     1,     1,     1,     0,     2,     1,
       1,     1,     1,     0,     2,     1,     2,     1,     2,     2,
       2,     0,     1,     2,     4,     1,     1,     1,     3,     2,
       1,     3,     1,     3,     5,     3,     1,     3,     1,     3,
       1,     1,     2,     2,     3,     3,     1,     3,     3,     3,
       1,     1,     1,     1,     1,     1,     3,     3,     3
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
#line 166 "dmcpar.y"
    {cur = NULL;}
#line 1782 "y.tab.c"
    break;

  case 8:
#line 182 "dmcpar.y"
    {
		   char fname[256];
		   sprintf(fname,"%s.dh", zone.z_zone.name);
		   remove(fname);
		   istemplate = 1;
		}
#line 1793 "y.tab.c"
    break;

  case 9:
#line 189 "dmcpar.y"
    {
		   istemplate = 0;
		}
#line 1801 "y.tab.c"
    break;

  case 13:
#line 200 "dmcpar.y"
    {
		  /* read and register template */
		  char buf[255];
		  FILE *f;
		  CByteBuffer Buf;

		  if ((f = fopen((yyvsp[0].str), "rb")) == NULL)
		  {
		    fprintf(stderr, "%s: ", (yyvsp[0].str));
		    fatal("Couldn't open DIL template.");
		  }
		  else
		  {
		     /* read the template from temporary file */
		     int i;

		     while (!feof(f)) {
		       i = fgetc(f);
		       if (i != -1)
			 Buf.Append8(i);
		     }

		     cur_tmpl = bread_diltemplate(&Buf);

		     assert(Buf.GetReadPosition() == Buf.GetLength());

		     /*fprintf(stderr,"DMC reading template (core %d==%d)\n",
			     Buf.GetLength(), cur_tmpl->coresz);*/
		     /*		  dumpdiltemplate(cur_tmpl);*/

		     fclose(f);
		     remove((yyvsp[0].str));
		  
		     /* register DIL template in zone */
		     if (search_block(cur_tmpl->prgname, 
				      (const char **) tmplnames, TRUE) != -1) 
		     {
			/* redifinition of template */
			sprintf(buf,"Redefinition of template named '%s'",
				cur_tmpl->prgname);
			fatal(buf);			
		     } else {
			if (!zone.z_tmpl) {
			   zone.z_tmpl = cur_tmpl;
			} else {
			   cur_tmpl->next = zone.z_tmpl;
			   zone.z_tmpl = cur_tmpl;
			}
			tmplnames = add_name(cur_tmpl->prgname, tmplnames);
		     }
		  }
	       }
#line 1858 "y.tab.c"
    break;

  case 14:
#line 258 "dmcpar.y"
    { cur = 0; }
#line 1864 "y.tab.c"
    break;

  case 18:
#line 264 "dmcpar.y"
    {
			  if (cur)
			  {
			    cur->next = mcreate_unit(UNIT_ST_ROOM);
			    cur = cur->next;
			  }
			  else
			  {
			    zone.z_rooms = mcreate_unit(UNIT_ST_ROOM);
			    cur = zone.z_rooms;
			  }
			  cur_extra = 0;
			}
#line 1882 "y.tab.c"
    break;

  case 20:
#line 280 "dmcpar.y"
    {
			  UNIT_IDENT_ASSIGN(cur,(yyvsp[-2].str));
			}
#line 1890 "y.tab.c"
    break;

  case 26:
#line 292 "dmcpar.y"
    {
			  ROOM_LANDSCAPE(cur) = (yyvsp[0].num);
			}
#line 1898 "y.tab.c"
    break;

  case 27:
#line 296 "dmcpar.y"
    {
			  ROOM_FLAGS(cur) = (yyvsp[0].num);
			}
#line 1906 "y.tab.c"
    break;

  case 28:
#line 300 "dmcpar.y"
    {
			  UNIT_IN(cur) = (struct unit_data *) (yyvsp[0].str);
			}
#line 1914 "y.tab.c"
    break;

  case 29:
#line 304 "dmcpar.y"
    {
			  ROOM_RESISTANCE(cur) = (yyvsp[0].num);
			}
#line 1922 "y.tab.c"
    break;

  case 31:
#line 316 "dmcpar.y"
    {
			  ROOM_EXIT(cur, cur_ex = (yyvsp[0].num)) = mcreate_exit();
			}
#line 1930 "y.tab.c"
    break;

  case 34:
#line 324 "dmcpar.y"
    {
			  ROOM_EXIT(cur,cur_ex)->to_room =
			    (struct unit_data *) (yyvsp[0].str);
			}
#line 1939 "y.tab.c"
    break;

  case 35:
#line 329 "dmcpar.y"
    {
			  ROOM_EXIT(cur,cur_ex)->key =
			    (struct file_index_type *) (yyvsp[0].str);
			}
#line 1948 "y.tab.c"
    break;

  case 36:
#line 334 "dmcpar.y"
    {
			   ROOM_EXIT(cur, cur_ex)->open_name.
			     CopyList((const char **) (yyvsp[0].str_list));
			}
#line 1957 "y.tab.c"
    break;

  case 37:
#line 339 "dmcpar.y"
    {
			  ROOM_EXIT(cur, cur_ex)->exit_info = (yyvsp[0].num);
			}
#line 1965 "y.tab.c"
    break;

  case 38:
#line 343 "dmcpar.y"
    {
			   extern const char *dirs[];

			   if (cur_extra)
			   {
			      cur_extra->next = new (class extra_descr_data);
			      cur_extra = cur_extra->next;
			   }
			   else
			   {
			      UNIT_EXTRA_DESCR(cur) =
				new (class extra_descr_data);
			      cur_extra = UNIT_EXTRA_DESCR(cur);
			   }
			   cur_extra->next = 0;
			   
			   cur_extra->names.AppendName((char *) dirs[cur_ex]);
			   /* strcat($2, "\n\r"); */
			   strip_trailing_blanks((yyvsp[0].str));
			   cur_extra->descr.Reassign((yyvsp[0].str));
			}
#line 1991 "y.tab.c"
    break;

  case 41:
#line 374 "dmcpar.y"
    {
			  if (cur)
			  {
			    cur->next = mcreate_unit(UNIT_ST_OBJ);
			    cur = cur->next;
			  }
			  else
			  {
			    zone.z_objects = mcreate_unit(UNIT_ST_OBJ);
			    cur = zone.z_objects;
			  }
			  cur_extra = 0;
			}
#line 2009 "y.tab.c"
    break;

  case 43:
#line 390 "dmcpar.y"
    {
			  UNIT_IDENT_ASSIGN(cur, (yyvsp[-2].str));
			}
#line 2017 "y.tab.c"
    break;

  case 49:
#line 402 "dmcpar.y"
    {
			  OBJ_VALUE(cur, (yyvsp[-1].num)) = (yyvsp[0].num);
			}
#line 2025 "y.tab.c"
    break;

  case 50:
#line 406 "dmcpar.y"
    {
			  OBJ_PRICE(cur) = (yyvsp[0].num);
			}
#line 2033 "y.tab.c"
    break;

  case 51:
#line 410 "dmcpar.y"
    {
			  OBJ_PRICE_DAY(cur) = (yyvsp[0].num);
			}
#line 2041 "y.tab.c"
    break;

  case 52:
#line 414 "dmcpar.y"
    {
			  OBJ_TYPE(cur) = (yyvsp[0].num);
			}
#line 2049 "y.tab.c"
    break;

  case 53:
#line 418 "dmcpar.y"
    {
			  OBJ_FLAGS(cur) = (yyvsp[0].num);
			}
#line 2057 "y.tab.c"
    break;

  case 54:
#line 422 "dmcpar.y"
    {
			  OBJ_RESISTANCE(cur) = (yyvsp[0].num);
			}
#line 2065 "y.tab.c"
    break;

  case 57:
#line 435 "dmcpar.y"
    {
			  if (cur)
			  {
			    cur->next = mcreate_unit(UNIT_ST_NPC);
			    cur = cur->next;
			  }
			  else
			  {
			    zone.z_mobiles = mcreate_unit(UNIT_ST_NPC);
			    cur = zone.z_mobiles;
			  }
			  cur_extra = 0;
			}
#line 2083 "y.tab.c"
    break;

  case 59:
#line 451 "dmcpar.y"
    {
			  UNIT_IDENT_ASSIGN(cur, (yyvsp[-2].str));
			}
#line 2091 "y.tab.c"
    break;

  case 65:
#line 463 "dmcpar.y"
    {
			  CHAR_MANA(cur) = (yyvsp[0].num);
			}
#line 2099 "y.tab.c"
    break;

  case 66:
#line 467 "dmcpar.y"
    {
			  NPC_FLAGS(cur) = (yyvsp[0].num);
			}
#line 2107 "y.tab.c"
    break;

  case 67:
#line 471 "dmcpar.y"
    {
			   char buf[1024], tmp[100];

			   if (CHAR_MONEY(cur))
			     warning("Redefinition of money...");

			   for (myi = 0, *buf = '\0'; myi <= mon_top; myi++)
			   {
			      sprintf(tmp, " %d %d ~",
				      mon_list[myi][0], mon_list[myi][1]);
			      strcat(buf, tmp);
			   }

			   CHAR_MONEY(cur) = (char *) mmalloc(strlen(buf) + 1);
			   strcpy(CHAR_MONEY(cur), buf);
			}
#line 2128 "y.tab.c"
    break;

  case 68:
#line 488 "dmcpar.y"
    {
			  CHAR_EXP(cur) = (yyvsp[0].num);
			}
#line 2136 "y.tab.c"
    break;

  case 69:
#line 492 "dmcpar.y"
    {
			  CHAR_SEX(cur) = (yyvsp[0].num);
			}
#line 2144 "y.tab.c"
    break;

  case 70:
#line 496 "dmcpar.y"
    {
			  CHAR_LEVEL(cur) = (yyvsp[0].num);
			}
#line 2152 "y.tab.c"
    break;

  case 71:
#line 500 "dmcpar.y"
    {
			  CHAR_RACE(cur) = (yyvsp[0].num);
			}
#line 2160 "y.tab.c"
    break;

  case 72:
#line 504 "dmcpar.y"
    {
			  CHAR_ATTACK_TYPE(cur) = (yyvsp[0].num);
			}
#line 2168 "y.tab.c"
    break;

  case 73:
#line 508 "dmcpar.y"
    {
			  CHAR_POS(cur) = (yyvsp[0].num);
			}
#line 2176 "y.tab.c"
    break;

  case 74:
#line 512 "dmcpar.y"
    {
			  NPC_DEFAULT(cur) = (yyvsp[0].num);
			}
#line 2184 "y.tab.c"
    break;

  case 75:
#line 516 "dmcpar.y"
    {
			  CHAR_FLAGS(cur) = (yyvsp[0].num);
			}
#line 2192 "y.tab.c"
    break;

  case 76:
#line 520 "dmcpar.y"
    {
		 	  CHAR_NATURAL_ARMOUR(cur) = (yyvsp[0].num);
			}
#line 2200 "y.tab.c"
    break;

  case 77:
#line 524 "dmcpar.y"
    {
		 	  CHAR_SPEED(cur) = (yyvsp[0].num);
			}
#line 2208 "y.tab.c"
    break;

  case 78:
#line 528 "dmcpar.y"
    {
			  CHAR_DEFENSIVE(cur) = (yyvsp[0].num);
		        }
#line 2216 "y.tab.c"
    break;

  case 79:
#line 532 "dmcpar.y"
    {
			  CHAR_OFFENSIVE(cur) = (yyvsp[0].num);
			}
#line 2224 "y.tab.c"
    break;

  case 80:
#line 536 "dmcpar.y"
    {
			  CHAR_ABILITY(cur,(yyvsp[-1].num)) = (yyvsp[0].num);
			}
#line 2232 "y.tab.c"
    break;

  case 81:
#line 540 "dmcpar.y"
    {
			  NPC_WPN_SKILL(cur,(yyvsp[-1].num)) = (yyvsp[0].num);
			}
#line 2240 "y.tab.c"
    break;

  case 82:
#line 544 "dmcpar.y"
    {
			  NPC_SPL_SKILL(cur,(yyvsp[-1].num)) = (yyvsp[0].num);
			}
#line 2248 "y.tab.c"
    break;

  case 83:
#line 554 "dmcpar.y"
    {
			   UNIT_NAMES(cur).CopyList((const char **) (yyvsp[0].str_list));
			   /*if (UNIT_NAMES(cur) && UNIT_NAME(cur))
			     CAP(UNIT_NAME(cur)); */
			}
#line 2258 "y.tab.c"
    break;

  case 84:
#line 560 "dmcpar.y"
    {
			  CByteBuffer Buf;
			  FILE *f;
			  char buf[255];

			  if ((f = fopen((yyvsp[0].str), "rb")) == NULL)
			  {
			    fprintf(stderr, "%s: ", (yyvsp[0].str));
			    fatal("Couldn't open DIL program.");
			  }
			  else
			  {
			     if (!UNIT_FUNC(cur))
			     {
				UNIT_FUNC(cur) = mcreate_func();
				cur_func = UNIT_FUNC(cur);
			     }
			     else
			     {
				cur_func->next = mcreate_func();
				cur_func = cur_func->next;
			     }
			     cur_func->index = SFUN_DIL_INTERNAL;
			     cur_func->heart_beat = WAIT_SEC * 10;
			     cur_func->flags = SFB_ALL;

			     myi = 0;
			     int i;
			     while (!feof(f))
			     {
				i = fgetc(f);
				if (i != -1)
				  Buf.Append8(i);
			     }

			     /* always read latest version written by DIL */
			     cur_func->data = bread_dil(&Buf, NULL, 255, NULL);

			     assert(Buf.GetReadPosition() == Buf.GetLength());

			     /* fprintf(stderr,"DMC reading program\n");*/
			     /*			  dumpdil(cur_func->data);*/

			     fclose(f);
			     
			     remove((yyvsp[0].str));

			     /* register DIL template in zone */
			     cur_prg = (struct dilprg *)(cur_func->data);
			     cur_tmpl= cur_prg->sp->tmpl;
			     cur_prg->flags |= DILFL_COPY;
			     cur_tmpl->flags |= DILFL_COPY;
			     /* register DIL template in zone */
			     if (search_block(cur_tmpl->prgname, 
					      (const char **) tmplnames, TRUE) != -1) 
			     {
				/* redifinition of template */
				sprintf(buf,"Redefinition of template "
					"named '%s'",
					cur_tmpl->prgname);
				fatal(buf);			
			     } else {
				if (!zone.z_tmpl) {
				   zone.z_tmpl = cur_tmpl;
				} else {
				   cur_tmpl->next = zone.z_tmpl;
				   zone.z_tmpl = cur_tmpl;
				}
				tmplnames = add_name(cur_tmpl->prgname,
						     tmplnames);
			     }
			  }
		       }
#line 2336 "y.tab.c"
    break;

  case 85:
#line 634 "dmcpar.y"
    {
			  struct dilargstype *argcopy;

			  CREATE(argcopy, struct dilargstype, 1);

			  *argcopy = *((yyvsp[-2].dilargs));
			  argcopy->name = (yyvsp[-4].str);

			  if (!UNIT_FUNC(cur))
			  {
			    UNIT_FUNC(cur) = mcreate_func();
			    cur_func = UNIT_FUNC(cur);
			  }
			  else
			  {
			    cur_func->next = mcreate_func();
			    cur_func = cur_func->next;
			  }
			  cur_func->index = SFUN_DILCOPY_INTERNAL;
                          cur_func->data = argcopy;
			  cur_func->heart_beat = WAIT_SEC * 10;
			  cur_func->flags = SFB_ALL;
		       }
#line 2364 "y.tab.c"
    break;

  case 86:
#line 658 "dmcpar.y"
    {
			   UNIT_TITLE(cur).Reassign((yyvsp[0].str));
			}
#line 2372 "y.tab.c"
    break;

  case 87:
#line 662 "dmcpar.y"
    {
			   UNIT_OUT_DESCR(cur).Reassign((yyvsp[0].str));
			}
#line 2380 "y.tab.c"
    break;

  case 88:
#line 666 "dmcpar.y"
    {
			   if (IS_ROOM(cur) && !str_is_empty((yyvsp[0].str)))
			   {
			      memmove((yyvsp[0].str)+3, (yyvsp[0].str), strlen((yyvsp[0].str))+1);
			      
			      (yyvsp[0].str)[0] = ' ';
			      (yyvsp[0].str)[1] = ' ';
			      (yyvsp[0].str)[2] = ' ';
			   }

			   UNIT_IN_DESCR(cur).Reassign((yyvsp[0].str));
			}
#line 2397 "y.tab.c"
    break;

  case 89:
#line 679 "dmcpar.y"
    {
			   if (IS_ROOM(cur) && !str_is_empty((yyvsp[0].str)))
			   {
			      memmove((yyvsp[0].str)+3, (yyvsp[0].str), strlen((yyvsp[0].str))+1);

			      (yyvsp[0].str)[0] = ' ';
			      (yyvsp[0].str)[1] = ' ';
			      (yyvsp[0].str)[2] = ' ';

			      UNIT_IN_DESCR(cur).Reassign((yyvsp[0].str));
			   }
			   else
			     UNIT_OUT_DESCR(cur).Reassign((yyvsp[0].str));
			}
#line 2416 "y.tab.c"
    break;

  case 90:
#line 694 "dmcpar.y"
    {
			  UNIT_ALIGNMENT(cur) = (yyvsp[0].num);
			}
#line 2424 "y.tab.c"
    break;

  case 91:
#line 698 "dmcpar.y"
    {
			  if (cur_extra)
			  {
			     cur_extra->next = new (class extra_descr_data);
			     cur_extra = cur_extra->next;
			  }
			  else
			  {
			    UNIT_EXTRA_DESCR(cur) =
			      new (class extra_descr_data);
			    cur_extra = UNIT_EXTRA_DESCR(cur);
			  }
			  cur_extra->next = 0;
			  cur_extra->names.CopyList((const char **) (yyvsp[-1].str_list));
			  /* strcat($3, "\n\r"); */
			  strip_trailing_blanks((yyvsp[0].str));
			  cur_extra->descr.Reassign((yyvsp[0].str));
			}
#line 2447 "y.tab.c"
    break;

  case 92:
#line 717 "dmcpar.y"
    {
			  UNIT_KEY(cur) = (struct file_index_type *) (yyvsp[0].str);
			}
#line 2455 "y.tab.c"
    break;

  case 93:
#line 721 "dmcpar.y"
    {
			  UNIT_OPEN_FLAGS(cur) |= (yyvsp[0].num);
			}
#line 2463 "y.tab.c"
    break;

  case 94:
#line 725 "dmcpar.y"
    {
			  UNIT_MANIPULATE(cur) |= (yyvsp[0].num);
			}
#line 2471 "y.tab.c"
    break;

  case 95:
#line 729 "dmcpar.y"
    {
			  UNIT_MAX_HIT(cur) = (yyvsp[0].num);
			}
#line 2479 "y.tab.c"
    break;

  case 96:
#line 733 "dmcpar.y"
    {
			  UNIT_FLAGS(cur) |= (yyvsp[0].num);
			}
#line 2487 "y.tab.c"
    break;

  case 97:
#line 737 "dmcpar.y"
    {
			  UNIT_BASE_WEIGHT(cur) = (yyvsp[0].num);
			}
#line 2495 "y.tab.c"
    break;

  case 98:
#line 741 "dmcpar.y"
    {
			  UNIT_CAPACITY(cur) = (yyvsp[0].num);
			}
#line 2503 "y.tab.c"
    break;

  case 99:
#line 745 "dmcpar.y"
    {
			  UNIT_SIZE(cur) = (yyvsp[0].num);
			}
#line 2511 "y.tab.c"
    break;

  case 100:
#line 753 "dmcpar.y"
    {
			  UNIT_LIGHTS(cur) = (yyvsp[0].num);
			}
#line 2519 "y.tab.c"
    break;

  case 101:
#line 757 "dmcpar.y"
    {
			  UNIT_BRIGHT(cur) = (yyvsp[0].num);
			}
#line 2527 "y.tab.c"
    break;

  case 102:
#line 761 "dmcpar.y"
    {
			  UNIT_MINV(cur) = (yyvsp[0].num);
			}
#line 2535 "y.tab.c"
    break;

  case 103:
#line 765 "dmcpar.y"
    {
			  if (!UNIT_FUNC(cur))
			  {
			    UNIT_FUNC(cur) = mcreate_func();
			    cur_func = UNIT_FUNC(cur);
			  }
			  else
			  {
			    cur_func->next = mcreate_func();
			    cur_func = cur_func->next;
			  }
			}
#line 2552 "y.tab.c"
    break;

  case 104:
#line 778 "dmcpar.y"
    {
			  switch ((yyvsp[0].num))
			  {
			    case SFUN_PERSIST_INTERNAL:
			    case SFUN_DILCOPY_INTERNAL:
			    case SFUN_HUNTING:
			    case SFUN_INTERN_SHOP:
			    case SFUN_EAT_AND_DELETE:
			    case SFUN_PAIN:
			    case SFUN_NPC_VISIT_ROOM:
			    case SFUN_MERCENARY_HUNT:
			    case SFUN_PERSIST_INIT:
			    case SFUN_TEACHING:
			    case SFUN_CHANGE_HOMETOWN:
			    case SFUN_DIL_INTERNAL:
			    case SFUN_FROZEN:
			    case SFUN_RETURN_TO_ORIGIN:
			    case SFUN_GUILD_INTERNAL:
			    case SFUN_DEMI_STUFF:
			    case SFUN_LINK_DEAD:
			    case SFUN_DIL_COPY:
			      fatal("Illegal special assignment.");
			      break;
			  }
			  cur_func->index = (yyvsp[0].num);
			}
#line 2583 "y.tab.c"
    break;

  case 106:
#line 806 "dmcpar.y"
    {
			  if (!UNIT_AFFECTED(cur))
			  {
			    UNIT_AFFECTED(cur) = mcreate_affect();
			    cur_aff = UNIT_AFFECTED(cur);
			  }
			  else
			  {
			    cur_aff->next = mcreate_affect();
			    cur_aff = cur_aff->next;
			  }
			}
#line 2600 "y.tab.c"
    break;

  case 110:
#line 824 "dmcpar.y"
    {
			   char buf[1024];
			   if (cur_func->data)
			     fatal("Redefinition of data for special.");
			   
			   cur_func->data = (yyvsp[0].str);
			}
#line 2612 "y.tab.c"
    break;

  case 111:
#line 832 "dmcpar.y"
    {
			   if (cur_func->data)
			     fatal("Redefinition of data for special.");
			   cur_func->data = (yyvsp[0].str);
			}
#line 2622 "y.tab.c"
    break;

  case 112:
#line 838 "dmcpar.y"
    {
			  cur_func->heart_beat = (yyvsp[0].num);
			}
#line 2630 "y.tab.c"
    break;

  case 113:
#line 842 "dmcpar.y"
    {
			  cur_func->flags = (yyvsp[0].num);
			}
#line 2638 "y.tab.c"
    break;

  case 116:
#line 850 "dmcpar.y"
    {
			  cur_aff->id = (yyvsp[0].num);
			}
#line 2646 "y.tab.c"
    break;

  case 117:
#line 854 "dmcpar.y"
    {
			  cur_aff->duration = (yyvsp[0].num);
			}
#line 2654 "y.tab.c"
    break;

  case 118:
#line 858 "dmcpar.y"
    {
			  cur_aff->data[(yyvsp[-1].num)] = (yyvsp[0].num);
			}
#line 2662 "y.tab.c"
    break;

  case 119:
#line 862 "dmcpar.y"
    {
			  cur_aff->firstf_i = (yyvsp[0].num);
			}
#line 2670 "y.tab.c"
    break;

  case 120:
#line 866 "dmcpar.y"
    {
			  cur_aff->tickf_i = (yyvsp[0].num);
			}
#line 2678 "y.tab.c"
    break;

  case 121:
#line 870 "dmcpar.y"
    {
			  cur_aff->lastf_i = (yyvsp[0].num);
			}
#line 2686 "y.tab.c"
    break;

  case 122:
#line 874 "dmcpar.y"
    {
			  cur_aff->applyf_i = (yyvsp[0].num);
			}
#line 2694 "y.tab.c"
    break;

  case 123:
#line 878 "dmcpar.y"
    {
			  cur_aff->beat = (yyvsp[0].num);
			}
#line 2702 "y.tab.c"
    break;

  case 124:
#line 887 "dmcpar.y"
    {
                   dilargs.no = 0;
                   (yyval.dilargs) = &dilargs;
                }
#line 2711 "y.tab.c"
    break;

  case 125:
#line 892 "dmcpar.y"
    {
                   dilargs.no = 1;
		   dilargs.dilarg[0] = *(yyvsp[0].dilarg);
                   (yyval.dilargs) = &dilargs;
                }
#line 2721 "y.tab.c"
    break;

  case 126:
#line 898 "dmcpar.y"
    {
                   dilargs.no++;
		   dilargs.dilarg[dilargs.no-1] = *(yyvsp[0].dilarg);
                   (yyval.dilargs) = &dilargs;
                }
#line 2731 "y.tab.c"
    break;

  case 127:
#line 906 "dmcpar.y"
    {
		   struct dilargtype *dat;

		   dat = (struct dilargtype *)
		     mmalloc(sizeof(struct dilargtype));
		   dat->type = DILV_SLP;
		   dat->data.stringlist = (yyvsp[0].str_list);
		   (yyval.dilarg) = dat;
                }
#line 2745 "y.tab.c"
    break;

  case 128:
#line 916 "dmcpar.y"
    {
		   struct dilargtype *dat;

		   dat = (struct dilargtype *)
		     mmalloc(sizeof(struct dilargtype));
		   dat->type = DILV_SP;
		   dat->data.string = (yyvsp[0].str);
		   (yyval.dilarg) = dat;
                }
#line 2759 "y.tab.c"
    break;

  case 129:
#line 926 "dmcpar.y"
    {
		   struct dilargtype *dat;

		   dat = (struct dilargtype *)
		     mmalloc(sizeof(struct dilargtype));
		   dat->type = DILV_INT;
		   dat->data.num = (yyvsp[0].num);
		   (yyval.dilarg) = dat;
                }
#line 2773 "y.tab.c"
    break;

  case 133:
#line 944 "dmcpar.y"
    {
			   zone.z_zone.lifespan = (yyvsp[0].num);
			}
#line 2781 "y.tab.c"
    break;

  case 134:
#line 948 "dmcpar.y"
    {
			   zone.z_zone.weather = (yyvsp[0].num);
			}
#line 2789 "y.tab.c"
    break;

  case 135:
#line 952 "dmcpar.y"
    {
			  zone.z_zone.reset_mode = (yyvsp[0].num);
			}
#line 2797 "y.tab.c"
    break;

  case 136:
#line 956 "dmcpar.y"
    {
			  zone.z_zone.creators = (yyvsp[0].str_list);
			}
#line 2805 "y.tab.c"
    break;

  case 137:
#line 960 "dmcpar.y"
    {
			  zone.z_zone.notes = (yyvsp[0].str);
			}
#line 2813 "y.tab.c"
    break;

  case 138:
#line 964 "dmcpar.y"
    {
			  zone.z_zone.title = (yyvsp[0].str);
			}
#line 2821 "y.tab.c"
    break;

  case 139:
#line 968 "dmcpar.y"
    {
			  zone.z_zone.help = (yyvsp[0].str);
			}
#line 2829 "y.tab.c"
    break;

  case 140:
#line 972 "dmcpar.y"
    {
			  zone.z_zone.name = (yyvsp[0].str);
			}
#line 2837 "y.tab.c"
    break;

  case 142:
#line 982 "dmcpar.y"
    {
			  cur_cmd = 0;
			}
#line 2845 "y.tab.c"
    break;

  case 147:
#line 992 "dmcpar.y"
    {
			  cur_cmd->direction = ZON_DIR_CONT;
			}
#line 2853 "y.tab.c"
    break;

  case 148:
#line 996 "dmcpar.y"
    {
			  cur_cmd->direction = ZON_DIR_NEST;
			}
#line 2861 "y.tab.c"
    break;

  case 150:
#line 1002 "dmcpar.y"
    {
			  if (cur_cmd->direction == ZON_DIR_CONT)
			    cur_cmd->direction = ZON_DIR_UNNEST;
			  else if (cur_cmd->direction == ZON_DIR_NEST)
			    cur_cmd->direction = ZON_DIR_CONT;
			  else /* UNNEST, gotta put in a nop, here */
			  {
			    cur_cmd->next = (struct reset_command *)
			      mmalloc(sizeof(struct reset_command));
			    cur_cmd = cur_cmd->next;
			    cur_cmd->cmd_no = 0; /* nop */
			    cur_cmd->direction = ZON_DIR_UNNEST;
			  }
			}
#line 2880 "y.tab.c"
    break;

  case 151:
#line 1018 "dmcpar.y"
    {
			  if (cur_cmd)
			  {
			    cur_cmd->next = (struct reset_command *)
			      mmalloc(sizeof(struct reset_command));
			    cur_cmd = cur_cmd->next;
			  }
			  else
			  {
			    zone.z_table = (struct reset_command *)
			      mmalloc(sizeof(struct reset_command));
			    cur_cmd = zone.z_table;
			  }
			  /* init ? */
			  cur_cmd->next = 0;
			  cur_cmd->cmpl = 0;
			  /* cur_cmd->num1 = 0;
			     cur_cmd->num2 = 0; */
			}
#line 2904 "y.tab.c"
    break;

  case 152:
#line 1039 "dmcpar.y"
    {
			  cur_cmd->cmd_no = 1;
			  cur_cmd->ref1 = (yyvsp[-1].str);
			  if (!cur_cmd->num1)
			    cur_cmd->num1 = 0;
			  if (!cur_cmd->num2)
			    cur_cmd->num2 = 0;
			}
#line 2917 "y.tab.c"
    break;

  case 153:
#line 1048 "dmcpar.y"
    {
			  cur_cmd->cmd_no = 6;
			  cur_cmd->ref1 = (yyvsp[-1].str);
			  if (!cur_cmd->num1)
			    cur_cmd->num1 = 0;
			  if (!cur_cmd->num2)
			    cur_cmd->num2 = 0;
			}
#line 2930 "y.tab.c"
    break;

  case 154:
#line 1057 "dmcpar.y"
    {
			  cur_cmd->cmd_no = 2;
			  cur_cmd->ref1 = (yyvsp[-1].str);
			  if (!cur_cmd->num1)
			    cur_cmd->num1 = 0;
			}
#line 2941 "y.tab.c"
    break;

  case 155:
#line 1064 "dmcpar.y"
    {
			  cur_cmd->cmd_no = 3;
			  cur_cmd->ref1 = (yyvsp[-2].str);
			  cur_cmd->num1 = (yyvsp[-1].num);
			  cur_cmd->num2 = (yyvsp[0].num);
			}
#line 2952 "y.tab.c"
    break;

  case 156:
#line 1071 "dmcpar.y"
    {
			  cur_cmd->cmd_no = 4;
			  cur_cmd->ref1 = (yyvsp[0].str);
			}
#line 2961 "y.tab.c"
    break;

  case 157:
#line 1076 "dmcpar.y"
    {
			  cur_cmd->cmd_no = 5;
			  cur_cmd->ref1 = (yyvsp[-2].str);
			  cur_cmd->ref2 = (yyvsp[0].str);
			}
#line 2971 "y.tab.c"
    break;

  case 158:
#line 1082 "dmcpar.y"
    {
			   cur_cmd->cmd_no = 7;
			   cur_cmd->ref1 = 0;
			   cur_cmd->num1 = (yyvsp[0].num);
			   cur_cmd->num2 = 0;
			}
#line 2982 "y.tab.c"
    break;

  case 159:
#line 1089 "dmcpar.y"
    {
			  cur_cmd->cmd_no = 0;
			}
#line 2990 "y.tab.c"
    break;

  case 162:
#line 1097 "dmcpar.y"
    {
			  cur_cmd->ref2 = (yyvsp[0].str);
			}
#line 2998 "y.tab.c"
    break;

  case 166:
#line 1104 "dmcpar.y"
    {
			  cur_cmd->cmpl = !0;
			}
#line 3006 "y.tab.c"
    break;

  case 172:
#line 1115 "dmcpar.y"
    {
			  cur_cmd->cmpl = !0;
			}
#line 3014 "y.tab.c"
    break;

  case 176:
#line 1124 "dmcpar.y"
    {
			  cur_cmd->num2 = (yyvsp[0].num);
			}
#line 3022 "y.tab.c"
    break;

  case 177:
#line 1128 "dmcpar.y"
    {
			  cur_cmd->cmpl = !0;
			}
#line 3030 "y.tab.c"
    break;

  case 178:
#line 1133 "dmcpar.y"
    {
			  cur_cmd->num3 = (yyvsp[0].num);
			}
#line 3038 "y.tab.c"
    break;

  case 179:
#line 1138 "dmcpar.y"
    {
			  cur_cmd->num2 = (yyvsp[0].num);
			}
#line 3046 "y.tab.c"
    break;

  case 180:
#line 1143 "dmcpar.y"
    {
			  cur_cmd->num1 = (yyvsp[0].num);
			}
#line 3054 "y.tab.c"
    break;

  case 183:
#line 1158 "dmcpar.y"
    {
			   mon_top = 0;
			   mon_list[mon_top][0] = (yyvsp[-1].num);
			   mon_list[mon_top][1] = (yyvsp[0].num);

			   (yyval.num) = (yyvsp[-1].num) * money_types[(yyvsp[0].num)].relative_value;
			}
#line 3066 "y.tab.c"
    break;

  case 184:
#line 1166 "dmcpar.y"
    {
			   ++mon_top;
			   mon_list[mon_top][0] = (yyvsp[-1].num);
			   mon_list[mon_top][1] = (yyvsp[0].num);

			   (yyval.num) = (yyvsp[-3].num);

			   (yyval.num) += (yyvsp[-1].num) * money_types[(yyvsp[0].num)].relative_value;
			}
#line 3080 "y.tab.c"
    break;

  case 185:
#line 1177 "dmcpar.y"
    {
			   int myi;
			   (yyval.num) = -1;

			   for (myi = 0; myi <= MAX_MONEY; myi++)
			     if (!strcmp((yyvsp[0].str), money_types[myi].abbrev))
			     {
				(yyval.num) = myi;
				break;
			     }

			   if (myi > MAX_MONEY)
			     fatal("Not a legal money denominator.");
			}
#line 3099 "y.tab.c"
    break;

  case 186:
#line 1194 "dmcpar.y"
    {
			  (yyval.str_list) = (yyvsp[0].str_list);
			}
#line 3107 "y.tab.c"
    break;

  case 187:
#line 1198 "dmcpar.y"
    {
			   (yyval.str_list) = (char **) mmalloc(sizeof(char *) * 2);
			   (yyval.str_list)[0] = (yyvsp[0].str);
			   (yyval.str_list)[1] = 0;
			}
#line 3117 "y.tab.c"
    break;

  case 188:
#line 1206 "dmcpar.y"
    {
			  (yyval.str_list) = (char **) mmalloc(sizeof(char *) * (str_top + 1));
			  for (myi = 0; myi <= str_top; myi++)
			    (yyval.str_list)[myi] = str_list[myi];
			  (yyval.str_list)[myi] = 0;
			}
#line 3128 "y.tab.c"
    break;

  case 189:
#line 1213 "dmcpar.y"
    {
			  (yyval.str_list) = 0;
			}
#line 3136 "y.tab.c"
    break;

  case 190:
#line 1219 "dmcpar.y"
    {
			  str_list[str_top = 0] = (yyvsp[0].str);
			}
#line 3144 "y.tab.c"
    break;

  case 191:
#line 1223 "dmcpar.y"
    {
			  str_list[++str_top] = (yyvsp[0].str);
			}
#line 3152 "y.tab.c"
    break;

  case 192:
#line 1230 "dmcpar.y"
    {
			   (yyval.str) = (yyvsp[0].str);
			}
#line 3160 "y.tab.c"
    break;

  case 193:
#line 1234 "dmcpar.y"
    {
			   char *res;
			   res = (char *) mmalloc(strlen((yyvsp[-2].str)) + strlen((yyvsp[0].str)) + 2 + 3);
			   strcpy(res, (yyvsp[-2].str));
			   strcat(res, (yyvsp[0].str));
			   (yyval.str) = res;
			}
#line 3172 "y.tab.c"
    break;

  case 194:
#line 1245 "dmcpar.y"
    {
			   char *res;
			   char buf[100];
			   if (strlen((yyvsp[-4].str)) > 0 && isdigit((yyvsp[-4].str)[strlen((yyvsp[-4].str))-1]))
			     sprintf(buf, " %d ", (yyvsp[-1].num));
			   else
			     sprintf(buf, "%d", (yyvsp[-1].num));

			   res = (char *) mmalloc(strlen((yyvsp[-4].str)) + strlen(buf)+2+3);
			   strcpy(res, (yyvsp[-4].str));
			   strcat(res, buf);
			   (yyval.str) = res;
			}
#line 3190 "y.tab.c"
    break;

  case 195:
#line 1260 "dmcpar.y"
    {
			   char *res;
			   char buf[100];
			   if (strlen((yyvsp[-2].str)) > 0 && isdigit((yyvsp[-2].str)[strlen((yyvsp[-2].str))-1]))
			     sprintf(buf, " %d", (yyvsp[0].num));
			   else
			     sprintf(buf, "%d", (yyvsp[0].num));

			   res = (char *) mmalloc(strlen((yyvsp[-2].str)) + strlen(buf)+2+3);
			   strcpy(res, (yyvsp[-2].str));
			   strcat(res, buf);
			   (yyval.str) = res;
			}
#line 3208 "y.tab.c"
    break;

  case 196:
#line 1277 "dmcpar.y"
    {
			  (yyval.num) = (yyvsp[0].num);
			}
#line 3216 "y.tab.c"
    break;

  case 197:
#line 1281 "dmcpar.y"
    {
			  (yyval.num) = (yyvsp[-1].num);
			}
#line 3224 "y.tab.c"
    break;

  case 198:
#line 1286 "dmcpar.y"
    {
			  (yyval.num) = (yyvsp[0].num);
			}
#line 3232 "y.tab.c"
    break;

  case 199:
#line 1290 "dmcpar.y"
    {
			  (yyval.num) = (yyvsp[-2].num) | (yyvsp[0].num);
			}
#line 3240 "y.tab.c"
    break;

  case 200:
#line 1297 "dmcpar.y"
    {
			  (yyval.num) = (yyvsp[0].num);
			}
#line 3248 "y.tab.c"
    break;

  case 201:
#line 1302 "dmcpar.y"
    {
			  (yyval.num) = (yyvsp[0].num);
			}
#line 3256 "y.tab.c"
    break;

  case 202:
#line 1306 "dmcpar.y"
    {
			  (yyval.num) = -(yyvsp[0].num);
			}
#line 3264 "y.tab.c"
    break;

  case 203:
#line 1310 "dmcpar.y"
    {
			  (yyval.num) = (yyvsp[0].num);
			}
#line 3272 "y.tab.c"
    break;

  case 204:
#line 1314 "dmcpar.y"
    {
			  (yyval.num) = (yyvsp[-2].num) + (yyvsp[0].num);
			}
#line 3280 "y.tab.c"
    break;

  case 205:
#line 1318 "dmcpar.y"
    {
			  (yyval.num) = (yyvsp[-2].num) - (yyvsp[0].num);
			}
#line 3288 "y.tab.c"
    break;

  case 206:
#line 1323 "dmcpar.y"
    {
			  (yyval.num) = (yyvsp[0].num);
			}
#line 3296 "y.tab.c"
    break;

  case 207:
#line 1327 "dmcpar.y"
    {
			  (yyval.num) = (yyvsp[-2].num) * (yyvsp[0].num);
			}
#line 3304 "y.tab.c"
    break;

  case 208:
#line 1331 "dmcpar.y"
    {
			  if ((yyvsp[0].num) != 0)
			    (yyval.num) = (yyvsp[-2].num) / (yyvsp[0].num);
			  else
			  {
			    warning("Division by zero!");
			    (yyval.num) = 0;
			  }
			}
#line 3318 "y.tab.c"
    break;

  case 209:
#line 1342 "dmcpar.y"
    {
			  (yyval.num) = (yyvsp[-1].num);
			}
#line 3326 "y.tab.c"
    break;

  case 210:
#line 1346 "dmcpar.y"
    {
			  (yyval.num) = (yyvsp[0].num);
			}
#line 3334 "y.tab.c"
    break;

  case 211:
#line 1352 "dmcpar.y"
    {
			   (yyval.str) = (char *) mmalloc(strlen((yyvsp[0].sym)) + 1);
			   strcpy((yyval.str), (yyvsp[0].sym));
			}
#line 3343 "y.tab.c"
    break;

  case 212:
#line 1358 "dmcpar.y"
    {
			  if (strlen((yyvsp[0].sym)) > FI_MAX_ZONENAME)
			  {
			    warning("Zone name too long. truncated.");
			    (yyvsp[0].sym)[FI_MAX_ZONENAME] = '\0';
			  }
			  strcpy((yyval.sym), (yyvsp[0].sym));
			}
#line 3356 "y.tab.c"
    break;

  case 213:
#line 1368 "dmcpar.y"
    {
			   (yyval.str) = (char *) mmalloc(strlen((yyvsp[0].sym)) + 1);
			   strcpy((yyval.str), (yyvsp[0].sym));
			}
#line 3365 "y.tab.c"
    break;

  case 214:
#line 1374 "dmcpar.y"
    {
			  if (strlen((yyvsp[0].sym)) > FI_MAX_UNITNAME)
			  {
			    warning("Unit name too long. truncated.");
			    (yyvsp[0].sym)[FI_MAX_UNITNAME] = '\0';
			  }
			  strcpy((yyval.sym), (yyvsp[0].sym));
			}
#line 3378 "y.tab.c"
    break;

  case 215:
#line 1384 "dmcpar.y"
    {
			  if (!zone.z_zone.name)
			  {
			    fatal("local zonename must be defined.");
			    (yyval.str) = empty_ref;
			  }
			  else
			  {
			     (yyval.str) = (char *) mmalloc(strlen((yyvsp[0].sym)) +
					     strlen(zone.z_zone.name) + 2);
			     strcpy((yyval.str), zone.z_zone.name);
			     strcpy((yyval.str) + strlen(zone.z_zone.name) + 1, (yyvsp[0].sym));
			  }
			}
#line 3397 "y.tab.c"
    break;

  case 216:
#line 1399 "dmcpar.y"
    {
			   (yyval.str) = (char *) mmalloc(strlen((yyvsp[-2].sym)) + strlen((yyvsp[0].sym)) + 2);
			   strcpy((yyval.str), (yyvsp[0].sym));
			   strcpy((yyval.str) + strlen((yyvsp[0].sym)) + 1, (yyvsp[-2].sym));
			}
#line 3407 "y.tab.c"
    break;

  case 217:
#line 1405 "dmcpar.y"
    {
			   (yyval.str) = (char *) mmalloc(strlen((yyvsp[-2].sym)) + strlen((yyvsp[0].sym)) + 2);
			  strcpy((yyval.str), (yyvsp[-2].sym));
			  strcpy((yyval.str) + strlen((yyvsp[-2].sym)) + 1, (yyvsp[0].sym));
			}
#line 3417 "y.tab.c"
    break;

  case 218:
#line 1412 "dmcpar.y"
    {
			  (yyval.num) = (yyvsp[-1].num);
			}
#line 3425 "y.tab.c"
    break;


#line 3429 "y.tab.c"

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
#line 1415 "dmcpar.y"


void syntax(const char *str)
{
   extern char *yytext;

   fprintf(stderr, "%s: %d: %s\n   Token: '%s'\n", cur_filename,
	   linenum, str, yytext);
   errcon = 1;
}


void fatal(const char *str)
{
  fprintf(stderr, "%s: %d: %s\n", cur_filename, linenum, str);
  errcon = 1;
}


void real_warning(const char *str)
{
   fprintf(stderr, "WARNING: %s: %d: %s\n", cur_filename, linenum, str);
}


void warning(const char *str)
{
   extern int fatal_warnings;

   if (fatal_warnings)
     fatal(str);
   else
     real_warning(str);
}


/* Read a string from fl, allocate space for it and return it */
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

void dumpdiltemplate(struct diltemplate *tmpl) {

  int i;
  fprintf(stderr,"Name: '%s'\n",tmpl->prgname);
  fprintf(stderr,"Crc: %d\n",tmpl->varcrc);
  fprintf(stderr,"Return type: %d\n",tmpl->rtnt);
  fprintf(stderr,"# Arguments: %d\n",tmpl->argc);
  for (myi=0;myi<tmpl->argc;myi++)
    fprintf(stderr,"Arg %d type: %d\n",i,tmpl->argt[myi]);
  fprintf(stderr,"Coresize: %d\n",tmpl->coresz);
  fprintf(stderr,"Core: %p\n",tmpl->core);
  fprintf(stderr,"# Vars: %d\n",tmpl->varc);
  for (myi=0;myi<tmpl->varc;myi++) 
    fprintf(stderr,"Var %d type: %d\n",myi,tmpl->vart[myi]);
  fprintf(stderr,"# References: %d\n",tmpl->xrefcount);
  for (myi=0;myi<tmpl->xrefcount;myi++) {
     fprintf(stderr,"Xref %d: '%s'\n",myi,tmpl->xrefs[myi].name);
     fprintf(stderr,"     Return type %d\n",tmpl->xrefs[myi].rtnt);
     fprintf(stderr,"     # Arguments: %d\n",tmpl->xrefs[myi].argc);
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

