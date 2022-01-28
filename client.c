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


#include <termios.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>

#include "lib/clientlist.h"
#include "lib/ranking.h"
#include "lib/log.h"
#include "lib/authenticate.h"
#include "lib/terminal.h"


#define STATUS_START_MENU 0
#define STATUS_HARDGAME_MENU 1
#define STATUS_HANDLE_GAME 2
#define SIGNAL_CHECKLOGIN "SIGNAL_CHECKLOGIN"
#define SIGNAL_CREATEUSER "SIGNAL_CREATEUSER"
#define SIGNAL_OK "SIGNAL_OK"
#define SIGNAL_ERROR "SIGNAL_ERROR"
#define SIGNAL_CLOSE "SIGNAL_CLOSE"

// game
#define SIGNAL_NEWGAME "SIGNAL_NEWGAME"
#define SIGNAL_2PLAYERS "SIGNAL_2PLAYERS"
#define SIGNAL_CHAT "SIGNAL_CHAT"
#define SIGNAL_ABORTGAME "SIGNAL_ABORTGAME"
#define SIGNAL_INPUT "SIGNAL_INPUT"
#define SIGNAL_INPUT2 "SIGNAL_INPUT2"
#define SIGNAL_WIN "SIGNAL_WIN"
#define SIGNAL_LOST "SIGNAL_LOST"
#define SIGNAL_VIEWLOG "SIGNAL_VIEWLOG"
#define SIGNAL_LOGLINE "SIGNAL_LOGLINE"
#define SIGNAL_CHECK_FALSE "SIGNAL_CHECK_FALSE"
#define SIGNAL_CHECK_TRUE "SIGNAL_CHECK_TRUE"
#define SIGNAL_LIST "LIST"
#define SIGNAL_CONNECT "CONNECT"
#define SIGNAL_NOT_FOUND "NOT FOUND"
// show info
#define INFO_QUIT "Quit game (y or n)?"
#define INFO_WIN "You Win. Do you want to view log (y or n)?"

// ranking
#define SIGNAL_RANKING "SIGNAL_RANKING"


#define SERVER_PORT 9000
#define MAX_NAME_SZE 20
#define BUFF_SIZE 1024

char send_msg[BUFF_SIZE], recv_msg[BUFF_SIZE];
char client_name[MAX_NAME_SZE] ={0};


int status; // status of fungame
char choice, token[] = "#";
char error[100], user[100], id[30];

// draw table
char *table;
int playerTurn, col, row, userVal;

// trang thai choi
int checkStatus;

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
    client_send_to_server(listen_fd,send_msg);
    printf("%s\n",send_msg );
    client_recv_from_server(listen_fd,recv_msg);
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
     return -1;

  }
  // return 0;
}

