CC = cc -Wall -Wextra -pedantic -O2 -std=c99
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
	${CDSRC} ${CC} -c ${SERVSRC}
	${CDSRC} ${CC} ${THRD} -o ../${BIN}/${SER}\
		${SERVOBJ}

clear:
	${CDSRC} rm -f ${SERVOBJ}

