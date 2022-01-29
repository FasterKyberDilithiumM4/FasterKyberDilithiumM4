#ifndef NTT_PARAMS_H
#define NTT_PARAMS_H

#define ARRAY_N 256

#define NTT_N 256
#define LOGNTT_N 8

#define Q1 8380417
// omegaQ1 = Q1pr^((Q1 - 1) / (NTT_N << 1)) mod Q1
#define omegaQ1 1753
// invomegaQ1 = omegaQ1^{-1} mod Q1
#define invomegaQ1 731434
// RmodQ1 = 2^32 mod^{+-} Q1
#define RmodQ1 4193792
// invNQ1 = NTT_N^{-1} mod Q1
#define invNQ1 8314945

#endif

