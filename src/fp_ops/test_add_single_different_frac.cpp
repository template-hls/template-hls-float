#include "thls/tops/fp_flopoco_add_single_v1.hpp"

#include "thls/tops/make_input.hpp"

#include <random>

#include <cmath>

std::mt19937 rng;
std::uniform_real_distribution<float> urng;
std::normal_distribution<float> grng;
std::exponential_distribution<float> erng;

using namespace thls;

template<class TTypeR,class TTypeA,class TTypeB,class TImpl>
void test_add(const TImpl &impl, const TTypeA &fa, const TTypeB &fb)
{
	TTypeR fgot=impl(fa,fb, 0); // Version without debug output
	TTypeR fref;
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

template<class TTypeR, class TTypeA, class TTypeB,class TImpl>
void test_impl(TImpl &impl)
{	
	std::vector<TTypeA> args_a;
	make_input(args_a, 100);
	
	std::vector<TTypeB> args_b;
	make_input(args_b, 100);
	
	
	
	
	for(unsigned i=0; i<args_a.size(); i++){
		std::cerr<<"i = "<<i<<"\n";
		for(unsigned j=0; j<args_b.size(); j++){
			test_add<TTypeR,TTypeA,TTypeB,TImpl >(impl, args_a[i], args_b[j]);
		}	
	}
	
}


int main()
{
	test_impl<fp_flopoco<8,23>,fp_flopoco<8,23>,fp_flopoco<8,23> >(add_single<8,23,8,23,8,23>);
	test_impl<fp_flopoco<8,16>,fp_flopoco<8,16>,fp_flopoco<8,16> >(add_single<8,16,8,16,8,16>);
	
	test_impl<fp_flopoco<8,16>,fp_flopoco<8,10>,fp_flopoco<8,16> >(add_single<8,16,8,10,8,16>);
	test_impl<fp_flopoco<8,16>,fp_flopoco<8,16>,fp_flopoco<8,10> >(add_single<8,16,8,16,8,10>);
	
	test_impl<fp_flopoco<8,27>,fp_flopoco<8,26>,fp_flopoco<8,27> >(add_single<8,27,8,26,8,27>);
	test_impl<fp_flopoco<8,27>,fp_flopoco<8,27>,fp_flopoco<8,26> >(add_single<8,27,8,27,8,26>);
	
	test_impl<fp_flopoco<8,30>,fp_flopoco<8,20>,fp_flopoco<8,30> >(add_single<8,30,8,20,8,30>);
	test_impl<fp_flopoco<8,30>,fp_flopoco<8,30>,fp_flopoco<8,20> >(add_single<8,30,8,30,8,20>);
  
	fprintf(stderr, "Done\n");


    return 0;
}
	
