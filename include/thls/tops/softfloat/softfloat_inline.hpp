#ifndef softfloat_inline_hpp
#define softfloat_inline_hpp

#include "src/platform.h"

#include <assert.h>


INLINE uint_fast8_t softfloat_countLeadingZeros32( uint32_t a )
{
    if(a==0){
        return 32;
    }else{
        return __builtin_clz (a);
    }
}
#define softfloat_countLeadingZeros32 softfloat_countLeadingZeros32

INLINE uint_fast8_t softfloat_countLeadingZeros64( uint64_t a )
{
    if(a==0){
        return 64;
    }else{
        return __builtin_clzl (a);
    }
}
#define softfloat_countLeadingZeros64 softfloat_countLeadingZeros64


void softfloat_mul64To128M( uint64_t a, uint64_t b, uint32_t *zPtr )
{
    auto ab=((unsigned __int128)(a)) * ((unsigned __int128)(b));
    zPtr[0]=uint32_t(ab);
    zPtr[1]=uint32_t(ab>>32);
    zPtr[2]=uint32_t(ab>>64);
    zPtr[3]=uint32_t(ab>>96);
}
#define softfloat_mul64To128M softfloat_mul64To128M

#include "src/f32_add.c"
#include "src/f32_mul.c"
#include "src/f32_div.c"
#include "src/f32_eq.c"
#include "src/f32_le.c"
#include "src/f32_lt.c"

#include "src/f64_add.c"
#include "src/f64_mul.c"
#include "src/f64_div.c"
#include "src/f64_eq.c"
#include "src/f64_le.c"
#include "src/f64_lt.c"

#include "src/s_addMagsF32.c"
#include "src/s_subMagsF32.c"
#include "src/s_normSubnormalF32Sig.c"
#include "src/s_propagateNaNF32UI.c"
#include "src/s_roundPackToF32.c"
#include "src/s_normRoundPackToF32.c"

#include "src/s_addMagsF64.c"
#include "src/s_subMagsF64.c"
#include "src/s_normSubnormalF64Sig.c"
#include "src/s_propagateNaNF64UI.c"
#include "src/s_roundPackToF64.c"
#include "src/s_normRoundPackToF64.c"

//#include "src/s_shortShiftRightJam64.c"
//#include "src/s_shiftRightJam32.c"
//#include "src/s_countLeadingZeros32.c"
#include "src/s_countLeadingZeros8.c"
#include "src/softfloat_raiseFlags.c"

#include "src/s_approxRecip32_1.c"

#endif