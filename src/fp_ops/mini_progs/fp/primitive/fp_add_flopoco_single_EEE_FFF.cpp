#include "thls/tops/bit_heap.hpp"

using namespace thls;

static const int wE = EEE;
static const int wF = FFF;

fw_uint<wE+wF+2> fp_add_single_EEE_FFF(
    fw_uint<wE+wF+2> a,
    fw_uint<wE+wF+2> b
)
{
#pragma HLS INTERFACE ap_ctrl_none register port=return
#pragma HLS PIPELINE
    fp_flopoco<wE,wF> fA(a), fB(b);
    auto fRes=add_single(a, b);
    return fRes.bits;
}
