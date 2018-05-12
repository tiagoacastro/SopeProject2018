#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>

#define MAX_ROOM_SEATS  9999
#define MAX_CLI_SEATS   99

typedef struct {
  int clientPid;
  int available;
} Seat;

typedef struct {
  int pid;
  int seats;
  int seatList[MAX_CLI_SEATS];
} Request;

void *officeHandler(void *arg);
void alarmHandler(int sig);
void requestHandler(int fd);
