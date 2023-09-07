#include <stdio.h>
#include <string.h>
char history[100][100];
int count_history = 0;
char** spilt_instruction;
void add_history(char instruction[]){
    strcpy( history[count_history] , instruction );
    count_history++;
}
int Input(){
    char instruction[100];
    printf("\n>>>");
    fgets(instruction , sizeof(instruction) , stdin);
    if (strlen( instruction ) != 0)
    {
        add_history(instruction);
        return 0;
    }
    else
    {
        return 1;
    }   
}
void print_curr_dir(){

    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("\nDir: %s", cwd);
}

void help(){
    puts("\n Commands supported""\n");
}

void split_string(char *str){
    char* token;
    
    token = strtok(str , " ");
    strcpy(spilt_instruction)
    while (token != NULL)
    {
        
    }
    
    
}
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
