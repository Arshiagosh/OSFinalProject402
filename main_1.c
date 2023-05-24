#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>

#define SIZE 1024
#define PORT 8080

int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr, "Usage: %s [source_path] [destination_address] [file splitting size]\n", argv[0]);
        exit(1);
    }

    DIR* dir = opendir(argv[1]);
    if (dir)
    {
        // Directory
        printf("Source path is a directory\n");

        int pid = fork();
        if (pid == 0)
        {

            char cmd1[100];
            sprintf(cmd1,"server/server %s %d %s",argv[2],PORT,argv[3]);
            system(cmd1);
        }
        else if (pid > 0)
        {
      
            wait(NULL);
            char cmd2[100];
            sprintf(cmd2,"client/client %s %d %s -d",argv[1],PORT,argv[3]); // -d flag for directory
            system(cmd2);
        }
        else
        {
            fprintf(stderr, "Fork failed.\n");
            exit(1);
        }
    }
    else if (ENOENT == errno)
    {

        printf("Source path does not exist\n");
        exit(1);
    }
    else
    {
        // file
        printf("Source path is a file\n");

        char cmd1[100];
        sprintf(cmd1,"server/server %s %d %s",argv[2],PORT,argv[3]);
        char cmd2[100];
        sprintf(cmd2,"client/client %s %d %s",argv[1],PORT,argv[3]);

        int pid = fork();
        if (pid == 0)
        {
            wait(NULL);
            system(cmd2);
        }
        else if (pid > 0)
        {
            system(cmd1);
        }
        else
        {
            fprintf(stderr, "Fork failed.\n");
            exit(1);
        }
    }

    return 0;
}