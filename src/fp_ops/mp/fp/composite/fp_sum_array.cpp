template<class T>
extern T fp_sum_array_native_8_23(
    int n,
    const T *x
)
{
    T acc;
    for(int i=0; i<n; i++){
        acc+=x[i];
    }
    
    return acc;
}
