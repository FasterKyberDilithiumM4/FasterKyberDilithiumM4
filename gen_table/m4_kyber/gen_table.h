#ifndef GEN_TABLE_H
#define GEN_TABLE_H

#include <stddef.h>
#include <stdbool.h>

#include "tools.h"

void gen_CT_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod)
    );

void gen_CT_negacyclic_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod)
    );

void gen_inv_CT_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod),
    void (*expmod) (void *_des, void *_src, size_t _size, void *_mod)
    );

void gen_twist_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod)
    );

void gen_streamlined_CT_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod),
    struct compress_profile *_profile, bool pad
    );

void gen_streamlined_CT_negacyclic_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *_mod),
    struct compress_profile *_profile, bool pad
    );

void gen_streamlined_inv_CT_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod) (void *_des, void *_src1, void *_src2, void *_mod),
    void (*expmod) (void *_des, void *_src, size_t e, void *_mod),
    struct compress_profile *_profile, bool pad
    );

void gen_streamlined_inv_CT_negacyclic_table_generic(
    void *des,
    void *scale1, void *omega,
    void *scale2, void *twist_omega,
    void *mod,
    size_t size,
    void (*mulmod) (void *_des, void *_src1, void *_src2, void *_mod),
    void (*expmod) (void *_des, void *_src, size_t e, void *_mod),
    struct compress_profile *_profile, bool pad
    );

void gen_mul_table_generic(
    void *des,
    void *scale, void *omega,
    void *mod,
    size_t size,
    void (*mulmod)(void *_des, void *_src1, void *_src2, void *mod)
    );

#endif

