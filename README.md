# DikuMUD2
DikuMUD II from 1997

------------

March 3, 2020
It works. But not completely done. Waiting for Makefile from Ken Perry regarding PP.

-----------
February 2020, Michael Seifert

I've compiled this old venerable project for Ubuntu 19. Making the old wheels turn on a 64 bit architecture :) 

A few notes on getting it running.

1) Clone the repo

   cd dm-dist-ii/

2) Compile
      touch .depends ; make depend
      make all
      cd PP/
      ./ppmake 

3) Be sure to create a directory "ply/" and in there one directory for
   each letter in the alphabet "a/", "b/", ..., "z/", etc.

4) Make symbolic links
     ln -s PP/pp
     ln -s DIL/dil

------------

To make it run:

1) compile all zones:
      DMC/dmc zon/*.zon

2) Get the Mplex/mplex up and running.
      Mplex/mplex &
   This program accepts (telnet) connections and in turn communicates with
   the dmserver. Default port is 4242. There's a way to have one with color
   support and one without. The multiplexers communicate with the dmserver
   on port 4999

3) Run Diku II:

     ./dmserver
     (or gdb dmserver)

