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
// void execArgsPiped(char** parsed, int numPipes) {
//     if (numPipes <= 0) {
//         execArgs(parsed); // Execute the last command in the pipeline
//         return;
//     }

//     // Create a pipe
//     int pipefd[2];
//     if (pipe(pipefd) < 0) {
//         printf("\nPipe could not be initialized");
//         return;
//     }

//     pid_t pid = fork();

//     if (pid == -1) {
//         printf("\nFailed forking child..");
//         return;
//     } else if (pid == 0) {
//         // Child process
//         close(pipefd[0]); // Close the read end of the pipe

//         // Redirect stdout to the write end of the pipe
//         dup2(pipefd[1], STDOUT_FILENO);
//         close(pipefd[1]); // Close the write end of the pipe

//         execArgsPiped(parsed + (numPipes * 2), numPipes - 1); // Recursive call for the next command in the pipeline
//         exit(0);
//     } else {
//         // Parent process
//         wait(NULL);

//         // Close the write end of the pipe
//         close(pipefd[1]);

//         // Redirect stdin to the read end of the pipe
//         dup2(pipefd[0], STDIN_FILENO);
//         close(pipefd[0]);

//         execArgs(parsed); // Execute the first command in the pipeline
//     }
// }

// Function to execute piped system commands with multiple pipes
void execArgsPiped(char** commands, int numCommands) {
    if (numCommands <= 0) {
        // No more commands to execute, return
        return;
    }

    int pipefd[2]; // 0 is read end, 1 is write end
    pid_t pid;

    // Create a pipe
    if (pipe(pipefd) < 0) {
        perror("Pipe could not be initialized");
        return;
    }

    pid = fork();

    if (pid == -1) {
        perror("Failed forking child");
        return;
    } else if (pid == 0) {
        // Child process

        // Close the read end of the pipe
        close(pipefd[0]);

        // Redirect stdout to the write end of the pipe
        dup2(pipefd[1], STDOUT_FILENO);

        // Close the write end of the pipe
        close(pipefd[1]);

        // Execute the current command (commands[0])
        execvp(commands[0], commands);

        // If execvp fails, display an error
        perror("Could not execute command");
        exit(EXIT_FAILURE);
    } else {
        // Parent process

        // Close the write end of the pipe
        close(pipefd[1]);

        // Redirect stdin to the read end of the pipe
        dup2(pipefd[0], STDIN_FILENO);

        // Close the read end of the pipe
        close(pipefd[0]);

        // Recursively call execArgsPiped for the remaining commands
        execArgsPiped(commands + 2, numCommands - 1);

        // Wait for the child process to finish
        wait(NULL);
    }
}

// You'll need to rearrange your command strings into this three dimensional array
// of pointers, but by doing so you allow an arbitrary number of commands, each with
// an arbitrary number of arguments.
// int executePipe(char ***commands, int inputfd)
// {
//     // commands is NULL terminated
//     if (commands[1] == NULL)
//     {
//         // If we get here there's no further commands to execute, so run the 
//         // current one, and send its result back.
//         pid_t pid;
//         int status;
//         if ((pid = fork()) == 0)
//         {
//             // Set up stdin for this process.  Leave stdout alone so output goes to the
//             // terminal.  If you want '>' / '>>' redirection to work, you'd do that here
//             if (inputfd != -1)
//             {
//                 dup2(inputfd, STDIN_FILENO);
//                 close(inputfd);
//             }
//             execvp(commands[0][0], commands[0]);
//             perror("execvp");
//             exit(EXIT_FAILURE);
//         }
//         else if (pid < 0)
//         {
//             perror("fork");
//             exit(EXIT_FAILURE);
//         }
//         waitpid(pid, &status, 0);
//         return status;
//     }
//     else
//     {
//         // Somewhat similar to the above, except we also redirect stdout for the
//         // next process in the chain
//         int fds[2];
//         if (pipe(fds) != 0)
//         {
//             perror("pipe");
//             exit(EXIT_FAILURE);
//         }
//         pid_t pid;
//         int status;
//         if ((pid = fork()) == 0)
//         {
//             // Redirect stdin if needed
//             if (inputfd != -1)
//             {
//                 dup2(inputfd, STDIN_FILENO);
//                 close(inputfd);
//             }
//             dup2(fds[1], STDOUT_FILENO);
//             close(fds[1]);
//             execvp(commands[0][0], commands[0]);
//             perror("execvp");
//             exit(EXIT_FAILURE);
//         }
//         else if (pid < 0)
//         {
//             perror("fork");
//             exit(EXIT_FAILURE);
//         }
//         // This is where we handle piped commands.  We've just executed
//         // commands[0], and we know there's another command in the chain.
//         // We have everything needed to execute that next command, so call
//         // ourselves recursively to do the heavy lifting.
//         status = executePipe(++commands, fds[0]);
//         // As written, this returns the exit status of the very last command
//         // in the chain.  If you pass &status as the second parameter here
//         // to waitpid, you'll get the exit status of the first command.
//         // It is left as an exercise to the reader to figure how to get the
//         // the complete list of exit statuses
//         waitpid(pid, NULL, 0);
//         return status;
//     }
// }
char** break_pipes_1(char *str) {
    char **commands;
    commands = (char**)malloc(sizeof(char*) * 100);  // Allocate memory for command pointers
    int i = 0;

    char *token = strtok(str, "|");
    while (token != NULL) {
        commands[i] = (char*)malloc(strlen(token) + 1);
        strcpy(commands[i], token);
        token = strtok(NULL, "|");
        i++;
    }

    commands[i] = NULL;  // Null-terminate the command array
    return commands;
}
char** break_spaces(char *str) {
    char **command;
    command = (char**)malloc(sizeof(char*) * 100);
    int i = 0;
    char *token = strtok(str, " \n"); // Include '\n' to remove the newline character from the token
    while (token != NULL) {
        command[i] = (char*)malloc(strlen(token) + 1);
        strcpy(command[i], token);
        token = strtok(NULL, " \n");
        i++;
    }
    command[i] = NULL;
    return command;
}
int main(int argc, char const *argv[]) {
    printf("\n\nSHELL STARTED\n\n----------------------------\n\n");
    char *str = "cat helloworld.c | grep print | wc -l";
    char **command = break_spaces(str);
    int i = 0;
    while (command[i] != NULL) {
        printf("_%s_\n", command[i]);
        i++;
    }

    // Free allocated memory
    i = 0;
    while (command[i] != NULL) {
        free(command[i]);
        i++;
    }
    free(command);

    return 0;
}

