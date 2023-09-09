#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int executePipes(char ***commands, int inputfd) {
    int status;
    int fds[2];
    int prev_fd = inputfd; // Initialize prev_fd with the inputfd

    for (int i = 0; commands[i] != NULL; i++) {
        if (pipe(fds) != 0) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork();

        if (pid == 0) {
            // Child process
            // Redirect stdin if needed
            if (prev_fd != STDIN_FILENO) {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }

            // Redirect stdout to the write end of the pipe
            dup2(fds[1], STDOUT_FILENO);
            close(fds[0]); // Close the read end of the pipe

            execvp(commands[i][0], commands[i]);
            perror("execvp");
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        // Close the write end of the previous pipe (except for the first command)
        if (prev_fd != inputfd) {
            close(prev_fd);
        }

        prev_fd = fds[0]; // Set prev_fd to the read end of the current pipe
    }

    // Wait for the last command to finish
    waitpid(-1, &status, 0);
    
    return status;
}


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

int main(int argc, char const *argv[]) {
    char *command1[] = {"cat" , "helloworld.c",NULL};
    char *command2[] = {"grep" , "print",NULL};
    char *command3[] = {"wc" , "-l", NULL};
    char **commands[] = {command1, command2,command3 NULL};

    printf("\n\nSHELL STARTED\n\n----------------------------\n\n");
    executePipe(commands , -1);




    //int status = executePipe(commands, STDIN_FILENO);  // Use STDIN_FILENO for input

    // // Handle the exit status of the last command (command3)
    // if (WIFEXITED(status)) {
    //     int exit_status = WEXITSTATUS(status);
    //     printf("Last command exited with status: %d\n", exit_status);
    // } else {
    //     printf("Last command terminated abnormally.\n");
    // }

    return 0;
}
