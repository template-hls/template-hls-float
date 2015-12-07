#include "thls/tops/row_heap.hpp"

#include <random>

#include <cmath>

 #include <stdio.h>
     #include <mpfr.h>
	 
#include <iostream>

std::mt19937 rng;
std::uniform_real_distribution<float> urng;
std::normal_distribution<float> grng;
std::exponential_distribution<float> erng;

using namespace thls;

//#define QUICK

template<int WA,int WB,int WC,int WD,int WE>
void test_level_5(const fw_uint<WB> &b,const fw_uint<WC> &c,const fw_uint<WD> &d,const fw_uint<WE> &e)
{
	mpz_t ref, got;
	mpz_init(ref);
	mpz_init(got);
	
	for(int i=0; i<(1<<WA); i++){
		fw_uint<WA> a(i);
		
		auto bh_a=fw_uint_to_row_heap(a);
		auto bh_b=fw_uint_to_row_heap(b);
		auto bh_c=fw_uint_to_row_heap(c);
		auto bh_d=fw_uint_to_row_heap(d);
		auto bh_e=fw_uint_to_row_heap(e);
		
		const int WT=thls_ctMax(WE,thls_ctMax(thls_ctMax(WC,WD),thls_ctMax(WA,WB)));
		
		auto bh_abcde=bh_a+bh_b+bh_c+bh_d+bh_e;
		
		auto abcde=extu<WT>(a)+extu<WT>(b)+extu<WT>(c)+extu<WT>(d)+extu<WT>(e);
		
		abcde.to_mpz(ref);
		row_heap_to_mpz(got, bh_abcde);
		
		if(mpz_cmp(ref, got)){
			std::cerr<<"heap: a="<<a<<", b="<<b<<", c="<<c<<", d="<<d<<", e="<<e<<" = "<<abcde<<"\n";
			mpfr_fprintf(stderr, "  got=%Zd, ref=%Zd\n", got, ref);
		}
		
		auto bh_abcde_comp=compress(bh_abcde);
		
		row_heap_to_mpz(got, bh_abcde_comp);
		if(mpz_cmp(ref, got)){
			std::cerr<<"comp: a="<<a<<", b="<<b<<", c="<<c<<", d="<<d<<", e="<<e<<" = "<<abcde<<"\n";
			mpfr_fprintf(stderr, "  got=%Zd, ref=%Zd\n", got, ref);
		}
		
		
		static_assert(bh_abcde_comp.depth<=2, "Bit heap not compressed.");
		
		fw_uint<WT> abcde_comp=collapse(bh_abcde_comp);
		
		abcde_comp.to_mpz(got);
		if(mpz_cmp(ref, got)){
			std::cerr<<" a="<<a<<", b="<<b<<", c="<<c<<", d="<<d<<", e="<<e<<" = "<<abcde<<"\n";
			mpfr_fprintf(stderr, "  got=%Zd, ref=%Zd\n", got, ref);
		}
		
		test_level_5<4>(a,b,c,d);
	}
	
	mpz_clear(ref);
	mpz_clear(got);
}

