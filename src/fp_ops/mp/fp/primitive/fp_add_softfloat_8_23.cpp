#include "thls/tops/fp_softfloat.hpp"

extern uint32_t fp_add_softfloat_8_23(
    uint32_t a,
    uint32_t b
)
{
#pragma HLS INTERFACE ap_ctrl_none register port=return
#pragma HLS PIPELINE
    return f32_add( float32_t{a}, float32_t{b} ).v;
}
