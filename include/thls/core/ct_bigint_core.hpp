#ifndef ct_bigint_core_hpp
#define ct_bigint_core_hpp

#include "ct_bigint_itf.hpp"

#include "constfunc_helpers.hpp"


namespace thls
{

namespace detail
{
    typedef uint32_t limb_t;
    enum{ limb_bits = 16 };
    enum{ limb_mask = (1<<limb_bits)-1 };
    
    // This is intented to stop expansions getting crazy. The
    // compiler expansion limit will also stop it, but this is
    // also incorporates domain specific knowledge
    enum{ max_limbs = 256 };

    template<limb_t TLimb, class TTail>
    class ct_limb;

    class ct_zero;

    template<class TTail>
    class ct_neg;

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
    
    template<class TA>
    struct ct_canonicalise;

    template<class TA,int digits>
    struct ct_ensure_digits;

    template<class TA,limb_t L> struct ct_mul_single;
    
    template<limb_t TLimb, class TTail>
    struct ct_limb
    {
        THLS_STATIC_ASSERT(!TTail::is_neg, "The tail of a limb must be another limb or zero (not ct_neg).");
        THLS_STATIC_ASSERT(TTail::limb_count < max_limbs, "Integer would exceed max_limbs, which is likely to cause performance problems.");
            
        THLS_STATIC_CONST(limb_t,limb,TLimb);
        
        typedef TTail tail;
        
        // Number of limbs (including this one)
        THLS_STATIC_CONST(unsigned, limb_count, TTail::limb_count+1);
        
        // Index of this specific limb
        THLS_STATIC_CONST(int,limb_pos,1+(TTail::limb_pos));
        
        static void print_interior(FILE *dst)
        {
            fprintf(dst, "%04x", limb);
            TTail::print_interior(dst);
        }
        
   
        THLS_STATIC_CONST(bool,is_neg,false);
        THLS_STATIC_CONST(bool,is_pos,(limb>0) || (TTail::is_pos));
        THLS_STATIC_CONST(bool,is_zero,(limb==0) && (TTail::is_zero));
        
        static void print(FILE *dst)
        {
            fprintf(dst, "0x%x", limb);
            TTail::print_interior(dst);
        }
        
        static rt_int_t value()
        {
            limb_t v=limb;  // avoid constructor taking it by reference
            return (rt_int_t(v)<<(int)(limb_pos*limb_bits))+(TTail::value());
        }
    };
    

    class ct_zero
    {   
        THLS_STATIC_CONST(unsigned,limb_count,0);  // No limbs!
        
        THLS_STATIC_CONST(int,limb_pos,-1);   // Make the first limb have digit zero
        
        static void print_interior(FILE *)
        {}
        
    // public interface
        THLS_STATIC_CONST(bool,is_neg,false);
        THLS_STATIC_CONST(bool,is_pos,false);
        THLS_STATIC_CONST(bool,is_zero,true);
        
        static void print(FILE *dst)
        {
            fprintf(dst, "0");
        }
        
        static rt_int_t value()
        {
            return rt_int_t(0);
        }
    };

    template<class TTail>
    class ct_neg
    {
        THLS_STATIC_CONST(unsigned,limb_count,TTail::limb_count);  // This doesn't add any limbs
        
        typedef TTail tail;
        
    // Public interface
        
        THLS_STATIC_ASSERT(!TTail::is_neg, "Tail of ct_neg should be zero or positive.");
        
        THLS_STATIC_CONST(bool,is_neg,!TTail::is_zero);
        THLS_STATIC_CONST(bool,is_pos,false);
        THLS_STATIC_CONST(bool,is_zero,(TTail::is_zero)); // can have negative zero (though is equal to pos zero)
        
        static void print(FILE *dst)
        {
            fprintf(dst, "-");
            TTail::print(dst);
        }
        
        static rt_int_t value()
        {
            return -(TTail::value());
        }
    };
    
    template<class TTailTail>
    class ct_neg<ct_neg<TTailTail> >;
    
    
    
    ////////////////////////////////////////////////////////////
    // Makes sure that the number has exactly this many limbs.
    // It will be padded high with zeros, or there is an error
    // if it has more than that limb count

    template<class T, unsigned limbs>
    struct ct_pad_to_limb_count
    {
        THLS_STATIC_ASSERT(limbs <= T::limb_count, "Attempt to set limb count to less than current limb count");
        THLS_STATIC_ASSERT(limbs <= max_limbs, "Attempt to set limb count to more that max_limbs");
        
        template<class TB,int todo>
        struct worker
        {
            typedef typename worker<TB,todo-1>::type next;
            typedef ct_limb<0,next> type;
        };
        
        template<class TB>
        struct worker<TB,0>
        {
            typedef TB type;
        };
        
        enum{ to_add = limbs > T::limb_pos ? limbs-T::limb_pos : 0 };
        
        typedef typename worker<T,to_add>::type type;
    };

    template<class TT, int digits>
    struct ct_pad_to_limb_count<ct_neg<TT>,digits>;

    /////////////////////////////////////////////////////////////
    // Insert the given number of zeros as the least significant limgs

    template<class T,unsigned places=1>
    struct ct_shift_left_limbs
    {
        typedef ct_limb<
            T::limb,
            typename ct_shift_left_limbs<typename T::tail>::type
        > type;
    };

    template<unsigned places>
    struct ct_shift_left_limbs<ct_zero,places>
    {
        typedef ct_limb<
            0,
            typename ct_shift_left_limbs<ct_zero,places-1>::type
        > type;
    };
    
    template<>
    struct ct_shift_left_limbs<ct_zero,0>
    {
        typedef ct_zero value;
    };


    ////////////////////////////////////////////////
    // Given any number, make sure that the leading
    // digit is non-zero, or it is zer
    
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
    
    template<>
    struct ct_canonicalise<ct_neg<ct_zero> >
    {
        typedef ct_zero value;
    };

}; // detail

}; // thls


#endif
