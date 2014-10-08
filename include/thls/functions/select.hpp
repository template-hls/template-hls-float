


template<typename A, typename B>
typename union_type<A,B>::type select(bool sel, const A &a, const B &b)
{
	typedef typename union_type<A,B>::type res_t;
	return sel ? res_t(a) : res_t(b);
}
