#ifndef thls_fp_flopoco_fma_hpp
#define thls_fp_flopoco_fma_hpp

#include "thls/tops/fp_flopoco.hpp"

#include "thls/tops/fp_flopoco_add_dual_v1.hpp"
#include "thls/tops/fp_flopoco_mul_v1.hpp"
#include "thls/tops/fp_convert.hpp"

namespace thls
{

/*
BUG: This current implement has double-rounding, and doesn't quite
guarantee the fma contract - it has more precision, but can round
to the wrong result.
*/
template<int wER,int wFR, int wEX,int wFX,int wEY,int wFY,int wEZ,int wFZ>
THLS_INLINE fp_flopoco<wER,wFR> fma(const fp_flopoco<wEX,wFX> &x, const fp_flopoco<wEY,wFY> &y, const fp_flopoco<wEZ,wFZ> &z, int DEBUG=0)
{
    // TODO : The extra exponent bit is to stop overflow to infinity in some test cases,
    // as I'm too stupid to understand how this should work.
    const int wEXY=(wEX > wEY ? wEX : wEY)+2;
    fp_flopoco<wEXY,wFX> xx;
    fp_flopoco<wEXY,wFY> yy;
    convert(xx,x);
    convert(yy,y);


    const int wFXY=wFX+wFY+2;
    auto xy=mul<wEXY,wFXY>(xx,yy, DEBUG);

    const int wEXYZ=wEXY > wEZ ? wEXY : (wEZ+1);
    const int wFXYZ=thls_ctMax(wFXY,wFZ); // HACK!

    fp_flopoco<wEXYZ,wFXYZ> xyxy;
    fp_flopoco<wEXYZ,wFXYZ> zz;

    convert(xyxy, xy);
    convert(zz, z);

    // TODO : This is a travesty.
    // ERROR : Double-rounding!!!!
    auto xyz=add_fast<wEXYZ,wFXYZ>(xyxy, zz, DEBUG);

    fp_flopoco<wER,wFR> res;
    convert(res, xyz);
    //std::cerr<<"xyz = "<<xyz.str()<<"\n";
    //std::cerr<<"res = "<<res.str()<<"\n";


    /*fp_flopoco<wER,wFR> res2;
    ref_convert(res2, xyz);
    if(!res2.equals(res).to_bool()){
        std::cerr<<xyz.str()<<"\n";
        std::cerr<<res.str()<<"\n";
        std::cerr<<res2.str()<<"\n";
        convert(res, xyz);
        assert(0);
    }*/



    return res;
}

template<int wER,int wFR, int wEX,int wFX,int wEY,int wFY,int wEZ,int wFZ>
THLS_INLINE fp_flopoco<wER,wFR> flopoco_fma(const fp_flopoco<wEX,wFX> &x, const fp_flopoco<wEY,wFY> &y, const fp_flopoco<wEZ,wFZ> &z, int DEBUG=0)
{
    return fma<wER,wFR>(x,y,z);
}



}; // thls

#endif
