#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

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

#define MAX_HISTORY 100
#define MAX_COMMAND_LENGTH 100

char history[MAX_HISTORY][MAX_COMMAND_LENGTH];
int pid_history[MAX_HISTORY];
time_t start_time_history[MAX_HISTORY];
time_t end_time_history[MAX_HISTORY];
int count_history = 0;

void add_history(const char *command, int pid, time_t start_time, time_t end_time) {
    if (count_history < MAX_HISTORY) {
        strcpy(history[count_history], command);
        pid_history[count_history] = pid;
        start_time_history[count_history] = start_time;
        end_time_history[count_history] = end_time;
        count_history++;
    } else {
        // Handle the case when the history array is full
        printf("History is full. Cannot add more commands.\n");
    }
}

void display_history() {
    printf("\nCommand History:\n");
    for (int i = 0; i < count_history; i++) {
        printf("Command: %s\n", history[i]);
        printf("PID: %d\n", pid_history[i]);
        printf("Start Time: %s", ctime(&start_time_history[i]));
        printf("End Time: %s", ctime(&end_time_history[i]));
        printf("-------------------------------\n");
    }
}

void signal_handler(int signum) {
    if (signum == SIGINT) {
        // Ctrl+C was pressed
        printf("\nCtrl+C received. Displaying command history before exiting.\n");
        display_history();
        exit(0);
    }
}

void setup_signal_handler() {
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigaction(SIGINT, &sa, NULL);
}

void execute_command(const char *command) {
    int pid = getpid();
    time_t start_time = time(NULL);
    // Simulate command execution (sleep for 2 seconds)
    sleep(2);
    time_t end_time = time(NULL);
    add_history(command, pid, start_time, end_time);

    if (strcmp(command, "history") == 0) {
        display_history();
    } else {
        // Execute the command or handle other functionality
        printf("Executing: %s\n", command);
    }
}


// The signal_handler function is defined to handle the SIGINT signal (Ctrl+C). When Ctrl+C is pressed, it displays the command history using display_history and then exits the program.

// The setup_signal_handler function sets up the signal handler to call signal_handler when Ctrl+C is received.