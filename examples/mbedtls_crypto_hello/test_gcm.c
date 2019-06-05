#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mbedtls/gcm.h"
#include "mbedtls/md5.h"
#include "b64/urlsafe_b64.h"

#define MD5_LEN 16
#define NONCE_LEN 12

const char input[] =
        "zo4tqMcnsFaFbljkmz19R5JiwlOYsp2iwQe6pa2PTIOFnyPxCZccDqHD6uulB4s8usBHBFkqrnyB18g6_D3n4djB1cb7Epk4qeu07FIf8ZCCUEdjjc9d_Tu-owMOe5AvDbYL9iUqltLDpxMXrVFrXk2xOPHwe4KQ6GTtA6eoF_Lbzp-ZUjyMSaEZY7Qc-4XxGm8VAU-xFYMTf3b1fjdLj6CrjAWHVaQsnas5FG-YuweEaeVSpJypa15C6-4SLrA4-RzV4IXvAGIrsytPbLZnwJkwC35J6doHeM9nYy6CnrDxIHcP71EUXkDa3i9KrGZUR55Y_nzQ012wUM3FK1YGO3fpBcSGkbM2HXHmGmJrdXUlLq44SzkXZWAOdIkRH6_TtpAWCZCRhlo20hT0zpxfAS6cstoZg5-ES4nYiMLyMKXpC-toZiDysBxjSL9oN6DCvafiz6lahKaLMdvEWv_nQYQWxC3Xs1AB2QiKaIJyypQWrewAl6vEK4ffvdJr6orNft2ttLKTtU-BdTqERqmuHLqg2orFIcb8qONyjkbhVd_ctcHthiouAWSeXyBF5e2dnLvy3AlgPeVwbel2gCDFZTgI6EEwiknAjKL3kHN-6hB2WWUjrnUsCAol-Uan-4fH0p_g_tru-8N0quDk7BMmSKdzQBWDMWvkHruP8cxkQ0djIRsxm-YNahWl5OMb7vGZ6kFrNlBGSQeKvtYiBbpI9fMxambutOCWtY0a7f2zO7c5GkCHbkly32ZkcDsGO2l94nHCieYuaPJmDXQ7MRNMJoUPYuGhRbTM24kbqEbfC4dK0bP0sGCfNxMK1uPFNOT2DHVYc94c4pgoRB-M7nbyzGLjKWiPJprPidzX84N0bxtfTJBfH-qqC266XCu4ZBEMz4O2_2T2R9AHpMauAxoJ0VjGt_pm5ePcp6xYGnC8iaODEiOLNMcBzKobMO4CMoRfQN_CCa9PApThtyJOXKamW9G59ghcN2I9Blu4WIripVyMTuEkKAAWJcKVTSX6UAN685zmPFsqjXsxQ7x301UeWCw9i4B3bjaGUjWWuIR6pOkla0Fnv5ObwVvQa_13kp057pnUui3caydOafLS_1NWGxmDkCKKi3R2gh4G5YsuN4cRaMx1ZoX9N1_hUzmdtq08WWh2Y5PcPstCxZoaeD-g-U5AfzbPKfcjnzLprmJF4Y42pbmGbpeBniqzn8X2VVk0tG273ckjo4LuOiOemYAX0AIIQw1LWcGi5Bvo_X-COEvwFDHrDoe3ZPB5wSk5GpmuzcxugBs9Ok6U15X1v241xKZck_BLiuOLlaZL5F1VyA==";

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
    unsigned char output[4096] = { 0 };

    /* Step 1: Get md5sum of dsk */
    mbedtls_md5_init(&md5_ctx);
    mbedtls_md5_starts(&md5_ctx);
    mbedtls_md5_update(&md5_ctx, dsk, strlen(dsk));
    mbedtls_md5_finish(&md5_ctx, dsk_md5);

    /* Step 2: b64 decode */
    un_b64_len = urlsafe_b64_decode(input, intput_len, un_b64, un_b64_len);

    /* Step 3: Get nonce from first 12 bytes of b64 decode */
    memcpy(nonce, un_b64, NONCE_LEN);

    /* Step 4: AES-GCM decrypto */

    mbedtls_gcm_init(&gcm_ctx);
    mbedtls_gcm_setkey(&gcm_ctx, MBEDTLS_CIPHER_ID_AES, (const unsigned char*)dsk_md5,
            MD5_LEN * 8);
    mbedtls_gcm_starts(&gcm_ctx, MBEDTLS_GCM_DECRYPT, (const unsigned char*)nonce,
            NONCE_LEN, NULL, 0);
    mbedtls_gcm_update(&gcm_ctx, un_b64_len, (const unsigned char*)un_b64, output);
    mbedtls_gcm_free(&gcm_ctx);

    /* output */
    printf("Output:\n%s\n", output);

}
