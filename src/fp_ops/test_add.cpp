#include "thls/tops/fp_flopoco_add_single_v1.hpp"

#include <random>
#include <cmath>

std::mt19937 rng;
std::uniform_real_distribution<float> urng;
std::normal_distribution<float> grng;
std::exponential_distribution<float> erng;

using namespace thls;

template<class TType,class TImpl>
void test_add(const TImpl &impl, const TType &fa, const TType &fb)
{
	TType fgot=impl(fa,fb, 0); // Version without debug output
	TType fref;
	ref_add(fref, fa, fb);


	if(!fref.equals(fgot).to_bool()){
		impl(fa,fb, 1); // Do version with debug output

		std::cerr<<"  a : "<<fa.str()<<"  "<<fa.to_double_approx()<<"\n";
		std::cerr<<" + \n";
		std::cerr<<"  b : "<<fa.str()<<"  "<<fa.to_double_approx()<<"\n";
		std::cerr<<" = \n";
		std::cerr<<"ref : "<<fref.str()<<"  "<<fref.to_double_approx()<<"\n";
		std::cerr<<" vs \n";
		std::cerr<<"ref : "<<fgot.str()<<"  "<<fgot.to_double_approx()<<"\n";

		std::stringstream tmp;
		tmp<<fa.bits<<" "<<fb.bits<<" \n";
		tmp<<"1 "<<fref.bits<<" \n";

		std::cerr<<tmp.str()<<"\n";

		exit(1);
	}
}

template<class TType ,class TImpl>
void test_impl(TImpl &impl)
{
	typedef std::numeric_limits<TType> limits_t;

	std::vector<TType> args;

	mpfr_t tmp;
	mpfr_init2(tmp, TType::frac_bits+1);

	for(float a=-5; a<=+5; a++){
		mpfr_set_d(tmp, a, MPFR_RNDN);
	    args.push_back(TType(tmp));
	}

	args.push_back(limits_t::infinity());
	args.push_back(limits_t::neg_infinity());


	mpfr_set_d(tmp, 1, MPFR_RNDN);
	for(int i=0;i<20;i++){
		mpfr_nextabove(tmp);
		args.push_back(TType(tmp));
	}

	mpfr_set_d(tmp, 1, MPFR_RNDN);
	for(int i=0;i<20;i++){
		mpfr_nextbelow(tmp);
		args.push_back(TType(tmp));
	}

	gmp_randstate_t state;
	gmp_randinit_default(state);

	for(int i=0; i<10; i++){
		mpfr_urandomb(tmp, state);
		args.push_back(TType(tmp));
	}

	mpfr_t tmp2;
	mpfr_init2(tmp2, TType::frac_bits+1);

	for(int i=0; i<100; i++){
		mpfr_urandomb(tmp, state);
		args.push_back(TType(tmp));

		// Avoid mpfr_grandom, as VHLS uses an older version of mpfr.h
		mpfr_set_d(tmp, ldexp(grng(rng),52), MPFR_RNDN);
		mpfr_add_d(tmp, tmp, grng(rng), MPFR_RNDN);
		args.push_back(TType(tmp));

		mpfr_set_d(tmp2, ldexp(grng(rng),52), MPFR_RNDN);
		mpfr_add_d(tmp2, tmp2, grng(rng), MPFR_RNDN);
		args.push_back(TType(tmp2));

		mpfr_div(tmp, tmp, tmp2, MPFR_RNDN);
		args.push_back(TType(tmp));
	}

	mpfr_clear(tmp2);
	gmp_randclear(state);


	for(unsigned i=0; i<args.size(); i++){
		std::cerr<<"i = "<<i<<"\n";
		for(unsigned j=0; j<args.size(); j++){
			test_add<TType >(impl, args[i], args[j]);
		}
	}

	mpfr_clear(tmp);
}


int main()
{

	test_impl<fp_flopoco<8,26>>(add<8,26,8,26,8,26>);
	//test_impl<fp_flopoco<8,25>>(add<8,25,8,25,8,25>);
	test_impl<fp_flopoco<8,24>>(add<8,24,8,24,8,24>);
	//test_impl<fp_flopoco<8,23>>(add<8,23,8,23,8,23>);
	//test_impl<fp_flopoco<8,22>>(add<8,22,8,22,8,22>);
	//test_impl<fp_flopoco<8,21>>(add<8,21,8,21,8,21>);
	//test_impl<fp_flopoco<8,20>>(add<8,20,8,20,8,20>);
	test_impl<fp_flopoco<8,19>>(add<8,19,8,19,8,19>);
	//test_impl<fp_flopoco<8,18>>(add<8,18,8,18,8,18>);
	test_impl<fp_flopoco<8,17>>(add<8,17,8,17,8,17>);
	//test_impl<fp_flopoco<8,16>>(add<8,16,8,16,8,16>);
	//test_impl<fp_flopoco<8,15>>(add<8,15,8,15,8,15>);
	test_impl<fp_flopoco<8,14>>(add<8,14,8,14,8,14>);
	//test_impl<fp_flopoco<8,13>>(add<8,13,8,13,8,13>);

	// FAIL:
	//test_impl<fp_flopoco<8,12>>(add<8,12,8,12,8,12>);
	//test_impl<fp_flopoco<8,11>>(add<8,11,8,11,8,11>);


	fprintf(stderr, "Done\n");


    return 0;
}

