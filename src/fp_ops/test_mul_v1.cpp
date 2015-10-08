#include "thls/tops/fp_flopoco_mul_v1.hpp"
#include "test_mul_generic.hpp"

int main()
{

    test_impl(mul_v1<8,23,8,23,8,23>);

    return 0;
}
