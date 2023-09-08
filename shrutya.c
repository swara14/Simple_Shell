#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>
    
// }
// int main(int argc, char const *argv[])
// {   
//     int i = 0;
//     while (i < 5)
//     {
//         Input();
//         i++;
//     }
//     for (int i = 0; i < count_history; i++)
//     {
//         puts(history[i]);
//     }
//     return 0;
// }
char **history;
int count_history = 0;
void allocate_history(){
    history = (char**)malloc(sizeof(char*) * 100 );//perform check
    for (int i = 0; i < 100; i++)
    {
        history[i] = (char*)malloc(sizeof(char)*100);
    }
    
}

void add_to_history(char *str){
    strcpy( history[count_history] , str );
    count_history++;
}

char* Input(){
    char *input_str = (char*)malloc(100);
    fgets(input_str ,100, stdin);// possible error
    if (strlen(input_str) != 0)
    {
        add_to_history(input_str);
    }
    return input_str;
}
// char** break_spaces(char *str){
//     char **command;
//     command = (char**)malloc(sizeof(char*)*100);
//     int i = 0;
//     char *token = strtok(str , " ");
//     while (token != NULL)
//     {
//         command[i] = (char*)malloc(strlen(token) + 1 );
//         strcpy( command[i] , token);
//         token = strtok(NULL , " "); 
//         i++;
//     }
//     command[i] = NULL;
//     return command;
// }
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

char** break_pipes_1(char *str){
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

char*** break_pipes_2( char **str){ // segmentation here
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

// void execArgs(char** parsed)
// {
//     // Forking a child
//     pid_t pid = fork(); 
  
//     if (pid == -1) {
//         printf("\nFailed forking child..");
//         return;
//     } else if (pid == 0) {
//         if (execvp(parsed[0], parsed) < 0) {
//             printf("\nCould not execute command..\n");
//         }
//         exit(0);
//     } else {
//         // waiting for child to terminate
//         wait(NULL); 
//         return;
//     }
// }
void executeCommand(char** argv) {
    int pid = fork();

    if (pid < 0) {
        perror("Forking child failed.");
        exit(0);
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

int main(int argc, char const *argv[])
{
    allocate_history();
    char *str;
    char **command_1;
    char ***command_2;
    int i = 0 ;
    printf("\n\nSHELL STARTED\n\n----------------------------\n\n");
    executeCommand( break_pipes_1(Input()) );
    // command_1 = break_pipes_1(str);
    // command_2 = break_pipes_2(command_1);
    // executePipe(command_2 , -1 );

    // while (1)
    // {   
    //     str = Input();
    //     //printf("entered command is:%s\n" , str );
    //     command = break_pipes_1( str );
    //     i = 0;
    //     // while (command[i] != NULL)
    //     // {
    //     //     printf("_%s_" , command[i]);
    //     //     i++;
    //     // }  
    //     executeCommand( command );
    // }  
    return 0;
}
