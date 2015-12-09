#ifndef fw_uint_on_ap_uint_hpp
#define fw_uint_on_ap_uint_hpp

#ifndef fw_uint_hpp
#error "Include fw_uint.hpp, rather than this file."
#endif

#include "C:\Usr\Xilinx2015.4\Vivado_HLS\2015.4\common\technology\autopilot\ap_int.h"
//#include "ap_int.h"

namespace thls
{

template<int W>
struct fw_uint
{
    static const int width=W;

    // ap_uint doesn't allow widths <= 0
    static const int SafeW=width<=0 ? 1 : width;

    ap_uint<SafeW> bits;

    THLS_CONSTEXPR fw_uint()
    	: bits(0)
    {}

    fw_uint(const fw_uint &x)
    	: bits(x.bits)
    {}


    // Construct 1-bit vector from boolean
    #if __cplusplus >= 201103L
    template<int WW=W>
    fw_uint(bool b, typename std::enable_if<WW==1>::type * = nullptr )
        : bits(b)
    {}
    #else
    template<int WW=W>
    explicit fw_uint(bool b)
        : bits(b)
    {
        assert(WW==1);
    }
    #endif

    explicit fw_uint(int v)
        : bits(v)
    {
        assert(v>=0); // must be non-negative
        assert(v < (1ll<<W)); // Must be in range
    }

    explicit fw_uint(uint64_t v)
        : bits(v)
    {
        assert(v>=0); // must be non-negative
        assert(v < (1ll<<W)); // Must be in range
    }

    explicit fw_uint(const char *value)
        : bits(value)
    {
        // TODO : Really need to range-check the actual number
    }

    explicit fw_uint(const ap_uint<SafeW> &x)
        : bits(x)
    {}

    explicit fw_uint(const ap_range_ref<SafeW,false> &x)
        : bits(x)
    {}

#ifndef THLS_SYNTHESIS
    explicit fw_uint(mpz_class x)
    {
        if(mpz_sizeinbase(x.get_mpz_t(),2)>W){
            throw std::runtime_error("mpz is too large.");
        }
        std::string dec=x.get_str(); // LAZY
        bits=ap_uint<SafeW>(dec.c_str());
    }
#endif

    fw_uint<W> operator+(const fw_uint<W> &o) const
    {
        ap_uint<SafeW> a(bits), b(o.bits);
        return fw_uint<W>(ap_uint<SafeW>(a+b));
    }

    fw_uint operator+(int b) const
    {
        return fw_uint(ap_uint<SafeW>(bits+b));
    }

    fw_uint<W> operator-(const fw_uint<W> &o) const
    {
        ap_uint<SafeW> a(bits), b(o.bits);
        return fw_uint<W>(ap_uint<SafeW>(a-b));
    }

    fw_uint operator-(int b) const
    {
        return fw_uint(ap_uint<SafeW>(bits-b));
    }

    template<int O>
    fw_uint<O+W> operator*(const fw_uint<O> &o) const
    {
    	static const int SafeRW = (O+W <= 0) ? 1 : (O+W);
        ap_uint<SafeRW> a(bits), b(o.bits);
        return fw_uint<O+W>(ap_uint<SafeRW>(a*b));
    }

    fw_uint<1> operator<(const fw_uint &o) const
    { return fw_uint<1>(bits < o.bits); }

    fw_uint<1> operator<(int o) const
    {
        assert(o>=0);
        return fw_uint<1>(bits < o);
    }

    fw_uint<1> operator<=(const fw_uint &o) const
    { return fw_uint<1>(bits <= o.bits); }

    fw_uint<1> operator<=(int o) const
    {
        assert(o>=0);
        return fw_uint<1>(bits <= o);
    }

    fw_uint<1> operator==(const fw_uint &o) const
    { return fw_uint<1>(bits == o.bits); }

    fw_uint<1> operator==(int o) const
    {
        assert(o>=0);
        return fw_uint<1>(bits == o);
    }

    fw_uint<1> operator>=(const fw_uint &o) const
    { return fw_uint<1>(bits >= o.bits); }

    fw_uint<1> operator>=(int o) const
    {
        assert(o>=0);
        return fw_uint<1>(bits >= o);
    }

    fw_uint<1> operator>(const fw_uint &o) const
    { return fw_uint<1>(bits > o.bits); }

    fw_uint<1> operator>(int o) const
    {
        assert(o>=0);
        return fw_uint<1>(bits > o);
    }

    fw_uint<1> operator!=(const fw_uint &o) const
    { return fw_uint<1>(bits != o.bits); }

    fw_uint<1> operator!=(int o) const
    {
        assert(o>=0);
        return fw_uint<1>(bits != o);
    }

    //////////////////////////////////////////////
    // Bitwise ops.
    // numeric_std says result width is size of the LHS

