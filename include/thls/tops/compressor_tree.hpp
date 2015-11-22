#ifndef thls_compressor_tree_hpp
#define thls_compressor_tree_hpp

#error "Never tested. Look at bit_heap.hpp"

#include <thls/tops/fw_uint.hpp>

#ifndef THLS_SYNTHESIS
#endif

namespace thls
{
    fw_uint<2> compress_bits(const fw_uint<1> &a, const fw_uint<1> &b)
    { return zpad<2>(a)+zpad<2>(b); }
    
    fw_uint<2> compress_bits(const fw_uint<1> &a, const fw_uint<1> &b, const fw_uint<1> &c)
    { return zpad<2>(a)+zpad<2>(b)+zpad<2>(c); }
    
    namespace detail
    {
        template<int W>
        struct compress_words_impl
        {
            std::pair<fw_uint<W> > eval(const fw_uint<W> &a, const fw_uint<W> &b, const fw_uint<W> &c)
            {
                const int S=W/2;
                
                auto lo=compress_words_impl(take_lsbs<S>(a), take_lsbs<S>(b), take_lsbs<S>(c));
                auto hi=compress_words_impl(drop_lsbs<S>(a), drop_lsbs<S>(b), drop_lsbs<S>(c));
                
                return std::pair<fw_uint<W> >(
                    concat(hi.first,lo.first),
                    concat(hi.second,lo.second)
                );

            }            
        };
        
        template<1>
        struct compress_words_impl
        {
            std::pair<fw_uint<1> > eval(const fw_uint<1> &a, const fw_uint<1> &b, const fw_uint<1> &c)
            {
                auto res=compress_bits(a,b,c);
                return std::pair<fw_uint<1> >(
                    get_bit<1>(res),
                    get_bit<0>(res)
                );
            }            
        };
    };
    
    template<int WA,int WB,int WC>
    std::pair<fw_uint<ctMax3(WA,WB,WC)+2>,fw_uint<ctMax3(WA,WB,WC)>> compress_words(const fw_uint<WA> &a, const fw_uint<WB> &b, const fw_uint<WC> &c)
    {
        const int W=ctMax3(WA,WB,WC,WD)+2;
        return detail::compress_words_impl<W>(extu<W>(a),extu<W>(b),extu<W>(c));
    }
    
    template<int WA,int WB,int WC,int WD>
    std::pair<fw_uint<ctMax4(WA,WB,WC,WD)+2> > compress_words(const fw_uint<WA> &a, const fw_uint<WB> &b, const fw_uint<WC> &c, const fw_uint<WD> &d)
    {
        const int W=ctMax3(WA,WB,WC,WD)+2;
        return detail::compress_words_impl<W>(extu<W>(a),extu<W>(b),extu<W>(c),extu<W>(d));
    }
    
    template<int WA,int WB,int WC,int WD>
    fw_uint<W+1> add_compress(const fw_uint<W> &a, const fw_uint<W> &b, const fw_uint<W> &c)
    {
        auto r=compress_words(a,b,c);
        return zpad_lo<1>(r.first) + zpad_hi<1>(r.second);
    }
    
    template<class W>
    fw_uint<W+1> add_compress(const fw_uint<W> &a, const fw_uint<W> &b, const fw_uint<W> &c, const fw_uint<W> &d)
    {
        auto r=compress_words(a,b,c,d);
        return zpad_lo<1>(r.first) + zpad_hi<1>(r.second);
    }
    
};

#endif
