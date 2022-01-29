#include "NTT_params.h"

#include "tools.h"
#include "ntt_c.h"



void CT_butterfly_int32(
    void *src,
    size_t indx_a, size_t indx_b,
    void *twiddle,
    void *mod,
    size_t size
    ){

    CT_butterfly_generic(
        src,
        indx_a, indx_b,
        twiddle,
        mod,
        size,
        addmod_int32,
        submod_int32,
        mulmod_int32
        );

}

void m_layer_CT_butterfly_int32(
    void *src,
    size_t layers, size_t step,
    void *_root_table,
    void *mod,
    size_t size
    ){

    m_layer_CT_butterfly_generic(
        src,
        layers, step,
        _root_table,
        mod,
        size,
        CT_butterfly_int32
        );

}

void m_layer_inv_CT_butterfly_int32(
    void *src,
    size_t layers, size_t step,
    void *_root_table,
    void *mod,
    size_t size
    ){

    m_layer_inv_CT_butterfly_generic(
        src,
        layers, step,
        _root_table,
        mod,
        size,
        CT_butterfly_int32
        );

}

void CT_butterfly_generic(
    void *src,
    size_t indx_a, size_t indx_b,
    void *twiddle,
    void *mod,
    size_t size,
    void (*addmod)(void *des, void *src1, void *src2, void *mod),
    void (*submod)(void *des, void *src1, void *src2, void *mod),
    void (*mulmod)(void *des, void *src1, void *src2, void *mod)
    ){

    char tmp[size];

    mulmod(tmp, src + indx_b * size, twiddle, mod);
    submod(src + indx_b * size, src + indx_a * size, tmp, mod);
    addmod(src + indx_a * size, src + indx_a * size, tmp, mod);

}

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
    ){

    size_t real_count, real_step, twiddle_count, jump;
    void *real_root_table;

    for(size_t i = 0; i < layers; i++){

        twiddle_count = 1 << i;

        real_count = 1 << (layers - 1 - i);

        jump = step << (layers - i);

        real_root_table = _root_table + ((1 << i) - 1) * size;

        real_step = step << (layers - 1 - i);

        for(size_t k = 0; k < real_count; k++){
            for(size_t j = 0; j < twiddle_count; j++){
                CT_butterfly(
                    src + (j * jump + k * step) * size,
                    0, real_step,
                    real_root_table + j * size,
                    mod,
                    size
                    );
            }
        }

    }

}


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
    ){

    size_t real_count, real_step, twiddle_count, jump;
    void *real_root_table;

    for(size_t i = 0; i < layers; i++){

        twiddle_count = 1 << i;

        real_count = 1 << (layers - 1 - i);

        jump = step << (i + 1);

        real_root_table = _root_table + ((1 << i) - 1) * size;

        real_step = step << i;

        for(size_t k = 0; k < real_count; k++){
            for(size_t j = 0; j < twiddle_count; j++){
                CT_butterfly(
                    src + (j * step + k * jump) * size,
                    0, real_step,
                    real_root_table + j * size,
                    mod,
                    size
                    );
            }
        }

    }

}

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
    ){

    size_t step, offset;
    size_t real_start_level, real_end_level;
    void *real_root_table;
    size_t *level_indx;

    real_start_level = 0;
    for(size_t i = 0; i < start_level; i++){
        real_start_level += (_profile->merged_layers)[i];
    }

    real_end_level = real_start_level;
    for(size_t i = start_level; i < end_level; i++){
        real_end_level += (_profile->merged_layers)[i];
    }

    real_root_table = _root_table + ((1 << real_start_level) - 1) * size;

    level_indx = (_profile->merged_layers) + start_level;

    for(size_t level = real_start_level; level <= real_end_level; level += *(level_indx++)){

        step = ARRAY_N >> (level + (*level_indx));

        offset = 0;

        for(size_t count = 0; count < (1 << level); count++){

            for(size_t i = 0; i < step; i++){
                m_layer_CT_butterfly(
                    src + (offset + i) * size,
                    *level_indx, step,
                    real_root_table,
                    mod,
                    size
                    );
            }

            offset += ARRAY_N >> level;

            real_root_table += ((1 << (*level_indx)) - 1) * size;

        }

    }

}


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
    ){

    size_t step, offset;
    size_t real_start_level, real_end_level;
    void *real_root_table;
    size_t *level_indx;

    real_start_level = 0;
    for(size_t i = 0; i < start_level; i++){
        real_start_level += (_profile->merged_layers)[i];
    }

    real_end_level = real_start_level;
    for(size_t i = start_level; i < end_level; i++){
        real_end_level += (_profile->merged_layers)[i];
    }

    real_root_table = _root_table + ((1 << real_start_level) - 1) * size;

    level_indx = (_profile->merged_layers) + start_level;

    for(size_t level = real_start_level; level <= real_end_level; level += *(level_indx++)){

        step = (ARRAY_N >> LOGNTT_N) << level;

        for(size_t count = 0; count < (1 << level); count++){

            offset = count * (ARRAY_N >> LOGNTT_N);

            for(size_t i = 0; i < (NTT_N >> ((*level_indx) + level)); i++){

                for(size_t j = 0; j < (ARRAY_N >> LOGNTT_N); j++){
                    m_layer_inv_CT_butterfly(
                        src + (offset + j) * size,
                        *level_indx, step,
                        real_root_table,
                        mod,
                        size
                        );
                }

                offset += (ARRAY_N >> LOGNTT_N) << ((*level_indx) + level);

            }

            real_root_table += ((1 << (*level_indx)) - 1) * size;

        }

    }

}

