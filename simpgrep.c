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
  if(argc < 3){
    printf("Wrong number of arguments\n");
    exit(1);
  }

  if(strcmp(argv[1], "simpgrep") != 0){
    printf("Unknown command\n");
    exit(2);
  }

  FILE *fp;

  // initialsing the file pointer to read
  if((fp = fopen(argv[argc-1],"r"))==NULL){
    printf("Could not open text file\n");
    exit(3);
  }

  char* line = NULL;
  int   i = 0;
  int   l = 0;
  int   n = 0;
  int   c = 0;
  int   w = 0;
  int   r = 0;

  //pattern
  char* pattern = strdup(argv[argc-2]);

  //flags
	for (int j = 2; j < argc-2; j++) {
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

  if(w){
    pattern = (char *)realloc(pattern, sizeof(pattern)+2*sizeof(char));
    char* space = strdup(" ");
    strcat(space, pattern);
    strcpy(pattern, space);
    space = strdup(" ");
    strcat(pattern, space);
  }

  size_t len = 0;

  while (getline(&line, &len, fp) != -1) {
    if(!i){
      if(strstr(line, pattern) != NULL) {
        printf("%s", line);
      }
    }else{
      if(strcasestr(line, pattern) != NULL) {
        printf("%s", line);
      }
    }
   }

  fclose(fp);

  if(line)
    free(line);

  free(pattern);

  return 0;
}
