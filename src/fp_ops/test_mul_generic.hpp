#ifndef test_mul_generic_hpp
#define test_mul_generic_hpp

#include "thls/tops/fp_flopoco_mul_v1.hpp"

#ifndef __SYNTHESIS__
#include <random>
#endif

#include <stdint.h>

template<class T>
void build_test_numbers(std::vector<T> &a)
{
	a.push_back(0);
	a.push_back(std::numeric_limits<T>::infinity());
	a.push_back(-std::numeric_limits<T>::infinity());
	a.push_back(std::numeric_limits<T>::quiet_NaN());
	a.push_back(std::numeric_limits<T>::min());		// smallest normalised
	a.push_back(std::numeric_limits<T>::lowest());		// smallest non-normalised
	a.push_back(std::numeric_limits<T>::max());		// smallest non-normalised

	// Numbers from -10..10
	for(float i=-10;i<10;i++){
		a.push_back(i);
	}

	T x=1.0;
	for(float i=0;i<16;i++){
		x=nextafter(x, 2);
		a.push_back(x);
	}
	x=1.0;
	for(float i=0;i<16;i++){
		x=nextafter(x, 0.5);
		a.push_back(x);
	}

	std::mt19937 rng;
	std::uniform_real_distribution<double> urng;
	std::normal_distribution<double> grng;
	std::exponential_distribution<double> erng;
}


template<class TImpl,class TA,class TB,class TR>
bool test_impl(TImpl &impl)
{
	std::vector<TA> a;
	std::vector<TB> b;



	for(float a=-2; a<=+2; a++){
	        for(float b=-2; b<=+2; b++){

	            fp_flopoco<8,23> fa(a), fb(b);

	            fp_flopoco<8,23> fr=mul<8,23>(fa,fb);
				fp_flopoco<8,23> want(a*b);

	            if(got.bits!=want.bits){
					#ifndef __SYNTHESIS__
					std::cerr<<a<<" ("<<fa.str()<<" = "<<fa.to_float()<<") * "<<b<<" ("<<fb.str()<<" = "<<fb.to_float()<<") \n    = "<<got.to_float()<<" ("<<got.str()<<"\n\n";
					std::cerr<<"   got="<<got.str()<<"\n";
					std::cerr<<"  want="<<want.str()<<"\n";
					#endif
					return false;
				}
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
	            return false;
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
	            return false;
	        }

	    }
}

#endif
