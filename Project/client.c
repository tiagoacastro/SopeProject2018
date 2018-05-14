#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define MAX_CLI_SEATS   99
#define WIDTH_PID 5
#define WIDTH_XXNN 5
#define WIDTH_SEAT 4
#define xstr(x) str(x)
#define str(x) #x
#define format(x) "%0" xstr(x) "d"
#define log_to_open format(2) "-OPEN\n"
#define log_to_close format(2) "-CLOSE\n"
#define log_booking format(2) "-" format(WIDTH_PID) "-" format(2) ":"
#define log_client format(WIDTH_PID) " " format(2) "." format(2) " " format(WIDTH_SEAT) "\n"
#define log_error format(WIDTH_PID) " " format(3)

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

  //printf("\n%d acabou de ler com a mnsg %s\n\n", pid, msg);

  if(strlen(msg) > 0)
    writeToClog(msg);

  close(status);
  close(requests);
  free(r);
  remove(sn);

  return 0;
}

int writeToClog(char* status){
  int i;
  //fclose(fopen("clog.txt", "w"));
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
  bookFile = fopen("cbook.txt", "a");
  fprintf(bookFile, format(WIDTH_SEAT) ,nrseat);
  fprintf(bookFile, "\n");
  fflush(bookFile);
  return 0;
}

int writeError(char error[]){
  fprintf(clogFile, log_error ,getpid(),error);
    fprintf(bookFile, "\n");
  fflush(clogFile);
  return 0;
}
