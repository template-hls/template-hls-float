#include "thls/tops/fp_flopoco_inv_nr.hpp"

#include "thls/tops/make_input.hpp"

#include <random>

#include <cmath>

std::mt19937 rng;
std::uniform_real_distribution<float> urng;
std::normal_distribution<float> grng;
std::exponential_distribution<float> erng;

using namespace thls;


template<class TTypeR,class TImpl>
void test_inv(const TImpl &impl, const TTypeR &fa)
{
	TTypeR fgot=impl(fa, 0); // Version without debug output
	TTypeR fref;
	ref_inv(fref, fa);
	
	
	if(!fref.equals(fgot).to_bool()){
		impl(fa, 1); // Do version with debug output
		
		std::cerr<<" 1 / a : "<<fa.str()<<"  "<<fa.to_double_approx()<<"\n";
		std::cerr<<" = \n";
		std::cerr<<"ref : "<<fref.str()<<"  "<<fref.to_double_approx()<<"\n";		
		std::cerr<<" vs \n";
		std::cerr<<"got : "<<fgot.str()<<"  "<<fgot.to_double_approx()<<"\n";		
		
		std::stringstream tmp;
		tmp<<fa.bits<<" "<<fa.bits<<" \n";
		tmp<<"1 "<<fref.bits<<" \n";

		std::cerr<<tmp.str()<<"\n";
		
		exit(1);
	}
}

template<class TTypeR,class TTypeY,class TImpl>
void test_inv_faithful(const TImpl &impl, const TTypeY &fa)
{
	TTypeR fgot=impl(fa, 0); // Version without debug output
	TTypeR fref1, fref2;
	ref_inv_faithful(fref1, fref2, fa);


	if(!fref1.equals(fgot).to_bool() && !fref2.equals(fgot).to_bool()){
		impl(fa, 1); // Do version with debug output

		std::cerr<<" 1 / a : "<<fa.str()<<"  "<<fa.to_double_approx()<<"\n";
		std::cerr<<" = \n";
		std::cerr<<"ref : "<<fref1.str()<<"  "<<fref1.to_double_approx()<<"\n";
		std::cerr<<"ref : "<<fref2.str()<<"  "<<fref2.to_double_approx()<<"\n";
		std::cerr<<" vs \n";
		std::cerr<<"got : "<<fgot.str()<<"  "<<fgot.to_double_approx()<<"\n";


		exit(1);
	}
}

template<class TType ,class TImpl>
void test_impl(TImpl &impl)
{	
	std::vector<TType> args;
	
	make_input(args, 100000);

	fprintf(stderr, "Testing %lu inputs\n", args.size());
	for(unsigned i=0; i<args.size(); i++){
		test_inv<TType >(impl, args[i]);
	}
	fprintf(stderr, "Pass\n");
}

template<class TTypeR, class TTypeY ,class TImpl>
void test_impl_faithful(TImpl &impl)
{
	std::vector<TTypeY> args;

	make_input(args, 100000);

	fprintf(stderr, "Testing %lu inputs: ER=%u,FR=%u, EY=%u,FY=%u\n", args.size(), TTypeR::exp_bits, TTypeR::frac_bits, TTypeY::exp_bits, TTypeY::frac_bits);
	for(unsigned i=0; i<args.size(); i++){
		test_inv_faithful<TTypeR,TTypeY >(impl, args[i]);
	}
	fprintf(stderr, "Pass\n");
}

template<int LSBR,int LSBY>
double test_frac_newton_step()
{
	double worst=-1;

	for(uint64_t i=1ull<<(fw_fix<0,LSBY>::W-1); i < (1ull<<fw_fix<0,LSBY>::W); i++){
        fw_fix<0,LSBY> y(i);
        auto r=inv_frac_table_newton_iter3<LSBR>(y);

		double err=std::abs(r.to_double()-1/y.to_double());
		double eps=std::log2(std::abs(err));

		//std::cerr<<g.to_double()<<", "<<1/y.to_double()<<", "<<g2.to_double()<<", "<<r.to_double()<<"\n";

		if(err>worst) {
			//std::cerr<<g.to_double()<<", "<<1/y.to_double()<<", "<<g2.to_double()<<", "<<r.to_double()<<"\n";


			std::cerr << "  y=" << y.to_double() << ", 1/y=" << 1 / y.to_double() //<< ", guess1=" << g.to_double()
					  << ", approx=" << r.to_double() << ", err=2^" << eps << "\n";
			worst=err;
		}
    }
    return worst;
}

