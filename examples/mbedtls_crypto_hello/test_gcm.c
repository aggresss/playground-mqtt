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
            "s76k9mKx_SKW-9JR_A7SoUp0AYobAcbVWBCZXNBpsgYgpc9OD_7f8WRyohz1iwyN9EFUxRMips_9ntVLhBfFm9iLRWasR61A8EiyJQr8GLrwqiYAMsDqn05Tl_ELZJLzetii76PNmMEur7hHB9TtVGS9-unSC2RFmuhpJ2pS5Lo9wD8TYrglnSpxurad28Wu_OV42WVWb3dsL8dkHd6ziIQl6AfA3YWT5ycZ_rw2YypdRhNaAyd3LENX2hpJ7UFCfkAE-LmQsj_2JQyxP3tvXbIMjnNiqoV32rB-GKEGfdftQSs6N3kZ2LTezx1pC4-lNZPmzCECWSAwxUiL3PYRCKKR_o3qa88tYzybDvnpDGbW-cgQ8fkfvCX1hW2Kg_OWTO9-Y9ud5-dkJF1_4qU9orxzfuD5CxUAafbqAtDuKkjvDY9xQHQl2euHdl7JwLVvEgAlq6mIYPjPEqfptFgOYl95hhWHz7Ok5IgHKAhc91a8PX-ycM0RiF49A7fd5biy_7PFki3Q8OL4jbBpAqF3X922L-tBqJ5ZmKPadphzwSRp7rBaSti5RE83JTZAsDFJXIOlY5_FWNS2jEP5g1fAmIfzRPDk5Rlid10FLins2myl5k4CDUuJDGClqC7EjO_bslxOQbIvEOYfU7L1KDpO8Z928ohQuAc4ZHwTiCf8duFyiDJb0dEtusF5tel-5e2qTlRccWdzXPit88kIrWlPbWigNhJQV0aN8E4ayit_yuKSTvZwxTt78coowvp2aR-IMeMOLGURjchTflEBadsnYKcFeFGXMVpuMxV-SrBurrE1T59Cr15daaT1_wYbrDt-JA7nsmCOTLHLVMafIzWOkft7v55FNfMoDZKR-5IWCNQKd7JTf-Y2B7jjjpvcnsKUyvhE7-LGDb314GcHjN3cfOYP2fZTzq0dqweVruSYRUbq5mkPtACSzr9e1kThUdd3qGC5Ew5Eeidvi4de8-WG8u_flh7h24c4_rt4aJsXDnVRUD1ZJ-tkjHxjMipNqEgT3tI_-_CxqaJvUoTR98ZY_P966ySX6tf9_MJVDcJsChs-c8oDGo-jEWZAvvuX1hCnqjJG5YDQCzmVGdG7CWQilaRu0_1ftaPf6etbN-X5jnfBu7iHKGwTeGUfIp71RgSjWoH8G2eC2n1umPXVa-HUnFoya2kkyVJ9gFkoPktxizK3-t1_kdz4KpSPMp-YDdL13vQZcFJOFpcvHeGkAaaaLqrWppxeU-0QmbGcNEoPtY5umZf1yzFsJm63_cvQosLRyQST2eykXxKE93-7r5_v9XKsi9JtiNs=";
        const char dsk[] = "uWxCtA04ZKI0IFn10DGlcdNixAHELUqqiQ60kzfW";

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
