extern double fp_div_native_11_52(
    double a,
    double b
)
{
#pragma HLS INTERFACE ap_ctrl_none register port=return
#pragma HLS PIPELINE
    return a/b;
}
