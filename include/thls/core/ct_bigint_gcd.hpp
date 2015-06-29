#ifndef tlhs_ct_gcd_hpp
#define tlhs_ct_gcd_hpp

#include "ct_bigint_core.hpp"

namespace thls
{

namespace detail
{

template<class TA,class TB>
struct ct_gcd_impl;

template<class T>
struct ct_gcd_impl<T,T>
{
    typedef T type;
};

template<class TA>
struct ct_gcd_impl<TA,ct_zero>
{
    typedef TA type;
};

template<class TB>
struct ct_gcd_impl<ct_zero,TB>
{
    typedef TB type;
};

template<class TU,class TV>
struct ct_gcd_impl
{
    typedef typename select_c<
    ct_is_even<TU>::value && ct_is_even<TV>::value, // (gcd(u>>1,v>>1)<<1)
        typename ct_left_shift<typename ct_gcd_impl<
            typename ct_right_shift<TU,1>::type, typename ct_right_shift<TV,1>::type
        >::type,1>::type,
    ct_is_even<TU>::value && ct_is_odd<TV>::value,  // (gcd(u>>1,v)<<1)
        typename ct_left_shift<typename ct_gcd_impl<
            typename ct_right_shift<TU,1>::type, TV
        >::type,1>::type,
    ct_is_odd<TU>::value && ct_is_even<TV>::value,  // (gcd(u,v>>1)<<1)
        typename ct_left_shift<typename ct_gcd_impl<
            TU, typename ct_right_shift<TV,1>::type
        >::type,1>::type,
    ct_greater_than<TU,TV>::value,  // (gcd((u-v)>>1,v)
        typedef ct_gcd_impl<
            typename ct_shift_right<typename ct_sub<TU,TV>::type,1>::type,
            TV
        >::type,
    true, // (gcd((v-u)>>1,u)
        typedef ct_gcd_impl<
            TU,
            typename ct_shift_right<typename ct_sub<TV,TU>::type,1>::type
    >::type,
  >::type type;
};

template<class TA,class TB>
struct ct_gcd
{   
    typedef typename detail::ct_gcd_impl<
        typename ct_abs<TA>::type,
        typename ct_abs<TB>::type
    >::type type;
};

}; // tlhs

#endif
