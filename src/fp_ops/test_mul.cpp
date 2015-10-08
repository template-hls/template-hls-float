#include "thls/tops/fp_flopoco_mul_v1.hpp"

#ifndef __SYNTHESIS__
#include <random>
#endif

#include <stdint.h>

ap_uint<34> hls_mul(ap_uint<34> a, ap_uint<34> b)
{
#pragma HLS INTERFACE ap_ctrl_none register port=return
#pragma HLS PIPELINE

	fp_flopoco<8,23> fr=mul<8,23>(fp_flopoco<8,23>(fw_uint<34>(a)), fp_flopoco<8,23>(fw_uint<34>(b)));
	return fr.bits.bits;
	//return a+b;
}

ap_uint<66> hls_mul_dbl(ap_uint<66> a, ap_uint<66> b)
{
#pragma HLS INTERFACE ap_ctrl_none register port=return
#pragma HLS PIPELINE

	fp_flopoco<11,52> fr=mul<11,52>(fp_flopoco<11,52>(fw_uint<66>(a)), fp_flopoco<11,52>(fw_uint<66>(b)));
	return fr.bits.bits;
	//return a+b;
}

ap_uint<34> hls_mul_positive(ap_uint<31> a, ap_uint<31> b)
{
#pragma HLS INTERFACE ap_ctrl_none register port=return
#pragma HLS PIPELINE

	fw_uint<34> fa( ap_uint<34>( (ap_uint<3>(0b010),a) ) ), fb( ap_uint<34>( (ap_uint<3>(0b010),b) ) );

	fp_flopoco<8,23> fr=mul<8,23>(fp_flopoco<8,23>(fa), fp_flopoco<8,23>(fb));
	return fr.bits.bits;
	//return a+b;
}

ap_uint<34> hls_mul_v3(ap_uint<34> a, ap_uint<34> b)
{
#pragma HLS INTERFACE ap_ctrl_none register port=return
#pragma HLS PIPELINE

	fp_flopoco<8,23> fr=mul_v3<8,23>(fp_flopoco<8,23>(fw_uint<34>(a)), fp_flopoco<8,23>(fw_uint<34>(b)));
	return fr.bits.bits;
	//return a+b;
}

ap_uint<34> hls_mul4(ap_uint<34> a, ap_uint<34> b, ap_uint<34> c, ap_uint<34> d)
{
#pragma HLS INTERFACE ap_ctrl_none register port=return
#pragma HLS PIPELINE

	fp_flopoco<8,23> ab=mul<8,23>(fp_flopoco<8,23>(fw_uint<34>(a)), fp_flopoco<8,23>(fw_uint<34>(b)));
	fp_flopoco<8,23> cd=mul<8,23>(fp_flopoco<8,23>(fw_uint<34>(c)), fp_flopoco<8,23>(fw_uint<34>(d)));
	fp_flopoco<8,23> abcd=mul<8,23>(ab,cd);
	return abcd.bits.bits;
	//return a+b;
}

float ip_mul(float a, float b)
{
#pragma HLS INTERFACE ap_ctrl_none register port=return
#pragma HLS PIPELINE

	return a*b;
}

double ip_mul_dbl(double a, double b)
{
#pragma HLS INTERFACE ap_ctrl_none register port=return
#pragma HLS PIPELINE

	return a*b;
}

float ip_mul4(float a, float b, float c, float d)
{
#pragma HLS INTERFACE ap_ctrl_none register port=return
#pragma HLS PIPELINE

	return a*b*c*d;
}

#ifndef __SYNTHESIS__
void test_bits()
{
    fw_uint<1> t(1), f(0);

    assert(t!=f);
    assert(t==1);
    assert(f==0);

    assert( get_bit<0>(t)==1);
    assert(get_bit<1>(t)==0);

    assert(get_bit<0>(f)==0);
    assert(get_bit<1>(f)==0);

    fw_uint<8> alt(0b10101010);

    assert(get_bit<0>(alt)==0);
    assert(get_bit<1>(alt)==1);
    assert(get_bit<6>(alt)==0);
    assert(get_bit<7>(alt)==1);

    assert( (get_bits<7,0>(alt)==alt) );
    assert( (get_bits<6,3>(alt)==0b0101) );
    assert( (get_bits<7,4>(alt)==0b1010) );
}

std::mt19937 rng;
std::uniform_real_distribution<float> urng;
std::normal_distribution<float> grng;

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
    test_bits();

    for(float a=0;a<10;a++){
        fp_flopoco<8,23> fa(a);

        #ifndef __SYNTHESIS__
        std::cerr<<"a="<<a<<", fa="<<fa.str()<<" \n    = "<<fa.to_float()<<"\n";
        #endif
    }


    test_impl(mul<8,23,8,23,8,23>);
    test_impl(mul_v2<8,23,8,23,8,23>);
    test_impl(mul_v3<8,23,8,23,8,23>);



    return 0;
}
#endif
