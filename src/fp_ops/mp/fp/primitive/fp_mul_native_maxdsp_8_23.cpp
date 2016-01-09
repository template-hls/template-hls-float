extern float fp_mul_native_maxdsp_8_23(
    float a,
    float b
)
{
#pragma HLS INTERFACE ap_ctrl_none register port=return
#pragma HLS PIPELINE
    
    float temp;
    #pragma HLS RESOURCE variable=temp core=FMul_maxdsp 
    temp=a*b;
    return temp;
}
