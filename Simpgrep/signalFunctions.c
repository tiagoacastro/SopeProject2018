#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include "signalFunctions.h"

int termination_flag = 0;

void toggle_termination_flag(){
        termination_flag = (termination_flag + 1) % 2;
}

int flag_is_set(){
        return termination_flag == 1 ? OK : ERROR;
}

int confirm_termination(){
        printf("%d : Are you sure you want to terminate (Y/N)? ", getpid());
        char answer;
        scanf("%c", &answer);
        char c; while((c = getchar()) != '\n' && c != EOF) ; //Clear stdin

        if(answer == 'Y' || answer == 'y'){
                fileHandler("Exited program with CTRL^C");


            
                kill(0,SIGUSR1);
              }
        else{
                toggle_termination_flag();
                return ERROR;
        }
}

void exitHandler(int signo) {
  exit(0);
}
void sigint_handler(int signo){
        toggle_termination_flag();
        if(confirm_termination() == 0) {
                char groupid[256];
                sprintf(groupid, "-%d", getpgrp());
                execlp("kill", "kill", "-9", groupid, NULL); //Kill with SIGTERM signal
        }
}

int install_sigint_handler(){
        struct sigaction temp;
        temp.sa_handler = sigint_handler;
        temp.sa_flags = 0;
        sigfillset(&temp.sa_mask);
        return sigaction(SIGINT, &temp, NULL)==0 ? OK : ERROR;
}
