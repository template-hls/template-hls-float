#include "thls/tops/row_heap.hpp"

using namespace thls;

fw_uint<32> add32x8_row_heap(
    const fw_uint<32> &x0,
    const fw_uint<32> &x1,
    const fw_uint<32> &x2,
    const fw_uint<32> &x3,
    const fw_uint<32> &x4,
    const fw_uint<32> &x5,
    const fw_uint<32> &x6,
    const fw_uint<32> &x7
)
{
    auto bh_x0=fw_uint_to_row_heap(x0);
    auto bh_x1=fw_uint_to_row_heap(x1);

    auto bh_x2=fw_uint_to_row_heap(x2);
    auto bh_x3=fw_uint_to_row_heap(x3);

    auto bh_x4=fw_uint_to_row_heap(x4);
    auto bh_x5=fw_uint_to_row_heap(x5);

    auto bh_x6=fw_uint_to_row_heap(x6);
    auto bh_x7=fw_uint_to_row_heap(x7);

    auto bh_x01234567=bh_x0+bh_x1+bh_x2+bh_x3+bh_x4+bh_x5+bh_x6+bh_x7;

    auto bh_c01234567=compress(bh_x01234567);

    fw_uint<32> res=collapse(bh_c01234567);

    return res;
}
