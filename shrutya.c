#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
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
    char *input_str = (char*)malloc(sizeof(char)*100);
    fgets(input_str ,sizeof(input_str) *100, stdin);// possible error
    if (strlen(input_str) != 0)
    {
        add_to_history(input_str);
    }
    
}
char** break_spaces(char *str){
    char **command;
    command = (char**)malloc(sizeof(char*)*100);
    int i = 0;
    char *token = strtok(str , " ");
    while (token != NULL)
    {
        command[i] = (char*)malloc(strlen(token) + 1 );
        strcpy( command[i] , token);
        token = strtok(NULL , " "); 
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

char*** break_command(char* str){
    
}

int main(int argc, char const *argv[])
{
    allocate_history();
    char **command;
        for (int i = 0; i < 3; i++)
        {
            Input();
            command = break_command(history[i]);
            int j = 0;
            while (command[j] != NULL) 
            {
                printf("%s\n", command[j]);
                
                j++;
            }
        }
            
    return 0;
}
