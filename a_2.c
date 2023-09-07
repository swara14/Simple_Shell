#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_COM 1000
#define MAXLIST 100

void executeCommand(int argc, char* argv[]) {
    int pid = fork();

    if (pid < 0) {
        perror("Forking child failed.");
        return;
    }

    else if (pid == 0) {
        if (execvp(argv[0], argv) < 0) {
            perror("Command execution failed.");
            exit(1); 
        }
        exit(0);
    }

    else {
        int res;
        waitpid(pid, &res, 0);

        if (WIFEXITED(res)) {
            int exit_status = WEXITSTATUS(res);
            printf("\nChild process exited with status %d\n", exit_status);
        } else {
            printf("\nChild process did not exit normally\n");
        }
        return;
    }
}

void execute_piped(int argc1, char* argv1[], int argc2, char* argv2[]){
    int pipe_fd[2];
    int p1, p2;

    if (pipe(pipe_fd) < 0) {
        perror("Pipe could not be initialized.");
        return;
    }

    p1 = fork();
    if (p1 < 0) {
        perror("Could not fork.");
        return;
    }

    if (p1 == 0) {
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);

        if (execvp(argv1[0], argv1) < 0) {
            perror("Could not execute first command.");
            exit(1);
        }
    } 
    
    else {
        p2 = fork();

        if (p2 < 0) {
            perror("Could not fork.");
            return;
        }

        if (p2 == 0) {
            close(pipe_fd[1]);
            dup2(pipe_fd[0], STDIN_FILENO);
            close(pipe_fd[0]);

            if (execvp(argv2[0], argv2) < 0) {
                perror("Could not execute second command.");
                exit(1);
            }
        } 
        else {
            close(pipe_fd[0]);
            close(pipe_fd[1]);

            waitpid(p1, NULL, 0);
            waitpid(p2, NULL, 0);
        }
    }
}
