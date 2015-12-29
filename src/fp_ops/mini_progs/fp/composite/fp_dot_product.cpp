template<class T>
extern T fp_dot_product(
    int n,
    const T *a,
    const T *b
)
{
    T acc;
    for(int i=0; i<n; i++){
        acc+=a[i]*b[i];
    }
    
    return acc;
}
