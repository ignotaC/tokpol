# Tok Pol
Made for God Glory.
Hail Supreame God one in great trinity.
Prise Saint Maria, mother of God.

STILL NOT FINISHED.

My friend asked for program that will work as communicator.
TokPol is shortcut from Talk Poland.

OpenBSD license style, NO DEPENDECIES.
Clang or gcc should do it. It's C99 + posix.
I know it's unefficient but I wanted to write
at leats one time in my life one thread per client program.
Windows users must compile under cygwin.
In future maybe no cygwin client version.
But not server.

Client and server need configuration. In server we basically set
port we run it under and user + pass. Everything is in open file.
It's for some buddy talk so it's just a kiddo security.
Server keeps history up to 1024 messages. Rest is simply removed.

Full description how to use is in MANUAL file.

Biggest advantage is how small and basic this soft is.

How to install?
Run make this readme directory.
make client:	make client
make server:	make server

No make install. You manage binaries on your own.
