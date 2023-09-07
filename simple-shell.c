#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>
#define clear() printf("\033[H\033[J")
void execArgs(char** parsed)
{
    // Forking a child
    pid_t pid = fork(); 
  
    if (pid == -1) {
        printf("\nFailed forking child..");
        return;
    } else if (pid == 0) {
        if (execvp(parsed[0], parsed) < 0) {
            printf("\nCould not execute command..\n");
        }
        exit(0);
    } else {
        // waiting for child to terminate
        wait(NULL); 
        return;
    }
}

int main(int argc, char const *argv[])
{   
    char** parsed = (char**)malloc(sizeof(char*)*100);
    
    parsed[0] = "./helloworld";
    //parsed[1] = "./Assignment-2/file.txt";
    // parsed[2] = "";
    printf("\n\nSHELL STARTED\n\n----------------------------\n\n");
    execArgs(parsed);
    return 0;
}
