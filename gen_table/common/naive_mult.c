
#include <stdlib.h>
#include <memory.h>

#include "tools.h"
#include "naive_mult.h"

void naive_mulR(
    void *des,
    void *src1, void *src2,
    size_t len, void *twiddle,
    void *mod,
    size_t size,
    void (*addmod)(void *_des, void *_src1, void *_src2, void *_mod),
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod)
    ){

    char buff[(len << 1) * size];
    char tmp[size];

    memset(buff, 0, (len << 1) * size);

    for(size_t i = 0; i < len; i++){
        for(size_t j = 0; j < len; j++){
            mulmod(tmp, src1 + i * size, src2 + j * size, mod);
            addmod(buff + (i + j) * size, buff + (i + j) * size, tmp, mod);
        }
    }

    for(size_t i = ((len - 1) << 1); i >= len; i--){
        mulmod(tmp, buff + i * size, twiddle, mod);
        addmod(des + (i - len) * size, buff + (i - len) * size, tmp, mod);
    }
    memcpy(des + (len - 1) * size, buff + (len - 1) * size, size);

}


void point_mul(
    void *des,
    void *src1, void *src2,
    size_t len, size_t jump,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod)
    ){

    for(size_t i = 0; i < len; i++){
        for(size_t j = 0; j < jump; j++){
            mulmod(des + (i * jump + j) * size, src1 + (i * jump + j) * size, src2 + i * size, mod);
        }
    }

}







