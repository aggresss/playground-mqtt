#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mbedtls/gcm.h"
#include "mbedtls/sha1.h"
#include "b64/urlsafe_b64.h"

#define SHA1_LEN 20
#define NONCE_LEN 12
#define GCM_TAG_LEN 16

const char input[] =
        "u0n_QFGNHxNp2AqnIuIS5YG4vZqQmuP3smzLI1tFXlNwdQoHYLCatlIq-UAIn0IspT6vdhv857Ey-A0X8ScQQElMSM34xZ9S2gsYN5qUXgHTnZPK1orsh15gFzMwxd0L0HsqXa0WT5CewR7FZ4ax19DDwe8E4NozbnO52p1GKlMEIsTZEWsi71n_avAFUTMKjUngrmlg7BXVzOP9jgCZb6meAERxmdDELGlMOOtFrLlueSbDRSQVitn7BzO5X_Zl2pCurb-yBBTF94Ebz10_IsL1Nmcd3ryyvfuXdWLLgi-uNhi47beyIlZQHsSjMb-7llnyucUqO7jPG2nNm4aGX7q_7kK6ayga5hGsXa3kq3swUDrvgu5YE94bMZgDW4xGGK9ykDaQuif6jQEq5PK8oJofNKkRjTd5VnfA4fq4szLfjkgJm1AjXDndlG4nrvhHEwxtjCG0ZjzkxNyDB0CCbxquZqGDpjzwbgYKX5fDnlgSla3wrawLkf6l1zhEMhvOx5TJT8Ooy79DGFI_Syq0H1qcQN6ZEjdxypKOmif6pfHrYRh8bL9_WSG6OpONMBwP4tIhzKoldQgqMSZoLgXZ0BwRZvsDl1VflZAOs3Rh58NP6Wu5OmRcVh2CxvS2v_zS4iMy760AwUv-9PO-J4DDks6EyaBLaAubBz12oCdl2X2BEHunYkelST5k5Ay1z5Nf0SMYreXuSf_JrtgUhg-_hk2RGK5aPps1vmJHoPGRNCy9iVpYEOOFHvCP1fURidKbMUHyZLyVTaCK19yO1bPDebWWaVYCUgVDF7QFi88Wd-RA0klIty3GWOXExV0uRkswxJEqa5-d3-L_J9L-qjeAnolOMN1EupMBqa96S-8xKl73aFDzqi_9PUob-30NwxMmFa6LdUDC7GEGXxdIvTNrm5OVsiwuVlsPMaYDcbfXnjAVefJEj0jcpdo5qr1UZobBX0biodfe8M0TuI0KPIkJPcN0WhEa-EOBbtoV8GP7MhaEFknKo6iWz9lZcCE5RS06WoCgvC8LEYkk3LXXaXRlWagvYJE-uB_kN4tAhsep838SG8tbuAmkaz9lRSORkpZF8PcFZ8LLd8rek5VJ1u-i0ihaB2Dh69KzMKpwbcaWFotF9x3fFC2d2A-taBd8h6pwxzVk5hEM_M4ub3HL8X-BK7DQ5FIkrH4IgsCrBt_CE-AzjhFOIKydhKAeDQbud-S_lcun-cLuRfbBfJAvxd5JS1k4e2jS707cbOncc_hlcdcqMdSekwE5-LFkPRhRXoLGMxnXZmzRf3ZFS2Nw0zfPXCM8S1anJOLn";
const char dsk[] = "muX5HAtI02ghIiYDPh468RvJgeiMwbhn8BHyCt5O";

int main()
{
    size_t intput_len = strlen(input);
    unsigned char un_b64[4096] = { 0 };
    size_t un_b64_len = 4096;
    unsigned char dsk_sha1[SHA1_LEN] = { 0 };
    unsigned char nonce[NONCE_LEN] = { 0 };
    mbedtls_sha1_context sha1_ctx;
    mbedtls_gcm_context gcm_ctx;
    unsigned char gcm_tag[GCM_TAG_LEN] = { 0 };
    unsigned char output[4096] = { 0 };

    /* Step 1: Get md5sum of dsk */
    mbedtls_sha1_init(&sha1_ctx);
    mbedtls_sha1_starts(&sha1_ctx);
    mbedtls_sha1_update_ret(&sha1_ctx, (const unsigned char*) dsk, strlen(dsk));
    mbedtls_sha1_finish(&sha1_ctx, dsk_sha1);

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
            (const unsigned char*) dsk_sha1,
            128);
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
