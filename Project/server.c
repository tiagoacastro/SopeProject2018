#include "server.h"

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static Seat room[MAX_ROOM_SEATS];
static int timeout = 0;
static Request* request = NULL;
static newRequest = 0;

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

  unsigned int seats = atoi(argv[1]);
  unsigned int nOffices = atoi(argv[2]);
  unsigned int timeout = atoi(argv[3]);

  if(mkfifo("requests", 0660) == -1){
    printf("Error creating requests FIFO");
    return -3;
  }

  int fd;
  if((fd = open("requests", O_RDONLY | O_NONBLOCK)) == -1){
    printf("Error opening FIFO");
    return -4;
  }

  for(unsigned int i = 0; i < MAX_ROOM_SEATS; i++){
    room[i].clientPid = -1;
    room[i].available = 1;
  }

  pthread_t offices[nOffices];
  int threadErr;
  for(unsigned int t = 0; t < nOffices; t++) {
      threadErr = pthread_create(&offices[t], NULL, officeHandler, fd);
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

  close(fd);
  remove("requests");

  exit(0);
}

void *officeHandler(void *arg){
  /*
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
  */
}
