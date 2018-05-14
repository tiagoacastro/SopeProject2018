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

int writeToClog(char* status);
int writeToCBook(int nrseat);
int writeError(char error[]);
void alarmHandler(int sig);
static void sigint_handler(int sig);

