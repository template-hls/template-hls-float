#include "thls/tops/fp_flopoco_mul_v1.hpp"

using namespace thls;

static const int wE1 = ${EEE1};
static const int wF1 = ${FFF1};

static const int wE2 = ${EEE2};
static const int wF2 = ${FFF2};

extern fw_uint<wE1+wF1+3> fp_mul_flopoco_ratio_${EEE1}_${FFF1}_${EEE2}_${FFF2}(
    fw_uint<wE1+wF1+3> a,
    fw_uint<wE2+wF2+3> b
)
{
#pragma HLS INTERFACE ap_ctrl_none register port=return
#pragma HLS PIPELINE
    fp_flopoco<wE1,wF1> fA(a);
    fp_flopoco<wE2,wF2> fB(b);
    auto fRes=mul<wE1,wF1>(fA, fB);
    return fRes.bits;
}
