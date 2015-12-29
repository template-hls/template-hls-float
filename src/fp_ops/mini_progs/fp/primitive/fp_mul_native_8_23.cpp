extern float fp_mul_native_8_23(
    float a,
    float b
)
{
#pragma HLS INTERFACE ap_ctrl_none register port=return
#pragma HLS PIPELINE
    return a*b;
}
