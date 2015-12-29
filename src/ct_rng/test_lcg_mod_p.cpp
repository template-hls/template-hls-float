#include "thls/tops/ct_rng_lcg_mod_p.hpp"

int main()
{
    thls::ct_rng::verify_ct_rng<thls::ct_rng::make_minstdrand0<>::value>::run();
    thls::ct_rng::verify_ct_rng<thls::ct_rng::make_lcg_mod_2y32m5<>::value>::run();
}
