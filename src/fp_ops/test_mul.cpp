#include "thls/tops/fp_flopoco_mul_v1.hpp"

#include <random>

std::mt19937 rng;
std::uniform_real_distribution<float> urng;
std::normal_distribution<float> grng;

using namespace thls;

template<class TImpl>
void test_impl(TImpl &impl)
{
	for(float a=-2; a<=+2; a++){
	        for(float b=-2; b<=+2; b++){

	            fp_flopoco<8,23> fa(a), fb(b);

	            fp_flopoco<8,23> fr=mul<8,23>(fa,fb);

	            #ifndef __SYNTHESIS__
	            std::cerr<<a<<" ("<<fa.str()<<" = "<<fa.to_float()<<") * "<<b<<" ("<<fb.str()<<" = "<<fb.to_float()<<") \n    = "<<fr.to_float()<<" ("<<fr.str()<<"\n\n";
	            #endif

	            assert(a*b==fr.to_float());
	        }
	    }

	    for(int i=0; i<10000; i++){
	        float a=urng(rng);
	        float b=urng(rng);

	        fp_flopoco<8,23> fa(a), fb(b);

	        fp_flopoco<8,23> got=mul<8,23>(fa,fb);
	        fp_flopoco<8,23> want(a*b);

	        if(got.bits!=want.bits){
	            #ifndef __SYNTHESIS__
	        	std::cerr<<a<<" ("<<fa.str()<<" = "<<fa.to_float()<<") * "<<b<<" ("<<fb.str()<<" = "<<fb.to_float()<<") \n    = "<<got.to_float()<<" ("<<got.str()<<"\n\n";
	            std::cerr<<"   got="<<got.str()<<"\n";
	            std::cerr<<"  want="<<want.str()<<"\n";
	            #endif
	            exit(1);
	        }

	    }

	    for(int i=0; i<10000; i++){
	        float a=grng(rng);
	        float b=grng(rng);

	        fp_flopoco<8,23> fa(a), fb(b);

	        fp_flopoco<8,23> got=mul<8,23>(fa,fb);
	        fp_flopoco<8,23> want(a*b);

	        if(got.bits!=want.bits){
	            #ifndef __SYNTHESIS__
	        	std::cerr<<a<<" ("<<fa.str()<<" = "<<fa.to_float()<<") * "<<b<<" ("<<fb.str()<<" = "<<fb.to_float()<<") \n    = "<<got.to_float()<<" ("<<got.str()<<"\n\n";
	            std::cerr<<"   got="<<got.str()<<"\n";
	            std::cerr<<"  want="<<want.str()<<"\n";
	            #endif
	            exit(1);
	        }

	    }

	    for(int i=0; i<10000; i++){
	        float a=grng(rng)/grng(rng);
	        float b=grng(rng)/grng(rng);

	        fp_flopoco<8,23> fa(a), fb(b);

	        fp_flopoco<8,23> got=mul<8,23>(fa,fb);
	        fp_flopoco<8,23> want(a*b);

	        if(got.bits!=want.bits){
	            #ifndef __SYNTHESIS__
	        	std::cerr<<a<<" ("<<fa.str()<<" = "<<fa.to_float()<<") * "<<b<<" ("<<fb.str()<<" = "<<fb.to_float()<<") \n    = "<<got.to_float()<<" ("<<got.str()<<"\n\n";
	            std::cerr<<"   got="<<got.str()<<"\n";
	            std::cerr<<"  want="<<want.str()<<"\n";
	            #endif
	            exit(1);
	        }

	    }
}

int main()
{

    /*for(float a=0;a<10;a++){
        fp_flopoco<8,23> fa(a);

        #ifndef __SYNTHESIS__
        std::cerr<<"a="<<a<<", fa="<<fa.str()<<" \n    = "<<fa.to_float()<<"\n";
        #endif
    }*/


    test_impl(mul<8,23,8,23,8,23>);
    //test_impl(mul_v2<8,23,8,23,8,23>);
    //test_impl(mul_v3<8,23,8,23,8,23>);



    return 0;
}
	
