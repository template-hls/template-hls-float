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
	test_impl<fp_flopoco<8,23>,fp_flopoco<8,23>,fp_flopoco<8,23> >(add<8,23,8,23,8,23>);
	test_impl<fp_flopoco<10,23>,fp_flopoco<10,23>,fp_flopoco<10,23> >(add<10,23,10,23,10,23>);
	test_impl<fp_flopoco<8,16>,fp_flopoco<8,16>,fp_flopoco<8,16> >(add<8,16,8,16,8,16>);
	test_impl<fp_flopoco<10,16>,fp_flopoco<10,16>,fp_flopoco<10,16> >(add<10,16,10,16,10,16>);
	
	test_impl<fp_flopoco<10,16>,fp_flopoco<8,16>,fp_flopoco<10,16> >(add<10,16,8,16,10,16>);
	test_impl<fp_flopoco<10,16>,fp_flopoco<10,16>,fp_flopoco<8,16> >(add<10,16,10,16,8,16>);
	
	test_impl<fp_flopoco<10,23>,fp_flopoco<8,23>,fp_flopoco<10,23> >(add<10,23,8,23,10,23>);
	test_impl<fp_flopoco<10,23>,fp_flopoco<10,23>,fp_flopoco<8,23> >(add<10,23,10,23,8,23>);
	
	test_impl<fp_flopoco<12,40>,fp_flopoco<8,40>,fp_flopoco<12,40> >(add<12,40,8,40,12,40>);
	test_impl<fp_flopoco<12,40>,fp_flopoco<12,40>,fp_flopoco<8,40> >(add<12,40,12,40,8,40>);
	
	fprintf(stderr, "Done\n");


    return 0;
}
	
