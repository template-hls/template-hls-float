#include "thls/tops/ct/ct_vec_unique.hpp"
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
    static const int N = 8;

    typedef create_ct_vec<N, std::integral_constant<int,0> >::type zeros;

    typedef ct_vec_unique<zeros>::type zeros_unique;

    ct_vec_for_each<zeros>(dump());
    ct_vec_for_each<zeros_unique>(dump());

    typedef create_ct_vec_rotation<1,N>::type iota;
    typedef ct_vec_unique<iota>::type iota_unique;
    ct_vec_for_each<iota>(dump());
    ct_vec_for_each<iota_unique>(dump());
    
    return 0;
}
