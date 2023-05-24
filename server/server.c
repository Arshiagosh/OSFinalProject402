#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define SIZE 1024
#define PORT 8080

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

int main(int argc, char *argv[])
{
    char *ip = "127.0.0.1";
    int port = PORT;
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
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

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
        printf("[+]Listening...\n");
    }
    else
    {
        perror("[-]Error in listening");
        exit(1);
    }
    addr_size = sizeof(new_addr);
    new_sock = accept(sockfd,(struct sockaddr*)&new_addr,&addr_size);

    write_file(new_sock, argv[1]);
    printf("[+]Data written in the next file.\n");
    
    return 0;
}
