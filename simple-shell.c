#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <stdbool.h>
#include <signal.h> 
#include <sys/time.h>
#include <time.h>

long get_time(){

    struct timeval time , *address_time = &time;
    gettimeofday(address_time, NULL);
    long epoch_time = time.tv_sec * 1000 ;
    return epoch_time+ time.tv_usec / 1000;
    
}

bool and_flag = false;
char history[100][100];
int pid_history[100], count_history = 0, child_pid;
long start_time_history[100], end_time_history[100] ,start_time;
bool flag_for_Input = true;

void add_to_history(char *command, int pid, long start_time_ms, long end_time_ms) {
    strcpy(history[count_history], command);
    pid_history[count_history] = pid;
    start_time_history[count_history] = start_time_ms;
    end_time_history[count_history] = end_time_ms;
    count_history++;
}

void display_history() {
    printf("\n Command History: \n");
    printf("-------------------------------\n");

    for (int i = 0; i < count_history; i++) {
        printf("Command: %s\n", history[i]);
        printf("PID: %d\n", pid_history[i]);
        printf("Start Time (ms): %ld\n", start_time_history[i]);
        printf("End Time (ms): %ld\n", end_time_history[i]);
        printf("-------------------------------\n");
    }
}

void signal_handler(int signum) {
    if (signum == SIGINT) {
        // Ctrl+C was pressed
        printf("\n---------------------------------\n");
        display_history();        
        exit(0);
    }
}

void setup_signal_handler() {
    struct sigaction sh;
    sh.sa_handler = signal_handler;
    sigaction(SIGINT, &sh, NULL);
}


void executeCommand(char** argv) {
    int pid = fork();
    child_pid = pid;

    if (pid < 0) {
        perror("Forking child failed.");
        exit(1);
    }

    else if (pid == 0) { //child process

        if (and_flag) signal(SIGHUP, SIG_IGN);

        execvp(argv[0], argv); 
        printf("Command failed.");
        exit(1);
    }

    else { 

        if (!and_flag)
        {
            int ret;
            int pid = wait(&ret);

            if (WIFEXITED(ret)) {
                if (WEXITSTATUS(ret) == -1)
                {
                    printf("Exit = -1\n");
                }
            } else {
                printf("\nChild process did not exit normally with pid :%d\n" , pid);
            }
        }
        return;
    }
}

void executePipe(char ***commands, int inputfd) {// inputfd is -1 if passing for first time
    if (commands[1] == NULL) {
        int pid;
        pid = fork();
        child_pid = pid;
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
    char* sep = "|";
    commands = (char**)malloc(sizeof(char*) * 100); 
    
    if (commands == NULL) {
        printf("Memory allocation failed\n");
        exit(1); 
    }
    int i = 0;

    char *token = strtok(str, sep);

    while (token != NULL) {
        commands[i] = (char*)malloc(strlen(token) + 1);
        if (commands[i] == NULL) {
            printf("Memory allocation failed\n");
            exit(1); 
        }
        strcpy(commands[i], token);
        token = strtok(NULL, sep);
        i++;
    }

    commands[i] = NULL;  // Null-terminate the command array
    return commands;
}

char** break_spaces(char *str) {  
    char **command;
    char *sep = " \n";
    command = (char**)malloc(sizeof(char*) * 100);

    if (command == NULL) {
        printf("Memory allocation failed\n");
        exit(1); 
    }

    int i = 0;
    char *token = strtok(str,sep ); // Include '\n' to remove the newline character from the token
    while (token != NULL) {
        command[i] = (char*)malloc(strlen(token) + 1);
        if (command[i] == NULL) {
            printf("Memory allocation failed\n");
            exit(1); 
        }

        strcpy(command[i], token);
        token = strtok(NULL, sep);
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
    if (command == NULL) {
        printf("Memory allocation failed\n");
        exit(1); 
    }

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
    if (input_str == NULL) {
        printf("Memory allocation failed\n");
        exit(1); 
    }
    flag_for_Input = false;
    fgets(input_str ,100, stdin);// possible error
    if (strlen(input_str) != 0 && input_str[0] != '\n' && input_str[0] != ' ')
    {   
        flag_for_Input = true;
    }
    return input_str;
}

bool check_and(char* str){

    if (str[strlen(str) - 2] == '&')
    {
        str[strlen(str) - 2] = '\0';
        return true;
    }
    return false;
    
}

void executeScript(char *filename) {
    //printf("___%s___\n" , filename);
    FILE *scriptFile = fopen(filename, "r");
    if (scriptFile == NULL) {
        perror("Error opening script file");
        return;
    }

    char line[100];
    while (fgets(line, sizeof(line), scriptFile) != NULL) {
        // Remove newline character if it exists
        int len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        // Check if the line contains pipes
        if (check_for_pipes(line)) {
            char **command_1 = break_pipes_1(line);
            char ***command_2 = break_pipes_2(command_1);
            executePipe(command_2, -1);
        } else {
            char **command_1 = break_spaces(line);
            executeCommand(command_1);
        }
    }

    fclose(scriptFile);
}

// int main(int argc, char const *argv[]) {
//     setup_signal_handler(); // Set up the Ctrl+C handler

//     char *str, *str_for_history = (char *)malloc(100);
//     if (str_for_history == NULL)
//     {
//         printf("Error allocating memory\n");
//         exit(1);
//     }
    
//     char **command_1;
//     char ***command_2;
//     char c[100]; // to print the current directory
//     printf("\n\nSHELL STARTED\n\n----------------------------\n\n");

//     while (1) {
//         getcwd(c, sizeof(c));
//         printf("Shell> %s>>> ", c);
//         str = Input(); // Get user input

//         if (flag_for_Input == true) {
//             strcpy(str_for_history, str);
//             start_time = get_time();

//             and_flag = check_and(str);

//             if (check_for_pipes(str)) {
//                 // If pipes are present, execute piped commands
//                 command_1 = break_pipes_1(str);
//                 command_2 = break_pipes_2(command_1);
//                 executePipe(command_2, -1);
//             } else {
//                 // If no pipes, execute a single command
//                 command_1 = break_spaces(str);
//                 executeCommand(command_1);
//             }

//             add_to_history(str_for_history, child_pid, start_time, get_time());
//         }
//     }

//     return 0;
// }

int main(int argc, char const *argv[]) {
    setup_signal_handler(); // Set up the Ctrl+C handler

    char *str, *str_for_history = (char *)malloc(100);
    if (str_for_history == NULL) {
        printf("Error allocating memory\n");
        exit(1);
    }

    char c[100]; // to print the current directory
    printf("\n\nSHELL STARTED\n\n----------------------------\n\n");

    while (1) {
        getcwd(c, sizeof(c));
        printf("Shell> %s>>> ", c);
        str = Input(); // Get user input

        if (flag_for_Input == true) {
            strcpy(str_for_history, str);
            start_time = get_time();

            and_flag = check_and(str);

            // Check if the input starts with a special character (e.g., '@') to indicate a script file
            if (str[0] == '@') {
                // Execute commands from the specified script file
                str[strlen(str) - 1] = '\0';
                executeScript(++str); // Skip the special character
            } else {
                if (check_for_pipes(str)) {
                    // If pipes are present, execute piped commands
                    char **command_1 = break_pipes_1(str);
                    char ***command_2 = break_pipes_2(command_1);
                    executePipe(command_2, -1);
                } else {
                    // If no pipes, execute a single command
                    char **command_1 = break_spaces(str);
                    executeCommand(command_1);
                }
            }

            add_to_history(str_for_history, child_pid, start_time, get_time());
        }
    }

    return 0;
}



