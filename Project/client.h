#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define MAX_ROOM_SEATS  9999
#define MAX_CLI_SEATS   99
#define DELAY()         sleep(0)
#define WIDTH_PID 5
#define WIDTH_XXNN 5
#define WIDTH_SEAT 4
#define xstr(x) str(x)
#define str(x) #x
#define format(x) "%0" xstr(x) "d"
#define log_client format(WIDTH_PID) " " format(2) "." format(2) " " format(WIDTH_SEAT) "\n"
#define log_error format(WIDTH_PID) " %s"

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
