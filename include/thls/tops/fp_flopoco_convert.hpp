#ifndef thls_fp_flopoco_convert_hpp
#define thls_fp_flopoco_convert_hpp

#include "thls/tops/fp_flopoco.hpp"
#include "thls/tops/fp_promote.hpp"
#include "thls/tops/fp_convert.hpp"

namespace thls
{

template<int wER,int wFR, int wEX,int wFX>
THLS_INLINE fp_flopoco<wER,wFR> convert(const fp_flopoco<wEX,wFX> &x, int DEBUG)
{
    fp_flopoco<wER,wFR> res;
    convert(res, x);
    return res;
}

}; // thls

#endif
