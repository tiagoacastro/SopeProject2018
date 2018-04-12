#define _GNU_SOURCE
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<string.h>

int main(int argc,char *argv[])
{
  if (checkArguments(argc) == 1)
    exit(1);

  if (initializeFile(argc, argv) == null)
    exit(2);

  int i = 0;
  int l = 0;
  int n = 0;
  int c = 0;
  int w = 0;
  int r = 0;

  //Duplicates the string passed as argument
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
/*
  if(line)
    free(line);

  if(lineCopy)
    free(lineCopy);

  if(pch)
    free(pch);
*/
  free(pattern);

  return 0;
}

int checkArguments(int argc){
  if(argc < 3){
    printf("Wrong number of arguments\n");
    return 1;
  }
}

FILE * initializeFile(int argc, char *argv[]){

  FILE *fp;
  // initialsing the file pointer to read
  if((fp = fopen(argv[argc-1],"r"))==NULL){
    printf("Could not open text file\n");
    return null;
  } else return fp;
}
