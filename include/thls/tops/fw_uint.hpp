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


   The width semantics are more strict than ieee.numeric_std,
   so we have:

     decltype(fw_uint<A> + fw_uint<A>)  = fw_uint<A>
     decltype(fw_uint<A> - fw_uint<A>)  = fw_uint<A>

     decltype(fw_uint<A> * fw_uint<B>)  = fw_uint<A+B>

     decltype(fw_uint<A> & fw_uint<A>)  = fw_uint<A>
     decltype(fw_uint<A> | fw_uint<A>)  = fw_uint<A>
     decltype(fw_uint<A> ^ fw_uint<A>)  = fw_uint<A>

     decltype(fw_uint<A> < fw_uint<A>)  = fw_uint<1>
     decltype(fw_uint<A> <= fw_uint<A>) = fw_uint<1>
     decltype(fw_uint<A> == fw_uint<A>) = fw_uint<1>
     decltype(fw_uint<A> >= fw_uint<A>) = fw_uint<1>
     decltype(fw_uint<A> > fw_uint<A>)  = fw_uint<1>
     decltype(fw_uint<A> != fw_uint<A>) = fw_uint<1>

     decltype(fw_uint<A> >> int) = fw_uint<A>  // 0<=shift<A
     decltype(fw_uint<A> << int) = fw_uint<A>  // 0<=shift<A

   Other operations:

     // IDX must be valid if executed (it is legal to compile with invalid IDX)
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
//#define THLS_INLINE

/* Error:
 *
 * In file included from C:\Usr\Xilinx2015.4\Vivado_HLS\2015.4\common\technology\autopilot\ap_int.h:60:
In file included from C:/Usr/Xilinx2015.4/Vivado_HLS/2015.4/common/technology/autopilot/ap_int_syn.h:68:
In file included from C:/Usr/Xilinx2015.4/Vivado_HLS/2015.4/win64/tools/clang/bin\..\lib\clang\3.1/../../../include/c++/4.5.2\iostream:39:
In file included from C:/Usr/Xilinx2015.4/Vivado_HLS/2015.4/win64/tools/clang/bin\..\lib\clang\3.1/../../../include/c++/4.5.2\ostream:39:
In file included from C:/Usr/Xilinx2015.4/Vivado_HLS/2015.4/win64/tools/clang/bin\..\lib\clang\3.1/../../../include/c++/4.5.2\ios:39:
In file included from C:/Usr/Xilinx2015.4/Vivado_HLS/2015.4/win64/tools/clang/bin\..\lib\clang\3.1/../../../include/c++/4.5.2\exception:151:
C:/Usr/Xilinx2015.4/Vivado_HLS/2015.4/win64/tools/clang/bin\..\lib\clang\3.1/../../../include/c++/4.5.2\nested_exception.h:122:61: error: redefinition of default argument
    __throw_with_nested(_Ex&& __ex, const nested_exception* = 0)
                                                            ^ ~
C:/Usr/Xilinx2015.4/Vivado_HLS/2015.4/win64/tools/clang/bin\..\lib\clang\3.1/../../../include/c++/4.5.2\nested_exception.h:110:56: note: previous definition is here
    __throw_with_nested(_Ex&&, const nested_exception* = 0)
 *
 *
 * Solution:
 * Go delete the second zero.
 */



// Workaround for buggy std library which doesn't declare type before
// using it.
namespace std{
	class type_info;
};

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

	namespace thls
	{
		template<bool b>
		struct STATIC_ASSERT_FAIL;

		template<>
		struct STATIC_ASSERT_FAIL<true>
		{
			enum{ value = 1 };
		};
	}

	#define THLS_JOIN2(a,b) a##b
	#define THLS_JOIN(a,b) THLS_JOIN2(a,b)
	#define THLS_STATIC_ASSERT(c,msg) enum { THLS_JOIN(static_assert,__COUNTER__) = sizeof(STATIC_ASSERT_FAIL<bool(c)>::value) }

	namespace thls
	{
		template<int a,int b>
		struct ctMax_impl
		{
			static const int value=a>b?a:b;
		};
	};

	#define thls_ctMax(a,b) (::thls::ctMax_impl<(a),(b)>::value)

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

