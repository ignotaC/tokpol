CC = cc -Wall -Wextra -pedantic -O2 -std=c99
THRD = -pthread
SRC = src
BIN = bin
CDSRC = cd ${SRC} &&

SER=tokpol_serv
CLI=tokpol_cli

SERVSRC = server.c\
	  base_func.c\
	  thread.c\
	  serv_func.c\
	  use_file.c

SERVOBJ = server.o\
	  base_func.o\
	  thread.o\
	  serv_func.o\
	  use_file.o
server:
	${CDSRC} ${CC} -c ${SERVSRC}
	${CDSRC} ${CC} -o ../${BIN}/${SER}\
		${SERVOBJ}

clear:
	${CDSRC} rm -f ${SERVOBJ}

