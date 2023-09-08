#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int executePipe(char ***commands, int inputfd) {
    if (commands[1] == NULL) {
        // Execute the last command
        pid_t pid;
        int status;

        if ((pid = fork()) == 0) {
            // Redirect stdin if needed
            if (inputfd != STDIN_FILENO) {
                dup2(inputfd, STDIN_FILENO);
                close(inputfd);
            }

            execvp(commands[0][0], commands[0]);
            perror("execvp");
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        waitpid(pid, &status, 0);
        return status;
    } else {
        // Execute piped commands
        int fds[2];

        if (pipe(fds) != 0) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        pid_t pid;
        int status;

        if ((pid = fork()) == 0) {
            // Redirect stdin if needed
            if (inputfd != STDIN_FILENO) {
                dup2(inputfd, STDIN_FILENO);
                close(inputfd);
            }

            dup2(fds[1], STDOUT_FILENO);
            close(fds[1]);

            execvp(commands[0][0], commands[0]);
            perror("execvp");
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        close(fds[1]);
        status = executePipe(++commands, fds[0]);
        waitpid(pid, NULL, 0);
        return status;
    }
}

int main(int argc, char const *argv[]) {
    char *command1[] = {"cat", "fib.c", NULL};
    char *command2[] = {"wc", "-l", NULL};

    char **commands[] = {command1, command2,  NULL};

    printf("\n\nSHELL STARTED\n\n----------------------------\n\n");

    int status = executePipe(commands, STDIN_FILENO);  // Use STDIN_FILENO for input

    // Handle the exit status of the last command (command3)
    if (WIFEXITED(status)) {
        int exit_status = WEXITSTATUS(status);
        printf("Last command exited with status: %d\n", exit_status);
    } else {
        printf("Last command terminated abnormally.\n");
    }

    return 0;
}
