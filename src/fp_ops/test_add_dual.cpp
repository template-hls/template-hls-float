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

		std::cerr<<"  a : "<<fa.str()<<"  "<<fa.to_double_approx()<<"\n";
		std::cerr<<" + \n";
		std::cerr<<"  b : "<<fb.str()<<"  "<<fb.to_double_approx()<<"\n";
		std::cerr<<" = \n";
		std::cerr<<"ref : "<<fref.str()<<"  "<<fref.to_double_approx()<<"\n";
		std::cerr<<" vs \n";
		std::cerr<<"got : "<<fgot.str()<<"  "<<fgot.to_double_approx()<<"\n";

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

	test_impl<fp_flopoco<8,23>>(add_dual<8,23,8,23,8,23>);
	test_impl<fp_flopoco<6,23>>(add_dual<6,23,6,23,6,23>);
	test_impl<fp_flopoco<6,12>>(add_dual<6,12,6,12,6,12>);
	test_impl<fp_flopoco<5,12>>(add_dual<5,12,5,12,5,12>);
	test_impl<fp_flopoco<5,11>>(add_dual<5,11,5,11,5,11>);
	test_impl<fp_flopoco<5,10>>(add_dual<5,10,5,10,5,10>);


	fprintf(stderr, "Done\n");


    return 0;
}

