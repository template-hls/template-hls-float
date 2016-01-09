#include "thls/tops/bit_heap.hpp"

using namespace thls;

fw_uint<64> add64x8(
    fw_uint<64> x0,
    fw_uint<64> x1,
    fw_uint<64> x2,
    fw_uint<64> x3,
    fw_uint<64> x4,
    fw_uint<64> x5,
    fw_uint<64> x6,
    fw_uint<64> x7
)
{
#pragma HLS INTERFACE ap_ctrl_none register port=return
#pragma HLS PIPELINE
    return x0+x1+x2+x3+x4+x5+x6+x7;
}

