#ifndef thls_tops_ct_rng_lcg_mod_p_hpp
#define thls_tops_ct_rng_lcg_mod_p_hpp

#include "thls/tops/ct/ct_rng.hpp"

namespace thls
{
namespace ct
{

template<uint32_t X,uint32_t M>
struct inv_mod;

static const uint32_t lcg_mod_p_A1=1588635695ul; // From TABLES OF LINEAR CONGRUENTIAL GENERATORS OF DIFFERENT SIZES AND GOOD LATTICE STRUCTURE
static const uint32_t lcg_mod_p_A1inv=3870709308ul;
static const uint32_t lcg_mod_p_M1=4294967291ul; // 2^32-5
    
template<>
struct inv_mod<lcg_mod_p_A1,lcg_mod_p_M1>
{ static const uint32_t value = lcg_mod_p_A1inv; };

template<>
struct inv_mod<lcg_mod_p_A1inv,lcg_mod_p_M1>
{ static const uint32_t value = lcg_mod_p_A1; };

static const uint32_t lcg_mod_p_A2=16807ul; // minstdrand
static const uint32_t lcg_mod_p_A2inv=1407677000ul;
static const uint32_t lcg_mod_p_M2=2147483647ul; // 2^31-1
    
template<>
struct inv_mod<lcg_mod_p_A2,lcg_mod_p_M2>
{ static const uint32_t value = lcg_mod_p_A2inv; };

template<>
struct inv_mod<lcg_mod_p_A2inv,lcg_mod_p_M2>
{ static const uint32_t value = lcg_mod_p_A2; };


/*
    Calculates mod(A^steps,M), where steps can be be positive
    or negative.

    Requires O(log steps) template expansions.

    For negative steps, there must be a specialisation of
    inv_mod<A,M> (or a general implementation).

*/
template<int steps,uint32_t A, uint32_t M>
struct lcg_mod_p_multiplier
{
protected:
    static const uint32_t effA = (steps < 0) ? inv_mod<A,M>::value : A;
    static const uint32_t effSteps = (steps < 0) ? -steps : steps;
    
    static const uint64_t prev = lcg_mod_p_multiplier<effSteps/2,effA,M>::value;
    static const uint64_t prev_sqr = (prev*prev)%M;
public:    
    static const uint32_t value = uint32_t( ((prev_sqr * ((effSteps&1)?effA:1))) % M );
};


template<uint32_t A, uint32_t M>
struct lcg_mod_p_multiplier<0,A,M>
{
    static const uint32_t value = 1;
};


template<uint32_t X,uint32_t A,uint32_t M>
struct lcg_mod_p_state
{
public:
    static const uint32_t value = X;
};



template<uint32_t S=2013771743>
struct make_minstdrand0
{
    static_assert((S%lcg_mod_p_M2)!=0, "Seed must not be zero.");
    
    typedef lcg_mod_p_state<(S%lcg_mod_p_M2),lcg_mod_p_A2,lcg_mod_p_M2> type;
};

template<uint32_t S=3922919429>
struct make_lcg_mod_2y32m5
{
    static_assert((S%lcg_mod_p_M1)!=0, "Seed must not be zero.");
    
    typedef lcg_mod_p_state<(S%lcg_mod_p_M1),lcg_mod_p_A1,lcg_mod_p_M1> type;
};



template<uint32_t S,uint32_t A,uint32_t M,int distance>
struct skip_by<lcg_mod_p_state<S,A,M>, distance >
{
private:
    // These templates are (hopefully) shared amongst all instances
    static const uint32_t Ai=lcg_mod_p_multiplier<distance,A,M>::value;

    static const uint32_t S2=uint32_t( (uint64_t(S) * Ai) % M );
public:    
    typedef lcg_mod_p_state<S2,A,M> type;
};


template<>
struct verify_ct_rng_fixed<make_minstdrand0<>::type>
{
    static const uint32_t value1 = 1119407881ul; // mod(2013771743*power_mod(16807,1,2147483647),2147483647)
    static const uint32_t value2 = 1931508247ul;
    static const uint32_t value19937 = 1009599722ul;
    static const uint32_t value2y24 = 246285303ul;
};


template<>
struct verify_ct_rng_fixed<make_lcg_mod_2y32m5<>::type>
{
    static const uint32_t value1 = 4020269147ul; // mod(3922919429*power_mod(1588635695,1,4294967291),4294967291)
    static const uint32_t value2 = 2884350374ul;
    static const uint32_t value19937 = 3384465501ul;
    static const uint32_t value2y24 = 906608749ul;
};


}; // ct_rng
}; // thls

template struct thls::ct::verify_ct_rng<thls::ct::make_minstdrand0<>::type>;

#endif
