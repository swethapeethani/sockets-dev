#include<stdio.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<arpa/inet.h>
#define PORT 8081


int main(int argc, char const *argv[])
{
    int sockfd;



    if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
        perror("Socket");
        exit(-1);
    }

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    //saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    saddr.sin_port = htons(PORT);

    if(inet_pton(AF_INET,"127.0.0.1",&saddr.sin_addr) <=0)
    {
        perror("INET_PTON");
        exit(-1);
    }

    socklen_t len = sizeof(saddr);
    if(connect(sockfd,(struct sockaddr*)&saddr, len) <0 )
    {
        perror("Connect");
        exit(-1);
    }

    printf("connected to server : Send msg to server \n");

    
    char buff[100];
    //char* message = (char *)malloc(sizeof(100));
    char* message = "Hello I would like to connect to u";
    int msglen = strlen(message);
    //message[msglen] = '\0';
    
    bzero(buff,sizeof(buff));
    printf("Msg sent is  :  %s\n", message);
    send(sockfd,message, strlen(message),0);

    int length = read(sockfd,&buff,sizeof(buff));
    buff[length] = NULL;

    printf("Msg received is  :  %s\n", buff);

    close(sockfd);

    return 0;
}
