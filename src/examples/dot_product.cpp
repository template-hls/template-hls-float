#include "thls/tops/policy_flopoco.hpp"

#include <random>

typedef thls::policy_flopoco<8,23> fp_policy_t;

typedef fp_policy_t::value_t fp_t;

float dot_product(
    int n,
    const float *a,
    const float *b
){
    fp_t acc;
    for(int i=0; i<n; i++){
        //std::cerr<<"   x : "<<data[i]<<"\n";
        fp_t fa=thls::to<fp_t>( a[i] );
        fp_t fb=thls::to<fp_t>( b[i] );
        //std::cerr<<"  fx : "<<fx.raw.bits<<"\n";
        acc += fa*fb;
        //std::cerr<<"   a : "<<acc.raw.bits<<"\n";
    }
    return thls::to<float>(acc);
}

int main()
{
    std::mt19937 urng;
    std::uniform_real_distribution<float> udist;
    
    for(int n=0; n<1000; n++){
        std::vector<float> va, vb;
        for(int i=0; i<n; i++){
            float u=udist(urng);
            float a=+1;
            if(u>=0.5){
                a=-1;
                u=u-0.5;
            }
            if(u>0.25){
                a=a*udist(urng);
            }else{
                a=a*expf(udist(urng));
            }
            va.push_back(a);

            u=udist(urng);
            a=+1;
            if(u>=0.5){
                a=-1;
                u=u-0.5;
            }
            if(u>0.25){
                a=a*udist(urng);
            }else{
                a=a*expf(udist(urng));
            }
            vb.push_back(a);
        }
        
        float ref=0.0f;
        for(unsigned i=0; i<n; i++){
            float ab=va[i]*vb[i];
            ref += ab;
        }
        float got=dot_product(n, &va[0], &vb[0] );
        
        fprintf(stderr, "%u : %f, %f, %g\n", n, ref, got, ref-got);
    }
    return 0;
}
