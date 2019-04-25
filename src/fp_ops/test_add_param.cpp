#include "thls/tops/fp_flopoco_primitives.hpp"

#include "thls/tops/make_input.hpp"

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
		mpfr_t refV, gotV;
		mpfr_init2(refV, TType::frac_bits+1);
		mpfr_init2(gotV, TType::frac_bits+1);
		
		fref.get(refV);
		fgot.get(gotV);
		
		mpfr_nexttoward(gotV, refV);
		
		// NOTE : We now only check for faithfully rounded
		// TODO : Push the test-cases back into flopoco
		
		//if( !mpfr_cmp(gotV,refV) ){
	//		// ignore faithfully rounded
		//}else{
			
			impl(fa,fb, 1); // Do version with debug output

			std::cerr<<"  a : "<<fa.str()<<"  "<<fa.to_double_approx()<<"\n";
			std::cerr<<" + \n";
			std::cerr<<"  b : "<<fb.str()<<"  "<<fb.to_double_approx()<<"\n";
			std::cerr<<" = \n";

			std::cerr<<"ref : "<<fref.str()<<"  "<<fref.to_double_approx()<<"\n";
			std::cerr<<" vs \n";
			std::cerr<<"got : "<<fgot.str()<<"  "<<fgot.to_double_approx()<<"\n";

			std::cerr<<"\n  nextup = "<<nextup(fref).str()<<"\n";
			std::cerr<<"\n  nextdown = "<<nextdown(fref).str()<<"\n";

			exit(1);
		//}
		
		mpfr_clear(refV);
		mpfr_clear(gotV);
	}
}

template<class TType ,class TImpl>
void test_impl(TImpl &impl)
{
	std::vector<TType> args;

	make_input(args, 100);

	for(unsigned i=0; i<args.size(); i++){
		std::cerr<<"i = "<<i<<"\n";
		for(unsigned j=0; j<args.size(); j++){
			test_add<TType >(impl, args[i], args[j]);
		}
	}
}

template<int E, int W>
void test_for_E_W()
{
	test_impl<fp_flopoco<E,W>>(add<E,W,E,W,E,W>);
}

int main()
{
	test_for_E_W<${__E__},${__F__}>();

	fprintf(stderr, "Done\n");


    return 0;
}

