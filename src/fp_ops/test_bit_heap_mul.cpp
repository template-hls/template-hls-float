#include "thls/tops/bit_heap_mul.hpp"

#include <random>

#include <cmath>

 #include <stdio.h>
     #include <mpfr.h>

std::mt19937 rng;
std::uniform_real_distribution<float> urng;
std::normal_distribution<float> grng;
std::exponential_distribution<float> erng;

using namespace thls;


int main()
{
	fw_uint<17> a(0x5678CC);
	fw_uint<24> b(0x1234FF);
	
	auto bh=bit_heap_mul<3>(a,b);
	
	mpz_t fromBH, fromUI;
	mpz_init(fromBH);
	mpz_init(fromUI);
	
	(a*b).to_mpz(fromUI);
	bit_heap_to_mpz(fromBH, bh);
	
	std::cerr<<bh<<"\n";
	
	mpfr_fprintf(stderr, "%Zd, %Zd\n", fromBH, fromUI);
	
	
	auto rr=bit_heap_collapse(compress(bh));
	
	rr.to_mpz(fromBH);
	
	mpfr_fprintf(stderr, "%Zd, %Zd\n", fromBH, fromUI);
	
	fprintf(stderr, "Done\n");


    return 0;
}
	
