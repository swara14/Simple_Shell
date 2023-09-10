#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int child_pid;
void executePipe(char ***commands, int inputfd) {
    int prev_pipe[2];
    int curr_pipe[2];
    int pid;

    prev_pipe[0] = inputfd;

    while (*commands != NULL) {
        pipe(curr_pipe);
        pid = fork();

        if (pid < 0) {
            printf("Forking child failed.\n");
            exit(1);
        } else if (pid == 0) {
            // Child process
            if (prev_pipe[0] != STDIN_FILENO) {
                dup2(prev_pipe[0], STDIN_FILENO);
                close(prev_pipe[0]);
            }

            if (*commands[1] != NULL) {
                close(curr_pipe[0]);
                dup2(curr_pipe[1], STDOUT_FILENO);
                close(curr_pipe[1]);
            }

            execvp((*commands)[0], *commands);
            exit(1);
        } else {
            // Parent process
            close(prev_pipe[0]);
            close(curr_pipe[1]);
            wait(NULL);

            // Move to the next command
            commands++;
            prev_pipe[0] = curr_pipe[0];
        }
    }
}
int main(int argc, char const *argv[]) {
    char **command_1 = (char **)malloc(sizeof(char *) * 100);
    char **command_2 = (char **)malloc(sizeof(char *) * 100);
    char **command_3 = (char **)malloc(sizeof(char *) * 100);
    command_1[0] = "cat";
    command_1[1] = "helloworld.c";
    command_1[2] = NULL;
    command_2[0] = "grep";
    command_2[1] = "print";
    command_2[2] = NULL; // Terminate the array with NULL
    command_3[0] = "wc";
    command_3[1] = "-l";
    command_3[2] = NULL; // Terminate the array with NULL
    char ***commands = (char ***)malloc(sizeof(char **) * 100);
    commands[0] = command_1;
    commands[1] = command_2;
    commands[2] = command_3;
    commands[3] = NULL;
    executePipe(commands, -1);

    return 0;
}
