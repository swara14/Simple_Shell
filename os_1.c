#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define MAXCOM 1000 // max number of letters to be supported
#define MAXLIST 100 // max number of commands to be supported

void execArgs(char** parsed)
{
    // Forking a child
    pid_t pid = fork();

    if (pid == -1) {
        printf("\nForking Child Failed..");
        return;
    } else if (pid == 0) {
        // Child process
        if (execvp(parsed[0], parsed) < 0) {
            printf("\nCould not execute command..");
        }
        exit(0);
    } else {
        // Parent process
        // waiting for child to terminate
        wait(NULL);
        return;
    }
}


void execArgsPiped(char** parsedArgs, char** parsedArgsPiped) {
    int pipefd[2];
    pid_t p1, p2;

    if (pipe(pipefd) < 0) {
        perror("Pipe could not be initialized");
        return;
    }

    p1 = fork();
    if (p1 < 0) {
        perror("Could not fork");
        return;
    }
    if (p1 == 0) {
        // Child 1: Writing to the pipe
        close(pipefd[0]);  // Close the read end
        dup2(pipefd[1], STDOUT_FILENO);  // Redirect stdout to the write end of the pipe
        close(pipefd[1]);  // Close the write end

        // Execute the first command
        if (execvp(parsedArgs[0], parsedArgs) < 0) {
            perror("Could not execute command 1");
            exit(0);
        }
    } 
    else {
        // Parent
        p2 = fork();

        if (p2 < 0) {
            perror("Could not fork");
            return;
        }

        if (p2 == 0) {
            // Child 2: Reading from the pipe
            close(pipefd[1]);  // Close the write end
            dup2(pipefd[0], STDIN_FILENO);  // Redirect stdin to the read end of the pipe
            close(pipefd[0]);  // Close the read end

            // Execute the second command
            if (execvp(parsedArgsPiped[0], parsedArgsPiped) < 0) {
                perror("Could not execute command 2");
                exit(0);
            }
        } else {
            // Parent
            close(pipefd[0]);
            close(pipefd[1]);

            // Wait for both child processes to finish
            waitpid(p1, NULL, 0);
            waitpid(p2, NULL, 0);
        }
    }
}


int main()
{
    char inputString[MAXCOM], *parsedArgs[MAXLIST];
    char* parsedArgsPiped[MAXLIST];
    int execFlag = 0;
    init_shell();

    while (1) {
        // print shell line
        printDir();
        // take input
        if (takeInput(inputString))
            continue;
        // process
        execFlag = processString(inputString,
        parsedArgs, parsedArgsPiped);
        // execflag returns zero if there is no command
        // or it is a builtin command,
        // 1 if it is a simple command
        // 2 if it is including a pipe.

        // execute
        if (execFlag == 1)
            execArgs(parsedArgs);

        if (execFlag == 2)
            execArgsPiped(parsedArgs, parsedArgsPiped);
    }
    return 0;
}
