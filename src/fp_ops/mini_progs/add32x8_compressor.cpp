#include "thls/tops/bit_heap.hpp"

using namespace thls;

fw_uint<32> add32x8_compressor(
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
    auto bh_x0=fw_uint_to_bit_heap(x0);
    auto bh_x1=fw_uint_to_bit_heap(x1);

    auto bh_x2=fw_uint_to_bit_heap(x2);
    auto bh_x3=fw_uint_to_bit_heap(x3);

    auto bh_x4=fw_uint_to_bit_heap(x4);
    auto bh_x5=fw_uint_to_bit_heap(x5);

    auto bh_x6=fw_uint_to_bit_heap(x6);
    auto bh_x7=fw_uint_to_bit_heap(x7);

    auto bh_x01=merge_bit_heaps(bh_x0,bh_x1);
    auto bh_x23=merge_bit_heaps(bh_x2,bh_x3);
    auto bh_x45=merge_bit_heaps(bh_x4,bh_x5);
    auto bh_x67=merge_bit_heaps(bh_x6,bh_x7);

    auto bh_x0123=merge_bit_heaps(bh_x01,bh_x23);
    auto bh_x4567=merge_bit_heaps(bh_x45,bh_x67);

    auto bh_x01234567=merge_bit_heaps(bh_x0123,bh_x4567);

    auto bh_c01234567=compress(bh_x01234567);

    fw_uint<32> res=bit_heap_collapse(bh_c01234567);

    return res;
}
