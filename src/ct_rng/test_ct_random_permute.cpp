#include "thls/tops/ct/ct_vec_permute.hpp"
#include "thls/tops/ct/ct_vec_create.hpp"
#include "thls/tops/ct/ct_vec_algorithms.hpp"

#include "thls/tops/ct/ct_rng_lcg_mod_2y32.hpp"

using namespace thls::ct;

struct dump
{
    template<class T,int V>
    void operator()(int i, const std::integral_constant<T,V> *)
    {
        std::cerr<<i<<" = "<<V<<"\n";
    }
};

int main()
{
    typedef make_quick_and_dirty<>::type rng;
    
    static const int N = 32;
    
    typedef create_ct_vec_rotation<1,N>::type iota;

    ct_vec_for_each<iota>(dump());

    typedef ct_vec_permute<rng,iota>::type perm;

    ct_vec_for_each<perm>(dump());
    
    return 0;
}
