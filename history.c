#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <stdbool.h>
#include <signal.h> 
#include <time.h> 

char history[100][100];
int pid_history[100] , count_history = 0 ;
time_t start_time_history[100] , end_time_history[100] , start_time;
bool flag_for_Input = true;

void add_to_history(char *command, int pid, time_t start_time, time_t end_time) {
    strcpy(history[count_history], command);
    pid_history[count_history] = pid;
    start_time_history[count_history] = start_time;
    end_time_history[count_history] = end_time;
    count_history++;
}

void display_history() {
    printf("\n Command History: \n");
    printf("-------------------------------\n");

    for (int i = 0; i < count_history; i++) {
        printf("Command: %s\n", history[i]);
        printf("PID: %d\n", pid_history[i]);
        printf("Start Time: %s", ctime(&start_time_history[i]));
        printf("End Time: %s", ctime(&end_time_history[i]));
        printf("-------------------------------\n");
    }
}
