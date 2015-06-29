#ifndef ct_bigint_add_hpp
#define ct_bigint_add_hpp

#include "ct_bigint_core.hpp"

namespace thls
{

template<class TA,class TB>
struct ct_add;

template<>
struct ct_add<ct_zero,ct_zero>
{
    HLS_SC(limb_t,carry,0);
    typedef ct_zero value;  
};

template<limb_t L,class T>
struct ct_add<ct_zero,ct_limb<L,T> >
{
    HLS_SC(limb_t,carry,0);
    typedef ct_limb<L,T> value;  
};

template<limb_t L,class T>
struct ct_add<ct_limb<L,T>,ct_zero >
{
    HLS_SC(limb_t,carry,0);
    typedef ct_limb<L,T> value;  
};

template<class TA,class TB>
struct ct_add
{
private:
    // Add an extra digit for any carry
    enum{ digits = 1+ TA::digit_pos > TB::digit_pos ? TA::digit_pos : TB::digit_pos };
    
    typedef typename ct_ensure_digits<TA,digits>::value FA;
    typedef typename ct_ensure_digits<TB,digits>::value FB;
    
    template<int x,class A, class B>
    struct worker
    {
        typedef worker<0,typename A::tail,typename B::tail> lower;
        
        HLS_SC(limb_t,full_sum,(lower::carry)+(A::limb)+(B::limb));
        HLS_SC(limb_t,carry,full_sum>>limb_bits);
        HLS_SC(limb_t,sum,full_sum&limb_mask);
        
        typedef ct_limb<sum,typename lower::value> value;
    };
    
    template<int x>
    struct worker<x,ct_zero,ct_zero>
    {
        HLS_SC(limb_t,carry,0);
        typedef ct_zero value;
    };
    
    typedef typename worker<0,FA,FB>::value raw;
    
public:
    typedef typename ct_canonicalise<raw>::value value;
};

}; // thls


#endif
