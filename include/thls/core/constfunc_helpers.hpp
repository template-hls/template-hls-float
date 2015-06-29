#ifndef constexpr_helpers_hpp
#define constexpr_helpers_hpp

#include <stdint.h>
#include <assert.h>

#ifdef VIVADO_HLS

// Vivado HLS doesn't support static_assert
// http://stackoverflow.com/a/1980141
#define THLS_ASSERT_CONCAT_(a, b) a##b
#define THLS_ASSERT_CONCAT(a, b) HLS_ASSERT_CONCAT_(a, b)
#define THLS_STATIC_ASSERT(e,msg) enum { HLS_ASSERT_CONCAT(static_assert_, __COUNTER__) = 1/(!!(e)) }

// Vivado HLS has problems with static const members, and thinks they
// aren't constant
#define THLS_STATIC_CONST(type, name, value) \
	enum _##name : type{ name = (type)(value) }; \
	THLS_STATIC_ASSERT(sizeof(name)==sizeof(type), "Static const integral does not have the right size."); \
	THLS_STATIC_ASSERT( name == (value), "Static const integral does not have the right size.")

#else

#define THLS_STATIC_CONST(type, name, value) \
	static const type name = value
	
#define THLS_STATIC_ASSERT(e,msg)	\
	static_assert(e,msg)

#endif

namespace thls
{

template<int64_t a,int64_t b>
struct ctMax
{ static const int64_t val =  a>b ? a : b; };

template<int64_t a,int64_t b>
struct ctMin
{ static const int64_t val =  a<=b ? a : b; };

template<int64_t a,int64_t b, int64_t c, int64_t d>
struct ctMax4
{ static const int64_t val =  ctMax<ctMax<a,b>::val,ctMax<c,d>::val>::val; };

template<int64_t a,int64_t b, int64_t c, int64_t d>
struct ctMin4
{ static const int64_t val =  ctMin<ctMin<a,b>::val,ctMin<c,d>::val>::val; };

template<int64_t v, int places>
struct ctCheckedShiftLeft
{
	THLS_STATIC_ASSERT(places>=0, "Negative shifts not allowed.");
	THLS_STATIC_ASSERT(places<64, "Shifts must be less than width of value.");
	THLS_STATIC_ASSERT( ((v<<places)>>places)==v, "Value is too large, MSBs have been lost.");
	
	static const int64_t val = v<<places;
};

template<int eA, int64_t vA, int eB, int64_t vB>
struct ctFixCompare
{
	static const int eLo=ctMin<eA,eB>::val;
	
	static const int64_t nA=ctCheckedShiftLeft<vA,(eA-eLo)>::val;
	static const int64_t nB=ctCheckedShiftLeft<vA,(eA-eLo)>::val;
	
	static const int val = (nA < nB) ? -1 : (nA>nB) ? +1 : 0;
};


template<int eA, int64_t vA, int eB, int64_t vB>
struct ctFixLessThan
{ static const bool val = ctFixCompare<eA,vA,eB,vB>::val < 0; };

template<int eA, int64_t vA, int eB, int64_t vB>
struct ctFixLessThanEquals
{ static const bool val = ctFixCompare<eA,vA,eB,vB>::val <= 0; };

template<int eA, int64_t vA, int eB, int64_t vB>
struct ctFixEquals
{ static const bool val = ctFixCompare<eA,vA,eB,vB>::val == 0; };

template<int eA, int64_t vA, int eB, int64_t vB>
struct ctFixGreaterThanEquals
{ static const bool val = ctFixCompare<eA,vA,eB,vB>::val >= 0; };

template<int eA, int64_t vA, int eB, int64_t vB>
struct ctFixGreaterThan
{ static const bool val = ctFixCompare<eA,vA,eB,vB>::val > 0; };

}; // thls

#endif
