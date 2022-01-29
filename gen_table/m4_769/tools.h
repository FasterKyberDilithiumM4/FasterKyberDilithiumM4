#ifndef TOOLS_H
#define TOOLS_H

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct compress_profile{
    size_t compressed_layers;
    size_t merged_layers[16];
};

void cmod_int16(void *des, void *src, void *mod);
void cmod_int32(void *des, void *src, void *mod);
void cmod_int64(void *des, void *src, void *mod);

void addmod_int32(void *des, void *src1, void *src2, void *mod);
void submod_int32(void *des, void *src1, void *src2, void *mod);

void mulmod_int16(void *des, void *src1, void *src2, void *mod);
void mulmod_int32(void *des, void *src1, void *src2, void *mod);

void expmod_int16(void *des, void *src, size_t e, void *mod);
void expmod_int32(void *des, void *src, size_t e, void *mod);

void bitreverse(void *src, size_t len, size_t size);

#endif

