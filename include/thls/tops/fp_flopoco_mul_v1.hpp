#ifndef thls_fp_flopoco_mul_v1_hpp
#define thls_fp_flopoco_mul_v1_hpp

#include "thls/tops/fp_flopoco.hpp"

namespace thls
{

template<int wER,int wFR, int wEX,int wFX,int wEY,int wFY>
THLS_INLINE fp_flopoco<wER,wFR> mul(const fp_flopoco<wEX,wFX> &x, const fp_flopoco<wEY,wFY> &y, int DEBUG=0)
{

    fw_uint<wEX+wFY+3> X=x.bits;
    fw_uint<wEX+wFY+3> Y=y.bits;

    fw_uint<1> sign= get_bit<wEX+wFX>(X) ^ get_bit<wEY+wFY>(Y);

    /* Exponent Handling */
    fw_uint<wEX> expX=get_bits<wEX+wFX-1,wFX>(X);
    fw_uint<wEY> expY=get_bits<wEY+wFY-1,wFY>(Y);

    //Add exponents and substract bias
	fw_uint<wEX+2> expSumPreSub= zpad_hi<2>(expX) + zpad_hi<2>(expY);
    fw_uint<wEX+2> bias( (1<<(wER-1))-1 );

    if(DEBUG){
    std::cerr<<"expSumPreSub="<<expSumPreSub<<"="<<expSumPreSub.to_int()<<", bias="<<bias<<"="<<bias.to_int()<<"\n";
    }

    fw_uint<wEX+2> expSum=expSumPreSub - bias;

    /* Significand Handling */
    fw_uint<1+wFX> sigX= opad_hi<1>( get_bits<wFX-1,0>(X) );
    fw_uint<1+wFY> sigY= opad_hi<1>( get_bits<wFY-1,0>(Y) );


    const int sigProdSize=wFX+1 + wFY+1;
    fw_uint<sigProdSize> sigProd=full_mul(sigX,sigY);

    if(DEBUG){
    assert(wFX+wFY<53); // Need to go to arbitrary precision to print
    std::cerr<<"  sigX="<<sigX<<"="<<sigX.to_uint64()<<" = "<<ldexp((double)sigX.to_uint64(),-wFX-1)<<"\n";
    std::cerr<<"  sigY="<<sigY<<"="<<sigY.to_uint64()<<" = "<<ldexp((double)sigY.to_uint64(),-wFY-1)<<"\n";
    std::cerr<<"  sigProd="<<sigProd<<"="<<sigProd.to_uint64()<<" = "<<ldexp((double)sigProd.to_uint64(),-wFX-wFY-2)<<"\n";
    }

    /* Exception Handling, assumed to be faster than both exponent and significand computations */
    fw_uint<4> excSel=concat(get_bits<wEX+wFX+2,wEX+wFX+1>(X) , get_bits<wEY+wFY+2,wEY+wFY+1>(Y));

    if(DEBUG){
    std::cerr<<"excX="<<get_bits<wEX+wFX+2,wEX+wFX+1>(X)<<", excY="<<get_bits<wEY+wFY+2,wEY+wFY+1>(Y)<<"\n";
    }


    fw_uint<2> exc;
    if( (excSel==0b0000 || excSel==0b0001 || excSel==0b0100).to_bool() ){
        exc=fw_uint<2>(0b00);
    }else if( (excSel==0b0101).to_bool() ){
        exc=fw_uint<2>(0b01);
    }else if( (excSel==0b0110 || excSel==0b1001 || excSel==0b1010).to_bool()){
        exc=fw_uint<2>(0b10);
    }else{
        exc=fw_uint<2>(0b11);
    }

    if(DEBUG){
    std::cerr<<"  excSel="<<excSel<<", exc="<<exc<<"\n";
    }

	fw_uint<1> norm=get_bit<sigProdSize-1>(sigProd);
    if(DEBUG){
    std::cerr<<"  norm="<<norm<<"\n";
    }

    // exponent update
    fw_uint<wEX+2> expPostNorm= expSum + zpad_hi<wEX+1>(norm);

    if(DEBUG){
    std::cerr<<"  expPostNorm="<<expPostNorm<<" = "<<expPostNorm.to_int()<<"\n";
    }

	//  exponent update is in parallel to the mantissa shift, so get back there
    fw_uint<3+wER+wFR> R;

    
    if (1+wFR >= wFX+wFY+2) {
        /* => no rounding needed - possible padding;
           in this case correctlyRounded_ is irrelevant: result is exact  */
        fw_uint<wFR> resSig = select(norm,
           concat(get_bits<wFX+wFY,0>(sigProd) , zg<1+wFR-(wFX+wFY+2)>()),
           concat(get_bits<wFX+wFY-1,0>(sigProd) , zg<1+wFR-(wFX+wFY+2)+1>())
        );

        fw_uint<2> expPostNormB = get_bits<wER+1,wER>(expPostNorm);
        fw_uint<2> excPostNorm = select(
            expPostNormB==0b00, fw_uint<2>(0b01),
            expPostNormB==0b01, fw_uint<2>(0b10),
            (expPostNormB==0b11) || (expPostNormB==0b10) , fw_uint<2>(0b11),
            fw_uint<2>(0b11)// Impossible last case?
        ); 

        fw_uint<2> finalExc=select(exc==0b11||exc==0b10||exc==0b00,  exc , excPostNorm);

        R = concat(finalExc,sign, get_bits<wER-1,0>(expPostNorm), resSig);
    }
    else{
        // significand normalization shift
        fw_uint<sigProdSize> sigProdExt = select(norm==1 , concat(get_bits<sigProdSize-2,0>(sigProd) , zg<1>())
                                                         , concat(get_bits<sigProdSize-3,0>(sigProd) , zg<2>()) );

        fw_uint<2+wER+wFR> expSig = concat(expPostNorm , get_bits<sigProdSize-1,sigProdSize-wFR>(sigProdExt));

        fw_uint<1> sticky = get_bit<wFX+wFY+1-wFR>(sigProdExt);

        fw_uint<1> guard = select( get_bits<wFX+wFY+1-wFR-1,0>(sigProdExt) == zg<wFX+wFY+1-wFR-1+1>() , fw_uint<1>(0) , fw_uint<1>(1) );

        fw_uint<1> round = sticky & ( (guard & ~get_bit<wFX+wFY+1-wFR+1>(sigProdExt))
            | (get_bit<wFX+wFY + 1 - wFR+1>(sigProdExt) ));

        if(DEBUG){
        std::cerr<<"  round="<<round<<"\n";
        }

        fw_uint<2+wER+wFR> expSigPostRound = expSig + zpad_hi<2+wER+wFR-1>(round);

        fw_uint<2> expSigPostRoundB=get_bits<wER+wFR+1,wER+wFR>(expSigPostRound);

        fw_uint<2> excPostNorm = select(expSigPostRoundB==0b00, fw_uint<2>(0b01) ,
                                 expSigPostRoundB==0b01, fw_uint<2>(0b10) ,
                                 (expSigPostRoundB==0b11||expSigPostRoundB==0b10),  fw_uint<2>(0b00) ,
                                 fw_uint<2>(0b11));

        fw_uint<2> finalExc=select (exc==0b11||exc==0b10||exc==0b00,  exc , excPostNorm);

        R = concat(finalExc,sign, get_bits<wER+wFR-1,0>(expSigPostRound));
    }

    return fp_flopoco<wER,wFR>(R);
}

}; // thls

#endif
