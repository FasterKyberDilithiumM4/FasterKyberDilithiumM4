#ifndef NTT_C_H
#define NTT_C_H

#include "tools.h"


void CT_butterfly_int32(
    void *src,
    size_t indx_a, size_t indx_b,
    void *twiddle,
    void *mod,
    size_t size
    );

void m_layer_CT_butterfly_int32(
    void *src,
    size_t layers, size_t step,
    void *_root_table,
    void *mod,
    size_t size
    );

void m_layer_inv_CT_butterfly_int32(
    void *src,
    size_t layers, size_t step,
    void *_root_table,
    void *mod,
    size_t size
    );

void CT_butterfly_generic(
    void *src,
    size_t indx_a, size_t indx_b,
    void *twiddle,
    void *mod,
    size_t size,
    void (*addmod)(void *des, void *src1, void *src2, void *mod),
    void (*submod)(void *des, void *src1, void *src2, void *mod),
    void (*mulmod)(void *des, void *src1, void *src2, void *mod)
    );

void m_layer_CT_butterfly_generic(
    void *src,
    size_t layers, size_t step,
    void *_root_table,
    void *mod,
    size_t size,
    void (*CT_butterfly)(
        void *src,
        size_t indx_a, size_t indx_b,
        void *twiddle,
        void *mod,
        size_t size
        )
    );

void m_layer_inv_CT_butterfly_generic(
    void *src,
    size_t layers, size_t step,
    void *_root_table,
    void *mod,
    size_t size,
    void (*CT_butterfly)(
        void *src,
        size_t indx_a, size_t indx_b,
        void *twiddle,
        void *mod,
        size_t size
        )
    );

void compressed_CT_NTT_generic(
    void *src,
    size_t start_level, size_t end_level,
    void *_root_table,
    void *mod,
    struct compress_profile *_profile,
    size_t size,
    void (*m_layer_CT_butterfly)(
        void *src,
        size_t layers, size_t step,
        void *_root_table,
        void *mod,
        size_t size
        )
    );

void compressed_inv_CT_NTT_generic(
    void *src,
    size_t start_level, size_t end_level,
    void *_root_table,
    void *mod,
    struct compress_profile *_profile,
    size_t size,
    void (*m_layer_inv_CT_butterfly)(
        void *src,
        size_t layers, size_t step,
        void *_root_table,
        void *mod,
        size_t size
        )
    );


// void _CT_butterfly(int *src, int indx_a, int indx_b, int twiddle, int _Q);

// void _m_layer_CT_butterfly(int *src, int layers, int step, int *_root_table, int _Q);
// void _m_layer_inv_CT_butterfly(int *src, int layers, int step, int *_root_table, int Q);

// void _compressed_CT_NTT(int *src, int start_level, int end_level, int *_root_table, int _Q, struct compress_profile *_profile);
// void compressed_inv_CT_NTT(int *src, int start_level, int end_level, int *_root_table, int _Q, struct compress_profile *_profile);

#endif

