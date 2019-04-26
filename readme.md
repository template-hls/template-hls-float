This library provides pure C++ implementations of floating-point operators,
but is designed to compile into efficient hardware using HLS tool-chains.

The library is intended to support heterogenous floating-point data-types within
a program, which means:

- Each floating-point variable can have a user-specified exponent and fraction
  width, so you can tightly tune range and precision.

- Binary operators can accept inputs of different floating-point types. This is
  particularly useful for multipliers, as the core integer multiplier can be 
  sized according to the precise widths of each input.

- Output formats can be different to input formats, and can be more or less
  precise. This is useful with multipliers for retaining more accuracy, but
  also for other operators where data-path analysis shows that accuracy needs
  to shrink or grow.

The overall approach is described in an FCCM 2019 paper:
"Templatised soft floating-point for High-Level Synthesis",
David B. Thomas, FCCM, 2019.

Note that this is very much an alpha library, and is lacking in a user-friendly
API and proper documentation. Now that the commercial tools have front-ends that
support this kind of activity, the plan is to evolve the API a bit.

Getting Started
===============

### Platform setup

For all platforms: add the `include` directory to the include search path:
```
-I ${TEMPLATE_HLS_FLOAT_SOURCE}/include
```

- Native C++: no extra setup is needed, it should work out of the box.

- Vivado HLS: you need to select the ap_int backend
  1 - make sure that `ap_int.h` is on the include directory search path (it will be by default in a VHLS project)
  2 - Compile your source files with `-DTHLS_FW_UINT_ON_AP_UINT=1`

- Intel HLS: you need to select the ac_int backend
  1 - make sure that the FPGA optimised `ac_int.hpp` is on the include directory search path
  2 - Compile your source files with `-DTHLS_FW_UINT_ON_AC_UINT=1`

- Legup: there is no specific backend for legup; just compile as if it was native C++.
    However, with the current version of this library and the last open-source version of
    Legup it doesn't compile. On an older/simpler version of the library it did.

### Source code

Bring in the floating-point operators by including `thls/thls_float.hpp`.

Declare a floating-point variable/type using `thls::fp_flopoco<E,F>`, where `E` is the
number of exponent bits and `F` is the number of fractional bits (excluding the implicit bit).
So for example:
- `thls::fp_flopoco<8,23>` ~= `float`
- `thls::fp_flopoco<11,52>` ~= `double`
- `thls::fp_flopoco<5,10>` ~= `half`
- `thls::fp_flopoco<8,7>` ~= `bfloat16`
The flopoco type does not support denormals, so it will under-flow straight to zero
(though this is true of most FPGA floating-point IP cores).

Floating-point operations are performed using function calls rather than
operator overloading. The return-type is specified independently of
the input types:
```
const int E=8;
const int F=23;
thls::fp_flopoco<E,F> mul3(const thls::flopoco<E,F> &a, const thls::flopoco<E,F> &b, const thls::flopoco<E,F> &c)
{
    return thls::mul<E,F>(a, thls::mul<E,F>(b, c));
}
```
or if we wanted to maintain full accuracy:
```
const int E=8;
const int F=23;
thls::fp_flopoco<E,3*F+2> mul3(const thls::flopoco<E,F> &a, const thls::flopoco<E,F> &b, const thls::flopoco<E,F> &c)
{
    return thls::mul<E,3*F+2>(a, thls::mul<E,2*F+1>(b, c));
}
```

### Operators

This provides many things, but the main soft-float
parts are arbitrary precision (in a,b,res) versions of:

- binary addition : `thls::add`
- binary multiplication : `thls::mul`
- binary division : `thls::div`
- binary comparison : `thls::less_than`, ...
- unary negation : `thls::neg` (used to implement subtraction) 
- unary reciprocal : `thls::inv`

They are all designed to turn into reasonably decent
pipelines through aggressive templating and inlining.

A current limitation is that while the operators are all
parametrised in floating-point types, only the multiplier does
it properly (though it is also the one that benefits most).
In particular, the adder does *not* allow for a smaller fraction
width than the inputs, as there were bugs in the original
implementation. However, it does allow for different sized
inputs.

Support beyond addition and multiplication is weaker,
and things that are not currently supported are int->float
and float->int operations.

Policies
========

A "policy" is something that gives a space of numbers,
including the operations on those numbers. Current policies
are:

- native : Just use float or double.

- flopoco : numbers are held in flopoco format, which is faster,
  but two-bits wider than IEEE.

- ieee_on_flopoco : uses flopoco internally, but converts from IEEE
  on input and back to IEEE on output. Not a great idea usually.
  
Each policy type `policy_type` defines a few things:

- `policy_type::value_t` : The type of values encoded in this format.

- `policy_type::to_mpfr` : Converts a value_t to a floating-point number.

- `policy_type::from_mpfr` : Converts a floating-point number to a value_t

An example of using a policy is:

````
#include "thls/tops/policy_flopoco.hpp"

// Create a single-precision flopoco policy called fp_policy_t
typedef thls::policy_flopoco<8,23> fp_policy_t;

// This is the type of floats under this policy
typedef fp_policy_t::value_t fp_t;

// A function that has floats on the outside, but uses policy on the inside
float sum( int n, const float *data ){
    fp_t acc;
    for(int i=0; i<n; i++){
        fp_t fx=thls::to<fp_t>( data[i] ); // Convert a float to the policy float type
        acc += fx;
    }
    return thls::to<float>(acc); // export back out of the policy float type
}
````

