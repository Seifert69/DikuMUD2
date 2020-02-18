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

#
#
#  Set the flag "-DMUD_DEBUG" to get full memory debug, type checks,
#     and command / sfun history.
#



#SYSFLAGS	=	-DBITCONV
SYSFLAGS	=	-DMPLEX -DMUD_DEBUG -DMEMCHECK -DMEMORY_DEBUG
#SYSFLAGS        =       -DPROFILE -pg


include $(MAKEFILE)

LEXFILTER=	| perl lexfilter $(SYSPORT) # Got to filter lex for bad code

LINK.cpp=	$(CPP) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)
LINK.c	=	$(LD) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)

DEBUG	=	-g -O6

CFL	=	$(DEBUG) $(SYSPORT) $(SYSFLAGS) $(EXTRA_CFLAGS)
CFLAGS	=	-I .. $(CFL) -pedantic -Wall -Wno-unused -Wno-variadic-macros

#LDFLAGS	=	-static
OPT	=	#-O2 	# -O might be a good idea, but we have a tough time just getting the show on the road.   :)

# SOURCES

# c-files for the multiplexer
MPLEX_SRCS = mplex.c network.c translate.c bytestring.c

# OBJECTS

MPLEX_OBJS = $(MPLEX_SRCS:.c=.o) ../select.o ../queue.o ../textutil.o \
        ../memory.o ../utility.o ../common.o ../protocol.o

   # ------------------------------ oOoOoOo ------------------------------ #

all:	mplex

mplex:	$(MPLEX_OBJS)
	$(LINK.c) $(OPT) $(MPLEX_OBJS) -o $@

clean:
	-rm -f *.o mplex core

# Need to get the tmp_*.c files into .depends as well
depend:
	@echo Updating dependencies...
	@$(MKDPND) -I .. $(MPLEX_SRCS) > .depends
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

#.include ".depends"		# For `normal' make
include .depends		# For gnu-make


