#ifndef constexpr_helpers_hpp
#define constexpr_helpers_hpp

#include <stdint.h>
#include <assert.h>

// Vivado HLS doesn't support static_assert
// http://stackoverflow.com/a/1980141
#define HLS_ASSERT_CONCAT_(a, b) a##b
#define HLS_ASSERT_CONCAT(a, b) HLS_ASSERT_CONCAT_(a, b)
#define HLS_STATIC_ASSERT(e,msg) enum { HLS_ASSERT_CONCAT(static_assert_, __COUNTER__) = 1/(!!(e)) }

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
	HLS_STATIC_ASSERT(places>=0, "Negative shifts not allowed.");
	HLS_STATIC_ASSERT(places<64, "Shifts must be less than width of value.");
	HLS_STATIC_ASSERT( ((v<<places)>>places)==v, "Value is too large, MSBs have been lost.");
	
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



template<class A, class B>
struct add_type;

template<>
struct add_type<int64_t,int64_t>
{ typedef int64_t type; };

template<class A, class B>
struct mul_type;

template<>
struct mul_type<int64_t,int64_t>
{ typedef int64_t type; };

template<class A,class B>
struct union_type;

template<>
struct union_type<int64_t,int64_t>
{ typedef int64_t type; };

template<typename A, typename B>
typename union_type<A,B>::type select(bool sel, const A &a, const B &b)
{
	typedef typename union_type<A,B>::type res_t;
	return sel ? res_t(a) : res_t(b);
}

#endif
