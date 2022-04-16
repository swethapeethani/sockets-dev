#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#define PORT 8081

int main(int argc, char const *argv[])
{
    int sockfd;

    if((sockfd = socket(AF_INET,SOCK_STREAM,0)) <0 )
    {
        perror("socket");
        exit(-1);
    }

    int optval =1;
    if(setsockopt(sockfd,SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&optval,sizeof(optval)))
    {
        perror("setsockopt");
        exit(-1);
    }

    struct sockaddr_in saddr,caddr;
    saddr.sin_port = htons(PORT);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sockfd,(struct sockaddr*)&saddr, sizeof(saddr)) < 0 )
    {
        perror("bind");
        exit(-1);
    }

    if((listen(sockfd, 3)) <0)
    {
        perror("Listen");
        exit(-1);
    }

    int connfd;

    // caddr.sin_family = AF_INET;
    // caddr.sin_port = htons(PORT);
    // caddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    socklen_t socklen = sizeof(saddr);

    if((connfd = accept(sockfd, (struct sockaddr *)&saddr, &socklen)) < 0)
    {
        perror("Accept");
        exit(-1);
    }

    char buff[100];
    bzero(buff, sizeof(buff));
    int len = 0;

    if((len = read(connfd, buff, sizeof(buff))) <0 )
    {
        perror("Read");
        exit(-1);
    }
    printf( "len is %d\n", len);
    buff[len]= NULL;

    printf("Message received from client is  %s\n", buff);
    send(connfd, &buff, len ,0);

    printf( "Sent msg back to Client %s \n", buff);

    close(connfd);

    return 0;
}
