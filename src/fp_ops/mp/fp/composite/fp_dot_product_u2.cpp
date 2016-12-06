template<class T>
extern T fp_dot_product_u2(
    int n,
    const T *a,
    const T *b
)
{
    assert(n%2==0);
    
    T acc=0;
    T locAcc;
    for(int i=0; i<n; i+=2){
        acc = acc + (a[i]*b[i]) + (a[i+1]*b[i+1]);
    }
    
    return acc;
}
