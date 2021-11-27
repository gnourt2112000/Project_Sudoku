#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ranking.h"

char inforRankingOfUser[1000];

node* makeNewNode(userInfor user){
  node *new=(node*)malloc(sizeof(node));
  new->user=user;
  new->next=NULL;
  return new;
}

void insert(userInfor user){
  node* new = makeNewNode(user);
  if( rootRank == NULL ) {
    rootRank = new;
    cur = rootRank;
  }else {
    new->next=cur->next;
    cur->next = new;
    cur = cur->next;
  }
}

void displayNode(node* p){ // hien thi 1 node
  if (p==NULL){
    printf("Loi con tro NULL\n");
    return;
  }
  userInfor tmp = p->user;
  printf("%-20s%-10d%-10d\n", tmp.username, tmp.numberOfWin, tmp.point);
}

void traversingList(){ // duyet ca list
  node* p;
  for ( p = rootRank; p!= NULL; p = p->next )
    displayNode(p);
}

void to_free(node* rootRank){ // giai phong list
  node *to_free;
  to_free=rootRank;
  while(to_free!=NULL){
    rootRank=rootRank->next;
    free(to_free);
    to_free=rootRank;
  }
}

void readFileRanking(){
  FILE *f;
  f = fopen("ranking.txt", "r");
  if(f==NULL){// check file
    printf("Cannot open file ranking.txt!!!\n");
    return;
  }
  userInfor user;
  while(!feof(f)){
    fscanf(f, "%s %d %d\n", user.username, &user.numberOfWin, &user.point);
    insert(user);
  }
  fclose(f);
}

void updateFileRanking(){
  FILE *f = fopen("ranking.txt", "w");
  node* p;
  for ( p = rootRank; p!= NULL; p = p->next ){
      fprintf(f, "%s %d %d\n", p->user.username, p->user.numberOfWin, p->user.point);
  }
  fclose(f);
}

node* checkUser(char name[100]){// kiem tra user co trong list chua
  node* p;
  for ( p = rootRank; p!= NULL; p = p->next ){
    if(strcmp(p->user.username,name)==0){ // =0 la giong nhau
      // printf("%s %d\n", p->user.username, p->user.point);
      return p; // tra ve 1 la user da ton tai
    }

  }
  return NULL; // tra ve 0 la user chua ton tai
}

int getInforUser(char name[100]){
  node* infor = checkUser(name);
  if( infor != NULL){
    // to-do
    sprintf(inforRankingOfUser,"%s#%d#%d", infor->user.username, infor->user.numberOfWin, infor->user.point);
    return 1;

  }else{
    // to-do
    return -1;
  }
}

// sort_by_point
void sortRanking(){
  node *p, *q;
  userInfor tmp;
  for (p = rootRank; p->next != NULL; p = p->next)
    for (q = p->next; q != NULL; q = q->next)
      if (p->user.point < q->user.point)
      {
        tmp = p->user;
        p->user = q->user;
        q->user = tmp;
      }
}

void displayNodeOfRanking(node* p, int top, char id[100]){ // hien thi 1 node
  if (p==NULL){
    printf("Loi con tro NULL\n");
    return;
  }
  node* tmpNode = checkUser(id);
  userInfor tmp = p->user;
  if( strcmp(tmp.username, tmpNode->user.username ) == 0){
    printf("\033[0;33m%-5d%-20s%-10d%-10d\033[0;37m\n", top, tmp.username, tmp.numberOfWin, tmp.point);
  }else{
    if(tmp.point != -2147483648)
      printf("\033[0;37m%-5d%-20s%-10d%-10d\n", top, tmp.username, tmp.numberOfWin, tmp.point);
  }
}

void printfRanking(char id[100]){ // duyet ca list
  sortRanking();
  node* p;
  int top = 1;
  for ( p = rootRank; p!= NULL; p = p->next ){
    displayNodeOfRanking(p, top, id);
    top++;
  }
}

void displayNodeOfRankingNotID(node* p, int top){ // hien thi 1 node
  if (p==NULL){
    printf("Loi con tro NULL\n");
  }else{
    userInfor tmp = p->user;
    if(tmp.point != -2147483648)
      printf("\033[0;37m%-5d%-20s%-10d%-10d\n", top, tmp.username, tmp.numberOfWin, tmp.point);
  }
}

void printfRankingNotID(){ // duyet ca list
  sortRanking();
  node* p;
  int top = 1;

  for ( p = rootRank; p!= NULL; p = p->next ){
    displayNodeOfRankingNotID(p, top);
    top++;
  }

}
