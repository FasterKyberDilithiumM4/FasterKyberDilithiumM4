#include "api.h"
#include "hal.h"
#include "randombytes.h"
#include "sendfn.h"

#include <stdint.h>
#include <string.h>

#include "smallntt.h"


#include "params.h"
#include "symmetric.h"
#include "sign.h"
#include "poly.h"
#include "polyvec.h"
#include "ntt.h"
#include "smallpoly.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define printcycles(S, U) send_unsignedll((S), (U))

int main(void)
{

    unsigned long long t0, t1;
    hal_setup(CLOCK_BENCHMARK);
    poly buf;
    int16_t b_prime[N];
#if DILITHIUM_MODE == 3
    int16_t a[N];
    int16_t b[N];
    int16_t c[N];
#else
    int32_t a[N];
    int32_t b[N];
    int32_t c[N];
#endif
    for (int i = 0; i < 60; i++)
    {
        hal_send_str("==========================");
    }

    for (int crypto_i = 0; crypto_i < CRYPTO_ITERATIONS; crypto_i++)
    {

        // ### NTT ###
        t0 = hal_get_time();
        poly_ntt(&buf);
        t1 = hal_get_time();
        printcycles("NTT cycles:", t1 - t0);
        
        // ### iNTT ###
        t0 = hal_get_time();
        poly_invntt_tomont(&buf);
        t1 = hal_get_time();
        printcycles("iNTT cycles:", t1 - t0);
#ifdef opt
        // ### small NTT ###
        t0 = hal_get_time();
        small_ntt(a);
        t1 = hal_get_time();
        printcycles("small NTT cycles:", t1 - t0);

        t0 = hal_get_time();
        small_invntt_tomont(a);
        t1 = hal_get_time();
        printcycles("small iNTT cycles:", t1 - t0);

        t0 = hal_get_time();
        small_point_mul(b_prime, b);
        t1 = hal_get_time();
        printcycles("small point_mul cycles:", t1 - t0);

        t0 = hal_get_time();
        small_asymmetric_mul(c, a, b, b_prime);
        t1 = hal_get_time();
        printcycles("small asymmetric_mul cycles:", t1 - t0);

#endif
        hal_send_str("OK KEYS\n");

        hal_send_str("#");
    }

    while (1)
        ;
    return 0;
}
