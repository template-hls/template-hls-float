#ifndef thls_tops_ct_algorithms_hpp
#define thls_tops_ct_algorithms_hpp

#include "thls/tops/ct/ct_vec.hpp"

namespace thls
{
namespace ct
{

////////////////////////////////////////////////////////////////////////////
// Apply a functor to all elements in order

template<class V,class F,int off>
struct ct_vec_for_each_impl;

template<class L,class R,class F,int off>
struct ct_vec_for_each_impl<ct_vec_branch<L,R>,F,off>
{
    static void go(F f)
    {
        ct_vec_for_each_impl<L,F,off>::go(f);
        ct_vec_for_each_impl<R,F,off+L::N>::go(f);
    }
};

template<class C,class F,int off>
struct ct_vec_for_each_impl<ct_vec_leaf<C>,F,off>
{
    static void go(F f)
    {
        f(off, static_cast<const C*>(nullptr));
    }
};

template<class V,class F>
void ct_vec_for_each(F f)
{
    ct_vec_for_each_impl<V,F,0>::go(f);
}


/////////////////////////////////////////////////////////
// Apply a reduction to everything in a vector

template<class V,class MapRed,int off>
struct ct_vec_map_reduce_impl;

template<class L,class R,class MapRed,int off>
struct ct_vec_map_reduce_impl<ct_vec_branch<L,R>,MapRed,off>
{
private:
    typedef ct_vec_map_reduce_impl<L,MapRed,off> left;
    typedef ct_vec_map_reduce_impl<R,MapRed,off+L::N> right;

    typedef typename left::return_type left_return;
    typedef typename right::return_type right_return;
public:
    typedef typename MapRed::template reduce_type<left_return,right_return>::type return_type;

    static return_type go(MapRed mr)
    {
        auto a=ct_vec_map_reduce_impl<L,MapRed,off>::go(mr);
        auto b=ct_vec_map_reduce_impl<R,MapRed,off+L::N>::go(mr);
        return mr.reduce(a,b);
    }
};

template<class C,class MapRed,int off>
struct ct_vec_map_reduce_impl<ct_vec_leaf<C>,MapRed,off>
{
    typedef typename MapRed::template map_type<off,C>::type return_type;

    static return_type go(MapRed mr)
    {
        return mr.template map<off,C>();
    }
};

template<class V,class MapRed>
typename ct_vec_map_reduce_impl<V,MapRed,0>::return_type ct_vec_map_reduce(MapRed mr)
{
   return ct_vec_map_reduce_impl<V,MapRed,0>::go(mr);
}


}; // ct_rng
}; // thls

#endif
