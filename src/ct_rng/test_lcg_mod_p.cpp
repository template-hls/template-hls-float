#include "thls/tops/ct/ct_rng_lcg_mod_p.hpp"

int main()
{
    thls::ct::verify_ct_rng<thls::ct::make_minstdrand0<>::type>::run();
    thls::ct::verify_ct_rng<thls::ct::make_lcg_mod_2y32m5<>::type>::run();
}
