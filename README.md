# Tok Pol
Made for God Glory.
Hail Supreame God one in great trinity.
Prise Saint Maria, mother of God.

Alpha 1.0

My friend asked for program that will work as communicator.
TokPol is shortcut from Talk Poland.

OpenBSD license style, NO DEPENDECIES for server.
Clang or gcc should do it. It's C99 + posix.
Client needs tcl.
I know it's unefficient but I wanted to write
at leats one time in my life one thread per client program.
Server -> Unix. Client under TCL should work on any platform.

Server need configuration. In server we basically set
port we run it under and user + pass also histroy file.
Everything is in open file.
I repeat everything on server is in open file. Password too.
It's for some buddy talk so it's just a kiddo security.
Server keeps history up to 1024 messages. Rest is simply removed.

Biggest advantage is how small and basic this soft is.

How to install?
Client on unix needs to get tcl and wish packages. After that he runs ./tokpol script.
On windows client needs to download tcl+tk binary. I belive easiest way to do it is from here:
https://www.magicsplat.com/tcl-installer/index.html
After that download tokpol and tkopol.bat, put them in same directory. To run program run tokpol.bat

To compile server:
Run make in makefile directory.

make clear to reamove objects.

No make install. You manage binaries/scripts on your own.
