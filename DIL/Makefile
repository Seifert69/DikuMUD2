# The makefile this makefile includes
MAKEFILE =	Makefile.linux

# You may have to change the use of include, ifneq, and endif to fit your make.
#
# Oct 22, 1993 [gnort] : Did a major overhaul of Makefile.
# Jun 25, 1994 [gnort] : And another one.
# Feb 03, 1995 [gnort] : Split the makefile into architecture dependant parts.
#
# After adding/changing #include's in sourcefiles or adding new sourcefiles:
# > make depend
#
# If make complains about a file `.depends' not present:
# > touch .depends ; make depend
#
# NOTE!!  Using lex instead of flex may cause severe grief, if you forget
# to change the declaration in dmcpar.y from *yytext to yytext[] as well!!

#
#
#  Set the flag "-DMUD_DEBUG" to get full memory debug, type checks,
#     and command / sfun history.
#


#SYSFLAGS	=	-DBITCONV
SYSFLAGS	=	-DMUD_DEBUG -DMEMCHECK -DMEMORY_DEBUG
#SYSFLAGS        =       -DPROFILE -pg

include $(MAKEFILE)

LEXFILTER=	| perl lexfilter $(SYSPORT) # Got to filter lex for bad code

LINK.cpp=	$(CPP) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)
LINK.c	=	$(LD) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)

DEBUG	=	#-O6

CFL	=	$(DEBUG) $(SYSPORT) $(SYSFLAGS) $(EXTRA_CFLAGS)
CFLAGS	=	-I .. -DDIL $(CFL) -pedantic -Wall -Wno-unused -Wno-format-overflow -Wno-variadic-macros

# (f)lex/yacc/bison generated c-files are notorious for their lousy C
# hence, no warnings...
CFLAGS2	= 	$(CFL) -Wno-format-overflow -Wno-variadic-macros

YFLAGS	=	-d -v 	# y.tab.h generator, and verbose output

LDLIBS	=	$(EXTRA_LDLIBS)
#LDFLAGS	=	-static

OPT	=	#-O2 	# -O might be a good idea, but we have a tough time
                        # just getting the show on the road.   :)

# SOURCES

# c-files for dil
DIL_SRCS = tmp_dilpar.c tmp_dillex.c utility.c string.c structs.c

# OBJECTS

DIL_OBJS  =	$(DIL_SRCS:.c=.o)  dmc_db_file.o

   # ------------------------------ oOoOoOo ------------------------------ #

all:	dil

clean:
	-rm -f *.o tmp_* dil core

realclean:	clean
	-rm -f $(SERVER) blktest dil dmc mplex header

# Need to get the tmp_*.c files into .depends as well
depend:	tmp_dilpar.c tmp_dillex.c
	@echo Updating dependencies...
	@$(MKDPND) -I .. $(DIL_SRCS) > .depends
	@echo Done.

DIFFOPT	=	--ignore-all-space --ignore-blank-lines --context --new-file \
		--exclude-from=.patch_exclude

diff:
	@echo Remember, 1 is diff\'s normal exit-value...
	-$(DIFF) $(DIFFOPT) old_src . > Valhalla.patch

TAGS:	$(SERV_SRCS) $(SERV_HDRS) # Only tags for server...
	@echo Building TAGS file....
	@etags $(SERV_SRCS) $(SERV_HDRS)
	@echo Done.

#zones:	data
#.if $(ZONEDIR) != "zon/"
#	# Only copy if ZONEDIR is not the dir to copy from!
#	@cp zon/*.reset zon/*.data zon/zonelist $(ZONEDIR)
#.endif
#
backup:
	tar cvf - *.[chly] | gzip -9 >! dikusrc.tgz
	tar cvf - Makefile TODO distfile | gzip -9 >! dikumisc.tgz
	tar cvf - zon/* | gzip -9 >! dikuzon.tgz
	tar cvf - lib/link/* | gzip -9 >! dikulib.tgz

src:
	tar cf - *.[chly] Filelist Makefile.* TODO TODO.gnort .patch_exclude \
          lexfilter sign addheader pack.perl list.perl \
          | gzip -1 > Src.tgz

dil:	$(DIL_OBJS)
	$(LINK.c) $(OPT) $(DIL_OBJS) ../memory.o ../textutil.o \
        ../namelist.o ../files.o ../extra.o ../bytestring.o ../common.o -o $@

tmp_dilpar.c: dilpar.y
	$(YACC) $(YFLAGS) dilpar.y
	@-if [ -f y.output ]; then mv y.output tmp_dilpar.output; fi
	@mv y.tab.c tmp_dilpar.c
	@mv y.tab.h tmp_dilpar.h

tmp_dilpar.o:	tmp_dilpar.c
	$(CC) -I .. $(OPT) $(CFLAGS2) -c tmp_dilpar.c

tmp_dillex.c:	dillex.l
	$(LEX) -t dillex.l $(LEXFILTER) > tmp_dillex.c

tmp_dillex.o:	tmp_dillex.c
	$(CC) -I .. $(OPT) $(CFLAGS2) -c tmp_dillex.c

# dmc_db_file.o is dependant on db_file.o to inherit its dependencies...
# This may mean that db_file.c is compiled twice when not needed, but hey!
dmc_db_file.o:	db_file.c
	$(CC) -DDMC $(OPT) $(CFLAGS) -c db_file.c -o $@


#.include ".depends"		# For `normal' make
include .depends		# For gnu-make


