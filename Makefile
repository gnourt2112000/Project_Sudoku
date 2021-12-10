compile:
	gcc server.c lib/log.c lib/authenticate.c lib/terminal.c lib/clientlist.c lib/ranking.c lib/solvesudoku.c -o server
	gcc client.c lib/log.c lib/authenticate.c lib/terminal.c lib/ranking.c -o client
