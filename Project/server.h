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
#define DELAY()         usleep(50000)
#define WIDTH_PID 5
#define WIDTH_XXNN 5
#define WIDTH_SEAT 4
#define xstr(x) str(x)
#define str(x) #x
#define format(x) "%0" xstr(x) "d"
#define log_to_open format(2) "-OPEN\n"
#define log_to_close format(2) "-CLOSE\n"
#define log_booking format(2) "-" format(WIDTH_PID) "-" format(2) ":"

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
void writeTicketInfo(int officeNr, int action, int booked, int bookedSeats[], Request* r);
void writeToSBook(int nrseat);
static void sigint_handler(int sig);
void resetLogFiles();
