CC = cc -O2 -std=c99
CFLAGS = -Wall -Wextra -Wpedantic -D_POSIX_C_SOURCE=200809L
THRD = -pthread
SRC = src
BIN = .
CDSRC = cd ${SRC} &&

SER=tokpol_serv

SERVSRC = server.c\
	  base_func.c\
	  thread.c\
	  serv_func.c\
	  use_file.c

SERVOBJ = server.o\
	  base_func.o\
	  thread.o \
	  serv_func.o\
	  use_file.o
server:
	${CDSRC} ${CC} ${CFLAGS} -c ${SERVSRC}
	${CDSRC} ${CC} ${THRD} -o ../${BIN}/${SER}\
		${SERVOBJ}

clear:
	${CDSRC} rm -f ${SERVOBJ}

