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
CLIENT_SRCS = str_parse.cc arg.cc client.cc channel.cc inter.cc updown.cc \
              system.cc network.cc

SRCS = $(CLIENT_SRCS)

CLIENT_OBJS =	$(CLIENT_SRCS:.cc=.o)

   # ------------------------------ oOoOoOo ------------------------------ #

all: client slwtest

client:	$(CLIENT_OBJS) Slw/slw.o
	$(LINK.c) -static $(CLIENT_OBJS) Slw/slw.o ../utility.o -o $@ $(LDLIBS)

slwtest:	test.o Slw/slw.o
	$(LINK.c) -static test.o ../utility.o Slw/slw.o -o $@ $(LDLIBS)


#
#all: init.o
#
#init.o:	$(CLIENT_OBJS)
#	ld -m elf_i386 -r -o init.o $(CLIENT_OBJS)
#

$(CLIENT_OBJS) test.o:
	$(CC) -I Slw/ -D CLIENT $(OPT) $(CFLAGS) -c $(@:.o=.cc) -o $@


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

clean:
	@-rm -f *.o client

depend:
	@echo Updating dependencies...
	@$(MKDPND) -ISlw $(SRCS) test.cc > .depends
	@echo Done.

.depends:
	@touch .depends
	@make depend

#.include ".depends"		# For `normal' make
include .depends		# For gnu-make
