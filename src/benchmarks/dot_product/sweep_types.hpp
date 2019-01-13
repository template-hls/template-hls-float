#include <cstdint>

#include "hls_half.h"

#include "thls/tops/fp_flopoco.hpp"
#include "thls/tops/fp_flopoco_add_dual_v1.hpp"
#include "thls/tops/fp_flopoco_mul_v1.hpp"
#include "thls/tops/fp_softfloat.hpp"

typedef half nat16;
typedef float nat32;
typedef double nat64;

typedef thls::fp_flopoco<5,10> hls16;
typedef thls::fp_flopoco<7,16> hls24;
typedef thls::fp_flopoco<8,23> hls32;
typedef thls::fp_flopoco<9,38> hls48;
typedef thls::fp_flopoco<11,52> hls64;

typedef thls::fp_softfloat32 soft32;


template<class TAcc, class TLeft, class TRight>
struct native_dot_traits
{
	typedef TAcc result_t;
	typedef TLeft left_t;
	typedef TRight right_t;

	static TAcc add(const TAcc &a, const TAcc &b)
	{ return a+b; }

    static TAcc add_mul(const TAcc &a, const TLeft &x, const TRight &y)
    { return a+(TAcc)(x*(TLeft)y); }

    static TAcc dot2(const TLeft &x0, const TRight &y0, const TLeft &x1, const TRight &y1)
    {
        return ((TAcc)(x0*y0)) + ((TAcc)(x1*y1));
    }
};

template<class TAcc, class TLeft, class TRight>
struct flopoco_full_dot_traits{
    typedef TAcc result_t;
    typedef TLeft left_t;
    typedef TRight right_t;

    static const int TAE=TAcc::exp_bits;
    static const int TAF=TAcc::frac_bits;

    static const int TLE=TLeft::exp_bits;
    static const int TLF=TLeft::frac_bits;

    static const int TRE=TRight::exp_bits;
    static const int TRF=TRight::frac_bits;


    static const int TME=TAE;
    static const int TMF=TAF;

    static result_t add(const result_t &a, const result_t &b)
    { return thls::add_dual<TAE,TAF>(a,b); }

    static result_t add_mul(const result_t a, const left_t &x, const right_t &y){
        return thls::add_dual<TAE, TAF>(a, thls::mul<TME,TMF>(x,y));
    }

    static result_t dot2(const left_t &x0, const right_t &y0, const left_t &x1, const right_t &y1){
        return thls::add_dual<TAE, TAF>(thls::mul<TME,TMF>(x0,y0), thls::mul<TME,TMF>(x1,y1));
    }
};

template<class TAcc, class TLeft, class TRight>
struct flopoco_round_dot_traits{
    typedef TAcc result_t;
    typedef TLeft left_t;
    typedef TRight right_t;

    static const int TAE=TAcc::exp_bits;
    static const int TAF=TAcc::frac_bits;

    static const int TLE=TLeft::exp_bits;
    static const int TLF=TLeft::frac_bits;

    static const int TRE=TRight::exp_bits;
    static const int TRF=TRight::frac_bits;


    static const int TME=TAE;
    static const int TMF=TLF>TRF ? TLF : TRF;

    THLS_INLINE static result_t add(const result_t &a, const result_t &b)
    {
#pragma HLS INLINE RECURSIVE
    	return thls::add_single<TAE,TAF>(a,b);
    }

    THLS_INLINE static result_t add_mul(const result_t a, const left_t &x, const right_t &y){
#pragma HLS INLINE RECURSIVE
        return thls::add_single<TAE, TAF>(a, thls::mul<TME,TMF>(x,y));
    }

    THLS_INLINE static result_t dot2(const left_t &x0, const right_t &y0, const left_t &x1, const right_t &y1){
#pragma HLS INLINE RECURSIVE
    	thls::fp_flopoco<TME,TLF> x0p, x1p;
    	thls::fp_flopoco<TME,TRF> y0p, y1p;
    	thls::promote(x0p, x0);
    	thls::promote(x1p, x1);
    	thls::promote(y0p, y0);
    	thls::promote(y1p, y1);
        auto a0=thls::mul<TAE,TMF>(x0p,y0p);
        auto a1=thls::mul<TAE,TMF>(x1p,y1p);
        thls::fp_flopoco<TAE,TAF> a0p, a1p;
        thls::promote(a0p, a0);
        thls::promote(a1p, a1);
        return thls::add_single<TAE,TAF>(a0p, a1p);
    }
};


struct softfloat32_dot_traits{
    typedef soft32 result_t;
    typedef soft32 left_t;
    typedef soft32 right_t;

    THLS_INLINE static result_t add(const result_t &a, const result_t &b)
    {
#pragma HLS INLINE RECURSIVE
    	return thls::add(a,b);
    }

    THLS_INLINE static result_t add_mul(const result_t a, const left_t &x, const right_t &y){
#pragma HLS INLINE RECURSIVE
        return thls::add(a, thls::mul(x,y));
    }

    THLS_INLINE static result_t dot2(const left_t &x0, const right_t &y0, const left_t &x1, const right_t &y1){
#pragma HLS INLINE RECURSIVE
    	return thls::add(thls::mul(x0, y0), thls::mul(x1,y1));
    }
};
