#include <stdio.h>

#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"

#define EXIT_IF_TRUE(x) if (x)                              \
    do {                                                    \
            fprintf(stderr, "Check '%s' is true\n", #x);    \
            ERR_print_errors_fp(stderr);                    \
            exit(2);                                        \
    }while(0)

int main(int argc, char **argv)
{
    SSL_CTX     *ctx;
    SSL         *ssl;
    X509        *client_cert;

    char szBuffer[1024];
    int nLen;

    struct sockaddr_in addr;
    int len;
    int nListenFd, nAcceptFd;

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
    EXIT_IF_TRUE (!SSL_CTX_load_verify_locations(ctx, "cacert.cer", NULL));

    // 加载自己的证书
    EXIT_IF_TRUE (SSL_CTX_use_certificate_file(ctx, "server.cer", SSL_FILETYPE_PEM) <= 0) ;

    // 加载自己的私钥
    EXIT_IF_TRUE (SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM) <= 0) ;

    // 判定私钥是否正确
    EXIT_IF_TRUE (!SSL_CTX_check_private_key(ctx));

    // 创建并等待连接
    nListenFd = cutil_socket_new(SOCK_STREAM);
    cutil_socket_bind(nListenFd, NULL, 8812, 1);

    memset(&addr, 0, sizeof(addr));
    len = sizeof(addr);
    nAcceptFd = accept(nListenFd, (struct sockaddr *)&addr, (size_t *)&len);
    printf("Accept a connect from [%s:%d]\n",
        inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

    // 将连接付给SSL
    EXIT_IF_TRUE( (ssl = SSL_new (ctx)) == NULL);
    SSL_set_fd (ssl, nAcceptFd);
    EXIT_IF_TRUE( SSL_accept (ssl) != 1);

    // 进行操作
    memset(szBuffer, 0, sizeof(szBuffer));
    nLen = SSL_read(ssl,szBuffer, sizeof(szBuffer));
    fprintf(stderr, "Get Len %d %s ok\n", nLen, szBuffer);
    strcat(szBuffer, " this is from server");
    SSL_write(ssl, szBuffer, strlen(szBuffer));

    // 释放资源
    SSL_free (ssl);
    SSL_CTX_free (ctx);
    close(nAcceptFd);
}
