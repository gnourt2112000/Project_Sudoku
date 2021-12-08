CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm

all: client server

client: client.c lib/log.c lib/authenticate.c lib/terminal.c lib/ranking.c
	${CC} client.c lib/log.c lib/authenticate.c lib/terminal.c lib/ranking.c -o client
server: server.c lib/log.c lib/authenticate.c lib/terminal.c lib/clientlist.c lib/ranking.c lib/solvesudoku.c
 	${CC} server.c lib/log.c lib/authenticate.c lib/terminal.c lib/clientlist.c lib/ranking.c lib/solvesudoku.c -o server

clean:
	rm -f *.o *~
