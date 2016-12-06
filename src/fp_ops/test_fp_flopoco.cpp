#include "thls/tops/fp_flopoco.hpp"

#include <random>

#define CHECK(x) if(!(x)){ fprintf(stderr, "Fail : %s, %d - %s\n", __FILE__, __LINE__, #x); exit(1); }

#define CHECK_EQUAL_FW(x,y) if((x!=y).to_bool()){ std::cerr<<"Fail : "<<__FILE__<<" , "<<__LINE__<<"\n  x="<<x<<"\n  y="<<y<<"\n"; exit(1); }

using namespace thls;

template<int ExpBits,int FracBits>
void test1()
{
    //typedef thls::fp_flopoco<ExpBits,FracBits> T;
    typedef std::numeric_limits<thls::fp_flopoco<ExpBits,FracBits> > traits;

    mpfr_t in, out;
    mpfr_init2(in, FracBits+1);
    mpfr_init2(out, FracBits+1);

    mpfr_set_inf(in, +1);
    traits::infinity().get(out);
    CHECK(mpfr_equal_p(in,out));

    mpfr_set_inf(in, -1);
    traits::neg_infinity().get(out);
    std::cerr<<traits::neg_infinity().str()<<"\n";
    mpfr_fprintf(stderr, "-inf = %Rg\n", out);
    CHECK(mpfr_equal_p(in,out));

    traits::quiet_NaN().get(out);
    mpfr_fprintf(stderr, "nan = %Rg\n", out);
    CHECK(mpfr_nan_p(out));

    // lowest -> -inf
    nextdown(traits::lowest()).get(out);
    mpfr_fprintf(stderr, "-inf = %Rg\n", out);
    CHECK(mpfr_inf_p(out));
    CHECK(mpfr_sgn(out)<0);

    // highest -> +inf
    traits::max().get(out);
    std::cerr<<traits::max().str()<<"\n";
    mpfr_fprintf(stderr, "%Rg\n", out);
    nextup(traits::max()).get(out);
    std::cerr<<nextup(traits::max()).str()<<"\n";
    mpfr_fprintf(stderr, "%Rg\n", out);
    CHECK(mpfr_inf_p(out));
    CHECK(mpfr_sgn(out)>0);


    for(double i=0; i<10; i++){
        mpfr_set_d(in, i, MPFR_RNDN);

        thls::fp_flopoco<ExpBits,FracBits> one(in);
        
        std::cerr<<"  "<<one.str()<<"\n";

        one.get(out);

        mpfr_fprintf(stderr, "%Rf = %Rf\n", in, out);

        CHECK(mpfr_equal_p(in,out));
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
    //typedef std::numeric_limits<thls::fp_flopoco<ExpBits,FracBits> > traits;

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

        CHECK(mpfr_equal_p(ref,got));
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

void test_single()
{
    static const int FracBits=23;
    static const int ExpBits=8;
    
    typedef std::numeric_limits<thls::fp_flopoco<ExpBits,FracBits> > traits_t;
    typedef thls::fp_flopoco<ExpBits,FracBits> fp_t;
    
    fw_uint<34> posInfBits("0b1000000000000000000000000000000000");
    fw_uint<34> negInfBits("0b1010000000000000000000000000000000");
    
    fw_uint<34> posOne("0b0100111111100000000000000000000000");
    fw_uint<34> negOne("0b0110111111100000000000000000000000");
    
    fw_uint<34> negMax("0b01_11111111_11111111_11111111_11111111");
    fw_uint<34> posMax("0b01_01111111_11111111_11111111_11111111");
    
    fw_uint<34> negMin("0b01_10000000_00000000_00000000_00000000");
    fw_uint<34> posMin("0b01_00000000_00000000_00000000_00000000");
    
    CHECK_EQUAL_FW( traits_t::infinity().bits, posInfBits);
    CHECK_EQUAL_FW( traits_t::neg_infinity().bits, negInfBits);
    
    CHECK_EQUAL_FW( traits_t::pos_one().bits, posOne);
    CHECK_EQUAL_FW( traits_t::neg_one().bits, negOne);
    
    CHECK_EQUAL_FW( traits_t::max().bits, posMax);
    CHECK_EQUAL_FW( traits_t::neg_max().bits, negMax);
    
    CHECK_EQUAL_FW( traits_t::min().bits, posMin);
    CHECK_EQUAL_FW( traits_t::neg_min().bits, negMin);
    
    mpfr_t tmp;
    mpfr_init2(tmp, FracBits+1);
    
    mpfr_t tmp2;
    mpfr_init2(tmp2, FracBits+1);
    
    // Create +1
    mpfr_set_ui(tmp, 1, MPFR_RNDN);
    fp_t fromMPFR=fp_t(tmp);
    CHECK_EQUAL_FW(posOne, fromMPFR.bits);
    
    fromMPFR.get(tmp2);
    CHECK(mpfr_cmp(tmp,tmp2)==0);
    
    // Create -1
    mpfr_set_si(tmp, -1, MPFR_RNDN);
    fromMPFR=fp_t(tmp);
    CHECK_EQUAL_FW(negOne, fromMPFR.bits);
    
    fromMPFR.get(tmp2);
    CHECK(mpfr_cmp(tmp,tmp2)==0);
    
    // Create pos max = 1.11111111 * 2^MaxExp
    mpfr_set_ui(tmp, (1u<<(FracBits+1))-1, MPFR_RNDN);
    mpfr_mul_2si(tmp, tmp, -FracBits+(1<<(ExpBits-1)), MPFR_RNDN);
    mpfr_fprintf(stderr, "%.16Rg\n", tmp);
    fromMPFR=fp_t(tmp);
    CHECK_EQUAL_FW(posMax, fromMPFR.bits);
    
    fromMPFR.get(tmp2);
    CHECK(mpfr_cmp(tmp,tmp2)==0);
    
    // Create neg max = -1.11111111 * 2^MaxExp
    mpfr_set_ui(tmp, (1u<<(FracBits+1))-1, MPFR_RNDN);
    mpfr_mul_2si(tmp, tmp, -FracBits+(1<<(ExpBits-1)), MPFR_RNDN);
    mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
    mpfr_fprintf(stderr, "%.16Rg\n", tmp);
    fromMPFR=fp_t(tmp);
    CHECK_EQUAL_FW(negMax, fromMPFR.bits);
    
    fromMPFR.get(tmp2);
    CHECK(mpfr_cmp(tmp,tmp2)==0);
    
    // Create pos min = 1.000000000 * 2^MinExp
    mpfr_set_ui(tmp, (1u<<(FracBits)), MPFR_RNDN);
    mpfr_mul_2si(tmp, tmp, -FracBits-(1<<(ExpBits-1))+1, MPFR_RNDN);
    mpfr_fprintf(stderr, "%.16Rg\n", tmp);
    fromMPFR=fp_t(tmp);
    CHECK_EQUAL_FW(posMin, fromMPFR.bits);
    
    fromMPFR.get(tmp2);
    CHECK(mpfr_cmp(tmp,tmp2)==0);
    
    mpfr_clear(tmp);
    mpfr_clear(tmp2);
}

int main()
{
    try{
        
        test_single();
        
        test<8,23>();
        test<8,24>();
        test<8,25>();
        test<8,26>();
        test<8,29>();
        test<8,30>();
        test<8,31>();
        test<8,32>();
        test<8,33>();
        test<9,40>();
        test<11,52>();
        test<6,4>();
        
        fprintf(stderr, "Done\n");
    }catch(std::exception &e){
        std::cerr<<"Exception : "<<e.what()<<"\n";
        exit(1);
    }

    return 0;
}
