#include "thls/tops/fp_convert.hpp"

#include "thls/tops/make_input.hpp"

#include <random>
#include <cmath>

std::mt19937 rng;
std::uniform_real_distribution<float> urng;
std::normal_distribution<float> grng;
std::exponential_distribution<float> erng;

using namespace thls;

template<class TTypeTo,class TTypeFrom,class TImpl>
void test_convert(const TImpl &impl, const TTypeFrom &fa)
{
	TTypeTo fgot;
    impl(fgot,fa); // Version without debug output
	TTypeTo fref;
	ref_convert(fref, fa);


	if(!fref.equals(fgot).to_bool()){
		mpfr_t refV, gotV;
		mpfr_init2(refV, TTypeTo::frac_bits+1);
		mpfr_init2(gotV, TTypeTo::frac_bits+1);
		
		fref.get(refV);
		fgot.get(gotV);
		
		mpfr_nexttoward(gotV, refV);
		

		if( !mpfr_cmp(gotV,refV) ){
			// ignore faithfully rounded
		}else{


			std::cerr<<"  a : "<<fa.str()<<"  "<<fa.to_double_approx()<<"\n";
			std::cerr<<" = \n";
			std::cerr<<"ref : "<<fref.str()<<"  "<<fref.to_double_approx()<<"\n";
			std::cerr<<" vs \n";
			std::cerr<<"got : "<<fgot.str()<<"  "<<fgot.to_double_approx()<<"\n";

			std::stringstream tmp;
			tmp<<fa.bits<<"\n";
			tmp<<"1 "<<fref.bits<<" \n";

			std::cerr<<tmp.str()<<"\n";

            impl(fref, fa); // Do version with debug output

			exit(1);
		}
		mpfr_clear(refV);
		mpfr_clear(gotV);
	}
}

template<class TTypeTo,class TTypeFrom ,class TImpl>
void test_impl(TImpl &impl)
{
	std::vector<TTypeFrom> args;

	make_input(args, 10000);

	for(unsigned i=0; i<args.size(); i++){
		std::cerr<<"i = "<<i<<"\n";
		test_convert<TTypeTo,TTypeFrom >(impl, args[i]);

	}
}


int main()
{
	test_impl<fp_flopoco<9,23>,fp_flopoco<8,22>>(flopoco_convert<9,23,8,22>);
	test_impl<fp_flopoco<9,23>,fp_flopoco<8,23>>(flopoco_convert<9,23,8,23>);
	test_impl<fp_flopoco<9,23>,fp_flopoco<8,24>>(flopoco_convert<9,23,8,24>);
	test_impl<fp_flopoco<9,23>,fp_flopoco<8,25>>(flopoco_convert<9,23,8,25>);


	test_impl<fp_flopoco<7,23>,fp_flopoco<8,22>>(flopoco_convert<7,23,8,22>);
	test_impl<fp_flopoco<7,23>,fp_flopoco<8,23>>(flopoco_convert<7,23,8,23>);
	test_impl<fp_flopoco<7,23>,fp_flopoco<8,24>>(flopoco_convert<7,23,8,24>);
	test_impl<fp_flopoco<7,23>,fp_flopoco<8,25>>(flopoco_convert<7,23,8,25>);

    test_impl<fp_flopoco<8,23>,fp_flopoco<8,22>>(flopoco_convert<8,23,8,22>);
    test_impl<fp_flopoco<8,23>,fp_flopoco<8,23>>(flopoco_convert<8,23,8,23>);
    test_impl<fp_flopoco<8,23>,fp_flopoco<8,24>>(flopoco_convert<8,23,8,24>);
    test_impl<fp_flopoco<8,23>,fp_flopoco<8,25>>(flopoco_convert<8,23,8,25>);





	fprintf(stderr, "Done\n");


    return 0;
}

