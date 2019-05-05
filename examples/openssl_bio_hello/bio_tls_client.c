#include <stdio.h>
#include <string.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

int main()
{
    int iResult = 0; // 零为成功，负数为错误码

    BIO* bio = NULL;
    SSL* ssl = NULL;
    SSL_CTX* ctx = NULL;
    X509* pX509 = NULL;
    SSL_METHOD* sslMethod = NULL;

    char commonName[512] = { 0 };
    X509_NAME* pX509_NAME = NULL;

    // http://192.168.12.39/testssl 对应的 HTTP 请求协议
    // 其中的 /testssl/ 最后的 / 是必须的
    char szHttpReq[] = "GET /testssl/ HTTP/1.1\r\n"
            "Host:192.168.12.39\r\n"
            "Connection: keep-alive\r\n\r\n";

    // 一次收到的字节数
    unsigned int uiRecBytes = 0;

    // 接收服务端信息的字节缓冲区
    unsigned char baRecBuffer[1024];
    // 最终收完后的服务端信息缓冲区
    unsigned char* pbRecFinish = NULL;
    // 临时备份缓冲区
    unsigned char* pbRecBak = NULL;
    // 临时备份缓冲区字节大小
    unsigned int uiRecBakLen = 0;

    // 按行打印收到的信息
    char* pMsgline = NULL;

    // 初始化 OpenSSL 库，只需以下四行代码
    // 初始化 SSL 算法库函数，调用 SSL 系列函数之前必须调用此函数！
    SSL_library_init();
    // 加载 BIO 抽像库错误信息
    ERR_load_BIO_strings();
    // 加载 SSL 抽像库错误信息
    SSL_load_error_strings();
    // 加载所有加密和散列函数
    OpenSSL_add_all_algorithms();

    do // 非循环，只是为了减少分支缩进
    {
        // 设置客户端使用的 SSL 协议算法
        sslMethod = TLSv1_client_method();
        if ( NULL == sslMethod) {
            printf("TLSv1_client_method err: %s\n",
                    ERR_error_string(ERR_get_error(),
                    NULL));
            iResult = -1;
            break;
        }

        // 创建 SSL 上下文
        ctx = SSL_CTX_new(sslMethod);
        if ( NULL == ctx) {
            printf("SSL_CTX_new err: %s\n", ERR_error_string(ERR_get_error(),
            NULL));
            iResult = -2;
            break;
        }

        // 加载可信任的 CA 证书
        if (0 == SSL_CTX_load_verify_locations(ctx, "certnew.pem",
        NULL)) {
            printf("SSL_CTX_load_verify_locations err: %s\n",
                    ERR_error_string(ERR_get_error(),
                    NULL));
            iResult = -3;
            break;
        }

        // 加载客户端证书
        if (0 == SSL_CTX_use_certificate_file(ctx, "test1.pem",
        SSL_FILETYPE_PEM)) {
            printf("SSL_CTX_use_certificate_file err: %s\n",
                    ERR_error_string(ERR_get_error(),
                    NULL));
            iResult = -4;
            break;
        }

        // 加载客户端私钥文件
        if (0 == SSL_CTX_use_PrivateKey_file(ctx, "private.key",
        SSL_FILETYPE_PEM)) {
            printf("SSL_CTX_use_PrivateKey_file err: %s\n",
                    ERR_error_string(ERR_get_error(),
                    NULL));
            iResult = -5;
            break;
        }

        // 验证私钥是否与证书一致
        if (0 == SSL_CTX_check_private_key(ctx)) {
            printf("SSL_CTX_check_private_key err: %s\n",
                    ERR_error_string(ERR_get_error(),
                    NULL));
            iResult = -6;
            break;
        }

        // 创建 BIO 对象
        bio = BIO_new_ssl_connect(ctx);
        if ( NULL == bio) {
            printf("BIO_new_ssl_connect err: %s\n",
                    ERR_error_string(ERR_get_error(),
                    NULL));
            iResult = -7;
            break;
        }

        // 获得指向 SSL 结构的指针
        BIO_get_ssl(bio, &ssl);

        // SSL_MODE_AUTO_RETRY - 如果服务端希望进行一次新的握手，OpenSSL 后台处理它。
        // 没有 SSL_MODE_AUTO_RETRY 此选项，则新握手返回错误，且设置 retry 标记。
        SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

        // 建立与服务端的连接
        BIO_set_conn_hostname(bio, "192.168.12.39:443");

        // 为了确认成功打开连接，需执行 BIO_do_connect 函数
        // 该调用还将执行握手来建立安全连接
        if (0 >= BIO_do_connect(bio)) {
            printf("BIO_do_connect err: %s\n", ERR_error_string(ERR_get_error(),
            NULL));
            iResult = -8;
            break;
        }

        // 连接建立后，必须检查证书，以确定它是否有效。
        // 实际上，OpenSSL 为我们完成了这项任务。
        // 如果证书有致命的问题(如：哈希值无效)，那么将无法建立连接。
        // 如果证书无致命的问题(如：已过期或尚不合法时)，那么可以继续使用连接。
        // 调用 SSL_get_verify_result 来查明证书是否通了 OpenSSL 的检验。
        // 如果证书通过了包括信任检查在内的 OpenSSL 的内部检查，则返回 X509_V_OK 。
        // 如果有地方出了问题，则返回一个错误码，该代码被记录在命令行工具的 verify 选项下。
        if ( X509_V_OK != SSL_get_verify_result(ssl)) {
            printf("SSL_get_verify_result err: %s\n",
                    ERR_error_string(ERR_get_error(),
                    NULL));
            iResult = -9;
            break;
        }

        // 如果您希望向用户显示证书的内容，或者要根据主机名或证书权威对证书进行验证，
        // 那么就需要检索证书的内容。
        // 要在验证测试结果之后再检索证书，请调用 SSL_get_peer_certificate()。
        // 它返回一个指向该证书的 X509 指针，如果证书不存在，就返回 NULL 。
        pX509 = SSL_get_peer_certificate(ssl);
        if ( NULL == pX509) {
            printf("SSL_get_peer_certificate err: %s\n",
                    ERR_error_string(ERR_get_error(),
                    NULL));
            iResult = -10;
            break;
        }

        // 使用 X509_get_subject_name() 从证书中检索 X509_NAME 结构。
        // 这会返回一个指向 X509_NAME 的对象。
        pX509_NAME = X509_get_subject_name(pX509);
        if ( NULL == pX509_NAME) {
            printf("X509_get_subject_name err: %s\n",
                    ERR_error_string(ERR_get_error(),
                    NULL));
            iResult = -10;
            break;
        }

        X509_NAME_get_text_by_NID(pX509_NAME,
        NID_commonName, commonName, 512);
        if (0 != strcasecmp(commonName, "192.168.12.39")) {
            printf("Certificate's name 192.168.12.39 != %s\n", commonName);
            iResult = -11;
            break;
        }

        // 通过 SSL 发送 HTTP 请求
        // BIO_write 会试着将字节写入套接字。
        // 它将返回实际写入的字节数、0 或者 -1。
        // 同 BIO_read ，0 或 -1 不一定表示错误。
        // BIO_should_retry 是找出问题的途径。
        // 如果需要重试写操作，它必须使用和前一次完全相同的参数。
        if (0 >= BIO_write(bio, szHttpReq, strlen(szHttpReq))) {
            printf("Sent HTTP request failed by BIO_write\n");
            iResult = -12;
            break;
        }

        // 开始接收服务端信息数据
        // BIO_read 将尝试从服务器读取一定数目的字节。
        // 它返回读取的字节数、 0 或者 -1。
        // 在受阻塞的连接中，该函数返回 0，表示连接已经关闭，而 -1 则表示连接出现错误。
        // 在非阻塞连接的情况下，返回 0 表示没有可以获得的数据，返回 -1 表示连接出错。
        // 可以调用 BIO_should_retry 来确定是否可能重复出现该错误。
        while (0
                < (uiRecBytes = BIO_read(bio, (void*) baRecBuffer,
                        sizeof(baRecBuffer)))) {
            // 最终收完后的服务端信息缓冲区指针不为空说明要先备份后扩展
            uiRecBakLen = 0; // 每次都清空以防计算错误
            if ( NULL != pbRecFinish) {
                uiRecBakLen = strlen((char*) pbRecFinish);

                pbRecBak = (unsigned char*) malloc(uiRecBakLen);

                // 分配失败
                if ( NULL == pbRecBak) {
                    printf("pbRecBak call malloc to allocate mem fail\n");
                    free(pbRecFinish);
                    pbRecFinish = NULL;

                    uiRecBakLen = 0; // 每次都清空以防计算错误

                    break;
                }

                // 结尾的零没有一起拷贝!!!
                memcpy(pbRecBak, pbRecFinish, uiRecBakLen);

                free(pbRecFinish);
                pbRecFinish = NULL;
            }

            // 备份缓冲区和接收缓冲区都没有结尾的零，所以要多分一个字节
            pbRecFinish = (unsigned char*) malloc(uiRecBakLen + uiRecBytes + 1);

            // 分配失败
            if (( NULL == pbRecFinish) && ( NULL != pbRecBak)) {
                printf("pbRecFinish call malloc to allocate mem fail\n");
                free(pbRecBak);
                pbRecBak = NULL;

                uiRecBakLen = 0; // 每次都清空以防计算错误

                break;
            }

            // 全清零，此操作比数组快，且为字符串最后补零了
            memset(pbRecFinish, 0, uiRecBakLen + uiRecBytes + 1);

            // 先恢复备份的
            if ( NULL != pbRecBak) {
                memcpy(pbRecFinish, pbRecBak, uiRecBakLen);

                free(pbRecBak);
                pbRecBak = NULL;
            }

            // 再拷贝入新收的
            memcpy(pbRecFinish + uiRecBakLen, baRecBuffer, uiRecBytes);

            // 未收满，说明没有数据可收了
            if (sizeof(baRecBuffer) > uiRecBytes) {
                break;
            }
        }

        if ( NULL != pbRecFinish) {
            printf("%s\n", strtok((char*) pbRecFinish, "\r\n"));
            while ( NULL != (pMsgline = strtok( NULL, "\r\n"))) {
                printf("%s\n", pMsgline);
            }

            free(pbRecFinish);
            pbRecFinish = NULL;
        }

        break; // 流程到此结束
    } while (0);

    if ( NULL != pbRecFinish) {
        // 释放申请的内存
        free(pbRecFinish);
        pbRecFinish = NULL;
    }

    if ( NULL == bio) {
        // 释放内部结构体相关的内存
        BIO_free_all(bio);
    }

    if ( NULL != ctx) {
        // 清除 SSL 上下文
        SSL_CTX_free(ctx);
    }

    return iResult;
}