    fw_uint operator~() const
    { return fw_uint(ap_uint<SafeW>(~bits)); }

    template<int O>
    fw_uint operator&(const fw_uint<O> &o) const
    {
        ap_uint<W> b(o.bits);
        return fw_uint<W>(ap_uint<SafeW>(bits&b));
    }

    fw_uint operator&(int b) const
    {
        assert(b>=0);
        return fw_uint(ap_uint<SafeW>(bits&b));
    }

    template<int O>
    fw_uint operator|(const fw_uint<O> &o) const
    {
        ap_uint<W> b(o.bits);
        return fw_uint<W>(ap_uint<SafeW>(bits|b));
    }

    fw_uint operator|(int b) const
    {
        assert(b>=0);
        return fw_uint(ap_uint<SafeW>(bits|b));
    }

    template<int O>
    fw_uint operator^(const fw_uint<O> &o) const
    {
        ap_uint<W> b(o.bits);
        return fw_uint<W>(ap_uint<SafeW>(bits^b));
    }

    fw_uint operator^(int b) const
    {
        assert(b>=0);
        return fw_uint(bits^b);
    }


    fw_uint operator>>(int dist) const
    {
        assert(0<=dist && dist<W);
        ap_uint<SafeW> res(bits>>dist);
        return fw_uint(res);
    }
    
    fw_uint operator<<(int dist) const
    {
        assert(0<=dist && dist<W);
        ap_uint<SafeW> res(bits<<dist);
        return fw_uint(res);
    }

#ifndef THLS_SYNTHESIS
    std::string to_string() const
    {
        std::string acc="";
        for(int i=0;i<W;i++){
            if((0==(i%8))&&(i!=0)){
                acc="_"+acc;
            }
            acc=(bits[i]?"1":"0")+acc;
        }
        return "0b"+acc;
    }
#endif

    int to_int() const
    {
        return (int)bits;
    }

    uint64_t to_uint64() const
    { return (uint64_t)bits; }

    bool to_bool() const
    {
        assert(W==1);
        return bits==1;
    }

    #ifndef THLS_SYNTHESIS
    mpz_class to_mpz_class() const
    {
        std::string dec( bits.to_string() );   // LAZY
        return mpz_class(dec);
    }
    #endif

    //explicit operator bool() const
    //{ return to_bool(); }

    operator bool() const
    { return to_bool(); }
};



inline fw_uint<1> operator||(const fw_uint<1> &a, const fw_uint<1> &b)
{ return fw_uint<1>(ap_uint<1>(a.bits||b.bits)); }

inline fw_uint<1> operator&&(const fw_uint<1> &a, const fw_uint<1> &b)
{ return fw_uint<1>(ap_uint<1>(a.bits&&b.bits)); }

#ifndef THLS_SYNTHESIS
template<int W>
inline std::ostream &operator<<(std::ostream &dst, const fw_uint<W> &x)
{
    dst<<x.to_string();
    return dst;
}
#endif


template<int HI,int LO,int W>
inline fw_uint<HI-LO+1> get_bits(const fw_uint<W> &x)
{
	static const int SafeRW = (HI-LO+1 <= 0) ? 1 : (HI-LO+1);
    return fw_uint<HI-LO+1>(ap_uint<SafeRW>(x.bits.range(HI,LO)));
}


// NOTE: There was a nice C++11 implementation of concat, but
// I dropped it for simplicity and compiler compatibility. At
// some point it could be brought back, but this will do for now.

template<int WA>
inline fw_uint<WA> concat(const fw_uint<WA> &a)
{ return a; }

template<int WA,int WB>
inline fw_uint<WA+WB> concat(const fw_uint<WA> &a, const fw_uint<WB> &b)
{
	static const int SafeRW= (WA+WB <= 0) ? 1 : WA+WB;
	return fw_uint<WA+WB>(ap_uint<SafeRW>( (a.bits,b.bits) ));
}

template<int WA,int WB,int WC>
inline fw_uint<WA+WB+WC> concat(const fw_uint<WA> &a, const fw_uint<WB> &b, const fw_uint<WC> &c)
{
	static const int SafeRW= (WA+WB+WC <= 0) ? 1 : WA+WB+WC;
	return fw_uint<WA+WB+WC>(ap_uint<SafeRW>( (a.bits,b.bits,c.bits) ));
}

template<int WA,int WB,int WC,int WD>
inline fw_uint<WA+WB+WC+WD> concat(const fw_uint<WA> &a, const fw_uint<WB> &b, const fw_uint<WC> &c, const fw_uint<WD> &d)
{
	static const int SafeRW= (WA+WB+WC+WD <= 0) ? 1 : WA+WB+WC+WD;
	return fw_uint<WA+WB+WC+WD>(ap_uint<SafeRW>( (a.bits,b.bits,c.bits,d.bits) ));
}
    
}; // thls

#endif
