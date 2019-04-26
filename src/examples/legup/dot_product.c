#include "thls/thls_float.hpp"

const int E=8;
const int F=23;
using fp_t = thls::fp_flopoco<E,F>;

fp_t dot_product(fp_t x0, fp_t x1, fp_t y0, fp_t y1)
{
    return thls::add<E,F>( thls::mul<E,F>(x0, y0), thls::mul<E,F>(x1,y1) );
}

#define x0   (volatile fp_t *)0xf00000000
#define x1 (volatile fp_t *)0xf00000008
#define x2   (volatile fp_t *)0xf0000000C
#define x3   (volatile fp_t *)0xf00000010

int main(int argc, char *argv[])
{

    fp_t tmp=dot_product((const fp_t &)*x0, (const fp_t &)*x1, (const fp_t &)*x2, (const fp_t &)*x3);

    return tmp.is_zero().to_bool();
}
