#ifndef thls_constant_mult_v8_kcm_hpp
#define thls_constant_mult_v8_kcm_hpp

#include "thls/tops/fw_uint.hpp"

#include <random>

namespace thls {


template<const unsigned K>
struct cmult_impl_v8_kcm
{
    static constexpr const char *description=
"Recursive KCM implementation. Takes K bits at a time, then recursively adds.";

    static constexpr int find_next_one(uint64_t x)
    {
        return x==0 ? -1 : x&1 ? 0 : 1+find_next_one(x>>1);
    }

    template<unsigned WR, unsigned WI, uint64_t C>
    struct helper
    {
        static const constexpr unsigned WIcurr = thls_ctMin(K,WI);
        static const constexpr unsigned WIrem = WI-WIcurr;
        static const constexpr unsigned WRrem = thls_ctMax(0, WR-WIcurr);

        static fw_uint<WR> lut_filler(unsigned i)
        {
            const uint64_t M = WR==0 ? 0ull : (0xFFFFFFFFFFFFFFFFull>>(64-WR));
            return take_lsbs<WR>(fw_uint<WR>::from_uint64(C&M) * fw_uint<WR>::from_bits(i&M)); 
        }

        THLS_INLINE static fw_uint<WR> go(const fw_uint<WI> &x) {
            static lut<WR,WIcurr> entries(lut_filler);   

            fw_uint<WR> curr=entries(take_lsbs<WIcurr>(x));
            if(WRrem==0){
                return curr;
            }else{
                fw_uint<WRrem> rem=helper<WRrem, WIrem, C>::go(drop_lsbs<WIcurr>(x));
                fw_uint<WRrem> curr_hi=drop_lsbs<WR-WRrem>(curr);
                fw_uint<WR-WRrem> curr_lo=take_lsbs<WR-WRrem>(curr);
                return concat( curr_hi+rem , curr_lo );
            }
        }
    };

    template<unsigned WR, uint64_t C>
    struct helper<WR, 0, C>
    {
        THLS_INLINE static fw_uint<WR> go(const fw_uint<0> &x)
        { return zg<WR>(); }
    };

    template<unsigned WR, unsigned WI, uint64_t C>
    THLS_INLINE static fw_uint<WR> go(const fw_uint<WI> &x)
    {
        static_assert(C>0, "Constant is zero.");

        const unsigned WIa=thls_ctMin(WR,WI);
        const unsigned WC=thls_ctLog2Ceil(C);
        const unsigned WCa=thls_ctMin(WR,WC);
        const uint64_t WC_MASK=0xFFFFFFFFFFFFFFFFull>>(64-WCa);
        const uint64_t Ca = WC_MASK & C;

        if(Ca==0){
            // We allow the constant after truncation to be zero. Probably 
            // we should allow zero in general?
            return zg<WR>();
        }else {
            const int zpad=find_next_one(Ca);
            if(zpad<0){
                static_assert(zpad >=0 || zpad==-1, "Pre-condition.");
                return zg<WR>();
            }else{
                const uint64_t Crem= (zpad<0) ? 1 : Ca>>zpad; // Sigh. Roll on static if
                
                const fw_uint<WIa> xa=take_lsbs<WIa>(x);
                return zpad_lo<zpad>(helper<WR-zpad, WIa, Crem>::go(xa));
            }
        }
    }
};

using cmult_impl_v8_kcm_k2 = cmult_impl_v8_kcm<2>;
using cmult_impl_v8_kcm_k4 = cmult_impl_v8_kcm<4>;
using cmult_impl_v8_kcm_k5 = cmult_impl_v8_kcm<5>;

}; // thls

#endif
