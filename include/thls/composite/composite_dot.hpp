#ifndef composite_dot_par_hpp
#define composite_dot_par_hpp

namespace thls
{
namespace composite
{


template<unsigned N>
class dot_par_impl
{
    THLS_INLINE T go(const T *x, const T *y)
    { return dot_par_impl<N/2>(x,y) + dot_par_impl<N-N/2>(x+N/2, y+N/2); }
};

template<>
class dot_par_impl<1>
{
    THLS_INLINE T go(const T *x, const T *y)
    { return x[0]*y[0]; }
};

template<unsigned N>
T dot_par(const T *x, const T *y)
{ return dot_par_impl<T>::go(x,y); }


template<unsigned NV=1>
T dot_seq(unsigned n, const T *x, const T *y)
{
    assert( (n%NV) == 0);
    T acc();
    for(unsigned i=0; i<n; i++){
        acc += dot_par<NV>( x + i*NV, y + i*NV );
    }
    return acc;
}

};
};

#endif