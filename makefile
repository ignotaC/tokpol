CC = cc -Wall -Wextra -pedantic -O2 -std=c99
THRD = -pthread
SRC = src
BIN = bin

SERVSRC = server.c\
	  base_func.c\
	  thread.c\
	  serv_func.c

SERVOBJ = server.o\
	  base_func.o\
	  thread.o\
	  serv_func.o
server:
	cd ${SRC}  && \
	${CC} ${THRD} -c ${SERVSRC}

clear:
	cd ${SRC} && \
	rm -f ${SERVOBJ}

