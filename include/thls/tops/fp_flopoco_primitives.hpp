#ifndef fp_flopoco_hardware_primitives_hpp
#define fp_flopoco_hardware_primitives_hpp

#include "fp_flopoco.hpp"

#include "thls/tops/fp_flopoco_add_single_v1.hpp"
#include "thls/tops/fp_flopoco_mul_v1.hpp"
#include "thls/tops/fp_flopoco_add_single_v1.hpp"
#include "thls/tops/fp_flopoco_div_v1.hpp"
#include "thls/tops/fp_flopoco_neg_v1.hpp"
#include "thls/tops/fp_flopoco_inv_v1.hpp"
#include "thls/tops/fp_flopoco_neg_v1.hpp"
#include "thls/tops/fp_flopoco_convert.hpp"

namespace thls{

template<int wER,int wFR, int wEX,int wFX,int wEY,int wFY>
THLS_INLINE fp_flopoco<wER,wFR> add(const fp_flopoco<wEX,wFX> &x, const fp_flopoco<wEY,wFY> &y, int DEBUG)
{
    return add_small<wER,wFR>(x,y,DEBUG);
}

template<int wER,int wFR, int wEX,int wFX,int wEY,int wFY>
THLS_INLINE fp_flopoco<wER,wFR> mul(const fp_flopoco<wEX,wFX> &x, const fp_flopoco<wEY,wFY> &y, int DEBUG)
{
    return mul_v1<wER,wFR>(x,y,DEBUG);
}

template<int wER,int wFR, int wEX,int wFX,int wEY,int wFY>
THLS_INLINE fp_flopoco<wER,wFR> square(const fp_flopoco<wEX,wFX> &x, int DEBUG)
{
    return mul_v1<wER,wFR>(x,x,DEBUG);
}

template<int wER,int wFR, int wEX,int wFX,int wEY,int wFY>
THLS_INLINE fp_flopoco<wER,wFR> div(const fp_flopoco<wEX,wFX> &x, const fp_flopoco<wEY,wFY> &y, int DEBUG)
{
    return div_v1<wER,wFR>(x,y,DEBUG);
}

template<int wER,int wFR, int wEX,int wFX>
THLS_INLINE fp_flopoco<wER,wFR> inv(const fp_flopoco<wEX,wFX> &x, int DEBUG)
{
    return inv_v1<wER,wFR>(x,DEBUG);
}

template<int wER,int wFR, int PLACES, int wEX,int wFX>
THLS_INLINE fp_flopoco<wER,wFR> ldexp_const(const fp_flopoco<wEX,wFX> &x, int DEBUG);

template<int wER,int wFR, int wEX,int wFX>
THLS_INLINE fp_flopoco<wER,wFR> neg(const fp_flopoco<wEX,wFX> &x, int DEBUG)
{
    return neg_v1<wER,wFR>(x);
}

// This is just implemented directly in the header for now.
/*template<int wER,int wFR, int wEX,int wFX>
THLS_INLINE fp_flopoco<wER,wFR> convert(const fp_flopoco<wEX,wFX> &x, int DEBUG=0);
*/

}; // namespace thls

#endif
