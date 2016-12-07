#include "thls/tops/fp_flopoco_fp2fix.hpp"

#include "thls/tops/make_input.hpp"

#include <random>
#include <cmath>

std::mt19937 rng;
std::uniform_real_distribution<float> urng;
std::normal_distribution<float> grng;
std::exponential_distribution<float> erng;

using namespace thls;

template<bool Signed, int MSBO, int LSBO, int wEI, int wFI, bool trunc_p, class TImpl>
void test_fp2fix(const TImpl &impl, const fp_flopoco<wEI,wFI> &fa)
{
	auto fgot=impl(fa, 0); // Version without debug output
	auto fref=ref_fp2fix<Signed,MSBO,LSBO,wEI,wFI,trunc_p>(fa);


	if(!fref.equals(fgot).to_bool()){
		mpfr_t refV, gotV;
		mpfr_init2(refV, wFI+1);
		mpfr_init2(gotV, wFI+1);
		
		fref.get(refV);
		fgot.get(gotV);
		
		impl(fa, 1); // Do version with debug output

		std::cerr<<"  a : "<<fa.str()<<"  "<<fa.to_double_approx()<<"\n";
		std::cerr<<" = \n";

		std::cerr<<"ref : "<<fref.str()<<"  "<<fref.to_double_approx()<<"\n";
		std::cerr<<" vs \n";
		std::cerr<<"got : "<<fgot.str()<<"  "<<fgot.to_double_approx()<<"\n";

		std::cerr<<"\n  nextup = "<<nextup(fref).str()<<"\n";
		std::cerr<<"\n  nextdown = "<<nextdown(fref).str()<<"\n";

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
	}
}


int main()
{

	//test_impl<fp_flopoco<8,24>>(add_single<8,24,8,24,8,24>);

	test_impl<fp_flopoco<8,23>>(fp2fix<false, 0,-16, 8, 23, false>);
//	test_impl<fp_flopoco<11,52>>(add_single<11,52,11,52,11,52>);
/*
	test_impl<fp_flopoco<6,10>>(add_single<6,10,6,10,6,10>);
	test_impl<fp_flopoco<6,11>>(add_single<6,11,6,11,6,11>);
	test_impl<fp_flopoco<6,12>>(add_single<6,12,6,12,6,12>);
	test_impl<fp_flopoco<6,13>>(add_single<6,13,6,13,6,13>);
	test_impl<fp_flopoco<6,14>>(add_single<6,14,6,14,6,14>);
	test_impl<fp_flopoco<6,15>>(add_single<6,15,6,15,6,15>);

	test_impl<fp_flopoco<8,12>>(add_single<8,12,8,12,8,12>);
	test_impl<fp_flopoco<8,11>>(add_single<8,11,8,11,8,11>);*/


	fprintf(stderr, "Done\n");


    return 0;
}

