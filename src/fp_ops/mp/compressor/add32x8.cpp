#include "thls/tops/bit_heap.hpp"

using namespace thls;

fw_uint<32> add32x8(
    fw_uint<32> x0,
    fw_uint<32> x1,
    fw_uint<32> x2,
    fw_uint<32> x3,
    fw_uint<32> x4,
    fw_uint<32> x5,
    fw_uint<32> x6,
    fw_uint<32> x7
)
{
#pragma HLS INTERFACE ap_ctrl_none register port=return
#pragma HLS PIPELINE
    return x0+x1+x2+x3+x4+x5+x6+x7;
}

