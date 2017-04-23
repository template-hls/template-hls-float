template<class T>
extern void mmm(
    int n,
    const T *a,
    const T *b,
    T *res
)
{
    for(int r=0; r<n; r++){
        for(int c=0; c<n; c++){
            T acc;
            for(int i=0; i<n; i++){
                acc+=a[r*n+i]*b[r*i+c];
            }
            res[r*n+c]=acc;
        }
    }
}
