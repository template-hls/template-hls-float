#include "thls/tops/policy_ieee_on_flopoco.hpp"

using namespace thls;

static const int wE = EEE;
static const int wF = FFF;

typedef policy_ieee_on_flopoco<wE,wF> policy_t;

extern policy_t::value_t fp_div_ieee_on_flopoco_EEE_FFF(
        policy_t::value_t a,
        policy_t::value_t b
)
{
#pragma HLS INTERFACE ap_ctrl_none register port=return
#pragma HLS PIPELINE
    return a/b;
}
