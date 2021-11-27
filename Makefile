CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm

all: client server

client: client.c clientHelper.c ranking.c checkinput.c
	${CC} client.c clientHelper.c ranking.c checkinput.c -o client

server: server.c serverHelper.c checkinput.c linklist.c ranking.c
	${CC} server.c serverHelper.c checkinput.c linklist.c ranking.c -o server

clean:
	rm -f *.o *~
