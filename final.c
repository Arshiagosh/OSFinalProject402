//User Manual
// This program is a file transfer tool that allows sending files or directories over a network using the client-server model. The program takes in three arguments:

// first argument is source_path - The path of the file or directory to be transferred.
// second argument is destination_address - The IP address or hostname of the destination machine. in this project, it is 127.0.0.1 but we can change it easily.
// third argument is file splitting size - The size of each chunk into which the file will be split during transmission.
// Here is how to use the program:

// Compile the program using a C compiler. For example, using GCC:

// gcc -o file_transfer_tool main.c
// Run the compiled program with the three required arguments:

// ./file_transfer_tool [source_path] [destination_address] [file splitting size]
// Example usage:

// ./file_transfer_tool /home/desktop/ 127.0.0.1 1024
// This will transfer the entire 'documents' directory located at '/home/desktop/' to the machine at IP address '127.0.0.1', splitting the files into chunks of size 1024 bytes.

// Alternatively, you can also transfer a single file by specifying its full path instead of a directory.id you want transfer directory it is implemented by fork.

// During execution, the program forks two child processes; one for the server and another for the client. The server listens on port 8080 waiting for a connection from the client. Once the connection is established, the client starts transferring the files to the server.

// If the source path provided does not exist or an invalid number of arguments are given, the program will display an error message and terminate.

// Note that both the client and server executables need to be present in the same directory as the file_transfer_tool executable for this program to work correctly.
// enjoy using this code :))

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
        fprintf(stderr, "Error: Invalid number of arguments provided.\n");
        fprintf(stderr, "Usage: %s [source_path] [destination_address] [file splitting size]\n", argv[0]);
        exit(EXIT_FAILURE);
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
            if(system(cmd1) == -1){
                fprintf(stderr, "Error: Failed to execute system command '%s'\n", cmd1);
                exit(EXIT_FAILURE);
            }
        }
        else if (pid > 0)
        {
      
            wait(NULL);
            char cmd2[100];
            sprintf(cmd2,"client/client %s %d %s -d",argv[1],PORT,argv[3]); // -d flag for directory
            if(system(cmd2) == -1){
                fprintf(stderr, "Error: Failed to execute system command '%s'\n", cmd2);
                exit(EXIT_FAILURE);
             }
        }
        else
        {
            fprintf(stderr, "Error: Fork failed.\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (ENOENT == errno)
    {

        printf("Error: Source path '%s' does not exist.\n", argv[1]);
        exit(EXIT_FAILURE);
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
            if(system(cmd2) == -1){
                fprintf(stderr, "Error: Failed to execute system command '%s'\n", cmd2);
                exit(EXIT_FAILURE);
            }
        }
        else if (pid > 0)
        {
            if(system(cmd1) == -1){
                fprintf(stderr, "Error: Failed to execute system command '%s'\n", cmd1);
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            fprintf(stderr, "Error: Fork failed.\n");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}