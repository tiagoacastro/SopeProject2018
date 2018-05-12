#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define WIDTH_PID       5
#define WIDTH_XXNN      5
#define WIDTH_SEAT      4
#define MAX_CLI_SEATS   99

typedef struct {
  int pid;
  int seats;
  int seatList[MAX_CLI_SEATS];
} Request;

int writeToClog(char* status);
int writeToBook(int nrseat);
int writeError(char error[]);
void alarmHandler(int sig);
