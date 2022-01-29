#include "api.h"
#include "hal.h"
#include "randombytes.h"
#include "sendfn.h"

#include <stdint.h>
#include <string.h>

#include "params.h"
#include "symmetric.h"
#include "indcpa.h"
#include "poly.h"
#include "polyvec.h"
#include "ntt.h"
#include "implvariant.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

extern void doublebasemul_asm(int16_t *r, const int16_t *a, const int16_t *b, int16_t zeta);

#ifdef opt
#include "matacc.h"
extern void basemul_asm_opt(int16_t *, const int16_t *, const int16_t *, const int16_t *);
extern void basemul_asm_acc_opt(int16_t *, const int16_t *, const int16_t *, const int16_t *);
extern void doublebasemul_asm_cache_16_32_wrapper(int16_t *);
extern void doublebasemul_asm_acc_cache_32_32_wrapper(int16_t *);
extern void doublebasemul_asm_acc_cache_32_16_wrapper(int16_t *);
extern void doublebasemul_asm_opt_16_32_wrapper(int16_t *);
extern void doublebasemul_asm_acc_opt_32_32_wrapper(int16_t *);
extern void doublebasemul_asm_acc_opt_32_16_wrapper(int16_t *);

#elif defined(optstack)
#include "matacc.h"
extern void doublebasemul_asm_wrapper(int16_t *);
extern void doublebasemul_asm_acc_wrapper(int16_t *);
#else
extern void doublebasemul_asm_acc(int16_t *r, const int16_t *a, const int16_t *b, int16_t zeta);
#endif

