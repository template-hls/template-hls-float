#ifndef ct_bigint_mul_hpp
#define ct_bigint_mul_hpp

#include "ct_bigint_core.hpp"

namespace thls
{
    
namespace detail
{
    template<class TA, limb_t LB>
    struct ct_mul_single
    {
        template<int x,class A,limb_t B>
        struct working
        {
            typedef working<x,typename A::tail,B> lower;
            
            HLS_SC(limb_t,full,(A::limb)*B+(lower::carry));
            HLS_SC(limb_t,carry,full>>limb_bits);
            HLS_SC(limb_t,prod,full&limb_bits);
            
            typedef ct_limb<prod,typename lower::value> value;
        };
        
        template<int x,limb_t B>
        struct working<x,ct_zero,B>
        {
            HLS_SC(limb_t,carry,0);
            typedef ct_zero value;
        };
        
        typedef working<0, TA, LB> lower;
        typedef ct_limb<lower::carry,typename lower::value> raw;
    public:
        typedef typename ct_canonicalise<raw>::value value;
    };
};    

template<class TA,class TB>
struct ct_mul
{
public:
    BOOST_STATIC_ASSERT(false, "This should never get invoked, missing a specialisation, or invalid input types.");
};

template<class TA,limb_t LB,class TB>
struct ct_mul<TA,ct_limb<LB,TB> >
{
private:
    typedef typename detail::ct_mul_single<TA,LB>::value partial;
    typedef typename ct_mul<TA,TB>::value lower;

    typedef typename detail::ct_shift_left_limbs<partial,1>::value aligned;
public:
    typedef typename ct_add<lower,aligned>::value value;
};


template<class TA>
struct ct_mul<TA,ct_zero>
{
public:
    typedef ct_zero value;
};

}; // thls


#endif
