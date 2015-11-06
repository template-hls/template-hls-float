#include "thls/tops/fw_uint.hpp"

#include <random>

#include <stdint.h>

void test_bits()
{
    fw_uint<1> t(1), f(0);

    assert(t!=f);
    assert(t==1);
    assert(f==0);

    assert( get_bit<0>(t)==1);
    assert(get_bit<1>(t)==0);

    assert(get_bit<0>(f)==0);
    assert(get_bit<1>(f)==0);

    fw_uint<8> alt(0b10101010);

    assert(get_bit<0>(alt)==0);
    assert(get_bit<1>(alt)==1);
    assert(get_bit<6>(alt)==0);
    assert(get_bit<7>(alt)==1);

    assert( (get_bits<7,0>(alt)==alt) );
    assert( (get_bits<6,3>(alt)==0b0101) );
    assert( (get_bits<7,4>(alt)==0b1010) );
	
	assert(concat(alt,alt)==fw_uint<16>(0b1010101010101010));
	assert(zpad_hi<8>(alt)==fw_uint<16>(0b0000000010101010));
	assert(opad_hi<8>(alt)==fw_uint<16>(0b1111111110101010));
	
	assert(alt*alt == fw_uint<16>(0b111000011100100) );
}
int main()
{
    test_bits();
    return 0;
}
