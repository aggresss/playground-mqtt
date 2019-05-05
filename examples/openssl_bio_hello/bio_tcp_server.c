#include <stdio.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/err.h>

int main()
{
    BIO *abio, *cbio, *cbio2;
    ERR_load_BIO_strings();
    abio = BIO_new_accept("55555");
    /* First call to BIO_accept() sets up accept BIO */
    if (BIO_do_accept(abio) <= 0) {
        fprintf(stderr, "Error setting up accept\n");
        ERR_print_errors_fp(stderr);
        exit(0);
    }
    /* Wait for incoming connection */
    if (BIO_do_accept(abio) <= 0) {
        fprintf(stderr, "Error accepting connection\n");
        ERR_print_errors_fp(stderr);
        exit(0);
    }
    fprintf(stderr, "Connection 1 established\n");
    /* Retrieve BIO for connection */
    cbio = BIO_pop(abio);
    BIO_puts(cbio, "Connection 1: Sending out Data on initial connection\n");
    fprintf(stderr, "Sent out data on connection 1\n");
    /* Wait for another connection */
    if (BIO_do_accept(abio) <= 0) {
        fprintf(stderr, "Error accepting connection\n");
        ERR_print_errors_fp(stderr);
        exit(0);
    }
    fprintf(stderr, "Connection 2 established\n");
    /* Close accept BIO to refuse further connections */
    cbio2 = BIO_pop(abio);
    BIO_free(abio);
    BIO_puts(cbio2, "Connection 2: Sending out Data on second\n");
    fprintf(stderr, "Sent out data on connection 2\n");
    BIO_puts(cbio, "Connection 1: Second connection established\n");
    /* Close the two established connections */
    BIO_free(cbio);
    BIO_free(cbio2);
}
