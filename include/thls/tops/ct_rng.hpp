#ifndef thls_tops_ct_rng_hpp
#define thls_tops_ct_rng_hpp

#include <cstdint>
#include <iostream>
#include <cstdlib>

/*
    Rationale
    
    We want something that can evaluate at compile-time, and
    allows us to sample an RNG at specific offsets, as well as
    skip. We also want to avoid high template recursion depths
    where possible.
    
    So the interface looks like:
    
    typedef make_rng<SEED>::type rng_inst; // Create rng seeded at SEED (uint32_t)
    
    uint32_t value=rng_inst::value; // Value at i=0 (no stepping)
    
    uint32_t v10=sample_at<rng_inst>::value; // Value at i=10
    
    typedef skip_by<rng_inst,10>::value rng_inst10;
    
    assert(rng_inst10::value == v10);

*/

namespace thls
{
namespace ct_rng
{

template<class T,int distance>
struct skip_by;

template<class T,int distance>
struct sample_at
{
private:    
    typedef typename skip_by<T,distance>::value skipped;
public:
    static const uint32_t value = skipped::value;
};


template<class rng0>
struct verify_ct_rng_fixed;
/*{
    static const uint32_t value1 = ...;
    static const uint32_t value2 = ...;
    static const uint32_t value19937 = ...;
};*/


template<class rng0>
struct verify_ct_rng
{
#define verify_ct_rng_CHECK_EQUAL(a,b) \
    if((a)!=(b)){ \
        std::cerr<<"Fail : "<<__LINE__<<", "<< #a << " != " << #b << ", "<<a<<" vs "<<b<<"\n"; \
        exit(1); \
    }
    
#define verify_ct_rng_CHECK_UNEQUAL(a,b) \
    if((a)==(b)){ \
        std::cerr<<"Fail : "<<__LINE__<<", "<< #a << " != " << #b << ", "<<a<<" vs "<<b<<"\n"; \
        exit(1); \
    }
    
    static void run()
    {
        static const uint32_t value0 = rng0::value;
        
        typedef typename skip_by<rng0,0>::value rng0_id;
        verify_ct_rng_CHECK_EQUAL(rng0_id::value , value0);
        
        typedef typename skip_by<rng0,1>::value rng1;
        static const uint32_t value1 = rng1::value;    
        verify_ct_rng_CHECK_UNEQUAL(value1 , value0);
        verify_ct_rng_CHECK_EQUAL(verify_ct_rng_fixed<rng0>::value1, value1);
        
        typedef typename skip_by<rng0,2>::value rng2;
        typedef typename skip_by<rng1,1>::value rng2_step;
        verify_ct_rng_CHECK_EQUAL(rng2::value , rng2_step::value);
        verify_ct_rng_CHECK_EQUAL(rng2::value , (sample_at<rng0,2>::value));
        verify_ct_rng_CHECK_EQUAL(verify_ct_rng_fixed<rng0>::value2, rng2::value);
        
        typedef typename skip_by<rng0,3>::value rng3;
        typedef typename skip_by<rng2,1>::value rng3_step;
        verify_ct_rng_CHECK_EQUAL(rng3::value , rng3_step::value);
        verify_ct_rng_CHECK_EQUAL(rng3::value , (sample_at<rng0,3>::value));
        
        typedef typename skip_by<rng0,4>::value rng4;
        typedef typename skip_by<rng3,1>::value rng4_step;
        verify_ct_rng_CHECK_EQUAL(rng4::value , rng4_step::value);
        verify_ct_rng_CHECK_EQUAL(rng4::value , (sample_at<rng0,4>::value));
        
        typedef typename skip_by<rng0,5>::value rng5;
        typedef typename skip_by<rng4,1>::value rng5_step;
        verify_ct_rng_CHECK_EQUAL(rng5::value , rng5_step::value);
        verify_ct_rng_CHECK_EQUAL(rng5::value , (sample_at<rng0,5>::value));
        
        typedef typename skip_by<rng0,6>::value rng6;
        typedef typename skip_by<rng5,1>::value rng6_step;
        verify_ct_rng_CHECK_EQUAL(rng6::value , rng6_step::value);
        verify_ct_rng_CHECK_EQUAL(rng6::value , (sample_at<rng0,6>::value));
        
        typedef typename skip_by<rng0,19937>::value rng19937;
        verify_ct_rng_CHECK_EQUAL(verify_ct_rng_fixed<rng0>::value19937, rng19937::value);
        
        typedef typename skip_by<rng0,(1<<24)>::value rng2y24;
        verify_ct_rng_CHECK_EQUAL(verify_ct_rng_fixed<rng0>::value2y24, rng2y24::value);
        
        std::cerr<<"(Skip 2y24 = "<<rng2y24::value<<")\n";
        
        typedef typename skip_by<rng6,-1>::value rng5_bw;
        verify_ct_rng_CHECK_EQUAL(rng5::value, rng5_bw::value);
        
        typedef typename skip_by<rng6,-5>::value rng1_bw;
        verify_ct_rng_CHECK_EQUAL(rng1::value, rng1_bw::value);
    }
    
#undef verify_ct_rng_CHECK_UNEQUAL
};


}; // ct_rng
}; // thls

#endif
