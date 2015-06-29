#ifndef ct_bigint_create_hpp
#define ct_bigint_create_hpp

#include "ct_bigint_core.hpp"

namespace thls
{
    namespace detail
    {
        template<int todo, int orig>
        struct ct_int_create_from_int_aux
        {
            typedef typename ct_int_create_from_int_aux<(todo>>limb_bits),orig>::type lower;
            
            THLS_STATIC_CONST(limb_t, here, lower::leftover&limb_mask);
            THLS_STATIC_CONST(limb_t, leftover, lower::leftover>>limb_bits);
            
            typedef ct_limb<here, typename lower::type> type;
        };
        
        template<int orig>
        struct ct_int_create_from_int_aux<0, orig>
        {
            THLS_STATIC_CONST(int, leftover, 0);
            typedef ct_zero type;
        };
    };
    
    template<int x>
    struct ct_int_create_from_int
    {
        typedef typename select_c<
            (x<0), typename ct_int_create_from_int<-x>::type,
            (x==0), ct_zero,
            true, ct_int_create_from_int_aux<x, x>::type
        >::type type;
    }

    //! Create positve mask containing `bits` ones
    template<unsigned bits>
    struct ct_int_create_mask;

    //! Create 2^bits
    template<unsigned bits>
    struct ct_int_create_2ui;

}; // thls


#endif