/*
Hiển thị phần đăng kí
*/
int menuRegister(int listen_fd){
  error[0] = '\0';
  char pass[100], comfirmPass[100], *str;
  while(1){
    memset(send_msg, 0 ,sizeof(send_msg));
    memset(recv_msg, 0 ,sizeof(recv_msg));
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
      client_send_to_server(listen_fd,send_msg);
      client_recv_from_server(listen_fd,recv_msg);
      	str = strtok(recv_msg, token);
      	if(strcmp(str, SIGNAL_OK) == 0)
      	  break;
      	else if(strcmp(str, SIGNAL_ERROR) == 0){
      	  str = strtok(NULL, token);
      	  strcpy(error, str);
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
int menuStart(int listen_fd){
  int checkSignin = 0, checkRegister = 0;
  error[0] = '\0';
  while(1){
    memset(send_msg, 0 ,sizeof(send_msg));
    memset(recv_msg, 0 ,sizeof(recv_msg));
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
      checkSignin = menuSignin(listen_fd);
      if( checkSignin == 0){
        break;
      } else if( checkSignin == -1)
        return -1;
    }
    else if(choice == '2'){
      checkRegister = menuRegister(listen_fd);
      if(checkRegister == 0)
        break;
      else if( checkRegister == -1)
        return -1;
    }
    else if(choice == '3') {
      close(listen_fd);
      exit(-1);
    }
    else sprintf(error,"No option %c", choice);
  }

  return 0;
}

/*
Hiển thị phần game
*/
int menuGame(int listen_fd){
  error[0] = '\0';
  char* str;
  while(1){
    memset(send_msg, 0 ,sizeof(send_msg));
    memset(recv_msg, 0 ,sizeof(recv_msg));
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

    sprintf(send_msg, "%s#%s", SIGNAL_NEWGAME, user);
    client_send_to_server(listen_fd,send_msg);
    client_recv_from_server(listen_fd,recv_msg);
      str = strtok(recv_msg, token);
      if(strcmp(str, SIGNAL_OK) == 0){
      	str = strtok(NULL, token);
      	strcpy(id, str);
      	  //init table
        str = strtok(NULL, token);
      	table = malloc(81);
      	for(int i = 0; i < 81; i++){
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
  }
  return 0;
}


int handleRanking(int listen_fd){
  char* str;
  error[0] = '\0';
  memset(send_msg, 0 ,sizeof(send_msg));
  memset(recv_msg, 0 ,sizeof(recv_msg));
  clearScreen();
  printf("----------HARDGAME----------\n");
  printf("\033[0;33m\tUsername: %s \033[0;37m\n", user);
  printf("--------Sudoku Ranking--------\n");
  printf("Your information: \n");
  sprintf(send_msg, "%s#%s", SIGNAL_RANKING, user);
  client_send_to_server(listen_fd,send_msg);
  client_recv_from_server(listen_fd,recv_msg);

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
      str = strtok(NULL, token);
      FILE *out=fopen("ranking.txt","w");
	    fputs(str,out);
	    fclose(out);
      readFileRanking("ranking.txt");
      printf("%-5s%-20s%-10s%-10s\n", "TOP", "ID", "Win", "Point");
      if(numberOfWin == -1) printfRankingNotID();
      else printfRanking(id);
      rootRank = NULL; cur = NULL; new = NULL;
      // write_file(listen_fd);
    }


  printf("Press 'q' to quit: ");
  choice = getchar();
  while(getchar() != '\n');
  if(choice == 'q' || choice == 'Q') return 1;
  else return 1;
}



int menuHardGame(int listen_fd){
  error[0] = '\0';
  while(1){
    memset(send_msg, 0 ,sizeof(send_msg));
    memset(recv_msg, 0 ,sizeof(recv_msg));
    clearScreen();
    if(error[0] != '\0'){
      printf("Error: %s!\n", error);
      error[0] = '\0';
    }
    printf("\t ______________________________\n");
    printf("\t|----------HARDGAME------------|\n");
    printf("\t|\033[0;33m\tWelcome %s \033[0;37m        |\n", user);
    printf("\t|1.Sudoku game                 |\n");
    printf("\t|2.Sudoku 2 players            |\n");
    printf("\t|3.Sudoku ranking              |\n");
    printf("\t|4.Sign out                    |\n");
    printf("\t|______________________________|\n");
    printf("\tYour choice: ");
    scanf("%c", &choice);
    while(getchar() != '\n');
    if(choice == '1'){
      checkStatus = 1;
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
int viewLog(int listen_fd){
  char usernameLog[100], *str;
  memset(send_msg, 0 ,sizeof(send_msg));
  memset(recv_msg, 0 ,sizeof(recv_msg));
  sprintf(send_msg, "%s#%s", SIGNAL_VIEWLOG, id);
  client_send_to_server(listen_fd,send_msg);
  client_recv_from_server(listen_fd,recv_msg);

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
      str = strtok(NULL,token);

    }
    else if(strcmp(str, SIGNAL_ERROR) == 0){
      strcpy(send_msg, SIGNAL_CLOSE);

      client_send_to_server(listen_fd,send_msg);
      close(listen_fd);

      str = strtok(NULL, token);
      strcpy(error, str);
      return -1;
    }
  printf("%s",str );
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

  for(i = 0; i < 9; i++){
    if(i == 3 || i == 6){
      printf("%s","                             \n");
    }
    for(j = 0; j < 9; j++){
      if(j == 3 || j == 6){
        printf("%s"," ");
      }
        if(i == row && j == col){
          printf("\033[0;37m[\033[0;37m%c\033[0;37m]", table[i * 9 + j]); // white
        }
        else{
          printf("\033[0;36m[\033[0;33m%c\033[0;36m]", table[i * 9 + j]); // yellow

        }
      }
      printf("\n");
  }
}

/*
Xử lí, điều khiển quân bằng w,a,s,d, ấn q thì thoát
*/
int handleGame(int listen_fd){
  char c, info[100], *str;
  setPrivateTerminal();
  error[0] = '\0';
  info[0] = '\0';

  playerTurn = 1;
  col = 4;
  row = 4;
  int firstPoint = 3;
  while(1){
    memset(send_msg, 0 ,sizeof(send_msg));
    memset(recv_msg, 0 ,sizeof(recv_msg));
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
      	  else if(strcmp(strstr(info, INFO_WIN),INFO_WIN) == 0){
      	    if(choice == 'y' || choice == 'Y'){
      	      if(viewLog(listen_fd) == 0){
            		sprintf(send_msg, "%s#%s#%s",SIGNAL_ABORTGAME, id, user);
                client_send_to_server(listen_fd,send_msg);
                client_recv_from_server(listen_fd,recv_msg);
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
      	  if(strcmp(info, INFO_QUIT) == 0 || strcmp(strstr(info, INFO_QUIT),INFO_QUIT) == 0)
      	    info[0] = '\0';
      	  else if(strcmp(strstr(info, INFO_WIN),INFO_WIN) == 0){
      	    sprintf(send_msg, "%s#%s#%s",SIGNAL_ABORTGAME, id, user);
            client_send_to_server(listen_fd,send_msg);
            client_recv_from_server(listen_fd,recv_msg);
      	    return -1;
      	  }
      	}
      	else if(choice == 'y' || choice == 'Y'){
      	  if(strcmp(info, INFO_QUIT) == 0 || strcmp(strstr(info, INFO_QUIT),INFO_QUIT) == 0){
      	    sprintf(send_msg, "%s#%s#%s",SIGNAL_ABORTGAME, id, user);
            client_send_to_server(listen_fd,send_msg);
            client_recv_from_server(listen_fd,recv_msg);
      	    return -1;
      	  }
      	  else if(strcmp(strstr(info, INFO_WIN),INFO_WIN) == 0){
      	    if(viewLog(listen_fd) == 0){
      	      sprintf(send_msg, "%s#%s#%s",SIGNAL_ABORTGAME, id, user);
              client_send_to_server(listen_fd,send_msg);
              client_recv_from_server(listen_fd,recv_msg);
      	      return -1;
      	    }
      	  }
      	}

      	setPrivateTerminal();

      }
      else{
      	c = getchar();
      	if(c == 'w' && row > 0) row--;
      	else if(c == 's' && row < 9 - 1) row++;
      	else if(c == 'd' && col < 9 - 1) col++;
      	else if(c == 'a' && col > 0) col--;
        else if(c == '1' && table[row * 9 + col] == ' '){
      	  table[row * 9 + col] = '1';
          playerTurn = 0;
          userVal = 1;
      	}
        else if(c == '2' && table[row * 9 + col] == ' '){
      	  table[row * 9 + col] = '2';
          playerTurn = 0;
          userVal = 2;
      	}
        else if(c == '3' && table[row * 9 + col] == ' '){
      	  table[row * 9 + col] = '3';
          playerTurn = 0;
          userVal = 3;
      	}
        else if(c == '4' && table[row * 9 + col] == ' '){
      	  table[row * 9 + col] = '4';
          playerTurn = 0;
          userVal = 4;
      	}
        else if(c == '5' && table[row * 9 + col] == ' '){
      	  table[row * 9 + col] = '5';
          playerTurn = 0;
          userVal = 5;
      	}
        else if(c == '6' && table[row * 9 + col] == ' '){
      	  table[row * 9 + col] = '6';
          playerTurn = 0;
          userVal = 6;
      	}
        else if(c == '7' && table[row * 9 + col] == ' '){
      	  table[row * 9 + col] = '7';
          playerTurn = 0;
          userVal = 7;
      	}
        else if(c == '8' && table[row * 9 + col] == ' '){
      	  table[row * 9 + col] = '8';
          playerTurn = 0;
          userVal = 8;
      	}
        else if(c == '9' && table[row * 9 + col] == ' '){
      	  table[row * 9 + col] = '9';
          playerTurn = 0;
          userVal = 9;
      	}
      	else if(c == 'q') strcpy(info, INFO_QUIT);
      }
    }
    else {
      sprintf(send_msg, "%s#%s#%s#%d#%d#%d", SIGNAL_INPUT, id, user, col, row, userVal);
      client_send_to_server(listen_fd,send_msg);
      client_recv_from_server(listen_fd,recv_msg);
      	str = strtok(recv_msg, token);
      	if(strcmp(str, SIGNAL_INPUT) == 0){
      	  str = strtok(NULL, token);
          if(strcmp(str, SIGNAL_CHECK_FALSE) == 0){
              table[row * 9 + col] = ' ';
              firstPoint--;
              char result[100];
              sprintf(result,"You lose 1 point, you have %d points left! %s",firstPoint,INFO_QUIT);
              strcpy(info,result);


          }
      	}
      	else if(strcmp(str, SIGNAL_WIN) == 0){
          char result[100];
          str = strtok(NULL, token);

          sprintf(result,"Result: %s\n%s",str,INFO_WIN);
          strcpy(info,result);
        }
      	else if(strcmp(str, SIGNAL_ERROR) == 0){
      	  str = strtok(NULL, token);
      	  strcpy(error, str);
      	}
      playerTurn = 1;
    }
  }
  setNormalTerminal();
  return 0;
}

void handleGame2Players(int listen_fd){
  clearScreen();
  printf("\033[0;37m-------------------------ROOM---------------------\n");
  printf("\tWelcome to %s\n",user);
  printf("\tNhap LIST de xem danh sach nguoi dang online\n");
  printf("\tNhap CONNECT:username de ket noi voi nguoi do\n");
  printf("\tNhan q de thoat\n");
  printf("-------------------------------------------------------\n");
  char firstPlayer[30];
  int out = 0;
  while(1){
    if(out == 1) break;
    memset(send_msg, 0 ,sizeof(send_msg));
    memset(recv_msg, 0 ,sizeof(recv_msg));

    scanf("%s",send_msg );

    if(strcmp(send_msg,"q")==0) break;

    send(listen_fd, send_msg, strlen(send_msg), 0);
    // client_send_to_server(listen_fd,send_buff);
    client_recv_from_server(listen_fd,recv_msg);

    if(strcmp(send_msg,SIGNAL_LIST)==0){
      printf("%s\n",recv_msg );
      continue;
    }
    if(strcmp(recv_msg,SIGNAL_ERROR)==0){
      printf("%s\n","Syntax error" );
      continue;
    }
    if(strcmp(recv_msg,SIGNAL_NOT_FOUND)==0){
      printf("%s\n","Not found" );
      continue;
    }


    error[0] = '\0';
    char* str;

    while(1){
      if(out == 1) break;
      if(error[0] != '\0'){
        printf("Error: %s!\n", error);
        printf("Do you want to try again? (y or n): ");
        choice = getchar();
        while(getchar() != '\n');
        if(choice =='n' || choice =='N'){
          error[0] = '\0';
          out = 1;
          break;
        }
        else if(choice !='y' && choice !='Y')
          continue;
        else{
          error[0] = '\0';
          continue;
        }
      }

        str = strtok(recv_msg, token);
        if(strcmp(str, SIGNAL_OK) == 0){
          str = strtok(NULL, token);
          strcpy(id, str);
          str = strtok(NULL, token);
          table = malloc(81);
          for(int i = 0; i < 81; i++){
            if(str[i] != '0')
               table[i] = str[i];
            else
                table[i] = ' ';
          }
          str = strtok(NULL,token);
          strcpy(firstPlayer,str);
        }
        else if(strcmp(str, SIGNAL_ERROR) == 0){
           str = strtok(NULL, token);
           strcpy(error, str);
        }
        char c, info[100];
        setPrivateTerminal();
        error[0] = '\0';
        info[0] = '\0';
        if(strcmp(user,firstPlayer) != 0)
          playerTurn = 1;
        else
          playerTurn = 0;
        col = 4;
        row = 4;
        while(1){
          if(out == 1) break;
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
            if(info[0] != '\0'){
              setNormalTerminal();

              printf("\033[0;37m%s\n", info);
              while(choice != 'q'){
                choice = getchar();
                if(choice == 'q'){
                  out = 1;
                  continue;
                }
              }
              setPrivateTerminal();
            }

            else{
              c = getchar();
              if(c == 'w' && row > 0) row--;
              else if(c == 's' && row < 9 - 1) row++;
              else if(c == 'd' && col < 9 - 1) col++;
              else if(c == 'a' && col > 0) col--;
              else if(c == '1' && table[row * 9 + col] == ' '){
                table[row * 9 + col] = '1';
                userVal = 1;
                sprintf(send_msg, "%s#%s#%s#%d#%d#%d", SIGNAL_INPUT2, id, user, col, row, userVal);
                client_send_to_server(listen_fd,send_msg);
                client_recv_from_server(listen_fd,recv_msg);
                  str = strtok(recv_msg, token);
                  if(strcmp(str, SIGNAL_INPUT2) == 0){
                    str = strtok(NULL, token);
                    if(strcmp(str, SIGNAL_CHECK_FALSE) == 0){
                      str = strtok(NULL, token); // //get column
                      col = atoi(str);

                      str = strtok(NULL, token); //get row
                      row = atoi(str);
                        table[row * 9 + col] = ' ';
                        playerTurn = 0;
                    }
                  }
                  else if(strcmp(str, SIGNAL_WIN) == 0){
                    char result[100];
                    char point[10];
                    char winner[30];
                    str = strtok(NULL, token);
                    strcpy(point,str);
                    str = strtok(NULL, token);
                    strcpy(winner,str);
                    sprintf(result,"Result: %s\n%s",point,"You win");
                    strcpy(info,result);

                    continue;
                  }
                  else if(strcmp(str, SIGNAL_ERROR) == 0){
                    str = strtok(NULL, token);
                    strcpy(error, str);
                    continue;
                  }
              }
              else if(c == '2' && table[row * 9 + col] == ' '){
                table[row * 9 + col] = '2';

                userVal = 2;
                sprintf(send_msg, "%s#%s#%s#%d#%d#%d", SIGNAL_INPUT2, id, user, col, row, userVal);
                client_send_to_server(listen_fd,send_msg);
                client_recv_from_server(listen_fd,recv_msg);
                  str = strtok(recv_msg, token);
                  if(strcmp(str, SIGNAL_INPUT2) == 0){
                    str = strtok(NULL, token);
                    if(strcmp(str, SIGNAL_CHECK_FALSE) == 0){
                      str = strtok(NULL, token); // //get column
                      col = atoi(str);

                      str = strtok(NULL, token); //get row
                      row = atoi(str);
                        table[row * 9 + col] = ' ';
                        playerTurn = 0;
                    }
                  }
                  else if(strcmp(str, SIGNAL_WIN) == 0){
                    char result[100];
                    char point[10];
                    char winner[30];
                    str = strtok(NULL, token);
                    strcpy(point,str);
                    str = strtok(NULL, token);
                    strcpy(winner,str);
                    sprintf(result,"Result: %s\n%s",point,"You win");
                    strcpy(info,result);
                    continue;
                  }
                  else if(strcmp(str, SIGNAL_ERROR) == 0){
                    str = strtok(NULL, token);
                    strcpy(error, str);
                    continue;
                  }
              }
              else if(c == '3' && table[row * 9 + col] == ' '){
                table[row * 9 + col] = '3';

                userVal = 3;
                sprintf(send_msg, "%s#%s#%s#%d#%d#%d", SIGNAL_INPUT2, id, user, col, row, userVal);
                client_send_to_server(listen_fd,send_msg);
                client_recv_from_server(listen_fd,recv_msg);
                  str = strtok(recv_msg, token);
                  if(strcmp(str, SIGNAL_INPUT2) == 0){
                    str = strtok(NULL, token);
                    if(strcmp(str, SIGNAL_CHECK_FALSE) == 0){
                      str = strtok(NULL, token); // //get column
                      col = atoi(str);

                      str = strtok(NULL, token); //get row
                      row = atoi(str);
                        table[row * 9 + col] = ' ';
                        playerTurn = 0;
                    }
                  }
                  else if(strcmp(str, SIGNAL_WIN) == 0){
                    char result[100];
                    char point[10];
                    char winner[30];
                    str = strtok(NULL, token);
                    strcpy(point,str);
                    str = strtok(NULL, token);
                    strcpy(winner,str);
                    sprintf(result,"Result: %s\n%s",point,"You win");
                    strcpy(info,result);
                  }
                  else if(strcmp(str, SIGNAL_ERROR) == 0){
                    str = strtok(NULL, token);
                    strcpy(error, str);
                    continue;
                  }
              }
              else if(c == '4' && table[row * 9 + col] == ' '){
                table[row * 9 + col] = '4';

                userVal = 4;
                sprintf(send_msg, "%s#%s#%s#%d#%d#%d", SIGNAL_INPUT2, id, user, col, row, userVal);
                client_send_to_server(listen_fd,send_msg);
                client_recv_from_server(listen_fd,recv_msg);
                  str = strtok(recv_msg, token);
                  if(strcmp(str, SIGNAL_INPUT2) == 0){
                    str = strtok(NULL, token);
                    if(strcmp(str, SIGNAL_CHECK_FALSE) == 0){
                      str = strtok(NULL, token); // //get column
                      col = atoi(str);

                      str = strtok(NULL, token); //get row
                      row = atoi(str);
                        table[row * 9 + col] = ' ';
                        playerTurn = 0;
                    }
                  }
                  else if(strcmp(str, SIGNAL_WIN) == 0){
                    char result[100];
                    char point[10];
                    char winner[30];
                    str = strtok(NULL, token);
                    strcpy(point,str);
                    str = strtok(NULL, token);
                    strcpy(winner,str);
                    sprintf(result,"Result: %s\n%s",point,"You win");
                    strcpy(info,result);
                    continue;
                  }
                  else if(strcmp(str, SIGNAL_ERROR) == 0){
                    str = strtok(NULL, token);
                    strcpy(error, str);
                    continue;
                  }
              }
              else if(c == '5' && table[row * 9 + col] == ' '){
                table[row * 9 + col] = '5';

                userVal = 5;
                sprintf(send_msg, "%s#%s#%s#%d#%d#%d", SIGNAL_INPUT2, id, user, col, row, userVal);
                client_send_to_server(listen_fd,send_msg);
                client_recv_from_server(listen_fd,recv_msg);
                  str = strtok(recv_msg, token);
                  if(strcmp(str, SIGNAL_INPUT2) == 0){
                    str = strtok(NULL, token);
                    if(strcmp(str, SIGNAL_CHECK_FALSE) == 0){
                      str = strtok(NULL, token); // //get column
                      col = atoi(str);

                      str = strtok(NULL, token); //get row
                      row = atoi(str);
                        table[row * 9 + col] = ' ';
                        playerTurn = 0;
                    }
                  }
                  else if(strcmp(str, SIGNAL_WIN) == 0){
                    char result[100];
                    char point[10];
                    char winner[30];
                    str = strtok(NULL, token);
                    strcpy(point,str);
                    str = strtok(NULL, token);
                    strcpy(winner,str);
                    sprintf(result,"Result: %s\n%s",point,"You win");
                    strcpy(info,result);
                    continue;
                  }
                  else if(strcmp(str, SIGNAL_ERROR) == 0){
                    str = strtok(NULL, token);
                    strcpy(error, str);
                    continue;
                  }
              }
              else if(c == '6' && table[row * 9 + col] == ' '){
                table[row * 9 + col] = '6';

                userVal = 6;
                sprintf(send_msg, "%s#%s#%s#%d#%d#%d", SIGNAL_INPUT2, id, user, col, row, userVal);
                client_send_to_server(listen_fd,send_msg);
                client_recv_from_server(listen_fd,recv_msg);
                  str = strtok(recv_msg, token);
                  if(strcmp(str, SIGNAL_INPUT2) == 0){
                    str = strtok(NULL, token);
                    if(strcmp(str, SIGNAL_CHECK_FALSE) == 0){
                      str = strtok(NULL, token); // //get column
                      col = atoi(str);

                      str = strtok(NULL, token); //get row
                      row = atoi(str);
                        table[row * 9 + col] = ' ';
                        playerTurn = 0;
                    }
                  }
                  else if(strcmp(str, SIGNAL_WIN) == 0){
                    char result[100];
                    char point[10];
                    char winner[30];
                    str = strtok(NULL, token);
                    strcpy(point,str);
                    str = strtok(NULL, token);
                    strcpy(winner,str);
                    sprintf(result,"Result: %s\n%s",point,"You win");
                    strcpy(info,result);
                    continue;
                  }
                  else if(strcmp(str, SIGNAL_ERROR) == 0){
                    str = strtok(NULL, token);
                    strcpy(error, str);
                    continue;
                  }
              }
              else if(c == '7' && table[row * 9 + col] == ' '){
                table[row * 9 + col] = '7';

                userVal = 7;
                sprintf(send_msg, "%s#%s#%s#%d#%d#%d", SIGNAL_INPUT2, id, user, col, row, userVal);
                client_send_to_server(listen_fd,send_msg);
                client_recv_from_server(listen_fd,recv_msg);
                  str = strtok(recv_msg, token);
                  if(strcmp(str, SIGNAL_INPUT2) == 0){
                    str = strtok(NULL, token);
                    if(strcmp(str, SIGNAL_CHECK_FALSE) == 0){
                      str = strtok(NULL, token); // //get column
                      col = atoi(str);

                      str = strtok(NULL, token); //get row
                      row = atoi(str);
                        table[row * 9 + col] = ' ';
                        playerTurn = 0;
                    }
                  }
                  else if(strcmp(str, SIGNAL_WIN) == 0){
                    char result[100];
                    char point[10];
                    char winner[30];
                    str = strtok(NULL, token);
                    strcpy(point,str);
                    str = strtok(NULL, token);
                    strcpy(winner,str);
                    sprintf(result,"Result: %s\n%s",point,"You win");
                    strcpy(info,result);
                    continue;
                  }
                  else if(strcmp(str, SIGNAL_ERROR) == 0){
                    str = strtok(NULL, token);
                    strcpy(error, str);
                    continue;
                  }
              }
              else if(c == '8' && table[row * 9 + col] == ' '){
                table[row * 9 + col] = '8';

                userVal = 8;
                sprintf(send_msg, "%s#%s#%s#%d#%d#%d", SIGNAL_INPUT2, id, user, col, row, userVal);
                client_send_to_server(listen_fd,send_msg);
                client_recv_from_server(listen_fd,recv_msg);
                  str = strtok(recv_msg, token);
                  if(strcmp(str, SIGNAL_INPUT2) == 0){
                    str = strtok(NULL, token);
                    if(strcmp(str, SIGNAL_CHECK_FALSE) == 0){
                      str = strtok(NULL, token); // //get column
                      col = atoi(str);

                      str = strtok(NULL, token); //get row
                      row = atoi(str);
                        table[row * 9 + col] = ' ';
                        playerTurn = 0;
                    }
                  }
                  else if(strcmp(str, SIGNAL_WIN) == 0){
                    char result[100];
                    char point[10];
                    char winner[30];
                    str = strtok(NULL, token);
                    strcpy(point,str);
                    str = strtok(NULL, token);
                    strcpy(winner,str);
                    sprintf(result,"Result: %s\n%s",point,"You win");
                    strcpy(info,result);
                    continue;
                  }
                  else if(strcmp(str, SIGNAL_ERROR) == 0){
                    str = strtok(NULL, token);
                    strcpy(error, str);
                  }
              }
              else if(c == '9' && table[row * 9 + col] == ' '){
                table[row * 9 + col] = '9';

                userVal = 9;
                sprintf(send_msg, "%s#%s#%s#%d#%d#%d", SIGNAL_INPUT2, id, user, col, row, userVal);
                client_send_to_server(listen_fd,send_msg);
                client_recv_from_server(listen_fd,recv_msg);
                  str = strtok(recv_msg, token);
                  if(strcmp(str, SIGNAL_INPUT2) == 0){
                    str = strtok(NULL, token);
                    if(strcmp(str, SIGNAL_CHECK_FALSE) == 0){
                        str = strtok(NULL, token); // //get column
                        col = atoi(str);

                        str = strtok(NULL, token); //get row
                        row = atoi(str);
                        table[row * 9 + col] = ' ';
                        playerTurn = 0;
                    }
                  }
                  else if(strcmp(str, SIGNAL_WIN) == 0){
                    char result[100];
                    char point[10];
                    char winner[30];
                    str = strtok(NULL, token);
                    strcpy(point,str);
                    str = strtok(NULL, token);
                    strcpy(winner,str);
                    sprintf(result,"Result: %s\n%s",point,"You win");
                    strcpy(info,result);
                    continue;
                  }
                  else if(strcmp(str, SIGNAL_ERROR) == 0){
                    str = strtok(NULL, token);
                    strcpy(error, str);
                    continue;
                  }
              }
              else if(c == 'q'){
                strcpy(info, INFO_QUIT);
                continue;
              }

            }
          }
          else {
              client_recv_from_server(listen_fd,recv_msg);
              str = strtok(recv_msg, token);
              if(strcmp(str, SIGNAL_INPUT2) == 0){
                str = strtok(NULL, token);
                if(strcmp(str, SIGNAL_CHECK_FALSE) == 0){
                    str = strtok(NULL, token); // //get column
                    col = atoi(str);

                    str = strtok(NULL, token); //get row
                    row = atoi(str);
                    table[row * 9 + col] = ' ';
                    playerTurn = 1;
                }else if(strcmp(str, SIGNAL_CHECK_TRUE) == 0){
                  str = strtok(NULL, token); // //get column
                  col = atoi(str);

                  str = strtok(NULL, token); //get row
                  row = atoi(str);

                  str = strtok(NULL, token); //get row

                  table[row * 9 + col] = str[0];
                }
              }
              else if(strcmp(str, SIGNAL_WIN) == 0){
                char result[100];
                char point[10];
                char winner[30];
                str = strtok(NULL, token);
                strcpy(point,str);
                str = strtok(NULL, token);
                strcpy(winner,str);
                strcpy(info,"You lose!");
                str = strtok(NULL, token); // //get column
                col = atoi(str);

                str = strtok(NULL, token); //get row
                row = atoi(str);

                str = strtok(NULL, token); //get row

                table[row * 9 + col] = str[0];
                playerTurn=1;

                continue;
              }
              else if(strcmp(str, SIGNAL_ERROR) == 0){
                str = strtok(NULL, token);
                strcpy(error, str);
                continue;
              }
          }

        }
        setNormalTerminal();
    }
  }
}


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

    while(1) {
         max_fd = client_build_fdsets(listen_fd, &readfds, &writefds, &exceptfds);
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
    server_addr.sin_addr.s_addr = inet_addr("172.16.0.20");


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
          handleGame2Players(*listen_fd);
          out:
          free(table);
          table = NULL;
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
     memset(recv_msg, 0 ,sizeof(recv_msg));
     if((read_bytes = recv(client_socket, recv_msg, BUFF_SIZE, 0)) > 0) {
            // printf("%s\n",recv_msg);
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
    char recv_msgg[BUFF_SIZE] ;
    char send_buff[BUFF_SIZE] ;
    memset(recv_msgg, 0 ,sizeof(recv_msgg));
    memset(send_buff, 0 ,sizeof(send_buff));

    int action = select(max_fd+1,readfds,writefds,NULL,NULL);


    if(action == -1 || action == 0) {
        perror("ERROR: select");
        exit(0);
    }
    scanf("%s\n",send_buff );
    printf("%s\n",send_buff );
    send(listen_fd, send_buff, strlen(send_buff), 0);
    // client_send_to_server(listen_fd,send_buff);
    client_recv_from_server(listen_fd,recv_msgg);


    return 0;
}
