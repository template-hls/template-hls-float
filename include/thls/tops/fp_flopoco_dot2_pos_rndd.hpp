#ifndef dot2_pos_hpp
#define dot2_pos_hpp

#include "fp_flopoco.hpp"

namespace thls
{

/*
    We are doing x.y = sum(x[i]*y[i], i=0..n-1) where all x and y are positive

    Each term x[i]*y[i] has an msb of e(x[i])+e(y[i]) or overflows to e(x[i])+e(y[i])+1

    The largest values comes when all the terms have the same exponent,
    and they all overflow:
    max(e(x[i])+e(y[i])+1+log2ceil(n))

    The min value comes when the largest value determines the result exponent directly:
    max(e(x[i])+e(y[i]))

    So the maximum post addition shift is 1+log2ceil(n):
    - n=2 -> 2 bits,
    - n=3 -> 3 bits
    - n=4 -> 3 bits
    - ...
    - n=8 -> 4 bits  (i.e. one 6-lut)

    We are simplifying here to n=2, and all types are the same.

    So our goal is:
    - size two dot product
    - Input and output types all the same
    - Round to -inf

    We've assumed both input vectors have the same WF, and we'll take 
    WF=4 for diagrams. Here f just means fractional bits, not hex.

    Input format:

    1.ffff

    Product format has two bits due to possible overflow, WAB=WF+WF

    0   -4  -8=WAB=WF*2
    |    |   |
    1.ffff   |
     *       |
    1.ffff   |
    =        |
   ??.ffffffff

    The largest possible value is when the two exponents are equal:

    ??.ffffffff
      +
    ??.ffffffff
      =
   ???.ffffffff

    The smallest minor value that could matter is when the large value
    is as small as possible (no overflow), 

      0   -4  -8=-WAB
      |    |   |
     01.ffffffff
      +
                ??.ffffffff
      +         
                1
      =
     ??.ffffffff

    So we need to form the partial sum with WAB+1 fractional bits.


*/
template<int WE, int WF, bool log=false>
fp_flopoco<WE,WF> dot2_pos_rndd(
    const fp_flopoco<WE,WF> &a0,
    const fp_flopoco<WE,WF> &a1,
    const fp_flopoco<WE,WF> &b0,
    const fp_flopoco<WE,WF> &b1
){

    assert( (a0.is_positive() & a0.is_normal()).to_bool() );
    assert( (a1.is_positive() & a1.is_normal()).to_bool() );
    assert( (b0.is_positive() & b0.is_normal()).to_bool() );
    assert( (b1.is_positive() & b1.is_normal()).to_bool() );

    if(log){
        fprintf(stderr, "dot2_pos_rndd(%g,%g,%g,%g)\n", a0.to_double(), a1.to_double(), b0.to_double(), b1.to_double());
    }

    auto fraca0=opad_hi<1>(a0.get_frac_bits());
    auto fraca1=opad_hi<1>(a1.get_frac_bits());
    auto fracb0=opad_hi<1>(b0.get_frac_bits());
    auto fracb1=opad_hi<1>(b1.get_frac_bits());

    const int bias( (1<<(WE-1))-1 );

    // Form the full-width products, as we don't know which is bigger yet
    auto ab0_frac=fraca0*fracb0;
    auto ab0_exp=zpad_hi<1>(a0.get_exp_bits())+zpad_hi<1>(b0.get_exp_bits());
    auto ab1_frac=fraca1*fracb1;
    auto ab1_exp=zpad_hi<1>(a1.get_exp_bits())+zpad_hi<1>(b1.get_exp_bits());

    if(log){
        std::cerr<<"  ab0_frac="<<ab0_frac<<", ab1_frac="<<ab1_frac<<"\n";
    }

    // Swap the full width products and exponents to get the biggest one
    auto swap = ab1_exp > ab0_exp;
    auto maj_frac=select( swap , ab1_frac , ab0_frac );
    auto maj_exp=select( swap , ab1_exp , ab0_exp );
    auto min_frac=select( swap , ab0_frac , ab1_frac );
    auto min_exp=select( swap , ab0_exp , ab1_exp );
    auto shift=maj_exp-min_exp;

    if(log){
        std::cerr<<"  maj_frac="<<maj_frac<<", min_frac="<<min_frac<<"\n";
    }

    auto maj_frac_full=concat(zg<1>(), maj_frac, zg<1>());
    auto min_frac_full=concat(zg<1>(), min_frac, zg<1>());
    bool shift_loss = (shift >= min_frac_full.width).to_bool();

    auto min_frac_shifted= shift_loss ? zg<min_frac_full.width>() : (min_frac_full >> shift.to_int());

    if(log){
        std::cerr<<"  maj_frac_full="<<maj_frac_full<<", min_frac_shifted="<<min_frac_shifted<<"\n";
    }

    auto frac_full=maj_frac_full+min_frac_shifted;
    if(log){
        std::cerr<<"  frac_full="<<frac_full<<"\n";
        std::cerr<<"  value = "<<ldexp( frac_full.to_uint64(), maj_exp.to_int()-2*bias-2*WE-1 );
    }

    // At this point we have  ???.ffffffff  where there is
    // guaranteed to be a 1 somewhere in the top three bits.
    auto top_three=take_msbs<3>(frac_full);
    assert(top_three.to_int() != 0);

    // We are rounding down, so just dump all the lower parts before shifting
    auto frac_pre_norm=take_msbs<3+WF>(frac_full);  

    // We need to renormalise, so possibilities are:
    //  1??.ffffffff
    //  01?.ffffffff
    //  001.ffffffff
    fw_uint<WF> frac_post_norm;
    fw_uint<WE+2> exp_post_norm;
    fw_uint<WE+2> exp_post_norm_adj;
    if(get_bits<2,2>(top_three).to_bool()){
        frac_post_norm=get_bits<1+WF,2>(frac_pre_norm);
        exp_post_norm_adj=fw_uint<2+WE>( + 2);
    }else if(get_bits<1,1>(top_three).to_bool()){
        frac_post_norm=get_bits<WF,1>(frac_pre_norm);
        exp_post_norm_adj=fw_uint<2+WE>( + 1);
    }else{
        frac_post_norm=get_bits<WF-1,0>(frac_pre_norm);
        exp_post_norm_adj=fw_uint<2+WE>( + 0);
    }
    exp_post_norm=zpad_hi<1>(maj_exp) + exp_post_norm_adj;

    if(log){
        std::cerr<<"  frac_post_norm="<<frac_post_norm<<"\n";
    }

    // Final conversion back
    fw_uint<2> flags(0b01);
    if( (exp_post_norm < bias).to_bool() ){ 
        flags=fw_uint<2>(0b00);// underflow to zero
    }else if( (exp_post_norm >= bias+(1<<WE) ).to_bool() ){
        flags=fw_uint<2>(0b10); // overflow to inf
    }

    return fp_flopoco<WE,WF>(flags, zg<1>(), take_lsbs<WE>(exp_post_norm-bias), frac_post_norm );
}

template<int WE, int WF>
fp_flopoco<WE,WF> dot2_pos_rndd_ref_large(
    const fp_flopoco<WE,WF> &a0,
    const fp_flopoco<WE,WF> &a1,
    const fp_flopoco<WE,WF> &b0,
    const fp_flopoco<WE,WF> &b1
){
    assert( (a0.is_positive() & a0.is_normal()).to_bool() );
    assert( (a1.is_positive() & a1.is_normal()).to_bool() );
    assert( (b0.is_positive() & b0.is_normal()).to_bool() );
    assert( (b1.is_positive() & b1.is_normal()).to_bool() );


    MPFR_DECL_INIT(va0, WF+1);
    MPFR_DECL_INIT(va1, WF+1);
    MPFR_DECL_INIT(vb0, WF+1);
    MPFR_DECL_INIT(vb1, WF+1);
    MPFR_DECL_INIT(vab0, WF+WF+2);
    MPFR_DECL_INIT(vab1, WF+WF+2);
    MPFR_DECL_INIT(vr, WF+1);

    a0.get(va0);
    a1.get(va1);
    b0.get(vb0);
    b1.get(vb1);

    mpfr_mul(vab0, va0, vb0, MPFR_RNDN); // Both of these should be exact
    mpfr_mul(vab1, va1, vb1, MPFR_RNDN);

    mpfr_add(vr, vab0, vab1, MPFR_RNDD); // Does the single rounding operaton

    return fp_flopoco<WE,WF>(vr, true);
}

template<int WE, int WF>
fp_flopoco<WE,WF> dot2_pos_rndd_ref_small(
    const fp_flopoco<WE,WF> &a0,
    const fp_flopoco<WE,WF> &a1,
    const fp_flopoco<WE,WF> &b0,
    const fp_flopoco<WE,WF> &b1
){
    if((WF+1)*4 > 52){
        throw std::runtime_error("Types are too wide to reliably do in double.");
    }

    double dres=a0.to_double()*b0.to_double() + a1.to_double()*b1.to_double();

    MPFR_DECL_INIT(vres, WF+1);
    mpfr_set_d(vres, dres, MPFR_RNDD);

    return fp_flopoco<WE,WF>(vres, true);
}

template<int WE, int WF>
fp_flopoco<WE,WF> dot2_pos_rndd_ref(
    const fp_flopoco<WE,WF> &a0,
    const fp_flopoco<WE,WF> &a1,
    const fp_flopoco<WE,WF> &b0,
    const fp_flopoco<WE,WF> &b1
){
    if((WF+1)*4 < 52){
        return dot2_pos_rndd_ref_small(a0,a1,b0,b1);
    }else{
        return dot2_pos_rndd_ref_large(a0,a1,b0,b1);
    }
}

template<int WE, int WF>
void test_dot2_pos_rndd()
{
    using fp_t = fp_flopoco<WE,WF>;
    using traits=std::numeric_limits<fp_t>;

    fp_t x0, x1, x2, x3;

    fp_t upper=traits::max();

    for(x0=traits::min(); x0.is_normal().to_bool(); x0=nextup(x0)){
        std::cerr<<"x0 = "<<x0<<"\n";
        for(x1=traits::min(); x1.is_normal().to_bool(); x1=nextup(x1)){
            //std::cerr<<"  x1 = "<<x1<<"\n";
            for(x2=traits::min(); x2.is_normal().to_bool(); x2=nextup(x2)){
                for(x3=traits::min(); x3.is_normal().to_bool(); x3=nextup(x3)){
                    auto ref=dot2_pos_rndd_ref(x0,x1,x2,x3);
                    auto got=dot2_pos_rndd(x0,x1,x2,x3);
                    //fprintf(stderr, "  ref=%lg, got=%lg\n", ref.to_double(), got.to_double());
                    if(ref.to_double() != got.to_double()){
                        fprintf(stderr, "  ERROR : ref=%lg, got=%lg\n", ref.to_double(), got.to_double());
                        exit(1);
                    }
                }
            }
        }
    }
}

}; // thls

#endif
