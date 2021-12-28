<<<<<<< HEAD
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
/*
Library of socket
*/
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
/*
https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.1.0/com.ibm.zos.v2r1.bpxbd00/rttcga.htm
https://www.daemon-systems.org/man/tcgetattr.3.html
*/
=======
#include<stdio.h>
#include<sys/socket.h>
#include<sys/select.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include <time.h>
#include <ctype.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include <netinet/in.h>


>>>>>>> update
#include <termios.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>

#include "lib/clientlist.h"
#include "lib/ranking.h"
#include "lib/log.h"
#include "lib/authenticate.h"
#include "lib/terminal.h"

<<<<<<< HEAD
#define BUFF_SIZE 1024
int VIEWSIZE = 9;

// fungame
#define STATUS_START_MENU 0
#define STATUS_FUNGAME_MENU 1
=======

#define STATUS_START_MENU 0
#define STATUS_HARDGAME_MENU 1
>>>>>>> update
#define STATUS_HANDLE_GAME 2
#define SIGNAL_CHECKLOGIN "SIGNAL_CHECKLOGIN"
#define SIGNAL_CREATEUSER "SIGNAL_CREATEUSER"
#define SIGNAL_OK "SIGNAL_OK"
#define SIGNAL_ERROR "SIGNAL_ERROR"
#define SIGNAL_CLOSE "SIGNAL_CLOSE"

// game
#define SIGNAL_NEWGAME "SIGNAL_NEWGAME"
<<<<<<< HEAD
=======
#define SIGNAL_2PLAYERS "SIGNAL_2PLAYERS"
#define SIGNAL_CHAT "SIGNAL_CHAT"
>>>>>>> update
#define SIGNAL_ABORTGAME "SIGNAL_ABORTGAME"
#define SIGNAL_TURN "SIGNAL_TURN"
#define SIGNAL_WIN "SIGNAL_WIN"
#define SIGNAL_LOST "SIGNAL_LOST"
#define SIGNAL_VIEWLOG "SIGNAL_VIEWLOG"
#define SIGNAL_LOGLINE "SIGNAL_LOGLINE"
#define SIGNAL_CHECK_FALSE "SIGNAL_CHECK_FALSE"
#define SIGNAL_CHECK_TRUE "SIGNAL_CHECK_TRUE"
// show info
#define INFO_QUIT "Quit game (y or n)?"
#define INFO_WIN "You Win. Do you want to view log (y or n)?"

// ranking
#define SIGNAL_RANKING "SIGNAL_RANKING"

<<<<<<< HEAD
// client connect to server
struct sockaddr_in server_addr;
int PORT, sock, recieved, isCommunicating;
char* serverAddress;
char send_msg[BUFF_SIZE], recv_msg[BUFF_SIZE];

// client variable
=======

#define SERVER_PORT 9000
#define MAX_NAME_SZE 20
#define MAX_BUFFER_SIZE 1024
#define BUFF_SIZE 1024
int VIEWSIZE = 9;
char send_msg[BUFF_SIZE], recv_msg[BUFF_SIZE];
char client_name[MAX_NAME_SZE] ={0};


>>>>>>> update
int status; // status of fungame
char choice, token[] = "#";
char error[100], user[100], id[30];

// draw table
char *table;
int size=9, playerTurn, col, row, userVal;
int viewC = 0, viewR = 0;

// trang thai choi
int checkStatus;

<<<<<<< HEAD

