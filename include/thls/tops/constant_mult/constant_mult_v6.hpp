#ifndef thls_constant_mult_v6_hpp
#define thls_constant_mult_v6_hpp

#include "thls/tops/fw_uint.hpp"

#include <random>

namespace thls {

struct cmult_impl_v6_booth
{
    static constexpr const char *description=
"Detect runs of ones and encode as booth-style +- runs, "
"attempting to do widths more carefully. It should reduce in smaller"
"width adders from the HLS tools point of view, though in principle"
"the tool could do constant-propagation on the v5 version.";

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
        static_assert(C&1, "Constant must be normalised.");

        static constexpr int run_length=find_next_zero(C);
        // Warning: post_zeros is guaranteed to be -1 at some point
        static constexpr int post_zeros=find_next_one(C>>run_length); 

        // If these are the last-bits, just make sure Crem is zero
        static constexpr int width=post_zeros==-1 ? run_length : post_zeros+run_length;
        static constexpr uint64_t Crem=C>>width;

        THLS_INLINE static fw_uint<WR> go(const fw_uint<WI> &x) {
            static_assert(WR>=width, "Logic violation.");

            fw_uint<WR> curr;
            if(run_length==1) {
                curr=resize<WR>(x);
            }else if(run_length==2) {
                curr=resize<WR>(zpad_lo<1>(x)) + resize<WR>(x);
            }else{
                //std::cerr<<"run_length="<<run_length<<"\n";
                auto hip=resize<WR>(zpad_lo<run_length>(x));
                auto lop=resize<WR>(x);
                //std::cerr<<"  hip="<<hip<<"\n";
                //std::cerr<<"  lop="<<lop<<"\n";
                curr = hip-lop;
            }

            if(Crem==0){
                return curr;
            }else{
                fw_uint<WR - width> base = helper<WR - width, WI, Crem>::go(x);
                return curr + zpad_lo<width>(base);
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



}; // thls

#endif
