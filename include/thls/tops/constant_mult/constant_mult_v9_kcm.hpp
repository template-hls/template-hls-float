#ifndef thls_constant_mult_v9_kcm_hpp
#define thls_constant_mult_v9_kcm_hpp

#include "thls/tops/fw_uint.hpp"

#include <random>

namespace thls {

template<unsigned K, unsigned WR, uint64_t C>
struct kcm_lookup_table_switch;

template<unsigned WR, uint64_t C>
struct kcm_lookup_table_switch<1, WR, C>
{
    static_assert(WR<=64, "There is a limit on the max width of this method.");

    static fw_uint<WR> go(const fw_uint<1> &x)
    {
        using bits_t = typename fw_uint<WR>::bits_t;
        const bits_t M= bits_t( (WR==0) ? 0ull : (0xFFFFFFFFFFFFFFFFull>>(64-WR)) );
        bits_t val;
        switch(x.to_int()){
        case 0: val = (0 * C) & M; break;
        case 1: val = (1 * C) & M; break;
        }
        return fw_uint<WR>::from_bits(val & M);
    }
};

template<unsigned WR, uint64_t C>
struct kcm_lookup_table_switch<2, WR, C>
{
    static_assert(WR<=64, "There is a limit on the max width of this method.");

    static fw_uint<WR> go(const fw_uint<2> &x)
    {
        using bits_t = typename fw_uint<WR>::bits_t;
        const bits_t M= bits_t( (WR==0) ? 0ull : (0xFFFFFFFFFFFFFFFFull>>(64-WR)) );
        bits_t val;
        switch(x.to_int()){
        case 0: val = (0 * C) & M; break;
        case 1: val = (1 * C) & M; break;
        case 2: val = (2 * C) & M; break;
        case 3: val = (3 * C) & M; break;
        }
        return fw_uint<WR>::from_bits(val & M);
    }
};

template<unsigned WR, uint64_t C>
struct kcm_lookup_table_switch<3, WR, C>
{
    static_assert(WR<=64, "There is a limit on the max width of this method.");

    static fw_uint<WR> go(const fw_uint<3> &x)
    {
        using bits_t = typename fw_uint<WR>::bits_t;
        const bits_t M= bits_t( (WR==0) ? 0ull : (0xFFFFFFFFFFFFFFFFull>>(64-WR)) );
        bits_t val;
        switch(x.to_int()){
        case 0: val = (0 * C) & M; break;
        case 1: val = (1 * C) & M; break;
        case 2: val = (2 * C) & M; break;
        case 3: val = (3 * C) & M; break;
        case 4: val = (4 * C) & M; break;
        case 5: val = (5 * C) & M; break;
        case 6: val = (6 * C) & M; break;
        case 7: val = (7 * C) & M; break;
        }
        return fw_uint<WR>::from_bits(val & M);
    }
};

template<unsigned WR, uint64_t C>
struct kcm_lookup_table_switch<4, WR, C>
{
    static_assert(WR<=64, "There is a limit on the max width of this method.");

    static fw_uint<WR> go(const fw_uint<3> &x)
    {
        using bits_t = typename fw_uint<WR>::bits_t;
        const bits_t M= bits_t( (WR==0) ? 0ull : (0xFFFFFFFFFFFFFFFFull>>(64-WR)) );
        bits_t val;
        switch(x.to_int()){
        case 0: val = (0 * C) & M; break;
        case 1: val = (1 * C) & M; break;
        case 2: val = (2 * C) & M; break;
        case 3: val = (3 * C) & M; break;
        case 4: val = (4 * C) & M; break;
        case 5: val = (5 * C) & M; break;
        case 6: val = (6 * C) & M; break;
        case 7: val = (7 * C) & M; break;
        case 8: val = (8 * C) & M; break;
        case 9: val = (9 * C) & M; break;
        case 10: val = (10 * C) & M; break;
        case 11: val = (11 * C) & M; break;
        case 12: val = (12 * C) & M; break;
        case 13: val = (13 * C) & M; break;
        case 14: val = (14 * C) & M; break;
        case 15: val = (15 * C) & M; break;
        }
        return fw_uint<WR>::from_bits(val & M);
    }
};


template<const unsigned K>
struct cmult_impl_v9_kcm
{
    static constexpr const char *description=
"Recursive KCM implementation. Uses switch-based lookup table because... reasons.";

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

        THLS_INLINE static fw_uint<WR> go(const fw_uint<WI> &x) {
            fw_uint<WR> curr=kcm_lookup_table_switch<WR,WIcurr,C>::go(take_lsbs<WIcurr>(x));
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

using cmult_impl_v9_kcm_k1 = cmult_impl_v8_kcm<1>;
using cmult_impl_v9_kcm_k2 = cmult_impl_v8_kcm<2>;

}; // thls

#endif
