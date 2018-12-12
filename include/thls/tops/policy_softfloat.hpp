#ifndef policy_softfloat_hpp
#define policy_softfloat_hpp

#include "thls/tops/policy_test.hpp"
#include "thls/tops/policy_native.hpp"

#include "thls/tops/softfloat/src/softfloat.h"
#include "thls/tops/softfloat/softfloat_inline.hpp"

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
{ return f64_le(a,b); }

inline bool operator >(float64_t a, float64_t b)
{ return f64_le(b,a); }

inline bool operator <=(float64_t a, float64_t b)
{ return f64_le(a,b); }

inline bool operator >=(float64_t a, float64_t b)
{ return f64_le(b,a); }


namespace thls {
   
    struct policy_softfloat_single {
        typedef float32_t value_t;

        static value_t from_mpfr(mpfr_t x, bool allowOverUnderFlow = false) {
            float fx=policy_native_single::from_mpfr(x,allowOverUnderFlow);
            return *(value_t*)&fx;
        }

        static void to_mpfr(mpfr_t dst, value_t x) {
            float fx=*(float*)&x;
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

        static value_t from_mpfr(mpfr_t x, bool allowOverUnderFlow = false) {
            double fx=policy_native_double::from_mpfr(x,allowOverUnderFlow);
            return *(value_t*)&fx;
        }

        static void to_mpfr(mpfr_t dst, value_t x) {
            double fx=*(double*)&x;
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
