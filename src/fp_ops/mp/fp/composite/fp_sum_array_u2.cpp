template<class T>
extern T fp_sum_array_u2(
    int n,
    const T *x
)
{
    assert(n%2==0);
    
    T acc=0;
    for(int i=0; i<n; i++2){
        acc += x[i] + x[i+1];
    }
    
    return acc;
}
