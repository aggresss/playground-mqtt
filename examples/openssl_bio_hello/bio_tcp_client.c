#include <openssl/bio.h>
int main()
{
    BIO *cbio, *out;
    int len;
    char tmpbuf[1024];

    cbio = BIO_new_connect("localhost:55555");
    if(cbio == NULL) {
        fprintf(stderr, "Error create new conncet\n");
    }

    out = BIO_new_fp(stdout, BIO_NOCLOSE);
    if(out == NULL) {
        fprintf(stderr, "Error create new fp\n");
    }

    if (BIO_do_connect(cbio) <= 0) {
        fprintf(stderr, "Error connecting to server\n");
        return -1;
    }

    BIO_puts(cbio, "GET / HTTP/1.0\n\n");
    for (;;) {
        len = BIO_read(cbio, tmpbuf, 1024);
        if (len <= 0)
            break;
        BIO_write(out, tmpbuf, len);
    }
    BIO_free(cbio);
    BIO_free(out);
    return 0;
}
