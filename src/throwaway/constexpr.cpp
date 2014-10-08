/*
#include <stdint.h>
#include <assert.h>

template<int64_t a,int64_t b>
struct ctMax
{ static const int64_t val =  a>b ? a : b; };

template<int64_t a,int64_t b>
struct ctMin
{ static const int64_t val =  a<=b ? a : b; };

template<int64_t a,int64_t b, int64_t c, int64_t d>
struct ctMax4
{ static const int64_t val =  ctMax<ctMax<a,b>::val,ctMax<c,d>::val>::val; };

template<int64_t a,int64_t b, int64_t c, int64_t d>
struct ctMin4
{ static const int64_t val =  ctMin<ctMin<a,b>::val,ctMin<c,d>::val>::val; };

template<int64_t Tlow, int64_t Thigh>
struct ival
{
	static const int64_t low = Tlow;
	static const int64_t high = Thigh;

	ival(int64_t x)
		: val(x)
	{
		assert(low<=x);
		assert(x<=high);
	}

	int64_t val;

	operator int64_t()
	{ return val; }
};

template<class A, class B>
struct ival_add_type
{ typedef ival<A::low+B::low,A::high+B::high> type; };

template<int64_t lowA, int64_t highA, int64_t lowB, int64_t highB>
ival<lowA+lowB,highA+highB> operator+(const ival<lowA,highA> &a, const ival<lowB,highB> &b)
{
	return a.val+b.val;
}

template<class A,class B>
struct ival_mul_type
{
	typedef ival<
			ctMin4<A::low*B::low,A::low*B::high,A::high*B::low,A::high*B::high>::val,
			ctMax4<A::low*B::low,A::low*B::high,A::high*B::low,A::high*B::high>::val
			> type;
};

template<int64_t lowA, int64_t highA, int64_t lowB, int64_t highB>
ival<
	ctMin4<lowA*lowB,lowA*highB,highA*lowB,highA*highB>::val,
	ctMax4<lowA*lowB,lowA*highB,highA*highB,highA*highB>::val
>
	operator*(const ival<lowA,highA> &a, const ival<lowB,highB> &b)
{
	return a.val*b.val;
}


template<int places, int64_t lowA, int64_t highA>
ival<(lowA>>places),(highA>>places)> shift_right(const ival<lowA,highA> &a)
{
	return a.val>>places;
}

template<class... Xs>
struct product_type;

template<class X0>
struct product_type<X0>
{ typedef X0 type; };

template<class X0,class... Xr>
struct product_type<X0,Xr...>
{
	typedef typename product_type<Xr...>::type base_type;
	typedef typename ival_mul_type<X0,base_type >::type type;
};

template<class... Xs>
typename product_type<Xs...>::type product(Xs ...xs);

template<class X0>
typename product_type<X0>::type product(X0 x0)
{ return x0; }

template<class X0, class... Xr>
typename product_type<X0,Xr...>::type product(X0 x0, Xr ...xr)
{
	return x0 * product(xr...);
}


template<class X, class... Cr>
struct horner_type;

template<class X, class C0>
struct horner_type<X,C0>
{ typedef C0 type; };

template<class X,class C0, class... Cr>
struct horner_type<X,C0,Cr...>
{
	typedef typename horner_type<X,Cr...>::type base_type;
	typedef typename ival_mul_type<X,base_type >::type mul_type;
	typedef typename ival_add_type<C0,mul_type>::type type;
};

template<class X, class... Cr>
typename horner_type<X,Cr...>::type horner(X x, Cr... cr);

template<class X, class C0>
typename horner_type<X,C0>::type horner(X x, C0 c0)
{ return c0; }

template<class X, class C0, class... Cr>
typename horner_type<X,C0,Cr...>::type horner(X x, C0 c0, Cr... cr)
{ return c0+x*horner(x,cr...); }

*/

#include "ival.hpp"
#include "product.hpp"
#include "horner.hpp"

int64_t top(int64_t _x, int64_t _c0, int64_t _c1, int64_t _c2, int64_t _c3)
{
	ival<256,-127> x(_x);
	ival<8,-5> c0(_c0);
	ival<11,-4> c1(_c1);
	ival<8,-3> c2(_c2);
	ival<2,-7> c3(_c3);

	int64_t tmp=horner(_x, _c0, _c1, _c3);
	auto tmp2= horner(x, c0, constant_ival<10>(), c2, c3);
	return tmp2.val+tmp;
}

