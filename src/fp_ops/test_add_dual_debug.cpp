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

	bool trigger=false;
	static fw_uint<35> triggerA("0b010_10000000_10000000_00000000_00000000");
	static fw_uint<35> triggerB("0b011_10011010_00000000_00000000_00000000");

	if( (triggerA==fa.bits && triggerB==fb.bits).to_bool()  ){
    //if(!fref.equals(fgot).to_bool()){
		impl(fa,fb, 1); // Do version with debug output

		mpfr_t tmp;
		mpfr_init2(tmp, TType::frac_bits+1);

		fa.get(tmp);
		std::cerr<<"  a : "<<fa.str();
		mpfr_fprintf(stderr, " %.Re\n", tmp);

		std::cerr<<" + \n";

		fb.get(tmp);
		std::cerr<<"  b : "<<fb.str();
		mpfr_fprintf(stderr, " %.Re\n", tmp);

		std::cerr<<" = \n";
		fref.get(tmp);
		std::cerr<<"  ref : "<<fref.str();
		mpfr_fprintf(stderr, " %.Re\n", tmp);

		std::cerr<<" vs \n";

		fgot.get(tmp);
		std::cerr<<"  got : "<<fgot.str();
		mpfr_fprintf(stderr, " %.Re\n", tmp);

		mpfr_clear(tmp);

		exit(0); // not an error
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


int main()
{

	test_impl<fp_flopoco<8,24>>(add_dual<8,24,8,24,8,24>);
	


	fprintf(stderr, "Done\n");


    return 0;
}

