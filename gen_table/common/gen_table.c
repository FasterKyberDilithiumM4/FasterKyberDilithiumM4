
#include <stdlib.h>
#include <memory.h>

#include "NTT_params.h"

#include "tools.h"
#include "gen_table.h"

void gen_CT_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod)
    ){

    char zeta[size];
    char twiddle[size];

    memcpy(zeta, omega, size);

    memcpy(twiddle, scale, size);
    for(size_t i = 0; i < (NTT_N >> 1); i++){
        memcpy(des, twiddle, size);
        des += size;
        mulmod(twiddle, twiddle, zeta, mod);
    }

    des -= size * (NTT_N >> 1);
    bitreverse(des, NTT_N >> 1, size);

}


void gen_CT_negacyclic_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod)
    ){

    char zeta[size];

    memcpy(zeta, omega, size);

    memcpy(des, scale, size);
    for(size_t i = 1; i < NTT_N; i++){
        mulmod(des + i * size, des + (i - 1) * size, zeta, mod);
    }

    bitreverse(des, NTT_N, size);

    for(size_t i = 1; i < NTT_N; i++){
        memcpy(des + (i - 1) * size, des + i * size, size);
    }

}

void gen_inv_CT_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod),
    void (*expmod) (void *_des, void *_src, size_t _size, void *_mod)
    ){
    // printf("# gen_inv_CT_table_generic\n");
    char zeta[size];
    char twiddle[size];

    for(size_t level = 0; level < LOGNTT_N; level++){
        expmod(zeta, omega, (1 << LOGNTT_N) >> (level + 1), mod);
        memcpy(twiddle, scale, size);
        for(size_t i = 0; i < (1 << level); i++){
            // printf("->%5ld\n", *((int32_t*)twiddle));
            memcpy(des, twiddle, size);
            des += size;
            mulmod(twiddle, twiddle, zeta, mod);
        }
    }

}

void gen_streamlined_CT_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod),
    struct compress_profile *_profile, bool pad
    ){

    char zeta[size];

    size_t start_level;

    char tmp[NTT_N * size];
    void *level_ptr[LOGNTT_N];

    memcpy(zeta, omega, size);

    gen_CT_table_generic(
        tmp, scale, zeta, mod,
        size,
        mulmod
    );

    for(size_t i = 0; i < LOGNTT_N; i++){
        level_ptr[i] = tmp;
    }

    start_level = 0;
    for(size_t i = 0; i < _profile->compressed_layers; i++){
        for(size_t j = 0; j < (1 << start_level); j++){
            if(pad){
                memset(des, 0, size);
                des += size;
            }
            for(size_t k = 0; k < (_profile->merged_layers[i]); k++){
                for(size_t h = 0; h < (1 << k); h++){
                    memcpy(des,
                        level_ptr[start_level + k] + (j * (1 << k) + h) * size,
                        size);
                    des += size;
                }
            }
        }
    start_level += (_profile->merged_layers)[i];
    }

}

void gen_streamlined_CT_negacyclic_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod),
    struct compress_profile *_profile, bool pad
    ){

    char zeta[size];

    size_t start_level;

    char tmp[NTT_N * size];
    void *level_ptr[LOGNTT_N];

    memcpy(zeta, omega, size);

    gen_CT_negacyclic_table_generic(
        tmp, scale, zeta, mod,
        size,
        mulmod
    );

    for(size_t i = 0; i < LOGNTT_N; i++){
        level_ptr[i] = tmp + size * ((1 << i) - 1);
    }

    start_level = 0;
    for(size_t i = 0; i < _profile->compressed_layers; i++){
        for(size_t j = 0; j < (1 << start_level); j++){
            if(pad){
                memset(des, 0, size);
                des += size;
            }
            for(size_t k = 0; k < (_profile->merged_layers[i]); k++){
                for(size_t h = 0; h < (1 << k); h++){
                    memcpy(des,
                        level_ptr[start_level + k] + (j * (1 << k) + h) * size,
                        size);
                    des += size;
                }
            }
        }
    start_level += (_profile->merged_layers)[i];
    }

}


