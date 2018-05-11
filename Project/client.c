#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  int pid = getpid();
  printf("** Running process %d (PGID %d) **\n", pid, getpgrp());

  if (argc == 4)
    printf("ARGS: %s | %s | %s\n", argv[1], argv[2], argv[3]);
printf("15\n");
  //send request
  int requests = open("requests", O_WRONLY);
  if(requests == -1){
    printf("Error opening requests FIFO\n");
    return -3;
  }
printf("22\n");
  char message[100];
  int messagelen;
  sprintf(message,"%d %d %d", pid, argv[2], argv[3]);
  messagelen=strlen(message)+1;
  write(requests,message,messagelen);
printf("28\n");
  close(requests);

  //get status
  char sn[12];

  sprintf(sn, "ans%d", pid);
  printf("FIFO name: %s\n", sn);
  if(mkfifo(sn, 0660) == -1){
    printf("Error creating status FIFO\n");
    return -1;
  }
printf("40\n");
  int status = open(sn, O_RDONLY);
  if(status == -1){
    printf("Error opening status FIFO\n");
    return -2;
  }
printf("46\n");
  char* ret;
  char temp[100];
  time_t endwait;
  time_t start = time(NULL);
  time_t seconds = argv[1];
  endwait = start + seconds;

  while (start < endwait && ret == NULL)
  {
      ret = fgets(temp, 100, status);
      start = time(NULL);
  }
printf("59\n");
  close(status);

  return 0;
}