template<int WA,int WB,int WC,int WD>
void test_level_4(const fw_uint<WB> &b,const fw_uint<WC> &c,const fw_uint<WD> &d)
{
	mpz_t ref, got;
	mpz_init(ref);
	mpz_init(got);
	
	for(int i=0; i<(1<<WA); i++){
		fw_uint<WA> a(i);
		
		auto bh_a=fw_uint_to_row_heap(a);
		auto bh_b=fw_uint_to_row_heap(b);
		auto bh_c=fw_uint_to_row_heap(c);
		auto bh_d=fw_uint_to_row_heap(d);
		
		const int WT=thls_ctMax(thls_ctMax(WC,WD),thls_ctMax(WA,WB));
		
		auto bh_abcd=bh_a+bh_b+bh_c+bh_d;
		
		auto abcd=extu<WT>(a)+extu<WT>(b)+extu<WT>(c)+extu<WT>(d);
		
		abcd.to_mpz(ref);
		row_heap_to_mpz(got, bh_abcd);
		
		if(mpz_cmp(ref, got)){
			std::cerr<<"heap: a="<<a<<", b="<<b<<", c="<<c<<", d="<<d<<" = "<<abcd<<"\n";
			mpfr_fprintf(stderr, "  got=%Zd, ref=%Zd\n", got, ref);
		}
		
		auto bh_abcd_comp=compress(bh_abcd);
		
		row_heap_to_mpz(got, bh_abcd_comp);
		if(mpz_cmp(ref, got)){
			std::cerr<<"comp: a="<<a<<", b="<<b<<", c="<<c<<", d="<<d<<" = "<<abcd<<"\n";
			mpfr_fprintf(stderr, "  got=%Zd, ref=%Zd\n", got, ref);
		}
		
		
		static_assert(bh_abcd_comp.depth<=2, "Bit heap not compressed.");
		
		fw_uint<WT> abcd_comp=collapse(bh_abcd_comp);
		
		abcd_comp.to_mpz(got);
		if(mpz_cmp(ref, got)){
			std::cerr<<" a="<<a<<", b="<<b<<", c="<<c<<", d="<<d<<" = "<<abcd<<"\n";
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
		
		auto bh_a=fw_uint_to_row_heap(a);
		auto bh_b=fw_uint_to_row_heap(b);
		auto bh_c=fw_uint_to_row_heap(c);
		
		const int WT=thls_ctMax(WC,thls_ctMax(WA,WB));
		
		auto bh_abc=bh_a+bh_b+bh_c;
		
		auto abc=extu<WT>(a)+extu<WT>(b)+extu<WT>(c);
		
		abc.to_mpz(ref);
		row_heap_to_mpz(got, bh_abc);
		
		if(mpz_cmp(ref, got)){
			std::cerr<<" a="<<a<<", b="<<b<<", c="<<c<<" = "<<abc<<"\n";
			mpfr_fprintf(stderr, "  got=%Zd, ref=%Zd\n", got, ref);
		}
		
		auto bh_abc_comp=compress(bh_abc);
		
		row_heap_to_mpz(got, bh_abc_comp);
		if(mpz_cmp(ref, got)){
			std::cerr<<"comp: a="<<a<<", b="<<b<<", c="<<c<<" = "<<abc<<"\n";
			mpfr_fprintf(stderr, "  got=%Zd, ref=%Zd\n", got, ref);
		}
				
		static_assert(bh_abc_comp.depth<=2, "Bit heap not compressed.");
		
		fw_uint<WT> abc_comp=collapse(bh_abc_comp);
		
		abc_comp.to_mpz(got);
		if(mpz_cmp(ref, got)){
			std::cerr<<" a="<<a<<", b="<<b<<", c="<<c<<" = "<<abc<<"\n";
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
		
		auto bh_a=fw_uint_to_row_heap(a);
		auto bh_b=fw_uint_to_row_heap(b);
		
		auto bh_ab=bh_a+bh_b;
		
		auto ab=a+b;
		
		ab.to_mpz(ref);
		row_heap_to_mpz(got, bh_ab);
		
		if(mpz_cmp(ref, got)){
			std::cerr<<" a="<<a<<", b="<<b<<" = "<<ab<<"\n";
			mpfr_fprintf(stderr, "  got=%Zd, ref=%Zd, W(ref)=%d\n", got, ref, ab.width);
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
	mpz_t ref, got;
	mpz_init(ref);
	mpz_init(got);
	
	for(int i=0; i<(1<<W); i++){
		fw_uint<W> tmp(i);
		
		auto bh_a=fw_uint_to_row_heap(tmp);
		
		tmp.to_mpz(ref);
		row_heap_to_mpz(got, bh_a);
		
		if(mpz_cmp(ref, got)){
			std::cerr<<" a="<<"a\n";
			mpfr_fprintf(stderr, "  got=%Zd, ref=%Zd\n", got, ref);
		}
		
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
	
	mpz_clear(ref);
	mpz_clear(got);
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

	test();
	
	fprintf(stderr, "Done\n");


    return 0;
}
	
