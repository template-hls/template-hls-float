#ifndef thls_tops_ct_rng_lcg_mod_2y32_hpp
#define thls_tops_ct_rng_lcg_mod_2y32_hpp

#include "thls/tops/ct_rng.hpp"

namespace thls
{
namespace ct_rng
{
    static const uint32_t lcg_mod_2y32_A = 1664525ul;
    static const uint32_t lcg_mod_2y32_C = 1013904223ul;

/*
    Calculates mod(A^steps,M), where steps can be be positive
    or negative.

    Requires O(log steps) template expansions.

    For negative steps, there must be a specialisation of
    inv_mod<A,M> (or a general implementation).
    
    x'  = mod(x*A+C,M)
    
    |x| = |A C| |x|
    |1|   |0 1| |1|
    
    T =    |A C|
           |0 1|
    
    T^2 =  |A C| |A C|  =  |A^2  A*C+C|
           |0 1| |0 1|     | 0     1  |
    
    T1*T2= |A1 C1| |A2 C2|  =  |A1*A2  A1*C2+C1|
           |0   1| |0   1|     | 0        1    |

*/
template<int64_t stepsOrig,uint32_t A,uint32_t C>
struct lcg_mod_2y32_multiplier
{
protected:
    static const int64_t steps = (stepsOrig < 0) ? ((int64_t(1)<<32)+stepsOrig) : stepsOrig;

    typedef lcg_mod_2y32_multiplier<steps/2,A,C> prev;
    
    static const uint32_t A1 = prev::vA * prev::vA;
    static const uint32_t C1 = prev::vA * prev::vC + prev::vC;

    typedef lcg_mod_2y32_multiplier<steps%2,A,C> curr;
    static const uint32_t A2 = curr::vA;
    static const uint32_t C2 = curr::vC;

public:    
    static const uint32_t vA = A1*A2;
    static const uint32_t vC = A1*C2+C1;
};


template<uint32_t A, uint32_t C>
struct lcg_mod_2y32_multiplier<0,A,C>
{
    static const uint32_t vA = 1;
    static const uint32_t vC = 0;
};

template<uint32_t A, uint32_t C>
struct lcg_mod_2y32_multiplier<1,A,C>
{
    static const uint32_t vA = A;
    static const uint32_t vC = C;
};


template<uint32_t X,uint32_t A,uint32_t C>
struct lcg_mod_2y32_state
{
public:
    static const uint32_t value = X;
};



template<uint32_t S=949333985>
struct make_quick_and_dirty
{
    typedef lcg_mod_2y32_state<S,lcg_mod_2y32_A,lcg_mod_2y32_C> value;
};


template<uint32_t S,uint32_t A,uint32_t C,int distance>
struct skip_by<lcg_mod_2y32_state<S,A,C>, distance >
{
private:
    // These templates are (hopefully) shared amongst all instances
    static const uint32_t Ai=lcg_mod_2y32_multiplier<distance,A,C>::vA;
    static const uint32_t Ci=lcg_mod_2y32_multiplier<distance,A,C>::vC;

    static const uint32_t Si=S*Ai + Ci;
public:    
    typedef lcg_mod_2y32_state<Si,A,C> value;
};


template<>
struct verify_ct_rng_fixed<make_quick_and_dirty<>::value>
{
    /*  T:matrix([1664525,1013904223],[0,1]);
        x0:matrix([949333985,1]);
    */
    
    static const uint32_t value1 = 3977611212ul; // mod(T.x0,2^32)[1][1]
    static const uint32_t value2 = 495853755ul;
    static const uint32_t value19937 = 513342252ul; // mod( (T^^19937).x0,2^32)[1][1];
    static const uint32_t value2y24 = 563458017ul; // mod( mod(mod(mod(T^^256,2^32)^^256,2^32)^^256,2^32).x0,2^32)[1][1];
};


}; // ct_rng
}; // thls

template struct thls::ct_rng::verify_ct_rng<thls::ct_rng::make_quick_and_dirty<>::value>;

#endif
