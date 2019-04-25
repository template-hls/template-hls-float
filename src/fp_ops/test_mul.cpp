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
void test_mul(const TImpl &impl, const TType &fa, const TType &fb)
{
	TType fgot=impl(fa,fb, 0); // Version without debug output
	TType fref;
	ref_mul(fref, fa, fb);
	
	
	if(!fref.equals(fgot).to_bool()){
		impl(fa,fb, 1); // Do version with debug output
		
		std::cerr<<"  a : "<<fa.str()<<"  "<<fa.to_double_approx()<<"\n";
		std::cerr<<" * \n";
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
			test_mul<TType >(impl, args[i], args[j]);
		}	
	}
}


int main()
{
	
	test_impl<fp_flopoco<8,26>>(mul<8,26,8,26,8,26>);
	test_impl<fp_flopoco<8,25>>(mul<8,25,8,25,8,25>);
	test_impl<fp_flopoco<8,24>>(mul<8,24,8,24,8,24>);
	test_impl<fp_flopoco<8,23>>(mul<8,23,8,23,8,23>);
	test_impl<fp_flopoco<8,22>>(mul<8,22,8,22,8,22>);
	test_impl<fp_flopoco<8,21>>(mul<8,21,8,21,8,21>);
	test_impl<fp_flopoco<8,20>>(mul<8,20,8,20,8,20>);
	test_impl<fp_flopoco<8,19>>(mul<8,19,8,19,8,19>);
	test_impl<fp_flopoco<8,18>>(mul<8,18,8,18,8,18>);
	test_impl<fp_flopoco<8,17>>(mul<8,17,8,17,8,17>);
	test_impl<fp_flopoco<8,16>>(mul<8,16,8,16,8,16>);
	test_impl<fp_flopoco<8,15>>(mul<8,15,8,15,8,15>);
	test_impl<fp_flopoco<8,14>>(mul<8,14,8,14,8,14>);
	test_impl<fp_flopoco<8,13>>(mul<8,13,8,13,8,13>);
	test_impl<fp_flopoco<8,12>>(mul<8,12,8,12,8,12>);
	test_impl<fp_flopoco<8,11>>(mul<8,11,8,11,8,11>);

  
	fprintf(stderr, "Done\n");


    return 0;
}
	
