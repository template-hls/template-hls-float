#ifndef thls_fp_flopoco_neg_v1_hpp
#define thls_fp_flopoco_neg_v1_hpp

#include "thls/tops/fp_flopoco.hpp"
#include "thls/tops/fp_promote.hpp"

namespace thls
{

template<int wER,int wFR, int wEX,int wFX>
THLS_INLINE fp_flopoco<wER,wFR> neg_v1(const fp_flopoco<wEX,wFX> &x, int DEBUG=0)
{
    auto res=fp_flopoco<wEX,wFX>(concat(
        x.get_flags(),
        ~x.get_sign(),
        x.get_exp_bits(),
        x.get_frac_bits()
    ));
    return thls::convert<wER,wFR>(res);
}

}; // thls

#endif
