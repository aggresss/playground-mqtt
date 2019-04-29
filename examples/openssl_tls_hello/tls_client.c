#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"

#include "config.h"

#define CERT_PATH PROJECT_SOURCE_DIR"/material/tls/"
#define MAXLINE 4096

#define EXIT_IF_TRUE(x) if (x)                              \
    do {                                                    \
            fprintf(stderr, "Check '%s' is true\n", #x);    \
            ERR_print_errors_fp(stderr);                    \
            exit(2);                                        \
    }while(0)

int main(int argc, char** argv)
{
    int sockfd, n;
    struct sockaddr_in servaddr;
    char sendline[MAXLINE], recvline[MAXLINE];

    SSL_METHOD *meth;
    SSL_CTX *ctx;
    SSL *ssl;

    // 初始化
    SSLeay_add_ssl_algorithms();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ERR_load_BIO_strings();

    // 我们使用SSL V3,V2
    EXIT_IF_TRUE((ctx = SSL_CTX_new (SSLv23_method())) == NULL);

    // 要求校验对方证书
//    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

    // 加载CA的证书
    EXIT_IF_TRUE (!SSL_CTX_load_verify_locations(ctx, CERT_PATH"ca.crt", NULL));

    // 加载自己的证书
    EXIT_IF_TRUE (SSL_CTX_use_certificate_file(ctx, CERT_PATH"client.crt", SSL_FILETYPE_PEM) <= 0) ;

    // 加载自己的私钥
    EXIT_IF_TRUE (SSL_CTX_use_PrivateKey_file(ctx, CERT_PATH"client.key", SSL_FILETYPE_PEM) <= 0) ;

    // 判定私钥是否正确
    EXIT_IF_TRUE (!SSL_CTX_check_private_key(ctx));

    if (argc != 2) {
        printf("usage: ./client <ipaddress>\n");
        return 0;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(6666);
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        printf("inet_pton error for %s\n", argv[1]);
        return 0;
    }

    if (connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
        printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }


    if (send(sockfd, sendline, strlen(sendline), 0) < 0) {
        printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }
    close(sockfd);
    return 0;


    // 将连接付给SSL
    EXIT_IF_TRUE( (ssl = SSL_new (ctx)) == NULL);
    SSL_set_fd (ssl, sockfd);
    EXIT_IF_TRUE( SSL_connect (ssl) != 1);

    // 进行操作
    printf("send msg to server: \n");
    fgets(sendline, MAXLINE, stdin);
    SSL_write(ssl, sendline, strlen(sendline));


    memset(recvline, 0, sizeof(recvline));
    n = SSL_read(ssl, recvline, sizeof(recvline));
    fprintf(stderr, "Get Len %d %s ok\n", n, recvline);

    // 释放资源
    SSL_free (ssl);
    SSL_CTX_free (ctx);
    close(sockfd);
}

