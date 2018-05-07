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

  char sn[12];
  int pid = getpid();
  sprintf(sn, "ans%d", pid);
  printf("FIFO name: %s\n", sn);

  if(mkfifo(sn, 0660) == -1){
    printf("Error creating status FIFO\n");
    return -1;
  }

  printf("r\n");

  int status = open(sn, O_RDONLY);
  if(status == -1){
    printf("Error opening status FIFO\n");
    return -2;
  }
  int requests = open(sn, O_WRONLY);
  if(requests == -1){
    printf("Error opening requests FIFO\n");
    return -3;
  }

  char* ret;
  char temp[100];
  time_t endwait;
  time_t start = time(NULL);
  time_t seconds = argv[1];
  endwait = start + seconds;

  while (start < endwait)
  {
      ret = fgets(temp, 100, status);
      sleep(1);
      start = time(NULL);
  }

  close(requests);
  close(status);

  return 0;
}
