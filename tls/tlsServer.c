#include<errno.h>
#include<unistd.h>
#include<malloc.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<resolv.h>

#include<openssl/ssl.h>
#include<openssl/err.h>

#define FAIL -1

int OpenListener(int port)
{
    int sd;
    struct sockaddr_in addr;

    sd = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&addr, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0)
    {
        perror(" Bind failed");
        abort();
    }

    if( listen(sd,5) != 0 )
    {
        perror("Listen failed");
        abort;
    }
    return sd;

}

int isRoot()
{
    if(getuid() != 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void LoadCertificates(SSL_CTX *ctx, char *CertFile, char *KeyFile)
{
    /*set local certificate from CertFile */
    if(SSL_CTX_use_certificate_file(ctx,CertFile,SSL_FILETYPE_PEM) <=0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }

    /* set private key from KeyFile(may be the same as CertFile) */
    if(SSL_CTX_use_PrivateKey_file(ctx,KeyFile, SSL_FILETYPE_PEM) <=0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }

    /* verify private key */
    if(!SSL_CTX_check_private_key(ctx))
    {
        fprintf(stderr, "Private key does  not match the public certificate\n");
        abort();
    }


}


SSL_CTX* InitServerCTX(void)
{
    SSL_METHOD *method;
    SSL_CTX *ctx;

    OpenSSL_add_all_algorithms();  /*load & register all cryptos, etc. */
    SSL_load_error_strings(); /*load all error messages*/
    method = TLSv1_2_server_method(); /*create new server-method instance */
    ctx = SSL_CTX_new(method); /*create new context from method */
    if( ctx == NULL)
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
    cert = SSL_get_peer_certificate(ssl);

    if(cert != NULL)
    {
        printf("Server Certificate\n");

        line = X509_NAME_oneline(X509_get_subject_name(cert),0,0);
        printf(" Subject Name : %s \n", line);
        free(line);

        line = X509_NAME_oneline(X509_get_issuer_name(cert),0,0);
        printf("Issuer : %s \n",line);
        free(line);
        X509_free(cert);

    }
    else
    {
        printf("No certificate\n");
    }
}

void  servlet(SSL* ssl) 
{

    char buf[1024] = {0};

    int sd, msgLen;
    const char* ServerResponse= "<Body><Name>aticleworld.com</Name><year>2</year><Author>Amlendra</Author></Body>";

    const char *cpValidMessage = "<Body><UserName>aticle</UserName><Password>123</Password></Body>";
    
    if( SSL_accept(ssl) == FAIL)   /*SSL protocol accept*/
    {
        ERR_print_errors_fp(stderr);
    }
    else
    {
        showCerts(ssl); /* get certificates*/
        msgLen = SSL_read(ssl,buf, sizeof(buf));
        buf[msgLen] = '\0';

        printf("client msg: %s\n",buf);
        if(msgLen > 0)
        {
            if(strcmp(cpValidMessage,buf) == 0)
            {
                SSL_write(ssl,ServerResponse,strlen(ServerResponse));

            }
            else
            {
                SSL_write(ssl,"Invlid Message", strlen("InvalidMessage"));
            }

        }
        else
        {
            ERR_print_errors_fp(stderr);
        }
    }
    sd = SSL_get_fd(ssl); //get socket connection
    SSL_free(ssl);     //release SSL state
    close(sd);  //close the connection

}


int main(int argv, char *argc[])
{
    SSL_CTX *ctx;
    int server;
    char *portnum;

    if(!isRoot())
    {
        printf("Run this program as ROOT user\n");
        exit(0);
    }

    if(argv != 2)
    {
        printf("Usage %s <portnum>\n", argc[0]);
        exit(0);
    }

    SSL_library_init();   /*Initialize SSL Library*/

    portnum = argc[1];
    ctx = InitServerCTX(); /* initialize SSL*/
    LoadCertificates(ctx,"mycert.pem", "mycert.pem");
    server = OpenListener(atoi(portnum));  /* creating socket for server and does bind and listening */

    while(1)
    {
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        int client = accept(server, (struct sockaddr*)&addr,&len );

        printf(" Connection details : %s : %d\n",inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

        SSL *ssl;
        ssl = SSL_new(ctx);   /* get new SSL state with context */

        SSL_set_fd(ssl, client);  /* set connection socket to SSL state */
        servlet(ssl);      /* service connection */
    }
    close(server);
    SSL_CTX_free(ctx);
}







