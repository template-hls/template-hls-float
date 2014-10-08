#ifndef horner_hpp
#define horner_hpp

/* Note: the decltype version works fine in recent g++, but vivado doesn't accept it. */

template<class X, class... Cr>
struct horner_type;

template<class X, class C0>
struct horner_type<X,C0>
{ typedef C0 type; };

template<class X,class C0, class... Cr>
struct horner_type<X,C0,Cr...>
{
	typedef typename horner_type<X,Cr...>::type base_type;
	typedef typename mul_type<X,base_type >::type mul_type;
	//typedef decltype(X(0)*base_type(0)) mul_type;
	typedef typename add_type<C0,mul_type>::type type;
	//typedef decltype(C0(0)+mul_type(0)) type;
};

template<class X, class... Cr>
typename horner_type<X,Cr...>::type horner(X x, Cr... cr);

template<class X, class C0>
typename horner_type<X,C0>::type horner(X x, C0 c0)
//auto horner(X x, C0 c0) -> C0
{ return c0; }

template<class X, class C0, class... Cr>
typename horner_type<X,C0,Cr...>::type horner(X x, C0 c0, Cr... cr)
//auto horner(X x, C0 c0, Cr... cr) -> decltype(c0+x*horner(x,cr...))
{ return c0+x*horner(x,cr...); }

#endif
