#ifndef ct_bigint_hpp
#define ct_bigint_hpp

#include <cstdio>
#include <cstdint>

#include <boost/mpl/if.hpp>
#include <boost/mpl/same_as.hpp>
#include <boost/multiprecision/cpp_int.hpp>

#include "base_ops.hpp"

typedef boost::multiprecision::cpp_int rt_wideint_t;

typedef uint32_t limb_t;
enum{ limb_bits = 16 };
enum{ limb_mask = (1<<limb_bits)-1 };


template<limb_t TLimb, class TTail>
struct ct_limb;

struct ct_zero;

template<class TTail>
struct ct_neg;

/*
    POS_NUMBER = ZERO
            || (limb,POS_NUMBER)
    
    NEG_NUMBER = (neg,POS_NUMBER)
    
    NUMBER = POS_NUMBER | NEG_NUMBER


    Canonical numbers have a leading non-zero limb, but
    zero limbs may be built up as intermediate calculations.
    
    Canonical zero is not negative, but it may arise in
    intermediate calculations, and should compare equal to zero.
*/


template<class TA,class TB>
class ct_less_than;

template<class TA,class TB>
class ct_equals;

template<class TA,class TB>
struct ct_greater_than;

template<class TA,class TB>
class ct_add;

template<class TA,class TB>
class ct_mul;

template<class TA,unsigned places>
class ct_shift_left;

template<class TA,unsigned places>
class ct_shift_right;


template<class TA>
struct ct_canonicalise;

template<class TA,int digits>
struct ct_ensure_digits;

template<class TA,unsigned nlimbs> struct ct_shift_limbs_left;
template<class TA,limb_t L> struct ct_mul_single;


template<limb_t TLimb, class TTail>
struct ct_limb
{
    HLS_STATIC_ASSERT(TLimb < limb_mask, "Limb is out of range.");
    
    HLS_SC(limb_t,limb,TLimb);
    
    typedef TTail tail;
    
    HLS_SC(int,digit_pos,1+(TTail::digit_pos));
    
    static void print_interior(FILE *dst)
    {
        fprintf(dst, "%04x", limb);
        TTail::print_interior(dst);
    }
    
public:    
    HLS_SC(bool,is_neg,false);
    HLS_SC(bool,is_pos,(limb>0) || (TTail::is_pos));
    HLS_SC(bool,is_zero,(limb==0) && (TTail::is_zero));
    
    static void print(FILE *dst)
    {
        fprintf(dst, "0x%x", limb);
        TTail::print_interior(dst);
    }
    
    static rt_wideint_t value()
    {
        limb_t v=limb;  // avoid constructor taking it by reference
        return (rt_wideint_t(v)<<(int)(digit_pos*limb_bits))+(TTail::value());
    }
};

struct ct_zero
{   
    
    HLS_SC(int,digit_pos,-1);   // Make the first limb have digit zero
    
    static void print_interior(FILE *)
    {}

    HLS_SC(bool,is_neg,false);
    HLS_SC(bool,is_pos,false);
    HLS_SC(bool,is_zero,true);
    
    static void print(FILE *dst)
    {
        fprintf(dst, "0");
    }
    
    static rt_wideint_t value()
    {
        return rt_wideint_t(0);
    }
};

template<class TTail>
struct ct_neg
{
    
    typedef TTail tail;
public:
    
    HLS_SC(bool,is_neg,(TTail::is_pos));
    HLS_SC(bool,is_pos,false);
    HLS_SC(bool,is_zero,(TTail::is_zero)); // can have negative zero (though is equal to pos zero)
    
    static void print(FILE *dst)
    {
        fprintf(dst, "-");
        tail::print(dst);
    }
    
    static rt_wideint_t value()
    {
        return -(TTail::value());
    }
};

template<class TA,class TB>
struct ct_less_than;


template<>
struct ct_less_than<ct_zero,ct_zero>
{ enum{ value = false }; };

template<class TB>
struct ct_less_than<ct_zero,TB>
{ enum{ value = TB::is_pos }; };

template<class TA>
struct ct_less_than<TA,ct_zero>
{ enum{ value = TA::is_neg }; };


template<class TTailA, class TTailB>
struct ct_less_than<ct_neg<TTailA>,ct_neg<TTailB> >
{ enum{ value = ct_less_than<TTailB,TTailA>::value }; };

template<class TTailA, class TB>
struct ct_less_than<ct_neg<TTailA>,TB>
{ enum{ value = true }; };

template<class TA, class TTailB>
struct ct_less_than<TA,ct_neg<TTailB> >
{ enum{ value = false }; };


