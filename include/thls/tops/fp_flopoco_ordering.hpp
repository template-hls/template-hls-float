#ifndef thls_fp_flopoco_ordering_hpp
#define thls_fp_flopoco_ordering_hpp

#include "fp_flopoco.hpp"
#include "fp_promote.hpp"

namespace thls
{


//! Does an IEEE style comparison
/*! \note This considers nans to be uncomparable
*/
template<int wEA,int wFA,int wEB,int wFB>
fw_uint<1> equal(const fp_flopoco<wEA,wFA> &a, const fp_flopoco<wEB,wFB> &b)
{
    static const int wE=thls_ctMax(wEA,wEB);
    static const int wF=thls_ctMax(wFA,wFB);
    
    fp_flopoco<wE,wF> na, nb;
    promote(na,a);
    promote(nb,b);
    
    return select(
        na.is_nan() | nb.is_nan(),
            zg<1>(), // Nan not equal to anything
        na.get_flags() != nb.get_flags(),
            zg<1>(), // Flags must be the same 
        na.is_zero(),
            og<1>(), // Any two zeros are the same
        // else
            na.get_sign()==nb.get_sign() && na.get_exp_frac_bits()==nb.get_exp_frac_bits()
    );
}

template<int wEA,int wFA,int wEB,int wFB>
fw_uint<1> not_equal(const fp_flopoco<wEA,wFA> &a, const fp_flopoco<wEB,wFB> &b)
{
    static const int wE=thls_ctMax(wEA,wEB);
    static const int wF=thls_ctMax(wFA,wFB);

    fp_flopoco<wE,wF> na, nb;
    promote(na,a);
    promote(nb,b);

    return select(
        na.is_nan() | nb.is_nan(),
            og<1>(), // Nan not equal to everything
        na.get_flags() != nb.get_flags(),
            og<1>(), // Flags must be the same
        na.is_zero(),
            zg<1>(), // Any two zeros are the same
        // else
            na.get_sign() != nb.get_sign() || na.get_exp_frac_bits()!=nb.get_exp_frac_bits()
    );
}



//! Does an IEEE style comparison
/*! \note This considers nans to be uncomparable
*/
template<int wEA,int wFA,int wEB,int wFB>
fw_uint<1> less_than(const fp_flopoco<wEA,wFA> &a, const fp_flopoco<wEB,wFB> &b)
{
    static const int wE=thls_ctMax(wEA,wEB);
    static const int wF=thls_ctMax(wFA,wFB);

    fp_flopoco<wE,wF> na, nb;
    promote(na,a);
    promote(nb,b);

    return select(
            (na.is_nan() | nb.is_nan()),
                zg<1>(),  // Any nan, return false
            (na.is_zero() & nb.is_zero()),
                zg<1>(),  // Zeros are equal
            // Pre: neither is nan, both are not zero
            na.is_negative() ^ nb.is_negative(),
                na.is_negative(), // If different signs, first must be negative for less than
            // Pre: not nan, not _both_ zero, same sign
            na.is_negative(),
                na.get_flags_exp_frac_bits()>nb.get_flags_exp_frac_bits(),
            // na.is_positive()
                na.get_flags_exp_frac_bits()<nb.get_flags_exp_frac_bits()
    );
}

//! Does an IEEE style comparison
/*! \note This considers nans to be uncomparable
*/
template<int wEA,int wFA,int wEB,int wFB>
fw_uint<1> less_than_equal(const fp_flopoco<wEA,wFA> &a, const fp_flopoco<wEB,wFB> &b)
{
    static const int wE=thls_ctMax(wEA,wEB);
    static const int wF=thls_ctMax(wFA,wFB);

    fp_flopoco<wE,wF> na, nb;
    promote(na,a);
    promote(nb,b);

    return select(
        (na.is_nan() | nb.is_nan()),
            zg<1>(),  // Any nan, return false
        (na.is_zero() & nb.is_zero()),
            og<1>(),  // Zeros are equal
        // Pre: neither is nan, both are not zero
        na.is_negative() ^ nb.is_negative(),
            na.is_negative(), // If different signs, first must be negative for less than equal
        // Pre: not nan, not _both_ zero, same sign
        na.is_negative(),
            na.get_flags_exp_frac_bits()>=nb.get_flags_exp_frac_bits(),
        // na.is_positive()
            na.get_flags_exp_frac_bits()<=nb.get_flags_exp_frac_bits()
    );
}

template<int wEA,int wFA,int wEB,int wFB>
fw_uint<1> greater_than(const fp_flopoco<wEA,wFA> &a, const fp_flopoco<wEB,wFB> &b)
{
    static const int wE=thls_ctMax(wEA,wEB);
    static const int wF=thls_ctMax(wFA,wFB);

    fp_flopoco<wE,wF> na, nb;
    promote(na,a);
    promote(nb,b);

    return select(
            (na.is_nan() | nb.is_nan()),
                zg<1>(),  // Any nan, return false
            (na.is_zero() & nb.is_zero()),
                zg<1>(),  // Zeros are equal
            // Pre: neither is nan, both are not zero
            na.is_negative() ^ nb.is_negative(),
                na.is_positive(), // If different signs, first must be negative for greater than
            // Pre: not nan, not _both_ zero, same sign
            na.is_negative(),
                na.get_flags_exp_frac_bits()<nb.get_flags_exp_frac_bits(),
            // na.is_positive()
                na.get_flags_exp_frac_bits()>nb.get_flags_exp_frac_bits()
    );
}

template<int wEA,int wFA,int wEB,int wFB>
fw_uint<1> greater_than_equal(const fp_flopoco<wEA,wFA> &a, const fp_flopoco<wEB,wFB> &b)
{
    static const int wE=thls_ctMax(wEA,wEB);
    static const int wF=thls_ctMax(wFA,wFB);

    fp_flopoco<wE,wF> na, nb;
    promote(na,a);
    promote(nb,b);

    return select(
            (na.is_nan() | nb.is_nan()),
                zg<1>(),  // Any nan, return false
            (na.is_zero() & nb.is_zero()),
                og<1>(),  // Zeros are equal
            // Pre: neither is nan, both are not zero
            na.is_negative() ^ nb.is_negative(),
                na.is_positive(), // If different signs, first must be positive for greater than equal
            // Pre: not nan, not _both_ zero, same sign
            na.is_negative(),
                na.get_flags_exp_frac_bits()<=nb.get_flags_exp_frac_bits(),
            // na.is_positive()
              na.get_flags_exp_frac_bits()>=nb.get_flags_exp_frac_bits()
    );
}

}; // thls

#endif
