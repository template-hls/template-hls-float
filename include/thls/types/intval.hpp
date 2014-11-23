#ifndef thls_types_intval_hpp
#define thls_types_intval_hpp

#include "thls/core.hpp"

#include "thls/types/fixval.hpp"

/*
template<int64_t Tlow, int64_t Thigh>
struct intval
{
	static const int64_t low = Tlow;
	static const int64_t high = Thigh;

	template<int64_t lowA, int64_t highA>
	intval(const intval<lowA,highA> &a)
		: val(a.val)
	{
		HLS_STATIC_ASSERT(low<=lowA, "Lower bound of source is too low.");
		HLS_STATIC_ASSERT(highA<=high, "Upper bound of source is too high.");
	}
	
	intval(int64_t x)
		: val(x)
	{
		assert(low<=x);
		assert(x<=high);
	}

	int64_t val;
};
*/

template <int64_t TLow,int64_t THigh>
using intval = fixval<0,TLow,THigh>;

template<int64_t v>
fixval<0,v,v> constant_intval()
{ return fixval<0,v,v>(v); }

template<int64_t TLow, int64_t THigh>
fixval<0,TLow,THigh> make_intval(int64_t v)
{ return fixval<0,TLow,THigh>(v); }


/*
template<int64_t lowA, int64_t highA, int64_t lowB, int64_t highB>
struct union_type<intval<lowA,highA>,intval<lowB,highB> >
{
	typedef intval<
		ctMin<lowA,lowB>::val,
		ctMin<highA,highB>::val
	> type;
};


template<int64_t lowA, int64_t highA, int64_t lowB, int64_t highB>
struct add_type<intval<lowA,highA>,intval<lowB,highB> >
{ typedef intval<lowA+lowB,highA+highB> type; };

template<int64_t lowA, int64_t highA, int64_t lowB, int64_t highB>
typename add_type<intval<lowA,highA>,intval<lowB,highB> >::type operator+(const intval<lowA,highA> &a, const intval<lowB,highB> &b)
{
	return a.val+b.val;
}

template<int64_t lowA, int64_t highA, int64_t lowB, int64_t highB>
struct mul_type<intval<lowA,highA>,intval<lowB,highB> >
{
	typedef intval<
			ctMin4<lowA*lowB,lowA*highB,highA*lowB,highA*highB>::val,
			ctMax4<lowA*lowB,lowA*highB,highA*lowB,highA*highB>::val
			> type;
};

template<int64_t lowA, int64_t highA, int64_t lowB, int64_t highB>
typename mul_type<intval<lowA,highA>,intval<lowB,highB> >::type
	operator*(const intval<lowA,highA> &a, const intval<lowB,highB> &b)
{
	return a.val*b.val;
}


template<int places, int64_t lowA, int64_t highA>
intval<(lowA>>places),(highA>>places)> shift_right(const intval<lowA,highA> &a)
{
	return a.val>>places;
}

*/

#endif
