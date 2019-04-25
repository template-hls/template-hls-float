#ifndef policy_ieee_on_flopoco_hpp
#define policy_ieee_on_flopoco_hpp

#include "thls/tops/fp_ieee.hpp"
#include "thls/tops/fp_flopoco.hpp"
#include "thls/tops/fp_convert.hpp"
#include "thls/tops/fp_flopoco_primitives.hpp"

#include "thls/tops/fp_flopoco_mul_v1.hpp"
#include "thls/tops/fp_flopoco_add_dual_v1.hpp"
#include "thls/tops/fp_flopoco_add_single_v1.hpp"
#include "thls/tops/fp_flopoco_div_v1.hpp"
#include "thls/tops/fp_flopoco_ordering.hpp"

#include "thls/tops/policy_test.hpp"

namespace thls
{

template<int wE,int wF>
struct fp_ieee_on_flopoco_wrapper
{
public:
    typedef fp_ieee<wE,wF> raw_t;
private:
    typedef fp_flopoco<wE,wF> internal_t;

    static internal_t in(const raw_t &x)
    {
        internal_t res;
        convert(res,x);
        return res;
    }
    
    static raw_t out(const internal_t &x)
    {
        raw_t res;
        convert(res,x);
        return res;
    }
public:
    
    raw_t raw;

    fp_ieee_on_flopoco_wrapper()
    {}
    
    fp_ieee_on_flopoco_wrapper(const raw_t &x)
        : raw(x)
    {}
    
    fp_ieee_on_flopoco_wrapper operator*(const fp_ieee_on_flopoco_wrapper &b) const
    {
        //auto ia=in(raw), ib=in(b.raw);
        //std::cerr<<raw.str()<<" -> "<<ia.str()<<"\n";
        //std::cerr<<b.raw.str()<<" -> "<<ib.str()<<"\n";
        return out(mul<wE,wF>(in(raw), in(b.raw)));
    }
    
    fp_ieee_on_flopoco_wrapper operator+(const fp_ieee_on_flopoco_wrapper &b) const
    { return out(add<wE,wF>(in(raw), in(b.raw))); }

    fp_ieee_on_flopoco_wrapper operator/(const fp_ieee_on_flopoco_wrapper &b) const
    { return out( div<wE,wF>(in(raw), in(b.raw))); }
    
    
    bool operator<(const fp_ieee_on_flopoco_wrapper &b) const
    { return less_than(in(raw), in(b.raw)).to_bool(); }
    
    bool operator<=(const fp_ieee_on_flopoco_wrapper &b) const
    { return less_than_equal(in(raw), in(b.raw)).to_bool(); }
    
    bool operator==(const fp_ieee_on_flopoco_wrapper &b) const
    { return equal(in(raw), in(b.raw)).to_bool(); }
    
    bool operator>=(const fp_ieee_on_flopoco_wrapper &b) const
    { return greater_than_equal(in(raw), in(b.raw)).to_bool(); }
    
    bool operator>(const fp_ieee_on_flopoco_wrapper &b) const
    { return greater_than(in(raw), in(b.raw)).to_bool(); }
    
    bool operator!=(const fp_ieee_on_flopoco_wrapper &b) const
    { return not_equal(in(raw), in(b.raw)).to_bool(); }
};

}; // thls


namespace std
{
    template<int ExpBits,int FracBits>
    class numeric_limits<thls::fp_ieee_on_flopoco_wrapper<ExpBits,FracBits> >
    {
        typedef thls::fp_ieee<ExpBits,FracBits> T;
        typedef std::numeric_limits<thls::fp_ieee<ExpBits,FracBits> > base_t;
    public:
        static const bool is_specialized = true;
        static const bool is_signed = true;
        static const bool is_integer = false;
        static const bool is_exact = false;
        static const bool has_infinity = true;
        static const bool has_quiet_NaN = true;
        static const bool has_signaling_NaN = false;
        static const bool has_denorm = std::denorm_absent;
        static const bool has_denorm_loss = true; // ?
        static const bool round_style = std::round_to_nearest;
        static const bool is_iec559 = false;
        static const bool is_bounded = true;
        static const bool is_modulo = false;
        static const int digits = FracBits+1;

