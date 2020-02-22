# DikuMUD2
DikuMUD II from 1997

==
2020-02-22
WORK IN PROGRESS BUT ALMOST DONE
Seems like I pretty much just need to find a new C pre-processor.
With a little help from Ken Perry on that we should be good to release.
==
February 2020, Michael Seifert

I've compiled it for Ubuntu 19. Trying to make the old wheels turn on a 64 bit architecture :) 

A few notes

First, get the mplex/mplex up and running. This program accepts (telnet) connections and in turn communicates with the dmserver. Default port is 4242 as far as I recall.

Secondly, run dmserver.

To compile zones

DMC/dmc zon/midgaard.zon

You have to create a directory "ply/" and in there one directory for each letter in the alphabet "a/", "b/", etc.
