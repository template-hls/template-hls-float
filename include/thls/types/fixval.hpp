#ifndef thls_types_fixval_hpp
#define thls_types_fixval_hpp

#include "thls/core.hpp"

/*! General purpose fixed-point type.
*/
template<int Tshift, int64_t Tlow, int64_t Thigh>
struct fixval
{
	static const int shift = Tshift;
	static const int64_t low = Tlow;
	static const int64_t high = Thigh;

	template<int eA, int64_t lowA, int64_t highA>
	fixval(const fixval<eA,lowA,highA> &a)
	{
		HLS_STATIC_ASSERT(lowA <= highA, "Original interval is invalid (internal error).");
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

/*

//! Fixed-point constant 
template<int Tshift, int64_t Tval>
struct fixval<Tshift,Tval,Tval>
{
	static const int shift = Tshift;
	static const int64_t low = Tval;
	static const int64_t high = Tval;

	template<int eA, int64_t lowA, int64_t highA>
	fixval(const fixval<eA,lowA,highA> &a)
	{
		HLS_STATIC_ASSERT(Tshift <= eA, "Cannot assign to fixed-point value with less precision (explicitly drop the LSBs first).");
		HLS_STATIC_ASSERT((ctFixLessThanEquals<shift, low, eA,lowA>::val), "Cannot assign to value with higher lower bound.");
		HLS_STATIC_ASSERT((ctFixGreaterThanEquals<eA,highA,shift,high>::val), "Cannot assign to value with lower upper bound.");
		
		assert(a.val==Tval);
	}
	
	fixval(int64_t x)
	{
		// We do not store the value
		assert(Tval==x);
	}

	HLS_SC(int64_t,val,Tval);	// Specialise it to a constant
};
*/


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
	
	HLS_SC(int64_t, nLowA, lowA<<sA);
	HLS_SC(int64_t, nHighA, highA<<sA);
	HLS_SC(int64_t, nLowB, lowB<<sB);
	HLS_SC(int64_t, nHighB, highB<<sB);

	typedef fixval<
		eMin,
		ctMin<nLowA,nLowB>::val,
		ctMax<nHighA,nHighB>::val
	> type;
};



///////////////////////////////////////////////////////
// Addition

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
	assert(lowA<=a.val);
	assert(a.val<=highA);
	
	assert(lowB<=b.val);
	assert(b.val<=highB);
	
	typedef add_type<fixval<eA,lowA,highA>,fixval<eB,lowB,highB> > res_traits;
	return (a.val<<res_traits::sA)+(b.val<<res_traits::sB);
}


/////////////////////////////////////////
// Multiplication

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
	assert(lowA<=a.val);
	assert(a.val<=highA);
	
	assert(lowB<=b.val);
	assert(b.val<=highB);
	
	return a.val*b.val;
}

//////////////////////////////////////////////
// Shift right by a variable


template<int eV,int64_t lowV,int64_t highV,int eS,int64_t lowS,int64_t highS>
struct shift_right_type<fixval<eV,lowV,highV>,fixval<eS,lowS,highS> >
{
	HLS_STATIC_ASSERT(eS==0, "Shift must be integer valued.");	// TODO : This is more restrictive than needed. Could usefully have eS>0
	HLS_STATIC_ASSERT(lowS>=0, "Shift value cannot include negative values.");
	
	typedef fixval<eV,(lowV>>lowS),(highV>>lowS)> type;
};


template<int eV,int64_t lowV,int64_t highV,int eS,int64_t lowS,int64_t highS>
struct shift_right_type<fixval<eV,lowV,highV>,fixval<eS,lowS,highS> >::type
	operator>>(const fixval<eV,lowV,highV> &v, const fixval<eS,lowS,highS> &s)
{
	assert(lowV<=v.val);
	assert(v.val<=highV);
	
	assert(lowS<=s.val);
	assert(s.val<=highS);
	
	return v.val>>s.val;
}


//////////////////////////////////////////////
// Shift right by a constant. It is entirely
// legal for both bounds to overflow to zero,
// in which case it collapses to a constant.
// It can also collapse to a constant for something
// like [8,9]>>1, which collapses to [4,4]

template<int places, int eA, int64_t lowA, int64_t highA>
struct shift_right_const_type<places,fixval<eA,lowA,highA> >
{
	typedef fixval<
		eA, (lowA>>places), (highA>>places)
		> type;
};


template<int places, int eA, int64_t lowA, int64_t highA>
typename shift_right_const_type<places,fixval<eA,lowA,highA> >::type
	shift_right_const(const fixval<eA,lowA,highA> &a)
{
	typedef typename shift_right_const_type<places,fixval<eA,lowA,highA> >::type res_t;
	return res_t(a.val>>places);
}

#endif
