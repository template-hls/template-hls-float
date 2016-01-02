#ifndef thls_tops_ct_vec_hpp
#define thls_tops_ct_vec_hpp

#include <cassert>
#include <type_traits>

namespace thls
{
namespace ct
{


/*
    Vectors are represented by (hopefully balanaced)
    binary trees of elements, which provides O(log n) access
    for reads and writes, and hopefully keeps O(n) algorithms
    as O(n log n) compile time algorithms.

    Except... turns out we need find, which is O(n).
*/

struct ct_vec_empty
{
    static const int N=0;
};

template<class L,class R>
struct ct_vec_branch
{
    static const int N=L::N+R::N;
};

template<class T>
struct ct_vec_leaf
{
    static const int N=1;

    typedef T type;
};


////////////////////////////////////////////////////////////////////////////
//! Read element I from vector T in O(log n) expansions
template<int I,class T>
struct ct_vec_read;

template<int I, class V>
struct ct_vec_read<I,ct_vec_leaf<V> >
{
    //static_assert(I==0, "Index out of range.");
    
    typedef V type;
};

template<int I, class L, class R>
struct ct_vec_read<I,ct_vec_branch<L,R> >
{
    //static_assert(0<=I && I<(L::N+R::N), "Index out of range.");
    
    typedef typename std::conditional<(I < L::N) ,
            typename ct_vec_read<I,L>::type ,
            typename ct_vec_read<I-L::N,R>::type
    >::type type;
};


////////////////////////////////////////////////////////////////////////////////
//! Write value W to element I of vector T in O(log n) expansions
template<int I,class W,class T>
struct ct_vec_write;

template<int I,class W,class V>
struct ct_vec_write<I,W,ct_vec_leaf<V> >
{
    //static_assert(I==0, "Index out of range.");
    
    typedef ct_vec_leaf<W> type;
};

template<int I,class W,class L,class R>
struct ct_vec_write<I,W,ct_vec_branch<L,R> >
{
private:
    //static_assert(0<=I && I<(L::N+R::N), "Index out of range.");

    typedef typename std::conditional< (I<L::N) , typename ct_vec_write<I,W,L>::type, L>::type left;
    typedef typename std::conditional< (I<L::N) , R, typename ct_vec_write<I-L::N,W,R>::type >::type right;
public:
    typedef ct_vec_branch<left,right> type;
};





//////////////////////////////////////////////////////////////////////////////////
//! Swap elements I0 and I1 of element T in O(log n) expansions
template<int I0,int I1,class T>
struct ct_vec_swap
{
    typedef typename ct_vec_read<I0,T>::type V0; // O(log n)
    typedef typename ct_vec_read<I1,T>::type V1; // O(log n)
    
    typedef typename ct_vec_write<I0,V1, typename ct_vec_write<I1,V0,T>::type>::type type;  // O(log n)
};



}; // ct_rng
}; // thls

#endif
