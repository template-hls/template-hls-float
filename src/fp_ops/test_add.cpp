#include "thls/tops/fp_flopoco_add_single_v1.hpp"

#include <random>

#include <cmath>

std::mt19937 rng;
std::uniform_real_distribution<float> urng;
std::normal_distribution<float> grng;
std::exponential_distribution<float> erng;

using namespace thls;

template<class TImpl>
void test_add(const TImpl &impl, float a, float b)
{
	fp_flopoco<8,23> fa(a), fb(b);

	fp_flopoco<8,23> fr=impl(fa,fb);

	#ifndef __SYNTHESIS__
	//std::cerr<<a<<" ("<<fa.str()<<" = "<<fa.to_float()<<")\n+\n"<<b<<" ("<<fb.str()<<" = "<<fb.to_float()<<")\n=\n"<<fr.to_float()<<" ("<<fr.str()<<"\n\n";
	#endif

	//assert( (a+b)==fr.to_float());
	float ref=a+b;
	float up=nextafterf(a+b, DBL_MAX);
	float down=nextafterf(a+b, -DBL_MAX);
	float got=fr.to_float();
	
	fp_flopoco<8,23> fref=ref_add<8,23>(fa, fb);
	float gfref=fref.to_float();
	
	if(gfref!=ref){
		fprintf(stderr, "%.12g + %.12g = %.12g, %.12g\n", a, b, gfref, ref);
	}
	
	if(std::isinf(ref)){
		if(!std::isinf(got)){
					std::cerr<<a<<" ("<<fa.str()<<" = "<<fa.to_float()<<")\n+\n"<<b<<" ("<<fb.str()<<" = "<<fb.to_float()<<")\n=\n"<<fr.to_float()<<" ("<<fr.str()<<"\n\n";
		}
		assert(std::isinf(got));
	}else if(std::isnan(ref)){
		if(!std::isnan(got)){
					std::cerr<<a<<" ("<<fa.str()<<" = "<<fa.to_float()<<")\n+\n"<<b<<" ("<<fb.str()<<" = "<<fb.to_float()<<")\n=\n"<<fr.to_float()<<" ("<<fr.str()<<"\n\n";
		}
		assert(std::isnan(got));
	}else if( (got<down) || (up <got) ){
		std::cerr<<a<<" ("<<fa.str()<<" = "<<fa.to_float()<<")\n+\n"<<b<<" ("<<fb.str()<<" = "<<fb.to_float()<<")\n=\n"<<fr.to_float()<<" ("<<fr.str()<<"\n\n";
		assert( down<=got && got<=up);
	}
	
}

template<class TImpl>
void test_impl(TImpl &impl)
{	
	test_add<TImpl>(impl, 1.0f,1.0f);
	test_add<TImpl>(impl, 2.0f,2.0f);
	test_add<TImpl>(impl, 1.5f,1.5f);
	test_add<TImpl>(impl, 1.1f,1.1f);
	test_add<TImpl>(impl, 1.0f,1.8f);
	test_add<TImpl>(impl, 1.0f,1.5f);
	test_add<TImpl>(impl, 1.5f,1.5f);
	
	std::vector<float> args;
	
	for(float a=0; a<=+5; a++){
	    args.push_back(a);
	}
	
	args.push_back(+INFINITY);
	args.push_back(-INFINITY);
	
	float oU=1.0f, oD=1.0f;
	for(int i=0;i<20;i++){
		oU=nextafterf(oU,2);
		oD=nextafterf(oD,0);
		args.push_back(oU);
		args.push_back(oU/2);
		args.push_back(oU/4);
		args.push_back(-oU);
		args.push_back(oD);
		args.push_back(oD/2);
		args.push_back(oD/4);
		args.push_back(-oD);
	}

	for(int i=0; i<1000; i++){
		float b=urng(rng);
		args.push_back(b);
	}

	for(int i=0; i<1000; i++){
	    float a=grng(rng);
	        
	    args.push_back(a);
	}
	
	for(int i=0; i<1000; i++){
	    float a=erng(rng);
	        
	    args.push_back(a);
	}

	for(int i=0; i<1000; i++){
		float a=grng(rng)/grng(rng);
		
		args.push_back(a);
	}
	
	for(int i=0; i<args.size(); i++){
		std::cerr<<"i = "<<i<<"\n";
		for(int j=0; j<args.size(); j++){
			test_add(impl, args[i], args[j]);
		}	
	}
}


int main()
{
	
	fprintf(stderr, "WARNING : This only appears to be faithfully rounded.\n");

    test_impl(add<8,23,8,23,8,23>);
  
	fprintf(stderr, "Done\n");
	fprintf(stderr, "WARNING : This only appears to be faithfully rounded.\n");

    return 0;
}
	
