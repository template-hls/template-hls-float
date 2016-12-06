#include "thls/tops/policy_flopoco.hpp"

#include <random>

typedef thls::policy_flopoco<8,23> fp_policy_t;

typedef fp_policy_t::value_t fp_t;

float sum(
    int n,
    const float *data
){
    fp_t acc;
    for(int i=0; i<n; i++){
        //std::cerr<<"   x : "<<data[i]<<"\n";
        fp_t fx=thls::to<fp_t>( data[i] );
        //std::cerr<<"  fx : "<<fx.raw.bits<<"\n";
        acc += fx;
        //std::cerr<<"   a : "<<acc.raw.bits<<"\n";
    }
    return thls::to<float>(acc);
}

int main()
{
    std::mt19937 urng;
    std::uniform_real_distribution<float> udist;
    
    for(int n=0; n<1000; n++){
        std::vector<float> data;
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
            data.push_back(a);
        }
        
        float ref=std::accumulate(data.begin(), data.end(), 0.0f);
        float got=sum(n, &data[0]);
        
        fprintf(stderr, "%u : %f, %f, %g\n", n, ref, got, ref-got);
    }
    return 0;
}
