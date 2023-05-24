#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>


#define SIZE 1024
#define PORT 8080

int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr, "Usage: %s [source_path] [destination_address] [file splitting size]\n", argv[0]);
        exit(1);
    }
    int port = 8080;
    char cmd1[100];
    sprintf(cmd1,"server/server %s %d",argv[2],port);
    char cmd2[100];
    sprintf(cmd2,"client/client %s %d",argv[1],port);
    
    int pid = fork();
    if (pid == 0)
    {
        wait(NULL);
        system(cmd2);
    }
    else
    {
        system(cmd1);
    }
}