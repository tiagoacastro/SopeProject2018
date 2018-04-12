#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<string.h>

int main(int argc,char *argv[])
{
  if(!strcmp(argv[0], "simpgrep") || argc < 3){
    printf("Wrong number of arguments");
    exit(1);
  }

  FILE *fp;

  // initialsing the file pointer to read
  if((fp = fopen(argv[argc-1],"r"))==NULL){
    printf("Could not open text file");
    exit(2);
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
	for (int j = 1; j < argc-2; j++) {
      if(strcmp(argv[j], "-i")){
        i = 1;
      } else if (strcmp(argv[j], "-l")){
        l = 1;
      } else if (strcmp(argv[j], "-n")){
        n = 1;
      } else if (strcmp(argv[j], "-c")){
        c = 1;
      } else if (strcmp(argv[j], "-w")){
        w = 1;
      } else if (strcmp(argv[j], "-r")){
        r = 1;
      }
  }

  size_t len = 0;

  while (getline(&line, &len, fp) != -1) {
    if(strstr(line, pattern) != NULL) {
      printf("%s", line);
    }
   }

  fclose(fp);

  if(line)
    free(line);

  free(pattern);

  return 0;
}
