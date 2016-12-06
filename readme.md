This provides many things, but the main soft-float
parts are arbitrary precision (in a,b,res) versions of:

- addition
- multiplication
- division
- comparison

They are all designed to turn into reasonably decent
pipelines through agressive templating and inlining.

Support beyond addition and multiplication is weaker,
and things that are not currently supported are int->float
and float->int operations.

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

There is a simple driver in `examples/sum.cpp`. To compile it,
make sure that `include` is on the include path. For example,
from the `examples` directory:

    g++ -std=c++11 -I ../../include/ sum.cpp

By default the back-end uses 8,16, 32, 64, or 128-bit integers
in the compiler, with asserts to inform the compiler about more
detailed widths. There is also an ap_uint backend for use in VHLS synthesis:

    g++ -DTHLS_FW_UINT_ON_AP_UINT=1 -std=c++11 -I ../../include/ sum.cpp
