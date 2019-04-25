#ifndef dot2_pos_hpp
#define dot2_pos_hpp

#include "fp_flopoco.hpp"

#include "thls/tops/function_utils.hpp"

namespace thls
{

template<int O=0,int W>
fw_uint<W> rotate4(fw_uint<2> index, const fw_uint<W> &x0, const fw_uint<W> &x1, const fw_uint<W> &x2, const fw_uint<W> &x3 )
{
    switch(index.to_int()){
    default: assert(0);
    case (0+O)%4: return x0;
    case (1+O)%4: return x1;
    case (2+O)%4: return x2;
    case (3+O)%4: return x3;
    }
}

template<int W, int WS>
fw_uint<W> safe_rshift(const fw_uint<W> &val, const fw_uint<WS> &places)
{
    int p=places.to_int();
    return p>=W ? zg<W>() : val>>p;
}

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

    We are simplifying here to n=4, and all types are the same.

    So our goal is:
    - size four dot product
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

    sum:
    ??.ffffffff
    ??.ffffffff
    ??.ffffffff
    ??.ffffffff
      =
  ????.ffffffff

    The smallest minor value that could matter is when the large value
    is as small as possible (no overflow), 

      0   -4  -8=-WAB -17=-2*WAB-1 -26=-3*WAB-2
      |    |   |         |         |
     01.11111111         |         |
      +                  |         |
               01.11111111         |
      +                            |
                         01.11111111
      +
                                  01.11111111
      =
     ??.ffffffff

    So we need to form the partial sum with 3*WAB+2 fractional bits.

    This is pessimistic, as it's not actually possible to get
    an all ones pattern as part of a product. The largest possible
    pattern is all ones * all ones, but that actually has WF zeros
    in the middle of it.

    If we consider a particular pattern p, then we can only achieve
    it if there exist two integers 0 <= x <= y <2^WF
    that p=(2^WF+x)*(2^WF+y).
    Which means... nothing.

    We know that p will always have a leading 1 in one of the two
    leading positions, so the question then becomes:
    what is the longest run of leading zeros in any product p?
    It's probably easiest to split it into 2^(2*WF) <= p < 2^(2*WF+1)
    and 2^(2*WF+1) <= p < w^(2*WF+2)

    We want to find a number 1<=r<WF, such that forall p
    bit(2*WF-1)=0 \/ bit(2*WF-2)=0 \/ ... \/ bit(2*WF-r)=0
    or
    hamming( p[2*WF-1..w*WF-r] ) < r

    Bleh. Goes away. Sets it up in z3...

    Ok, so the longest run of ones is actually 2*WF, regardless
    of whether it is in the overflow or non-overflow case.

    That gives us:

     0    -4 -7=-WAB+1 -15=-2*WAB+1  -23=-3*WAB+1
      |    |  |         /           /
     01.11111110       |          /
      +                |         /
              01.11111110       |
      +                         |
                       01.11111110
      +
                               01.11111111
      =
     ??.ffffffff

