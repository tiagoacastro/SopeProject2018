#include "server.h"

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static Seat room[MAX_ROOM_SEATS];
static int timeout = 0;
static Request* request;
static int newRequest = 0;
static unsigned int seats;
FILE * slogFile = NULL;
FILE * sbookFile = NULL;
int officeId = 0;

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
    if(i < seats)
      room[i].available = 1;
    else
      room[i].available = 0;
  }

  pthread_t offices[nOffices];
  int threadErr;
  for(unsigned int t = 0; t < nOffices; t++) {
      threadErr = pthread_create(&offices[t], NULL, officeHandler, NULL);
      if (threadErr) {
          exit(1);
      }
  }

  unsigned int ret = -1;
  Request* r = malloc(sizeof(Request));
  int lastpid;
  do {
    ret = read(fd,r,sizeof(Request));
    if (ret > 0 && r->pid != 0 && r->pid != lastpid) {
      request = r;
      lastpid = r->pid;
      /*
      printf("%d\n",request->pid);
      printf("%d\n",request->seats);
      for (size_t i = 0; i < MAX_CLI_SEATS; i++) {
        if(request->seatList[i] == 0)
          break;
        printf("%d\n",request->seatList[i]);
      }
      */
      newRequest = 1;
    }
  } while (!timeout);

  for (unsigned int i = 0; i < nOffices; i++) {
    pthread_join(offices[i], NULL);
  }

  slogFile = fopen("slog.txt", "a");
  fprintf(slogFile, "SERVER CLOSED");
  close(fd);
  remove("requests");
  pthread_mutex_destroy(&mutex);

  exit(0);
}

void *officeHandler(void *arg){
  int id = ++officeId;
  writeOffice(id, 1);
  int requestToBook = 0;
  Request* r;
  do {
    requestToBook = 0;
    if (timeout) {
      sleep(1);
      break;
    }
    pthread_mutex_lock(&mutex);
    if(newRequest){
      newRequest = 0;
      requestToBook = 1;
      r = request;
    }
    pthread_mutex_unlock(&mutex);

    if(requestToBook){
      pthread_mutex_lock(&mutex);
      char sn[12];
      sprintf(sn, "ans%d", r->pid);
      int fd = open(sn, O_WRONLY);
      printf("%d - vou tratar deste %s\n", id, sn);
      requestHandler(fd,id, r);
      close(fd);
      pthread_mutex_unlock(&mutex);
    }

  } while (1);
  writeOffice(id,0);
}

void requestHandler(int fd, int id, Request* request){
  Seat *s;
  if(request->seats > MAX_CLI_SEATS){
      write(fd,"-1",2);
      writeTicketInfo(id, 1, 0, NULL);
      printf("has error 1\n");
      return;
  }
  printf("checked error 1\n");
  int count = 0;
  for (unsigned int i = 0; i < seats; i++) {
    if(request->seatList[i] == 0){
      break;
    }
    count++;
  }
  if(count < request->seats || count > MAX_CLI_SEATS){
      write(fd,"-2",2);
      writeTicketInfo(id, 2, 0, NULL);
      printf("has error 2\n");
      return;
  }
  printf("checked error 2\n");

  for (unsigned int i = 0; i < count; i++) {
    if(request->seatList[i] < 1 || request->seatList[i] > seats){
      write(fd,"-3",2);
      writeTicketInfo(id, 3, 0, NULL);
      printf("has error 3\n");
      return;
    }
  }
  printf("checked error 3\n");

  if(request->seats == 0){
      write(fd,"-4",2);
      writeTicketInfo(id, 4, 0, NULL);
      printf("has error 4\n");
      return;
  }
  printf("checked error 4\n");

  int full=1;
  for (unsigned int i = 0; i < seats; i++) {
    if(isSeatFree(s, i)){
      full = 0;
      break;
    }
  }
  if(full){
    write(fd,"-6",2);
    writeTicketInfo(id, 6, 0, NULL);
    printf("has error 6\n");
    return;
  }
  printf("checked error 6\n");
/*
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
    writeTicketInfo(id, 5, 0, NULL);
    printf("has error 5\n");

    return;
  }
  printf("checked error 5, no errors\n");

  writeTicketInfo(id, 0, booked, bookedSeats);
  char message[250];
  char seat[5];
  sprintf(message, "%d", request->pid);
  for (unsigned int i = 0; i < count; i++) {
    sprintf(message, " %d", request->seatList[i]);
    strcat(message, seat);
  }
  printf("msg:\n%s\n", message);
  //write(fd,message,250);*/
  write(fd,"0",1);
}

int isSeatFree(Seat *seats, int seatNum){
  return (room[seatNum].available);
  printf("Checking if seat is free\n");
  DELAY();
}

void bookSeat(Seat *seats, int seatNum, int clientId){
  room[seatNum].clientPid = clientId;
  room[seatNum].available = 0;
  printf("Booking seat %d\n", seatNum);
  DELAY();
}

void freeSeat(Seat *seats, int seatNum) {
  room[seatNum].available = 1;
  room[seatNum].clientPid = -1;
  printf("Freeing seat %d\n", seatNum);
  DELAY();
}

void writeOffice(int officeNr, int state){
  slogFile = fopen("slog.txt", "a");
  //state = 1 if opened, 0 if closed
  if(state){
    fprintf(slogFile, "%.2d - OPEN\n", officeNr);
  }else fprintf(slogFile, "%.2d - CLOSED\n", officeNr);
}

void writeTicketInfo(int officeNr, int action, int booked, int bookedSeats[]) {
  slogFile = fopen("slog.txt", "a");

  fprintf(slogFile, "%.2d-%.5d-%.2d:\n", officeNr, request->pid, request->seats);

  unsigned int i;
  for( i = 0; i < request->seats; i++) {
    fprintf(slogFile, " %.4d", request->seatList[i]);
  }

  fprintf(slogFile, " -");

  unsigned int j;
  switch (action) {
    case 0 :
      for (j = 0; j < booked; j++) {
        fprintf(slogFile, " %.4d", bookedSeats[j]);
      }
    break;
    case 1:
      fprintf(slogFile, " MAX");
      break;
    case 2:
      fprintf(slogFile, " NST");
      break;
    case 3:
      fprintf(slogFile, " IID");
      break;
    case 4:
      fprintf(slogFile, " ERR");
      break;
    case 5:
      fprintf(slogFile, " NAV");
      break;
    case 6:
      fprintf(slogFile, " FUL");
      break;
    }

    fprintf(slogFile, "\n");
  }
