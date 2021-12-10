#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <assert.h>

// termios
struct termios org_opts, new_opts;
int res;

char error[100];

// Nhập từ bàn phím không yêu cầu enter để tiếp tục.
void setPrivateTerminal(){
  res = 0;
  //store old setttings
  res = tcgetattr(STDIN_FILENO, &org_opts);
  assert(res == 0);
  //set new terminal parms
  memcpy(&new_opts, &org_opts, sizeof(new_opts));
  new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE |ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
  //new_opts.c_cc[VMIN] = 1;
  tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
}

void setNormalTerminal(){
  // lấy thông tin thiết bị đầu cuối hiện tại và lưu trữ trong org_opts
  //restore old settings
  res = tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
  assert(res == 0);
}

/*
clear screen
*/
void clearScreen(){
  write(1,"\E[H\E[2J", 7);
}
