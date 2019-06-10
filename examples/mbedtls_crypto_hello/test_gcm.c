#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mbedtls/gcm.h"
#include "mbedtls/md5.h"
#include "b64/urlsafe_b64.h"

#define MD5_LEN 16
#define NONCE_LEN 12
#define GCM_TAG_LEN 16

const char input[] =
        "KEtxNMjaVARzb1GDqdwD-6_xXAnjrMDKLSFFiWLTXccN8LT3waCcNqYUie4iRAlXFYA5UL5tb4TqE-CP-HD7z2qg4i4Xg7dhAjeS_Iu7-nTALTui8bdZZum-BpJsb98F_hkgny2qJSB3zDpyCWAFXe-oB4zFPhHmgpgNGCAe9X-szlD2qefXo_DMO5iOBXzCUbeTKhF9TIeygE0BapY_I2FYoMYwRLyyoBRY7Bf5PfkKoM6WdGWmOrkK1zoz7rF504NQvprXtKqtMy-BtjhZ7RcDEcu2J6INQ00zWtf6QBFNec6gI_LVRsKiZuLIow7rvsdAANJMTx37pKLeamaSacUWA2-5rDOHuNNF_O52KJwkYKnOs-V0ZlJPKvgeKlmXPs66Ab19Lm4uivDYBh98hrgLn8Gu1X5K57P8MM8VP773yinIYRBhgcjmDK6MKhQ32p5i2uq3hSX__CdLTZhfisi4GZRZr1E309oWFWOvrNKGnncFN4_TSGEi0dtFsI0QfElHuWRhsigznI_FAuENgOsqi_DIKhwLwCkrhNtViK5I-U9ia2w_ypG7d455P_zGONRCtQjLqvEAYcVFRFIUXX29n6K6zW4Uip-CHssmSufWXo-b6fVKoWcASr_5pgvsLWXMva3fu8xCvwmsXg7psBBYQ9uemGOlCucNEpTkATO8DSF06oP7IDuqzN6zNWLOPLCxzvT0x7k3RhqClfC73jQxNp8Ha1Lp1o_SV330Gyyjsop8DVlXQasfPgKTayhhWZ2lj2NmDQinj9xrYNB5jVNW1prERD_lHdyS90lg19k0RJ5nHXaJc_kNA7B8M9U1Krvxekcwx7pq19tS5h_pB11arADfROEHBbHgMeURfZY1MMcV1oI-qr-mJS-auTRB3yBwqOXi-NA09SPhLciQqq8TRClKhVbu558EMdm7AWcSUhS01L983FRQvG9hK6dBkEyh4hPTTPwL0wTh0wM73BRxSv_GWXwhXNsFNsOSCIK9KE1uN-C9_5OlM7zVDk5idaKiNkutnyYLJIiswqJrT-8IbEA_cv8NppEdx1MgaNHn2W_eAc-zVCcEB59b52sbw7xo2aOiatZqz0ykJA6nWHSF1Xcoy4lgLWgBdAyApXF89Y-MGs6M6DmRE56Pu-oCI7dBMQ7Z-Rk9Oo_LpWokx52MIDaaeMOV5PVYjQEw8xkge7-OcDK_rvEs_gGAnLNPf4sCMzoWes7cuZ_G28ZkxuordU7iqiKHpCp0bgltM6prNvXBmIFAcgI3G0BN7n0aV01hEERkjTD50CdBOneA5Ol-peyGDSY=";
const char dsk[] = "uWxCtA04ZKI0IFn10DGlcdNixAHELUqqiQ60kzfW";

int main()
{
    size_t intput_len = strlen(input);
    unsigned char un_b64[4096] = { 0 };
    size_t un_b64_len = 4096;
    unsigned char dsk_md5[MD5_LEN] = { 0 };
    unsigned char nonce[NONCE_LEN] = { 0 };
    mbedtls_md5_context md5_ctx;
    mbedtls_gcm_context gcm_ctx;
    unsigned char gcm_tag[GCM_TAG_LEN] = { 0 };
    unsigned char output[4096] = { 0 };

    /* Step 1: Get md5sum of dsk */
    mbedtls_md5_init(&md5_ctx);
    mbedtls_md5_starts(&md5_ctx);
    mbedtls_md5_update_ret(&md5_ctx, (const unsigned char*) dsk, strlen(dsk));
    mbedtls_md5_finish(&md5_ctx, dsk_md5);

    /* Step 2: b64 decode */
    un_b64_len = urlsafe_b64_decode(input, intput_len, un_b64, un_b64_len);

    printf("URL safe b64 decode Length: %zu\n", un_b64_len);
    int i;
    for (i = 0; i < un_b64_len; i++) {
        printf("%d ", *(const unsigned char*) (un_b64 + i));
    }
    printf("\n");

    /* Step 3: Get nonce from first 12 bytes of b64 decode */
    memcpy(nonce, un_b64, NONCE_LEN);

    /* Step 4: AES-GCM decrypto */
    mbedtls_gcm_init(&gcm_ctx);
    mbedtls_gcm_setkey(&gcm_ctx, MBEDTLS_CIPHER_ID_AES,
            (const unsigned char*) dsk_md5,
            MD5_LEN * 8);
    mbedtls_gcm_starts(&gcm_ctx, MBEDTLS_GCM_DECRYPT,
            (const unsigned char*) nonce,
            NONCE_LEN, NULL, 0);
    mbedtls_gcm_update(&gcm_ctx, (un_b64_len - NONCE_LEN - GCM_TAG_LEN),
            (const unsigned char*) (un_b64 + NONCE_LEN), output);

    mbedtls_gcm_finish(&gcm_ctx, gcm_tag, GCM_TAG_LEN);
    mbedtls_gcm_free(&gcm_ctx);

    if (memcmp((const unsigned char*) (un_b64 + un_b64_len - GCM_TAG_LEN),
            gcm_tag, GCM_TAG_LEN) == 0) {
        printf("Verify Success.\n");
    } else {
        printf("Verify Faild.\n");
    }

    /* output */
    printf("Output:\n%s\n", output);

}
