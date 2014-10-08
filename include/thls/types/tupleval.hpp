#ifndef thls_types_tupleval_hpp
#define thls_types_tupleval_hpp

#include <tuple>

#include "ival.hpp"

template<typename ...Ta, typename ...Tb>
struct union_type<std::tuple<Ta...>,std::tuple<Tb...> >;

template<typename Ta0, typename Tb0>
struct union_type<std::tuple<Ta0>,std::tuple<Tb0> >
{
	typedef typename union_type<Ta0,Tb0>::type curr_type;
	
	typedef std::tuple<curr_type> type;
};
	
template<typename Ta0, typename ...Tar, typename Tb0, typename ...Tbr>
struct union_type<std::tuple<Ta0,Tar...>,std::tuple<Tb0,Tbr...> >
{
	// Technically this is not true... we could decide that union of nothing with T is T.
	static_assert(sizeof...(Tar)==sizeof...(Tbr), "Can't unify tuples of different sizes.");
	
	typedef typename union_type<Tar...,Tbr...>::type base_type;
	typedef typename union_type<Ta0,Tb0>::type curr_type;
	
	typedef std::tuple<curr_type,base_type> type;
};

template<typename Ta0, typename ...Tar, typename Tb0, typename ...Tbr>
struct add_type<std::tuple<Ta0,Tar...>,std::tuple<Tb0,Tbr...> >
{
	// Technically this is not true... we could decide that union of nothing with T is T.
	static_assert(sizeof...(Tar)==sizeof...(Tbr), "Can't add tuples of different sizes.");

	typedef typename add_type<Tar...,Tbr...>::type base_type;
	typedef typename add_type<Ta0,Tb0>::type curr_type;

	typedef std::tuple<curr_type,base_type> type;
};

/*
 * TODO :How on earth do you do pairwise addition?
 * Umm, I guess do a head and tail helper...
 * Nope, my head asplodes.
 */

#endif
