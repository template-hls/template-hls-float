#ifndef policy_native_hpp
#define policy_native_hpp

#include "thls/tops/policy_test.hpp"

namespace thls {

    struct policy_native_double {
        typedef double value_t;

        static value_t from_mpfr(mpfr_t x, bool allowOverUnderFlow = false) {
            double v = mpfr_get_d(x, MPFR_RNDN);
            if (mpfr_cmp_d(x, v)) {
                if (v == 0 || std::isinf(v)) {
                    if (!allowOverUnderFlow)
                        throw std::runtime_error("Conversion has over or underflowed.");
                } else if (std::numeric_limits<double>::lowest() <= std::abs(v) &&
                           std::abs(v) <= std::numeric_limits<double>::min()) {
                    if (!allowOverUnderFlow)
                        throw std::runtime_error("Conversion has underflowed.");
                } else {
                    mpfr_t xxA, xxR;
                    mpfr_init2(xxA, 53);
                    mpfr_init2(xxR, 100);
                    mpfr_sub_d(xxA, x, v, MPFR_RNDN);
                    mpfr_div(xxR, xxA, x, MPFR_RNDN);
                    mpfr_fprintf(stderr, "Converting %Rg, %g, errA=%Rg, errR=%Rg\n", x, v, xxA, xxR);
                    mpfr_clear(xxA);
                    throw std::runtime_error("Conversion is not exact.");
                }
            }
            return v;
        }

        static void to_mpfr(mpfr_t dst, value_t x) {
            if (mpfr_set_d(dst, x, MPFR_RNDN)) {
                throw std::runtime_error("Conversion is not exact.");
            }
        }

        static int ref_mul(mpfr_t r, mpfr_t a, mpfr_t b) {
            if (mpfr_get_prec(r) != 53)
                throw std::runtime_error("Invalid target precision.");

            auto emin = mpfr_get_emin(), emax = mpfr_get_emax();
            mpfr_set_emin(-1073);
            mpfr_set_emax(1024);

            int i = mpfr_mul(r, a, b, MPFR_RNDN);
            mpfr_subnormalize(r, i, MPFR_RNDN);

            mpfr_set_emin(emin);
            mpfr_set_emax(emax);

            return i;
        }

        static int ref_add(mpfr_t r, mpfr_t a, mpfr_t b) {
            if (mpfr_get_prec(r) != 53)
                throw std::runtime_error("Invalid target precision.");

            auto emin = mpfr_get_emin(), emax = mpfr_get_emax();
            mpfr_set_emin(-1073);
            mpfr_set_emax(1024);

            int i = mpfr_add(r, a, b, MPFR_RNDN);
            mpfr_subnormalize(r, i, MPFR_RNDN);

            mpfr_set_emin(emin);
            mpfr_set_emax(emax);

            return i;
        }

        static int ref_div(mpfr_t r, mpfr_t a, mpfr_t b) {
            if (mpfr_get_prec(r) != 53)
                throw std::runtime_error("Invalid target precision.");

            auto emin = mpfr_get_emin(), emax = mpfr_get_emax();
            mpfr_set_emin(-1073);
            mpfr_set_emax(1024);

            int i = mpfr_div(r, a, b, MPFR_RNDN);
            mpfr_subnormalize(r, i, MPFR_RNDN);

            mpfr_set_emin(emin);
            mpfr_set_emax(emax);

            return i;
        }
    };

    struct policy_native_single {
        typedef float value_t;

        static value_t from_mpfr(mpfr_t x, bool allowOverUnderFlow = false) {
            mpfr_t tmp;
            mpfr_init2(tmp, 24);

            auto emin = mpfr_get_emin(), emax = mpfr_get_emax();
            mpfr_set_emin(-148);
            mpfr_set_emax(128);

            int i = mpfr_set (tmp, x, MPFR_RNDN);
            mpfr_subnormalize(tmp, i, MPFR_RNDN);

            mpfr_set_emin(emin);
            mpfr_set_emax(emax);

            double v = mpfr_get_d(tmp, MPFR_RNDN);

            mpfr_clear(tmp);

            if (mpfr_cmp_d(x, v)) {
                if (v == 0 || std::isinf(v)) {
                    if (!allowOverUnderFlow)
                        throw std::runtime_error("Conversion has over or underflowed.");
                } else {
                    mpfr_t xxA, xxR;
                    mpfr_init2(xxA, 24);
                    mpfr_init2(xxR, 100);
                    mpfr_sub_d(xxA, x, v, MPFR_RNDN);
                    mpfr_div(xxR, xxA, x, MPFR_RNDN);
                    mpfr_fprintf(stderr, "Converting %Rg, %g, errA=%Rg, errR=%Rg\n", x, v, xxA, xxR);
                    mpfr_clear(xxA);
                    throw std::runtime_error("Conversion is not exact.");
                }
            }
            return v;
        }

        static void to_mpfr(mpfr_t dst, value_t x) {
            if (mpfr_set_d(dst, x, MPFR_RNDN)) {
                throw std::runtime_error("Conversion is not exact.");
            }
        }

        static int ref_mul(mpfr_t r, mpfr_t a, mpfr_t b) {
            if (mpfr_get_prec(r) != 24)
                throw std::runtime_error("Invalid target precision.");

            auto emin = mpfr_get_emin(), emax = mpfr_get_emax();
            mpfr_set_emin(-148);
            mpfr_set_emax(128);

            int i = mpfr_mul(r, a, b, MPFR_RNDN);
            mpfr_subnormalize(r, i, MPFR_RNDN);

            mpfr_set_emin(emin);
            mpfr_set_emax(emax);

            return i;
        }

        static int ref_add(mpfr_t r, mpfr_t a, mpfr_t b) {
            if (mpfr_get_prec(r) != 24)
                throw std::runtime_error("Invalid target precision.");

            auto emin = mpfr_get_emin(), emax = mpfr_get_emax();
            mpfr_set_emin(-148);
            mpfr_set_emax(128);

            int i = mpfr_add(r, a, b, MPFR_RNDN);
            mpfr_subnormalize(r, i, MPFR_RNDN);

            mpfr_set_emin(emin);
            mpfr_set_emax(emax);

            return i;
        }

        static int ref_div(mpfr_t r, mpfr_t a, mpfr_t b) {
            if (mpfr_get_prec(r) != 24)
                throw std::runtime_error("Invalid target precision.");

            auto emin = mpfr_get_emin(), emax = mpfr_get_emax();
            mpfr_set_emin(-148);
            mpfr_set_emax(128);


            int i = mpfr_div(r, a, b, MPFR_RNDN);
            mpfr_subnormalize(r, i, MPFR_RNDN);

            mpfr_set_emin(emin);
            mpfr_set_emax(emax);

            return i;
        }
    };

};// thls

#endif
