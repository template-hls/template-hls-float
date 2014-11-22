#include "thls/types/intval.hpp"

int64_t add_ival_uint8(int64_t _a, int64_t _b)
{
	auto a=make_intval<0,255>(_a);
	auto b=make_intval<0,255>(_b);
	
	return (a+b).val;
}

