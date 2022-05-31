#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include <inttypes.h>

#include"client.h"
#define PORT 53
#define TYPE_A 0x0001
#define TYPE_CNAME 0x0005
#define TYPE_NAME_SERVER 0x0002
#define TYPE_MAIL_SERVER 0x000f
#define TYPE_AAAA 0x001c
#define TYPE_ANY 0x00ff

// #define QRFLAG 0x8000
// #define STDQUERY 0x87FF
// #define STDQUERY_INV 0x2000
// #define AUTHDNS 0x0400
// #define TRUNC 0x0200
// #define RD_RECURSIVEQUERY 0x0100
// #define Z_AUTHAUTHENTICATED 0x0020
// #define RCODE_NOERROR 0x0000
// #define RCODE_FORMATERROR 0x0004
// #define RCODE_SERVERFAILURE 0x0002
// #define RCODE_NAMEDOESNOTEXIST 0x0001

void main(int argc, char *argv[])
{
    struct dnsheader dnshdr;
    memset(&dnshdr,0,sizeof(struct dnsheader));
    dnshdr.id = htons(0x1234);
    /*QR 1 bit, OPCODE:4 bits, AA:1 , TC:1, RD:1,RA:1,Z:3  RCODE:4 bits */
    dnshdr.qr = 0;
    dnshdr.opcode = 0;
    dnshdr.aa = 0;
    dnshdr.tc = 0;
    dnshdr.rd = 1; //recursion desired
    dnshdr.ra = 0;
    dnshdr.z = 0;
    dnshdr.ad =1;
    dnshdr.cd = 0;
    dnshdr.rcode = 0;
    dnshdr.qdcount = htons(1);
    dnshdr.ancount = 0;
    dnshdr.nscount = 0;
    dnshdr.arcount = htons(1);

    //dnshdr.flags = htons(0x0100);

   // char host[] = "www.bing.com";//"peethani.me";
     printf(" host addr is %s\n", argv[1]);
    // char host[strlen(argv[1]) + 1];
    // memcpy(host, argv[1], strlen(argv[1]));
    // host[]

    char *host = argv[1];
    struct query qry;
    memset(&qry,0, sizeof(struct query));
    if(argc >= 3 )
    {
         if( strcmp(argv[2], "A" ) == 0)
         {
                qry.qtype = htons(TYPE_A);
        }
        else if(strcmp(argv[2], "AAAA") == 0)
        {
            qry.qtype = htons(TYPE_AAAA);
        }
        else if(strcmp(argv[2], "CNAME") == 0)
        {
            qry.qtype = htons(TYPE_CNAME);
        }
        else if(strcmp(argv[2], "NS") == 0)
        {
            qry.qtype = htons(TYPE_NAME_SERVER);
        }
        else if(strcmp(argv[2], "MX") == 0)
        {
            qry.qtype = htons(TYPE_MAIL_SERVER);
        }
        else if(strcmp(argv[2], "ANY") == 0)
        {
            qry.qtype = htons(TYPE_ANY);
        }
        else 
            qry.qtype = htons(TYPE_A);
    }  
    else 
    {
        qry.qtype = htons(TYPE_A);  
    }
    qry.qclass =htons(0x0001);

    int hostlen = strlen(argv[1]);
    printf("Host name len is %d\n",hostlen);
    
    qry.qname = calloc(hostlen + 2, sizeof(char));
    memcpy(qry.qname + 1,host, hostlen);

    unsigned char *countptr = (unsigned char *)qry.qname;
    u_int8_t count = 0;
    for(int i=0; i < hostlen; i++)
    {
        if(host[i] == '.')
        {
            *countptr = count;
            countptr = qry.qname +i + 1;
            count = 0;
        }
        else
        {
            count++;
        }
    }
    *countptr = count;

    /* to set QR flag to query: set it zero for response : set it to 1 */
    // if( dnshdr.qr ==0 )
    // {
    //     printf(" QR flag set to query\n");
    // }
    // else
    // {
    //     printf("QR flag set to Response \n");
    // }


    size_t pktlen = sizeof(struct dnsheader) + hostlen + 2 +sizeof(qry.qtype) + sizeof(qry.qclass);
    unsigned char *buff = calloc(pktlen, sizeof(unsigned char));
    unsigned char *tempbuff = (unsigned char *)buff;

    memcpy(tempbuff, &dnshdr, sizeof(dnshdr));
    tempbuff += sizeof(dnshdr);

    memcpy(tempbuff, qry.qname,hostlen +1);
    tempbuff += hostlen + 2;

    memcpy(tempbuff, &qry.qtype, sizeof(qry.qtype));
    tempbuff += sizeof(qry.qtype);

    memcpy(tempbuff, &qry.qclass, sizeof(qry.qclass));

//     int index = sizeof(struct dnsheader) +1 ;

//    int j = index;
//     qry.qname = &buff[j];
//     for(int i =0; i< addrlen;i++)
//     {
//         int count =0;
//         j++;
//         while(addr[i] != '.' && i<addrlen)
//         {
//             buff[j] = addr[i];
//             count++;
//             i++;
//             j++;
//         }
//         printf("j-count is %d and count is %d and j is %d\n", j-count-1, count,j);
//         buff[j-count-1] = (count);

//     }

    int sockfd;
    sockfd =socket(AF_INET,SOCK_DGRAM,0);

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    inet_pton(AF_INET,"192.168.1.1", &saddr.sin_addr.s_addr);
    saddr.sin_port = htons(PORT);

    // if(connect(sockfd, (struct sockaddr *)&saddr, sizeof(struct sockaddr)) < 0)
    // {
    //     perror("Connect Failed");
    //     exit(0);
    // }

    /*************/
    int i;
    int start = 0; // starting offset 
    int end = pktlen; // ending offset

for (i = start & ~15; i < end; i++)
{
    if  ((i & 15) == 0) 
       printf("%04x ",i);
    printf((i<start)?"   ":"%02x%c",(unsigned char)buff[i],((i+1)&15)?' ':'\n');
}
if ((i & 15) != 0)
    printf("\n"); 
    /********************/
    printf(" pktlen is %ld\n", pktlen);

    sendto(sockfd,buff, pktlen,0, (struct sockaddr *)&saddr, (socklen_t)sizeof(saddr));

    char recvbuff[512];
    memset(recvbuff,0,sizeof(buff));
    int len = recvfrom(sockfd,recvbuff,512,0, (struct sockaddr *)&saddr,&len);

    printf(" Message len is %d Received is \n",len);
    start = 0; // starting offset 
     end = len; // ending offset

    //int i;

for (i = start & ~15; i < end; i++)
{
    if  ((i & 15) == 0) 
       printf("%04x ",i);
    printf((i<start)?"   ":"%02x%c",(unsigned char)recvbuff[i],((i+1)&15)?' ':'\n');
}
if ((i & 15) != 0)
    printf("\n");

    close(sockfd);

//struct dnsheader *responsehdr = (struct dnsheader *)recvbuff;
struct dnsheader *responsehdr;
responsehdr = calloc(sizeof(struct dnsheader), sizeof(unsigned char));
memcpy(responsehdr, recvbuff, sizeof(struct dnsheader));
//printf("response hdr addr is %ld and recvbuff addr is %ld \n", responsehdr, recvbuff);
if((ntohs(responsehdr->rcode)) !=0 && ntohs(responsehdr->opcode) != dnshdr.opcode )
{
    printf("Received RCODE error\n");
    exit;
}

printf("Flag QR val %x , qdcount %x ,nscount %x , arcount %x  , ancount %x\n", ntohs(responsehdr->qr),ntohs(responsehdr->qdcount),ntohs(responsehdr->nscount),ntohs(responsehdr->arcount), ntohs(responsehdr->ancount));

unsigned char *nameptr = (unsigned char *)(recvbuff + sizeof(struct  dnsheader) + 1);
uint8_t total =0;
uint8_t *field_len = nameptr;
while(*field_len != 0)
{
    ++total;
    ++field_len;
    // total += *field_len + 1;
    // *field_len = '.';
    // printf("%c\t %d \t", *field_len,total);
    // field_len = nameptr + total;
}

//printf("addr of name ptr %ld and its value  len is %d field_len is %ld \n", nameptr, total, field_len);
++field_len;
unsigned short type;
memcpy(&type, field_len, sizeof(unsigned short));
printf("Received type %x\n", ntohs(type));

if(ntohs(type) == TYPE_ANY)
{
    //unsigned short rec_type;
    memcpy(&type, field_len+6, sizeof(unsigned short));
}

for(int i = 0; i < ntohs(responsehdr->ancount); i++)
{
    if(ntohs(type) == TYPE_A )
    {
        dns_record_a *a_records = (dns_record_a *)(field_len + 4);
        printf("addr of A-Record %ld and its size is %d\n", a_records, sizeof(dns_record_a));

    // for(int i = 0; i < ntohs(responsehdr->ancount); i++)
        {
            printf("Type: %"PRId16 "\n", ntohs(a_records->type));
            printf("Class: %d \n", ntohs(a_records->class));
            printf("TTL: %d sec\n", ntohl(a_records->ttl));
            printf("Len of RR specific data in octets %d\n", ntohs(a_records->rlength));
            printf("IPv4:  %08" PRIx32 "\n", ntohl(*(uint32_t *)&a_records->addr));
            printf("IPv4: %s \n", inet_ntoa(a_records->addr));
            //inet_ntoa()
        field_len += sizeof(dns_record_a);
        }
    }
    else if(ntohs(type) == TYPE_MAIL_SERVER)
    {
        dns_record_mx *mx_records = (dns_record_mx *)(field_len +4);
        for(int i = 0; i < ntohs(responsehdr->ancount); i++)
        {
            printf("Type: %"PRId16 "\n", ntohs(mx_records->type));
            printf("Class: %d \n", ntohs(mx_records->class));
            printf("TTL: %d sec\n", ntohl(mx_records->ttl));
            printf("Len of RR specific data in octets %d\n", ntohs(mx_records->rlength));

            mx_records->name = &mx_records->rlength + 2;
            int index =  (unsigned char *)recvbuff - mx_records->name;

            printf(" index is %d name addr %ld, recvbuff %ld \n", index, mx_records->name, recvbuff);
        // if(index < len)
                unsigned char *temp = mx_records->name;
            //[mx_records->rlength] = '\0';
            
            for(int i=0; i< ntohs(mx_records->rlength); i++)
            {
                //while(*temp != 0)
                printf("%c", *temp);
                //puts(temp);
                temp++;
            }
            //printf(" MX name is %31s\n", mx_records->name);
            field_len += sizeof(dns_record_mx) +  ntohs(mx_records->rlength) - sizeof(unsigned char *);
        }

        

    }
    else if(ntohs(type) == TYPE_AAAA)
    {
        dns_record_aaaa *aaaa_rec = (dns_record_aaaa *)(field_len + 4);
        printf("AAAA Record\n");
            //printf("Type: %"PRId16 "\n", ntohs(aaaa_rec->type));
            printf("Class: %d \n", ntohs(aaaa_rec->class));
            printf("TTL: %d sec\n", ntohl(aaaa_rec->ttl));
            printf("Len of RR specific data in octets %d\n", ntohs(aaaa_rec->rlength));

            char addr_ipv6[INET6_ADDRSTRLEN];
            //printf("IPv4:  %08" PRIx32 "\n", ntohl(*(uint32_t *)&aaaa_rec->addr));
            printf("IPv6: %s \n", inet_ntop(AF_INET6,&aaaa_rec->addr,addr_ipv6, INET6_ADDRSTRLEN));
            field_len += sizeof(dns_record_aaaa);
            
    }
}

}
