#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
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
    int listenfd, connfd, n, ret;
    struct sockaddr_in servaddr;
    char buff[MAXLINE];

    SSL_CTX *ctx;
    SSL *ssl;
    X509 *client_cert;

    // 初始化
    SSLeay_add_ssl_algorithms();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ERR_load_BIO_strings();

    // 我们使用SSL V3,V2
    EXIT_IF_TRUE((ctx = SSL_CTX_new (SSLv23_method())) == NULL);

    // 要求校验对方证书
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

    // 加载CA的证书
    EXIT_IF_TRUE (!SSL_CTX_load_verify_locations(ctx, CERT_PATH"ca.crt", NULL));

    // 加载自己的证书
    EXIT_IF_TRUE (SSL_CTX_use_certificate_file(ctx, CERT_PATH"server.crt", SSL_FILETYPE_PEM) <= 0) ;

    // 加载自己的私钥
    EXIT_IF_TRUE (SSL_CTX_use_PrivateKey_file(ctx, CERT_PATH"server.key", SSL_FILETYPE_PEM) <= 0) ;

    // 判定私钥是否正确
    EXIT_IF_TRUE (!SSL_CTX_check_private_key(ctx));

    // 创建并等待连接
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(6666);

    if (bind(listenfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) == -1) {
        printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }

    if (listen(listenfd, 10) == -1) {
        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }

    printf("========waiting for client's request========\n");
    while (1) {
        if ((connfd = accept(listenfd, (struct sockaddr*) NULL, NULL)) == -1) {
            printf("accept socket error: %s(errno: %d)\n", strerror(errno),
                    errno);
            continue;
        }

        // 将连接付给SSL
        EXIT_IF_TRUE( (ssl = SSL_new (ctx)) == NULL);
        SSL_set_fd (ssl, connfd);

        ret = SSL_accept(ssl);
        if (ret != 1) {
            printf("%d %s\n", ret, SSL_state_string_long(ssl));
        }

        // 进行操作
        memset(buff, 0, sizeof(buff));
        n = SSL_read(ssl, buff, sizeof(buff));
        printf("Get length %d message: %s\n", n, buff);
        strcat(buff, " echo from server");
        SSL_write(ssl, buff, strlen(buff));

        // 释放资源
        SSL_free (ssl);
        SSL_CTX_free (ctx);
        close(connfd);
    }
    close(listenfd);
    return 0;
}
