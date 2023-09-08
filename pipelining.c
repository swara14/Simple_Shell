#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int executePipe(char ***commands, int inputfd , int number_of_pipes) {
    int fd[2] , pid , ret ,status;
    pipe(fd);
    for (int i = 0; commands[i] != NULL; i++)
    {
        pid = fork();
        if( pid == 0 ){
            close(fd[0]);
            dup2(fd[1] , STDOUT_FILENO);
            execvp(commands[i][0] , commands[i]);
            perror("Command execution failed.");
            exit(1);
        }
        else
        {
            close(fd[1]);
            dup2(fd[0] , STDIN_FILENO);
            wait(NULL);
        }
        wait(NULL);
                
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