template<class TA,class TB>
struct ct_less_than
{
    enum{ value =   (TA::digit_pos > TB::digit_pos) ? ((TA::limb!=0) ? false : ct_less_than<typename TA::tail,TB>::value)   :
                    (TA::digit_pos < TB::digit_pos) ? ((TB::limb!=0) ? true : ct_less_than<TA,typename TB::tail>::value) :
                    (TA::limb!=TB::limb) ? (TA::limb<TB::limb) :
                    ct_less_than<typename TA::tail,typename TB::tail>::value
    };
};


template<class TA,class TB>
struct ct_equals;

template<>
struct ct_equals<ct_zero,ct_zero>
{ enum{ value=true }; };

template<class TB>
struct ct_equals<ct_zero,TB>
{ enum{ value=TB::is_zero }; };

template<class TA>
struct ct_equals<TA,ct_zero>
{ enum{ value=TA::is_zero }; };

template<class TTailA, class TTailB>
struct ct_equals<ct_neg<TTailA>,ct_neg<TTailB> >
{ enum{ value = ct_equals<TTailB,TTailA>::value }; };

template<class TTailA, class TB>
struct ct_equals<ct_neg<TTailA>,TB>
{ enum{ value = false }; };

template<class TA, class TTailB>
struct ct_equals<TA,ct_neg<TTailB> >
{ enum{ value = false }; };


template<class TA,class TB>
struct ct_equals
{
    enum{ value =   TA::digit_pos > TB::digit_pos ? (TA::limb!=0 ? false : ct_equals<typename TA::tail,TB>::value )  :
                    TA::digit_pos < TB::digit_pos ? (TB::limb!=0 ? false : ct_equals<TA,typename TB::tail>::value )  :
                    TA::limb!=TB::limb ? false :
                    ct_equals<typename TA::tail,typename TB::tail>::value
    };
};


template<class TA,class TB>
struct ct_greater_than
{
  enum{ value = ct_less_than<TB,TA>::value };  
};



template<class TA,class TB>
struct ct_add;

template<>
struct ct_add<ct_zero,ct_zero>
{
    HLS_SC(limb_t,carry,0);
    typedef ct_zero value;  
};

template<limb_t L,class T>
struct ct_add<ct_zero,ct_limb<L,T> >
{
    HLS_SC(limb_t,carry,0);
    typedef ct_limb<L,T> value;  
};

template<limb_t L,class T>
struct ct_add<ct_limb<L,T>,ct_zero >
{
    HLS_SC(limb_t,carry,0);
    typedef ct_limb<L,T> value;  
};


template<int n>
struct ct_zero_limbs
{
    HLS_STATIC_ASSERT(n>=0, "n must be positive.");
    
    typedef typename boost::mpl::if_<
        boost::mpl::bool_<(n<=0)>,
        ct_zero,
        ct_limb<0,typename ct_zero_limbs<n-1>::value>
    >::type value;
};

template<>
struct ct_zero_limbs<0>
{
    typedef ct_zero value;
};


/*! Extend TTarget till it has at least that many digits */
template<class T, int digits>
struct ct_ensure_digits
{
    template<class TB,int todo>
    struct worker
    {
        typedef typename worker<TB,todo-1>::value next;
        typedef ct_limb<0,next> value;
    };
    
    template<class TB>
    struct worker<TB,0>
    {
        typedef TB value;
    };
    
    enum{ to_add = digits > T::digit_pos ? digits-T::digit_pos : 0 };
    
    typedef typename worker<T,to_add>::value value;
};

template<class TT, int digits>
struct ct_ensure_digits<ct_neg<TT>,digits>; // no instantiation for now


template<class T,unsigned nlimbs>
struct ct_shift_limbs_left
{
    typedef ct_limb<T::limb,typename ct_shift_limbs_left<typename T::tail,nlimbs>::value> value;
};

template<unsigned nlimbs>
struct ct_shift_limbs_left<ct_zero,nlimbs>
{
    typedef typename ct_zero_limbs<nlimbs>::value value;
};


template<class T,unsigned nlimbs>
struct ct_shift_limbs_right
{
    typedef typename boost::mpl::if_<
       boost::mpl::bool_<(T::digit_pos>=nlimbs)>,
        ct_limb<T::limb, typename ct_shift_limbs_right<typename T::tail,nlimbs>::value>,
        ct_zero
    >::type value;
};

template<unsigned nlimbs>
struct ct_shift_limbs_right<ct_zero,nlimbs>
{
    typedef ct_zero value;
};


template<class T,unsigned places,limb_t carryIn>
struct ct_shift_right_detail
{
    HLS_STATIC_ASSERT(places<limb_bits, "Should get rid of whole digits before here.");
    
    typedef typename ct_shift_right_detail<typename T::tail, places, ((T::limb<<(limb_bits-places))&limb_mask)>::value lower;
    
    typedef ct_limb<(carryIn |  (T::limb>>places)),lower> value;
};

