#include "client.h"

static FILE * clogFile = NULL;
static FILE * bookFile = NULL;
static int timeout = 0;
static Request* r;

void alarmHandler(int sig) {
    timeout = 1;
}

int main(int argc, char *argv[]) {
  int pid = getpid();
  printf("** Running process %d (PGID %d) **\n", pid, getpgrp());

  if (argc == 4)
    printf("ARGS: %s | %s | %s\n", argv[1], argv[2], argv[3]);

  unsigned int endTime = atoi(argv[1]);

  alarm((unsigned int) endTime);
  signal(SIGALRM, alarmHandler);

  //send request
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
  /*
  printf("%d\n",r->pid);
  printf("%d\n",r->seats);
  for (size_t i = 0; i < MAX_CLI_SEATS; i++) {
    if(r->seatList[i] == 0)
      break;
    printf("%d\n",r->seatList[i]);
  }
  */
  printf("%d a mandar request\n", pid);
  write(requests,r,sizeof(Request));

  //get status
  printf("%d a espera que abram\n", pid);
  int status = open(sn, O_RDONLY);
  if(status == -1){
    printf("Error opening status FIFO\n");
    return -2;
  }

  char msg[250] = {0};
  printf("%d a espera para ler\n", pid);
  while (!timeout)
  {
      read(status, msg, sizeof(msg));
      if (strlen(msg) > 0)
      {
        break;
      }

      sleep(1);
  }
  printf("%d acabou de ler com a mnsg %s\n", pid, msg);
  /*
  if(ret >= 0)
    writeToClog(msg);
    */
  close(status);
  close(requests);
  free(r);
  remove(sn);

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
      fprintf(clogFile, "%.5d %.2d.%.2d %.4d \n",getpid(),values[0], i, values[i]);
      writeToBook(values[i]);
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

int writeToBook(int nrseat) {
  bookFile = fopen("cbook.txt", "a");
  fprintf(bookFile, "%.4d \n",nrseat);
  fflush(bookFile);
}

int writeError(char error[]){
  fprintf(clogFile, "%.5d %.3s \n",getpid(),error);
  fflush(clogFile);
  return 0;
}
