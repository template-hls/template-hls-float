#include "thls/tops/bit_heap.hpp"

using namespace thls;

fw_uint<32> add32x4_compressor(
    fw_uint<32> x0,
    fw_uint<32> x1,
    fw_uint<32> x2,
    fw_uint<32> x3
)
{
#pragma HLS INTERFACE ap_ctrl_none register port=return
#pragma HLS PIPELINE
    auto bh_x0=fw_uint_to_bit_heap(x0);
    auto bh_x1=fw_uint_to_bit_heap(x1);

    auto bh_x2=fw_uint_to_bit_heap(x2);
    auto bh_x3=fw_uint_to_bit_heap(x3);

    auto bh_x01=merge_bit_heaps(bh_x0,bh_x1);
    auto bh_x23=merge_bit_heaps(bh_x2,bh_x3);

    auto bh_x0123=merge_bit_heaps(bh_x01,bh_x23);

    auto bh_c0123=compress(bh_x0123);

    fw_uint<32> res=bit_heap_collapse(bh_c0123);

    return res;
}
