#ifndef thls_fp_convert_hpp
#define thls_fp_convert_hpp

#include "fp_flopoco.hpp"
#include "fp_ieee.hpp"

/*
    This does potentially lossy conversion between formats. For
    lossless conversions, look at fp_promote.

*/

namespace thls
{

/*! Only converts with the same format */
template<int E,int F>
void convert(fp_flopoco<E,F> &dst, const fp_ieee<E,F> &src)
{
    dst=fp_flopoco<E,F>(concat(
        src.get_flags(),
        src.get_sign(),
        src.get_exp_bits(),
        src.get_frac_bits()
    ));
}

/*! Only converts with the same format */
template<int E,int F>
void convert(fp_ieee<E,F> &dst, const fp_flopoco<E,F> &src)
{
    auto over=src.is_normal() && src.get_exp_bits()==og<E>();
    auto under=src.is_normal() && src.get_exp_bits()==zg<E>();
    dst=fp_ieee<E,F>(concat(
        src.get_sign(),
        select(
            src.is_zero() | under,
                zg<E>(),
            src.is_nan() | src.is_inf() | over,
                og<E>(),
            // else
                src.get_exp_bits()
        ),
        select(
            src.is_zero() | src.is_inf() | over | under,
                zg<F>(),
            src.is_nan(),
                og<F>(),
            // else
                src.get_frac_bits()
        )
    ));
}


}; // thls

#endif
