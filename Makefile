compile:
	gcc -Wall -w -g3 -fsanitize=address server.c lib/log.c lib/authenticate.c lib/terminal.c lib/clientlist.c lib/ranking.c lib/solvesudoku.c -o server
	gcc -Wall -w -g3 -fsanitize=address client.c lib/log.c lib/authenticate.c lib/terminal.c lib/ranking.c -o client
