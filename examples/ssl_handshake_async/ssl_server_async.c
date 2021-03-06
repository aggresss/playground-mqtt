/*
 * Fork from https://github.com/yedf/openssl-example/blob/master/sync-ssl-svr.cc
 * */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/epoll.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "config.h"

#define CERT_PATH PROJECT_SOURCE_DIR"/material/tls/"

#define log(...) do { printf(__VA_ARGS__); fflush(stdout); } while(0)
#define check0(x, ...) if(x) do { log( __VA_ARGS__); exit(1); } while(0)
#define check1(x, ...) if(!x) do { log( __VA_ARGS__); exit(1); } while(0)

BIO* errBio;
SSL_CTX* g_sslCtx;

int epollfd, listenfd;

typedef struct Channel_s {
    int fd;
    SSL *ssl;
    bool tcpConnected;
    bool sslConnected;
    int events;
} Channel;


static Channel * create_channel(int fd, int events) {
        Channel * ch = malloc(sizeof(*ch));
        if (ch == NULL) {
            return NULL;
        }
        memset(ch, 0, sizeof(*ch));
        ch->fd = fd;
        ch->events = events;
        return ch;
    }

static void destroy_channel(Channel * ch)
{
    if (!ch) {
        return;
    }
    log("deleting fd %d\n", ch->fd);
    close(ch->fd);
    if (ch->ssl) {
        SSL_shutdown(ch->ssl);
        SSL_free(ch->ssl);
    }
    free(ch);
}

static void update_channel(Channel * ch) {
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = ch->events;
    ev.data.ptr = ch;
    log("modifying fd %d events read %d write %d\n",
        ch->fd, ev.events & EPOLLIN, ev.events & EPOLLOUT);
    int r = epoll_ctl(epollfd, EPOLL_CTL_MOD, ch->fd, &ev);
    check0(r, "epoll_ctl mod failed %d %s", errno, strerror(errno));
}

static int setNonBlock(int fd, bool value) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        return errno;
    }
    if (value) {
        return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }
    return fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
}


static void addEpollFd(int epollfd, Channel* ch) {
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = ch->events;
    ev.data.ptr = ch;
    log("adding fd %d events %d\n", ch->fd, ev.events);
    int r = epoll_ctl(epollfd, EPOLL_CTL_ADD, ch->fd, &ev);
    check0(r, "epoll_ctl add failed %d %s", errno, strerror(errno));
}

static int createServer(short port) {
    int fd = socket(AF_INET, SOCK_STREAM|SOCK_CLOEXEC, 0);
    setNonBlock(fd, 1);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    int r = bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr));
    check0(r, "bind to 0.0.0.0:%d failed %d %s", port, errno, strerror(errno));
    r = listen(fd, 20);
    check0(r, "listen failed %d %s", errno, strerror(errno));
    log("fd %d listening at %d\n", fd, port);
    return fd;
}

static void handleAccept() {
    struct sockaddr_in raddr;
    socklen_t rsz = sizeof(raddr);
    int cfd;
    while ((cfd = accept(listenfd,(struct sockaddr *)&raddr,&rsz))>=0) {
        struct sockaddr_in peer, local;
        socklen_t alen = sizeof(peer);
        int r = getpeername(cfd, (struct sockaddr*)&peer, &alen);
        if (r < 0) {
            log("get peer name failed %d %s\n", errno, strerror(errno));
            continue;
        }
        r = getsockname(cfd, (struct sockaddr *)&local, &alen);
        if (r < 0) {
            log("getsockname failed %d %s\n", errno, strerror(errno));
            continue;
        }
        setNonBlock(cfd, 1);
        Channel* ch = create_channel(cfd, EPOLLIN | EPOLLOUT);
        addEpollFd(epollfd, ch);
    }
}

static void handleHandshake(Channel* ch) {
    if (!ch->tcpConnected) {
        struct pollfd pfd;
        pfd.fd = ch->fd;
        pfd.events = POLLOUT | POLLERR;
        int r = poll(&pfd, 1, 0);
        if (r == 1 && pfd.revents == POLLOUT) {
            log("tcp connected fd %d\n", ch->fd);
            ch->tcpConnected = true;
            ch->events = EPOLLIN | EPOLLOUT | EPOLLERR;
            update_channel(ch);
        } else {
            log("poll fd %d return %d revents %d\n", ch->fd, r, pfd.revents);
            destroy_channel(ch);
            return;
        }
    }
    if (ch->ssl == NULL) {
        ch->ssl = SSL_new (g_sslCtx);
        check0(ch->ssl == NULL, "SSL_new failed");
        int r = SSL_set_fd(ch->ssl, ch->fd);
        check0(!r, "SSL_set_fd failed");
        log("SSL_set_accept_state for fd %d\n", ch->fd);
        SSL_set_accept_state(ch->ssl);
    }
    int r = SSL_do_handshake(ch->ssl);
    if (r == 1) {
        ch->sslConnected = true;
        log("ssl connected fd %d\n", ch->fd);
        return;
    }
    int err = SSL_get_error(ch->ssl, r);
    int oldev = ch->events;
    if (err == SSL_ERROR_WANT_WRITE) {
        ch->events |= EPOLLOUT;
        ch->events &= ~EPOLLIN;
        log("return want write set events %d\n", ch->events);
        if (oldev == ch->events) return;
        update_channel(ch);
    } else if (err == SSL_ERROR_WANT_READ) {
        ch->events |= EPOLLIN;
        ch->events &= ~EPOLLOUT;
        log("return want read set events %d\n", ch->events);
        if (oldev == ch->events) return;
        update_channel(ch);
    } else {
        log("SSL_do_handshake return %d error %d errno %d msg %s\n", r, err, errno, strerror(errno));
        ERR_print_errors(errBio);
        destroy_channel(ch);;
    }
}

