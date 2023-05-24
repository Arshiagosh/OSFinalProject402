#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SIZE 1024

typedef struct {
    int port;
    char *ip;
    char *fileNamePart;
} argvThread;

void write_file(int sockfd, char* destinationPath)
{
    int n;
    FILE *fp;
    char *filename = destinationPath;
    char buffer[SIZE];
    
    
    fp = fopen(filename,"w");
    if (fp==NULL)
    {
        perror("[-]Error in creating file.");
        exit(1);
    }

    while(1)
    {
        n = recv(sockfd,buffer,SIZE,0);
        if(n<=0)
        {
            break;
            return;
        }
        fprintf(fp,"%s",buffer);
        bzero(buffer,SIZE);
    }
    return;
}

void *recieveOnePart(void *args)
{
    
    argvThread *actual_args = args;
    int e;

    int sockfd,new_sock;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    char buffer[SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0 )
    {
        perror("[-]Error in socket");
        exit(1);
    }
    printf("[+]Server socket created.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = actual_args->port;
    server_addr.sin_addr.s_addr = inet_addr(actual_args->ip);

    e = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (e < 0)
    {
        perror("[-]Error in binding");
        exit(1);
    }
    printf("[+]Binding successful.\n");

    e = listen(sockfd,1000);
    if (e==0)
    {
        printf("[+]Listening on Port:%d\n",actual_args->port);
    }
    else
    {
        perror("[-]Error in listening");
        exit(1);
    }
    addr_size = sizeof(new_addr);
    new_sock = accept(sockfd,(struct sockaddr*)&new_addr,&addr_size);

    write_file(new_sock, actual_args->fileNamePart);
    printf("[+]Data written in the next file.... Port:%d\n",actual_args->port);
    
    free(actual_args);

}

int main(int argc, char *argv[])
{
    char *ip = "127.0.0.1";
    int port = atoi(argv[2]);
    int e;

    int threadCount = atoi(argv[3]);
    char *fileName = argv[1];

    pthread_t threadsForReceiving[threadCount];
    
    for (int i=0;i<threadCount;i++)
    {
        char *fileNamePart;
        if (i<10)
        {
            sprintf(fileNamePart,"%sx0%d",fileName,i);
        }
        else
        {
            sprintf(fileNamePart,"%sx%d",fileName,i);
        }
        argvThread *args = malloc(sizeof *args);
        args->port = port+i;
        args->ip = ip;
        args->fileNamePart = fileNamePart;
        //void argvThread = [sendOnePart,port+i,ip,fileNamePart];
        pthread_create(&threadsForReceiving[i],NULL,recieveOnePart,args);
    }

    for (int i=0;i<threadCount;i++)
    {
        pthread_join(threadsForReceiving[i],NULL);
    }
}
