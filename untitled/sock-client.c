#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define PORT 8000

int main()
{
    int status;
    int sockfd;
    struct addrinfo hints;
    struct addrinfo *servinfo=NULL;

    memset(&hints,0,sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if((status=getaddrinfo("www.google.com","https",&hints,&servinfo)==-1)){
        perror("status");
        exit(1);
    }

    sockfd=socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if(sockfd<0){
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }
    if(connect(sockfd,servinfo->ai_addr,servinfo->ai_addrlen)<0){
        perror("Unable to connect");
        exit(EXIT_FAILURE);
    }
    SSL *ssl;
    SSL_library_init();
    SSLeay_add_ssl_algorithms();
    SSL_load_error_strings();
    const SSL_METHOD *meth=TLSv1_2_client_method();
    SSL_CTX *ctx = SSL_CTX_new(meth);
    ssl=SSL_new(ctx);
    if(!ssl){
        printf("Error creating SSL\n");
        //log_ssl();
        fflush(stdout);
        return -1;
    }
    int sock = SSL_get_fd(ssl);
    SSL_set_fd(ssl, sockfd);
    int err = SSL_connect(ssl);
    if (err <= 0) {
        printf("Error creating SSL connection.  err=%x\n", err);
        //log_ssl();
        fflush(stdout);
        return -1;
    }
    printf ("SSL connection using %s\n", SSL_get_cipher (ssl));

    char *buf;
    buf=calloc(65536,65536*sizeof(char));
    //int numbytes;
    char *msg = "GET / HTTP/1.0\nHost: google.com\nUser-Agent: Mozilla/5.0 (X11; U; Linux i686; ru; rv:1.9b5) Gecko/2008050509 Firefox/3.0b5\nAccept: text/html\nConnection: close\n\n";
    int len;

    len = strlen(msg);
    //send(sockfd, msg, len, 0);
    int numbytes = SSL_write(ssl, msg, len);
    SSL_read(ssl,buf,65536);
//    if ((numbytes = recv(sockfd, buf, 65536-1, 0)) == -1) {

//        perror("send");
//        exit(1);
//    }

   printf("client: received->\n'%s'\n",buf);
    buf[numbytes] = '\0';
    close(sockfd);
    freeaddrinfo(servinfo);
    free(buf);

}
