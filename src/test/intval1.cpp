#include "thls/types/intval.hpp"

int64_t add_ival_uint8(int64_t _a, int64_t _b)
{
	auto a=intval<0,127> a(_a);
	auto b=intval<0,127> b(_b);
	
	return (a+b).val;
}

