#ifndef thls_tops_make_input_hpp
#define thls_tops_make_input_hpp

#include <vector>
#include <mpfr.h>
#include <random>

namespace thls
{

template<class TType>
void make_input(std::vector<TType> &args, int n)
{
	typedef std::numeric_limits<TType> limits_t;
	
	
	mpfr_t tmp;
	mpfr_init2(tmp, TType::frac_bits+1);
    
    ////////////////////////////////////////
    // Basic edge cases
    args.push_back(limits_t::neg_infinity());
    args.push_back(limits_t::neg_max());
    args.push_back(limits_t::neg_one());
    args.push_back(limits_t::neg_min());
    args.push_back(limits_t::neg_zero());
    args.push_back(limits_t::zero());
    args.push_back(limits_t::min());
    args.push_back(limits_t::one());
	args.push_back(limits_t::max());
	args.push_back(limits_t::infinity());
	
    ////////////////////////////////////////
	// Small integers
	for(double a=1; a<=10; a++){
		mpfr_set_d(tmp, a, MPFR_RNDN);
	    args.push_back(TType(tmp));
        mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
        args.push_back(TType(tmp));
        mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
	}
	
    
    ////////////////////////////////////////
    // Numbers just above one
	mpfr_set_d(tmp, 1, MPFR_RNDN);
	for(int i=0;i<10;i++){
		mpfr_nextabove(tmp);
		args.push_back(TType(tmp));
        mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
        args.push_back(TType(tmp));
        mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
	}
	
    /////////////////////////////////////////
    // Numbers just below one
	mpfr_set_d(tmp, 1, MPFR_RNDN);
	for(int i=0;i<10;i++){
		mpfr_nextbelow(tmp);
		args.push_back(TType(tmp));
        mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
        args.push_back(TType(tmp));
        mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
	}
    
    ////////////////////////////////////////
    // Numbers just above zero
	limits_t::min().get(tmp);
	for(int i=0;i<10;i++){
		mpfr_nextabove(tmp);
		args.push_back(TType(tmp));
        mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
        args.push_back(TType(tmp));
        mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
	}
    
    ////////////////////////////////////////
    // Numbers just below infinity
	limits_t::max().get(tmp);
	for(int i=0;i<10;i++){
		mpfr_nextbelow(tmp);
		args.push_back(TType(tmp));
        mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
        args.push_back(TType(tmp));
        mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
	}
    
    //////////////////////////////////////
    // Binary powers less than one
    for(int i=1;i<TType::frac_bits+4;i++){
        mpfr_set_d(tmp, ldexp(1.0, -i), MPFR_RNDN);
        args.push_back(TType(tmp));
        mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
        args.push_back(TType(tmp));
        mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
    }
	
	gmp_randstate_t state;
	gmp_randinit_default(state);
	
	mpfr_t tmp2;
	mpfr_init2(tmp2, TType::frac_bits+1);
    
    mpz_t rr;
    mpz_init(rr);
	
	while(args.size()<(unsigned)n){
        // Avoid mpfr_grandom, as VHLS uses an older version of mpfr.h
        
		// Ratio of uniforms distribution, produces uniform over
        // [0,0.5), then long tailed distribution from 0.5 up.
        mpfr_urandomb(tmp, state);
        mpfr_urandomb(tmp2, state);
        mpfr_div(tmp, tmp, tmp2, MPFR_RNDN);
        
		args.push_back(TType(tmp, true));
        mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
        args.push_back(TType(tmp, true));
        mpfr_mul_si(tmp, tmp, -1, MPFR_RNDN);
	}
	
    mpz_clear(rr);
	mpfr_clear(tmp2);
	gmp_randclear(state);
	mpfr_clear(tmp);
}

};

#endif
