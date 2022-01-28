#include <stdio.h>
#include <stdlib.h>

typedef struct elementtype{
  char username[50]; // Khai bao username
  int numberOfWin;
  int point;
}userInfor;

typedef struct node{
  userInfor user;
  struct node *next;
}node;

node *rootRank, *cur,*new;

node* makeNewNode(userInfor user);

void insert(userInfor user);

void displayNode(node* p);

void traversingList();

void to_free(node* rootRank);

void readFileRanking(char* filename);

void updateFileRanking();

node* checkUser(char name[100]);

int getInforUser(char name[100]);

void sortRanking();

void displayNodeOfRanking(node* p, int top, char id[100]);

void printfRanking(char id[100]);

void displayNodeOfRankingNotID(node* p, int top);

void printfRankingNotID();
