#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include"resp.h"

#define PORT 6379

void main()
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
    //socklen_t len = sizeof(saddr);
    if(connect(sockfd,(struct sockaddr*)&saddr, sizeof(saddr)) <0 )
    {
        perror("Connect");
        exit(-1);
    }
    printf("connected to server : Send msg to server \n");

    struct resprequest req;
    bzero(&req, sizeof(req));
    req.state = 1;
    //char *tempbuf = "*2\r\n$5\r\nHELLO\r\n$5\r\nWORLD\r\n";
    char *tempbuf = "*2\r\n*3\r\n:1\r\n:2\r\n$3\r\nOMG\r\n*2\r\n+Hello\r\n-World\r\n";
    //char *tempbuf = "HellothisisRESPclient";
    // printf("addr : tempbuf %ld  req.buff %ld\n", tempbuf, req.buff);
    //printf(" strlen of tempbuff is %d\n",strlen(tempbuf));
    printf(" tempbuf is %s\n", tempbuf);
    memmove(req.buff, tempbuf,strlen(tempbuf));
    //strncpy(req.buff,tempbuf,strlen(tempbuf));
    req.buffsize = strlen(req.buff);
    printf(" buff size is %d\n", req.buffsize);
    req.buff[req.buffsize] = '\0';
    req.usedsize = req.buffsize;
    req.index =0;

    printf("string to be sent is %s \n",req.buff);
    printf(" state is %d \t usedsize %d \t index %d and req size is %d \n",req.state,req.usedsize,req.index,sizeof(req));
    printf("\n\n");
    send(sockfd,&req, sizeof(req),0);

    char buff[100];
    
    int length = read(sockfd,buff,sizeof(buff));
    buff[length] = '\0';

    printf("Msg received is  :  %s\n", buff);

    close(sockfd);

}
