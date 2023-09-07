#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>
#define clear() printf("\033[H\033[J")


int main(int argc, char const *argv[])
{   
    char** parsed = (char**)malloc(sizeof(char*)*100);
    
    parsed[0] = "uniq";
    parsed[1] = "./Assignment-2/file.txt";
    // parsed[2] = "./Assignment-2/helloworld.c";
    printf("\n\nSHELL STARTED\n\n----------------------------\n\n");
    execArgs(parsed);
    return 0;
}
