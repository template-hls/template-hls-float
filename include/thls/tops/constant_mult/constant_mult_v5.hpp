#ifndef thls_constant_mult_v5_hpp
#define thls_constant_mult_v5_hpp

#include "thls/tops/fw_uint.hpp"

#include <random>

namespace thls {

struct cmult_impl_v5_booth
{
    static constexpr const char *description=
"Detect runs of ones and encode as booth-style +- runs";

    static constexpr int find_next_one(uint64_t C)
    {
        return C==0 ? -1 : (C&1) ? 0 : 1+find_next_one(C>>1);
    }

    static constexpr int find_next_zero(uint64_t C)
    {
        return (C&1)==0 ? 0 : 1+find_next_zero(C>>1);
    }

    template<unsigned WR, unsigned WI, uint64_t C>
    struct helper
    {
        static constexpr int pad_lsbs=find_next_one(C);
        static constexpr int run_length=find_next_zero(C>>pad_lsbs);
        static constexpr int width=pad_lsbs+run_length;
        static constexpr uint64_t Crem=C>>width;

        THLS_INLINE static fw_uint<WR> go(const fw_uint<WI> &x) {
            static_assert(WR>=width, "Logic violation.");

            fw_uint<WR - width> base = helper<WR - width, WI, Crem>::go(x);
            //const int rl=run_length;
            //const int pl=pad_lsbs;

            if(run_length==1) {
                return resize<WR>(zpad_lo<pad_lsbs>(x)) + zpad_lo<width>(base);
            }else if(run_length==2) {
                return (resize<WR>(zpad_lo<pad_lsbs+1>(x)) + resize<WR>(zpad_lo<pad_lsbs>(x))) + zpad_lo<width>(base);
            }else{
                const int ppp=pad_lsbs+run_length;
                auto hip1=zpad_lo<ppp>(x);
                auto hip=resize<WR>(hip1);
                auto lop=resize<WR>(zpad_lo<pad_lsbs>(x));
                return (hip-lop) + zpad_lo<width>(base);
            }
        }
    };

    template<unsigned WR, unsigned WI>
    struct helper<WR,WI,0>
    {
        THLS_INLINE static fw_uint<WR> go(const fw_uint<WI> &x)
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
            return zg<WR>();
        }else {
            return helper<WR, WIa, Ca>::go(take_lsbs<WIa>(x));
        }
    }
};



}; // thls

#endif
