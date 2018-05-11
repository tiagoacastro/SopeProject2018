#include "server.h"

int reading;
unsigned int open_time;

int timesUp = 0;

void myHandler(int mySignal) {
  timesUp = 1;
}

int main(int argc, char *argv[], char* env[]){
  reading = 0;

  printf("hello");
  if(argc != 4){
    printf("wrong arguments: server <num_room_seats> <num_ticket_offices> <timeout>\n");
    return -1;
  }

  if(argv[1] <= 0 || argv[2] <= 0 || argv[3] <= 0){
    printf("argument values not acceptable, must be positive integers\n");
    return -2;
  }

  unsigned int seats = argv[1];
  unsigned int nOffices = argv[2];
  unsigned int timeout = argv[3];

  if(mkfifo("requests", 0660) == -1){
    printf("Error creating requests FIFO");
    return -3;
  }

  int fd = open("requests", O_RDONLY);

  if(fd == -1){
    printf("Error opening FIFO");
    return -4;
  }

  Seat seatsArr[seats];

  for(unsigned int j = 0; j < seats; j++){
    seatsArr[j].available = 0;
  }

  for (unsigned int i = 0; i < nOffices; i++) {
    pthread_t office;
    pthread_create(&office, NULL, office, NULL);
  }

  signal(SIGALRM, myHandler);
  alarm(timeout);

  close(fd);

  system("find . -type p -delete");

  free(seatsArr);

  return 0;
}

void *office(void *arg){
  int fd = arg;
  int count = 0;
  char str[200];
  while(count < open_time*60){
    if(!reading){
      readline(fd,str);
    }
    count++;
  }
  return NULL;
}

int readline(int fd, char *str){
  reading = 1;
  int n;
  do{
    n = read(fd,str,1);
  }
  while (n>0 && *str++ != '\0');
  reading = 0;
  return (n>0);
}

/*
int isSeatFree(Seat *seats, int seatNum){

  return (seats[seatNum]->available);
}

void bookSeat(Seat *seats, int seatNum, int clientId){

  seats[seatNum]->clientId = clientId;
  seats[seatNum]->available = 1;
}

void freeSeat(Seat *seats, int seatNum) {

  seats[seatNum]->available = 0;
}

*/
