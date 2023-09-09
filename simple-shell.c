#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <stdbool.h>
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
        printf("\nCtrl+C received. Displaying command history and program details before exiting.\n");
        display_history();
        
        // Print program details
        printf("Program PID: %d\n", getpid());
        time_t current_time = time(NULL);
        printf("Program Exit Time: %s", ctime(&current_time));
        
        exit(0);
    }
}

void setup_signal_handler() {
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigaction(SIGINT, &sa, NULL);
}


void executeCommand(char** argv) {
    int pid = fork();

    if (pid < 0) {
        perror("Forking child failed.");
        exit(1);
    }

    else if (pid == 0) { //child process
        execvp(argv[0], argv); // wrong condition ,exec only returns 
        perror("Command execution failed.");
        exit(1);
    }

    else { //parent process
        int ret;
        int pid = wait(&ret);

        if (WIFEXITED(ret)) {
            printf("%d Exit = %d\n",pid,WEXITSTATUS(ret));
        } else {
            printf("\nChild process did not exit normally with pid :%d\n" , pid);
        }
        return;
    }
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

char*** break_pipes_2( char **str){ // breaks the command we got from the last function on the basis of spaces
                                    //eg : {"cat helloworld.c" , "grep print" , "wc -l", NULL} is broken into 
                                    //{{"cat" , "helloworld.c",NULL},{"grep" , "print",NULL}, {"wc" , "-l", NULL} , NULL}
                                    //this is then passed to executePipe
    char ***command;
    command = (char***)malloc(sizeof(char**)*100);
    int len = 0 ,i = 0;
    while (str[len] != NULL)
    {
        len++;
    }
    for (i = 0; i < len; i++)
    {
        command[i] = break_spaces( str[i] );
    }
    command[i] = NULL;
    return command;

}

bool check_for_pipes(char* str) {
    for (int i = 0; str[i] != '\0'; i++) { // Loop until the end of the string
        if (str[i] == '|') { // Use single quotes for character literals
            return true;
        }
    }
    return false;
}
char* Input(){   // to take input from user , returns the string entered
    char *input_str = (char*)malloc(100);
    fgets(input_str ,100, stdin);// possible error
    // if (strlen(input_str) != 0)
    // {
    //     add_to_history(input_str);
    // }
    return input_str;
}
int main(int argc, char const *argv[]) {
    setup_signal_handler(); // Set up the Ctrl+C handler

    char *str;
    char **command_1;
    char ***command_2;
    char c[100] ; // to print the current directory
    printf("\n\nSHELL STARTED\n\n----------------------------\n\n");

    while (1) {
        getcwd( c , sizeof(c));
        printf("Shell>>> %s " , c);
        str = Input(); // Get user input

        if (check_for_pipes(str)) {

            // If pipes are present, execute piped commands

            command_1 = break_pipes_1(str);     
            command_2 = break_pipes_2(command_1);
            executePipe(command_2, -1);
        } else {
            // If no pipes, execute a single command
            command_1 = break_spaces(str);
            executeCommand(command_1);
        }
    }

    return 0;
}


