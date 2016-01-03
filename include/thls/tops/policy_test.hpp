#ifndef policy_test_hpp
#define policy_test_hpp

#include <stdexcept>
#include <map>
#include <type_traits>
#include <limits>
#include <vector>
#include <cmath>

#include <mpfr.h>


namespace thls
{

struct policy_concept {
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
        if(mpfr_get_prec(r)!=53)
            throw std::runtime_error("Invalid target precision.");

        auto emin=mpfr_get_emin(), emax=mpfr_get_emax();
        mpfr_set_emin (-1073); mpfr_set_emax (1024);

        int i = mpfr_mul (r, a, b, MPFR_RNDN);
        mpfr_subnormalize (r, i, MPFR_RNDN);

        mpfr_set_emin(emin);
        mpfr_set_emax(emax);

        return i;
    }

    static int ref_add(mpfr_t r, mpfr_t a, mpfr_t b) {
        if(mpfr_get_prec(r)!=53)
            throw std::runtime_error("Invalid target precision.");

        auto emin=mpfr_get_emin(), emax=mpfr_get_emax();
        mpfr_set_emin (-1073); mpfr_set_emax (1024);

        int i = mpfr_add (r, a, b, MPFR_RNDN);
        mpfr_subnormalize (r, i, MPFR_RNDN);

        mpfr_set_emin(emin);
        mpfr_set_emax(emax);

        return i;
    }

    static int ref_div(mpfr_t r, mpfr_t a, mpfr_t b) {
        if(mpfr_get_prec(r)!=53)
            throw std::runtime_error("Invalid target precision.");

        auto emin=mpfr_get_emin(), emax=mpfr_get_emax();
        mpfr_set_emin (-1073); mpfr_set_emax (1024);

        int i = mpfr_div (r, a, b, MPFR_RNDN);
        mpfr_subnormalize (r, i, MPFR_RNDN);

        mpfr_set_emin(emin);
        mpfr_set_emax(emax);

        return i;
    }
};


template<class TPolicy>
struct test_policy
{
    typedef TPolicy policy_t;
    typedef typename TPolicy::value_t value_t;
    typedef std::numeric_limits<value_t> limits_t;

    static void make_input(std::vector<value_t> &args, int n)
    {
        typedef std::numeric_limits<value_t> limits_t;

        mpfr_fprintf(stderr, "Prec = %d\n", limits_t::digits);

        mpfr_t tmp;
        mpfr_init2(tmp, limits_t::digits);

        ////////////////////////////////////////
        // Small integers
        for(double a=1; a<=10; a++){
            mpfr_set_d(tmp, a, MPFR_RNDN);
            args.push_back(policy_t::from_mpfr(tmp));
            mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
            args.push_back(policy_t::from_mpfr(tmp));
            mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
        }

        ////////////////////////////////////////
        // Basic edge cases
        mpfr_set_inf(tmp,-1);
        args.push_back(policy_t::from_mpfr(tmp));

        policy_t::to_mpfr(tmp, limits_t::max());
        mpfr_mul_si(tmp,tmp,-1,MPFR_RNDN);
        args.push_back(policy_t::from_mpfr(tmp));

        policy_t::to_mpfr(tmp, limits_t::min());
        mpfr_mul_si(tmp,tmp,-1,MPFR_RNDN);
        args.push_back(policy_t::from_mpfr(tmp));

        mpfr_set_zero(tmp,-1);
        args.push_back(policy_t::from_mpfr(tmp));

        mpfr_set_zero(tmp,+1);
        args.push_back(policy_t::from_mpfr(tmp));

        args.push_back(limits_t::min());
        args.push_back(limits_t::max());
        args.push_back(limits_t::infinity());


        ////////////////////////////////////////
        // Numbers just above one
        mpfr_set_d(tmp, 1, MPFR_RNDN);
        for(int i=0;i<10;i++){
            mpfr_nextabove(tmp);
            args.push_back(policy_t::from_mpfr(tmp));
            mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
            args.push_back(policy_t::from_mpfr(tmp));
            mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
        }

        /////////////////////////////////////////
        // Numbers just below one
        mpfr_set_d(tmp, 1, MPFR_RNDN);
        for(int i=0;i<10;i++){
            mpfr_nextbelow(tmp);
            args.push_back(policy_t::from_mpfr(tmp));
            mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
            args.push_back(policy_t::from_mpfr(tmp));
            mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
        }

        ////////////////////////////////////////
        // Numbers just above zero
        policy_t::to_mpfr(tmp, limits_t::min());
        for(int i=0;i<10;i++){
            mpfr_nextabove(tmp);
            args.push_back(policy_t::from_mpfr(tmp));
            mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
            args.push_back(policy_t::from_mpfr(tmp));
            mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
        }

        ////////////////////////////////////////
        // Numbers just below infinity
        policy_t::to_mpfr(tmp, limits_t::max());
        for(int i=0;i<10;i++){
            mpfr_nextbelow(tmp);
            args.push_back(policy_t::from_mpfr(tmp));
            mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
            args.push_back(policy_t::from_mpfr(tmp));
            mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
        }

        //////////////////////////////////////
        // Binary powers around one, covering range for potential normalisation problems
        for(int i=-limits_t::digits-6;i<limits_t::digits+6;i++){
            mpfr_set_d(tmp, ldexp(1.0, -i), MPFR_RNDN);
            args.push_back(policy_t::from_mpfr(tmp,true));
            mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
            args.push_back(policy_t::from_mpfr(tmp,true));
            mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
        }

        gmp_randstate_t state;
        gmp_randinit_default(state);

        mpfr_t tmp2;
        mpfr_init2(tmp2, limits_t::digits);

        mpz_t rr;
        mpz_init(rr);

        while(args.size()<(unsigned)n){
            // Avoid mpfr_grandom, as VHLS uses an older version of mpfr.h

            // Ratio of uniforms distribution, produces uniform over
            // [0,0.5), then long tailed distribution from 0.5 up.
            mpfr_urandomb(tmp, state);
            mpfr_urandomb(tmp2, state);
            mpfr_div(tmp, tmp, tmp2, MPFR_RNDN);

            args.push_back(policy_t::from_mpfr(tmp, true));
            mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
            args.push_back(policy_t::from_mpfr(tmp, true));
            mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
        }

        mpz_clear(rr);
        mpfr_clear(tmp2);
        gmp_randclear(state);
        mpfr_clear(tmp);
    }


