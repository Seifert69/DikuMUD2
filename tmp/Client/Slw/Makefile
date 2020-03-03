# Oct 22, 1993 [gnort] : Did a major overhaul of Makefile.
# Jun 25, 1994 [gnort] : And another one.
#
# After adding/changing #include's in sourcefiles or adding new sourcefiles:
# > make depend
#
# If make complains about a file `.depends' not present:
# > touch .depends ; make depend
#
# NOTE!!  Using lex instead of flex may cause severe grievance, if you forget
# to change the declaration in dmcpar.y from *yytext to yytext[] as well!!

# options for system porting [HHS]

SYSPORT	= -DLINUX

CC	=	g++
LD	=	g++
MKDPND	=	g++ -MM $(CFL)

LINK.c	=	$(LD) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)

DEBUG	=	-g #-pg
CFL	=	-m486 -O4 $(DEBUG) $(BLTIN) $(SYSPORT)
CFLAGS	=	$(CFL) -Wall -Wno-unused -pipe -pedantic

# Various stupidities in the marcel header-files kills system.c with -pedantic
CFLAGS2 =	$(CFL) -ansi -Wall
LDLIBS	=	#-lm	# Mathlib for the server
OPT	=		# -O might be a good idea, but we have a tough time
 			# just getting the show on the road.   :)

# c-files for server
CLIENT_SRCS = serial.cc packet.cc slide.cc log.cc queue.cc timer.cc \
              timeout.cc mytime.cc select.cc

SRCS = $(CLIENT_SRCS)

CLIENT_OBJS =	$(CLIENT_SRCS:.cc=.o)

   # ------------------------------ oOoOoOo ------------------------------ #

all: slw.o


slw.o:	$(CLIENT_OBJS)
	ld -m elf_i386 -r -o slw.o $(CLIENT_OBJS)

$(CLIENT_OBJS):
	$(CC) -D CLIENT $(OPT) $(CFLAGS) -c $(@:.o=.cc) -o $@


#all: libslw.so.y.x
#
# This generates a shared library!
#
#libslw.so.y.x:	$(CLIENT_OBJS)
#	gcc -shared -Wl,-soname,libslw.so.y -o libslw.so.y.x $(CLIENT_OBJS)
#
#$(CLIENT_OBJS):
#	gcc -shared -D CLIENT $(OPT) $(CFLAGS) -c $(@:.o=.cc) -o $@
#

depend:
	@echo Updating dependencies...
	@$(MKDPND) -I. $(SRCS) > .depends
	@echo Done.

clean:
	@-rm -f *.o

#.include ".depends"		# For `normal' make
include .depends		# For gnu-make
