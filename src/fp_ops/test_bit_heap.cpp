#include "thls/tops/bit_heap.hpp"

#include <random>

#include <cmath>

 #include <stdio.h>
     #include <mpfr.h>

std::mt19937 rng;
std::uniform_real_distribution<float> urng;
std::normal_distribution<float> grng;
std::exponential_distribution<float> erng;

using namespace thls;

//#define QUICK

template<int WA,int WB,int WC,int WD>
void test_level_4(const fw_uint<WB> &b,const fw_uint<WC> &c,const fw_uint<WD> &d)
{
	mpz_t ref, got;
	mpz_init(ref);
	mpz_init(got);
	
	for(int i=0; i<(1<<WA); i++){
		fw_uint<WA> a(i);
		
		auto bh_a=fw_uint_to_bit_heap(a);
		auto bh_b=fw_uint_to_bit_heap(b);
		auto bh_c=fw_uint_to_bit_heap(c);
		auto bh_d=fw_uint_to_bit_heap(d);
		
		const int WT=thls_ctMax(thls_ctMax(WC,WD),thls_ctMax(WA,WB));
		
		auto bh_ab=merge_bit_heaps(bh_a,bh_b);
		auto bh_cd=merge_bit_heaps(bh_c,bh_d);
		auto bh_abcd=merge_bit_heaps(bh_ab,bh_cd);
		
		auto abcd=extu<WT>(a)+extu<WT>(b)+extu<WT>(c)+extu<WT>(d);
		
		abcd.to_mpz(ref);
		bit_heap_to_mpz(got, bh_abcd);
		
		if(mpz_cmp(ref, got)){
			std::cerr<<" a="<<a<<", b="<<b<<", c="<<c<<", c="<<d<<" = "<<abcd<<"\n";
			mpfr_fprintf(stderr, "  got=%Zd, ref=%Zd\n", got, ref);
		}
		
		auto bh_abcd_comp=compress(bh_abcd);
		
		bit_heap_to_mpz(got, bh_abcd_comp);
		if(mpz_cmp(ref, got)){
			std::cerr<<" a="<<a<<", b="<<b<<", c="<<c<<", c="<<d<<" = "<<abcd<<"\n";
			mpfr_fprintf(stderr, "  got=%Zd, ref=%Zd\n", got, ref);
		}
		
		static_assert(bh_abcd_comp.max_height<=2, "Bit heap not compressed.");
		
		fw_uint<WT> abcd_comp=bit_heap_collapse(bh_abcd_comp);
		
		abcd_comp.to_mpz(got);
		if(mpz_cmp(ref, got)){
			std::cerr<<" a="<<a<<", b="<<b<<", c="<<c<<", c="<<d<<" = "<<abcd<<"\n";
			mpfr_fprintf(stderr, "  got=%Zd, ref=%Zd\n", got, ref);
		}
	}
	
	mpz_clear(ref);
	mpz_clear(got);
}

template<int WA,int WB,int WC>
void test_level_3(const fw_uint<WB> &b,const fw_uint<WC> &c)
{
	mpz_t ref, got;
	mpz_init(ref);
	mpz_init(got);
	
	for(int i=0; i<(1<<WA); i++){
		fw_uint<WA> a(i);
		
		auto bh_a=fw_uint_to_bit_heap(a);
		auto bh_b=fw_uint_to_bit_heap(b);
		auto bh_c=fw_uint_to_bit_heap(c);
		
		const int WT=thls_ctMax(WC,thls_ctMax(WA,WB));
		
		auto bh_ab=merge_bit_heaps(bh_a,bh_b);
		auto bh_abc=merge_bit_heaps(bh_ab,bh_c);
		
		auto ab=extu<WT>(a)+extu<WT>(b)+extu<WT>(c);
		
		ab.to_mpz(ref);
		bit_heap_to_mpz(got, bh_abc);
		
		if(mpz_cmp(ref, got)){
			std::cerr<<" a="<<a<<", b="<<b<<", c="<<c<<" = "<<ab<<"\n";
			mpfr_fprintf(stderr, "  got=%Zd, ref=%Zd\n", got, ref);
		}
		
		#ifndef QUICK
		test_level_4<1>(a,b,c);
		test_level_4<2>(a,b,c);
		test_level_4<3>(a,b,c);
		#endif
		test_level_4<4>(a,b,c);
		#ifndef QUICK
		test_level_4<5>(a,b,c);
		#endif
	}
	
	mpz_clear(ref);
	mpz_clear(got);
}


