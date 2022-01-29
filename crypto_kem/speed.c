#include "api.h"
#include "hal.h"
#include "randombytes.h"
#include "sendfn.h"

#include <stdint.h>
#include <string.h>

#include "params.h"
#include "poly.h"

#define MAX(a,b) (((a)>(b))?(a):(b))

#define printcycles(S, U) send_unsignedll((S), (U))

int main(void){

    unsigned char key_a[CRYPTO_BYTES], key_b[CRYPTO_BYTES];
    unsigned char sk[CRYPTO_SECRETKEYBYTES];
    unsigned char pk[CRYPTO_PUBLICKEYBYTES];
    unsigned char ct[CRYPTO_CIPHERTEXTBYTES];

    unsigned long long t0, t1;
    int i;
    int crypto_i;

    hal_setup(CLOCK_BENCHMARK);

    for(i = 0; i < 60; i++){
        hal_send_str("==========================");
    }

    for(crypto_i = 0; crypto_i < CRYPTO_ITERATIONS; crypto_i++){

        // Key-pair generation
        t0 = hal_get_time();
        crypto_kem_keypair(pk, sk);
        t1 = hal_get_time();
        printcycles("keypair cycles:", t1 - t0);

        // Encapsulation
        t0 = hal_get_time();
        crypto_kem_enc(ct, key_a, pk);
        t1 = hal_get_time();
        printcycles("encaps cycles:", t1 - t0);

        // Decapsulation
        t0 = hal_get_time();
        crypto_kem_dec(key_b, ct, sk);
        t1 = hal_get_time();
        printcycles("decaps cycles:", t1 - t0);

        if(memcmp(key_a, key_b, CRYPTO_BYTES)) {
            hal_send_str("ERROR KEYS\n");
        }
        else{
            hal_send_str("OK KEYS\n");
        }

        hal_send_str("#");
    }

    while(1);

    return 0;
}
