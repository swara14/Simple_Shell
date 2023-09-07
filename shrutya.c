#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>
#define clear() printf("\033[H\033[J")
// char history[100][100];
// int count_history = 0;
// char** spilt_instruction;
// void add_history(char instruction[]){
//     strcpy( history[count_history] , instruction );
//     count_history++;
// }
// int Input(){
//     char instruction[100];
//     printf("\n>>>");
//     fgets(instruction , sizeof(instruction) , stdin);
//     if (strlen( instruction ) != 0)
//     {
//         add_history(instruction);
//         return 0;
//     }
//     else
//     {
//         return 1;
//     }   
// }
// void print_curr_dir(){

//     char cwd[1024];
//     getcwd(cwd, sizeof(cwd));
//     printf("\nDir: %s", cwd);
// }

// void help(){
//     puts("\n Commands supported""\n");
// }

// void split_string(char *str){
//     char* token;
    
//     token = strtok(str , " ");
//     strcpy(spilt_instruction)
//     while (token != NULL)
//     {
        
//     }
    
    
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

char** break_pipes(char *str){
    char **command;
    command = (char**)malloc(sizeof(char*)*4);
    int i = 0;
    char *token = strtok(str , "|");
    while (token != NULL)
    {
        command[i] = (char*)malloc(strlen(token) + 1 );
        strcpy( command[i] , token);
        token = strtok(NULL , "|"); 
        i++;
    }
    command[i] = NULL;
    return command;
}

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

int main(int argc, char const *argv[])
{
    allocate_history();
    char *str;
    char **command;
    int i = 0;
    printf("\n\nSHELL STARTED\n\n----------------------------\n\n");
    while (1)
    {   
        str = Input();
        //printf("entered command is:%s\n" , str );
        command = break_spaces( str );
        i = 0;
        // while (command[i] != NULL)
        // {
        //     printf("_%s_" , command[i]);
        //     i++;
        // }  
        execArgs( command );
    }  
    return 0;
}
