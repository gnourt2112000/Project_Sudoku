#include <stdlib.h>
#include <string.h>

typedef struct _ClientInfo{
  char* id;
  char user[100];
  char address[20];
  char* table;
  char logfile[100];
  struct _ClientInfo* next;
}ClientInfo;

ClientInfo* root;

void initList();

ClientInfo* newInfo(char* id, char user[], char addr[], char logfile[]);

ClientInfo* getInfo(char* id);

void freeInfo(ClientInfo* i);

char* addInfo(char addr[], char user[]);

int removeInfo(char* id);

void printInfo(ClientInfo* i);
