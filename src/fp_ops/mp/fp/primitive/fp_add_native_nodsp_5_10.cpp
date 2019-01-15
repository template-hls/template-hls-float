#include "hls_half.h"

extern void fp_add_native_nodsp_5_10(
    const half *a,
    const half *b,
    half *res
)
{
#pragma HLS INTERFACE ap_ctrl_none register port=return
#pragma HLS PIPELINE
    half ha(*a);
    half hb(*b);
    half temp;
    #pragma HLS RESOURCE variable=temp core=HAddSub_nodsp
    temp=ha+hb;
    *res=temp;
}
