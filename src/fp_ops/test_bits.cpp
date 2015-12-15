#include "thls/tops/fw_uint.hpp"

#include <random>

#include <stdint.h>

using namespace thls;

#define assert_fw_uint( x ) if(!fw_uint<1>(x).to_bool()){ fprintf(stderr, "Fail, line %d : %s\n", __LINE__, #x); exit(1); }

void test_bits()
{
    fw_uint<1> t(1), f(0);

    assert_fw_uint( (t!=f).to_bool());
    assert_fw_uint( (t==1).to_bool());
    assert_fw_uint( (f==0).to_bool());

    assert_fw_uint( get_bit<0>(t)==1);

    assert_fw_uint(get_bit<0>(f)==0);

    fw_uint<8> alt(0b10101010);

    assert_fw_uint(get_bit<0>(alt)==0);
    assert_fw_uint(get_bit<1>(alt)==1);
    assert_fw_uint(get_bit<6>(alt)==0);
    assert_fw_uint(get_bit<7>(alt)==1);

    assert_fw_uint( (get_bits<7,0>(alt)==alt) );
    assert_fw_uint( (get_bits<6,3>(alt)==0b0101) );
    assert_fw_uint( (get_bits<7,4>(alt)==0b1010) );
	
	assert_fw_uint(concat(alt,alt)==fw_uint<16>(0b1010101010101010));
	assert_fw_uint(zpad_hi<8>(alt)==fw_uint<16>(0b0000000010101010));
	assert_fw_uint(opad_hi<8>(alt)==fw_uint<16>(0b1111111110101010));
	
	assert_fw_uint(alt*alt == fw_uint<16>(0b111000011100100) );
}
int main()
{
    test_bits();
    return 0;
}