template<unsigned places,limb_t carryIn>
struct ct_shift_right_detail<ct_zero,places,carryIn>
{
    HLS_STATIC_ASSERT(places<limb_bits, "Should get rid of whole digits before here.");
    
    typedef ct_zero value;
};

template<class T,unsigned places>
struct ct_shift_right
{
    typedef typename ct_shift_limbs_right<T,(places/limb_bits)>::value coarse;
    typedef typename ct_shift_right_detail<coarse,(places%limb_bits),0>::value fine;
    
    typedef typename ct_canonicalise<fine>::value value;
};



template<class T,unsigned places>
struct ct_shift_left_detail
{
    HLS_STATIC_ASSERT(places<limb_bits, "Should get rid of whole digits before here.");
    
    HLS_SC(limb_t,carry, T::limb>>(limb_bits-places));
    
    typedef ct_shift_left_detail<typename T::tail, places> lower;
    
    typedef ct_limb<(lower::carry | (T::limb<<places))&limb_mask,typename lower::value> value;
};

template<unsigned places>
struct ct_shift_left_detail<ct_zero,places>
{
    HLS_STATIC_ASSERT(places<limb_bits, "Should get rid of whole digits before here.");
    
    HLS_SC(limb_t,carry,0);
    
    typedef ct_zero value;
};

template<class T,unsigned places>
struct ct_shift_left
{
    typedef typename ct_shift_limbs_left<T,(places/limb_bits)>::value coarse;
    typedef ct_shift_left_detail<coarse,(places%limb_bits)> lower;
    typedef typename lower::value fine;
    
    typedef typename ct_canonicalise<ct_limb<lower::carry,typename lower::value> >::value value;
};


template<class T>
struct ct_canonicalise
{
    typedef T value;
};

template<class TTail>
struct ct_canonicalise<ct_limb<0,TTail> >
{
    typedef TTail value;
};

template<class TTail>
struct ct_canonicalise<ct_neg<TTail> >
{
    typedef ct_neg<typename ct_canonicalise<TTail>::value> value;
};


template<class TA,class TB>
struct ct_add
{
private:
    // Add an extra digit for any carry
    enum{ digits = 1+ TA::digit_pos > TB::digit_pos ? TA::digit_pos : TB::digit_pos };
    
    typedef typename ct_ensure_digits<TA,digits>::value FA;
    typedef typename ct_ensure_digits<TB,digits>::value FB;
    
    template<int x,class A, class B>
    struct worker
    {
        typedef worker<0,typename A::tail,typename B::tail> lower;
        
        HLS_SC(limb_t,full_sum,(lower::carry)+(A::limb)+(B::limb));
        HLS_SC(limb_t,carry,full_sum>>limb_bits);
        HLS_SC(limb_t,sum,full_sum&limb_mask);
        
        typedef ct_limb<sum,typename lower::value> value;
    };
    
    template<int x>
    struct worker<x,ct_zero,ct_zero>
    {
        HLS_SC(limb_t,carry,0);
        typedef ct_zero value;
    };
    
    typedef typename worker<0,FA,FB>::value raw;
    
public:
    typedef typename ct_canonicalise<raw>::value value;
};

template<class TA, limb_t LB>
struct ct_mul_single
{
    template<int x,class A,limb_t B>
    struct working
    {
        typedef working<x,typename A::tail,B> lower;
        
        HLS_SC(limb_t,full,(A::limb)*B+(lower::carry));
        HLS_SC(limb_t,carry,full>>limb_bits);
        HLS_SC(limb_t,prod,full&limb_mask);
        
        typedef ct_limb<prod,typename lower::value> value;
    };
    
    template<int x,limb_t B>
    struct working<x,ct_zero,B>
    {
        HLS_SC(limb_t,carry,0);
        typedef ct_zero value;
    };
    
    typedef working<0, TA, LB> lower;
    typedef ct_limb<lower::carry,typename lower::value> raw;
public:
    typedef typename ct_canonicalise<raw>::value value;
};

template<class TA,class TB>
struct ct_mul
{
public:
    
};

template<class TA,limb_t LB,class TB>
struct ct_mul<TA,ct_limb<LB,TB> >
{
private:
    typedef typename ct_mul_single<TA,LB>::value partial;
    typedef typename ct_mul<TA,TB>::value lower;

    typedef typename boost::mpl::if_<
        boost::mpl::same_as<TB,ct_zero>,
        ct_zero,
        typename ct_shift_limbs_left<partial,TB::digit_pos>::value
    > aligned;
public:
    typedef typename ct_add<lower,aligned>::value value;
};

template<class TA>
struct ct_mul<TA,ct_zero>
{
 public:
    typedef ct_zero value;
};


#endif
