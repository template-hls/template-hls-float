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

typedef ct_limb<7,plus_2p32> plus_xp48;

typedef ct_limb<11,plus_xp48> plus_xp64;

//typedef boost::mpl::list<zero,plus_2p16,plus_2p32> numbers;
//typedef boost::mpl::list<zero,plus_2p16,plus_two, plus_three> numbers;
typedef boost::mpl::list<zero,plus_one,plus_two,plus_three,plus_2p16,plus_2p32,plus_xp48,plus_xp64> numbers;

template<class A>
struct level1
{
    template<class B>
    void operator()(const B &)
    {
        //typedef typename ct_add<A,B>::value add;
        
        rt_wideint_t va=A::value(), vb=B::value();
        
        std::cerr<<"A = "<<va<<", B="<<vb<<"\n";
        fprintf(stderr, "a = ");
        A::print(stderr);
        fprintf(stderr, ", b=");
        B::print(stderr);
        fprintf(stderr, "\n");
        
        assert(va<vb == (ct_less_than<A,B>::value));
        assert(va>vb == (ct_greater_than<A,B>::value));
        assert((va==vb) == (ct_equals<A,B>::value));
        
        rt_wideint_t sum=va+vb;
        
        typedef typename ct_add<A,B>::value sum_t;
        
        assert(sum==sum_t::value());
        
        rt_wideint_t prod=va*vb;
        
        typedef ct_mul<A,B> mul_impl_t;
        typedef typename mul_impl_t::value mul_t;
        
        std::cerr<<" A*B = "<<mul_t::value()<<"\n";
        assert(prod==mul_t::value());
        
        typedef ct_shift_right<mul_t,1> sr1_impl_t;
        typedef typename sr1_impl_t::value sr1_t;
        
        rt_wideint_t sr1=prod>>1;
        
        std::cerr<<" ((A*B)>>1) = "<<sr1_t::value()<<"\n";
        std::cerr<<"  coarse="<<sr1_impl_t::coarse::value()<<"\n";
        std::cerr<<"  fine="<<sr1_impl_t::fine::value()<<"\n";
        assert(sr1==sr1_t::value());
        
        typedef ct_shift_right<mul_t,15> sr15_impl_t;
        typedef typename sr15_impl_t::value sr15_t;
        
        rt_wideint_t sr15=prod>>15;
        
        std::cerr<<" ((A*B)>>15) = "<<sr15_t::value()<<"\n";
        std::cerr<<"  coarse="<<sr15_impl_t::coarse::value()<<"\n";
        std::cerr<<"  fine="<<sr15_impl_t::fine::value()<<"\n";
        assert(sr15==sr15_t::value());
        
        typedef ct_shift_right<mul_t,16> sr16_impl_t;
        typedef typename sr16_impl_t::value sr16_t;
        
        rt_wideint_t sr16=prod>>16;
        
        std::cerr<<" ((A*B)>>16) = "<<sr16_t::value()<<"\n";
        std::cerr<<"  coarse="<<sr16_impl_t::coarse::value()<<"\n";
        std::cerr<<"  fine="<<sr16_impl_t::fine::value()<<"\n";
        assert(sr16==sr16_t::value());
        
        typedef ct_shift_right<mul_t,17> sr17_impl_t;
        typedef typename sr17_impl_t::value sr17_t;
        
        rt_wideint_t sr17=prod>>17;
        
        std::cerr<<" ((A*B)>>17) = "<<sr17_t::value()<<"\n";
        std::cerr<<"  coarse="<<sr17_impl_t::coarse::value()<<"\n";
        std::cerr<<"  fine="<<sr17_impl_t::fine::value()<<"\n";
        assert(sr17==sr17_t::value());
        
        
        typedef ct_shift_left<mul_t,1> sl1_impl_t;
        typedef typename sl1_impl_t::value sl1_t;
        
        rt_wideint_t sl1=prod<<1;
        
        std::cerr<<" ((A*B)<<1) = "<<sl1_t::value()<<"\n";
        std::cerr<<"  coarse="<<sl1_impl_t::coarse::value()<<"\n";
        std::cerr<<"  fine="<<sl1_impl_t::fine::value()<<"\n";
        assert(sl1==sl1_t::value());
        
        typedef ct_shift_left<mul_t,15> sl15_impl_t;
        typedef typename sl15_impl_t::value sl15_t;
        
        rt_wideint_t sl15=prod<<15;
        
        std::cerr<<" ((A*B)<<15) = "<<sl15_t::value()<<"\n";
        std::cerr<<"  coarse="<<sl15_impl_t::coarse::value()<<"\n";
        std::cerr<<"  fine="<<sl15_impl_t::fine::value()<<"\n";
        assert(sl15==sl15_t::value());
        
        typedef ct_shift_left<mul_t,16> sl16_impl_t;
        typedef typename sl16_impl_t::value sl16_t;
        
        rt_wideint_t sl16=prod<<16;
        
        std::cerr<<" ((A*B)<<16) = "<<sl16_t::value()<<"\n";
        std::cerr<<"  coarse="<<sl16_impl_t::coarse::value()<<"\n";
        std::cerr<<"  fine="<<sl16_impl_t::fine::value()<<"\n";
        assert(sl16==sl16_t::value());
        
        typedef ct_shift_left<mul_t,17> sl17_impl_t;
        typedef typename sl17_impl_t::value sl17_t;
        
        rt_wideint_t sl17=prod<<17;
        
        std::cerr<<" ((A*B)<<17) = "<<sl17_t::value()<<"\n";
        std::cerr<<"  coarse="<<sl17_impl_t::coarse::value()<<"\n";
        std::cerr<<"  fine="<<sl17_impl_t::fine::value()<<"\n";
        assert(sl17==sl17_t::value());
        
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


