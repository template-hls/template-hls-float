#ifndef thls_fp_flopoco_add_v5_hpp
#define thls_fp_flopoco_add_v5_hpp

#include "thls/tops/fp_flopoco.hpp"
#include "thls/tops/fp_promote.hpp"

#include "thls/tops/fp_flopoco_add_single_v1.hpp"

namespace thls
{



template<int wER,int wFR, int wEX,int wFX,int wEY,int wFY>
THLS_INLINE fp_flopoco<wER,wFR> add_single_v5(const fp_flopoco<wEX,wFX> &xPre, const fp_flopoco<wEY,wFY> &yPre, int DEBUG=0)
{
    // Allow wFX!=wFY and wER!=wFR, but require that wER=max(wEX,wEY) and wFR=max(wFX,wFY)

    const int wFXYmax = thls_ctMax(wFX,wFY);
    const int wFXYmin = thls_ctMin(wFX,wFY);

    const int wF = thls_ctMax(wFX,wFY);
    const int wE = thls_ctMax(wEX,wEY);

    fp_flopoco<wE,wF> x;
    fp_flopoco<wE,wF> y;
    promote(x, xPre);
    promote(y, yPre);

    THLS_STATIC_ASSERT(wE==wER, "Result exp must match promotion of args.");
    THLS_STATIC_ASSERT(wF==wFR, "Result frac must match promotion of args.");

    // Copyright : This is heavily based on copyright work of Bogdan Pasca and Florent de Dinechin (2010)

    /*if(DEBUG){
        std::cerr<<"  x = "<<x<<"\n";
        std::cerr<<"  y = "<<y<<"\n";
    }*/

    const int sizeRightShift = intlog2<wF+3>::value;

    /* Set up the IO signals */
    /* Inputs: 2b(Exception) + 1b(Sign) + wEX bits (Exponent) + wFX bits(Fraction) */
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //addFPInput ("X", wE, wF);
    //addFPInput ("Y", wE, wF);
    //addFPOutput("R", wE, wF);
    fw_uint<wE+wF+3> X=x.bits;
    fw_uint<wE+wF+3> Y=y.bits;
    fw_uint<wE+wF+3> R;

    //=========================================================================|
    //                          Swap/Difference                                |
    // ========================================================================|
    // -- Exponent difference and swap  --

    // We do the swap based on the original fractions, so if they are very different
    // sizes we get a faster comparator
    // For now we take:
    // - The post-adjusted exponents (will cost if they are not equal) from X and Y
    // - The pre-adjusted fractions from xPre and yPre
    assert(wFX-wFXYmin >= 0);
    assert(wFY-wFXYmin >= 0);
    fw_uint<2+wE+wFXYmin> excExpFracX = concat( get_bits<wE+wF+2, wE+wF+1>(X) , get_bits<wE+wF-1, wF>(X), get_bits<wFX-1,wFX-wFXYmin>(xPre.bits) );
    fw_uint<2+wE+wFXYmin> excExpFracY = concat( get_bits<wE+wF+2, wE+wF+1>(Y) , get_bits<wE+wF-1, wF>(Y), get_bits<wFY-1,wFY-wFXYmin>(yPre.bits) );
    //fw_uint<2+wE+wF> excExpFracX = concat( get_bits<wE+wF+2, wE+wF+1>(X) , get_bits<wE+wF-1,0>(X) );
    //fw_uint<2+wE+wF> excExpFracY = concat( get_bits<wE+wF+2, wE+wF+1>(Y) , get_bits<wE+wF-1,0>(Y) );

    fw_uint<wE+1> eXmeY = zpad_hi<1>(get_bits<wE+wF-1, wF>(X)) - zpad_hi<1>(get_bits< wE+wF-1, wF>(Y));
    fw_uint<wE+1> eYmeX = zpad_hi<1>(get_bits<wE+wF-1, wF>(Y)) - zpad_hi<1>(get_bits< wE+wF-1, wF>(X));

    if(DEBUG){
        std::cerr<<"  eXmeY = "<<eXmeY<<"\n";
        std::cerr<<"  eYmeX = "<<eYmeX<<"\n";
    }

    // NOTE : FloPoCo used logic here that swapped to an adder component
    // for wf>=30. Let's hope (heh) that the HLS tool does something
    // sensible for us.
    // TODO - Does it do anything sensible for large exponent?
    // NOTE: Need to be careful if the fractional widths are not equal, as we compare
    // truncated fractions. If one of the inputs is constant one we actually won't compare
    // anything from the fraction...
    fw_uint<1> swap= wFX < wFY
            ? excExpFracX <= excExpFracY // Should choose Y in the case of a tie
            : excExpFracX < excExpFracY;// Should choose X in the case of a tie
    //fw_uint<1> swap= excExpFracX < excExpFracY;

    if(DEBUG){
        std::cerr<<"  excExpFracX = "<<excExpFracX<<"\n";
        std::cerr<<"  excExpFracY = "<<excExpFracY<<"\n";
    }

    // depending on the value of swap, assign the corresponding values to the newX and newY signals
    auto newX = select(swap==0, X, Y);
    auto newY = select(swap==0, Y, X);
    //break down the signals
    auto expX = get_bits< wE+wF-1, wF>(newX);
    auto excX = get_bits< wE+wF+2, wE+wF+1>(newX);
    auto excY = get_bits< wE+wF+2, wE+wF+1>(newY);
    auto signX = get_bit<wE+wF>(newX);
    auto signY = get_bit<wE+wF>(newY);
    auto EffSub = signX ^ signY;
    fw_uint<6> sXsYExnXY = concat(signX, signY, excX, excY);
    auto fracY = select(excY==0b00, zg<wF+1>(), opad_hi<1>(get_bits<wF-1,0>(newY)));

    if(DEBUG){
        std::cerr<<"  fracY = "<<fracY<<"\n";
    }

    //exception bits: need to be updated but for not FIXME

    static const lut<2,6> excRt_lut([](int i) -> int {
        switch(i){
            case 0b000000: case 0b010000: case 0b100000: case 0b110000:
                return 0b00;
            case 0b000101: case 0b010101: case 0b100101: case 0b110101: case 0b000100: case 0b010100: case 0b100100: case 0b110100: case 0b000001: case 0b010001: case 0b100001: case 0b110001:
                return 0b01;
            case 0b111010: case 0b001010: case 0b001000: case 0b011000: case 0b101000: case 0b111000: case 0b000010: case 0b010010: case 0b100010: case 0b110010: case 0b001001: case 0b011001: case 0b101001: case 0b111001: case 0b000110: case 0b010110: case 0b100110: case 0b110110:
                return 0b10;
            default:
                return 0b11;
        }
    });
    auto excRt=excRt_lut(sXsYExnXY);

    if(DEBUG){
        std::cerr<<"   sXsYExnXY = "<<sXsYExnXY<<"\n";
        std::cerr<<"   excRt = "<<excRt<<"\n";
    }

    auto signR = select(sXsYExnXY==0b100000 || sXsYExnXY==0b010000, zg<1>(), signX);
    if(DEBUG){
        std::cerr<<"  signR = "<<signR<<"\n";
    }

    auto expDiff = select(swap==0, eXmeY, eYmeX);

    if(DEBUG){
        std::cerr<<"  expDiff = "<<expDiff<<"\n";
    }

    auto shiftedOut = expDiff >= (wF+2);

    //shiftVal=the number of positions that fracY must be shifted to the right
    fw_uint<sizeRightShift> shiftVal;

    if (wE>sizeRightShift) {
        shiftVal = checked_cast<sizeRightShift>(  select(shiftedOut==0, get_bits< sizeRightShift-1, 0>(expDiff), fw_uint<sizeRightShift>(wF+3)) );
    }
    else if (wE==sizeRightShift) {
        shiftVal = checked_cast<sizeRightShift>( get_bits<sizeRightShift-1,0>(expDiff) );
    }
    else 	{ //  wE< sizeRightShift
        shiftVal = checked_cast<sizeRightShift>( concat( zg<sizeRightShift-wE>(), expDiff) );

    }
    if(DEBUG){
        std::cerr<<"  shiftVal = "<<shiftVal<<"\n";
    }


    // shift right the significand of new Y with as many positions as the exponent difference suggests (alignment)
    // Building far path right shifter
    //sticky compuation in parallel with addition, no need for manageCriticalPath
    //compute sticky bit as the or of the shifted out bits during the alignment //

    // The shifter parameters are:
    //   wIn = wF+1
    //   maxShift = wF+3
    // which means;
    //   wOut = wIn+maxShift = wF+wF+4
    auto preFracY=zpad_lo<wF+3>(fracY);
    if(DEBUG){
        std::cerr<<"  preFracY = "<<preFracY<<"\n";
    }
    fw_uint<2*wF+4> shiftedFracY = preFracY >> shiftVal.to_int();
    fw_uint<1> sticky = get_bits<wF,0>(shiftedFracY)!=0;


    //pad fraction of Y [overflow][shifted frac having inplicit 1][guard][round]
    auto fracYfar = zpad_hi<1>(get_bits<2*wF+3,wF+1>(shiftedFracY));
    auto EffSubVector = select(EffSub, og<wF+4>(), zg<wF+4>());
    fw_uint<wF+4> fracYfarXorOp = fracYfar ^ EffSubVector;
    //pad fraction of X [overflow][inplicit 1][fracX][guard bits]
    fw_uint<wF+4> fracXfar = concat( fw_uint<2>(0b01), get_bits<wF-1,0>(newX), zg<2>() );

    auto cInAddFar = EffSub & ~sticky;

    if(DEBUG){
        std::cerr<<"  fracYfar = "<<fracYfar<<"\n";
        std::cerr<<"  fracYfarXorOp = "<<fracYfarXorOp<<"\n";
        std::cerr<<"  fracXfar = "<<fracXfar<<"\n";
        std::cerr<<"  cInAddFar = "<<cInAddFar<<"\n";
    }

    //result is always positive.
    fw_uint<wF+4> fracAddResult = add_with_cin(fracXfar,fracYfarXorOp,cInAddFar);
    if(DEBUG){
        std::cerr<<"  fracAddResult = "<<fracAddResult<<"\n";
    }

    //shift in place
    fw_uint<wF+5> fracGRS = concat(fracAddResult, sticky);

    //incremented exponent.
    fw_uint<wE+2> extendedExpInc = zpad_hi<2>(expX)+1;
    if(DEBUG){
        std::cerr<<"  extendedExpInc = "<<extendedExpInc<<"\n";
    }

    // LZOCShifterSticky
    //   wIn = wF+5
    //   wOut = wF+5
    //   wCount = intlog2(wF+5)
    //   computeSticky = false
    //   countType = 0  -> "spec" (rather than "gen")

    const int lzocCountWidth=intlog2<wF+5>::value;

    fw_uint<lzocCountWidth> nZerosNew;
    fw_uint<wF+5> shiftedFrac;
    LZOCShifter<wF+5>(shiftedFrac, nZerosNew, fracGRS);

    if(DEBUG){
        std::cerr<<"  shiftedFrac = "<<shiftedFrac<<"\n";
        std::cerr<<"  nZerosNew = "<<nZerosNew<<"\n";
    }

    //need to decide how much to add to the exponent
    //update exponent
    auto updatedExp = extendedExpInc - zpad_hi<wE+2-lzocCountWidth>(nZerosNew);
    auto eqdiffsign = nZerosNew==og<lzocCountWidth>();

    if(DEBUG){
        std::cerr<<"  updatedExp = "<<updatedExp<<"\n";
        std::cerr<<"  eqdiffsign = "<<eqdiffsign<<"\n";
    }

    //concatenate exponent with fraction to absorb the possible carry out
    auto expFrac = concat(updatedExp,get_bits<wF+3,3>(shiftedFrac));


    // 		//at least in parallel with previous 2 statements
    auto stk = get_bit<1>(shiftedFrac) | get_bit<0>(shiftedFrac);
    auto rnd = get_bit<2>(shiftedFrac);
    auto grd = get_bit<3>(shiftedFrac);
    auto lsb = get_bit<4>(shiftedFrac);

    if(DEBUG){
        std::cerr<<"  stk = "<<stk<<"\n";
        std::cerr<<"  rnd = "<<rnd<<"\n";
        std::cerr<<"  grd = "<<grd<<"\n";
        std::cerr<<"  lsb = "<<lsb<<"\n";
    }

    //decide what to add to the guard bit
    auto addToRoundBit = select(
        lsb==0 && grd==1 && rnd==0 && stk==0, zg<1>(),
        og<1>());
    //round
    auto RoundedExpFrac = add_cin(expFrac, addToRoundBit);
    if(DEBUG){
        std::cerr<<"  addToRoundBit = "<<addToRoundBit<<"\n";
        std::cerr<<"  expFrac = "<<expFrac<<"\n";
        std::cerr<<"  RoundedExpFrac = "<<RoundedExpFrac<<"\n";
    }

    //possible update to exception bits
    auto upExc = get_bits<wE+wF+2,wE+wF+1>(RoundedExpFrac);

    auto fracR = get_bits<wF,1>(RoundedExpFrac);
    auto expR = get_bits<wF+wE,wF+1>(RoundedExpFrac);

    auto exExpExc = concat(upExc, excRt);

    static const lut<2,4> excRt2_lut([](int i) -> int {
        switch(i){
            case 0b0000: case 0b0100: case 0b1000: case 0b1100: case 0b1001: case 0b1101:
                return 0b00;
            case 0b0001:
                return 0b01;
            case 0b0010: case 0b0110: case 0b1010: case 0b1110: case 0b0101:
                return 0b10;
            default:
                return 0b11;
        }
    });
    fw_uint<2> excRt2 = excRt2_lut(exExpExc);

    if(DEBUG){
        std::cerr<<"  exExpExc = "<<exExpExc<<"\n";
        std::cerr<<"  excRt2 = "<<excRt2<<"\n";
    }


    // TODO
    // TODO - Is there a bug in FloPoCo or here? Current behaviour suggests that (+infy)+(-infty) = 0
    // TODO
    // TODO - I think it should be NaN
    // TODO
    auto excR = select(eqdiffsign==1 && EffSub==1 && excRt2==0b01, zg<2>(), excRt2);

    // IEEE standard says in 6.3: if exact sum is zero, it should be +zero in RN
    auto signR2 = select(eqdiffsign==1 && EffSub==1 && excR==0b00, zg<1>(), signR);

    // assign result
    auto computedR = concat(excR,signR2,expR,fracR);
    R=computedR;

    return fp_flopoco<wER,wFR>(R);
}



}; // thls

#endif
