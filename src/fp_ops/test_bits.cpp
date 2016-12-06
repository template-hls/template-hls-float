#include "thls/tops/fw_uint.hpp"

#include <random>

#include <stdint.h>

#include <mpfr.h>
#include <iostream>

using namespace thls;

#define assert_fw_uint( x ) if(!fw_uint<1>(x).to_bool()){ fprintf(stderr, "Fail, line %d : %s\n", __LINE__, #x); exit(1); }

#define assert_bool( x ) if(!x){ fprintf(stderr, "Fail, line %d : %s\n", __LINE__, #x); exit(1); }

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
    assert_fw_uint(~concat(alt,alt)==fw_uint<16>(0b0101010101010101));
	assert_fw_uint(zpad_hi<8>(alt)==fw_uint<16>(0b0000000010101010));
	assert_fw_uint(opad_hi<8>(alt)==fw_uint<16>(0b1111111110101010));
	
	assert_fw_uint(alt*alt == fw_uint<16>(0b111000011100100) );
    
    mpz_t tmp1, tmp2;
    mpz_init(tmp1);
    mpz_init(tmp2);
    
    fw_uint<70> one70(1);
    auto var70=one70;
    for(int i=0; i<70; i++){
        auto x=(var70>>i);
        assert_fw_uint( x == one70 );
        
        var70.to_mpz_t(tmp2);
        mpz_set_ui(tmp1, 1);
        mpz_mul_2exp(tmp1, tmp1, i);
        
        mpfr_fprintf(stderr, "i=%d, ref=%Zd, got=%Zd\n", i, tmp1, tmp2);
        
        assert_bool(!mpz_cmp(tmp1, tmp2));
        
        fw_uint<70> y(tmp1);
        assert_bool(!mpz_cmp(tmp1,tmp2));
        
        var70=var70<<1;
    }
    
    one70=fw_uint<70>(1);
    var70=one70;
    for(int i=0; i<70; i++){
        auto x=(var70>>i);
        assert_fw_uint( x == one70 );
        var70=var70+var70;
    }
    
    
    
        fw_uint<120> one120(1);
    auto var120=one120;
    for(int i=0; i<120; i++){
        auto x=(var120>>i);
        assert_fw_uint( x == one120 );
        
        var120.to_mpz_t(tmp2);
        mpz_set_ui(tmp1, 1);
        mpz_mul_2exp(tmp1, tmp1, i);
        
        mpfr_fprintf(stderr, "i=%d, ref=%Zd, got=%Zd\n", i, tmp1, tmp2);
        
        assert_bool(!mpz_cmp(tmp1, tmp2));
        
        fw_uint<120> y(tmp1);
        assert_bool(!mpz_cmp(tmp1,tmp2));
        
        var120=var120<<1;
    }
    
    one120=fw_uint<120>(1);
    var120=one120;
    for(int i=0; i<120; i++){
        auto x=(var120>>i);
        assert_fw_uint( x == one120 );
        var120=var120+var120;
    }
    
    assert_fw_uint( concat(og<16>(),og<16>()) == og<32>() );
    assert_fw_uint( concat(og<32>(),og<32>()) == og<64>() );
    assert_fw_uint( concat(og<64>(),og<64>()) == og<128>() );

    std::cerr<< concat(thls::fw_uint<3>(0b010),thls::og<11>(),thls::og<52>())<<"\n";
    
    mpz_clear(tmp1);
    mpz_clear(tmp2);
}
int main()
{
    /*
    std::cerr<<std::hex<<"   1 = 0x"<<(fw_uint<1>::MASK)<<"\n";
    std::cerr<<std::hex<<"  16 = 0x"<<(fw_uint<16>::MASK)<<"\n";
    std::cerr<<std::hex<<"  32 = 0x"<<(fw_uint<32>::MASK)<<"\n";
    std::cerr<<std::hex<<"  33 = 0x"<<(fw_uint<33>::MASK)<<"\n";
    std::cerr<<std::hex<<"  64 = 0x"<<(fw_uint<64>::MASK)<<"\n";
    fprintf(stderr, "  65 = 0x%llx%08llx\n", uint64_t(fw_uint<65>::MASK>>64), uint64_t(fw_uint<65>::MASK&0xFFFFFFFFFFFFFFFFull));
    fprintf(stderr, " 128 = 0x%llx%08llx\n", uint64_t(fw_uint<128>::MASK>>64), uint64_t(fw_uint<128>::MASK&0xFFFFFFFFFFFFFFFFull));
    */
    
    test_bits();
    fprintf(stderr, "Done\n");
    return 0;
}