template<int WA,int WB>
void test_level_2(const fw_uint<WB> &b)
{
	mpz_t ref, got;
	mpz_init(ref);
	mpz_init(got);
	
	for(int i=0; i<(1<<WA); i++){
		fw_uint<WA> a(i);
		
		auto bh_a=fw_uint_to_bit_heap(a);
		auto bh_b=fw_uint_to_bit_heap(b);
		
		const int WT=thls_ctMax(WA,WB);
		
		auto bh_ab=merge_bit_heaps(bh_a,bh_b);
		
		auto ab=extu<WT>(a)+extu<WT>(b);
		
		ab.to_mpz(ref);
		bit_heap_to_mpz(got, bh_ab);
		
		if(mpz_cmp(ref, got)){
			std::cerr<<" a="<<a<<", b="<<b<<" = "<<ab<<"\n";
			mpfr_fprintf(stderr, "  got=%Zd, ref=%Zd\n", got, ref);
		}
		
		#ifndef QUICK
		test_level_3<1>(a,b);
		test_level_3<2>(a,b);
		test_level_3<3>(a,b);
		test_level_3<4>(a,b);
		#endif
		test_level_3<5>(a,b);
	}
	
	mpz_clear(ref);
	mpz_clear(got);
}

template<int W>
void test_level_1()
{
	for(int i=0; i<(1<<W); i++){
		fw_uint<W> tmp(i);
		
		#ifndef QUICK
		test_level_2<1>(tmp);
		test_level_2<2>(tmp);
		test_level_2<3>(tmp);
		#endif
		test_level_2<4>(tmp);
		#ifndef QUICK
		test_level_2<5>(tmp);
		#endif
	}
}

void test()
{
	#ifndef QUICK
	test_level_1<1>();
	test_level_1<2>();
	#endif
	test_level_1<3>();
	#ifndef QUICK
	test_level_1<4>();
	test_level_1<5>();
	#endif
}


int main()
{
	/*fw_uint<8> fw(0x11);
	auto bh=fw_uint_to_bit_heap(fw);
	
	mpz_t fromBH, fromUI;
	mpz_init(fromBH);
	mpz_init(fromUI);
	
	fw.to_mpz(fromUI);
	bit_heap_to_mpz(fromBH, bh);
	
	mpfr_fprintf(stderr, "%Zd, %Zd\n", fromBH, fromUI);
	
	
	fw_uint<8> fw2(0x3);
	
	auto bh2=fw_uint_to_bit_heap(fw2);
	
	fw2.to_mpz(fromUI);
	bit_heap_to_mpz(fromBH, bh2);
	
	mpfr_fprintf(stderr, "%Zd, %Zd\n", fromBH, fromUI);
	
	auto bh_bh2=merge_bit_heaps(bh,bh2);
	
	(fw+fw2).to_mpz(fromUI);
	bit_heap_to_mpz(fromBH, bh_bh2);
	
	mpfr_fprintf(stderr, "%Zd, %Zd\n", fromBH, fromUI);
	
	auto xx=compress(bh_bh2);
	
	(fw+fw2).to_mpz(fromUI);
	bit_heap_to_mpz(fromBH, xx);
	
	mpfr_fprintf(stderr, "%Zd, %Zd\n", fromBH, fromUI);
	
	
	
	auto bh_bh_bh2=merge_bit_heaps(bh,bh_bh2);
	
	(fw+fw+fw2).to_mpz(fromUI);
	bit_heap_to_mpz(fromBH, bh_bh_bh2);
	
	mpfr_fprintf(stderr, "%Zd, %Zd\n", fromBH, fromUI);
	
	auto xxx=compress(bh_bh_bh2);
	
	(fw+fw+fw2).to_mpz(fromUI);
	bit_heap_to_mpz(fromBH, xxx);
	
	mpfr_fprintf(stderr, "%Zd, %Zd\n", fromBH, fromUI);
	
	
	
	auto bh_bh_bh2_bh2=merge_bit_heaps(bh2,bh_bh_bh2);
	
	(fw+fw+fw2+fw2).to_mpz(fromUI);
	bit_heap_to_mpz(fromBH, bh_bh_bh2_bh2);
	
	mpfr_fprintf(stderr, "%Zd, %Zd\n", fromBH, fromUI);
	
	auto xxxx=compress(bh_bh_bh2_bh2);
	
	(fw+fw+fw2+fw2).to_mpz(fromUI);
	bit_heap_to_mpz(fromBH, xxxx);
	
	mpfr_fprintf(stderr, "%Zd, %Zd\n", fromBH, fromUI);
	*/
	
	test();
	
	fprintf(stderr, "Done\n");


    return 0;
}
	
