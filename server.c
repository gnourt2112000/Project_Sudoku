#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
/*
Library of socket
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include "lib/clientlist.h"
#include "lib/solvesudoku.h"
#include "lib/ranking.h"
#include "lib/log.h"
#include "lib/authenticate.h"

#define BUFF_SIZE 1024

// Auth
#define SIGNAL_CHECKLOGIN "SIGNAL_CHECKLOGIN"
#define SIGNAL_CREATEUSER "SIGNAL_CREATEUSER"
#define SIGNAL_OK "SIGNAL_OK"
#define SIGNAL_ERROR "SIGNAL_ERROR"
#define SIGNAL_CLOSE "SIGNAL_CLOSE"

// sudoku game
#define SIGNAL_NEWGAME "SIGNAL_NEWGAME"
#define SIGNAL_ABORTGAME "SIGNAL_ABORTGAME"
#define SIGNAL_TURN "SIGNAL_TURN"
#define SIGNAL_CHECK_FALSE "SIGNAL_CHECK_FALSE"
#define SIGNAL_CHECK_TRUE "SIGNAL_CHECK_TRUE"
#define SIGNAL_WIN "SIGNAL_WIN"
#define SIGNAL_LOST "SIGNAL_LOST"
#define SIGNAL_VIEWLOG "SIGNAL_VIEWLOG"
#define SIGNAL_LOGLINE "SIGNAL_LOGLINE"

// sudoku ranking
#define SIGNAL_RANKING "SIGNAL_RANKING"

// Sudoku

typedef struct sudoku
{
    /* data */
    char data[82];

}Sudoku;


// server connect to client
int PORT;
struct sockaddr_in server_addr,client_addr;
fd_set master;
char send_msg[BUFF_SIZE] , recv_msg[BUFF_SIZE];

// server variable
char token[] ="#";
char *str;
char game[82];
// int tttResult;
int** puzzle = NULL;
int** userPuzzle = NULL;
int** tempPuzzle = NULL;
int result;
int count;
int r;

/*
update sudoku ranking
*/
void updateRanking( char* user, int result){
  readFileRanking();
  node* tmp = checkUser(user);
  if(tmp == NULL ){ // user ko co trong danh sach
    userInfor sudokuUser;
    strcpy(sudokuUser.username, user);
    sudokuUser.numberOfWin=0; sudokuUser.point=0;
    insert(sudokuUser);
    updateFileRanking();
    tmp = checkUser(user);
  }

  tmp->user.numberOfWin++;
  tmp->user.point = tmp->user.point + result;
  updateFileRanking();
  rootRank = NULL; cur = NULL; new = NULL; tmp = NULL;
}

void initSudoku(){
  Sudoku sudoku[100];
  FILE *f = fopen("data/sudokuData.txt","r");
  int total_data = 0;
  while(!feof(f)){
      fscanf(f,"%s",sudoku[total_data].data);
      total_data++;
  }
  fclose(f);
  time_t t;
  srand((unsigned) time(&t));
  r = rand() % total_data;
  puzzle = createPuzzle(sudoku[r].data);
  userPuzzle = copyPuzzle(puzzle);
  tempPuzzle = copyPuzzle(puzzle);
  printPuzzle(userPuzzle);
  result = 3;
  count = 1;
  strcpy(game,sudoku[r].data);
}

