#ifndef thls_tops_ct_create_hpp
#define thls_tops_ct_create_hpp

#include "thls/tops/ct/ct_vec.hpp"

namespace thls
{
namespace ct
{

///////////////////////////////////////////////////////////////////////
/* Create a vector of values in O(n) expansions */
template<int N,class V>
struct create_ct_vec
{
    typedef ct_vec_branch<
        typename create_ct_vec<N/2,V>::type,
        typename create_ct_vec<N-N/2,V>::type
    > type;
};

template<class V>
struct create_ct_vec<1,V>
{
    typedef ct_vec_leaf<V> type;
};

template<class V>
struct create_ct_vec<0,V>
{
    typedef ct_vec_empty type;
};


/////////////////////////////////////////////////////////////////////////
/*
for(int i=0;i<r;i++){
    cycle[i]=perm[i]=(i+1)%r;
}
Takes O(n) expansions
*/

template<int O, int LocN,int ModN>
struct create_ct_vec_rotation_impl
{
    typedef ct_vec_branch<
        typename create_ct_vec_rotation_impl<O,LocN/2,ModN>::type,
        typename create_ct_vec_rotation_impl<O+LocN/2,LocN-LocN/2,ModN>::type
    > type;
};

template<int O, int ModN>
struct create_ct_vec_rotation_impl<O,1,ModN>
{
    typedef ct_vec_leaf< std::integral_constant<int,O%ModN> > type;
};

template<int O,int N>
struct create_ct_vec_rotation
{
    typedef typename create_ct_vec_rotation_impl<O,N,N>::type type;
};


}; // ct
}; // thls

#endif
