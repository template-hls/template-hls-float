#include "thls/types/intval.hpp"

#include <typeinfo>
#include <iostream>

int64_t add_ival_uint8(int64_t _a, int64_t _b)
{
	auto a=make_intval<0,255>(_a);
	auto b=make_intval<0,255>(_b);
	
	return (a+b).val;
}

int64_t mul_ival_uint8(int64_t _a, int64_t _b)
{
	auto a=make_intval<0,255>(_a);
	auto b=make_intval<0,255>(_b);
	
	return (a*b).val;
}

int64_t sr_0_ival_uint8(int64_t _a)
{
	return shift_right_const<0>(make_intval<0,255>(_a)).val;
}

int64_t sr_1_ival_uint8(int64_t _a)
{
	auto res=shift_right_const<1>(make_intval<0,255>(_a));
	HLS_STATIC_ASSERT(res.low==0,"Should have range [0,127].");
	HLS_STATIC_ASSERT(res.high==127,"Should have range [0,127].");
	return res.val;
}

int64_t sr_7_ival_uint8(int64_t _a)
{
	return shift_right_const<7>(make_intval<0,255>(_a)).val;
}

int64_t sr_8_ival_uint8(int64_t _a)
{
	auto res=shift_right_const<8>(make_intval<0,255>(_a));
	HLS_STATIC_ASSERT(res.low==0,"Should collapse to a point.");
	HLS_STATIC_ASSERT(res.high==0,"Should collapse to a point.");
	return res.val;
}