template<int wER,int wFR, int wEY,int wFY>
void test_inv_table_newton()
{
	test_impl_faithful<fp_flopoco<wER,wFR>,fp_flopoco<wEY,wFY> >(inv_table_newton<wER,wFR,wEY,wFY>);
}



int main()
{
	test_impl<fp_flopoco<8,16>>(inv_direct<8,16,8,16>);


	test_inv_table_newton<8,23,8,23>();
	test_inv_table_newton<8,23,8,22>();
	test_inv_table_newton<8,23,8,21>();
	test_inv_table_newton<8,23,8,20>();
	test_inv_table_newton<8,23,8,19>();
	test_inv_table_newton<8,23,8,18>();

	test_inv_table_newton<8,20,8,23>();
	test_inv_table_newton<8,20,8,22>();
	test_inv_table_newton<8,20,8,21>();
	test_inv_table_newton<8,20,8,20>();
	test_inv_table_newton<8,20,8,19>();
	test_inv_table_newton<8,20,8,18>();

	test_inv_table_newton<8,27,8,23>();
	test_inv_table_newton<8,27,8,22>();
	test_inv_table_newton<8,27,8,21>();
	test_inv_table_newton<8,27,8,20>();
	test_inv_table_newton<8,27,8,19>();
	test_inv_table_newton<8,27,8,18>();

	test_inv_table_newton<8,8,8,8>();
	test_inv_table_newton<8,9,8,9>();
	test_inv_table_newton<8,10,8,10>();
	test_inv_table_newton<8,11,8,11>();
	test_inv_table_newton<8,12,8,12>();
	test_inv_table_newton<8,13,8,13>();
	test_inv_table_newton<8,14,8,14>();
	test_inv_table_newton<8,15,8,15>();
	test_inv_table_newton<8,16,8,16>();
	test_inv_table_newton<8,17,8,17>();
	test_inv_table_newton<8,18,8,18>();
	test_inv_table_newton<8,19,8,19>();
	test_inv_table_newton<8,20,8,20>();
	test_inv_table_newton<8,21,8,21>();
	test_inv_table_newton<8,22,8,22>();
	test_inv_table_newton<8,23,8,23>();
	test_inv_table_newton<8,24,8,24>();
	test_inv_table_newton<8,25,8,25>();
	test_inv_table_newton<8,26,8,26>();
	test_inv_table_newton<8,27,8,27>();
	test_inv_table_newton<8,28,8,28>();
	test_inv_table_newton<8,29,8,29>();
	test_inv_table_newton<8,30,8,30>();
	test_inv_table_newton<8,31,8,31>();
	test_inv_table_newton<8,32,8,32>();
	test_inv_table_newton<8,33,8,33>();
	test_inv_table_newton<8,34,8,34>();
	test_inv_table_newton<8,35,8,35>();
	test_inv_table_newton<8,36,8,36>();
	test_inv_table_newton<8,37,8,37>();
	test_inv_table_newton<8,38,8,38>();
	test_inv_table_newton<8,39,8,39>();
	test_inv_table_newton<8,40,8,40>();
	test_inv_table_newton<8,41,8,41>();
	test_inv_table_newton<8,42,8,42>();
	test_inv_table_newton<8,43,8,43>();
	test_inv_table_newton<8,44,8,44>();
	test_inv_table_newton<8,45,8,45>();
	test_inv_table_newton<8,46,8,46>();
	test_inv_table_newton<8,47,8,47>();
	test_inv_table_newton<8,48,8,48>();


	/*
	test_frac_newton_step<-40,-23>();
	test_frac_newton_step<-23,-21>();
	test_frac_newton_step<-23,-22>();
	test_frac_newton_step<-23,-23>();
	test_frac_newton_step<-23,-24>();
	*/
  
	fprintf(stderr, "Done\n");


    return 0;
}
	
