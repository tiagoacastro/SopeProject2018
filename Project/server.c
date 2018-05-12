#include "server.h"

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static Seat room[MAX_ROOM_SEATS];
static int timeout = 0;
static Request* request = NULL;
static int newRequest = 0;
static unsigned int seats;

void alarmHandler(int sig) {
    printf("Time's up! \n");
    timeout = 1;
}

int main(int argc,char *argv[], char* env[]){
  if(argc != 4){
    printf("wrong arguments: server <num_room_seats> <num_ticket_offices> <open_time>\n");
    return -1;
  }

  if(argv[1] <= 0 || argv[2] <= 0 || argv[3] <= 0){
    printf("argument values not acceptable, must be positive integers\n");
    return -2;
  }

  if(atoi(argv[1]) > MAX_ROOM_SEATS){
    printf("Number of seats is too high");
    return -3;
  }

  seats = atoi(argv[1]);
  unsigned int nOffices = atoi(argv[2]);
  unsigned int endTime = atoi(argv[3]);

  alarm((unsigned int) endTime);
  signal(SIGALRM, alarmHandler);

  if(mkfifo("requests", 0660) == -1){
    printf("Error creating requests FIFO");
    return -4;
  }

  int fd = open("requests", O_RDONLY | O_NONBLOCK);
  if(fd == -1){
    printf("Error opening FIFO");
    return -5;
  }

  for(unsigned int i = 0; i < MAX_ROOM_SEATS; i++){
    room[i].clientPid = -1;
    room[i].available = 1;
  }

  pthread_t offices[nOffices];
  int threadErr;
  for(unsigned int t = 0; t < nOffices; t++) {
      threadErr = pthread_create(&offices[t], NULL, officeHandler, NULL);
      if (threadErr) {
          exit(1);
      }
  }

  Request* r = (Request*)malloc(sizeof(Request));
  do {
    r=read(fd,r,sizeof(Request*));
    if (r != NULL) {
      request = r;
      newRequest = 1;
    }
  } while (!timeout);

  for (unsigned int i = 0; i < nOffices; i++) {
    pthread_join(offices[i], NULL);
  }

  free(r);
  close(fd);
  remove("requests");

  exit(0);
}

void *officeHandler(void *arg){
  do {
    if (timeout) {
      sleep(1);
      break;
    }

    pthread_mutex_lock(&mutex);
    if(newRequest){
      newRequest = 0;

      char sn[12];
      sprintf(sn, "ans%d", request->pid);
      int fd = open("sn", O_WRONLY);

      requestHandler(fd);

      close(fd);
    }
    pthread_mutex_unlock(&mutex);

  } while (1);
}

void requestHandler(int fd){
  if(request->seats > MAX_CLI_SEATS){
      write(fd,"-1",2);
      return;
  }

  int count = 0;
  for (unsigned int i = 0; i < seats; i++) {
    if(request->seatList[i] != 0){
      count++;
    }
  }
  if(count < request->seats || count > MAX_CLI_SEATS){
      write(fd,"-2",2);
      return;
  }

  for (unsigned int i = 0; i < seats; i++) {
    if(request->seatList[i] < 1 || request->seatList[i] > seats){
      write(fd,"-3",2);
      return;
    }
  }

  if(request->seats == 0){
      write(fd,"-4",2);
      return;
  }

  int full=1;
  for (unsigned int i = 0; i < seats; i++) {
    if(room[i].available){
      full = 0;
      break;
    }
  }
  if(full){
    write(fd,"-6",2);
    return;
  }
}

int isSeatFree(Seat *seats, int seatNum){

  return (seats[seatNum].available);
}

void bookSeat(Seat *seats, int seatNum, int clientId){

  seats[seatNum].clientPid = clientId;
  seats[seatNum].available = 1;
}

void freeSeat(Seat *seats, int seatNum) {

  seats[seatNum].available = 0;
  seats[seatNum].clientPid = -1;
}