// static int _center_mul(int src1, int src2, int mod){
//     int t = (int)(((long long)src1 * (long long)src2) % (long long)mod);
//     if(t > (mod >> 1)){
//         return t - mod;
//     }
//     if(t < - (mod >> 1)){
//         return t + mod;
//     }
//     return t;
// }

// void _CT_butterfly(int *src, int indx_a, int indx_b, int twiddle, int _Q){
//     src[indx_b] = _center_mul(src[indx_b], twiddle, _Q);
//     src[indx_a] = _center_mul(src[indx_a] + src[indx_b], 1, _Q);
//     src[indx_b] = _center_mul(src[indx_a] - (src[indx_b] << 1), 1, _Q);
// }

// void _m_layer_CT_butterfly(int *src, int layers, int step, int *_root_table, int Q){

//     int real_count, real_step, twiddle_count, jump;
//     int *real_root_table;

//     for(int i = 0; i < layers; i++){

//         // the number of twiddle factors of layer i
//         twiddle_count = 1 << i;

//         // the number of butterflies sharing the same twiddle factor
//         real_count = 1 << (layers - 1 - i);

//         // distance for jumping to the next set of butterflies
//         // with different twiddle factor
//         jump = step << (layers - i);

//         // Invariance: real_count * twiddle_count = 1 << (layers - 1)

//         // move to the root table for layer i
//         real_root_table = _root_table + ((1 << i) - 1);

//         // calculate the true distance for butterflies
//         real_step = step << (layers - 1 - i);

//         // calculate NTTs at current layer with CT butterflies
//         for(int k = 0; k < real_count; k++){
//             for(int j = 0; j < twiddle_count; j++){
//                 _CT_butterfly(src + j * jump + k * step, 0, real_step, real_root_table[j], Q);
//             }
//         }

//     }

// }

// void _m_layer_inv_CT_butterfly(int *src, int layers, int step, int *_root_table, int Q){

//     int real_count, real_step, twiddle_count, jump;
//     int *real_root_table;

//     for(int i = 0; i < layers; i++){

//         // he number of twiddle factors of layer i
//         twiddle_count = 1 << i;

//         // the number of butterflies sharing the same twiddle factor
//         real_count = 1 << (layers - 1 - i);

//         // distance for jumping to the next set of butterflies
//         // with same twiddle factor
//         jump = step << (i + 1);

//         // Invariance: real_count * twiddle_count = 1 << (layers - 1)

//         // move to the root table for layer i
//         real_root_table = _root_table + ((1 << i) - 1);

//         // calculate the true distance for butterflies
//         real_step = step << i;

//         // calculate NTT inverses at current layer with CT butterflies
//         for(int k = 0; k < real_count; k++){
//             for(int j = 0; j < twiddle_count; j++){
//                 _CT_butterfly(src + j * step + k * jump, 0, real_step, real_root_table[j], Q);
//             }
//         }

//     }

// }

// compute compressed NTT with multi-layer CT butterflies
// from level
// profile.merged_layers[0] + ... + profile.merged_layers[start_level - 1]
// to level
// profile.merged_layers[0] + ... + profile.merged_layers[end_level] - 1
// void _compressed_CT_NTT(int *src, int start_level, int end_level, int *_root_table, int Q, struct compress_profile *_profile){

//     int step, offset;
//     int real_start_level, real_end_level;
//     int *real_root_table;
//     int *level_indx;

//     // set real_start_level to
//     // profile.merged_layers[0] + ... + profile.merged_layers[start_level - 1]
//     real_start_level = 0;
//     for(int i = 0; i < start_level; i++){
//         real_start_level += (_profile->merged_layers)[i];
//     }

