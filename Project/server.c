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

static void sigint_handler(int sig) {
  char answer;

  signal(sig, SIG_IGN);

  printf(
    "\n\nDo you want to stop all client processes? (Y to confirm, any other key to continue)\n"
  );

  scanf("%c%*[^\n]%*c", &answer);

  if (answer == 'y' || answer == 'Y') {
    remove("requests");
    exit(0);
  }

  printf("Resuming...\n");
}

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
    printf("Number of seats is too high\n");
    return -3;
  }

  seats = atoi(argv[1]);
  unsigned int nOffices = atoi(argv[2]);
  unsigned int endTime = atoi(argv[3]);

  alarm((unsigned int) endTime);
  signal(SIGALRM, alarmHandler);

  struct sigaction sa;
  sa.sa_handler = sigint_handler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);

  if (sigaction(SIGINT, &sa, NULL) == -1) {
    printf("Unable to install handler\n");
    return -6;
  }

  if(mkfifo("requests", 0660) == -1){
    printf("Error creating requests FIFO\n");
    return -4;
  }
  int fd = open("requests", O_RDONLY | O_NONBLOCK);
  if(fd == -1){
    printf("Error opening FIFO\n");
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
  Request* s = r;
  int lastpid;
  do {
    ret = read(fd,r,sizeof(Request));
    if (ret > 0 && r->pid != 0 && r->pid != lastpid) {
      request = r;
      lastpid = r->pid;
      newRequest = 1;
    }
  } while (!timeout);
  for (unsigned int i = 0; i < nOffices; i++) {
    pthread_join(offices[i], NULL);
  }



  close(fd);
  remove("requests");
  pthread_mutex_destroy(&mutex);
	/*sleep(1000);
	slogFile = fopen("slog.txt", "a");
  fprintf(slogFile, "SERVER CLOSED");
	*/
  free(s);
  exit(0);
}

void *officeHandler(void *arg){
  int id = ++officeId;
  writeOffice(id, 1);
  int requestToBook = 0;
  Request *r = malloc(sizeof (Request));
  Request *s = r;
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
      char sn[12];
      sprintf(sn, "ans%d", r->pid);
      int fd = open(sn, O_WRONLY);
      requestHandler(fd,id, r);
      close(fd);
    }

  } while (1);
  writeOffice(id,0);
  free(s);
  return NULL;
}

void requestHandler(int fd, int id, Request* r){
  Seat *s = NULL;
  if(r->seats > MAX_CLI_SEATS){
      write(fd,"-1",2);
      writeTicketInfo(id, 1, 0, NULL, r);
      return;
  }

  int count = 0;
  for (unsigned int i = 0; i < seats; i++) {
    if(r->seatList[i] == 0){
      break;
    }
    count++;
  }

  if(count < r->seats || count > MAX_CLI_SEATS){
      write(fd,"-2",2);
      writeTicketInfo(id, 2, 0, NULL, r);
      return;
  }

  for (unsigned int i = 0; i < count; i++) {
    if(r->seatList[i] < 1 || r->seatList[i] > seats){
      write(fd,"-3",2);
      writeTicketInfo(id, 3, 0, NULL, r);
      return;
    }
  }

  if(r->seats == 0){
      write(fd,"-4",2);
      writeTicketInfo(id, 4, 0, NULL, r);
      return;
  }

  int full=1;

  for (unsigned int i = 0; i < seats; i++) {
    pthread_mutex_lock(&mutex);
    if(isSeatFree(s, i)){
      full = 0;
      pthread_mutex_unlock(&mutex);
      break;
    }
    pthread_mutex_unlock(&mutex);
  }

  if(full){
    write(fd,"-6",2);
    writeTicketInfo(id, 6, 0, NULL, r);
    return;
  }

  int booked = 0;
  int bookedSeats[r->seats];
  for(unsigned int i = 0; i < r->seats; i++)
    bookedSeats[i] = -1;

  for (unsigned int i = 0; i < count; i++) {
    pthread_mutex_lock(&mutex);
    if (isSeatFree(s, r->seatList[i]-1)) {
      bookSeat(s, r->seatList[i]-1, r->pid);
      bookedSeats[booked] = r->seatList[i];
      booked++;
      if(booked == r->seats){
        pthread_mutex_unlock(&mutex);
        break;
      }
    }
    pthread_mutex_unlock(&mutex);
  }

  if(booked < r->seats){
    for (unsigned int i = 0; i < r->seats; i++) {
      if(bookedSeats[i] != -1){
        pthread_mutex_lock(&mutex);
        freeSeat(s, bookedSeats[i]-1);
        pthread_mutex_unlock(&mutex);
      }
    }
    write(fd,"-5",2);
    writeTicketInfo(id, 5, 0, NULL, r);
    return;
  }

  writeTicketInfo(id, 0, booked, bookedSeats, r);

	unsigned int z;
	for (z = 0; z < booked; z++) {
		writeToSBook(bookedSeats[z]);
	}

  char message[250];
  char seat[5];
  sprintf(message, "%d", r->seats);
  for (unsigned int i = 0; i < count; i++) {
    sprintf(seat, " %d", r->seatList[i]);
    strcat(message, seat);
  }
  write(fd,message,250);
}

int isSeatFree(Seat *seats, int seatNum){
  DELAY();
  return (room[seatNum].available);
}

void bookSeat(Seat *seats, int seatNum, int clientId){
  room[seatNum].clientPid = clientId;
  room[seatNum].available = 0;
  DELAY();
}

void freeSeat(Seat *seats, int seatNum) {
  room[seatNum].available = 1;
  room[seatNum].clientPid = -1;
  DELAY();
}

void writeOffice(int officeNr, int state){
  slogFile = fopen("slog.txt", "a");
  //state = 1 if opened, 0 if closed
  if(state){
    fprintf(slogFile, "%.2d-CLOSED\n", officeNr);
  }else {
		fprintf(slogFile, "%.2d-OPEN\n", officeNr);
	}
}

void writeTicketInfo(int officeNr, int action, int booked, int bookedSeats[], Request* r) {
  slogFile = fopen("slog.txt", "a");

  fprintf(slogFile, "%.2d-%.5d-%.2d:", officeNr, r->pid, r->seats);

  unsigned int i;
  for( i = 0; i < r->seats; i++) {
    fprintf(slogFile, " %.4d", r->seatList[i]);
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
		fflush(slogFile);
  }

	void writeToSBook(int nrseat) {
	  sbookFile = fopen("sbook.txt", "a");
		if (sbookFile == NULL) printf("banana \n");
	  fprintf(sbookFile, "%.4d \n",nrseat);
	  fflush(sbookFile);
	  return;
	}