static void handleDataRead(Channel* ch) {
    char buf[4096];
    int rd = SSL_read(ch->ssl, buf, sizeof buf);
    int ssle = SSL_get_error(ch->ssl, rd);
    if (rd > 0) {
        const char* cont = "HTTP/1.1 200 OK\r\nConnection: Close\r\n\r\n";
        int len1 = strlen(cont);
        int wd = SSL_write(ch->ssl, cont, len1);
        log("SSL_write %d bytes\n", wd);
        destroy_channel(ch);;
    }
    if (rd < 0 && ssle != SSL_ERROR_WANT_READ) {
        log("SSL_read return %d error %d errno %d msg %s", rd, ssle, errno, strerror(errno));
        destroy_channel(ch);;
        return;
    }
    if (rd == 0) {
        if (ssle == SSL_ERROR_ZERO_RETURN)
            log("SSL has been shutdown.\n");
        else
            log("Connection has been aborted.\n");
        destroy_channel(ch);;
    }
}

static void handleRead(Channel* ch) {
    if (ch->fd == listenfd) {
        return handleAccept();
    } else if (ch->sslConnected) {
        return handleDataRead(ch);
    } else {
        return handleHandshake(ch);
    }
}

static void handleWrite(Channel* ch) {
    if (!ch->sslConnected) {
        return handleHandshake(ch);
    }
    log("handle write fd %d\n", ch->fd);
    ch->events &= ~EPOLLOUT;
    update_channel(ch);
}

static void initSSL() {
    SSL_load_error_strings ();
    int r = SSL_library_init ();
    check0(!r, "SSL_library_init failed");
    g_sslCtx = SSL_CTX_new (SSLv23_method ());
    check0(g_sslCtx == NULL, "SSL_CTX_new failed");
    errBio = BIO_new_fd(2, BIO_NOCLOSE);

    r = SSL_CTX_use_certificate_file(g_sslCtx, CERT_PATH"server.crt", SSL_FILETYPE_PEM);
    check0(r<=0, "SSL_CTX_use_certificate_file %s failed", CERT_PATH"server.crt");
    r = SSL_CTX_use_PrivateKey_file(g_sslCtx, CERT_PATH"server.key", SSL_FILETYPE_PEM);
    check0(r<=0, "SSL_CTX_use_PrivateKey_file %s failed", CERT_PATH"server.key");
    r = SSL_CTX_check_private_key(g_sslCtx);
    check0(!r, "SSL_CTX_check_private_key failed");
    log("SSL inited\n");
}

int g_stop = 0;

static void loop_once(int epollfd, int waitms) {
    const int kMaxEvents = 20;
    struct epoll_event activeEvs[kMaxEvents];
    int n = epoll_wait(epollfd, activeEvs, kMaxEvents, waitms);
    for (int i = n-1; i >= 0; i --) {
        Channel* ch = (Channel*)activeEvs[i].data.ptr;
        int events = activeEvs[i].events;
        if (events & (EPOLLIN | EPOLLERR)) {
            log("fd %d handle read\n", ch->fd);
            handleRead(ch);
        } else if (events & EPOLLOUT) {
            log("fd %d handle write\n", ch->fd);
            handleWrite(ch);
        } else {
            log("unknown event %d\n", events);
        }
    }
}

static void handleInterrupt(int sig) {
    g_stop = true;
}

int main(int argc, char **argv)
{
    signal(SIGINT, handleInterrupt);
    initSSL();
    epollfd = epoll_create1(EPOLL_CLOEXEC);
    listenfd = createServer(443);
    Channel* li = create_channel(listenfd, EPOLLIN);
    addEpollFd(epollfd, li);
    while (!g_stop) {
        loop_once(epollfd, 100);
    }
    destroy_channel(li);
    close(epollfd);
    BIO_free(errBio);
    SSL_CTX_free(g_sslCtx);
    ERR_free_strings();
    log("program exited\n");
    return 0;
}
