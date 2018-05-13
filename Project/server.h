#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>

#define MAX_ROOM_SEATS  9999
#define MAX_CLI_SEATS   99
#define DELAY()         sleep(1)

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
void requestHandler(int fd, int id, Request* request);
int isSeatFree(Seat *seats, int seatNum);
void bookSeat(Seat *seats, int seatNum, int clientId);
void freeSeat(Seat *seats, int seatNum);
void writeOffice(int officeNr, int state);
void writeTicketInfo(int officeNr, int action, int booked, int bookedSeats[], Request* r);
