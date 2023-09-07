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

int main(int argc, char const *argv[])
{   
    char** parsed = (char**)malloc(sizeof(char*)*100);
    
    parsed[0] = "cat";
    parsed[1] = "helloworld.c";
    parsed[2] = "grep";
    parsed[3] = "print";
    parsed[4] = "wc";
    parsed[5] = "-l";
    parsed[6] = NULL;
    //parsed[1] = "./Assignment-2/file.txt";
    // parsed[2] = "";
    printf("\n\nSHELL STARTED\n\n----------------------------\n\n");
        //execArgsPiped(parsed , 2);
    char* commands[] = {"cat", "helloworld.c", "grep", "print", "wc", "-l", NULL};
    int numCommands = 6; // Number of commands in the array

    execArgsPiped(commands, numCommands);

    //execArgs(parsed);
    return 0;
}
