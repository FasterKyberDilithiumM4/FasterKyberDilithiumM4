#ifndef NTT_PARAMS_H
#define NTT_PARAMS_H

#define ARRAY_N 256

#define NTT_N 128
#define LOGNTT_N 7

#define Q1 769
#define Q1pr 11
// omegaQ1 = Q1pr^((Q1 - 1) / (NTT_N << 1)) mod Q1
#define omegaQ1 (-207)
// invomegaQ1 = omegaQ1^{-1} mod Q1
#define invomegaQ1 26
// RmodQ1 = 2^16 mod^{+-} Q1
#define RmodQ1 171
// Q1prime = -Q1^{-1} mod^{+-} 2^16
#define Q1prime 767



// invNQ1 = NTT_N^{-1} mod Q1
#define invNQ1 (-6)


// Q1Q1prime = Q1 || Q1prime
// #define Q1Q1prime 218172671

#endif