#define printcycles(S, U) send_unsignedll((S), (U))
static inline __attribute__((always_inline)) void absorb_squeeze(int transposed, xof_state *state, unsigned char buf[XOF_BLOCKBYTES + 2], const unsigned char *seed, int i, int j)
{
#ifndef nohash
  if (transposed)
    xof_absorb(state, seed, i, j);
  else
    xof_absorb(state, seed, j, i);

  xof_squeezeblocks(buf, 1, state);
#endif
}
int main(void)
{

  hal_setup(CLOCK_BENCHMARK);
  int32_t dummy32_a[KYBER_N], dummy32_b[KYBER_N], dummy32_c[KYBER_N];
  int16_t dummy16_a[KYBER_N], dummy16_b[KYBER_N], dummy16_c[KYBER_N], dummy16_d[KYBER_N];
  poly dummypoly_a, dummypoly_b, dummypoly_c, dummypoly_d;
  int16_t c[4];
  unsigned long long t0, t1;
  int i, j;
  int crypto_i;
  unsigned char buf[2 * KYBER_SYMBYTES];
  randombytes(buf, 2 * KYBER_SYMBYTES);
  unsigned char buf_matacc[XOF_BLOCKBYTES + 2];
  xof_state state;

  for (i = 0; i < 60; i++)
  {
    hal_send_str("==========================");
  }

  for (crypto_i = 0; crypto_i < CRYPTO_ITERATIONS; crypto_i++)
  {
    // make sure buffers contain random data so benchmarks will average
    randombytes(buf, 2 * KYBER_SYMBYTES);
    xof_absorb(&state, buf, i, j);
    xof_squeezeblocks(buf_matacc, 1, &state);
    
    // ### matrix-vector product ###
    t0 = hal_get_time();
    // ### matrix-vector product (matacc style) ###
#ifdef opt
    j = 0;
    absorb_squeeze(0, &state, buf_matacc, buf, 0, j);
    matacc_asm_cache_16_32(dummy32_a, dummy16_a, c, buf_matacc, zetas, &state, dummy16_b);
    for (j = 1; j < KYBER_K - 1; j++)
    {
      absorb_squeeze(0, &state, buf_matacc, buf, 0, j);
      matacc_asm_cache_32_32(dummy32_a, dummy16_a, c, buf_matacc, zetas, &state, dummy16_b);
    }
    absorb_squeeze(0, &state, buf_matacc, buf, 0, j);
    matacc_asm_cache_32_16(dummy16_c, dummy16_a, c, buf_matacc, zetas, &state, dummy16_b, dummy32_a);

    poly_invntt(&dummypoly_a);
    poly_ntt(&dummypoly_a);

    for (i = 1; i < KYBER_K; i++)
    {
      absorb_squeeze(0, &state, buf_matacc, buf, i, j);
      matacc_asm_opt_16_32(dummy32_a, dummy16_a, c, buf_matacc, &state, dummy16_b);
      for (j = 1; j < KYBER_K - 1; j++)
      {
        absorb_squeeze(0, &state, buf_matacc, buf, i, j);
        matacc_asm_opt_32_32(dummy32_a, dummy16_a, c, buf_matacc, &state, dummy16_b);
      }
      absorb_squeeze(0, &state, buf_matacc, buf, i, j);
      matacc_asm_opt_32_16(dummy16_c, dummy16_a, c, buf_matacc, &state, dummy16_b, dummy32_a);

      poly_invntt(&dummypoly_a);
      poly_ntt(&dummypoly_a);
    }
#elif defined(optstack)
    j = 0;
    for (i = 0; i < KYBER_K; i++)
    {
      absorb_squeeze(0, &state, buf_matacc, buf, i, j);
      matacc_asm(dummy16_a, dummy16_b, c, buf_matacc, zetas, &state);
      for (j = 1; j < KYBER_K; j++)
      {
        absorb_squeeze(0, &state, buf_matacc, buf, i, j);
        matacc_asm_acc(dummy16_a, dummy16_b, c, buf_matacc, zetas, &state);
      }
      poly_invntt(&dummypoly_a);

      poly_ntt(&dummypoly_a);
    }

#else
    // resemble original matacc code
    unsigned int buflen, off;
    unsigned int ctr, pos, k, l;
    uint16_t val0, val1;
    for (i = 0; i < KYBER_K; i++)
    {
      poly_zeroize(&dummypoly_a);

      for (j = 0; j < KYBER_K; j++)
      {
        ctr = pos = 0;
        absorb_squeeze(0, &state, buf_matacc, buf, i, j);
        buflen = XOF_BLOCKBYTES;

        k = 0;
        while (ctr < KYBER_N / 4)
        {
          val0 = ((buf_matacc[pos + 0] >> 0) | ((uint16_t)buf_matacc[pos + 1] << 8)) & 0xFFF;
          val1 = ((buf_matacc[pos + 1] >> 4) | ((uint16_t)buf_matacc[pos + 2] << 4)) & 0xFFF;
          pos += 3;

          if (val0 < KYBER_Q)
          {
            c[k++] = (int16_t)val0;
            if (k == 4)
            {
              doublebasemul_asm_acc(dummy16_a, dummy16_b, c, zetas[ctr]);
              ctr++;
              k = 0;
            }
          }

          if (val1 < KYBER_Q && ctr < KYBER_Q / 4)
          {
            c[k++] = (int16_t)val1;
            if (k == 4)
            {
              doublebasemul_asm_acc(dummy16_a, dummy16_b, c, zetas[ctr]);
              ctr++;
              k = 0;
            }
          }

          if (pos + 3 > buflen && ctr < KYBER_Q / 4)
          {
            off = buflen % 3;
            for (l = 0; l < off; l++)
              buf_matacc[l] = buf_matacc[buflen - off + l];
#ifndef nohash
            xof_squeezeblocks(buf + off, 1, &state);
#endif
            buflen = off + XOF_BLOCKBYTES;
            pos = 0;
          }
        }
      }
      poly_invntt(&dummypoly_a);

      poly_ntt(&dummypoly_a);
    }
#endif
    t1 = hal_get_time();
#ifdef nohash
    printcycles("MatrixVectorMul cycles (matacc,nohash):", t1 - t0);
#else
    printcycles("MatrixVectorMul cycles (matacc,hash):", t1 - t0);
#endif
    // ### inner prod (enc) ###
    t0 = hal_get_time();
#ifdef opt
    i = 0;
    poly_basemul_opt_16_32(dummy32_a, &dummypoly_a, &dummypoly_b, &dummypoly_c);
    for (i = 1; i < KYBER_K - 1; i++)
    {
      poly_basemul_acc_opt_32_32(dummy32_a, &dummypoly_b, &dummypoly_c, &dummypoly_d);
    }
    poly_basemul_acc_opt_32_16(&dummypoly_a, &dummypoly_b, &dummypoly_c, &dummypoly_d, dummy32_a);
    poly_invntt(&dummypoly_a);
#else
    i = 0;
    poly_basemul(&dummypoly_a, &dummypoly_b, &dummypoly_c);
    for (i = 1; i < KYBER_K; i++)
    {
      poly_basemul_acc(&dummypoly_a, &dummypoly_b, &dummypoly_c);
    }
    poly_invntt(&dummypoly_a);
#endif
    t1 = hal_get_time();
    printcycles("Inner prod cycles (enc):", t1 - t0);

    // ### inner prod (dec) ###
    t0 = hal_get_time();
#ifdef opt
    poly_ntt(&dummypoly_a);
    poly_frombytes_mul_16_32(dummy32_a, dummy16_a, dummy16_b);
    for (j = 1; j < KYBER_K - 1; j++)
    {
      poly_ntt(&dummypoly_a);
      poly_frombytes_mul_32_32(dummy32_a, dummy16_a, dummy16_b);
    }
    poly_ntt(&dummypoly_a);
    poly_frombytes_mul_32_16(dummy16_a, dummy16_b, dummy16_c, dummy32_a);

    poly_invntt(&dummypoly_a);
#elif defined(optstack)
    poly_ntt(&dummypoly_a);
    
    poly_frombytes_mul(dummy16_a, dummy16_a, dummy16_b);
    for(i = 1; i < KYBER_K; i++) {
        poly_ntt(&dummypoly_a);
        poly_frombytes_mul_acc(&dummypoly_a, &dummypoly_b, dummy16_b);
    }

    poly_invntt(&dummypoly_a);
#else
    poly_ntt(&dummypoly_a);
    poly_frombytes_mul(dummy16_a, dummy16_b);
    for (j = 1; j < KYBER_K; j++)
    {
      poly_ntt(&dummypoly_a);
      poly_frombytes_mul(dummy16_a, dummy16_b);
      poly_add(&dummypoly_a, &dummypoly_b, &dummypoly_c);
    }

    poly_invntt(&dummypoly_a);
#endif
    t1 = hal_get_time();
    printcycles("Inner prod cycles (dec):", t1 - t0);

    // ### NTT ###
    t0 = hal_get_time();
    poly_ntt(&dummypoly_a);
    t1 = hal_get_time();
    printcycles("NTT cycles:", t1 - t0);

    // ### iNTT ###
    t0 = hal_get_time();
    poly_invntt(&dummypoly_a);
    t1 = hal_get_time();
    printcycles("iNTT cycles:", t1 - t0);

    // ### basemul ###
#ifdef opt
    // basemul (on full poly)
    t0 = hal_get_time();
    poly_basemul_opt_16_32(dummy32_a, &dummypoly_a, &dummypoly_b, &dummypoly_c);
    t1 = hal_get_time();
    printcycles("poly_basemul_opt_16_32 cycles:", t1 - t0);

    t0 = hal_get_time();
    poly_basemul_acc_opt_32_32(dummy32_a, &dummypoly_b, &dummypoly_c, &dummypoly_d);
    t1 = hal_get_time();
    printcycles("poly_basemul_acc_opt_32_32 cycles:", t1 - t0);

    t0 = hal_get_time();
    poly_basemul_acc_opt_32_16(&dummypoly_a, &dummypoly_b, &dummypoly_c, &dummypoly_d, dummy32_a);
    t1 = hal_get_time();
    printcycles("poly_basemul_acc_opt_32_16 cycles:", t1 - t0);

    // double basemul (on 4 coeffs)
    t0 = hal_get_time();
    doublebasemul_asm_cache_16_32_wrapper(dummy16_a);
    t1 = hal_get_time();
    printcycles("doublebasemul_asm_cache_16_32_wrapper cycles:", t1 - t0);

    t0 = hal_get_time();
    doublebasemul_asm_acc_cache_32_32_wrapper(dummy16_a);
    t1 = hal_get_time();
    printcycles("doublebasemul_asm_acc_cache_32_32_wrapper cycles:", t1 - t0);

    t0 = hal_get_time();
    doublebasemul_asm_acc_cache_32_16_wrapper(dummy16_a);
    t1 = hal_get_time();
    printcycles("doublebasemul_asm_acc_cache_32_16_wrapper cycles:", t1 - t0);

    t0 = hal_get_time();
    doublebasemul_asm_opt_16_32_wrapper(dummy16_a);
    t1 = hal_get_time();
    printcycles("doublebasemul_asm_opt_16_32_wrapper cycles:", t1 - t0);

    t0 = hal_get_time();
    doublebasemul_asm_acc_opt_32_32_wrapper(dummy16_a);
    t1 = hal_get_time();
    printcycles("doublebasemul_asm_acc_opt_32_32_wrapper cycles:", t1 - t0);

    t0 = hal_get_time();
    doublebasemul_asm_acc_opt_32_16_wrapper(dummy16_a);
    t1 = hal_get_time();
    printcycles("doublebasemul_asm_acc_opt_32_16_wrapper cycles:", t1 - t0);
#elif defined(optstack)
    // basemul (on full poly)
    t0 = hal_get_time();
    poly_basemul(&dummypoly_a, &dummypoly_b, &dummypoly_c);
    t1 = hal_get_time();
    printcycles("poly_basemul cycles:", t1 - t0);

    t0 = hal_get_time();
    poly_basemul_acc(&dummypoly_a, &dummypoly_b, &dummypoly_c);
    t1 = hal_get_time();
    printcycles("poly_basemul_acc cycles:", t1 - t0);
    t0 = hal_get_time();
    doublebasemul_asm_wrapper(dummy16_a);
    t1 = hal_get_time();
    printcycles("doublebasemul_asm_wrapper cycles:", t1 - t0);

    t0 = hal_get_time();
    doublebasemul_asm_acc_wrapper(dummy16_a);
    t1 = hal_get_time();
    printcycles("doublebasemul_asm_acc_wrapper cycles:", t1 - t0);
#else
    // basemul (on full poly)
    t0 = hal_get_time();
    poly_basemul(&dummypoly_a, &dummypoly_b, &dummypoly_c);
    t1 = hal_get_time();
    printcycles("poly_basemul cycles:", t1 - t0);

    t0 = hal_get_time();
    poly_basemul_acc(&dummypoly_a, &dummypoly_b, &dummypoly_c);
    t1 = hal_get_time();
    printcycles("poly_basemul_acc cycles:", t1 - t0);

    // doublebasemul
    t0 = hal_get_time();
    doublebasemul_asm(dummy16_a, dummy16_b, dummy16_c, zetas[0]);
    t1 = hal_get_time();
    printcycles("doublebasemul_asm cycles:", t1 - t0);

    t0 = hal_get_time();
    doublebasemul_asm_acc(dummy16_a, dummy16_b, dummy16_c, zetas[0]);
    t1 = hal_get_time();
    printcycles("doublebasemul_asm_acc cycles:", t1 - t0);

#endif

    hal_send_str("OK KEYS\n");

    hal_send_str("#");
  }

  while (1)
    ;
  return 0;
}
