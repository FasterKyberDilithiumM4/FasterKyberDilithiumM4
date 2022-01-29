

#include "tools.h"


void cmod_int16(void *des, void *src, void *mod){
    int16_t mod_v = *(int16_t*)mod;
    int16_t t = (*(int16_t*)src) % mod_v;
    if(t >= (mod_v >> 1)){
        t -= mod_v;
    }
    if(t < -(mod_v >> 1)){
        t += mod_v;
    }
    *(int16_t*)des = t;
}

void cmod_int32(void *des, void *src, void *mod){
    int32_t mod_v = *(int32_t*)mod;
    int32_t t = (*(int32_t*)src) % mod_v;
    if(t >= (mod_v >> 1)){
        t -= mod_v;
    }
    if(t < -(mod_v >> 1)){
        t += mod_v;
    }
    *(int32_t*)des = t;
}

void cmod_int64(void *des, void *src, void *mod){
    int64_t mod_v = *(int64_t*)mod;
    int64_t t = (*(int64_t*)src) % mod_v;
    if(t >= (mod_v >> 1)){
        t -= mod_v;
    }
    if(t < -(mod_v >> 1)){
        t += mod_v;
    }
    *(int64_t*)des = t;
}

void mulmod_int16(void *des, void *src1, void *src2, void *mod){

    int32_t src1_v;
    int32_t src2_v;
    int32_t tmp_v;
    int32_t mod_v;
    int32_t des_v;

    src1_v = (int32_t)(*(int16_t*)src1);
    src2_v = (int32_t)(*(int16_t*)src2);
    tmp_v = src1_v * src2_v;
    mod_v = (int32_t)(*(int16_t*)mod);

    cmod_int32(&des_v, &tmp_v, &mod_v);

    *(int16_t*)des = (int16_t)des_v;

}

void addmod_int32(void *des, void *src1, void *src2, void *mod){

    int64_t src1_v;
    int64_t src2_v;
    int64_t tmp_v;
    int64_t mod_v;
    int64_t des_v;

    src1_v = (int64_t)(*(int32_t*)src1);
    src2_v = (int64_t)(*(int32_t*)src2);
    tmp_v = src1_v + src2_v;
    mod_v = (int64_t)(*(int32_t*)mod);

    cmod_int64(&des_v, &tmp_v, &mod_v);

    *(int32_t*)des = (int32_t)des_v;

}

void submod_int32(void *des, void *src1, void *src2, void *mod){

    int64_t src1_v;
    int64_t src2_v;
    int64_t tmp_v;
    int64_t mod_v;
    int64_t des_v;

    src1_v = (int64_t)(*(int32_t*)src1);
    src2_v = (int64_t)(*(int32_t*)src2);
    tmp_v = src1_v - src2_v;
    mod_v = (int64_t)(*(int32_t*)mod);

    cmod_int64(&des_v, &tmp_v, &mod_v);

    *(int32_t*)des = (int32_t)des_v;

}


void mulmod_int32(void *des, void *src1, void *src2, void *mod){

    int64_t src1_v;
    int64_t src2_v;
    int64_t tmp_v;
    int64_t mod_v;
    int64_t des_v;

    src1_v = (int64_t)(*(int32_t*)src1);
    src2_v = (int64_t)(*(int32_t*)src2);
    tmp_v = src1_v * src2_v;
    mod_v = (int64_t)(*(int32_t*)mod);

    cmod_int64(&des_v, &tmp_v, &mod_v);

    *(int32_t*)des = (int32_t)des_v;

}

void expmod_int16(void *des, void *src, size_t e, void *mod){

    int16_t src_v = *(int16_t*)src;
    int16_t tmp_v;

    tmp_v = 1;
    for(; e; e >>= 1){
        if(e & 1){
            mulmod_int16(&tmp_v, &tmp_v, &src_v, mod);
        }
        mulmod_int16(&src_v, &src_v, &src_v, mod);
    }

    memcpy(des, &tmp_v, sizeof(int16_t));

}

void expmod_int32(void *des, void *src, size_t e, void *mod){

    int32_t src_v = *(int32_t*)src;
    int32_t tmp_v;

    tmp_v = 1;
    for(; e; e >>= 1){
        if(e & 1){
            mulmod_int32(&tmp_v, &tmp_v, &src_v, mod);
        }
        mulmod_int32(&src_v, &src_v, &src_v, mod);
    }

    memcpy(des, &tmp_v, sizeof(int32_t));

}

void bitreverse(void *src, size_t len, size_t size){

    char tmp[size];

    for(size_t i = 0, j = 0; i < len; i++){
        if(i < j){
            memcpy(tmp, src + i * size, size);
            memcpy(src + i * size, src + j * size, size);
            memcpy(src + j * size, tmp, size);
        }
        for(size_t k = len >> 1; (j ^= k) < k; k >>= 1);
    }

}















