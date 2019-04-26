#include "HLS/hls.h"
#include "thls/thls_float.hpp"

const int E=8;
const int F=23;
using fp_t = thls::fp_flopoco<E,F>;

component fp_t dot_product(fp_t x0, fp_t x1, fp_t y0, fp_t y1)
{
    return thls::add<E,F>( thls::mul<E,F>(x0, y0), thls::mul<E,F>(x1,y1) );
}
