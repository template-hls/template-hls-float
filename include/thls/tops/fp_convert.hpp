#ifndef thls_fp_convert_hpp
#define thls_fp_convert_hpp

#include "fp_flopoco.hpp"
#include "fp_ieee.hpp"

/*
    This does potentiall lossy conversion between formats. For
    lossless conversions, look at fp_promote.

*/

namespace thls
{

namespace detail
{
    template<int DstE,int SrcE>
    struct promote_exp_impl
    {
        static fw_uint<DstE> go(const fw_uint<SrcE> &e)
        {
            static_assert( DstE >= SrcE, "Cannot promote to narrower exponent" );
            
            static const int SrcBias=(1<<(SrcE-1))-1;
            static const int DstBias=(1<<(DstE-1))-1;
            
            static_assert(DstBias >= SrcBias, "Assumptions violated"); 
            
            return zpad_hi<DstE-SrcE>(e)+(DstBias-SrcBias);
        }
    };
    
    template<int E>
    struct promote_exp_impl<E,E>
    {
        static const fw_uint<E> &go(const fw_uint<E> &e)
        { return e; }
    };
    
    template<int DstF,int SrcF>
    struct promote_frac_impl
    {
        static fw_uint<DstF> go(const fw_uint<SrcF> &f)
        {
            static_assert( DstF >= SrcF, "Cannot promote to narrower fraction.");
            return concat(f, zg<DstF-SrcF>());
        }
    };
    
    template<int F>
    struct promote_frac_impl<F,F>
    {
        static fw_uint<F> go(const fw_uint<F> &f)
        { return f; }
    };
    
    template<int DstE,int DstF,int SrcE,int SrcF>
    struct promote_impl
    {
        static fp_flopoco<DstE,DstF> go(const fp_flopoco<SrcE,SrcF> &src)
        {
            return fp_flopoco<DstE,DstF>(
                concat(
                    src.get_flags(),
                    src.get_sign(),
                    promote_exp_impl<DstE,SrcE>::go(src.get_exp_bits()),
                    promote_frac_impl<DstF,SrcF>::go(src.get_frac_bits())
                )
            );
        }
        
        static fp_ieee<DstE,DstF> go(const fp_ieee<SrcE,SrcF> &src)
        {
            return fp_ieee<DstE,DstF>(
                concat(
                    src.get_sign(),
                    promote_exp_impl<DstE,SrcE>::go(src.get_exp_bits()),
                    promote_frac_impl<DstF,SrcF>::go(src.get_frac_bits())
                )
            );
        }
    };
    
    template<int E, int F>
    struct promote_impl<E,F,E,F>
    {
        static const fp_flopoco<E,F> &go(const fp_flopoco<E,F> &src)
        { return src; }
        
        static const fp_ieee<E,F> &go(const fp_ieee<E,F> &src)
        { return src; }
    };
};

/*! Expand the exponent and/or fraction width. It is an error to attempt
    to narrow either with this function */
template<int DstE,int DstF,int SrcE,int SrcF>
void promote(fp_flopoco<DstE,DstF> &dst, const fp_flopoco<SrcE,SrcF> &src)
{
    dst=detail::promote_impl<DstE,DstF,SrcE,SrcF>::go(src);
}

template<int DstE,int DstF,int SrcE,int SrcF>
void promote(fp_ieee<DstE,DstF> &dst, const fp_ieee<SrcE,SrcF> &src)
{
    dst=detail::promote_impl<DstE,DstF,SrcE,SrcF>::go(src);
}


}; // thls

#endif
