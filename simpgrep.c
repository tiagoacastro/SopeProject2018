#define _GNU_SOURCE
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<string.h>
#include <signal.h>
#include "signalFunctions.h"

FILE *fp;

int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

void initializeLog() {

// initialsing the file pointer to read
  if((fp = fopen(getenv("LOGFILENAME"),"a"))==NULL){
    printf("Could not open text file\n");
    exit(2);
  }
}

void fileHandler(char * s) {
  if (fp != NULL){
    fprintf(fp, "%d - %s\n", getpid(), s);
  }
}

void checkFile(int argc,char *argv[], char* env[], char* pattern, int i, int l, int n, int c, int w, int r){
  FILE *fp;

  // initialsing the file pointer to read
  if((fp = fopen(argv[argc-1],"r"))==NULL){
    printf("Could not open text file\n");
    exit(2);
  }

  size_t len = 0;
  char* line = NULL;
  char* lineCopy = NULL;
  char* pch = NULL;
  int lines = 0;
  int linesFound = 0;

  while (getline(&line, &len, fp) != -1) {
    lines++;
    if(!w){
      if(!i){
        if(strstr(line, pattern) != NULL) {
          if(n)
            printf("%d:", lines);
          printf("%s", line);
          linesFound++;
        }
      }else{
        if(strcasestr(line, pattern) != NULL) {
          if(n)
            printf("%d:", lines);
          printf("%s", line);
          linesFound++;
        }
      }
    } else {
      lineCopy = strdup(line);
      pch = strtok (lineCopy," ,.-");
      while (pch != NULL)
      {
        if(!i){
          if(strcmp(pch, pattern) == 0) {
            if(n)
              printf("%d:", lines);
            printf("%s", line);
            linesFound++;
            break;
          }
        }else{
          if(strcasecmp(pch, pattern) == 0) {
            if(n)
              printf("%d:", lines);
            printf("%s", line);
            linesFound++;
            break;
          }
        }
        pch = strtok (NULL, " ,.-");
      }
    }
  }

  if(c)
    printf("string found in %d lines\n", linesFound);

    fclose(fp);
}

int main(int argc,char *argv[], char* env[])
{
  if(argc < 3){
    printf("Wrong number of arguments\n");
    exit(1);
  }

  initializeLog();

  fileHandler("Started program");

  signal(SIGUSR1, exitHandler);
  signal(SIGINT,confirm_termination);
  sleep(40);

  int   i = 0;
  int   l = 0;
  int   n = 0;
  int   c = 0;
  int   w = 0;
  int   r = 0;

  //pattern
  char* pattern = strdup(argv[argc-2]);

  //flags
	for (int j = 1; j < argc-2; j++) {
      if(strcmp(argv[j], "-i") == 0){
        i = 1;
      } else if (strcmp(argv[j], "-l") == 0){
        l = 1;
      } else if (strcmp(argv[j], "-n") == 0){
        n = 1;
      } else if (strcmp(argv[j], "-c") == 0){
        c = 1;
      } else if (strcmp(argv[j], "-w") == 0){
        w = 1;
      } else if (strcmp(argv[j], "-r") == 0){
        r = 1;
      }
  }

  if(is_regular_file(argv[argc-1])==1){
    checkFile(argc, argv, env, pattern, i, l, n, c, w, r);
  }

  free(pattern);

  fileHandler("Exited program");

  return 0;
}
