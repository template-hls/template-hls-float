#ifndef thls_fp_softfloat_hpp
#define thls_fp_softfloat_hpp

#include <thls/tops/fw_uint.hpp>

#include <thls/tops/softfloat/src/softfloat.h>
#include <thls/tops/softfloat/softfloat_inline.hpp>

#ifndef THLS_SYNTHESIS
#include <mpfr.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#endif

#include <limits>

namespace thls
{

struct fp_softfloat32
{

    enum{ exp_bits = 8 };
    enum{ frac_bits = 23 };

    THLS_INLINE THLS_CONSTEXPR fp_softfloat32()
        : value(float32_t{0})
    {}

    THLS_INLINE THLS_CONSTEXPR fp_softfloat32(const float _bits)
        : value(float32_t{*(uint32_t*)&_bits})
      {}

    THLS_INLINE THLS_CONSTEXPR fp_softfloat32(const float32_t &_bits)
        : value(_bits)
      {}

    THLS_INLINE THLS_CONSTEXPR fp_softfloat32(const fw_uint<32> &_bits)
        : value(float32_t{_bits.to_uint32()})
    {}
    
    THLS_INLINE THLS_CONSTEXPR fp_softfloat32(const fw_uint<2> _flags, const fw_uint<1> &_sign, const fw_uint<8> _exp, const fw_uint<23> &_frac)
        : value(float32_t{concat(_flags, _sign, _exp, _frac).to_uint32()} )
    {}

#ifndef THLS_SYNTHESIS
    // If allowUnderOrOverflow is on, then exponents out of range
    // will be flushed to zero or infinity.
    // Number of bits in number must _always_ match FracBits
    fp_softfloat(mpfr_t x, bool allowUnderOrOverflow=false);


    void get_exponent(int &e) const;
    void get_fraction(mpfr_t &dst) const;

    // The destination must have the same fractional width
    void get(mpfr_t dst) const;

    // Allows for rounding while extracting
    void get(mpfr_t dst, mpfr_rnd_t mode) const;
#endif


    float32_t value;

    THLS_INLINE fw_uint<32> get_bits() const
    { return fw_uint<32>( value.v); }

    float to_float() const
    { return *(float*)&value.v; }


    #ifndef THLS_SYNTHESIS
    double to_double_approx() const;
    std::string str() const;
    #endif

};

}; // thls

namespace std
{
    template<>
    class numeric_limits<thls::fp_softfloat32>
    {
        typedef thls::fp_softfloat32 T;
    public:
        static const bool is_specialized = true;
        static const bool is_signed = true;
        static const bool is_integer = false;
        static const bool is_exact = false;
        static const bool has_infinity = true;
        static const bool has_quiet_NaN = true;
        static const bool has_signaling_NaN = false;
        static const bool has_denorm = std::denorm_present;
        static const bool has_denorm_loss = true; // ?
        static const bool round_style = std::round_to_nearest;
        static const bool is_iec559 = true;
        static const bool is_bounded = true;
        static const bool is_modulo = false;
        static const int digits = 24;

        // TODO
        //static const int digits10 = (int)(FracBits * 0.30102999566398119521373889472449-0.5); // Conservative?
        //static const int max_digits10 = (int)(FracBits * 0.30102999566398119521373889472449+0.5+2); // Correct?

        static const int radix = 2;

        static const int bias = (1<<(8-1))-1;

        static const int min_exponent = 0-bias;
        static const int max_exponent = 0+bias+1;

        //one more than the smallest negative power of the radix that is a valid normalized floating-point value
        // static const int min_exponent10 = TODO;
        // one more than the largest integer power of the radix that is a valid finite floating-point value
        // static const int max_exponent10 = TODO;
        // the largest integer power of 10 that is a valid finite floating-point value

        static const bool traps = false;

        // identifies floating-point types that detect tinyness before rounding
        static const bool tinyness_before = false;

        static THLS_CONSTEXPR T min()
        {
            assert(0); // Not tested
            return float32_t{ 0x00800000ul }; 
        }

        static THLS_CONSTEXPR T max()
        {
            assert(0); // not tested
            return float32_t{ 0x7F000000ul }; 
        }


        static THLS_CONSTEXPR T lowest()
        { // == -max()
            assert(0); // not tested
            return float32_t{ 0xFF000000ul };
        }

        static THLS_CONSTEXPR T epsilon()
        { 
            assert(0); // not tested
            return float32_t{ 0x3F800001ul }; 
        }

        static THLS_CONSTEXPR T round_error()
        { 
            assert(0); // not tested
            return float32_t{ 0x3F000000ul };
        }

        static THLS_CONSTEXPR T infinity()
        {
            assert(0); // not tested
            return float32_t{ 0x7F800000ul }; 
        }


        static THLS_CONSTEXPR T quiet_NaN()
        {
            assert(0); // not tested
            return float32_t{ 0x7F800001ul }; 
        }

        static THLS_CONSTEXPR T signalling_NaN()
        {
            assert(0); // not tested
            return float32_t{ 0x7F800001ul }; 
        }

        static THLS_CONSTEXPR T denorm_min()
        {
            assert(0); // not tested
            return float32_t{ 0x00000001ul }; 
        }

        
    };
};

namespace thls{


inline fp_softfloat32 ref_mul(const fp_softfloat32 &a, const fp_softfloat32 &b)
{
    float r=a.to_float() * b.to_float();
   return fp_softfloat32{ r };
}

inline void ref_mul(fp_softfloat32 &dst, const fp_softfloat32 &a, const fp_softfloat32 &b)
{
    dst=ref_mul(a,b);
}

inline fp_softfloat32 ref_add(const fp_softfloat32 &a, const fp_softfloat32 &b)
{
    float r=a.to_float() + b.to_float();
   return fp_softfloat32{ r };
}

inline void ref_add(fp_softfloat32 &dst, const fp_softfloat32 &a, const fp_softfloat32 &b)
{
    dst=ref_add(a,b);
}

inline fp_softfloat32 ref_div(const fp_softfloat32 &a, const fp_softfloat32 &b)
{
    float r=a.to_float() / b.to_float();
   return fp_softfloat32{ r };
}

inline void ref_div(fp_softfloat32 &dst, const fp_softfloat32 &a, const fp_softfloat32 &b)
{
    dst=ref_div(a,b);
}



THLS_INLINE fp_softfloat32 mul(const fp_softfloat32 &x, const fp_softfloat32 &y, int DEBUG=0)
{
    return fp_softfloat32{ f32_mul(x.value, y.value)  };
}

THLS_INLINE fp_softfloat32 add(const fp_softfloat32 &x, const fp_softfloat32 &y, int DEBUG=0)
{
    return fp_softfloat32{ f32_add(x.value, y.value)  };
}

THLS_INLINE fp_softfloat32 div(const fp_softfloat32 &x, const fp_softfloat32 &y, int DEBUG=0)
{
    return fp_softfloat32{ f32_div(x.value, y.value)  };
}

}; // thls

#endif
