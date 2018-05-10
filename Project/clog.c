#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE * clogFile = NULL;
FILE * bookFile = NULL;

int main(){
  int oi[] = {-1};
  writeToClog(oi);
   int oi2[] = {1,12};
     writeToClog(oi2);
     int oi3[] = {3,12, 14, 20};
       writeToClog(oi3);
return 0;
}


int writeToClog(int values[]){
  int i;
clogFile=fopen("clog.txt", "a");

  if (values[0] > 0){
  for(i = 1; i<= values[0]; i++){
    fprintf(clogFile, "%.5d %.2d.%.2d %.4d \n",getpid(),values[0], i, values[i]);
    writeToBook(values[i]);
    fflush(clogFile);
  }
}
  else switch (values[0]) {
    case -1:
    writeError("MAX");
    break;
    case -2:
    writeError("NST");
    break;
    case -3:
    writeError("IDD");
    break;
    case -4:
    writeError("ERR");
    break;
    case -5:
    writeError("NAV");
    break;
    case -6:
    writeError("FUL");
    break;
  }
  return 0;
}

int writeToBook(int nrseat) {
  int i;
  bookFile = fopen("cbook.txt", "a");
  fprintf(bookFile, "%.4d \n",nrseat);
  fflush(bookFile);
}

int writeError(char error[]){
  fprintf(clogFile, "%.5d %.3s \n",getpid(),error);
  fflush(clogFile);
  return 0;
}
