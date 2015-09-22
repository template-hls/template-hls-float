#ifndef ct_bigint_itf_hpp
#define ct_bigint_itf_hpp

#include <cstdio>
#include <cstdint>

#include <boost/multiprecision/cpp_int.hpp>

namespace thls
{
    
///////////////////////////////////////////
// Interact with run-time
    
typedef boost::multiprecision::cpp_int rt_int_t;

//! Convert it into the equivalent run-time type
template<class T>
rt_int_t ct_int_to_rt();

//! Print the number as hexadecimal, with preceeding sign as necessary
template<class T>
void ct_print(FILE *dst);
   
///////////////////////////////////////////
// Create integers in various ways
    
//! Has a single member called `type` which is the compile-time value
template<int x>
struct ct_int_create_from_int;

//! Create positve mask containing `bits` ones
template<unsigned bits>
struct ct_int_create_mask;

//! Create 2^bits
template<unsigned bits>
struct ct_int_create_2ui;

//////////////////////////////////////////////
// Comparisons and properties

/* These all define a boolean called `value` holding the result. */

template<class TA>
class ct_is_negative;

template<class TA>
class ct_is_positive;

template<class TA>
class ct_is_zero;

template<class TA,class TB>
class ct_less_than;

template<class TA,class TB>
class ct_greater_than;

template<class TA,class TB>
class ct_equals;


/////////////////////////////////////////////
// Calculations and manipulations

/* All the compile-time operators produce their
    result as a type called `type`
*/

template<class TA,class TB>
class ct_add;

template<class TA,class TB>
class ct_sub;

//! return negative, if zero then return zero
template<class TA>
class ct_negate;

template<class TA>
class ct_abs;

template<class TA,class TB>
class ct_mul;

//! Pads low with zero bits
template<class TA,unsigned places>
struct ct_shift_left;

//! Maintains sign, unless value underflows to zero
template<class TA,unsigned places>
struct ct_shift_right;

// Need to thing about negative numbers
/*
template<class TA,class TB>
struct ct_and;

template<class TA,class TB>
struct ct_or;

template<class TA>
struct ct_not;
*/

}; // thls


#endif
