extern float fp_mul_native_fulldsp_8_23(
    float a,
    float b
)
{
#pragma HLS INTERFACE ap_ctrl_none register port=return
#pragma HLS PIPELINE
    
    float temp;
    #pragma HLS RESOURCE variable=temp core=FMul_fulldsp 
    temp=a*b;
    return temp;
}
