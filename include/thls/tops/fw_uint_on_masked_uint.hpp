#ifndef fw_uint_on_masked_uint_hpp
#define fw_uint_on_masked_uint_hpp

#ifndef fw_uint_hpp
#error "Include fw_uint.hpp, rather than this file."
#endif

#include <cstdint>
#include <climits>

#include <gmpxx.h>

template<int W>
struct fw_uint
{
    static_assert(W<=64, "Width too wide.");
    
    static const int width=W;
    
    static const uint64_t MASK=(0xFFFFFFFFFFFFFFFFULL>>(64-W));
    
    uint64_t bits;

    fw_uint()
    	: bits(0)
    {}

    fw_uint(const fw_uint &x)
    	: bits(x.bits)
    {}

    template<int WW=W>
    fw_uint(bool b)
        : bits(b?1:0)
    {
        THLS_STATIC_ASSERT(WW==1, "Can only construct from bool with W=1.");
    }

    explicit fw_uint(int v)
        : bits(v)
    {
        assert(v>=0); // must be non-negative
        assert(v < (1ll<<W)); // Must be in range
    }

    explicit fw_uint(uint64_t v)
        : bits(v & MASK)
    {
        assert(v>=0); // must be non-negative
        assert(v < (1ll<<W)); // Must be in range
    }

    explicit fw_uint(const char *value)
    {
        const char *ep=0;
        bits=strtoull(value, &ep, 0);
        assert(*ep==0);
        assert( bits<= MASK);
        bits=bits&MASK;
    }
        
    static fw_uint from_bits(const uint64_t &x)
    {
        return fw_uint(x);
    }


#ifndef HLS_SYNTHESIS
    explicit fw_uint(mpz_class x)
    {
        if(mpz_sizeinbase(x.get_mpz_t(),2)>W){
            throw std::runtime_error("mpz is too large.");
        }
        std::string dec=x.get_str(); // LAZY
        const char *ep=0;
        bits=strtoull(dec.c_str(), &ep, 0);
        assert(*ep==0);
        assert(bits<=MASK);
        bits=bits&MASK;
    }
#endif

    template<int O>
    fw_uint<thls_ctMax(W,O)> operator+(const fw_uint<O> &o) const
    {
        return fw_uint<thls_ctMax(W,O)>( (bits+o.bits) & MASK );
    }

    fw_uint operator+(int b) const
    {
        return fw_uint( uint64_t(bits+b) & MASK);
    }

    template<int O>
    fw_uint<thls_ctMax(W,O)> operator-(const fw_uint<O> &o) const
    {
        return fw_uint<thls_ctMax(W,O)>( (bits-o.bits) & MASK);
    }

    fw_uint operator-(int b) const
    {
        return fw_uint( uint64_t(bits-b) & MASK);
    }

    template<int O>
    fw_uint<O+W> operator*(const fw_uint<O> &o) const
    {
        return fw_uint<O+W>(bits*o.bits);
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
        return fw_uint<1>(bits == uint64_t(o));
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
    { return fw_uint( bits ^ MASK ); }

    template<int O>
    fw_uint operator&(const fw_uint<O> &o) const
    {
        return fw_uint<W>(bits&o.bits);
    }

    fw_uint operator&(int b) const
    {
        assert(b>=0);
        return fw_uint(bits&b);
    }

    template<int O>
    fw_uint operator|(const fw_uint<O> &o) const
    {
        return fw_uint<W>( (bits|o.bits) & MASK );
    }

    fw_uint operator|(int b) const
    {
        assert(b>=0);
        return fw_uint( uint64_t(bits|b) & MASK);
    }

    template<int O>
    fw_uint operator^(const fw_uint<O> &o) const
    {
        return fw_uint<W>( (bits^o.bits) & MASK );
    }

    fw_uint operator^(int b) const
    {
        assert(b>=0);
        return fw_uint( uint64_t(bits^b) & MASK);
    }


    fw_uint operator>>(int dist) const
    {
        assert(0<=dist && dist<W);
        return fw_uint(bits>>dist);
    }

    std::string to_string() const
    {
        std::string acc="";
        for(int i=0;i<W;i++){
            if((0==(i%8))&&(i!=0)){
                acc="_"+acc;
            }
            acc=(((bits>>i)&1)?"1":"0")+acc;
        }
        return "0b"+acc;
    }

    int to_int() const
    {
        assert(bits <= INT_MAX);
        return (int)bits;
    }

    uint64_t to_uint64() const
    { return bits; }

    bool to_bool() const
    {
        assert(W==1);
        return bits==1;
    }

    #ifndef HLS_SYNTHESIS
    mpz_class to_mpz_class() const
    {
        std::string dec( to_string() );   // LAZY
        return mpz_class(dec);
    }
    #endif

    //explicit operator bool() const
    //{ return to_bool(); }

    operator bool() const
    { return to_bool(); }
};



fw_uint<1> operator||(const fw_uint<1> &a, const fw_uint<1> &b)
{ return fw_uint<1>(a.bits||b.bits); }

fw_uint<1> operator&&(const fw_uint<1> &a, const fw_uint<1> &b)
{ return fw_uint<1>(a.bits&&b.bits); }

#ifndef HLS_SYNTHESIS
template<int W>
std::ostream &operator<<(std::ostream &dst, const fw_uint<W> &x)
{
    dst<<x.to_string();
    return dst;
}
#endif

template<int HI,int LO,int W>
fw_uint<HI-LO+1> get_bits(const fw_uint<W> &x)
{
    static const int WRES=HI-LO+1;
    static const uint64_t MRES=0xFFFFFFFFFFFFFFFFUL>>(64-WRES);
    return fw_uint<HI-LO+1>( (x.bits>>LO) & MRES );
}

template<int WA>
fw_uint<WA> concat(const fw_uint<WA> &a)
{ return a; }

template<int WA,int WB>
fw_uint<WA+WB> concat(const fw_uint<WA> &a, const fw_uint<WB> &b)
{
    return fw_uint<WA+WB>( (a.bits<<WB)+b.bits );
}

template<int WA,int WB,int WC>
fw_uint<WA+WB+WC> concat(const fw_uint<WA> &a, const fw_uint<WB> &b, const fw_uint<WC> &c)
{ return concat(a,concat(b,c)); }

template<int WA,int WB,int WC,int WD>
fw_uint<WA+WB+WC+WD> concat(const fw_uint<WA> &a, const fw_uint<WB> &b, const fw_uint<WC> &c, const fw_uint<WD> &d)
{ return concat(concat(a,b),concat(c,d)); }


#endif
