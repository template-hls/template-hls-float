#include "thls/tops/fp_flopoco_add_dual_v1.hpp"

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
		impl(fa,fb, 1); // Do version with debug output

		mpfr_t tmp;
		mpfr_init2(tmp, TType::frac_bits+1);

		fa.get(tmp);
		std::cerr<<"  a : "<<fa.str();
		mpfr_fprintf(stderr, "%.Re\n", tmp);

		std::cerr<<" + \n";

		fb.get(tmp);
		std::cerr<<"  b : "<<fb.str();
		mpfr_fprintf(stderr, "%.Re\n", tmp);

		std::cerr<<" = \n";
		fref.get(tmp);
		std::cerr<<"  ref : "<<fref.str();
		mpfr_fprintf(stderr, "%.Re\n", tmp);

		std::cerr<<" vs \n";

		fgot.get(tmp);
		std::cerr<<"  got : "<<fgot.str();
		mpfr_fprintf(stderr, "%.Re\n", tmp);

		mpfr_clear(tmp);

		std::stringstream tmp2;
		tmp2<<fa.bits<<" + "<<fb.bits<<" \n";
		tmp2<<" = "<<fref.bits<<" \n";

		std::cerr<<tmp2.str()<<"\n";

		exit(1);
	}
}

template<class TType ,class TImpl>
void test_impl(TImpl &impl)
{
	std::vector<TType> args;

	make_input(args, 500);

	for(unsigned i=0; i<args.size(); i++){
		std::cerr<<"i = "<<i<<"\n";
		for(unsigned j=0; j<args.size(); j++){
			test_add<TType >(impl, args[i], args[j]);
		}
	}
}


int main()
{
	test_impl<fp_flopoco<8,23>>(add_dual<8,23,8,23,8,23>);

	test_impl<fp_flopoco<11,60>>(add_dual<11,60,11,60,11,60>);
	test_impl<fp_flopoco<11,52>>(add_dual<11,52,11,52,11,52>);
	test_impl<fp_flopoco<11,50>>(add_dual<11,50,11,50,11,50>);
	test_impl<fp_flopoco<11,40>>(add_dual<11,40,11,40,11,40>);

	test_impl<fp_flopoco<10,40>>(add_dual<10,40,10,40,10,40>);
	test_impl<fp_flopoco<10,30>>(add_dual<10,30,10,30,10,30>);

	test_impl<fp_flopoco<9,37>>(add_dual<9,37,9,37,9,37>);
	test_impl<fp_flopoco<9,30>>(add_dual<9,30,9,30,9,30>);
	test_impl<fp_flopoco<9,23>>(add_dual<9,23,9,23,9,23>);


	test_impl<fp_flopoco<8,32>>(add_dual<8,32,8,32,8,32>);

	test_impl<fp_flopoco<6,23>>(add_dual<6,23,6,23,6,23>);
	test_impl<fp_flopoco<6,12>>(add_dual<6,12,6,12,6,12>);

	test_impl<fp_flopoco<5,12>>(add_dual<5,12,5,12,5,12>);
	test_impl<fp_flopoco<5,11>>(add_dual<5,11,5,11,5,11>);
	test_impl<fp_flopoco<5,10>>(add_dual<5,10,5,10,5,10>);


	fprintf(stderr, "Done\n");


    return 0;
}