/*
Kết nối với server
*/
int connectToServer(){
  isCommunicating = 1; // tao ket noi
  int errorConnect;
  recieved = -1;
  //Step 1: Construct socket
  if( (sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    strcpy(error,"Error Socket!!!");
    return -1;
  }

  //Step 2: Specify server address
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = inet_addr(serverAddress);

  // set timeout
  struct timeval timeout;
  timeout.tv_sec = 20; // after 20 seconds connect will timeout
  timeout.tv_usec = 0;
  if (setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0){
    return -1;
  }
  else if (setsockopt (sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0){
    return -1;
  }

  //Step 3: Request to connect server
  if( connect(sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) == -1){
    errorConnect = errno;
    sprintf(error,"Error! Can not connect to server! %s",strerror(errorConnect));
    return -1;
  }
  //Step 4: Communicate with server
  send(sock, send_msg, strlen(send_msg), 0);
  recieved = recv(sock, recv_msg, BUFF_SIZE, 0);
  recv_msg[recieved] = '\0';
  strcpy(send_msg, SIGNAL_CLOSE); // gui tin hieu ngat ket noi
  send(sock, send_msg, strlen(send_msg), 0);
  close(sock);
  if(recieved == -1){
    printf("\nError: Timeout!!!\n");
    return -1;
  }
  isCommunicating = 0; // ngat ket noi
  return 0;
}

/*
Hiển thị phần login
*/
int menuSignin(){
  error[0] = '\0';
  char pass[100], *str;
  while(1){
=======
//Function declaration
int client_create_socket(int *listen_fd);
int client_recv_from_server(int socket_client, char *recv_msg);
int client_send_to_server(int socket_client, char *send_msg);

int client_build_fdsets(int listenfd, fd_set *readfds, fd_set *writefds, fd_set *exceptfds);
int client_select(int max_fd, int listenfd, fd_set *readfds, fd_set *writefds);

int menuSignin(int listen_fd){
  error[0] = '\0';
  char pass[100], *str;
  while(1){
    memset(send_msg, 0 ,sizeof(send_msg));
    memset(recv_msg, 0 ,sizeof(recv_msg));
>>>>>>> update
    clearScreen();
    printf("\t ____________________________\n");
    printf("\t|----------HARDGAME----------|\n");
    printf("\t|          Sign in           |\n");
    printf("\t|____________________________|\n");
    if(error[0] != '\0'){
      printf("Error: %s!\n", error);
      printf("Do you want to try again?(y or n): ");
      choice = getchar();
      while(getchar() != '\n');
      if(choice=='n' || choice=='N'){
      	error[0] = '\0';
      	return -1;
      }
      else if(choice !='y' && choice !='Y'){
        continue;
      }
      else{
      	error[0] = '\0';
      	continue;
      }
    }
    printf("\tUsername: ");
    fgets(user, BUFF_SIZE, stdin);
    user[strlen(user)-1] = '\0';

    printf("\tPassword: ");
    fgets(pass, BUFF_SIZE, stdin);
    pass[strlen(pass)-1] = '\0';

    //check username and password
    sprintf(send_msg, "%s#%s#%s", SIGNAL_CHECKLOGIN, user, pass);
<<<<<<< HEAD
    if(connectToServer() == 0){
=======
    client_send_to_server(listen_fd,send_msg);
    printf("%s\n",send_msg );
    client_recv_from_server(listen_fd,recv_msg);
>>>>>>> update
      str = strtok(recv_msg, token);
      if(strcmp(str, SIGNAL_OK) == 0){
        // break;
        return 0;
      }
      else if(strcmp(str, SIGNAL_ERROR) == 0){
      	str = strtok(NULL, token);
      	strcpy(error, str);
        return -2;
      }
<<<<<<< HEAD
    }
    else {
      printf("Error! Cant connect to server!\n");
      return -1;
    }
=======
     return -1;

>>>>>>> update
  }
  // return 0;
}

/*
Hiển thị phần đăng kí
*/
<<<<<<< HEAD
int menuRegister(){
  error[0] = '\0';
  char pass[100], comfirmPass[100], *str;
  while(1){
=======
int menuRegister(int listen_fd){
  error[0] = '\0';
  char pass[100], comfirmPass[100], *str;
  while(1){
    memset(send_msg, 0 ,sizeof(send_msg));
    memset(recv_msg, 0 ,sizeof(recv_msg));
>>>>>>> update
    clearScreen();
    printf("\t ____________________________\n");
    printf("\t|----------HARDGAME----------|\n");
    printf("\t|          Register          |\n");
    printf("\t|____________________________|\n");
    if(error[0] != '\0'){
      printf("Error: %s!\n", error);
      printf("Do you want to try again?(y or n): ");
      choice = getchar();
      while(getchar() != '\n');
      if(choice =='n' || choice =='N'){
      	error[0] = '\0';
      	return -2;
      }
      else if(choice !='y' && choice !='Y')
        continue;
      else{
      	error[0] = '\0';
      	continue;
      }
    }
    printf("\tUsername: ");
    fgets(user, BUFF_SIZE, stdin);
    user[strlen(user)-1] = '\0';

    printf("\tPassword: ");
    fgets(pass, BUFF_SIZE, stdin);
    pass[strlen(pass)-1] = '\0';
    printf("\n\tConfirm password: ");
    fgets(comfirmPass, BUFF_SIZE, stdin);
    comfirmPass[strlen(comfirmPass)-1] = '\0';
    if(strcmp(pass, comfirmPass) == 0){
      // register new account
      sprintf(send_msg, "%s#%s#%s", SIGNAL_CREATEUSER, user, pass);
<<<<<<< HEAD
      if(connectToServer() == 0){
=======
      client_send_to_server(listen_fd,send_msg);
      client_recv_from_server(listen_fd,recv_msg);
>>>>>>> update
      	str = strtok(recv_msg, token);
      	if(strcmp(str, SIGNAL_OK) == 0)
      	  break;
      	else if(strcmp(str, SIGNAL_ERROR) == 0){
      	  str = strtok(NULL, token);
      	  strcpy(error, str);
      	}
<<<<<<< HEAD
      }
      else {
        printf("Error! Cant connect to server!\n");
        return -1;
      }
=======
>>>>>>> update
    }
    else{
      strcpy(error, "Password does not match");
    }
  }
  return 0;
}

/*
Hiển thị chọn login, create
*/
<<<<<<< HEAD
int menuStart(){
  int checkSignin = 0, checkRegister = 0;
  error[0] = '\0';
  while(1){
=======
int menuStart(int listen_fd){
  int checkSignin = 0, checkRegister = 0;
  error[0] = '\0';
  while(1){
    memset(send_msg, 0 ,sizeof(send_msg));
    memset(recv_msg, 0 ,sizeof(recv_msg));
>>>>>>> update
    clearScreen();
    if(error[0] != '\0'){
      printf("Error: %s!\n", error );
      error[0] = '\0';
    }
    printf("\t ____________________________\n");
    printf("\t|----------HARDGAME----------|\n");
    printf("\t|  1.Sign in                 |\n");
    printf("\t|  2.Register                |\n");
    printf("\t|  3.Exit                    |\n");
    printf("\t|____________________________|\n");
    printf("\tYour choice: ");
    scanf("%c", &choice);
    while(getchar() != '\n');
    if(choice == '1'){
<<<<<<< HEAD
      checkSignin = menuSignin();
=======
      checkSignin = menuSignin(listen_fd);
>>>>>>> update
      if( checkSignin == 0){
        break;
      } else if( checkSignin == -1)
        return -1;
    }
    else if(choice == '2'){
<<<<<<< HEAD
      checkRegister = menuRegister();
=======
      checkRegister = menuRegister(listen_fd);
>>>>>>> update
      if(checkRegister == 0)
        break;
      else if( checkRegister == -1)
        return -1;
    }
<<<<<<< HEAD
    else if(choice == '3') return -1;
=======
    else if(choice == '3') {
      close(listen_fd);
      exit(-1);
    }
>>>>>>> update
    else sprintf(error,"No option %c", choice);
  }

  return 0;
}

/*
Hiển thị phần game
*/
<<<<<<< HEAD
int menuGame(){
  error[0] = '\0';
  char* str;
  while(1){
=======
int menuGame(int listen_fd){
  error[0] = '\0';
  char* str;
  while(1){
    memset(send_msg, 0 ,sizeof(send_msg));
    memset(recv_msg, 0 ,sizeof(recv_msg));
>>>>>>> update
    clearScreen();
    printf("\t____________________________\n");
    printf("\t----------HARDGAME----------\n");
    printf("\033[0;33m\tUsername: %s \033[0;37m\n", user);
    printf("\tNEW GAME\n");
    printf("\t---------------------------\n");
    if(error[0] != '\0'){
      printf("Error: %s!\n", error);
      printf("Do you want to try again? (y or n): ");
      choice = getchar();
      while(getchar() != '\n');
      if(choice =='n' || choice =='N'){
      	error[0] = '\0';
      	return -1;
      }
      else if(choice !='y' && choice !='Y')
        continue;
      else{
      	error[0] = '\0';
      	continue;
      }
    }

    VIEWSIZE = 9;
    sprintf(send_msg, "%s#%d#%s", SIGNAL_NEWGAME, size, user);
<<<<<<< HEAD
    if(connectToServer() == 0){
=======
    client_send_to_server(listen_fd,send_msg);
    client_recv_from_server(listen_fd,recv_msg);
>>>>>>> update
      str = strtok(recv_msg, token);
      if(strcmp(str, SIGNAL_OK) == 0){
      	str = strtok(NULL, token);
      	strcpy(id, str);
      	  //init table
        str = strtok(NULL, token);
      	table = malloc(size * size);
      	for(int i = 0; i < size * size; i++){
          if(str[i] != '0')
      	     table[i] = str[i];
          else
              table[i] = ' ';
        }
      	break;
      }
      else if(strcmp(str, SIGNAL_ERROR) == 0){
      	 str = strtok(NULL, token);
      	 strcpy(error, str);
      }
<<<<<<< HEAD
    }else {
        printf("Error! Cant connect to server!\n");
    }
=======
>>>>>>> update
  }
  return 0;
}


<<<<<<< HEAD
int handleRanking(){
  char* str;
  error[0] = '\0';

=======
int handleRanking(int listen_fd){
  char* str;
  error[0] = '\0';
  memset(send_msg, 0 ,sizeof(send_msg));
  memset(recv_msg, 0 ,sizeof(recv_msg));
>>>>>>> update
  clearScreen();
  printf("----------HARDGAME----------\n");
  printf("\033[0;33m\tUsername: %s \033[0;37m\n", user);
  printf("--------Sudoku Ranking--------\n");
  printf("Your information: \n");
  sprintf(send_msg, "%s#%s", SIGNAL_RANKING, user);
<<<<<<< HEAD
  if(connectToServer() == 0){
=======
  client_send_to_server(listen_fd,send_msg);
  client_recv_from_server(listen_fd,recv_msg);
>>>>>>> update
    str = strtok(recv_msg, token);
    if(strcmp(str, SIGNAL_OK) == 0){
      str = strtok(NULL, token);
      strcpy(id, str);
      int numberOfWin;
      int point;

      str = strtok(NULL, token);
      numberOfWin = atoi(str);

      if(numberOfWin == -1){
        printf("You haven't played. You are not in the ranking.\n");
      } else{
        str = strtok(NULL, token); point = atoi(str);
        printf("\t%-20s: %s\n","Username-ID", id);
        printf("\t%-20s: %d\n","Number Of Wins", numberOfWin);
        printf("\t%-20s: %d\n","Point", point);
      }
      printf("------------------------------\n");
      printf("Ranking\n");
      printf("------------------------------------------------------------\n");
      readFileRanking();
      printf("%-5s%-20s%-10s%-10s\n", "TOP", "ID", "Win", "Point");
      if(numberOfWin == -1) printfRankingNotID();
      else printfRanking(id);
      rootRank = NULL; cur = NULL; new = NULL;
    }
<<<<<<< HEAD
  }
  else {
    printf("Error! Cant connect to server!\n");
  }
=======


>>>>>>> update
  printf("Press 'q' to quit: ");
  choice = getchar();
  while(getchar() != '\n');
  if(choice == 'q' || choice == 'Q') return 1;
  else return 1;
}


<<<<<<< HEAD
int menuHardGame(){
  error[0] = '\0';
  while(1){
=======

int menuHardGame(int listen_fd){
  error[0] = '\0';
  while(1){
    memset(send_msg, 0 ,sizeof(send_msg));
    memset(recv_msg, 0 ,sizeof(recv_msg));
>>>>>>> update
    clearScreen();
    if(error[0] != '\0'){
      printf("Error: %s!\n", error);
      error[0] = '\0';
    }
    printf("\t ______________________________\n");
    printf("\t|----------HARDGAME------------|\n");
    printf("\t|\033[0;33m\tWelcome %s \033[0;37m        |\n", user);
    printf("\t|1.Sudoku game                 |\n");
<<<<<<< HEAD
    printf("\t|2.Sudoku ranking              |\n");
    printf("\t|3.Sign out                    |\n");
=======
    printf("\t|2.Sudoku 2 players            |\n");
    printf("\t|3.Sudoku ranking              |\n");
    printf("\t|4.Sign out                    |\n");
>>>>>>> update
    printf("\t|______________________________|\n");
    printf("\tYour choice: ");
    scanf("%c", &choice);
    while(getchar() != '\n');
    if(choice == '1'){
      checkStatus = 1;
<<<<<<< HEAD
      if(menuGame() == 0) break;
    }
    else if(choice == '2'){
      checkStatus = 2;
      break;
    }
    else if(choice == '3'){
=======
      if(menuGame(listen_fd) == 0) break;
    }
    else if(choice == '2'){
      checkStatus = 2;
      //connectToClient(listen_fd);
      break;
    }
    else if(choice == '3'){
      checkStatus = 3;
      break;
    }
    else if(choice == '4'){
      close(listen_fd);
      exit(-1);
>>>>>>> update
      return -1;
    }
    else {
      sprintf(error,"%c Not an optional", choice);
    }
  }
  return 0;
}

<<<<<<< HEAD
/*
View log
*/
int viewLog(){
  char usernameLog[100], *str;

  sprintf(send_msg, "%s#%s", SIGNAL_VIEWLOG, id);
  if(connectToServer() == 0){
=======


/*
View log
*/
int viewLog(int listen_fd){
  char usernameLog[100], *str;
  memset(send_msg, 0 ,sizeof(send_msg));
  memset(recv_msg, 0 ,sizeof(recv_msg));
  sprintf(send_msg, "%s#%s", SIGNAL_VIEWLOG, id);
  client_send_to_server(listen_fd,send_msg);
  client_recv_from_server(listen_fd,recv_msg);
>>>>>>> update
    // get file name
    int j = 0;
    for(int i = 15; i < strlen(recv_msg); i++){
      usernameLog[j] = recv_msg[i];
      if(recv_msg[i] == '\0') break;
      j++;
    }
    usernameLog[j] = '\0';

    str = strtok(recv_msg, token);

    if(strcmp(str, SIGNAL_LOGLINE) == 0){
      str = strtok(NULL, token);
      printf("Username: %s ", str);

      str = strtok(NULL, token);
      printf("- SessionID: %s\n", str);
      str = strtok(NULL, token); // address
      // printf(" - Address: %s\n", str);
      str = strtok(NULL, token);
      printf("Day: %s ", str);
      str = strtok(NULL, token);
      str[strlen(str) - 4] = '\0';
      printf("- Time: %s\n", str);
    }
    else if(strcmp(str, SIGNAL_ERROR) == 0){
      strcpy(send_msg, SIGNAL_CLOSE);
<<<<<<< HEAD
      send(sock, send_msg, strlen(send_msg), 0);
      close(sock);
      isCommunicating = 0;
=======
      // send(sock, send_msg, strlen(send_msg), 0);
      client_send_to_server(listen_fd,send_msg);
      // client_recv_from_server(*listen_fd,recv_msg);
      close(listen_fd);

>>>>>>> update
      str = strtok(NULL, token);
      strcpy(error, str);
      return -1;
    }
    printLog(usernameLog); // in log
<<<<<<< HEAD
  }
  else {
    printf("Error! Cant connect to server!\n");
  }
=======


>>>>>>> update

  printf("Press 'q' to quit: ");
  choice = getchar();
  while(getchar() != '\n');
  if(choice == 'q' || choice == 'Q') return 0;
  else return 0;
}

/*
Vẽ bảng chơi
*/
void drawTable(){
  int i, j, k;

  for(i = 0; i < VIEWSIZE; i++){
    if(i == 3 || i == 6){
      printf("%s","                             \n");
    }
    for(j = 0; j < VIEWSIZE; j++){
      if(j == 3 || j == 6){
        printf("%s"," ");
      }
        if(i == row && j == col){
          printf("\033[0;37m[\033[0;37m%c\033[0;37m]", table[i * size + j]); // white
        }
        else{
          printf("\033[0;36m[\033[0;33m%c\033[0;36m]", table[i * size + j]); // yellow

        }
      }
      printf("\n");
  }
}

/*
Xử lí, điều khiển quân bằng w,a,s,d, ấn q thì thoát
*/
<<<<<<< HEAD
int handleGame(){
=======
int handleGame(int listen_fd){
>>>>>>> update
  char c, info[100], *str;
  setPrivateTerminal();
  error[0] = '\0';
  info[0] = '\0';

  playerTurn = 1;
  col = size / 2;
  row = size / 2;
  while(1){
<<<<<<< HEAD
=======
    memset(send_msg, 0 ,sizeof(send_msg));
    memset(recv_msg, 0 ,sizeof(recv_msg));
>>>>>>> update
    clearScreen();
    printf("\033[0;37m----------SUDOKU GAME----------\n");
    printf("\033[0;33m\tUsername: %s - GameID = %s\033[0;37m\n", user, id);
    printf("\tPress w,a,s,d to move \n");
    printf("\tPress 1 -> 9 to fill\n");
    printf("\tNeu ban dien sai thi se ko hien so va bi tru diem\n");
    printf("\tPress 'q' to quit\n");
    printf("-----------------------------\n");
    drawTable();
    if(playerTurn){
      if(error[0] != '\0'){
      	setNormalTerminal();

      	printf("Error: %s!\n", error);
      	printf("Do you want to try again? (y or n)\n");
      	printf("Your choice: ");
      	choice = getchar();
      	while(getchar() != '\n');
      	if(choice == 'n' || choice == 'N'){
      	  error[0] = '\0';
      	  return -1;
      	}
      	else if(choice == 'y' || choice == 'Y'){
      	  error[0] = '\0';
      	  if(info[0] == '\0')
      	    playerTurn = 0;
      	  else if(strcmp(info, INFO_WIN) == 0){
      	    if(choice == 'y' || choice == 'Y'){
<<<<<<< HEAD
      	      if(viewLog() == 0){
            		sprintf(send_msg, "%s#%s#%s",SIGNAL_ABORTGAME, id, user);
            		connectToServer();
=======
      	      if(viewLog(listen_fd) == 0){
            		sprintf(send_msg, "%s#%s#%s",SIGNAL_ABORTGAME, id, user);
                client_send_to_server(listen_fd,send_msg);
                client_recv_from_server(listen_fd,recv_msg);
>>>>>>> update
            		return -1;
      	      }
      	    }
      	  }
      	}
      	setPrivateTerminal();
      }
      else if(info[0] != '\0'){
      	setNormalTerminal();

      	printf("\033[0;37m%s\n", info);
      	printf("Your choice: ");
      	choice = getchar();
      	while(getchar() != '\n');
      	if(choice == 'n' || choice == 'N'){
      	  if(strcmp(info, INFO_QUIT) == 0)
      	    info[0] = '\0';
      	  else if(strcmp(info, INFO_WIN) == 0){
      	    sprintf(send_msg, "%s#%s#%s",SIGNAL_ABORTGAME, id, user);
<<<<<<< HEAD
      	    connectToServer();
=======
            client_send_to_server(listen_fd,send_msg);
            client_recv_from_server(listen_fd,recv_msg);
>>>>>>> update
      	    return -1;
      	  }
      	}
      	else if(choice == 'y' || choice == 'Y'){
      	  if(strcmp(info, INFO_QUIT) == 0){
      	    sprintf(send_msg, "%s#%s#%s",SIGNAL_ABORTGAME, id, user);
<<<<<<< HEAD
      	    connectToServer();
      	    return -1;
      	  }
      	  else if(strcmp(info, INFO_WIN) == 0){
      	    if(viewLog() == 0){
      	      sprintf(send_msg, "%s#%s#%s",SIGNAL_ABORTGAME, id, user);
      	      connectToServer();
=======
            client_send_to_server(listen_fd,send_msg);
            client_recv_from_server(listen_fd,recv_msg);
      	    return -1;
      	  }
      	  else if(strcmp(info, INFO_WIN) == 0){
      	    if(viewLog(listen_fd) == 0){
      	      sprintf(send_msg, "%s#%s#%s",SIGNAL_ABORTGAME, id, user);
              client_send_to_server(listen_fd,send_msg);
              client_recv_from_server(listen_fd,recv_msg);
>>>>>>> update
      	      return -1;
      	    }
      	  }
      	}
      	setPrivateTerminal();
      }
      else{
      	c = getchar();
      	if(c == 'w' && row > 0) row--;
      	else if(c == 's' && row < size - 1) row++;
      	else if(c == 'd' && col < size - 1) col++;
      	else if(c == 'a' && col > 0) col--;
        else if(c == '1' && table[row * size + col] == ' '){
      	  table[row * size + col] = '1';
          playerTurn = 0;
          userVal = 1;
      	}
        else if(c == '2' && table[row * size + col] == ' '){
      	  table[row * size + col] = '2';
          playerTurn = 0;
          userVal = 2;
      	}
        else if(c == '3' && table[row * size + col] == ' '){
      	  table[row * size + col] = '3';
          playerTurn = 0;
          userVal = 3;
      	}
        else if(c == '4' && table[row * size + col] == ' '){
      	  table[row * size + col] = '4';
          playerTurn = 0;
          userVal = 4;
      	}
        else if(c == '5' && table[row * size + col] == ' '){
      	  table[row * size + col] = '5';
          playerTurn = 0;
          userVal = 5;
      	}
        else if(c == '6' && table[row * size + col] == ' '){
      	  table[row * size + col] = '6';
          playerTurn = 0;
          userVal = 6;
      	}
        else if(c == '7' && table[row * size + col] == ' '){
      	  table[row * size + col] = '7';
          playerTurn = 0;
          userVal = 7;
      	}
        else if(c == '8' && table[row * size + col] == ' '){
      	  table[row * size + col] = '8';
          playerTurn = 0;
          userVal = 8;
      	}
        else if(c == '9' && table[row * size + col] == ' '){
      	  table[row * size + col] = '9';
          playerTurn = 0;
          userVal = 9;
      	}
      	else if(c == 'q') strcpy(info, INFO_QUIT);
      }
    }
    else {
      sprintf(send_msg, "%s#%s#%s#%d#%d#%d", SIGNAL_TURN, id, user, col, row, userVal);
<<<<<<< HEAD
      if(connectToServer() == 0){
=======
      client_send_to_server(listen_fd,send_msg);
      client_recv_from_server(listen_fd,recv_msg);
>>>>>>> update
      	str = strtok(recv_msg, token);
      	if(strcmp(str, SIGNAL_TURN) == 0){
      	  str = strtok(NULL, token);
          if(strcmp(str, SIGNAL_CHECK_FALSE) == 0){
              table[row * size + col] = ' ';
          }
      	}
      	else if(strcmp(str, SIGNAL_WIN) == 0){
          str = strtok(NULL, token);
          strcpy(info,INFO_WIN);
          //sprintf(info,"%s: %s\n%s","Result",str,INFO_WIN);
        }
      	else if(strcmp(str, SIGNAL_ERROR) == 0){
      	  str = strtok(NULL, token);
      	  strcpy(error, str);
      	}
<<<<<<< HEAD
      }
=======
>>>>>>> update
      playerTurn = 1;
    }
  }
  setNormalTerminal();
  return 0;
}

<<<<<<< HEAD
int main(int argc, char* argv[]){
  int err;
  if(argc != 3){
    printf("Syntax Error.\n");
    printf("Syntax: ./client IPAddress PortNumber\n");
    return 0;
  }
  serverAddress = argv[1];
  PORT = atoi(argv[2]);

  status = STATUS_START_MENU;
  while(1){
    if(status == STATUS_START_MENU){
      if(menuStart() == -1)
        break;
      else
        status = STATUS_FUNGAME_MENU;
    }
    else if(status == STATUS_FUNGAME_MENU){
      if(menuHardGame() == - 1)
        status = STATUS_START_MENU;
      else
        status = STATUS_HANDLE_GAME;
    }
    else if(status == STATUS_HANDLE_GAME){
      if(checkStatus == 1){
        if(handleGame() == - 1){
          free(table);
          table = NULL;
          status = STATUS_FUNGAME_MENU;
        }
      }
      else if(checkStatus == 2){
        handleRanking();
        status = STATUS_FUNGAME_MENU;
      }
    }
  }
  return 0;
=======

int main(int argc, char *argv[]) {

    int listen_fd = 0;
    int new_socket = 0;
    int max_fd = 0;
    fd_set readfds;
    fd_set writefds;
    fd_set exceptfds;

    if(client_create_socket(&listen_fd) != 0) {
        perror("ERROR : socket creation failed");
        exit(0);
    }
    max_fd = listen_fd;
    int k = 0;
out:
    while(1) {
         max_fd = client_build_fdsets(listen_fd, &readfds, &writefds, &exceptfds);
         if(k == 0){
           printf("\033[0;37m-------------------------ROOM CHAT---------------------\n");
           printf("\tNhap LIST de xem danh sach nguoi dang online\n");
           printf("\tNhap CONNECT:username de ket noi voi nguoi do\n");
           printf("\tNhan :q de thoat\n");
           printf("-------------------------------------------------------\n");
           k++;
         }
         if(client_select(max_fd,listen_fd, &readfds, &writefds)==-1){
           close(listen_fd);
           exit(-1);
           break;
         };
    }

    close(listen_fd);

    return 0;
}


//Create a client socket
int client_create_socket(int *listen_fd) {
    struct sockaddr_in server_addr;

    if((*listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("ERROR : socket creation failed");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;


    if(0!=connect(*listen_fd,(struct sockaddr *)&server_addr,sizeof(struct sockaddr))) {
        perror("ERROR : connect failed");
        return -1;
    }

    status = STATUS_START_MENU;
    while(1){
      if(status == STATUS_START_MENU){
        if(menuStart(*listen_fd) == -1)
          break;
        else
          status = STATUS_HARDGAME_MENU;
      }
      else if(status == STATUS_HARDGAME_MENU){
        if(menuHardGame(*listen_fd) == - 1)
          status = STATUS_START_MENU;
        else
          status = STATUS_HANDLE_GAME;
      }
      else if(status == STATUS_HANDLE_GAME){
        if(checkStatus == 1){
          if(handleGame(*listen_fd) == - 1){
            free(table);
            table = NULL;
            status = STATUS_HARDGAME_MENU;
          }
        }
        else if(checkStatus == 2){
          clearScreen();
          return 0;
          status = STATUS_HARDGAME_MENU;
        }
        else if(checkStatus == 3){
          handleRanking(*listen_fd);
          status = STATUS_HARDGAME_MENU;
        }
      }
    }


    return 0;

}

//Receive data from server
int client_recv_from_server(int client_socket, char *recv_msg) {
     int read_bytes = 0;

     if((read_bytes = recv(client_socket, recv_msg, MAX_BUFFER_SIZE, 0)) > 0) {
            printf("%s\n",recv_msg);
    }
    else if(read_bytes == 0) {
            printf("Client Disconnected\n");
            close(client_socket);
    }
    else {
            printf("ERROR: recv failed\n");
        }
    return 0;
}

//Send data to server
int client_send_to_server(int client_socket, char *send_msg) {
    int write_bytes = 0;
    int len =strlen(send_msg);

       if((write_bytes = send(client_socket, send_msg, len, 0)) <=0) {
            perror("ERROR : send failed");
            return -1;
        }

    return write_bytes;
}

//Refresh set Fds
int client_build_fdsets(int listenfd, fd_set *readfds, fd_set *writefds, fd_set *exceptfds) {
    int max_fd = listenfd;

    FD_ZERO(readfds);
    FD_SET(listenfd, readfds);
    FD_SET(listenfd, writefds);
    FD_SET(STDIN_FILENO,readfds);
    fcntl(STDIN_FILENO,F_SETFL,O_NONBLOCK);

    return max_fd;
}

//Client select call
int client_select(int max_fd,int listen_fd, fd_set *readfds, fd_set *writefds) {
    char recv_msgg[MAX_BUFFER_SIZE] ;
    char send_buff[MAX_BUFFER_SIZE] ;
    memset(recv_msgg, 0 ,sizeof(recv_msgg));
    memset(send_buff, 0 ,sizeof(send_buff));

    int action = select(max_fd+1,readfds,writefds,NULL,NULL);


    if(action == -1 || action == 0) {
        perror("ERROR: select");
        exit(0);
    }

     if(FD_ISSET(listen_fd,readfds)) {
        client_recv_from_server(listen_fd,recv_msgg);
    }

     if(FD_ISSET(STDIN_FILENO,readfds)) {
         if(read(0,send_buff,sizeof(send_buff))>0) {
           send_buff[strlen(send_buff)-1] = '\0';
           if(strcmp(send_buff,":q") != 0)
                client_send_to_server(listen_fd,send_buff);
           else{
             return -1;
           }
        }

    }

    return 0;
>>>>>>> update
}
