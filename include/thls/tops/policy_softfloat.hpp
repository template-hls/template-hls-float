#ifndef policy_softfloat_hpp
#define policy_softfloat_hpp

#include "thls/tops/policy_test.hpp"
#include "thls/tops/policy_native.hpp"

#include "thls/tops/softfloat/src/softfloat.h"
#include "thls/tops/softfloat/softfloat_inline.hpp"

#include "thls/tops/fw_uint.hpp"

namespace std
{


    template<>
    class numeric_limits<float32_t>
    {
    private:
        typedef float32_t T;
        typedef std::numeric_limits<float> ft;

        static float32_t to_T(float x)
        {
            float32_t res;
            memcpy(&res, &x, 4);
            return res;
        }
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
        static const int digits = ft::digits;

        static const int digits10 = ft::digits10;
        static const int max_digits10 = ft::max_digits10;

        static const int radix = 2;

        //static const int bias = ft::bias;

        static const int min_exponent = ft::min_exponent;
        static const int max_exponent = ft::max_exponent;

        static const int min_exponent10 = ft::min_exponent10;
        static const int max_exponent10 = ft::max_exponent10;
        
        static const bool traps = false;

        static const bool tinyness_before = ft::tinyness_before;

        static  T min()
        {
            return to_T(ft::min());
        }

        static  T max()
        {
            return to_T(ft::max());
        }

        static  T lowest()
        { // == -max()
            return to_T(ft::lowest());
        }

        static  T epsilon()
        { return to_T(ft::epsilon());}

        static  T round_error()
        { return to_T(ft::round_error()); }

        static  T infinity()
        { return to_T(ft::infinity()); }

        static  T quiet_NaN()
        { return to_T(ft::quiet_NaN()); }

        /*static  T signalling_NaN()
        { return to_T(ft::signalling_NaN()); }*/

        static  T denorm_min()
        { return to_T(ft::denorm_min()); }
    };

    
    template<>
    class numeric_limits<float64_t>
    {
    private:
        typedef float64_t T;
        typedef std::numeric_limits<double> ft;

        static float64_t to_T(double x)
        {
            float64_t res;
            memcpy(&res, &x, 8);
            return res;
        }
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
        static const int digits = ft::digits;

        static const int digits10 = ft::digits10;
        static const int max_digits10 = ft::max_digits10;

        static const int radix = 2;

        //static const int bias = ft::bias;

        static const int min_exponent = ft::min_exponent;
        static const int max_exponent = ft::max_exponent;

        static const int min_exponent10 = ft::min_exponent10;
        static const int max_exponent10 = ft::max_exponent10;
        
        static const bool traps = false;

        static const bool tinyness_before = ft::tinyness_before;

        static  T min()
        {
            return to_T(ft::min());
        }

        static  T max()
        {
            return to_T(ft::max());
        }

        static  T lowest()
        { // == -max()
            return to_T(ft::lowest());
        }

        static  T epsilon()
        { return to_T(ft::epsilon());}

        static  T round_error()
        { return to_T(ft::round_error()); }

        static  T infinity()
        { return to_T(ft::infinity()); }

        static  T quiet_NaN()
        { return to_T(ft::quiet_NaN()); }

        /*static  T signalling_NaN()
        { return to_T(ft::signalling_NaN()); }*/

        static  T denorm_min()
        { return to_T(ft::denorm_min()); }
    };
};

inline float32_t operator *(float32_t a, float32_t b)
{ return f32_mul(a,b); }

inline float32_t operator +(float32_t a, float32_t b)
{ return f32_add(a,b); }

inline float32_t operator /(float32_t a, float32_t b)
{ return f32_div(a,b); }

inline bool operator ==(float32_t a, float32_t b)
{ return f32_eq(a,b); }

inline bool operator !=(float32_t a, float32_t b)
{ return !f32_eq(a,b); }

inline bool operator <(float32_t a, float32_t b)
{ return f32_lt(a,b); }

inline bool operator >(float32_t a, float32_t b)
{ return f32_lt(b,a); }

inline bool operator <=(float32_t a, float32_t b)
{ return f32_le(a,b); }

inline bool operator >=(float32_t a, float32_t b)
{ return f32_le(b,a); }


inline float64_t operator *(float64_t a, float64_t b)
{ return f64_mul(a,b); }

inline float64_t operator +(float64_t a, float64_t b)
{ return f64_add(a,b); }

inline float64_t operator /(float64_t a, float64_t b)
{ return f64_div(a,b); }

inline bool operator ==(float64_t a, float64_t b)
{ return f64_eq(a,b); }

inline bool operator !=(float64_t a, float64_t b)
{ return !f64_eq(a,b); }

inline bool operator <(float64_t a, float64_t b)
{ return f64_lt(a,b); }

inline bool operator >(float64_t a, float64_t b)
{ return f64_lt(b,a); }

inline bool operator <=(float64_t a, float64_t b)
{ return f64_le(a,b); }

inline bool operator >=(float64_t a, float64_t b)
{ return f64_le(b,a); }


namespace thls {
   
    struct policy_softfloat_single {
        typedef float32_t value_t;

        static_assert(sizeof(value_t)==4, "Expected size to be exactly 4 bytes.");
        static_assert(sizeof(float)==4, "Expected size to be exactly 4 bytes.");

        static value_t from_mpfr(mpfr_t x, bool allowOverUnderFlow = false) {
            float fx=policy_native_single::from_mpfr(x,allowOverUnderFlow);
            value_t tmp;
            memcpy(&tmp, &fx, 4);
            return tmp;
        }

        static void to_mpfr(mpfr_t dst, value_t x) {
            float fx;
            memcpy(&fx, &x, 4);
            policy_native_single::to_mpfr(dst, fx);
        }

        static int ref_mul(mpfr_t r, mpfr_t a, mpfr_t b) {
            return policy_native_single::ref_mul(r,a,b);
        }

        static int ref_add(mpfr_t r, mpfr_t a, mpfr_t b) {
            return policy_native_single::ref_add(r,a,b);
        }

        static int ref_div(mpfr_t r, mpfr_t a, mpfr_t b) {
            return policy_native_single::ref_div(r,a,b);
        }
    };


    struct policy_softfloat_double {
        typedef float64_t value_t;

        static_assert(sizeof(value_t)==8, "Expected size to be exactly 8 bytes.");
        static_assert(sizeof(double)==8, "Expected size to be exactly 8 bytes.");

        static value_t from_mpfr(mpfr_t x, bool allowOverUnderFlow = false) {
            double fx=policy_native_double::from_mpfr(x,allowOverUnderFlow);
            value_t tmp;
            memcpy(&tmp, &fx, 8);
            return tmp;
        }

        static void to_mpfr(mpfr_t dst, value_t x) {
            double fx;
            memcpy(&fx, &x, 8);
            policy_native_double::to_mpfr(dst, fx);
        }

        static int ref_mul(mpfr_t r, mpfr_t a, mpfr_t b) {
            return policy_native_double::ref_mul(r,a,b);
        }

        static int ref_add(mpfr_t r, mpfr_t a, mpfr_t b) {
            return policy_native_double::ref_add(r,a,b);
        }

        static int ref_div(mpfr_t r, mpfr_t a, mpfr_t b) {
            return policy_native_double::ref_div(r,a,b);
        }
    };

};// thls

#endif
