#ifndef thls_constant_mult_v7_kcm_hpp
#define thls_constant_mult_v7_kcm_hpp

#include "thls/tops/fw_uint.hpp"

#include <random>

namespace thls {

template<const unsigned K>
struct cmult_impl_v7_kcm
{
    static constexpr const char *description=
"Naive KCM implementation. Iterates over shifted input using a single LUT. Hopefully "
"will get unrolled by the HLS tools. ";

    template<unsigned WR, unsigned WI, uint64_t C>
    struct helper
    {
        static const constexpr unsigned Wcurr = thls_ctMin(K,WI);
        static const constexpr unsigned Wrem = WI-Wcurr;

        static fw_uint<WR> lut_filler(unsigned i)
        {
            const uint64_t M = 0xFFFFFFFFFFFFFFFFull>>(64-WR);
            return take_lsbs<WR>(fw_uint<WR>::from_bits(C&M) * fw_uint<WR>::from_bits(i&M)); 
        }

        THLS_INLINE static fw_uint<WR> go(const fw_uint<WI> &x) {
            static lut<WR,K> entries(lut_filler);   

            fw_uint<WR> acc(0);

            for(unsigned i=0; i<WI; i+=K){
                #pragma HLS UNROLL
                fw_uint<WR> c=entries( take_lsbs<K>(x>>i) );
                acc = acc + ( c<<i );
            }

            return acc;
        }
    };

    template<unsigned WR, unsigned WI, uint64_t C>
    THLS_INLINE static fw_uint<WR> go(const fw_uint<WI> &x)
    {
        static_assert(C>0, "Constant is zero.");

        const unsigned WIa=thls_ctMax(K,thls_ctMin(WR,WI));
        const unsigned WC=thls_ctLog2Ceil(C);
        const unsigned WCa=thls_ctMin(WR,WC);
        const uint64_t WC_MASK=0xFFFFFFFFFFFFFFFFull>>(64-WCa);
        const uint64_t Ca = WC_MASK & C;

        if(Ca==0){
            // We allow the constant after truncation to be zero. Probably 
            // we should allow zero in general?
            return zg<WR>();
        }else {
            const fw_uint<WIa> xa=resize<WIa>(x); // This may need to extend if K>WI
            return helper<WR, WIa, Ca>::go(xa);
        }
    }
};

using cmult_impl_v7_kcm_k4 = cmult_impl_v7_kcm<4>;
using cmult_impl_v7_kcm_k5 = cmult_impl_v7_kcm<5>;

}; // thls

#endif
