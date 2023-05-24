#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SIZE 1024

typedef struct {
    int port;
    char *ip;
    char *fileNamePart;
} argvThread;

void send_file(FILE *fp, int sockfd)
{
    char data[SIZE] = {0};
    while(fgets(data,SIZE,fp)!=NULL)
    {
        if (send(sockfd,data,sizeof(data),0)==-1)
        {
            perror("[-]Error in sending data");
            exit(1);
        }
        bzero(data,SIZE);
    }
}

void *sendOnePart(void *args)
{
    
    argvThread *actual_args = args;
    int e;
    int sockfd;
    struct sockaddr_in server_addr;
    FILE *fp;
    char *filename = actual_args->fileNamePart;

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

    e = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (e == -1)
    {
        perror("[-]Error in connecting");
        exit(1);
    }
    printf("[+]Connected to server On Port:%d\n",actual_args->port);

    fp = fopen(filename,"r");
    if (fp==NULL)
    {
        perror("[-]Error in reading file.");
        exit(1);
    }

    send_file(fp,sockfd);
    printf("[+]File data send successfully...\n");

    close(sockfd);
    printf("[+]Disconnected from the server.\n");
    
    free(actual_args);

} 

void fileSplit(int segmentCount, char* largeFileName)
{
    char cmd[100];
    sprintf(cmd,"split -n %d %s %sx -d",segmentCount,largeFileName,largeFileName);
    system(cmd);
}

int main(int argc, char *argv[])
{
    char *ip = "127.0.0.1";
    int port = atoi(argv[2]);
    int e;

    int threadCount = atoi(argv[3]);
    char *fileName = argv[1];
    fileSplit(threadCount,fileName);
    pthread_t threadsForSending[threadCount];
    
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
        pthread_create(&threadsForSending[i],NULL,sendOnePart,args);
    }

    for (int i=0;i<threadCount;i++)
    {
        pthread_join(threadsForSending[i],NULL);
    }
    return 0;
}    
