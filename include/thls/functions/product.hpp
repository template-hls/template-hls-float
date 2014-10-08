#ifndef product_hpp
#define product_hpp

template<class... Xs>
struct product_type;

template<class X0>
struct product_type<X0>
{ typedef X0 type; };

template<class X0,class... Xr>
struct product_type<X0,Xr...>
{
	typedef typename product_type<Xr...>::type base_type;
	typedef typename mul_type<X0,base_type >::type type;
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

#endif
