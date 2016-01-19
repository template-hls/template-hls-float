#include "hls_half.h"

extern void fp_add_native_5_10(
    const half *a,
    const half *b,
    half *res
)
{
#pragma HLS INTERFACE ap_ctrl_none register port=return
#pragma HLS PIPELINE
    half ha(*a);
    half hb(*b);
    half hres=ha+hb;
    *res=hres;
}
