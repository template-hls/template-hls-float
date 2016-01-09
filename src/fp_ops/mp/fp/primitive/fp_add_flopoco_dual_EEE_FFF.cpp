#include "thls/tops/fp_flopoco_add_dual_v1.hpp"

using namespace thls;

static const int wE = ${EEE};
static const int wF = ${FFF};

extern fw_uint<wE+wF+3> fp_add_flopoco_dual_${EEE}_${FFF}(
    fw_uint<wE+wF+3> a,
    fw_uint<wE+wF+3> b
)
{
#pragma HLS INTERFACE ap_ctrl_none register port=return
#pragma HLS PIPELINE
    fp_flopoco<wE,wF> fA(a), fB(b);
    auto fRes=add_dual<wE,wF>(fA, fB);
    return fRes.bits;
}