void gen_twist_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod)
    ){
    // printf("# gen_twist_table_generic\n");
    char zeta[size];
    char twiddle[size];

    memcpy(zeta, omega, size);

    memcpy(twiddle, scale, size);
    for(size_t i = 0; i < NTT_N; i++){
        // printf("->%ld\n", *((int32_t*)twiddle));
        memcpy(des, twiddle, size);
        des += size;
        mulmod(twiddle, twiddle, zeta, mod);
    }

}

void gen_streamlined_inv_CT_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod) (void *_des, void *_src1, void *_src2, void *_mod),
    void (*expmod) (void *_des, void *_src, size_t e, void *_mod),
    struct compress_profile *_profile, bool pad
    ){

    char zeta[size];
    size_t start_level;

    char tmp[NTT_N * size];

    void *level_ptr[LOGNTT_N];

    memcpy(zeta, omega, size);

    gen_inv_CT_table_generic(
        tmp, scale, zeta, mod,
        size,
        mulmod,
        expmod
    );

    for(size_t i = 0; i < LOGNTT_N; i++){
        level_ptr[i] = tmp + size * ((1 << i) - 1);
    }

    start_level = 0;
    for(size_t i = 0; i < _profile->compressed_layers; i++){
        for(size_t j = 0; j < (1 << start_level); j++){
            if(pad){
                memset(des, 0, size);
                des += size;
            }
            for(size_t k = 0; k < (_profile->merged_layers[i]); k++){
                for(size_t h = 0; h < (1 << k); h++){
                    memcpy(
                        des,
                        level_ptr[start_level + k] + (j + (h << start_level)) * size,
                        size);
                    des += size;
                }
            }
        }
        start_level += (_profile->merged_layers)[i];
    }

}


void gen_streamlined_inv_CT_negacyclic_table_generic(
    void *des,
    void *scale1, void *omega,
    void *scale2, void *twist_omega,
    void *mod,
    size_t size,
    void (*mulmod) (void *_des, void *_src1, void *_src2, void *_mod),
    void (*expmod) (void *_des, void *_src, size_t e, void *_mod),
    struct compress_profile *_profile, bool pad
    ){

    char zeta[size];
    size_t start_level;

    char tmp[NTT_N * size];
    char tmp2[NTT_N * size];

    void *level_ptr[LOGNTT_N];
    char *twist_ptr;

    memcpy(zeta, omega, size);

    gen_inv_CT_table_generic(
        tmp, scale1, zeta, mod,
        size,
        mulmod,
        expmod
    );

    for(size_t i = 0; i < LOGNTT_N; i++){
        level_ptr[i] = tmp + size * ((1 << i) - 1);
    }

    memcpy(zeta, twist_omega, size);

    gen_twist_table_generic(
        tmp2, scale2, zeta, mod,
        size,
        mulmod
    );

    twist_ptr = tmp2;

    start_level = 0;
    for(size_t i = 0; i < _profile->compressed_layers; i++){
        for(size_t j = 0; j < (1 << start_level); j++){
            if(pad){
                memset(des, 0, size);
                des += size;
            }
            for(size_t k = 0; k < (_profile->merged_layers[i]); k++){
                for(size_t h = 0; h < (1 << k); h++){
                    memcpy(
                        des,
                        level_ptr[start_level + k] + (j + (h << start_level)) * size,
                        size);
                    printf("--> %ld (regular)\n", *((int32_t*)des));
                    des += size;
                }
            }
            if((i + 1) == _profile->compressed_layers){
                for(size_t k = 0; k < (1 << (_profile->merged_layers[i])); k++){
                    memcpy(
                        des,
                        twist_ptr + (j + k * (NTT_N >> (_profile->merged_layers[i]))) * size,
                        size);
                    printf("--> %ld (twist)\n", *((int32_t*)des));
                    des += size;
                }
            }
        }
        start_level += (_profile->merged_layers)[i];
    }

}


void gen_mul_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *mod)
    ){

    char zeta[size];
    char twiddle[size];

    memcpy(zeta, omega, size);

    memcpy(twiddle, scale, size);
    for(size_t i = 0; i < (NTT_N >> 1); i++){
        memcpy(des, twiddle, size);
        des += size;
        mulmod(twiddle, twiddle, zeta, mod);
    }

    des -= (NTT_N >> 1) * size;

    bitreverse(des, NTT_N >> 1, size);

}