#if !( defined(THLS_FW_UINT_ON_AP_UINT) || defined(THLS_FW_UINT_ON_CPP_UINT) || defined(THLS_FW_UINT_ON_MASKED_UINT) )
#	ifdef THLS_SYNTHESIS
#		define THLS_FW_UINT_ON_AP_UINT
#	else
#		define THLS_FW_UINT_ON_MASKED_UINT
#	endif
#endif

#if defined(THLS_FW_UINT_ON_AP_UINT)
#	include "fw_uint_on_ap_uint.hpp"
#elif defined(THLS_FW_UINT_ON_CPP_UINT)
#	include "fw_uint_on_cpp_int.hpp"
#elif defined(THLS_FW_UINT_ON_MASKED_UINT)
#	include "fw_uint_on_masked_uint.hpp"
#else
#	error "No fw_uint backend is selected."
#endif


namespace thls
{

/*! Does a cast from on width to another. The two widths
  may be different at compile time, but if the path is
  executed it must be the case that WD==WS. If that is
  not true, the function will assert and poison the result
  if possible.
*/
template<int WD,int WS>
fw_uint<WD> checked_cast(const fw_uint<WS> &s);

/*
template<int HI,int LO,int W>
THLS_INLINE fw_uint<HI-LO+1> get_bits(const fw_uint<W> &x);
*/

THLS_INLINE  fw_uint<1> operator||(const fw_uint<1> &a, const fw_uint<1> &b)
{ return a|b; }

THLS_INLINE  fw_uint<1> operator&&(const fw_uint<1> &a, const fw_uint<1> &b)
{ return a&b; }


template<int W>
THLS_INLINE fw_uint<W> zg()
{
    return fw_uint<W>(0);
}

