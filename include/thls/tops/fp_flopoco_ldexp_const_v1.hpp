#ifndef thls_fp_flopoco_ldexp_const_v1_hpp
#define thls_fp_flopoco_ldexp_const_v1_hpp

#include "thls/tops/fp_flopoco.hpp"
#include "thls/tops/fp_promote.hpp"

namespace thls
{

template<int wER,int wFR, int PLACES, int wEX,int wFX>
THLS_INLINE fp_flopoco<wER,wFR> ldexp_const_v1(const fp_flopoco<wEX,wFX> &x, int DEBUG=0)
{
    static_assert(PLACES < (1<<wEX) || -PLACES < (1<<wEX), "Shift looks too big to be intentional.");

    auto flags=x.get_flags();
    auto sign=x.get_sign();
    auto exp=x.get_exp();
    auto frac=x.get_frac_bits();

    exp = exp + fw_uint<wEX>(PLACES);
    if(flags==fw_uint<2>(0b01)){
        if((PLACES>0) && (exp > (og<wEX>()-PLACES)).to_bool()){
            flags=fw_uint<2>(0b10);
        }
        if( (PLACES>0) && (exp > (og<wEX>()-PLACES)).to_bool() ){
            
        }
}

}; // thls

#endif
