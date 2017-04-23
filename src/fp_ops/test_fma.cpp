#include "thls/tops/fp_flopoco_fma.hpp"

#include "thls/tops/make_input.hpp"

#include <random>
#include <cmath>

std::mt19937 rng;
std::uniform_real_distribution<float> urng;
std::normal_distribution<float> grng;
std::exponential_distribution<float> erng;

using namespace thls;

template<class TTypeR,class TTypeA,class TTypeB,class TTypeC,class TImpl>
void test_fma(const TImpl &impl, const TTypeA &fa, const TTypeB &fb, const TTypeC &fc)
{
	TTypeR fgot=impl(fa,fb, fc,0); // Version without debug output
	TTypeR fref;
	ref_fma(fref, fa, fb, fc);


	//if(!fref.equals(fgot).to_bool()){
	if(!fref.within_1ulp(fgot).to_bool()){

		mpfr_t refV, gotV;
		mpfr_init2(refV, TTypeR::frac_bits+1);
		mpfr_init2(gotV, TTypeR::frac_bits+1);
		
		fref.get(refV);
		fgot.get(gotV);
		

		impl(fa,fb,fc, 1); // Do version with debug output

		std::cerr<<"  a : "<<fa.str()<<"  "<<fa.to_double_approx()<<"\n";
		std::cerr<<" * \n";
		std::cerr<<"  b : "<<fb.str()<<"  "<<fb.to_double_approx()<<"\n";
		std::cerr<<" + \n";
		std::cerr<<"  c : "<<fc.str()<<"  "<<fc.to_double_approx()<<"\n";
		std::cerr<<" = \n";
		std::cerr<<"ref : "<<fref.str()<<"  "<<fref.to_double_approx()<<"\n";
		std::cerr<<" vs \n";
		std::cerr<<"got : "<<fgot.str()<<"  "<<fgot.to_double_approx()<<"\n";

		std::stringstream tmp;
		tmp<<fa.bits<<" "<<fb.bits<<" \n";
		tmp<<"1 "<<fref.bits<<" \n";

		std::cerr<<tmp.str()<<"\n";

		exit(1);

		mpfr_clear(refV);
		mpfr_clear(gotV);
	}
}

template<class TTypeR, class TTypeA,class TTypeB,class TTypeC,class TImpl>
void test_impl(TImpl &impl)
{
	std::vector<TTypeA> args1;
	std::vector<TTypeB> args2;
	std::vector<TTypeC> args3;

	make_input(args1, 100);
	make_input(args2, 100);
	make_input(args3, 100);

	for(unsigned i=0; i<args1.size(); i++){
		std::cerr<<"i = "<<i<<"\n";
		for(unsigned j=0; j<args1.size(); j++){
			//std::cerr<<"j = "<<j<<"\n";
			for(unsigned k=0; k<args3.size(); k++){
				test_fma<TTypeR >(impl, args1[i], args2[j], args3[k]);
			}
		}
	}
}


int main()
{
    test_impl<fp_flopoco<8,23>,fp_flopoco<8,23>,fp_flopoco<8,25>,fp_flopoco<8,23>>(flopoco_fma<8,23,8,23,8,25,8,23>);

	test_impl<fp_flopoco<8,23>,fp_flopoco<8,23>,fp_flopoco<8,23>,fp_flopoco<8,23>>(flopoco_fma<8,23,8,23,8,23,8,23>);
	test_impl<fp_flopoco<8,23>,fp_flopoco<8,24>,fp_flopoco<8,23>,fp_flopoco<8,23>>(flopoco_fma<8,23,8,24,8,23,8,23>);

	test_impl<fp_flopoco<8,23>,fp_flopoco<8,23>,fp_flopoco<8,23>,fp_flopoco<8,26>>(flopoco_fma<8,23,8,23,8,23,8,26>);

/*
	test_impl<fp_flopoco<11,52>>(add_single<11,52,11,52,11,52>);

	test_impl<fp_flopoco<6,10>>(add_single<6,10,6,10,6,10>);
	test_impl<fp_flopoco<6,11>>(add_single<6,11,6,11,6,11>);
	test_impl<fp_flopoco<6,12>>(add_single<6,12,6,12,6,12>);
	test_impl<fp_flopoco<6,13>>(add_single<6,13,6,13,6,13>);
	test_impl<fp_flopoco<6,14>>(add_single<6,14,6,14,6,14>);
	test_impl<fp_flopoco<6,15>>(add_single<6,15,6,15,6,15>);

	test_impl<fp_flopoco<8,12>>(add_single<8,12,8,12,8,12>);
	test_impl<fp_flopoco<8,11>>(add_single<8,11,8,11,8,11>);
	*/


	fprintf(stderr, "Done\n");


    return 0;
}

