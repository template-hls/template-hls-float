#include "thls/tops/fp_flopoco.hpp"

#include <random>

template<int ExpBits,int FracBits>
void test1()
{
    typedef thls::fp_flopoco<ExpBits,FracBits> T;
    typedef std::numeric_limits<thls::fp_flopoco<ExpBits,FracBits> > traits;

    mpfr_t in, out;
    mpfr_init2(in, FracBits+1);
    mpfr_init2(out, FracBits+1);

    mpfr_set_inf(in, +1);
    traits::infinity().get(out);
    assert(mpfr_equal_p(in,out));

    mpfr_set_inf(in, -1);
    traits::neg_infinity().get(out);
    std::cerr<<traits::neg_infinity().str()<<"\n";
    mpfr_fprintf(stderr, "%Rg\n", out);
    assert(mpfr_equal_p(in,out));

    traits::quiet_NaN().get(out);
    assert(mpfr_nan_p(out));

    // lowest -> -inf
    nextdown(traits::lowest()).get(out);
    assert(mpfr_inf_p(out));
    assert(mpfr_sgn(out)<0);

    // highest -> +inf
    nextup(traits::max()).get(out);
    assert(mpfr_inf_p(out));
    assert(mpfr_sgn(out)>0);


    for(double i=0; i<10; i++){
        mpfr_set_d(in, i, MPFR_RNDN);

        thls::fp_flopoco<ExpBits,FracBits> one(in);

        one.get(out);

        mpfr_fprintf(stderr, "%Rf = %Rf\n", in, out);

        assert(mpfr_equal_p(in,out));
    }

    std::mt19937 rng;
    std::uniform_real_distribution<double> urng;
    for(double i=0; i<10; i++){
        double d=urng(rng);
        mpfr_set_d(in, d, MPFR_RNDN);

        thls::fp_flopoco<ExpBits,FracBits> one(in);

        one.get(out);

        mpfr_fprintf(stderr, "%Rf = %Rf\n", in, out);
    }

    mpfr_clear(in);
    mpfr_clear(out);
}

template<int ExpBits,int FracBits>
void test2()
{
    typedef thls::fp_flopoco<ExpBits,FracBits> T;
    typedef std::numeric_limits<thls::fp_flopoco<ExpBits,FracBits> > traits;

    mpfr_t a,b,ref,got;

    mpfr_init2(a,FracBits+1);
    mpfr_init2(b,FracBits+1);
    mpfr_init2(ref,FracBits+1);
    mpfr_init2(got,FracBits+1);

    std::mt19937 rng;
    std::uniform_real_distribution<double> urng;

    for(int i=0;i<100;i++){
        double xa=urng(rng);
        double xb=urng(rng);

        mpfr_set_d(a, xa, MPFR_RNDN);
        mpfr_set_d(b, xb, MPFR_RNDN);
        mpfr_mul(ref, a, b, MPFR_RNDN);

        T fgot=thls::ref_mul<ExpBits,FracBits>(T(a),T(b));

        fgot.get(got);

        mpfr_fprintf(stderr, "  %Rf = %Rf\n", ref, got);

        assert(mpfr_equal_p(ref,got));
    }

    mpfr_clear(a);
    mpfr_clear(b);
    mpfr_clear(ref);
    mpfr_clear(got);
}

template<int ExpBits,int FracBits>
void test()
{
    test1<ExpBits,FracBits>();
    test2<ExpBits,FracBits>();
}

int main()
{
    test<8,23>();
    test<11,52>();
    test<6,4>();

    return 0;
}
