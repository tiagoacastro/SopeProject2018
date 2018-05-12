#include "server.h"

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static Seat room[MAX_ROOM_SEATS];
static int timeout = 0;
static Request* request;
static int newRequest = 0;
static unsigned int seats;

void alarmHandler(int sig) {
    printf("Time's up! \n");
    timeout = 1;
}

int main(int argc,char *argv[], char* env[]){
printf("16\n");
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
printf("43\n");
  int fd = open("requests", O_RDONLY | O_NONBLOCK);
  if(fd == -1){
    printf("Error opening FIFO");
    return -5;
  }

  for(unsigned int i = 0; i < MAX_ROOM_SEATS; i++){
    room[i].clientPid = -1;
    room[i].available = 1;
  }
printf("54\n");
  pthread_t offices[nOffices];
  int threadErr;
  for(unsigned int t = 0; t < nOffices; t++) {
      threadErr = pthread_create(&offices[t], NULL, officeHandler, NULL);
      if (threadErr) {
          exit(1);
      }
  }
printf("63\n");
  unsigned int ret = -1;
  Request* r = malloc(sizeof(Request));
  do {
    ret = read(fd,r,sizeof(Request));
    if (ret > 0) {
      request = r;
      printf("%d\n",69);
      printf("%d\n",request->pid);
      printf("%d\n",70);

      newRequest = 1;
    }
  } while (!timeout);
printf("72\n");
  for (unsigned int i = 0; i < nOffices; i++) {
    pthread_join(offices[i], NULL);
  }
printf("76\n");
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
printf("94\n");
      sprintf(sn, "ans%d", request->pid);
printf("96\n");
      int fd = open(sn, O_WRONLY | O_NONBLOCK);
printf("98\n");
      requestHandler(fd);
printf("100\n");
      close(fd);
    }
    pthread_mutex_unlock(&mutex);

  } while (1);
}

void requestHandler(int fd){
  Seat *s;

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
    if(isSeatFree(s, i)){
      full = 0;
      break;
    }
  }
  if(full){
    write(fd,"-6",2);
    return;
  }

  int booked = 0;
  int bookedSeats[request->seats];
  for(unsigned int i = 0; i < request->seats; i++)
    bookedSeats[i] = -1;

  for (unsigned int i = 0; i < count; i++) {
    if (isSeatFree(s, request->seatList[i]-1)) {
      bookSeat(s, request->seatList[i]-1, request->pid);
      bookedSeats[booked] = request->seatList[i]-1;
      booked++;
      if(booked == request->seats)
          break;
    }
  }

  if(booked < request->seats){
    for (unsigned int i = 0; i < request->seats; i++) {
      if(bookedSeats[i] != -1)
        freeSeat(s, bookedSeats[i]);
    }
    write(fd,"-5",2);
    return;
  }

  char message[250];
  char seat[5];
  sprintf(message, "%d", request->pid);
  for (unsigned int i = 0; i < count; i++) {
    sprintf(message, " %d", request->seatList[i]);
    strcat(message, seat);
  }
  write(fd,message,250);
}

int isSeatFree(Seat *seats, int seatNum){
  return (room[seatNum].available);
  printf("Checking if seat is free\n");
  DELAY();
}

void bookSeat(Seat *seats, int seatNum, int clientId){
  room[seatNum].clientPid = clientId;
  room[seatNum].available = 0;
  printf("Booking seat\n");
  DELAY();
}

void freeSeat(Seat *seats, int seatNum) {
  room[seatNum].available = 1;
  room[seatNum].clientPid = -1;
  printf("Freeing seat\n");
  DELAY();
}
