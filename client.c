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
#include <termios.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>

#include "linklist.h"
#include "checkinput.h"
#include "ranking.h"
#include "clientHelper.h"

#define BUFF_SIZE 1024
int VIEWSIZE = 9;

// fungame
#define STATUS_START_MENU 0
#define STATUS_FUNGAME_MENU 1
#define STATUS_HANDLE_GAME 2
#define SIGNAL_CHECKLOGIN "SIGNAL_CHECKLOGIN"
#define SIGNAL_CREATEUSER "SIGNAL_CREATEUSER"
#define SIGNAL_OK "SIGNAL_OK"
#define SIGNAL_ERROR "SIGNAL_ERROR"
#define SIGNAL_CLOSE "SIGNAL_CLOSE"

// game
#define SIGNAL_NEWGAME "SIGNAL_NEWGAME"
#define SIGNAL_ABORTGAME "SIGNAL_ABORTGAME"
#define SIGNAL_TURN "SIGNAL_TURN"
#define SIGNAL_WIN "SIGNAL_WIN"
#define SIGNAL_LOST "SIGNAL_LOST"
#define SIGNAL_VIEWLOG "SIGNAL_VIEWLOG"
#define SIGNAL_LOGLINE "SIGNAL_LOGLINE"
#define SIGNAL_CHECK_FALSE "SIGNAL_CHECK_FALSE"
#define SIGNAL_CHECK_TRUE "SIGNAL_CHECK_TRUE"
// show info
#define INFO_QUIT "Quit caro game (y or n)?"
#define INFO_WIN "You Win. Do you want to view log (y or n)?"

// caro ranking
#define SIGNAL_RANKING "SIGNAL_RANKING"

// client connect to server
struct sockaddr_in server_addr;
int PORT, sock, recieved, isCommunicating;
char* serverAddress;
char send_msg[BUFF_SIZE], recv_msg[BUFF_SIZE];

// client variable
int status; // status of fungame
char choice, token[] = "#";
char error[100], user[100], id[30];

// draw table
char *table;
int size=9, playerTurn, col, row, userVal;
int viewC = 0, viewR = 0;

// trang thai choi
int checkStatus;


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
    clearScreen();
    printf("----------HARDGAME----------\n");
    printf("\tSign in\n");
    printf("---------------------------\n");
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
    printf("Username: ");
    fgets(user, BUFF_SIZE, stdin);
    user[strlen(user)-1] = '\0';

    printf("Password: ");
    getPassword(pass);

    //check username and password
    sprintf(send_msg, "%s#%s#%s", SIGNAL_CHECKLOGIN, user, pass);
    if(connectToServer() == 0){
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
    }
    else {
      printf("Error! Cant connect to server!\n");
      return -1;
    }
  }
  // return 0;
}

