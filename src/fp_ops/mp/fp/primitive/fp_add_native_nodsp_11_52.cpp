extern double fp_add_native_nodsp_11_52(
    double a,
    double b
)
{
#pragma HLS INTERFACE ap_ctrl_none register port=return
#pragma HLS PIPELINE

    double temp;
    #pragma HLS RESOURCE variable=temp core=DAddSub_nodsp
    temp=a+b;
    return temp;
}
