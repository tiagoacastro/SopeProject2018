#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  printf("** Running process %d (PGID %d) **\n", getpid(), getpgrp());

  if (argc == 4)
    printf("ARGS: %s | %s | %s\n", argv[1], argv[2], argv[3]);

  char sn[10];
  strcpy(sn, "ans");
  //strcat(sn, getpid());
  char* statusName = strdup(sn);
  printf("FIFO name: %s\n", statusName);

  if(mkfifo(statusName, 0660) == -1){
    printf("Error creating status FIFO\n");
    return -1;
  }

  int status = fopen(statusName, "rb");
  int requests = fopen("requests", "wb");

  int timer = argv[1];
  timer *= 60;
  char* ret;
  char temp[100];

  while(timer > 0 && ret == NULL){
    ret = fgets(temp, 100, status);
    timer--;
  }

  close(requests);
  close(status);

  return 0;
}
