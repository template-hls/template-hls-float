#ifndef fw_uint_hpp
#define fw_uint_hpp

/* This provides a strongly typed arbitrary width unsigned
   integer, which is designed to catch errors in widths and
   casting. It backends on to ap_uint<W>, and ideally the
   compiler should completely strip it away, so there _should_
   be no run-time overhead.

   Construction and conversion are extremely strict, and explicit
   casting is needed in almost all places.

   Implicit constructors are:

     fw_uint<W>()                      // Construct to zero
     fw_uint<W>(const hw_uint<W> &x)   // Copy construct

   Explicit constructors are:

     // This is explicit to avoid calculations being done
     // in ap_uint world when constructing.
     explicit fw_uint<W>(const ap_uint<W> &x);

     fw_uint<1>(bool x);      // Only available if W==1
     explicit fw_uint<W>(int x);       // x must be non-negative and < 2**W
     explicit fw_uint<W>(uint64_t x);  // x must be <= 2**W


   The width semantics are designed to map to ieee.numeric_std,
   so we have:

     decltype(fw_uint<A> + fw_uint<B>)  = fw_uint<max(A,B)>
     decltype(fw_uint<A> - fw_uint<B>)  = fw_uint<max(A,B)>

     decltype(fw_uint<A> * fw_uint<B>)  = fw_uint<A+B>

     decltype(fw_uint<A> & fw_uint<B>)  = fw_uint<A>
     decltype(fw_uint<A> | fw_uint<B>)  = fw_uint<A>
     decltype(fw_uint<A> ^ fw_uint<B>)  = fw_uint<A>

     decltype(fw_uint<A> < fw_uint<B>)  = bool
     decltype(fw_uint<A> <= fw_uint<B>) = bool
     decltype(fw_uint<A> == fw_uint<B>) = bool
     decltype(fw_uint<A> >= fw_uint<B>) = bool
     decltype(fw_uint<A> > fw_uint<B>)  = bool
     decltype(fw_uint<A> != fw_uint<B>) = bool

     decltype(fw_uint<A> >> int) = fw_uint<A>  // 0<=shift<A
     decltype(fw_uint<A> << int) = fw_uint<A>  // 0<=shift<A

   Other operations:

     decltype(get_bit<IDX>(fw_uint<A>)) = bool

     decltype(get_bits<HI,LO>(fw_uint<A>)) = fw_uint<HI-LO+1>

     // Add N zeros at the hi or lo end
     decltype(zpad_hi<N>(fw_uint<W>)) = fw_uint<N+W>
     decltype(zpad_lo<N>(fw_uint<W>)) = fw_uint<W+N>

     // Add N ones at the hi or lo end
     decltype(opad_hi<N>(fw_uint<W>)) = fw_uint<N+W>
     decltype(opad_lo<N>(fw_uint<W>)) = fw_uint<W+N>

     decltype(concat(fw_uint<A>,fw_uint<B>,...fw_uint<Z>)) = fw_uint<A+B+...+Z>

*/

#define THLS_INLINE inline __attribute__((always_inline))

#include "ap_int.h"

#include <stdint.h>
#include <cassert>

#include <cmath>

#ifndef HLS_SYNTHESIS
#include <gmpxx.h>
#endif

#if __cplusplus >= 201103L

// ap_uint does not support constexpr anywhere
#define THLS_CONSTEXPR

constexpr int thls_ctMax(const int a, const int b)
{ return a>b ? a : b; }

#else // No C++11

#define THLS_CONSTEXPR

#define HLS_INLINE inline __attribute__((always_inline))

#define static_assert(c,msg) assert(c)

namespace thls
{
    template<int a,int b>
    struct ctMax_impl
    {
        static const int value=a>b?a:b;
    };
};

#define thls_ctMax(a,b) (ctMax_impl<(a),(b)>::value)

#endif

namespace detail
{
    template<int W>
    struct is_one
    { static const int value = 0; };

    template<>
    struct is_one<1>
    { static const int value = 1; };
};

template<int W>
struct fw_uint
{
    static const int width=W;

    ap_uint<W> bits;

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
    fw_uint(bool b)
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

    explicit fw_uint(const ap_uint<W> &x)
        : bits(x)
    {}

    explicit fw_uint(const ap_range_ref<W,false> &x)
        : bits(x)
    {}

#ifndef HLS_SYNTHESIS
    explicit fw_uint(mpz_class x)
    {
        if(mpz_sizeinbase(x.get_mpz_t(),2)>W){
            throw std::runtime_error("mpz is too large.");
        }
        std::string dec=x.get_str(); // LAZY
        bits=ap_uint<W>(dec.c_str());
    }
#endif

    template<int O>
    fw_uint<thls_ctMax(W,O)> operator+(const fw_uint<O> &o) const
    {
        ap_uint<thls_ctMax(W,O)> a(bits), b(o.bits);
        return fw_uint<thls_ctMax(W,O)>(ap_uint<thls_ctMax(W,O)>(a+b));
    }

    fw_uint operator+(int b) const
    {
        return fw_uint(ap_uint<W>(bits+b));
    }

    template<int O>
    fw_uint<thls_ctMax(W,O)> operator-(const fw_uint<O> &o) const
    {
        ap_uint<thls_ctMax(W,O)> a(bits), b(o.bits);
        return fw_uint<thls_ctMax(W,O)>(ap_uint<thls_ctMax(W,O)>(a-b));
    }

    fw_uint operator-(int b) const
    {
        return fw_uint(ap_uint<W>(bits-b));
    }

