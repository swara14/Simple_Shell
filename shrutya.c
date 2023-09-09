#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>


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


char* Input(){   // to take input from user , returns the string entered
    char *input_str = (char*)malloc(100);
    fgets(input_str ,100, stdin);// possible error
    if (strlen(input_str) != 0)
    {
        add_to_history(input_str);
    }
    return input_str;
}

char** break_spaces(char *str) { // breaks the user from the user , on basis of spaces 
                                // for eg:
                                // grep printf helloworld.c is broken into
                                //{"grep", "printf" , "helloworld.c" , NULL}
                                //return type is char**
                                // passed to executeCommand if no pipes are present
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

char** break_pipes_1(char *str){ // breaks the command we got from the Input function if pipes are present 
                                //which will be checked by the check_for_pipes
                                // if input is cat helloworld.c | grep print | wc -l
                                //returned value is of type char**
                                //{"cat helloworld.c" , "grep print" , "wc -l" , NULL}
                                //this is then passed to break_pipes_2
    char **command;
    command = (char**)malloc(sizeof(char*)*100);
    int i = 0;
    char *token = strtok(str , "|\n");
    while (token != NULL)
    {
        command[i] = (char*)malloc(strlen(token) + 1 );
        strcpy( command[i] , token);
        token = strtok(NULL , "|\n"); 
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

bool check_for_pipes( char* str ){
    for (int i = 0; i < strlen(str); i++)
    {
        if (strcmp(str[i] , "|") == 0 )
        {
            return true;
        }
        
    }
    return false;
}

int executePipe(char ***commands, int inputfd) {// inputfd is -1 if passing for first time
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
    setup_signal_handler(); // Set up the Ctrl+C handler

    char *str;
    char **command_1;
    char ***command_2;

    printf("\n\nSHELL STARTED\n\n----------------------------\n\n");

    while (1) {
        printf("Shell> ");
        str = Input(); // Get user input
        command_1 = break_pipes_1(str);

        if (check_for_pipes(str)) {
            // If pipes are present, execute piped commands
            command_2 = break_pipes_2(command_1);
            executePipe(command_2, -1);
        } else {
            // If no pipes, execute a single command
            executeCommand(command_1);
        }
    }

    return 0;
}