/*
Hiển thị phần đăng kí
*/
int menuRegister(){
  error[0] = '\0';
  char pass[100], comfirmPass[100], *str;
  while(1){
    clearScreen();
    printf("----------HARDGAME----------\n");
    printf("\tRegister\n");
    printf("---------------------------\n");
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
    printf("Username: ");
    fgets(user, BUFF_SIZE, stdin);
    user[strlen(user)-1] = '\0';

    printf("Password: ");
    getPassword(pass);
    printf("\nConfirm password: ");
    getPassword(comfirmPass);
    if(strcmp(pass, comfirmPass) == 0){
      // register new account
      sprintf(send_msg, "%s#%s#%s", SIGNAL_CREATEUSER, user, pass);
      if(connectToServer() == 0){
      	str = strtok(recv_msg, token);
      	if(strcmp(str, SIGNAL_OK) == 0)
      	  break;
      	else if(strcmp(str, SIGNAL_ERROR) == 0){
      	  str = strtok(NULL, token);
      	  strcpy(error, str);
      	}
      }
      else {
        printf("Error! Cant connect to server!\n");
        return -1;
      }
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
int menuStart(){
  int checkSignin = 0, checkRegister = 0;
  error[0] = '\0';
  while(1){
    clearScreen();
    if(error[0] != '\0'){
      printf("Error: %s!\n", error );
      error[0] = '\0';
    }
    printf("----------HARDGAME----------\n");
    printf("\t1.Sign in\n");
    printf("\t2.Register\n");
    printf("\t3.Exit\n");
    printf("---------------------------\n");
    printf("Your choice: ");
    scanf("%c", &choice);
    while(getchar() != '\n');
    if(choice == '1'){
      checkSignin = menuSignin();
      if( checkSignin == 0){
        break;
      } else if( checkSignin == -1)
        return -1;
    }
    else if(choice == '2'){
      checkRegister = menuRegister();
      if(checkRegister == 0)
        break;
      else if( checkRegister == -1)
        return -1;
    }
    else if(choice == '3') return -1;
    else sprintf(error,"No option %c", choice);
  }

  return 0;
}

/*
Hiển thị phần game caro, chọn size để chơi
*/
int menuGame(){
  error[0] = '\0';
  char* str;
  while(1){
    clearScreen();
    printf("----------HARDGAME----------\n");
    printf("\033[0;33m\tUsername: %s \033[0;37m\n", user);
    printf("\tNEW GAME\n");
    printf("---------------------------\n");
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
    if(connectToServer() == 0){
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
    }else {
        printf("Error! Cant connect to server!\n");
    }
  }
  return 0;
}


int handleRanking(){
  char* str;
  error[0] = '\0';

  clearScreen();
  printf("----------HARDGAME----------\n");
  printf("\033[0;33m\tUsername: %s \033[0;37m\n", user);
  printf("--------Sudoku Ranking--------\n");
  printf("Your information: \n");
  sprintf(send_msg, "%s#%s", SIGNAL_RANKING, user);
  if(connectToServer() == 0){
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
  }
  else {
    printf("Error! Cant connect to server!\n");
  }
  printf("Press 'q' to quit: ");
  choice = getchar();
  while(getchar() != '\n');
  if(choice == 'q' || choice == 'Q') return 1;
  else return 1;
}


int menuHardGame(){
  error[0] = '\0';
  while(1){
    clearScreen();
    if(error[0] != '\0'){
      printf("Error: %s!\n", error);
      error[0] = '\0';
    }
    printf("----------FUNGAME----------\n");
    printf("\033[0;33m\tWelcome %s \033[0;37m\n", user);
    printf("\t1.Sudoku game\n");
    printf("\t2.Sudoku ranking\n");
    printf("\t3.Sign out\n");
    printf("---------------------------\n");
    printf("Your choice: ");
    scanf("%c", &choice);
    while(getchar() != '\n');
    if(choice == '1'){
      checkStatus = 1;
      if(menuGame() == 0) break;
    }
    else if(choice == '2'){
      checkStatus = 2;
      break;
    }
    else if(choice == '3'){
      return -1;
    }
    else {
      sprintf(error,"%c Not an optional", choice);
    }
  }
  return 0;
}

/*
View log
*/
int viewLog(){
  char usernameLog[100], *str;

  sprintf(send_msg, "%s#%s", SIGNAL_VIEWLOG, id);
  if(connectToServer() == 0){
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
      send(sock, send_msg, strlen(send_msg), 0);
      close(sock);
      isCommunicating = 0;
      str = strtok(NULL, token);
      strcpy(error, str);
      return -1;
    }
    printLog(usernameLog); // in log
  }
  else {
    printf("Error! Cant connect to server!\n");
  }

  printf("Press 'q' to quit: ");
  choice = getchar();
  while(getchar() != '\n');
  if(choice == 'q' || choice == 'Q') return 0;
  else return 0;
}

/*
Vẽ bảng chơi caro
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
int handleGame(){
  char c, info[100], *str;
  setCustomTerminal();
  error[0] = '\0';
  info[0] = '\0';

  playerTurn = 1;
  col = size / 2;
  row = size / 2;
  while(1){
    clearScreen();
    printf("\033[0;37m----------CARO GAME----------\n");
    printf("\033[0;33m\tUsername: %s - GameID = %s\033[0;37m\n", user, id);
    printf("\tPress w,a,s,d to move \n");
    printf("\tPress 1 -> 9 to fill\n");
    printf("\tNeu ban dien sai thi se ko hien so va bi tru diem\n");
    printf("\tPress 'q' to quit\n");
    printf("-----------------------------\n");
    drawTable();
    if(playerTurn){
      if(error[0] != '\0'){
      	setDefaultTerminal();

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
      	      if(viewLog() == 0){
            		sprintf(send_msg, "%s#%s#%s",SIGNAL_ABORTGAME, id, user);
            		connectToServer();
            		return -1;
      	      }
      	    }
      	  }
      	}
      	setCustomTerminal();
      }
      else if(info[0] != '\0'){
      	setDefaultTerminal();

      	printf("\033[0;37m%s\n", info);
      	printf("Your choice: ");
      	choice = getchar();
      	while(getchar() != '\n');
      	if(choice == 'n' || choice == 'N'){
      	  if(strcmp(info, INFO_QUIT) == 0)
      	    info[0] = '\0';
      	  else if(strcmp(info, INFO_WIN) == 0){
      	    sprintf(send_msg, "%s#%s#%s",SIGNAL_ABORTGAME, id, user);
      	    connectToServer();
      	    return -1;
      	  }
      	}
      	else if(choice == 'y' || choice == 'Y'){
      	  if(strcmp(info, INFO_QUIT) == 0){
      	    sprintf(send_msg, "%s#%s#%s",SIGNAL_ABORTGAME, id, user);
      	    connectToServer();
      	    return -1;
      	  }
      	  else if(strcmp(info, INFO_WIN) == 0){
      	    if(viewLog() == 0){
      	      sprintf(send_msg, "%s#%s#%s",SIGNAL_ABORTGAME, id, user);
      	      connectToServer();
      	      return -1;
      	    }
      	  }
      	}
      	setCustomTerminal();
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
      if(connectToServer() == 0){
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
      }
      playerTurn = 1;
    }
  }
  setDefaultTerminal();
  return 0;
}

int main(int argc, char* argv[]){
  int err;
  if(argc != 3){
    printf("Syntax Error.\n");
    printf("Syntax: ./client IPAddress PortNumber\n");
    return 0;
  }
  if(check_IP(argv[1]) == 0){
    printf("IP address invalid\n");
    return 0;
  }
  if(check_port(argv[2]) == 0){
    printf("Port invalid\n");
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
}
