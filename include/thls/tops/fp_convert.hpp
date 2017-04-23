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


template<int ER,int FR,int EX,int FX>
void convert(fp_flopoco<ER,FR> &dst, const fp_flopoco<EX,FX> &src)
{
    fw_uint<2> rFlags=src.get_flags();
    fw_uint<1> rSign=src.get_sign();


    fw_uint<FX> xFrac=src.get_frac_bits();
    fw_uint<FR> rFrac;
    fw_uint<1> fracWrappedUp=fw_uint<1>(0);

    if(FR > FX) {
        rFrac = checked_cast<FR>( zpad_lo<FR-FX>(xFrac) );
    }else if(FR==FX){
        rFrac = checked_cast<FR>( xFrac );
    }else {
        if (rFlags.to_int() == 0b01) {
            auto keep = take_msbs<FR>(xFrac);
            auto drop = take_lsbs<FX - FR>(xFrac);

            /*
             *   kkk|d
             *
             *   kkk|0    -> kkk+0
             *   kk0|1    -> kkk+0
             *   kk1|1    -> kkk+1
             *
             *   kkk|dd
             *
             *   kkk|00    -> kkk+0     x.00 -> x
             *   kkk|01    -> kkk+0     x.25 -> x
             *   kk0|10    -> kkk+0     x.50 -> x if even(x)
             *   kk1|10    -> kkk+1     x.50 -> x+1 if odd(x)
             *   kkk|11    -> kkk+1     x.75 -> x+1
             *
             *   kkk|0    -> kkk+0
             *   kk0|1    -> kkk+0
             *   kk1|1    -> kkk+1
             *
             *
             *   kkk|ddd
             *
             *   kkk|011  -> kkk+0
             *   kk0|100  -> kkk+0
             *   kk1|100  -> kkk+1
             *   kkk|101  -> kkk+1
             *
             *   up= isDropOverHalf | (isDropHalf & isKeepOdd)
             */

            const auto HALF = concat(og<1>(), zg<(FX - FR) - 1>());
            auto isDropOverHalf = drop > HALF;
            auto isDropHalf = drop == HALF;
            auto isKeepOdd = take_lsbs<1>(keep);

            auto roundUp = isDropOverHalf | (isDropHalf & isKeepOdd);

            rFrac = keep+extu<FR>(roundUp);

            auto isKeepOnes = (keep == og<FR>());
            fracWrappedUp=(roundUp & isKeepOnes);
        }
    }

    fw_uint<EX> xExp=src.get_exp_bits();
    fw_uint<ER> rExp;

    if(ER > EX ){
        // Impossible for overflow to happen, even with fracWrapped Up
        const int ADJ = (1<<(ER-1)) - (1<<(EX-1));
        rExp = checked_cast<ER>(zpad_hi<ER-EX>(xExp)) + fw_uint<ER>(ADJ) + zpad_hi<ER-1>(fracWrappedUp);
    }else if(ER == EX){
        if( (fracWrappedUp & (checked_cast<ER>(xExp)==og<ER>())).to_bool() ){
            rFlags=fw_uint<2>(0b10); // infinity
        }
        rExp = checked_cast<ER>( xExp ) + zpad_hi<ER-1>(fracWrappedUp);
    }else{
        const int ADJ = (1<<(EX-1)) - (1<<(ER-1)); // positive amount to subtract off
        const int MIN_EXP = ADJ;
        const int MAX_EXP = ADJ + (1<<ER) -1;

        auto aExp = xExp + zpad_hi<EX-1>(fracWrappedUp);

        auto expWrapped = ((aExp==zg<EX>()) & fracWrappedUp);

        auto underflow = ~expWrapped & (aExp < fw_uint<EX>(MIN_EXP));   // Need to handle the case where fracWrappedUp caused a wrap to zero
        auto overflow = expWrapped | (aExp > fw_uint<EX>(MAX_EXP));
        auto tExp = take_lsbs<ER>( aExp - fw_uint<EX>( ADJ ));

        if (rFlags.to_int() == 0b01) {
            if(underflow.to_bool()){
                rFlags=fw_uint<2>(0b00);
            }else if(overflow.to_bool()){
                rFlags=fw_uint<2>(0b10);
            }
        }
        rExp = tExp;
    }



    dst = fp_flopoco<ER, FR>(
            rFlags,
            rSign,
            rExp,
            rFrac
    );
}

    template<int ER,int FR,int EX,int FX>
    void ref_convert(fp_flopoco<ER,FR> &dst, const fp_flopoco<EX,FX> &src)
    {
        mpfr_t ma, mr;
        mpfr_init2(ma,FX+1);
        mpfr_init2(mr,FR+1);

        src.get(ma);

        mpfr_set(mr,ma,MPFR_RNDN);

        fp_flopoco<ER,FR> res(mr,true);

        mpfr_clear(ma);
        mpfr_clear(mr);

        dst=res;
    };

    template<int ER,int FR,int EX,int FX>
    void flopoco_convert(fp_flopoco<ER,FR> &dst, const fp_flopoco<EX,FX> &src)
    {
        convert(dst,src);
    };


}; // thls

#endif
