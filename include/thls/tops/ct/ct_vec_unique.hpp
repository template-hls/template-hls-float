#ifndef thls_tops_ct_vec_unique_hpp
#define thls_tops_ct_vec_unique_hpp

#include "thls/tops/ct/ct_vec.hpp"
#include "thls/tops/ct/ct_vec_create.hpp"

#include <type_traits>

namespace thls
{
namespace ct
{

////////////////////////////////////////////////////////////////////////////
// Find the lowest value above a given value, or ct_vec_sentinel if none exists

struct ct_vec_sentinel_lower;
struct ct_vec_sentinel_upper;

template<class X,class Y>
struct ct_vec_compare;

template<class T, T va, T vb>
struct ct_vec_compare<std::integral_constant<T,va>,std::integral_constant<T,vb> >
{
    static const int value =  (va<vb) ? -1 : (va>vb) ? +1 : 0;
};

template<class T>
struct ct_vec_compare<T,ct_vec_sentinel_upper>
{
    static const int value =  -1;
};

template<class T>
struct ct_vec_compare<ct_vec_sentinel_upper, T>
{
    static const int value =  +1;
};

template<>
struct ct_vec_compare<ct_vec_sentinel_upper, ct_vec_sentinel_upper>
{
    static const int value =  0;
};


        template<class T>
struct ct_vec_compare<T,ct_vec_sentinel_lower>
{
    static const int value =  +1;
};

template<class T>
struct ct_vec_compare<ct_vec_sentinel_lower, T>
{
    static const int value =  -1;
};



template<class V,class T>
struct ct_vec_next_above;

template<class V,class X>
struct ct_vec_next_above<V,ct_vec_leaf<X> >
{
    typedef typename std::conditional<
        (ct_vec_compare<V,X>::value < 0), // This leaf is bigger than V
        X,  // Found new low
        ct_vec_sentinel_upper // Found nothing
    >::type type;
};

template<class V,class L,class R>
struct ct_vec_next_above<V,ct_vec_branch<L,R> >
{
private:
    typedef typename ct_vec_next_above<V,L>::type vL;
    typedef typename ct_vec_next_above<V,R>::type vR;
public:
    typedef typename std::conditional<
        (ct_vec_compare<vL,vR>::value < 0),
        vL,
        vR
    >::type type;
};

template<class Prev,class Vec>
struct ct_vec_reduce_unique_impl
{
    typedef typename ct_vec_next_above<Prev,Vec>::type curr;
    typedef ct_vec_reduce_unique_impl<curr,Vec> next;
    
    static const int N = std::is_same<curr,ct_vec_sentinel_upper>::value ? 0 : 1+next::N;
    
    template<int I,class Acc>
    struct build
    {
    private:
        typedef typename next::template build<I+1,Acc>::type next_acc;
    public:
        typedef typename ct_vec_write<I,curr,next_acc>::type type;
    };
};

template<class Vec>
struct ct_vec_reduce_unique_impl<ct_vec_sentinel_upper,Vec> {
    static const int N = 0;

    template<int I, class Acc>
    struct build {
        typedef Acc type;
    };
};


template<class Vec>
struct ct_vec_unique
{
private:
    typedef ct_vec_reduce_unique_impl<ct_vec_sentinel_lower,Vec> impl;
    static const int N = impl::N;
    typedef typename create_ct_vec<N,std::integral_constant<int,0> >::type zeros;
public:


    typedef typename impl::template build<0,zeros>::type type;
};


}; // ct
}; // thls

#endif
