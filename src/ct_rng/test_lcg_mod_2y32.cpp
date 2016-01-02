#include "thls/tops/ct/ct_rng_lcg_mod_2y32.hpp"

int main()
{
    thls::ct_rng::verify_ct_rng<thls::ct_rng::make_quick_and_dirty<>::type>::run();
}
