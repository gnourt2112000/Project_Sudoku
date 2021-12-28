#include <stdio.h>
#include <string.h>
#include <assert.h>

/*
in file log
*/
void printLog(char *logFile){
  char str[100];
  FILE* f = fopen(logFile, "r");
  if( f == NULL){
    printf("Error open file!!!\n");
    return;
  }
  while(fgets(str, 100, f) != NULL){
<<<<<<< HEAD
    if(str[strlen(str) - 1] == '\n') str[strlen(str) - 1] = '\0'; 
    printf("%s\n", str);
  }
  fclose(f);  
=======
    if(str[strlen(str) - 1] == '\n') str[strlen(str) - 1] = '\0';
    printf("%s\n", str);
  }
  fclose(f);
>>>>>>> update
}

/*
Ghi vào file log
*/
<<<<<<< HEAD
void writeLog(char* logFile, int col, int row){
  FILE* f = fopen(logFile, "a");
  fprintf(f, "You: column:%d - row:%d\n", col, row);
  fclose(f);
}
=======
void writeLog(char* logFile, int col, int row, int val){
  FILE* f = fopen(logFile, "a");
  fprintf(f, "You: column:%d - row:%d - value:%d\n", col, row, val);
  fclose(f);
}
>>>>>>> update
