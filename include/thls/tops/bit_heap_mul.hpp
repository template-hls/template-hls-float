#ifndef thls_bit_heap_mul_hpp
#define thls_bit_heap_mul_hpp

#include "bit_heap.hpp"

namespace thls
{

namespace detail
{
    template<int TILE, int WA, int WB, bool splitA, bool splitB>
    struct bit_heap_mul_build;
    
    template<int TILE, int WA, int WB>
    struct bit_heap_mul_build<TILE,WA,WB,false,false>
    {
        static_assert(WA>=0, "Need non-negative width A input.");
        static_assert(WB>=0, "Need non-negative width B input.");
        
        typedef typename uint_to_bit_heap<WA+WB>::heap_t heap_t;
        
        static heap_t build(const fw_uint<WA> &a, const fw_uint<WB> &b)
        {
            fw_uint<WA+WB> part(a*b);
            return uint_to_bit_heap<WA+WB>::build(part);
        }
    };
    
    template<int TILE, int WA, int WB>
    struct bit_heap_mul_build<TILE,WA,WB,false,true>
    {
        static_assert(WA>=0, "Need non-negative width A input.");
        static_assert(WB>=0, "Need non-negative width B input.");
        
        // We are going to split along B
        
        static const int WB_HI=TILE;
        static const int WB_LO=WB-TILE;
        
        static const bool splitAgain=WB_LO > TILE;
        
        typedef bit_heap_mul_build<TILE,WA,WB_HI,false,false> hi_next_t;
        typedef bit_heap_mul_build<TILE,WA,WB_LO,false,splitAgain> lo_next_t;
        
        typedef typename hi_next_t::heap_t hi_heap_orig_t;
        typedef typename bit_heap_shift_left_impl<WB_LO,hi_heap_orig_t>::heap_t hi_heap_t;
        
        typedef typename lo_next_t::heap_t lo_heap_t;
        
        typedef typename detail::merge_bit_heaps_impl<hi_heap_t,lo_heap_t>::heap_t heap_t;
        
        static heap_t build(const fw_uint<WA> &a, const fw_uint<WB> &b)
        {
            hi_heap_orig_t hi_orig=hi_next_t::build(a,take_msbs<TILE>(b));
            std::cerr<<hi_orig<<"\n";
            hi_heap_t hi=bit_heap_shift_left_impl<WB_LO,hi_heap_orig_t>::build(hi_orig);
            std::cerr<<hi<<"\n";
            lo_heap_t lo=lo_next_t::build(a, drop_msbs<TILE>(b));
            std::cerr<<lo<<"\n";
            auto res=detail::merge_bit_heaps_impl<hi_heap_t,lo_heap_t>::build(hi,lo);
            std::cerr<<res<<"\n";
            return res;
        }
    };
    
    template<int TILE, int WA, int WB>
    struct bit_heap_mul_build<TILE,WA,WB,true,false>
    {
        static_assert(WA>=0, "Need non-negative width A input.");
        static_assert(WB>=0, "Need non-negative width B input.");
    
        // Swap over b and a, let other case handle it
        
        typedef typename bit_heap_mul_build<TILE,WB,WA,false,true>::heap_t heap_t;
        
        static heap_t build(const fw_uint<WA> &a, const fw_uint<WB> &b)
        {
            return bit_heap_mul_build<TILE,WB,WA,false,true>::build(b,a);
        }
    };
    
    template<int TILE, int WA, int WB>
    struct bit_heap_mul_build<TILE,WA,WB,true,true>
    {
        static_assert(WA>=0, "Need non-negative width A input.");
        static_assert(WB>=0, "Need non-negative width B input.");
        
        // We are going to split along A, then let the other implementation handle B
        
        static const int WA_HI=TILE;
        static const int WA_LO=WA-TILE;
        
        static const bool splitAgain=WA_LO > TILE;
        
        typedef bit_heap_mul_build<TILE,WA_HI,WB,false,true> hi_next_t;
        typedef bit_heap_mul_build<TILE,WA_LO,WB,(WA_LO>TILE),true> lo_next_t;
        
        typedef typename hi_next_t::heap_t hi_heap_orig_t;
        
        typedef typename bit_heap_shift_left_impl<WA_LO,hi_heap_orig_t>::heap_t hi_heap_t;
        
        typedef typename lo_next_t::heap_t lo_heap_t;
        
        typedef typename detail::merge_bit_heaps_impl<hi_heap_t,lo_heap_t>::heap_t heap_t;
        
        static heap_t build(const fw_uint<WA> &a, const fw_uint<WB> &b)
        {
            hi_heap_orig_t hi_orig=hi_next_t::build(take_msbs<TILE>(a),b);
            hi_heap_t hi=bit_heap_shift_left_impl<WA_LO,hi_heap_orig_t>::build(hi_orig);
            lo_heap_t lo=lo_next_t::build(drop_msbs<TILE>(a),b);
            return detail::merge_bit_heaps_impl<hi_heap_t,lo_heap_t>::build(hi,lo);
        }
    };
}; // detail

template<int SPLIT, int WA,int WB>
typename detail::bit_heap_mul_build<SPLIT,WA,WB,(WA>SPLIT),(WB>SPLIT)>::heap_t bit_heap_mul(const fw_uint<WA> &a, const fw_uint<WB> &b)
{
    return detail::bit_heap_mul_build<SPLIT,WA,WB,(WA>SPLIT),(WB>SPLIT)>::build(a,b);
}

}; // thls

#endif
