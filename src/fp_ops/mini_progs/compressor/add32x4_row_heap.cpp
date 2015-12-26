#include "thls/tops/row_heap.hpp"

using namespace thls;

fw_uint<32> add32x4_row_heap(
    const fw_uint<32> &x0,
    const fw_uint<32> &x1,
    const fw_uint<32> &x2,
    const fw_uint<32> &x3
)
{
    auto bh_x0=fw_uint_to_row_heap(x0);
    auto bh_x1=fw_uint_to_row_heap(x1);

    auto bh_x2=fw_uint_to_row_heap(x2);
    auto bh_x3=fw_uint_to_row_heap(x3);

    auto bh_x0123=bh_x0+bh_x1+bh_x2+bh_x3;

    auto bh_c0123=compress(bh_x0123);

    fw_uint<32> res=collapse(bh_c0123);

    return res;
}
