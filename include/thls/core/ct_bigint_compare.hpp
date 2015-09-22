#ifndef ct_bigint_compare_hpp
#define ct_bigint_compare_hpp

#include "ct_bigint_core.hpp"

namespace thls
{

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

}; // thls


#endif
