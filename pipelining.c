#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

void executePipe(char ***commands, int inputfd) {// inputfd is -1 if passing for first time
    if (commands[1] == NULL) {
        int pid;
        pid = fork();
        if (pid < 0)
        {
            perror("Forking child failed.");
            exit(1);
        }
        
        else if (pid == 0) {
            if (inputfd != STDIN_FILENO) {
                dup2(inputfd, STDIN_FILENO);
                close(inputfd);
            }

            execvp(commands[0][0], commands[0]);
            exit(0);
        } 

        wait(NULL);
        return;
    }
    int fd[2] ,pid;
    pipe(fd);
    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) {
        if (inputfd != STDIN_FILENO) {
            dup2(inputfd, STDIN_FILENO);
            close(inputfd);
        }
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);

        execvp(commands[0][0], commands[0]);
        exit(0);
    }
    close(fd[1]);
    executePipe(++commands, fd[0]);
    wait(NULL);
    return;
    
}
