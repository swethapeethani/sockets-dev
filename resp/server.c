#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/tcp.h>
#include"decoder.c"

#define PORT 6379

void main()
{
    

    int sd = 0;
    sd = socket(AF_INET,SOCK_STREAM,0);

    int optval = 1;
    setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,(void *)&optval, sizeof(optval));
    //setsockopt(sd, IPPROTO_TCP,TCP_NODELAY,(void *)&flags, sizeof(flags));
    
    struct sockaddr_in saddr;
    memset(&saddr,0,sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PORT);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sd, (struct sockaddr*)&saddr, sizeof(saddr)) < 0)
    {
        perror("Bind");
        exit(1);
    }

    if(listen(sd,1024) < 0 )
    {
        perror("listen");
        exit(1);
    }

    int i=0;
    while(i<3)
    {
        socklen_t len = sizeof(saddr);
        int cfd =0;
        if( (cfd = accept(sd,(struct sockaddr*)&saddr, &len)) <0 )
        {
            perror("accept");
            continue;
        }

        char buff[102];
        bzero(buff, sizeof(buff));


        struct resprequest req;
        int reqsize = read(cfd, &req, sizeof(req));

        printf("reqsize received is %d\n", reqsize);
        printf("req state is %d  usedsize %d index %d\n", req.state, req.usedsize,req.index);
        printf("req buff is %s\n",req.buff);

        char *msg = " Received the request from client";

        send(cfd, msg, strlen(msg),0);

        decoderequest(&req,1); 

        close(cfd);
        i++;
        
    }
}