     So we need to keep 3*WAB-1 bits (still huge...)
*/
template<int WE, int WF, bool log=false>
fp_flopoco<WE,WF> dot4_pos_rndd(
    const fp_flopoco<WE,WF> &a0,
    const fp_flopoco<WE,WF> &a1,
    const fp_flopoco<WE,WF> &a2,
    const fp_flopoco<WE,WF> &a3,
    const fp_flopoco<WE,WF> &b0,
    const fp_flopoco<WE,WF> &b1,
    const fp_flopoco<WE,WF> &b2,
    const fp_flopoco<WE,WF> &b3
){

    assert( (a0.is_positive() & a0.is_normal()).to_bool() );
    assert( (a1.is_positive() & a1.is_normal()).to_bool() );
    assert( (a2.is_positive() & a2.is_normal()).to_bool() );
    assert( (a3.is_positive() & a3.is_normal()).to_bool() );
    assert( (b0.is_positive() & b0.is_normal()).to_bool() );
    assert( (b1.is_positive() & b1.is_normal()).to_bool() );
    assert( (b2.is_positive() & b2.is_normal()).to_bool() );
    assert( (b3.is_positive() & b3.is_normal()).to_bool() );

    if(log){
        fprintf(stderr, "dot2_pos_rndd(%g,%g,%g,%g,%g,%g,%g,%g)\n",
            a0.to_double(), a1.to_double(), a2.to_double(), a3.to_double(),
            b0.to_double(), b1.to_double(), b2.to_double(), b3.to_double());
    }

    auto fraca0=opad_hi<1>(a0.get_frac_bits());
    auto fraca1=opad_hi<1>(a1.get_frac_bits());
    auto fraca2=opad_hi<1>(a2.get_frac_bits());
    auto fraca3=opad_hi<1>(a3.get_frac_bits());
    auto fracb0=opad_hi<1>(b0.get_frac_bits());
    auto fracb1=opad_hi<1>(b1.get_frac_bits());
    auto fracb2=opad_hi<1>(b2.get_frac_bits());
    auto fracb3=opad_hi<1>(b3.get_frac_bits());

    const int bias( (1<<(WE-1))-1 );

    // Form the full-width products, as we don't know which is bigger yet
    auto ab0_frac=fraca0*fracb0;
    auto ab0_exp=zpad_hi<1>(a0.get_exp_bits())+zpad_hi<1>(b0.get_exp_bits());
    auto ab1_frac=fraca1*fracb1;
    auto ab1_exp=zpad_hi<1>(a1.get_exp_bits())+zpad_hi<1>(b1.get_exp_bits());
    auto ab2_frac=fraca2*fracb2;
    auto ab2_exp=zpad_hi<1>(a2.get_exp_bits())+zpad_hi<1>(b2.get_exp_bits());
    auto ab3_frac=fraca3*fracb3;
    auto ab3_exp=zpad_hi<1>(a3.get_exp_bits())+zpad_hi<1>(b3.get_exp_bits());

    if(log){
        std::cerr<<"  ab0_frac="<<ab0_frac<<", ab1_frac="<<ab1_frac<<"  ab2_frac="<<ab2_frac<<", ab3_frac="<<ab3_frac<<"\n";
    }

    /* DESIGN CHOICE:
        We can either work out the biggest exponent and then shift all of them,
        or we can work out the biggest exponent and then treat that one specially.
        If we select the biggest one, then we only need three shifters, but there
        is an extra layer of muxers.
        In this case the mux layer is only one LUT level, so it makes sense to
        special case the biggest one.
    */

    // We don't need to do a full sort here, we only need to select
    // the biggest one. So we'll work out the index of the biggest
    // one, then just rotate into place.
    auto bigger01 = ab1_exp > ab0_exp;
    auto bigger01_exp = select(bigger01, ab1_exp, ab0_exp);
    auto bigger23 = ab3_exp > ab2_exp;
    auto bigger23_exp = select(bigger23, ab3_exp, ab2_exp);

    auto bigger_01_23 = bigger23_exp > bigger01_exp;

    auto biggest_sel=concat(bigger_01_23, select(bigger_01_23, bigger23, bigger01) );

    // Rotate the full width products and exponents to get the biggest one
    auto x0_exp=rotate4<0>(biggest_sel, ab0_exp, ab1_exp, ab2_exp, ab3_exp);
    auto x1_exp=rotate4<1>(biggest_sel, ab0_exp, ab1_exp, ab2_exp, ab3_exp);
    auto x2_exp=rotate4<2>(biggest_sel, ab0_exp, ab1_exp, ab2_exp, ab3_exp);
    auto x3_exp=rotate4<3>(biggest_sel, ab0_exp, ab1_exp, ab2_exp, ab3_exp);
    auto x0_frac=rotate4<0>(biggest_sel, ab0_frac, ab1_frac, ab2_frac, ab3_frac);
    auto x1_frac=rotate4<1>(biggest_sel, ab0_frac, ab1_frac, ab2_frac, ab3_frac);
    auto x2_frac=rotate4<2>(biggest_sel, ab0_frac, ab1_frac, ab2_frac, ab3_frac);
    auto x3_frac=rotate4<3>(biggest_sel, ab0_frac, ab1_frac, ab2_frac, ab3_frac);

    auto shift1=x0_exp-x1_exp;
    auto shift2=x0_exp-x2_exp;
    auto shift3=x0_exp-x3_exp;

    const int WAB=2*WF;
    const int PAD_LOW=2*WAB-1;

    auto x0_frac_full=concat(zg<2>(), x0_frac, zg<PAD_LOW>());
    auto x1_frac_full=concat(zg<2>(), x1_frac, zg<PAD_LOW>());
    auto x2_frac_full=concat(zg<2>(), x2_frac, zg<PAD_LOW>());
    auto x3_frac_full=concat(zg<2>(), x3_frac, zg<PAD_LOW>());

    auto x0_frac_shifted= x0_frac_full;
    auto x1_frac_shifted= safe_rshift(x1_frac_full, shift1);
    auto x2_frac_shifted= safe_rshift(x2_frac_full, shift2);
    auto x3_frac_shifted= safe_rshift(x3_frac_full, shift3);

    auto frac_full=x0_frac_shifted+x1_frac_shifted+x2_frac_shifted+x3_frac_shifted;
    if(log){
        std::cerr<<"  frac_full="<<frac_full<<"\n";
        std::cerr<<"  value = "<<ldexp( to_double_approx(frac_full), x0_exp.to_int()-2*bias-2*WF-PAD_LOW );
    }

    // At this point we have  ????.ffffffff  where there is
    // guaranteed to be a 1 somewhere in the top four bits.
    auto top_four=take_msbs<4>(frac_full);
    assert(top_four.to_int() != 0);

    // We are rounding down, so just dump all the lower parts before shifting
    auto frac_pre_norm=take_msbs<4+WF>(frac_full);  

    // We need to renormalise, so possibilities are:
    //  1???.ffffffff
    //  01??.ffffffff
    //  001?.ffffffff
    //  0001.ffffffff
    fw_uint<WF> frac_post_norm;
    fw_uint<WE+2> exp_post_norm;
    fw_uint<WE+2> exp_post_norm_adj;
    if(get_bits<3,3>(top_four).to_bool()){
        frac_post_norm=get_bits<2+WF,3>(frac_pre_norm);
        exp_post_norm_adj=fw_uint<2+WE>( + 3);
    }else if(get_bits<2,2>(top_four).to_bool()){
        frac_post_norm=get_bits<1+WF,2>(frac_pre_norm);
        exp_post_norm_adj=fw_uint<2+WE>( + 2);
    }else if(get_bits<1,1>(top_four).to_bool()){
        frac_post_norm=get_bits<WF,1>(frac_pre_norm);
        exp_post_norm_adj=fw_uint<2+WE>( + 1);
    }else{
        frac_post_norm=get_bits<WF-1,0>(frac_pre_norm);
        exp_post_norm_adj=fw_uint<2+WE>( + 0);
    }
    exp_post_norm=zpad_hi<1>(x0_exp) + exp_post_norm_adj;

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
fp_flopoco<WE,WF> dot4_pos_rndd_ref_large(
    const fp_flopoco<WE,WF> &a0,
    const fp_flopoco<WE,WF> &a1,
    const fp_flopoco<WE,WF> &a2,
    const fp_flopoco<WE,WF> &a3,
    const fp_flopoco<WE,WF> &b0,
    const fp_flopoco<WE,WF> &b1,
    const fp_flopoco<WE,WF> &b2,
    const fp_flopoco<WE,WF> &b3
){
    assert( (a0.is_positive() & a0.is_normal()).to_bool() );
    assert( (a1.is_positive() & a1.is_normal()).to_bool() );
    assert( (a2.is_positive() & a2.is_normal()).to_bool() );
    assert( (a3.is_positive() & a3.is_normal()).to_bool() );
    assert( (b0.is_positive() & b0.is_normal()).to_bool() );
    assert( (b1.is_positive() & b1.is_normal()).to_bool() );
    assert( (b2.is_positive() & b2.is_normal()).to_bool() );
    assert( (b3.is_positive() & b3.is_normal()).to_bool() );

    MPFR_DECL_INIT(va, WF+1);
    MPFR_DECL_INIT(vb, WF+1);
    MPFR_DECL_INIT(vab, 2*(WF+1));
    MPFR_DECL_INIT(vacc, 12*(WF+2)); // NOTE: I think this should be 8 not 12, but that triggers the notExact test.
    MPFR_DECL_INIT(vr, WF+1);

    int notExact=0;

    a0.get(va);
    b0.get(vb);
    notExact |= mpfr_mul(vacc, va, vb, MPFR_RNDN); // Exact
    assert(!notExact);

    a1.get(va);
    b1.get(vb);
    notExact |=mpfr_mul(vab, va, vb, MPFR_RNDN); // Exact
    notExact |=mpfr_add(vacc, vacc, vab, MPFR_RNDN); // Exact
    assert(!notExact);

    a2.get(va);
    b2.get(vb);
    notExact |=mpfr_mul(vab, va, vb, MPFR_RNDN); // Exact
    notExact |=mpfr_add(vacc, vacc, vab, MPFR_RNDN); // Exact
    assert(!notExact);

    a3.get(va);
    b3.get(vb);
    notExact |=mpfr_mul(vab, va, vb, MPFR_RNDN); // Exact
    assert(!notExact);
    notExact |=mpfr_add(vacc, vacc, vab, MPFR_RNDN); // Exact
    assert(!notExact);

    mpfr_set(vr, vacc, MPFR_RNDD); // Does the single rounding operaton

    return fp_flopoco<WE,WF>(vr, true);
}

template<int WE, int WF>
fp_flopoco<WE,WF> dot4_pos_rndd_ref_small(
    const fp_flopoco<WE,WF> &a0,
    const fp_flopoco<WE,WF> &a1,
    const fp_flopoco<WE,WF> &a2,
    const fp_flopoco<WE,WF> &a3,
    const fp_flopoco<WE,WF> &b0,
    const fp_flopoco<WE,WF> &b1,
    const fp_flopoco<WE,WF> &b2,
    const fp_flopoco<WE,WF> &b3
){
    if((WF+1)*8 >= 52){
        throw std::runtime_error("Types are too wide to reliably do in double.");
    }

    double dres = a0.to_double()*b0.to_double()
                + a1.to_double()*b1.to_double()
                + a2.to_double()*b2.to_double()
                + a3.to_double()*b3.to_double();

    MPFR_DECL_INIT(vres, WF+1);
    mpfr_set_d(vres, dres, MPFR_RNDD);

    return fp_flopoco<WE,WF>(vres, true);
}

template<int WE, int WF>
fp_flopoco<WE,WF> dot4_pos_rndd_ref(
    const fp_flopoco<WE,WF> &a0,
    const fp_flopoco<WE,WF> &a1,
    const fp_flopoco<WE,WF> &a2,
    const fp_flopoco<WE,WF> &a3,
    const fp_flopoco<WE,WF> &b0,
    const fp_flopoco<WE,WF> &b1,
    const fp_flopoco<WE,WF> &b2,
    const fp_flopoco<WE,WF> &b3
){
    if((WF+1)*8 < 52){
        return dot4_pos_rndd_ref_small(a0,a1,a2,a3,b0,b1,b2,b3);
    }else{
        return dot4_pos_rndd_ref_large(a0,a1,a2,a3,b0,b1,b2,b3);
    }
}

template<int WE, int WF>
void test_dot4_pos_rndd(size_t n)
{
    fprintf(stderr, "test_dot4_pos_rndd<WE=%d,WF=%d>(n=%lu)\n", WE,WF,n);

    using fp_t = fp_flopoco<WE,WF>;
    using traits=std::numeric_limits<fp_t>;

    std::mt19937 rng;

    std::array<fp_t,8> args;
    fp_flopoco_random_test_source<WE,WF> source;
    source.disable_class(fp_flopoco_random_test_source<WE,WF>::NaN);
    source.disable_class(fp_flopoco_random_test_source<WE,WF>::PosInf);
    source.disable_class(fp_flopoco_random_test_source<WE,WF>::NegInf);
    source.disable_class(fp_flopoco_random_test_source<WE,WF>::NegNormal);
    source.disable_class(fp_flopoco_random_test_source<WE,WF>::NegZero);
    source.disable_class(fp_flopoco_random_test_source<WE,WF>::PosZero);

    for(unsigned r=0; r<n; r++){
        for(unsigned i=0; i<args.size(); i++){
            args[i]=source(rng);
        }

        auto ref=callN(dot4_pos_rndd_ref<WE,WF>, args);
        auto got=callN(dot4_pos_rndd<WE,WF,false>, args);
        //fprintf(stderr, "%g\n", ref.to_double());
        if(ref.to_double() != got.to_double()){
            fprintf(stderr, "  ERROR : ref=%lg, got=%lg\n", ref.to_double(), got.to_double());
            exit(1);
        }
    }
}

template<int WE, int WF>
void exhaust_dot4_pos_rndd()
{
    using fp_t = fp_flopoco<WE,WF>;
    using traits=std::numeric_limits<fp_t>;

    fp_t x0, x1, x2, x3, x4, x5, x6, x7;

    fp_t upper=traits::max();

    for(x0=traits::min(); x0.is_normal().to_bool(); x0=nextup(x0)){
        std::cerr<<"x0 = "<<x0<<"\n";
        for(x1=traits::min(); x1.is_normal().to_bool(); x1=nextup(x1)){
            std::cerr<<"  x1 = "<<x1<<"\n";
            for(x2=traits::min(); x2.is_normal().to_bool(); x2=nextup(x2)){
                std::cerr<<"    x2 = "<<x2<<"\n";
                for(x3=traits::min(); x3.is_normal().to_bool(); x3=nextup(x3)){
                    for(x4=traits::min(); x4.is_normal().to_bool(); x4=nextup(x4)){
                        for(x5=traits::min(); x5.is_normal().to_bool(); x5=nextup(x5)){
                            for(x6=traits::min(); x6.is_normal().to_bool(); x6=nextup(x6)){
                                for(x7=traits::min(); x7.is_normal().to_bool(); x7=nextup(x7)){
                                    auto ref=dot4_pos_rndd_ref(x0,x1,x2,x3,x4,x5,x6,x7);
                                    auto got=dot4_pos_rndd(x0,x1,x2,x3,x4,x5,x6,x7);
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
            }
        }
    }
}

}; // thls

#endif