//     // set real_end_level to
//     // profile.merged_layers[0] + ... + profile.merged_layers[end_level - 1]
//     real_end_level = real_start_level;
//     for(int i = start_level; i < end_level; i++){
//         real_end_level += (_profile->merged_layers)[i];
//     }

//     // set real_root_table to the pointer for twiddle factors
//     real_root_table = _root_table + ((1 << real_start_level) - 1);

//     // set level_indx as a pointer for profile.merged_layers
//     level_indx = (_profile->merged_layers) + start_level;

//     // iterate from start_level to end_level implicitly;
//     // we iterate explicitly as follows:
//     // level = real_start_level,
//     // level += profile.merged_layers[start_level],
//     // level += profile.merged_layers[start_level + 1],
//     // ...
//     // level += profile.merged_layers[end_level - 1]
//     for(int level = real_start_level; level <= real_end_level; level += *(level_indx++)){

//         // minimum distance between coefficients in current multi-layer CT butterflies
//         step = ARRAY_N >> (level + (*level_indx));

//         // we start at the beginning of the array
//         offset = 0;

//         // iterate through different sets of twiddle factors
//         for(int count = 0; count < (1 << level); count++){

//             // this loop suppots incomplete splitting with multi-layer
//             // CT butterflies for NTT
//             for(int i = 0; i < step; i++){

//                 // call for multi-layer CT butterfly
//                 _m_layer_CT_butterfly(src + offset + i, *level_indx, step, real_root_table, Q);

//             }

//             // update offset to the next multi-layer butterfly with
//             // a different set of twiddle factors
//             offset += ARRAY_N >> level;

//             // update the table for twiddle factors
//             real_root_table += ((1 << (*level_indx)) - 1);

//         }

//     }

// }

// compute compressed inverse NTT with multi-layer inverse CT butterflies
// from level
// profile.merged_layers[0] + ... + profile.merged_layers[start_level - 1]
// to level
// profile.merged_layers[0] + ... + profile.merged_layers[end_level] - 1
// void compressed_inv_CT_NTT(int *src, int start_level, int end_level, int *_root_table, int Q, struct compress_profile *_profile){

//     int step, offset;
//     int real_start_level, real_end_level;
//     int *real_root_table;
//     int *level_indx;

//     // set real_start_level to
//     // profile.merged_layers[0] + ... + profile.merged_layers[start_level - 1]
//     real_start_level = 0;
//     for(int i = 0; i < start_level; i++){
//         real_start_level += (_profile->merged_layers)[i];
//     }

//     // set real_end_level to
//     // profile.merged_layers[0] + ... + profile.merged_layers[end_level - 1]
//     real_end_level = real_start_level;
//     for(int i = start_level; i < end_level; i++){
//         real_end_level += (_profile->merged_layers)[i];
//     }

//     // set real_root_table to the pointer for twiddle factors
//     real_root_table = _root_table + ((1 << real_start_level) - 1);

//     // set level_indx as a pointer for profile.merged_layers
//     level_indx = (_profile->merged_layers) + start_level;

//     // iterate from start_level to end_level implicitly;
//     // we iterate explicitly as follows:
//     // level = real_start_level,
//     // level += profile.merged_layers[start_level],
//     // level += profile.merged_layers[start_level + 1],
//     // ...
//     // level += profile.merged_layers[end_level - 1]
//     for(int level = real_start_level; level <= real_end_level; level += *(level_indx++)){

//         // minimum distance between coefficients in current multi-layer CT butterflies
//         step = (ARRAY_N >> LOGNTT_N) << level;

//         // iterate through different sets of twiddle factors
//         for(int count = 0; count < (1 << level); count++){

//             // set offset to the starting index of multi-layer butterfies
//             // with the current twiddle factors
//             offset = count * (ARRAY_N >> LOGNTT_N);

//             // this loop suppots incomplete splitting with inverse CT butterflies
//             // for NTT inverse
//             for(int i = 0; i < (NTT_N >> ((*level_indx) + level)); i++){

//                 // this loop suppots incomplete splitting with multi-layer
//                 // inverse CT butterflies for NTT inverse
//                 for(int j = 0; j < (ARRAY_N >> LOGNTT_N); j++){

//                     // call for multi-layer inverse CT butterfly
//                     _m_layer_inv_CT_butterfly(src + offset + j, *level_indx, step, real_root_table, Q);

//                 }

//                 // update offset to the next multi-layer butterfly with
//                 // the same set of twiddle factors
//                 offset += (ARRAY_N >> LOGNTT_N) << ((*level_indx) + level);

//             }

//             // update the table for twiddle factors
//             real_root_table += ((1 << (*level_indx)) - 1);

//         }

//     }

// }











