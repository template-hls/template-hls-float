#ifndef thls_constant_mult_v10_booth_cache_hpp
#define thls_constant_mult_v10_booth_cache_hpp

#include "thls/tops/fw_uint.hpp"

#include <random>

namespace thls {

struct cmult_impl_v10_booth_cache
{

    static constexpr const char *description=
"Detect runs of ones and encode as booth-style +- runs, based on v6. Main "
"difference is that it caches runs in an attempt to force resource sharing "
"for tools that miss the opportunity to resource share different width "
"additions.";

    /* We don't just pre-populate all possible runs as we want to do the
    additions with the minimum possible addition width, so that the HLS
    tool chooses the smallest possible delay.
    
    That means we need to track the values calculated so far and their
    widths. However, the width needed is always decreasing, so we can
    always chop to the maximum possible remaining length. This relies
    on the HLS tool seeing truncation as a zero-cost operation though.

    We have a choice of either dynamically trying to update a dictionary,
    or just keeping things on an assoc list. If we assume constants are
    up to maybe 256-bits (if we move to a better literal int type),
    then there are at most 128-runs occurring. Many of these will be
    repeated at different widths, but it seems better to just push
    them onto an assoc list and keep things simple.
    */

    template<int WR, int WI, int RRunLength>
    static fw_uint<WR> make_run(const fw_uint<WI> &x)
    {
        if(RRunLength==1){
            return resize<WR>(x);
        }else if(RRunLength==2){
            return resize<WR>(zpad_lo<1>(x)) + resize<WR>(x);
        }else{
            return resize<WR>(zpad_lo<RRunLength>(x)) - resize<WR>(x);
        }
    }

    template<int TW, int TRunLength, class TTail>
    struct run_entry
    {
        static const constexpr int W=TW;
        static const constexpr int RunLength=TRunLength;

        const fw_uint<W> value;
        const TTail &tail;

        template<int WI>
        constexpr run_entry(const fw_uint<WI> &x, const TTail &_tail)
            : value( _tail.template get_value<TW, TRunLength,WI>(x) )
            , tail( _tail )
        {}

        template<int AW, int ARunLength, int WI>
        fw_uint<AW> get_value(const fw_uint<WI> &x) const
        {
            if(ARunLength==TRunLength){
                static_assert(AW<=W, "Logic error");
                return take_lsbs<AW>(value);
            }else{
                return tail.template get_value<AW,ARunLength>(x);
            }
        }
    };

    struct run_null
    {
        template<int AW, int ARunLength, int WI>
        fw_uint<AW> get_value(const fw_uint<WI> &x) const
        {
            return make_run<AW,WI,ARunLength>(x);
        }
    };

    static constexpr int find_next_one(uint64_t C)
    {
        return C==0 ? -1 : (C&1) ? 0 : 1+find_next_one(C>>1);
    }

    static constexpr int find_next_zero(uint64_t C)
    {
        return (C&1)==0 ? 0 : 1+find_next_zero(C>>1);
    }

    template<unsigned WR, unsigned WI, uint64_t C, class TCache>
    struct helper
    {
        static_assert(C&1, "Constant must be normalised.");

        static constexpr int run_length=find_next_zero(C);
        // Warning: post_zeros is guaranteed to be -1 at some point
        static constexpr int post_zeros=find_next_one(C>>run_length); 

        // If these are the last-bits, just make sure Crem is zero
        static constexpr int width=post_zeros==-1 ? run_length : post_zeros+run_length;
        static constexpr uint64_t Crem=C>>width;

        THLS_INLINE static fw_uint<WR> go(const fw_uint<WI> &x, const TCache &cache) {
            static_assert(WR>=width, "Logic violation.");

            using ncache_t = run_entry<WR,run_length,TCache>;
            const ncache_t ncache(x, cache);
            
            fw_uint<WR> curr = ncache.value;

            if(Crem==0){
                return curr;
            }else{
                fw_uint<WR - width> base = helper<WR - width, WI, Crem, ncache_t>::go(x, ncache);
                return curr + zpad_lo<width>(base);
            }
        }
    };

    template<unsigned WR, unsigned WI,class TCache>
    struct helper<WR,WI,0,TCache>
    {
        THLS_INLINE static fw_uint<WR> go(const fw_uint<WI> &x, const TCache &cache)
        { return zg<WR>(); }
    };

    template<unsigned WR, unsigned WI, uint64_t C>
    THLS_INLINE static fw_uint<WR> go(const fw_uint<WI> &x)
    {
        static_assert(C>0, "Constant is zero.");

        const unsigned WIa=thls_ctMin(WR,WI);
        const unsigned WC=thls_ctLog2Ceil(C);
        const unsigned WCa=thls_ctMin(WR,WC);
        const uint64_t WC_MASK=0xFFFFFFFFFFFFFFFFull>>(64-WCa);
        const uint64_t Ca = WC_MASK & C;

        if(Ca==0){
            // We allow the constant after truncation to be zero. Probably 
            // we should allow zero in general?
            return zg<WR>();
        }else {
            const int zpad=find_next_one(Ca);
            if(zpad<0){
                static_assert(zpad >=0 || zpad==-1, "Pre-condition.");
                return zg<WR>();
            }else{
                const uint64_t Crem= (zpad<0) ? 1 : Ca>>zpad; // Sigh. Roll on static if
                
                const fw_uint<WIa> xa=take_lsbs<WIa>(x);
                return zpad_lo<zpad>(helper<WR-zpad, WIa, Crem, run_null>::go(xa, run_null()));
            }
        }
    }
};



}; // thls

#endif
