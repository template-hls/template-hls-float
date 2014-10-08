#ifndef thls_types_fixval_hpp
#define thls_types_fixval_hpp

#include "thls/core.hpp"

template<int Tshift, int64_t Tlow, int64_t Thigh>
struct fixval
{
	static const int shift = Tshift;
	static const int64_t low = Tlow;
	static const int64_t high = Thigh;

	template<int eA, int64_t lowA, int64_t highA>
	fixval(const fixval<eA,lowA,highA> &a)
	{
		HLS_STATIC_ASSERT(Tshift <= eA, "Cannot assign to fixed-point value with less precision (explicitly drop the LSBs first).");
		HLS_STATIC_ASSERT((ctFixLessThanEquals<shift, low, eA,lowA>::val), "Cannot assign to value with higher lower bound.");
		HLS_STATIC_ASSERT((ctFixGreaterThanEquals<eA,highA,shift,high>::val), "Cannot assign to value with lower upper bound.");
	}
	
	fixval(int64_t x)
		: val(x)
	{
		assert(low<=x);
		assert(x<=high);
	}

	int64_t val;
};

template<int s, int64_t v>
fixval<s,v,v> fixval_constant()
{ return v; }


template<int eA, int64_t lowA, int64_t highA, int eB, int64_t lowB, int64_t highB>
struct union_type<fixval<eA,lowA,highA>,fixval<eB,lowB,highB> >
{
	static const int eMin = ctMin<eA,eB>::val;
	static const int eMax = ctMax<eA,eB>::val;
	static const int sA=eMax-eB;
	static const int sB=eMax-eA;
	
	static const int64_t nLowA=lowA;	//<<sA;
	static const int64_t nHighA=highA;	//<<sA;
	static const int64_t nLowB=lowB;	//<<sB;
	static const int64_t nHighB=highB;	//<<sB;

	typedef fixval<
		eMin,
		ctMin<nLowA,nLowB>::val,
		ctMax<nHighA,nHighB>::val
	> type;
};


template<int eA, int64_t lowA, int64_t highA, int eB, int64_t lowB, int64_t highB>
struct add_type<fixval<eA,lowA,highA>,fixval<eB,lowB,highB> >
{
	static const int eMin = ctMin<eA,eB>::val;
	static const int eMax = ctMax<eA,eB>::val;
	static const int sA=eMax-eB;
	static const int sB=eMax-eA;
	
	typedef fixval<
		eMin,
		(lowA<<sA)+(lowB<<sB),
		(highA<<sA)+(highB<<sB)
	> type;
};


template<int eA, int64_t lowA, int64_t highA, int eB, int64_t lowB, int64_t highB>
typename add_type<fixval<eA,lowA,highA>,fixval<eB,lowB,highB> >::type operator+(const fixval<eA,lowA,highA> &a, const fixval<eB,lowB,highB> &b)
{
	typedef add_type<fixval<eA,lowA,highA>,fixval<eB,lowB,highB> > res_traits;
	return (a.val<<res_traits::sA)+(b.val<<res_traits::sB);
}



template<int eA, int64_t lowA, int64_t highA, int eB, int64_t lowB, int64_t highB>
struct mul_type<fixval<eA,lowA,highA>,fixval<eB,lowB,highB> >
{
	typedef fixval<
			eA+eB,
			ctMin4<lowA*lowB,lowA*highB,highA*lowB,highA*highB>::val,
			ctMax4<lowA*lowB,lowA*highB,highA*lowB,highA*highB>::val
			> type;
};

template<int eA,int64_t lowA, int64_t highA, int eB,int64_t lowB, int64_t highB>
typename mul_type<fixval<eA,lowA,highA>,fixval<eB,lowB,highB> >::type
	operator*(const fixval<eA,lowA,highA> &a, const fixval<eB,lowB,highB> &b)
{
	return a.val*b.val;
}
/*

template<int places, int64_t lowA, int64_t highA>
ival<(lowA>>places),(highA>>places)> shift_right(const ival<lowA,highA> &a)
{
	return a.val>>places;
}
*/

#endif