    typedef std::map<std::string,std::pair<int,int> > results_t;

    static void checkComparison(
            results_t &results,
            std::string op,
            const value_t &a, const value_t &b, bool got,
            mpfr_t inA, mpfr_t inB, bool ref
    ){
        std::pair<int,int> &elt=results[op];
        elt.second++;

        if(got!=ref){
            elt.first++;
            if(elt.first==1){
                mpfr_fprintf(stderr, "Fail: %s, a=%Rg, b=%Rg, ref=%d, got=%d\n", op.c_str(), inA, inB, ref?1:0, got?1:0);
            }
        }
    }

    static void checkOperator(
            results_t &results,
            std::string op,
            const value_t &a, const value_t &b, const value_t &got,
            mpfr_t inA, mpfr_t inB, mpfr_t ref
    ){
        std::pair<int,int> &elt=results[op];
        elt.second++;

        value_t refTmp=policy_t::from_mpfr(ref,true);
        policy_t::to_mpfr(ref, refTmp);

        mpfr_t gotM;
        mpfr_init2(gotM, mpfr_get_prec(ref) );
        policy_t::to_mpfr(gotM, got);

        if(mpfr_cmp(gotM,ref) || (mpfr_nan_p(gotM)!=mpfr_nan_p(ref))){
            elt.first++;
            if(elt.first==1){
                mpfr_t tmp,tmp2;
                mpfr_init2(tmp,limits_t::digits);
                mpfr_init2(tmp2,limits_t::digits);
                mpfr_sub(tmp,gotM,ref,MPFR_RNDN);
                mpfr_div(tmp2,tmp,ref,MPFR_RNDN);
                mpfr_fprintf(stderr, "Fail: %s, a=%.Re, b=%.Re, ref=%.Re, got=%.Re,  errA=%.Re, errR=%.Re\n", op.c_str(), inA, inB, ref, gotM, tmp, tmp2);
                mpfr_clear(tmp);

            }
        }

        mpfr_clear(gotM);
    }

    static bool test_arithmetic(int n=100)
    {
        std::vector<value_t> input;

        make_input(input,n);

        mpfr_t inA, inB, resRef, resGot;
        mpfr_inits2(limits_t::digits, inA, inB, resRef, resGot, (mpfr_ptr)nullptr);

        results_t results;

        for(unsigned i=0; i<input.size(); i++)
        {
            value_t a=input[i];
            policy_t::to_mpfr(inA, a);

            for(unsigned j=0; j<input.size(); j++)
            {
                value_t b=input[j];
                policy_t::to_mpfr(inB, b);

                //mpfr_fprintf(stderr, "%Rg,%Rg,  %g,%g\n", inA,inB, a,b);


                checkComparison(results, "Equal", a, b, a==b, inA, inB, mpfr_cmp(inA,inB)==0 );
                checkComparison(results, "NotEqual", a, b, a!=b, inA, inB, mpfr_cmp(inA,inB) || mpfr_unordered_p(inA,inB) );
                checkComparison(results, "LessThan", a, b, a<b, inA, inB, mpfr_cmp(inA,inB)<0 );
                checkComparison(results, "LessThanEqual", a, b, a<=b, inA, inB, mpfr_cmp(inA,inB)<=0 );
                checkComparison(results, "GreaterThan", a, b, a>b, inA, inB, mpfr_cmp(inA,inB)>0 );
                checkComparison(results, "GreaterThanEqual", a, b, a>=b, inA, inB, mpfr_cmp(inA,inB)>=0 );

                policy_t::ref_mul(resRef, inA, inB);
                checkOperator(results, "Mul", a, b, a*b, inA, inB, resRef );

                policy_t::ref_add(resRef, inA, inB);
                checkOperator(results, "Add", a, b, a+b, inA, inB, resRef );

                policy_t::ref_div(resRef, inA, inB);
                checkOperator(results, "Div", a, b, a/b, inA, inB, resRef );
            }
        }



        mpfr_clears(inA,inB,resRef,resGot,(mpfr_ptr)0);

        int failedTests=0, totalTests=0;
        int failedVals=0, totalVals=0;
        for(auto r : results){
            if(r.second.first != 0){
                failedTests++;
                fprintf(stderr, "%s : %d / %d failed\n", r.first.c_str(), r.second.first,r.second.second);
            }
            failedVals+=r.second.first;
            totalVals+=r.second.second;
            totalTests++;
        }
        fprintf(stderr, "%d / %d  (%d / %d) Tests failed\n", failedTests, totalTests, failedVals, totalVals);
        return failedTests!=0;
    }


};

}; // thls

#endif