    template<int O>
    fw_uint<O+W> operator*(const fw_uint<O> &o) const
    {
        ap_uint<W+O> a(bits), b(o.bits);
        return fw_uint<O+W>(ap_uint<W+O>(a*b));
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
    { return fw_uint(ap_uint<W>(~bits)); }

    template<int O>
    fw_uint operator&(const fw_uint<O> &o) const
    {
        ap_uint<W> b(o.bits);
        return fw_uint<W>(ap_uint<W>(bits&b));
    }

    fw_uint operator&(int b) const
    {
        assert(b>=0);
        return fw_uint(ap_uint<W>(bits&b));
    }

    template<int O>
    fw_uint operator|(const fw_uint<O> &o) const
    {
        ap_uint<W> b(o.bits);
        return fw_uint<W>(ap_uint<W>(bits|b));
    }

    fw_uint operator|(int b) const
    {
        assert(b>=0);
        return fw_uint(ap_uint<W>(bits|b));
    }

    template<int O>
    fw_uint operator^(const fw_uint<O> &o) const
    {
        ap_uint<W> b(o.bits);
        return fw_uint<W>(ap_uint<W>(bits^b));
    }

    fw_uint operator^(int b) const
    {
        assert(b>=0);
        return fw_uint(bits^b);
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
            acc=(bits[i]?"1":"0")+acc;
        }
        return "0b"+acc;
    }

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

    #ifndef HLS_SYNTHESIS
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



fw_uint<1> operator||(const fw_uint<1> &a, const fw_uint<1> &b)
{ return fw_uint<1>(ap_uint<1>(a.bits||b.bits)); }

fw_uint<1> operator&&(const fw_uint<1> &a, const fw_uint<1> &b)
{ return fw_uint<1>(ap_uint<1>(a.bits&&b.bits)); }

template<int W>
std::ostream &operator<<(std::ostream &dst, const fw_uint<W> &x)
{
    dst<<x.to_string();
    return dst;
}


template<int B,int W>
fw_uint<W-B> take(const fw_uint<W> &x)
{
    static_assert(B>=0, "Cannot take negative bits.");
    static_assert(B<=W, "Cannot take more than bits in the variable.");
    return fw_uint<W-B>(x.bits); // truncate
}

template<int B,int W>
fw_uint<1> get_bit(const fw_uint<W> &x)
{
    return fw_uint<1>(ap_uint<1>(x.bits[B]));
}

template<int HI,int LO,int W>
fw_uint<HI-LO+1> get_bits(const fw_uint<W> &x)
{
    return fw_uint<HI-LO+1>(ap_uint<HI-LO+1>(x.bits.range(HI,LO)));
}

template<int A,int B>
fw_uint<A+B> full_mul(const fw_uint<A> &a, const fw_uint<B> &b)
{
    return a*b;
}

template<int W>
fw_uint<W> zg()
{
    return fw_uint<W>(0);
}

template<int W>
fw_uint<W> og()
{
    return ~zg<W>();
}

template<int P,int W>
fw_uint<P+W> zpad_hi(const fw_uint<W> &x)
{
    return fw_uint<P+W>(ap_uint<P+W>(  (zg<P>().bits,x.bits)  ));
}

template<int P,int W>
fw_uint<P+W> opad_hi(const fw_uint<W> &x)
{
    return fw_uint<P+W>(ap_uint<P+W>(  (og<P>().bits,x.bits)  ));
}

template<int P,int W>
fw_uint<P+W> zpad_lo(const fw_uint<W> &x)
{
    return fw_uint<P+W>(ap_uint<P+W>(  (x.bits,zg<P>().bits)  ));
}

template<int P,int W>
fw_uint<P+W> opad_lo(const fw_uint<W> &x)
{
    return fw_uint<P+W>(ap_uint<P+W>(  (x.bits,og<P>().bits)  ));
}

#if __cplusplus >= 201103L

template<class ...C>
struct concat_impl;

template<int WF, class ...CR>
struct concat_impl<fw_uint<WF>,CR...>
{
    static const int S = WF+concat_impl<CR...>::S;
};

template<int WF>
struct concat_impl<fw_uint<WF> >
{
    static const int S = WF;
};

template<>
struct concat_impl<>
{
    static const int S = 0;
};

fw_uint<0> concat()
{
    return fw_uint<0>();
}

template<class CH, class ...CR>
fw_uint<concat_impl<CH,CR...>::S> concat(const CH &h, const CR &...r)
{
    ap_uint<CH::width> a(h.bits);
    ap_uint<concat_impl<CR...>::S> b(concat(r...).bits);

    return fw_uint<concat_impl<CH,CR...>::S>(
        ap_uint<concat_impl<CH,CR...>::S>(
            (a,b)
        )
    );
}

#else // No C++11

template<int WA>
fw_uint<WA> concat(const fw_uint<WA> &a)
{ return a; }

template<int WA,int WB>
fw_uint<WA+WB> concat(const fw_uint<WA> &a, const fw_uint<WB> &b)
{ return fw_uint<WA+WB>(ap_uint<WA+WB>( (a.bits,b.bits) )); }

template<int WA,int WB,int WC>
fw_uint<WA+WB+WC> concat(const fw_uint<WA> &a, const fw_uint<WB> &b, const fw_uint<WC> &c)
{ return fw_uint<WA+WB+WC>(ap_uint<WA+WB+WC>( (a.bits,b.bits,c.bits) )); }

template<int WA,int WB,int WC,int WD>
fw_uint<WA+WB+WC+WD> concat(const fw_uint<WA> &a, const fw_uint<WB> &b, const fw_uint<WC> &c, const fw_uint<WD> &d)
{ return fw_uint<WA+WB+WC+WD>(ap_uint<WA+WB+WC+WD>( (a.bits,b.bits,c.bits,d.bits) )); }

#endif

#endif
