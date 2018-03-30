#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include<string.h>

int main(int argc,char *argv[])
{
  FILE *fp;
  char line[100];

  // initialsing the file pointer to read
  fp = fopen(argv[2],"r");

  int i;
	for (i = 2; i < argc; i++) {
  }

  return 0;
}
