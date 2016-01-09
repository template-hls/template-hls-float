extern float fp_add_native_nodsp_8_23(
    float a,
    float b
)
{
#pragma HLS INTERFACE ap_ctrl_none register port=return
#pragma HLS PIPELINE
    
    float temp;
    #pragma HLS RESOURCE variable=temp core=FAddSub_nodsp
    temp=a+b;
    return temp;
}
