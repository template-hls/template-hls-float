#ifndef thls_constant_mult_v11_csd_hpp
#define thls_constant_mult_v11_csd_hpp

#include "thls/tops/fw_uint.hpp"

#include <random>

namespace thls {

struct cmult_impl_v11_csd
{

    static constexpr const char *description=
"Convert to canonical signed digit form, then add up.";

    template<int W, int I, uint64_t A>
    struct csd_digit_list
    {
        static_assert(W>0, "Pre-condition");
        const uint64_t M=1ull<<(W-1);

        using tail_type = csd_digit_list<W,I-1,A>;

        static const int ap_curr = (A>>I)&1;
        static const int ap_up = I+1==W ? ap_curr : (A>>(I+1))&1;
        static const int ap_down = i==0 ? 0 : (A>>(I-1))&1;

        static const int theta = ap_curr ^ ap_down;
        static const int gamma = (1-tail_type::gamma) * theta;
        static const int sign = (1-2*ap_up) * gamma;

        template<int WR, int WI>
        static fw_uint<WR> go(const fw_uint<WI> &x)
        {
            #error "Here"

            fw_uint<WR-1> base=zpad_lsbs<1>(tail_type::go(drop_lsbs<1>(x)));
            if(sig==-1){
                return zpad_lsbs<1>(base) - resize<WR>(x);
            }
                return zpad_lsbs<1>(tail_type::go(drop_lsbs<1>(x)));
            }
        }
    };

    template<int W, uint64_t A>
    struct csd_digit_list<W,-1,A>
    {
        static_assert(W>0, "Pre-condition");
        static const int gamma = 0;
    };

    template<int WR, int WI, uint64_t C>
    struct helper
    {
        constexpr int WC = thls_ctLog2Ceil(C);
        using digits = csd_digit_list<WC,WC-1,C>;


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
                return zpad_lo<zpad>(helper<WR-zpad, WIa, Crem, run_null>::go(xa, run_null()));
            }
        }
    }
};



}; // thls

#endif
