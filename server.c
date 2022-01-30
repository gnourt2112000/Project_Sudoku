#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>


#include "lib/clientlist.h"
#include "lib/solvesudoku.h"
#include "lib/ranking.h"
#include "lib/log.h"
#include "lib/authenticate.h"
//Macros

#define BUFF_SIZE 1024
#define SERVER_PORT 9000
#define LISTEN_BACKLOG 5
#define MAX_NAME_SZE 20
#define NO_OF_CLIENTS 20
#define MAX_BUFFER_SIZE 1024
#define CONNECTED "Connected ....."


// Auth
#define SIGNAL_CHECKLOGIN "SIGNAL_CHECKLOGIN"
#define SIGNAL_CREATEUSER "SIGNAL_CREATEUSER"
#define SIGNAL_OK "SIGNAL_OK"
#define SIGNAL_ERROR "SIGNAL_ERROR"
#define SIGNAL_CLOSE "SIGNAL_CLOSE"

// sudoku game
#define SIGNAL_NEWGAME "SIGNAL_NEWGAME"
#define SIGNAL_2PLAYERS "SIGNAL_2PLAYERS"
#define SIGNAL_ABORTGAME "SIGNAL_ABORTGAME"
#define SIGNAL_INPUT "SIGNAL_INPUT"
#define SIGNAL_INPUT2 "SIGNAL_INPUT2"
#define SIGNAL_CHECK_FALSE "SIGNAL_CHECK_FALSE"
#define SIGNAL_CHECK_TRUE "SIGNAL_CHECK_TRUE"
#define SIGNAL_WIN "SIGNAL_WIN"
#define SIGNAL_LOST "SIGNAL_LOST"
#define SIGNAL_VIEWLOG "SIGNAL_VIEWLOG"
#define SIGNAL_LOGLINE "SIGNAL_LOGLINE"
#define SIGNAL_LIST "LIST"
#define SIGNAL_CONNECT "CONNECT"
#define SIGNAL_CAN_NOT_CONNECT "CAN'T CONNECT"

// sudoku ranking
#define SIGNAL_RANKING "SIGNAL_RANKING"


char *str;
char send_msg[BUFF_SIZE] , recv_msg[BUFF_SIZE];

static int listen_fd = 0;

typedef struct sudoku
{
    /* data */
    char data[82];

}Sudoku;


//Data structure
struct client {
    char cname[MAX_NAME_SZE];
    char playwith[MAX_NAME_SZE];
    int playwith_fd;
    int file_des;
    int port;
    char ip[INET_ADDRSTRLEN];
};
struct server_data {
    int total_client;
    struct client client_list[NO_OF_CLIENTS];
};

struct server_data server;
char send_msg[BUFF_SIZE] , recv_msg[BUFF_SIZE];

// server variable
char token[] ="#";
char game[82];
// int tttResult;
char* puzzle = NULL;
char* tempPuzzle = NULL;
char* userPuzzle = NULL;
int result;
int count;
int r;

struct sockaddr_in client_addr;
//Function declarations
int server_create_socket(int *listen_fd);
int server_new_client_handle(int listen_fd, int *new_socket_fd,char*buffer);
int server_recv_from_client(int socket_client, char *recv_msg);
int server_send_to_client(int socket_client, char *send_msg);

int server_build_fdsets(int listenfd, fd_set *readfds, fd_set *writefds, fd_set *exceptfds);
int server_select(int max_fd, int listenfd, fd_set *readfds, fd_set *writefds);
void server_delete_client(int socket_fd_del);
void server_add_new_client(struct sockaddr_in client_info, int new_socket_fd,char*buffer, char* ip, int port);
int process_recv_data(int socket,char*buffer);
int find_the_client_index_list(int socket);
int find_the_client_index_by_name(char*name);
void cleanup(void);




