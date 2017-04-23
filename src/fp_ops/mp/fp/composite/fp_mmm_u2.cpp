template<class T>
extern void mmm(
    int n,
    const T *a,
    const T *b,
    T *res
)
{
    assert(n%2==0);
    
    for(int r=0; r<n; r++){
        for(int c=0; c<n; c++){
            T acc=0;
            for(int i=0; i<n; i+=2){
                acc+= a[r*n+i]*b[r*i+c] + a[r*n+i+1]*b[r*(i+1)+c];
            }
            res[r*n+c]=acc;
        }
    }
}