        // TODO
        //static const int digits10 = (int)(FracBits * 0.30102999566398119521373889472449-0.5); // Conservative?
        //static const int max_digits10 = (int)(FracBits * 0.30102999566398119521373889472449+0.5+2); // Correct?

        static const int radix = 2;

        static const int bias = (1<<(ExpBits-1))-1;

        static const int min_exponent = 0-bias;
        static const int max_exponent = 0+bias+1;

        //one more than the smallest negative power of the radix that is a valid normalized floating-point value
        // static const int min_exponent10 = TODO;
        // one more than the largest integer power of the radix that is a valid finite floating-point value
        // static const int max_exponent10 = TODO;
        // the largest integer power of 10 that is a valid finite floating-point value

        static const bool traps = false;

        // identifies floating-point types that detect tinyness before rounding
        // static const bool tinyness_before = TODO;

        static THLS_CONSTEXPR T min()
        {
            return T(base_t::min());
        }

        static THLS_CONSTEXPR T max()
        {
            return T(base_t::max());
        }

        static THLS_CONSTEXPR T lowest()
        { // == -max()
            return T(base_t::lowest());
        }

        static THLS_CONSTEXPR T epsilon()
        { return T(base_t::epsilon());}

        static THLS_CONSTEXPR T round_error()
        { return T(base_t::round_error()); }

        static THLS_CONSTEXPR T infinity()
        { return T(base_t::infinity()); }

        static THLS_CONSTEXPR T quiet_NaN()
        { return T(base_t::quiet_NaN()); }

        static THLS_CONSTEXPR T signalling_NaN()
        { return T(base_t::signalling_NaN()); }

        static THLS_CONSTEXPR T denorm_min()
        { return T(base_t::denorm_min()); }
    };
};

namespace thls{


template<int wE,int wF>
struct policy_ieee_on_flopoco
{
    typedef fp_ieee_on_flopoco_wrapper<wE,wF> value_t;
    typedef fp_ieee<wE,wF> raw_t;

    static value_t from_mpfr(mpfr_t x, bool allowUnderOverflow=false)
    { return raw_t(x,allowUnderOverflow); }

    static void to_mpfr(mpfr_t dst, value_t x)
    { x.raw.get(dst); }

    static void ref_add(mpfr_t dst, mpfr_t a, mpfr_t b)
    {
        auto emin=mpfr_get_emin(), emax=mpfr_get_emax();

        mpfr_set_emin( -(1<<(wE-1))+2 );
        mpfr_set_emax( (1<<(wE-1)) );

        mpfr_add(dst, a, b, MPFR_RNDN);

        mpfr_set_emin(emin);
        mpfr_set_emax(emax);
    }

    static void ref_mul(mpfr_t dst, mpfr_t a, mpfr_t b)
    {
        auto emin=mpfr_get_emin(), emax=mpfr_get_emax();

        mpfr_set_emin( -(1<<(wE-1))+2 );
        mpfr_set_emax( (1<<(wE-1)) );

        mpfr_mul(dst, a, b, MPFR_RNDN);

        mpfr_set_emin(emin);
        mpfr_set_emax(emax);
    }

    static void ref_div(mpfr_t dst, mpfr_t a, mpfr_t b)
    {
        auto emin=mpfr_get_emin(), emax=mpfr_get_emax();

        mpfr_set_emin( -(1<<(wE-1))+2 );
        mpfr_set_emax( (1<<(wE-1)) );

        mpfr_div(dst, a, b, MPFR_RNDN);

        mpfr_set_emin(emin);
        mpfr_set_emax(emax);
    }
};

}; // thls

#endif
