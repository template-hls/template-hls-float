#include "thls/core/ct_bigint.hpp"

#include <boost/mpl/list.hpp>
#include <boost/mpl/for_each.hpp>

typedef ct_zero zero;
typedef ct_limb<1,zero> plus_one;
typedef ct_limb<2,zero> plus_two;
typedef ct_limb<3,zero> plus_three;
typedef ct_neg<plus_one> neg_one;

typedef ct_limb<1,plus_one> plus_2p16;
typedef ct_neg<plus_2p16> neg_2p16;

typedef ct_limb<1,plus_2p16> plus_2p32;

typedef boost::mpl::list<zero,plus_one>/*,plus_two,plus_three,plus_2p16,plus_2p32>*/ numbers;

template<class A>
struct level1
{
    template<class B>
    void operator()(const B &)
    {
        //typedef typename ct_add<A,B>::value add;
        
        rt_wideint_t va=A::value(), vb=B::value();
        
        std::cerr<<"A = "<<va<<", B="<<vb<<"\n";
        
        assert(va<vb == (ct_less_than<A,B>::value));
        assert(va>vb == (ct_greater_than<A,B>::value));
        assert((va==vb) == (ct_equals<A,B>::value));
        
        rt_wideint_t sum=va+vb;
        
        typedef typename ct_add<A,B>::value sum_t;
        
        assert(sum==sum_t::value());
        
        rt_wideint_t prod=va*vb;
        
        typedef typename ct_mul<A,B>::value mul_t;
        
        std::cerr<<" A*B = "<<mul_t::value()<<"\n";
        
        assert(prod==mul_t::value());
    }    
};

struct level2
{
    template<class A>
    void operator()(const A &)
    {
        level1<A> op;
        
        boost::mpl::for_each<numbers>(
            op
        );
    }    
};

int main()
{
    level2 lev;
    
    boost::mpl::for_each<numbers>(
        lev
    );    
  
    return 0;    
};


