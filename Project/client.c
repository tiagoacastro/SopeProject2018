#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "useful.h"

typedef struct {
  int pid;
  int seats;
  int seatList[MAX_CLI_SEATS];
} Request;

static FILE * clogFile = NULL;
static FILE * bookFile = NULL;
static int timeout = 0;

int writeToClog(char* status);
int writeToCBook(int nrseat);
int writeError(char error[]);
void alarmHandler(int sig);
static void sigint_handler(int sig);

void alarmHandler(int sig) {
    timeout = 1;
}

static void sigint_handler(int sig) {
  remove("requests");
  exit(0);
}

int main(int argc, char *argv[]) {
  int pid = getpid();
  printf("** Running process %d (PGID %d) **\n", pid, getpgrp());

  if (argc == 4)
    printf("ARGS: %s | %s | %s\n", argv[1], argv[2], argv[3]);

  unsigned int endTime = atoi(argv[1]);

  alarm((unsigned int) endTime);
  signal(SIGALRM, alarmHandler);

  struct sigaction sa;
  sa.sa_handler = sigint_handler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);

  if (sigaction(SIGINT, &sa, NULL) == -1) {
    printf("Unable to install handler\n");
    return -4;
  }

  int requests = open("requests", O_WRONLY);
  if(requests == -1){
    printf("Error opening requests FIFO\n");
    return -3;
  }

  char sn[12];
  sprintf(sn, "ans%d", pid);
  if(mkfifo(sn, 0660) == -1){
    printf("Error creating status FIFO\n");
    return -1;
  }

  Request* r = malloc(sizeof(Request));
  r->pid = pid;
  r->seats = atoi(argv[2]);
  for (unsigned int j = 0; j < MAX_CLI_SEATS; j++) {
    r->seatList[j] = 0;
  }
  int i = 0;
  char* token = strtok(argv[3], " ");
  while (token) {
    r->seatList[i] = atoi(token);
    i++;
    token = strtok(NULL, " ");
  }

  write(requests,r,sizeof(Request));

  int status = open(sn, O_RDONLY);
  if(status == -1){
    printf("Error opening status FIFO\n");
    return -2;
  }

  char msg[250] = {0};

  while (!timeout)
  {
      read(status, msg, sizeof(msg));
      if (strlen(msg) > 0)
      {
        break;
      }

      sleep(1);
  }
  printf("1, %d\n", pid);
  if(strlen(msg) > 0)
    writeToClog(msg);
  printf("2, %d\n", pid);
  close(status);
  printf("3, %d\n", pid);
  close(requests);
  printf("4, %d\n", pid);
  free(r);
  printf("5, %d\n", pid);
  remove(sn);
  printf("6, %d\n", pid);
  return 0;
}

int writeToClog(char* status){
  int i;
  clogFile=fopen("clog.txt", "a");
  int values[250];
  int t = 0;

  char* token = strtok(status, " ");
  while (token) {
    values[t] = atoi(token);
    t++;
    token = strtok(NULL, " ");
  }

  if (values[0] > 0){
    for(i = 1; i<= values[0]; i++){
      writeToCBook(values[i]);
      fprintf(clogFile, log_client,getpid(), i, values[0], values[i]);
      fflush(clogFile);
    }
  }
  else switch (values[0]) {
    case -1:
    writeError("MAX");
    break;
    case -2:
    writeError("NST");
    break;
    case -3:
    writeError("IDD");
    break;
    case -4:
    writeError("ERR");
    break;
    case -5:
    writeError("NAV");
    break;
    case -6:
    writeError("FUL");
    break;
  }
  return 0;
}

int writeToCBook(int nrseat) {
  printf("ola");
  bookFile = fopen("cbook.txt", "a");
  fprintf(bookFile, format(WIDTH_SEAT) ,nrseat);
  fprintf(bookFile, "\n");
  fflush(bookFile);
    printf("xau");

  return 0;
}

int writeError(char error[]){
  fprintf(clogFile, log_error , getpid(),atoi(error));
  fprintf(clogFile, "\n");
  fflush(clogFile);
  return 0;
}
