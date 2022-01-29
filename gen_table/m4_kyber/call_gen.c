#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "NTT_params.h"

#include "tools.h"
#include "gen_table.h"

#define BUFF_MAX (NTT_N << 3)

struct compress_profile profile;

// tool from: https://github.com/multi-moduli-ntt-saber/multi-moduli-ntt-saber/tree/master/gen_table

int main()
{

    // two "blocks" of merged layers
    profile.compressed_layers = 2;
    // first 4 layers merged...
    profile.merged_layers[0] = 4;
    // ...then 3
    profile.merged_layers[1] = 3;

    int16_t twiddle_int16[BUFF_MAX];

    int16_t scale_int16;
    int16_t scale2_int16;
    int16_t omega_int16;
    int16_t mod_int16;
    int32_t twist_omega_int16;

    // ================
    
    scale_int16 = RmodQ1;
    scale2_int16 = invNQ1;
    omega_int16 = omegaQ1;
    mod_int16 = Q1;
    twist_omega_int16 = invomegaQ1;
    // (w^(-1))^2 mod Q1
    expmod_int16(&omega_int16, &twist_omega_int16, 2, &mod_int16);
    // scale2_int16 = R * invN mod Q1
    mulmod_int16(&scale2_int16, &scale2_int16, &scale_int16, &mod_int16);
    // scale2_int16 = R^2 * invN mod Q1
    mulmod_int16(&scale2_int16, &scale2_int16, &scale_int16, &mod_int16);
    
    gen_streamlined_inv_CT_negacyclic_table_generic(
        twiddle_int16,
        &scale_int16, &omega_int16,
        &scale2_int16, &twist_omega_int16,
        &mod_int16,
        sizeof(int16_t),
        mulmod_int16,
        expmod_int16,
        &profile, 0);

    for (int i = 0; i < (NTT_N << 1) - 1; i++)
    {
        printf("%d, ", (twiddle_int16[i]+mod_int16)%mod_int16);
    }
    
    printf("\n\n");
    scale_int16 = RmodQ1;
    scale2_int16 = invNQ1;
    omega_int16 = omegaQ1;
    mod_int16 = Q1;
    twist_omega_int16 = invomegaQ1;

    gen_streamlined_CT_negacyclic_table_generic(
        twiddle_int16,
        &scale_int16, &omega_int16,
        &mod_int16,
        sizeof(int16_t),
        mulmod_int16,
        &profile, 0
        );
    for (int i = 0; i < (NTT_N  - 1); i++)
    {
        printf("%d, ", (twiddle_int16[i]+mod_int16)%mod_int16);
    }

}
