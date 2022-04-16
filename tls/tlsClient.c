#include<stdio.h>
#include<errno.h>
#include<unistd.h>
#include<malloc.h>
#include<string.h>
#include<sys/socket.h>
#include<resolv.h>
#include<netdb.h>
#include"openssl/ssl.h"
#include"openssl/err.h"

#define FAIL -1

int openConnection(const char* hostname, int port)
{
    int sd;
    struct hostent *host;
    struct sockaddr_in addr;

    if((host = gethostbyname(hostname)) == NULL)
    {
        perror(hostname);
        abort();
    }

    sd= socket(PF_INET,SOCK_STREAM,0);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = *(long*)(host->h_addr_list[0]);

    if(connect(sd,(struct sockaddr*)&addr,sizeof(addr)) !=0 )
    {
        close(sd);
        perror(hostname);
        abort();
    }
    return sd;
}

SSL_CTX* init_CTX(void)
{
    SSL_METHOD *method;
    SSL_CTX *ctx;

    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    method = TLSv1_2_client_method();
    ctx = SSL_CTX_new(method);

    if (ctx == NULL)
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}

void showCerts(SSL* ssl)
{
    X509 *cert;
    char *line;
    cert =SSL_get_peer_certificate(ssl);
    if(cert != NULL)
    {
        printf("Server Certificate:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert),0,0);
        printf(" Subject %s\n",line);
        free(line);

        line = X509_NAME_oneline(X509_get_issuer_name(cert),0,0);
        printf(" Issuer : %s\n",line);
        free(line);
        X509_free(cert);

    }
    else
    {
        printf("Info: No client certificates configured\n");
        printf(" No certificate was presented by peer or no connection established\n");
    }
}

int main(int count ,char *strings[])
{
    SSL_CTX *ctx;
    int server;
    SSL *ssl;
    char buf[1024];
    char acClientRequest[1024] = {0};
    int bytes;
    char *hostname, *portnum;

    if(count != 3)
    {
        printf("usage :%s <hostname> <portnum>\n", strings[0]);
        exit(0);
    }

    SSL_library_init();
    hostname = strings[1];
    portnum =  strings[2];

    ctx = init_CTX();

    server = openConnection(hostname, atoi(portnum));

    ssl = SSL_new(ctx); /*create new SSL connection state*/

    SSL_set_fd(ssl, server); /*attach the socket descriptor*/

    if( SSL_connect(ssl) == FAIL) /* perform the SSl connection */
    {
        ERR_print_errors_fp(stderr);
    }
    else
    {
        char acUserName[16] = {0};
        char acPassword[16] = {0};
        const char *cpRequestMessage = "<Body><UserName>%s</UserName><Password>%s</Password></Body>";
        
        printf("Enter the username : ");
        scanf("%s",acUserName);

        printf("\nEnter the password: ");
        scanf("%s",acPassword);

        sprintf(acClientRequest, cpRequestMessage, acUserName, acPassword);

        printf("\n\n Connected with %s encryption\n", SSL_get_cipher(ssl));

        showCerts(ssl);

        SSL_write(ssl,acClientRequest,strlen(acClientRequest)); /*encrypt and send message*/

        bytes = SSL_read(ssl, buf, sizeof(buf));

        buf[bytes] = 0;

        printf("Received: %s\n",buf);
    }

    close(server);
    SSL_CTX_free(ctx);
    return 0;

}