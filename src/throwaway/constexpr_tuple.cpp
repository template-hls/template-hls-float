
#include "fixval.hpp"
#include "tuple_helper.hpp"

int64_t top_tuple(int64_t _x, int64_t _c0, int64_t _c1, int64_t _c2, int64_t _c3, int64_t _c4)
{
	fixval<0,-8,+7> x(_x);
	fixval<0,-8,+7> c0(_c0);
	fixval<3,-8,+7> c1(_c1);
	fixval<1,-16,+12> c2(_c2);
	fixval<1,-16,+12> c3(_c3);
	fixval<-2,-16,+12> c4(_c4);
	
	auto t1=std::make_tuple(c0,c1);
	auto t2=std::make_tuple(c2,c3);
	
	typedef union_type<decltype(t1),decltype(t2)>::type tuple1u2;
	//typedef union_type<decltype(t1+t2)>::type tuple1p2;

	tuple1u2 xx(t1);
	//tuple1p2 xx(t1+t2);
	
	
	return 0;
}

