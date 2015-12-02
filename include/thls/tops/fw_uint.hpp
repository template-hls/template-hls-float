#ifndef fw_uint_hpp
#define fw_uint_hpp

/* This provides a strongly typed arbitrary width unsigned
   integer, which is designed to catch errors in widths and
   casting. It backends on to ap_uint<W>, and ideally the
   compiler should completely strip it away, so there _should_
   be no run-time overhead.

   Construction and conversion are extremely strict, and explicit
   casting is needed in almost all places.
   
   Because we don't have static_if, it is very likely that there
   will be code-paths which are compiled, but don't get executed
   in any run-time path. So it is legal for negative widths to
   be compiled, but it is illegal (undefined) if any such number
   is instantiated at run-time.

   Implicit constructors are:

     fw_uint<W>()                      // Construct to zero
     fw_uint<W>(const fw_uint<W> &x)   // Copy construct

   Explicit constructors are:

     // This is explicit to avoid calculations being done
     // in ap_uint world when constructing.
     explicit fw_uint<W>(const backend_uint<W> &x);
     
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

//#define THLS_INLINE inline __attribute__((always_inline))
#define THLS_INLINE

#include <stdint.h>
#include <cassert>

#include <cmath>

#if __cplusplus >= 201103L

// ap_uint does not support constexpr anywhere
#define THLS_CONSTEXPR

constexpr int thls_ctMax(const int a, const int b)
{ return a>b ? a : b; }

#define THLS_STATIC_ASSERT(c,msg) static_assert(c, msg)

#else // No C++11

#define THLS_CONSTEXPR

#define HLS_INLINE inline __attribute__((always_inline))

#define THLS_STATIC_ASSERT(c,msg) assert(c)

namespace thls
{
    template<int a,int b>
    struct ctMax_impl
    {
        static const int value=a>b?a:b;
    };
};

#define thls_ctMax(a,b) (thls::ctMax_impl<(a),(b)>::value)

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

//#include "fw_uint_on_cpp_int.hpp"
#include "fw_uint_on_masked_uint.hpp"


/*! Does a cast from on width to another. The two widths
  may be different at compile time, but if the path is
  executed it must be the case that WD==WS. If that is
  not true, the function will assert and poison the result
  if possible.
*/
template<int WD,int WS>
fw_uint<WD> checked_cast(const fw_uint<WS> &s);


template<int HI,int LO,int W>
fw_uint<HI-LO+1> get_bits(const fw_uint<W> &x);

template<int B,int W>
fw_uint<W-B> take(const fw_uint<W> &x)
{
    static_assert(B>=0, "Cannot take negative bits.");
    static_assert(B<=W, "Cannot take more than bits in the variable.");
    return get_bits<B,0>(x);
}

template<int B,int W>
fw_uint<1> get_bit(const fw_uint<W> &x)
{
    return get_bits<B,B,W>(x);
}


template<int W>
fw_uint<1> take_lsb(const fw_uint<W> &x)
{
  return get_bit<0>(x);
}

template<int W>
fw_uint<W-1> drop_lsb(const fw_uint<W> &x)
{
  return get_bits<W-1,1>(x);
}

template<int W>
fw_uint<1> take_msb(const fw_uint<W> &x)
{
  return get_bit<W-1>(x);
}

template<int W>
fw_uint<W-1> drop_msb(const fw_uint<W> &x)
{
  return get_bits<W-2,0>(x);
}


template<int B,int W>
fw_uint<B> take_lsbs(const fw_uint<W> &x)
{
  return get_bits<B-1,0>(x);
}

template<int B,int W>
fw_uint<W-B> drop_lsbs(const fw_uint<W> &x)
{
  return get_bits<W-1,B>(x);
}

template<int B,int W>
fw_uint<B> take_msbs(const fw_uint<W> &x)
{
  return get_bits<W-1,W-B>(x);
}

template<int B,int W>
fw_uint<W-B> drop_msbs(const fw_uint<W> &x)
{
  return get_bits<W-1-B,0>(x);
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
  return concat(zg<P>(),x);
}

template<int P,int W>
fw_uint<P+W> opad_hi(const fw_uint<W> &x)
{
  return concat(og<P>(),x);
}

template<int P,int W>
fw_uint<P+W> zpad_lo(const fw_uint<W> &x)
{
  return concat(x,zg<P>());
}

template<int P,int W>
fw_uint<P+W> opad_lo(const fw_uint<W> &x)
{
  return concat(x,og<P>());
}


template<int B,int W>
fw_uint<B> extu(const fw_uint<W> &x)
{
  static_assert(B>=W, "Cannot extend to smaller width.");
  return zpad_hi<B-W>(x);
}

template<int W>
fw_uint<W> select(bool b, const fw_uint<W> &a, const fw_uint<W> &c)
{
  if(b){
    return a;
  }else{
    return c;
  }
}

template<int WD,int WA>
struct lut
{
private:
  fw_uint<WD> entries[1<<WA];
public:
  template<class TInit>
  lut(const TInit &init)
  {
    for(int i=0; i< (1<<WA); i++){
      entries[i]=fw_uint<WD>(init(i));
    }
  }

  const fw_uint<WD> &operator()(const fw_uint<WA> &addr) const
  {
    return entries[addr.to_int()];
  }
  
};



#endif
