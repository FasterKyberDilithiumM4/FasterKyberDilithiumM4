#ifndef NAIVE_MULT
#define NAIVE_MULT

#include <stddef.h>

void naive_mulR(
    void *des,
    void *src1, void *src2,
    size_t len, void *twiddle,
    void *mod,
    size_t size,
    void (*addmod)(void *_des, void *_src1, void *_src2, void *_mod),
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod)
    );

void point_mul(
    void *des,
    void *src1, void *src2,
    size_t len, size_t jump,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod)
    );

#endif

