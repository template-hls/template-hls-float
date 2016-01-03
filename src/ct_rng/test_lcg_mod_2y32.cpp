#include "thls/tops/ct/ct_rng_lcg_mod_2y32.hpp"

int main()
{
    thls::ct::verify_ct_rng<thls::ct::make_quick_and_dirty<>::type>::run();
}