/*
Xử lí dữ liệu gửi từ client
*/
int handleFromClient(int fd){
  char *user, *pass, *id;
  int recieved, col, row, userVal;
  ClientInfo *info;

  recieved = recv( fd, recv_msg, BUFF_SIZE, 0);
  recv_msg[recieved] = '\0';
  
  str = strtok( recv_msg, token);
  if( strcmp(str, SIGNAL_CLOSE) == 0){
    FD_CLR(fd, &master); // Clears the bit for the file descriptor fd in the file descriptor set fdset.
    printf("Close connection from fd = %d\n", fd );
  }
  else if(strcmp(str, SIGNAL_CREATEUSER) == 0){
    // Create new user
    user = strtok(NULL, token);
    pass = strtok(NULL, token);
    if(isValid(user, NULL)){
      sprintf(send_msg,"%s#%s",SIGNAL_ERROR, "Account existed");
    } else{
      registerUser(user, pass);
      sprintf(send_msg, SIGNAL_OK);
    }

    send( fd, send_msg, strlen(send_msg), 0);
  }
  else if( strcmp(str, SIGNAL_CHECKLOGIN) == 0){
    // Login
    user = strtok(NULL, token);
    pass = strtok(NULL, token);
    if(isValid(user, pass)) strcpy(send_msg, SIGNAL_OK);
    else sprintf( send_msg,"%s#%s", SIGNAL_ERROR, "Username or Password is incorrect");
    // while(1); // test timeout
    send(fd, send_msg, strlen(send_msg), 0);
  }
  else if(strcmp(str, SIGNAL_NEWGAME) == 0){
    // Play new game
    initSudoku();
    str = strtok(NULL, token); //get size
    recieved = atoi(str);
    str = strtok(NULL, token); //get user
    id = addInfo(inet_ntoa(client_addr.sin_addr), recieved, str); //get id game
    printf(" game with id = %s\n", id);
    printf(" Game Info: ");
    printInfo( getInfo(id) );
    sprintf( send_msg,"%s#%s#%s", SIGNAL_OK, id, game);
    send( fd, send_msg, strlen(send_msg), 0);
  }
  else if(strcmp(str, SIGNAL_RANKING) == 0){
    // Handle sudoku ranking
    str = strtok(NULL, token);
    id = str;
    printf(" Ranking with id = %s\n", id);
    // xu li phan gui thong tin so tran thang, thua, diem
    readFileRanking();
    node* tmp = checkUser(id);
    char inforUser[100];
    if( tmp == NULL) strcpy(inforUser, "-1");
    else{
      printf("Username-ID: %s, Win: %d, Point: %d\n", tmp->user.username, tmp->user.numberOfWin, tmp->user.point);
      sprintf(inforUser,"%d#%d", tmp->user.numberOfWin, tmp->user.point);
    }
    // traversingList(); // duyet danh sach sudokuRanking in ra phia server
    rootRank = NULL; cur = NULL; new = NULL; tmp = NULL;
    sprintf(send_msg,"%s#%s#%s", SIGNAL_OK, id, inforUser);
    send(fd, send_msg, strlen(send_msg), 0);
  }
  else if(strcmp(str, SIGNAL_TURN) == 0){
    // Quay về server để xử lí game sudoku
    id = strtok(NULL, token); // get game id
    user = strtok(NULL, token); // get user name

    str = strtok(NULL, token); // //get column
    col = atoi(str);

    str = strtok(NULL, token); //get row
    row = atoi(str);

    str = strtok(NULL, token); //get row
    userVal = atoi(str);
    printf("Received a turn of game id = %s, user = %s : column = %d, row = %d userVal = %d\n", id, user, col, row, userVal);
    // set table
    info = getInfo(id);
    if(info != NULL){
      // write log
      writeLog(info->logfile, col, row);
      // player win
      int i,j;
      count = 1;
      for (i = 0; i < 9; i++){
          for(j = 0; j < 9; j++){
              if (userPuzzle[i][j] != 0){
                  count++;
              }
          }
      }
      printf("%d\n",count);
      if(count <= 81){
          int check = 0;
          if(checkBox(userPuzzle, row, col, userVal))
              userPuzzle[row][col] = userVal;
          else{
              check = 1;
              result--;
          }
          for (i = 0; i < 9; i++){
              for(j = 0; j < 9; j++){
                  tempPuzzle[i][j] = userPuzzle[i][j];
              }
          }
          if(!solvePuzzle(tempPuzzle)){
              userPuzzle[row][col] = 0;
              check = 1;
              result--;
          }
          if(check == 1){
            sprintf(send_msg, "%s#%s", SIGNAL_TURN,SIGNAL_CHECK_FALSE);
            send(fd, send_msg, strlen(send_msg), 0);
          }else{
            if(count == 81){
              printf("Player win\n");
              updateRanking(user, result);
              sprintf(send_msg, "%s#%d", SIGNAL_WIN,result);
              send(fd, send_msg, strlen(send_msg), 0);
            }else{
              sprintf(send_msg, "%s#%s", SIGNAL_TURN,SIGNAL_CHECK_TRUE);
              send(fd, send_msg, strlen(send_msg), 0);
            }
          }
      }
    }
    else{
      printf("Request a turn of game with id = %s REQUEST FAILED!\n", id);
      sprintf(send_msg, "%s#%s%s%s", SIGNAL_ERROR, "Game with id=", id, "not existed");
      send(fd, send_msg, strlen(send_msg), 0);
    }
  }
  else if(strcmp(str, SIGNAL_VIEWLOG) == 0){
    // View log
    // get id-username
    id = strtok(NULL, token);
    info = getInfo(id);
    if(info != NULL){
      printf("Request view log of sudoku game with id = %s | OK!\n", id);
      sprintf(send_msg, "%s#%s", SIGNAL_LOGLINE, info->logfile);
      send(fd, send_msg, strlen(send_msg), 0);
    }
    else{
      printf("Request view log of sudoku game with id = %s | FAILED!!!\n", id);
      printf("Game with id = %s not existed", id);
      sprintf(send_msg, "%s#%s%s%s", SIGNAL_ERROR, "Game with id=", id, "not existed");
      send(fd, send_msg, strlen(send_msg), 0);
    }
  }
  else if(strcmp(str, SIGNAL_ABORTGAME) == 0){
    // hủy bỏ trò chơi
    id = strtok(NULL, token);
    str = strtok(NULL, token);
    if(removeInfo(id) == 0){
      printf("Remove user %s's sudoku game with id = %s\n", str, id);
      strcpy(send_msg, SIGNAL_OK);
    }
    else{
      printf("Remove user %s's sudoku game with id = %s REQUEST FAILED!!!\n", str, id);
      printf("Game with id = %s not existed\n", id);
      sprintf(send_msg, "%s#%s%s%s", SIGNAL_ERROR, " game with id=", id, "not existed");
    }
    send(fd, send_msg, strlen(send_msg), 0);
  }
}

