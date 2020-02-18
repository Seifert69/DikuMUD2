# Makefile for lib/ files.
#
# To use, write: make

all:	races.dat spells.dat skills.dat weapons.dat abilities.dat

races.dat: races.def
	perl defconvert.perl races

spells.dat: spells.def
	perl defconvert.perl spells

skills.dat: skills.def
	perl defconvert.perl skills

weapons.dat: weapons.def
	perl defconvert.perl weapons

abilities.dat: abilities.def
	perl defconvert.perl abilities

clean:
	-rm -f spells.dat skills.dat weapons.dat abilities.dat races.dat \
               tmp.cpp tmp.lin swap.blk
