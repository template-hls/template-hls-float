#include "mul.hpp"

#include <random>

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

int main()
{
    test_bits();
    
    for(float a=0;a<10;a++){
        fp_flopoco<8,23> fa(a);
        
        #ifndef __SYNTHESIS__
        std::cerr<<"a="<<a<<", fa="<<fa.str()<<" \n    = "<<fa.to_float()<<"\n";
        #endif
    }
    
    
    
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
        
        #ifndef __SYNTHESIS__
        std::cerr<<a<<" ("<<fa.str()<<" = "<<fa.to_float()<<") * "<<b<<" ("<<fb.str()<<" = "<<fb.to_float()<<") \n    = "<<got.to_float()<<" ("<<got.str()<<"\n\n";
        #endif
        
        if(got.bits!=want.bits){
            #ifndef __SYNTHESIS__
            std::cerr<<"   got="<<got.str()<<"\n";
            std::cerr<<"  want="<<want.str()<<"\n";
            #endif
            assert(0);
        }
        
    }
    
    for(int i=0; i<10000; i++){
        float a=grng(rng);
        float b=grng(rng);
        
        fp_flopoco<8,23> fa(a), fb(b);
        
        fp_flopoco<8,23> got=mul<8,23>(fa,fb);
        fp_flopoco<8,23> want(a*b);
        
        #ifndef __SYNTHESIS__
        std::cerr<<a<<" ("<<fa.str()<<" = "<<fa.to_float()<<") * "<<b<<" ("<<fb.str()<<" = "<<fb.to_float()<<") \n    = "<<got.to_float()<<" ("<<got.str()<<"\n\n";
        #endif
        
        if(got.bits!=want.bits){
            #ifndef __SYNTHESIS__
            std::cerr<<"   got="<<got.str()<<"\n";
            std::cerr<<"  want="<<want.str()<<"\n";
            #endif
            assert(0);
        }
        
    }
    
    for(int i=0; i<10000; i++){
        float a=grng(rng)/grng(rng);
        float b=grng(rng)/grng(rng);
        
        fp_flopoco<8,23> fa(a), fb(b);
        
        fp_flopoco<8,23> got=mul<8,23>(fa,fb);
        fp_flopoco<8,23> want(a*b);
        
        #ifndef __SYNTHESIS__
        std::cerr<<a<<" ("<<fa.str()<<" = "<<fa.to_float()<<") * "<<b<<" ("<<fb.str()<<" = "<<fb.to_float()<<") \n    = "<<got.to_float()<<" ("<<got.str()<<"\n\n";
        #endif
        
        if(got.bits!=want.bits){
            #ifndef __SYNTHESIS__
            std::cerr<<"   got="<<got.str()<<"\n";
            std::cerr<<"  want="<<want.str()<<"\n";
            #endif
            assert(0);
        }
        
    }
    
    
    return 0;
}