    static const fw_uint<1> zg1 = zg<1>();
    static const fw_uint<2> zg2 = zg<2>();
    static const fw_uint<3> zg3 = zg<3>();
    static const fw_uint<4> zg4 = zg<4>();

template<int W>
THLS_INLINE fw_uint<W> og()
{
    return ~zg<W>();
}

static const fw_uint<1> og1 = og<1>();
static const fw_uint<2> og2 = og<2>();
static const fw_uint<3> og3 = og<3>();
static const fw_uint<4> og4 = og<4>();

template<int W>
THLS_INLINE fw_uint<W> cg(int x)
{
  return fw_uint<W>(x);
}


template<int B,int W>
THLS_INLINE fw_uint<W-B> take(const fw_uint<W> &x)
{
    assert(B>=0);
    assert(B<=W);
    return get_bits<B,0>(x);
}

template<int B,int W>
THLS_INLINE fw_uint<1> get_bit(const fw_uint<W> &x)
{
    return get_bits<B,B>(x);
}


template<int W>
THLS_INLINE fw_uint<1> take_lsb(const fw_uint<W> &x)
{
  return get_bit<0>(x);
}

template<int W>
THLS_INLINE fw_uint<W-1> drop_lsb(const fw_uint<W> &x)
{
  return get_bits<W-1,1>(x);
}

template<int W>
THLS_INLINE fw_uint<1> take_msb(const fw_uint<W> &x)
{
  return get_bit<W-1>(x);
}

template<int W>
THLS_INLINE fw_uint<W-1> drop_msb(const fw_uint<W> &x)
{
  return get_bits<W-2,0>(x);
}


template<int B,int W>
THLS_INLINE fw_uint<B> take_lsbs(const fw_uint<W> &x)
{
  assert(B>=0);
  assert(B<=W);
  if(B==0){
    return zg<B>();
  }else{
    return get_bits<B-1,0>(x);
  }
}

template<int B,int W>
THLS_INLINE fw_uint<W-B> drop_lsbs(const fw_uint<W> &x)
{
  assert(B>=0);
  assert(B<=W);
  if(B==W){
    return zg<W-B>();
  }else{
    return get_bits<W-1,B>(x);
  }
}

template<int B,int W>
THLS_INLINE fw_uint<B> take_msbs(const fw_uint<W> &x)
{
  assert(B>=0);
  assert(B<=W);
  if(B==0){
    return zg<B>();
  }else{
    return get_bits<W-1,W-B>(x);
  }
}

template<int B,int W>
THLS_INLINE fw_uint<W-B> drop_msbs(const fw_uint<W> &x)
{
  assert(B>=0);
  assert(B<=W);
  if(B==W){
    return zg<W-B>();
  }else{
    return get_bits<W-1-B,0>(x);
  }
}


template<int A,int B>
THLS_INLINE fw_uint<A+B> full_mul(const fw_uint<A> &a, const fw_uint<B> &b)
{
    return a*b;
}



template<int P,int W>
THLS_INLINE fw_uint<P+W> zpad_hi(const fw_uint<W> &x)
{
  return concat(zg<P>(),x);
}

template<int P,int W>
THLS_INLINE fw_uint<P+W> opad_hi(const fw_uint<W> &x)
{
  return concat(og<P>(),x);
}

template<int P,int W>
THLS_INLINE fw_uint<P+W> zpad_lo(const fw_uint<W> &x)
{
  return concat(x,zg<P>());
}

template<int P,int W>
THLS_INLINE fw_uint<P+W> opad_lo(const fw_uint<W> &x)
{
  return concat(x,og<P>());
}


template<int B,int W>
THLS_INLINE fw_uint<B> extu(const fw_uint<W> &x)
{
  THLS_STATIC_ASSERT(B>=W, "Cannot extend to smaller width.");
  return zpad_hi<B-W>(x);
}

template<int W>
THLS_INLINE fw_uint<W> add_with_cin(const fw_uint<W> &x, const fw_uint<W> &y, const fw_uint<1> &cin)
{
  return (x+y+zpad_hi<W-1>(cin));

  //return drop_lsb(concat(x,cin)+concat(y,cin));
}

template<int W>
THLS_INLINE fw_uint<W> add_sub(const fw_uint<1> &doSub, const fw_uint<W> &x, const fw_uint<W> &y)
{
  return add_with_cin(x, select(doSub, ~y, y), doSub);
}

template<int W>
THLS_INLINE fw_uint<W> add_cin(const fw_uint<W> &x, const fw_uint<1> &cin)
{
  return (x+zpad_hi<W-1>(cin));
}

template<class T>
THLS_INLINE const T &select(const fw_uint<1> &c0, const T &v0, const T &def)
{
  if(c0.to_bool()){
    return v0;
  }else{
    return def;
  }
}

template<class T>
THLS_INLINE const T &select(bool c0, const T &v0, const T &def)
{
  if(c0){
    return v0;
  }else{
    return def;
  }
}

template<class T,class C1>
THLS_INLINE const T &select(const fw_uint<1> &c0, const T &v0, const C1 &c1, const T &v1, const T &def)
{
  if(c0.to_bool()){
    return v0;
  }else{
    return select(c1,v1,def);
  }
}

template<class T,class C1>
THLS_INLINE const T &select(bool c0, const T &v0, const C1 &c1, const T &v1, const T &def)
{
  if(c0){
    return v0;
  }else{
    return select(c1,v1,def);
  }
}

template<class T,class C1,class C2>
THLS_INLINE const T &select(const fw_uint<1> &c0, const T &v0, const C1 &c1, const T &v1, const C2 &c2, const T &v2, const T &def)
{
  if(c0.to_bool()){
    return v0;
  }else{
    return select(c1,v1,c2,v2,def);
  }
}

template<class T,class C1,class C2>
THLS_INLINE const T &select(bool c0, const T &v0, const C1 &c1, const T &v1, const C2 &c2, const T &v2, const T &def)
{
  if(c0){
    return v0;
  }else{
    return select(c1,v1,c2,v2,def);
  }
}

template<class T,class C1,class C2,class C3>
THLS_INLINE const T &select(const fw_uint<1> &c0, const T &v0, const C1 &c1, const T &v1, const C2 &c2, const T &v2, const C3 &c3, const T &v3, const T &def)
{
  if(c0.to_bool()){
    return v0;
  }else{
    return select(c1,v1,c2,v2,c3,v3,def);
  }
}

template<class T,class C1,class C2,class C3>
THLS_INLINE const T &select(bool c0, const T &v0, const C1 &c1, const T &v1, const C2 &c2, const T &v2, const C3 &c3, const T &v3, const T &def)
{
  if(c0){
    return v0;
  }else{
    return select(c1,v1,c2,v2,c3,v3,def);
  }
}


template<class T,class C1,class C2,class C3,class C4>
THLS_INLINE const T &select(const fw_uint<1> &c0, const T &v0, const C1 &c1, const T &v1, const C2 &c2, const T &v2, const C3 &c3, const T &v3, const C4 &c4, const T &v4, const T &def)
{
  if(c0.to_bool()){
    return v0;
  }else{
    return select(c1,v1,c2,v2,c3,v3,c4,v4,def);
  }
}

template<class T,class C1,class C2,class C3,class C4>
THLS_INLINE const T &select(bool c0, const T &v0, const C1 &c1, const T &v1, const C2 &c2, const T &v2, const C3 &c3, const T &v3, const C4 &c4, const T &v4, const T &def)
{
  if(c0){
    return v0;
  }else{
    return select(c1,v1,c2,v2,c3,v3,c4,v4,def);
  }
}


template<class T,class C1,class C2,class C3,class C4,class C5>
THLS_INLINE const T &select(const fw_uint<1> &c0, const T &v0, const C1 &c1, const T &v1, const C2 &c2, const T &v2, const C3 &c3, const T &v3, const C4 &c4, const T &v4, const C5 &c5, const T &v5,const T &def)
{
  if(c0.to_bool()){
    return v0;
  }else{
    return select(c1,v1,c2,v2,c3,v3,c4,v4,c5,v5,def);
  }
}

template<class T,class C1,class C2,class C3,class C4,class C5>
THLS_INLINE const T &select(bool c0, const T &v0, const C1 &c1, const T &v1, const C2 &c2, const T &v2, const C3 &c3, const T &v3, const C4 &c4, const T &v4, const C5 &c5, const T &v5, const T &def)
{
  if(c0){
    return v0;
  }else{
    return select(c1,v1,c2,v2,c3,v3,c4,v4,c5,v5,def);
  }
}


template<int W>
THLS_INLINE fw_uint<W> copybit(const fw_uint<1> &b)
{
  return select(b, og<W>(), zg<W>());
}

/*

#error "Do not use - very poor implementation in VHLS"

template<int W>
THLS_INLINE fw_uint<W> select_unique(
  const fw_uint<1> &c0, const fw_uint<W> &v0,
  const fw_uint<1> &c1, const fw_uint<W> &v1
)
{
  assert((zpad_hi<1>(c0)+zpad_hi<1>(c1) == cg<2>(1)).to_bool());

  return (copybit<W>(c0)&v0) | (copybit<W>(c1)&v1);
}

template<int W>
THLS_INLINE fw_uint<W>select_unique(
  const fw_uint<1> &c0, fw_uint<W>v0,
  const fw_uint<1> &c1, fw_uint<W>v1,
  const fw_uint<1> &c2, fw_uint<W>v2
)
{
  assert((zpad_hi<1>(c0)+zpad_hi<1>(c1)+zpad_hi<1>(c2) == cg<2>(1)).to_bool());

  return (copybit<W>(c0)&v0) | (copybit<W>(c1)&v1) | (copybit<W>(c2)&v2);
}

template<int W>
THLS_INLINE fw_uint<W>select_unique(
  const fw_uint<1> &c0, fw_uint<W>v0,
  const fw_uint<1> &c1, fw_uint<W>v1,
  const fw_uint<1> &c2, fw_uint<W>v2,
  const fw_uint<1> &c3, fw_uint<W>v3
)
{
  assert((zpad_hi<2>(c0)+zpad_hi<2>(c1)+zpad_hi<2>(c2)+zpad_hi<2>(c2) == cg<3>(1)).to_bool());

  return (copybit<W>(c0)&v0) | (copybit<W>(c1)&v1) | (copybit<W>(c2)&v2) | (copybit<W>(c3)&v3);
}

template<int W>
THLS_INLINE fw_uint<W>select_unique(
  const fw_uint<1> &c0, fw_uint<W>v0,
  const fw_uint<1> &c1, fw_uint<W>v1,
  const fw_uint<1> &c2, fw_uint<W>v2,
  const fw_uint<1> &c3, fw_uint<W>v3,
  const fw_uint<1> &c4, fw_uint<W>v4
)
{
  assert((zpad_hi<2>(c0)+zpad_hi<2>(c1)+zpad_hi<2>(c2)+zpad_hi<2>(c3)+zpad_hi<2>(c4) == cg<3>(1)).to_bool());

  return (copybit<W>(c0)&v0) | (copybit<W>(c1)&v1) | (copybit<W>(c2)&v2) | (copybit<W>(c3)&v3) | (copybit<W>(c4)&v4);
}

template<int W>
THLS_INLINE fw_uint<W>select_unique(
  const fw_uint<1> &c0, fw_uint<W>v0,
  const fw_uint<1> &c1, fw_uint<W>v1,
  const fw_uint<1> &c2, fw_uint<W>v2,
  const fw_uint<1> &c3, fw_uint<W>v3,
  const fw_uint<1> &c4, fw_uint<W>v4,
  const fw_uint<1> &c5, fw_uint<W>v5
)
{
  assert((zpad_hi<2>(c0)+zpad_hi<2>(c1)+zpad_hi<2>(c2)+zpad_hi<2>(c3)+zpad_hi<2>(c4)+zpad_hi<2>(c5) == cg<3>(1)).to_bool());

  return (copybit<W>(c0)&v0) | (copybit<W>(c1)&v1) | (copybit<W>(c2)&v2) | (copybit<W>(c3)&v3) | (copybit<W>(c4)&v4) | (copybit<W>(c5)&v5);
}

template<int W>
THLS_INLINE fw_uint<W>select_unique(
  const fw_uint<1> &c0, fw_uint<W>v0,
  const fw_uint<1> &c1, fw_uint<W>v1,
  const fw_uint<1> &c2, fw_uint<W>v2,
  const fw_uint<1> &c3, fw_uint<W>v3,
  const fw_uint<1> &c4, fw_uint<W>v4,
  const fw_uint<1> &c5, fw_uint<W>v5,
  const fw_uint<1> &c6, fw_uint<W>v6
)
{
  assert( (zpad_hi<2>(c0)+zpad_hi<2>(c1)+zpad_hi<2>(c2)+zpad_hi<2>(c3)+zpad_hi<2>(c4)+zpad_hi<2>(c5)+zpad_hi<2>(c6) == cg<3>(1)).to_bool() );

  return (copybit<W>(c0)&v0) | (copybit<W>(c1)&v1) | (copybit<W>(c2)&v2) | (copybit<W>(c3)&v3) | (copybit<W>(c4)&v4) | (copybit<W>(c5)&v5) | (copybit<W>(c6)&v6);
}

template<int W>
THLS_INLINE fw_uint<W>select_unique(
  const fw_uint<1> &c0, fw_uint<W>v0,
  const fw_uint<1> &c1, fw_uint<W>v1,
  const fw_uint<1> &c2, fw_uint<W>v2,
  const fw_uint<1> &c3, fw_uint<W>v3,
  const fw_uint<1> &c4, fw_uint<W>v4,
  const fw_uint<1> &c5, fw_uint<W>v5,
  const fw_uint<1> &c6, fw_uint<W>v6,
  const fw_uint<1> &c7, fw_uint<W>v7
)
{
  assert( (zpad_hi<3>(c0)+zpad_hi<3>(c1)+zpad_hi<3>(c2)+zpad_hi<3>(c3)+zpad_hi<3>(c4)+zpad_hi<3>(c5)+zpad_hi<3>(c7) == cg<4>(1)).to_bool() );

  return (copybit<W>(c0)&v0) | (copybit<W>(c1)&v1) | (copybit<W>(c2)&v2) | (copybit<W>(c3)&v3) | (copybit<W>(c4)&v4) | (copybit<W>(c5)&v5) | (copybit<W>(c6)&v6) | (copybit<W>(c7)&v7);
}
*/


    template<int HI,int LO,int W>
    struct concat_array_impl {
    	THLS_INLINE static fw_uint<(HI - LO + 1) * W> go(const fw_uint<W> *x) {
          return concat(x[HI], concat_array_impl<HI - 1, LO, W>::go(x));
        };
    };

    template<int LO,int W>
    struct concat_array_impl<LO,LO,W> {
    	THLS_INLINE static fw_uint<W> go(const fw_uint<W> *x) {
          return x[LO];
        };
    };

    template<int HI,int LO,int W>
    THLS_INLINE fw_uint<(HI-LO+1)*W> concat_array(const fw_uint<W> *x)
    {
      static_assert(HI>=LO, "Range must be non-empty");
      return concat_array_impl<HI,LO,W>::go(x);
    };


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

  THLS_INLINE const fw_uint<WD> &operator()(const fw_uint<WA> &addr) const
  {
    return entries[addr.to_int()];
  }
};



template<int W>
inline std::ostream &operator<<(std::ostream &dst, const fw_uint<W> &x)
{
  #ifndef THLS_SYNTHESIS
    dst<<x.to_string();
  #endif
    return dst;
}



}; // thls

#endif
