#ifndef thls_core_base_ops_hpp
#define thls_core_base_ops_hpp

#include "thls/core/constfunc_helpers.hpp"

template<class A, class B>
struct add_type;

template<>
struct add_type<int64_t,int64_t>
{ typedef int64_t type; };


template<class A, class B>
struct mul_type;

template<>
struct mul_type<int64_t,int64_t>
{ typedef int64_t type; };


//! Returns a type which can hold values from A or B
/*! This is used later to support various other things
	like tables and select/multiplex. */
template<class A,class B>
struct union_type;

template<>
struct union_type<int64_t,int64_t>
{ typedef int64_t type; };


#endif