int main(int argc, char *argv[]){
  if(argc != 2){
    printf("Syntax Error.\n");
    printf("Syntax: ./server PortNumber\n");
    return 0;
  }
  PORT = atoi(argv[1]);

  int sock, connected, sin_size, true1 = 1;
  int fdmax, i, rc;
  fd_set read_fds;

  FD_ZERO(&master);
  FD_ZERO(&read_fds);
  initList();

  // Step 1: Construct a TCP socket to listen connection request
  if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("Socket Error!!!\n");
    exit(-1);
  }
  if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&true1,sizeof(int)) == -1) {
    perror("Setsockopt error!!!\n");
    exit(-2);
  }

  //Step 2: Bind address to socket
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  bzero(&(server_addr.sin_zero),8);

  if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
    perror("Unable to bind\n");
    exit(-3);
  }

  //Step 3: Listen request from client
  if (listen(sock, 5) == -1) {
    perror("Listen error\n");
    exit(-4);
  }
  printf("HARDGAME waiting for client on port %d\n", PORT);
  fflush(stdout);

  FD_SET(sock, &master);
  fdmax = sock;

  // Set timeout
  struct timeval timeout;
  timeout.tv_sec  = 1000;  // after 1000 seconds will timeout
  timeout.tv_usec = 0;
  //Step 4: Communicate with clients
  while(1){

    read_fds = master;
    rc = select(fdmax + 1, &read_fds, NULL, NULL, &timeout);
    if( rc == -1){
      perror("select() error!\n");
      exit(-6);
    }
    else if (rc == 0){
     printf("  select() timed out. End program.\n");
     exit(-5);
    }
    for(i = 0; i <= fdmax; i++){
      if(FD_ISSET(i, &read_fds)){
        if(i == sock){
          sin_size = sizeof(struct sockaddr_in);
          connected = accept(sock, (struct sockaddr*)&client_addr, &sin_size);
          if(connected == -1){
            perror("accept error!\n");
            exit(-7);
          }
          else{
            FD_SET(connected, &master);
            if(connected > fdmax)
              fdmax = connected;
            printf("Got a connection from (%s , %d) with fd = %d\n", inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port), connected);
            handleFromClient(connected);
          }
        }
        else{
          handleFromClient(i);
        }
      }
    }

  }

  close(sock);
  return 0;
}
