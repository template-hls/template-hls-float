#ifndef thls_tops_ct_vec_permute_hpp
#define thls_tops_ct_vec_permute_hpp

#include "thls/tops/ct/ct_rng.hpp"
#include "thls/tops/ct/ct_vec.hpp"

#include <cassert>

namespace thls
{
namespace ct
{


////////////////////////////////////////////////////////////////////////////
/*
static void Permute(uint32_t &s, vector<int> &p)
{
    for(int j=p.size();j>1;j--)
        swap(p[j-1],p[LCG(s)%j]);
    }
*/
template<int J, class Rng, class Vec>
struct ct_vec_permute_impl
{
private:
    static const int R = Rng::value>>16; // O(1)

    static_assert(J-1 < Vec::N, "Alert");
    static_assert((R%J) < Vec::N, "Alert");

    typedef typename ct_vec_swap<J-1, R%J, Vec>::type vec_next;  // O(log n)
    typedef typename skip_by<Rng,1>::type rng_next; // O(1)

    typedef ct_vec_permute_impl<J-1, rng_next, vec_next> next;
public:
    typedef typename next::type type;
    typedef typename next::rng rng;
};

template<class Rng, class Vec>
struct ct_vec_permute_impl<1,Rng,Vec>
{
    typedef Vec type;
    typedef Rng rng;
};

template<class Rng,class Vec>
struct ct_vec_permute
{
private:
    // n expansions, each of O(n log n)
    typedef ct_vec_permute_impl<Vec::N,Rng,Vec> res;
public:
    typedef typename res::type type;
    typedef typename res::rng rng;
};



}; // ct
}; // thls

#endif
