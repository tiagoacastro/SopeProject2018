#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

typedef struct {
  int clientId;
  int available;
} Seat;

int readline(int fd, char *str);
void *office(void *arg);
int isSeatFree(Seat  *seats, int seatNum);
void bookSeat(Seat *seats, int seatNum, int clientId);
void freeSeat(Seat *seats, int seatNum);
void myHandler(int mySignal);
