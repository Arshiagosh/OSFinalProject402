#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/wait.h>

#define SIZE 1024

pthread_mutex_t lock;
pthread_mutex_t lockin;

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
    printf("sockfd = %d\ndestination = %s\n",sockfd,destinationPath);
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
    //pthread_mutex_lock(&lock);
    argvThread *actual_args = (argvThread *)args;
    int e;
    int sockfd,new_sock;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    char buffer[SIZE];
    printf("file address: %s\n",actual_args->fileNamePart);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0 )
    {
        perror("S:[-]Error in socket");
        exit(1);
    }
    printf("S:[+]Server socket created.\n");
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = actual_args->port;
    server_addr.sin_addr.s_addr = inet_addr(actual_args->ip);

    e = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (e < 0)
    {
        perror("S:[-]Error in binding");
        exit(1);
    }
    printf("S:[+]Binding successful.\n");

    e = listen(sockfd,1000);
    if (e==0)
    {
        printf("S:[+]Listening on Port:%d\n",actual_args->port);
    }
    else
    {
        perror("S:[-]Error in listening");
        exit(1);
    }

    
    addr_size = sizeof(new_addr);
    new_sock = accept(sockfd,(struct sockaddr*)&new_addr,&addr_size);
    //pthread_mutex_unlock(&lock);
    //pthread_mutex_lock(&lockin);
    write_file(new_sock, actual_args->fileNamePart);
    printf("S:[+]Data written in the next file.... Port:%d\n",actual_args->port);
    //pthread_mutex_unlock(&lockin);
    

    //free(actual_args);

}

int main(int argc, char *argv[])
{
    
    char *ip = "127.0.0.1";
    int port = atoi(argv[2]);
    int e;
    

    int threadCount = atoi(argv[3]);
    char *fileName = argv[1];
    
    pthread_t threadsForReceiving[threadCount];
    argvThread args[threadCount];
    for (int i=0;i<threadCount;i++)
    {
        char fileNamePart[100];
        if (i<10)
        {
            sprintf(fileNamePart,"%sx0%d",fileName,i);
        }
        else
        {
            sprintf(fileNamePart,"%sx%d",fileName,i);
        }
        args[i].fileNamePart=fileNamePart;
        args[i].ip = ip;
        args[i].port = port+i;
        //printf("%s\n",args[i].fileNamePart);
        //{port+i,ip,fileNamePart};
        pthread_create(&threadsForReceiving[i],NULL,recieveOnePart,(void *)&args[i]);
    }
    
    for (int i=0;i<threadCount;i++)
    {
        pthread_join(threadsForReceiving[i],NULL);
    }
}