void updateRanking( char* user, int result){
  readFileRanking("data/ranking.txt");
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

char* initSudoku(){
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
  r = rand() % (total_data-1);
  puzzle = createPuzzle(sudoku[r].data);
  userPuzzle = copyPuzzle(puzzle);
  tempPuzzle = copyPuzzle(puzzle);
  printPuzzle(userPuzzle);
  result = 3;
  count = 1;
  strcpy(game,sudoku[r].data);
  return puzzle;

}

/*
Xử lí dữ liệu gửi từ client
*/



int main() {
    int new_socket = 0;
    fd_set readfds;
    fd_set writefds;
    fd_set exceptfds;
    int max_fd= 0;

    memset(&server,0,sizeof(struct server_data));
    printf("Server Started !!!\n");

    if(server_create_socket(&listen_fd) != 0) {
        perror("ERROR : creation socket failed");
        exit(0);
    }
    max_fd = listen_fd;

    while(1) {
        max_fd = server_build_fdsets(listen_fd, &readfds, &writefds, &exceptfds);
        server_select(max_fd,listen_fd, &readfds, &writefds);
    }

    cleanup();
    printf("Bye From server\n");

    return 0;
}


//create the server socket
int server_create_socket(int *listen_fd) {
    struct sockaddr_in server_addr;

    if((*listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("ERROR : socket creation failed");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr("172.16.0.20");

    if( 0!=bind(*listen_fd,(struct sockaddr*)&server_addr,sizeof(struct sockaddr))){
         perror("ERROR : socket bind failed");
         return -1;
    }

    if(0!=listen(*listen_fd, LISTEN_BACKLOG)) {
         perror("ERROR : socket listen failed");
         return -1;
    }
    return 0;

}

//Receiving socket data from clients
int server_recv_from_client(int client_socket, char *recv_msg) {
    int read_bytes = 0;
    memset(recv_msg,0,strlen(recv_msg));

    if((read_bytes = recv(client_socket, recv_msg, MAX_BUFFER_SIZE, 0)) > 0) {
        printf("\n[CLIENT : %d => SERVER] || Wrote [%d] number of bytes || BYTES = [%s]\n",client_socket,read_bytes,recv_msg);
        process_recv_data(client_socket, recv_msg);

    }
    else if(read_bytes == 0) {
           printf("Client Disconnected\n");
           server_delete_client(client_socket);
    }
    else {
            printf("ERROR: recv failed\n");
        }
    return 0;
}

int server_send_to_client(int client_socket, char *send_msg) {
    int write_bytes = 0;
    int len  =strlen(send_msg);
    if((write_bytes = send(client_socket, send_msg, len, 0)) > 0) {
            printf("\n[SERVER => CLIENT : %d] || Wrote [%d] number of bytes || BYTES = [%s]\n",client_socket,write_bytes, send_msg);
    }
    else {
            perror("Error : send failed");
            return -1;
    }

    return write_bytes;
}

int server_build_fdsets(int listenfd, fd_set *readfds, fd_set *writefds, fd_set *exceptfds) {
    int max_fd = listenfd;

    FD_ZERO(readfds);
    FD_SET(listenfd, readfds);
    FD_SET(STDIN_FILENO,readfds);
    fcntl(STDIN_FILENO,F_SETFL,O_NONBLOCK);

    for(int i = 0; i<server.total_client; i++) {
        FD_SET(server.client_list[i].file_des,readfds);
        max_fd++;
    }
    return max_fd;
}

//select based processing
int server_select(int max_fd,int listen_fd, fd_set *readfds, fd_set *writefds) {


    int new_socket_fd = 0;
    char recv_msgg[MAX_BUFFER_SIZE] ;
    char send_buff[MAX_BUFFER_SIZE] ;
    char buffer[MAX_BUFFER_SIZE];

    int action = select(max_fd+1,readfds,writefds,NULL,NULL);

    memset(recv_msgg, 0 ,sizeof(recv_msgg));
    memset(send_buff, 0 ,sizeof(send_buff));


    if(action == -1 || action == 0) {
        perror("ERROR: select");
        exit(0);
    }

    //check the server listenfd
     if(FD_ISSET(listen_fd,readfds)) {

       int len = sizeof(struct sockaddr);
       bzero(&client_addr,sizeof(client_addr));
       if((new_socket_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &len)) < 0) {
           perror("ERROR :accept failed");
           return -1;
       }

       char ip[INET_ADDRSTRLEN] = {0};
       int port = ntohs(client_addr.sin_port);
       inet_ntop(AF_INET, &(client_addr.sin_addr), ip, INET_ADDRSTRLEN);
       printf("[CLIENT-INFO] : [port = %d , ip = %s]\n",port, ip);

       if(server.total_client >=NO_OF_CLIENTS) {
           perror("ERROR : no more space for client to save");
       }

       while(1){
         memset(recv_msgg, 0 ,sizeof(recv_msgg));

       //server_recv_from_client(new_socket_fd,recv_msgg);
       int read_bytes = recv(new_socket_fd, recv_msgg, MAX_BUFFER_SIZE, 0);

       printf("\n[CLIENT : %d => SERVER] || Wrote [%d] number of bytes || BYTES = [%s]\n",new_socket_fd,read_bytes,recv_msgg);
       char *user, *pass;
       str = strtok( recv_msgg, token);

       if(strcmp(str, SIGNAL_CREATEUSER) == 0){
         // Create new user
         user = strtok(NULL, token);
         pass = strtok(NULL, token);

         if(isValid(user, NULL)){
           sprintf(send_buff,"%s#%s",SIGNAL_ERROR, "Account existed");
           // send(new_socket_fd, send_buff ,strlen(send_buff), 0);
           server_send_to_client(new_socket_fd,send_buff);
         } else{
           registerUser(user, pass);
           memset(send_buff, 0 ,sizeof(send_buff));
           sprintf(send_buff,"%s", SIGNAL_OK);
           server_add_new_client(client_addr ,new_socket_fd,user,ip,port);
           // send(new_socket_fd, send_buff, strlen(send_buff), 0);
           server_send_to_client(new_socket_fd,send_buff);
           goto out;
         }

         // send( fd, send_msg, strlen(send_msg), 0);
       }
       else if( strcmp(str, SIGNAL_CHECKLOGIN) == 0){
         // Login
         user = strtok(NULL, token);
         pass = strtok(NULL, token);
         if(isValid(user, pass)) {
           memset(send_buff, 0 ,sizeof(send_buff));
           sprintf(send_buff, "%s",SIGNAL_OK);
           server_add_new_client(client_addr ,new_socket_fd,user,ip,port);
           // send(new_socket_fd, send_buff, strlen(send_buff), 0);
           server_send_to_client(new_socket_fd,send_buff);
           goto out;
         }
         else{
           sprintf( send_buff,"%s#%s", SIGNAL_ERROR, "Username or Password is incorrect");
           // send(new_socket_fd, send_buff, strlen(send_buff), 0);
           server_send_to_client(new_socket_fd,send_buff);
         }

       }
       else{
         printf("Socket deleted  = [%d]\n",new_socket_fd);
         close(new_socket_fd);
         return 0;
       }
     }

    }
out:

    //check the data from stdin and send message to all connected clients
    if(FD_ISSET(STDIN_FILENO,readfds)) {
         if(read(0,send_buff,sizeof(send_buff))>0) {
            for(int i = 0;i<server.total_client;i++)
                server_send_to_client(server.client_list[i].file_des,send_buff);
        }
    }

    for(int i = 0; i<server.total_client; i++) {
           if(FD_ISSET(server.client_list[i].file_des,readfds)) {
               server_recv_from_client(server.client_list[i].file_des, recv_msgg);
    }
    }
    return 0;
}


//Detete the client data on client exit
void server_delete_client(int socket_fd_del) {
    int i = 0;
    int index = 0;

    for(i=0;i<NO_OF_CLIENTS;i++) {
        if(server.client_list[i].file_des == socket_fd_del) {
            for(index = i; index<NO_OF_CLIENTS;index++ ) {
                server.client_list[index] = server.client_list[index+1];
            }
        }
    }

    server.total_client--;
    printf("Socket deleted  = [%d]\n",socket_fd_del);
    close(socket_fd_del);
}

//Adding a new client to the server
void server_add_new_client(struct sockaddr_in client_info, int new_socket_fd, char*buffer, char* ip,int port) {

//populate the new client data
    strncpy(server.client_list[server.total_client].cname ,buffer,strlen(buffer));
    server.client_list[server.total_client].port = port;
    strcpy(server.client_list[server.total_client].ip, ip);
    server.client_list[server.total_client].file_des=new_socket_fd;

    server.total_client++;

}

//processing the received data from clients
int process_recv_data(int socket,char*buffer) {
    char play_c[MAX_BUFFER_SIZE];
    char buffer_send[MAX_BUFFER_SIZE] = {0};
    int index_sender = 0;
    int index_receiver = 0;
    int len = 0;

    index_sender = find_the_client_index_list(socket);
    char *user, *pass, *id;
    int recieved, col, row, userVal;
    ClientInfo *info;
    memset(send_msg,0,sizeof(send_msg));

    str = strtok( buffer, token);

    if(strncmp(buffer, SIGNAL_LIST,4) ==0) {
         memset(buffer,0,sizeof(buffer));
         for(int i=0;i<server.total_client;i++) {
             strcat(buffer,server.client_list[i].cname);
             strcat(buffer,";");
         }
        server_send_to_client(socket,buffer);
        goto out;
    }
    else if(strncmp(buffer, SIGNAL_CONNECT,7) == 0) {

        sscanf(buffer,"%*[^:]:%s",play_c);
        if(find_the_client_index_by_name(play_c) == -1 || strcmp(server.client_list[index_sender].cname,play_c)==0){
          server_send_to_client(socket,SIGNAL_CAN_NOT_CONNECT);
          goto out;
        }
        strcpy(server.client_list[index_sender].playwith, play_c);

        index_receiver = find_the_client_index_by_name(server.client_list[index_sender].playwith);
        server.client_list[index_sender].playwith_fd = server.client_list[index_receiver].file_des;
        // server_send_to_client(server.client_list[index_sender].file_des,CONNECTED);
        for(int i = 0;i!= index_sender && i<server.total_client; i++) {
               if(server.client_list[i].playwith_fd == server.client_list[index_sender].file_des) {


                   initSudoku();
                   id = addInfo(inet_ntoa(client_addr.sin_addr), server.client_list[index_sender].cname); //get id game
                   printf(" game with id = %s\n", id);
                   printf(" Game Info: ");
                   printInfo( getInfo(id) );
                   sprintf( send_msg,"%s#%s#%s#%s", SIGNAL_OK, id, game,server.client_list[index_sender].cname);
                   server_send_to_client(server.client_list[index_sender].playwith_fd,send_msg);
                   server_send_to_client(socket,send_msg);
                   break;
               }
        }
        goto out;
    }

    else if(strcmp(str, SIGNAL_NEWGAME) == 0){
      // Play new game

      str = strtok(NULL, token); //get user
      id = addInfo(inet_ntoa(client_addr.sin_addr), str); //get id game
      printf(" game with id = %s\n", id);
      printf(" Game Info: ");
      printInfo( getInfo(id) );
      strcpy(getInfo(id)->table,initSudoku());
      sprintf( send_msg,"%s#%s#%s", SIGNAL_OK, id, getInfo(id)->table);
      server_send_to_client(socket,send_msg);
      goto out;
      // send( socket, send_msg, strlen(send_msg), 0);
    }

    else if(strcmp(str, SIGNAL_RANKING) == 0){
      // Handle sudoku ranking
      char * buffer = 0;
      long length;
      FILE * f = fopen ("data/ranking.txt", "rb");

      if (f)
      {
        fseek (f, 0, SEEK_END);
        length = ftell (f);
        fseek (f, 0, SEEK_SET);
        buffer = malloc (length);
        if (buffer)
        {
          fread (buffer, 1, length, f);
        }
        fclose (f);
      }

      if (buffer)
      {
        printf("%s\n",buffer);

      }

      str = strtok(NULL, token);
      id = str;
      printf(" Ranking with id = %s\n", id);
      // xu li phan gui thong tin so tran thang, thua, diem
      readFileRanking("data/ranking.txt");
      node* tmp = checkUser(id);
      char inforUser[100];
      if( tmp == NULL) strcpy(inforUser, "-1");
      else{
        printf("Username-ID: %s, Win: %d, Point: %d\n", tmp->user.username, tmp->user.numberOfWin, tmp->user.point);
        sprintf(inforUser,"%d#%d", tmp->user.numberOfWin, tmp->user.point);
      }
      // traversingList(); // duyet danh sach sudokuRanking in ra phia server
      rootRank = NULL; cur = NULL; new = NULL; tmp = NULL;
      sprintf(send_msg,"%s#%s#%s#%s", SIGNAL_OK, id, inforUser,buffer);
      server_send_to_client(socket,send_msg);

      goto out;
    }
    else if(strcmp(str, SIGNAL_INPUT) == 0){
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
        writeLog(info->logfile, col, row, userVal);
        // player win
        int i,j;
        count = 1;
        for (i = 0; i < 9; i++){
            for(j = 0; j < 9; j++){
                if (info->table[i*9+j] != 0 + '0'){
                    count++;
                }
            }
        }
        printf("%d\n",count);
        if(count <= 81){
            int check = 0;
            if(checkBox(info->table, row, col, userVal))
                info->table[row*9+col] = userVal;
            else{
                check = 1;
                result--;
            }
            for (i = 0; i < 9; i++){
                for(j = 0; j < 9; j++){
                    tempPuzzle[i*9+j] = info->table[i*9+j];
                }
            }
            if(!solvePuzzle(tempPuzzle)){
                info->table[row*9+col] = 0+'0';
                check = 1;
                result--;
            }
            if(check == 1){
              sprintf(send_msg, "%s#%s", SIGNAL_INPUT,SIGNAL_CHECK_FALSE);
              server_send_to_client(socket,send_msg);
            }else{
              if(count == 81){
                count =1;
                printf("Player win\n");
                updateRanking(user, result);
                sprintf(send_msg, "%s#%d", SIGNAL_WIN,result);
                server_send_to_client(socket,send_msg);
              }else{
                sprintf(send_msg, "%s#%s", SIGNAL_INPUT,SIGNAL_CHECK_TRUE);
                server_send_to_client(socket,send_msg);
              }
            }
        }
      }
      else{
        printf("Request a turn of game with id = %s REQUEST FAILED!\n", id);
        sprintf(send_msg, "%s#%s%s%s", SIGNAL_ERROR, "Game with id=", id, "not existed");
        server_send_to_client(socket,send_msg);
      }
      goto out;
    }
    else if(strcmp(str, SIGNAL_INPUT2) == 0){
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

        int i,j;
        count = 1;
        for (i = 0; i < 9; i++){
            for(j = 0; j < 9; j++){
                if (userPuzzle[i*9+j] != 0+'0'){
                    count++;
                }
            }
        }
        printf("%d\n",count);
        if(count <= 81){
            int check = 0;
            if(checkBox(userPuzzle, row, col, userVal))
                userPuzzle[row*9+col] = userVal;
            else{
                check = 1;
            }
            for (i = 0; i < 9; i++){
                for(j = 0; j < 9; j++){
                    tempPuzzle[i*9+j] = userPuzzle[i*9+j];
                }
            }
            if(!solvePuzzle(tempPuzzle)){
                userPuzzle[row*8+col] = 0+'0';
                check = 1;
            }
            if(check == 1){
              sprintf(send_msg, "%s#%s#%d#%d#%d", SIGNAL_INPUT2,SIGNAL_CHECK_FALSE,col,row,userVal);
              server_send_to_client(socket,send_msg);
              server_send_to_client(server.client_list[index_sender].playwith_fd,send_msg);
            }else{
              if(count == 81){
                printf("Player win\n");
                updateRanking(user, 10);
                sprintf(send_msg, "%s#%d#%s#%d#%d#%d", SIGNAL_WIN,10,user,col,row,userVal);
                server_send_to_client(socket,send_msg);
                server_send_to_client(server.client_list[index_sender].playwith_fd,send_msg);
                server.client_list[index_sender].playwith_fd = 0;
              }else{
                sprintf(send_msg, "%s#%s#%d#%d#%d", SIGNAL_INPUT2,SIGNAL_CHECK_TRUE,col,row,userVal);
                server_send_to_client(socket,send_msg);
                server_send_to_client(server.client_list[index_sender].playwith_fd,send_msg);
              }
            }
        }
      }
      else{
        printf("Request a turn of game with id = %s REQUEST FAILED!\n", id);
        sprintf(send_msg, "%s#%s%s%s", SIGNAL_ERROR, "Game with id=", id, "not existed");
        server_send_to_client(socket,send_msg);
      }
      goto out;
    }
    else if(strcmp(str, SIGNAL_VIEWLOG) == 0){
      // View log
      // get id-username

      id = strtok(NULL, token);
      info = getInfo(id);

      if(info != NULL){
        printf("Request view log of sudoku game with id = %s | OK!\n", id);
        char * buffer = 0;
        long length;
        FILE * f = fopen (info->logfile, "rb");

        if (f)
        {
          fseek (f, 0, SEEK_END);
          length = ftell (f);
          fseek (f, 0, SEEK_SET);
          buffer = malloc (length);
          if (buffer)
          {
            fread (buffer, 1, length, f);
          }
          fclose (f);
        }

        if (buffer)
        {
          printf("%s\n",buffer);

        }
        sprintf(send_msg, "%s#%s#%s", SIGNAL_LOGLINE, info->logfile, buffer);
        server_send_to_client(socket,send_msg);
      }
      else{
        printf("Request view log of sudoku game with id = %s | FAILED!!!\n", id);
        printf("Game with id = %s not existed", id);
        sprintf(send_msg, "%s#%s%s%s", SIGNAL_ERROR, "Game with id=", id, "not existed");
        server_send_to_client(socket,send_msg);
      }
      goto out;
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
      server_send_to_client(socket,send_msg);
      goto out;
    }
    else {
      server_send_to_client(socket,SIGNAL_ERROR);
    }
    if(strlen(server.client_list[index_sender].playwith) != 0){
        snprintf(buffer_send,sizeof(buffer_send),"[%s] : %s",server.client_list[index_sender].cname,buffer);
        printf("Buffer  =%s\n",buffer_send);
        server_send_to_client(server.client_list[index_sender].playwith_fd,buffer_send);
    }



out:
    return 0;
}

//find index of the client data structure from client socket
int find_the_client_index_list(int socket) {
    int index = 0;
    for(int i = 0; i<server.total_client; i++) {
           if(server.client_list[i].file_des == socket) {
               index =i;
           }
    }
    return index;
}

//find index of the client data structure from client name
int find_the_client_index_by_name(char*name) {
    int index = -1;
    for(int i = 0; i<server.total_client; i++) {
           if(strcmp(server.client_list[i].cname,name) == 0) {
               index =i;
           }
    }
    return index;
}

void cleanup() {

    close(listen_fd);
    for(int i = 0; i<server.total_client; i++) {
           close(server.client_list[i].file_des);
    }
}
