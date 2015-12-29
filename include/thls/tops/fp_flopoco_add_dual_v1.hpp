#ifndef thls_fp_flopoco_add_dual_v1_hpp
#define thls_fp_flopoco_add_dual_v1_hpp

#include "thls/tops/fp_flopoco.hpp"
#include "thls/tops/fp_promote.hpp"

#include "thls/tops/fp_flopoco_add_single_v1.hpp"

namespace thls
{

template<int wER,int wFR, int wEX,int wFX,int wEY,int wFY>
THLS_INLINE fp_flopoco<wER,wFR> add_dual(const fp_flopoco<wEX,wFX> &xPre, const fp_flopoco<wEY,wFY> &yPre, int DEBUG=0)
{
    
    // Allow wFX!=wFY and wER!=wFR, but require that wER=max(wEX,wEY) and wFR=max(wFX,wFY)
    
    const int wF = thls_ctMax(wFX,wFY);
    const int wE = thls_ctMax(wEX,wEY);
    
    fp_flopoco<wE,wF> x;
    fp_flopoco<wE,wF> y;
    promote(x, xPre);
    promote(y, yPre);
    
    THLS_STATIC_ASSERT(wE==wER, "Result exp must match promotion of args.");
    THLS_STATIC_ASSERT(wF==wFR, "Result frac must match promotion of args.");
    
    // Copyright : This is heavily based on copyright work of Bogdan Pasca and Florent de Dinechin (2008)
    
    bool sub=false; // We are always an adder (for now)
    (void)sub;

    static const int sizeRightShift = intlog2<wF+3>::value;

    /* Set up the IO signals */
    /* Inputs: 2b(Exception) + 1b(Sign) + wE bits (Exponent) + wF bits(Fraction) */
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /*
    addFPInput ("X", wE, wF);
    addFPInput ("Y", wE, wF);
    addFPOutput("R", wE, wF);
    */

    //=========================================================================|
    //                          Swap/Difference                                |
    // ========================================================================|

    //vhdl<<"-- Exponent difference and swap  --"<<endl;
    //vhdl<<tab<<declare("inX",wE+wF+3) << " <= X;"<<endl;
    //vhdl<<tab<<declare("inY",wE+wF+3) << " <= Y;"<<endl;
    auto inX=x.bits;
    auto inY=y.bits;

    // signal which indicates whether or not the exception bits of X are greater or equal than/to the exception bits of Y
    //vhdl<<tab<<declare("exceptionXSuperiorY") << " <= '1' when inX("<<wE+wF+2<<" downto "<<wE+wF+1<<") >= inY("<<wE+wF+2<<" downto "<<wE+wF+1<<") else '0';"<<endl;
    auto exceptionXSuperiorY = get_bits<wE+wF+2,wE+wF+1>(inX) >= get_bits<wE+wF+2,wE+wF+1>(inY);

    // signal which indicates whether or not the exception bits of X are equal to the exception bits of Y
    //vhdl<<tab<<declare("exceptionXEqualY") << " <= '1' when inX("<<wE+wF+2<<" downto "<<wE+wF+1<<") = inY("<<wE+wF+2<<" downto "<<wE+wF+1<<") else '0';"<<endl;
    auto exceptionXEqualY=get_bits<wE+wF+2,wE+wF+1>(inX) == get_bits<wE+wF+2,wE+wF+1>(inY);

    // make the difference between the exponents of X and Y; expX - expY = expX + not(expY) + 1
    // pad exponents with sign bit
    //vhdl<<tab<<declare("signedExponentX",wE+1) << " <= \"0\" & inX("<<wE+wF-1<<" downto "<<wF<<");"<<endl;
    auto signedExponentX = zpad_hi<1>( get_bits<wE+wF-1,wF>(inX) );
    //vhdl<<tab<<declare("signedExponentY",wE+1) << " <= \"0\" & inY("<<wE+wF-1<<" downto "<<wF<<");"<<endl;
    auto signedExponentY = zpad_hi<1>( get_bits<wE+wF-1,wF>(inY) );
    //vhdl<<tab<<declare("exponentDifferenceXY",wE+1) << " <= signedExponentX - signedExponentY ;"<<endl;
    auto exponentDifferenceXY = signedExponentX - signedExponentY;
    //vhdl<<tab<<declare("exponentDifferenceYX",wE) << " <= signedExponentY("<<wE-1<<" downto 0) - signedExponentX("<<wE-1<<" downto 0);"<<endl;
    auto exponentDifferenceYX = drop_msb(signedExponentY) - drop_msb(signedExponentX);

    // SWAP when: [excX=excY and expY>expX] or [excY>excX]
    //vhdl<<tab<<declare("swap") << " <= (exceptionXEqualY and exponentDifferenceXY("<<wE<<")) or (not(exceptionXSuperiorY));"<<endl;
    auto swap = (exceptionXEqualY && get_bit<wE>(exponentDifferenceXY)) || ~(exceptionXSuperiorY);
    
    if(DEBUG){
        std::cerr<<"  swap = "<<swap<<"\n";
    }

    /*string pmY="inY";
    if ( sub ) {
        vhdl << tab << declare("mY",wE+wF+3)   << " <= inY" << range(wE+wF+2,wE+wF+1) << " & not(inY"<<of(wE+wF)<<") & inY" << range(wE+wF-1,0) << ";"<<endl;
        pmY = "mY";
    }
    */
    assert(!sub);
    auto pmY = inY;

    // depending on the value of swap, assign the corresponding values to the newX and newY signals
    //vhdl<<tab<<declare("newX",wE+wF+3) << " <= " << pmY << " when swap = '1' else inX;"<<endl;
    auto newX = select(swap, pmY, inX);
    //vhdl<<tab<<declare("newY",wE+wF+3) << " <= inX when swap = '1' else " << pmY << ";"<<endl;
    auto newY = select(swap, inX, pmY);
    
    if(DEBUG){
        std::cerr<<"  newX = "<<newX<<"\n";
        std::cerr<<"  newY = "<<newY<<"\n";
    }
    
    /*vhdl<<tab<<declare("exponentDifference",wE) << " <= " << "exponentDifferenceYX"
         << " when swap = '1' else exponentDifferenceXY("<<wE-1<<" downto 0);"<<endl;*/
    auto exponentDifference = select(swap, take_lsbs<wE>(exponentDifferenceYX), take_lsbs<wE>(exponentDifferenceXY));
    
    if(DEBUG){
        std::cerr<<"  exponentDifference = "<<exponentDifference<<"\n";
    }

    // determine if the fractional part of Y was shifted out of the operation //
    //vhdl<<tab<<declare("shiftedOut") << " <= ";
    fw_uint<1> shiftedOut;
    if (wE>sizeRightShift){
        /*
        for (int i=wE-1;i>=sizeRightShift;i--)
            if (i==sizeRightShift)
                vhdl<< "exponentDifference("<<i<<")";
            else
                vhdl<< "exponentDifference("<<i<<") or ";
        vhdl<<";"<<endl;
        */
        // expDiff(wE-1) | .. | expDiff(sizeRightShift)
        shiftedOut = get_bits<wE-1,sizeRightShift>(exponentDifference) != 0;
    }else{
        //vhdl<<tab<<"'0';"<<endl;
        shiftedOut = zg<1>();
    }

    //shiftVal=the number of positions that fracY must be shifted to the right
    //vhdl<<tab<<declare("shiftVal",sizeRightShift) << " <= " ;
    fw_uint<sizeRightShift> shiftVal;
    if (wE>sizeRightShift) {
        /*vhdl << "exponentDifference("<< sizeRightShift-1<<" downto 0)"
              << " when shiftedOut='0'"<<endl
              <<tab << tab << "    else CONV_STD_LOGIC_VECTOR("<<wF+3<<","<<sizeRightShift<<") ;" << endl;*/
        shiftVal = checked_cast<sizeRightShift>(
            select(~shiftedOut, take_lsbs<sizeRightShift>(exponentDifference), fw_uint<sizeRightShift>(wF+3))
        );
    }
    else if (wE==sizeRightShift) {
        //vhdl<<tab<<"exponentDifference;" << endl ;
        shiftVal = checked_cast<sizeRightShift>( exponentDifference );
    }
    else 	{ //  wE< sizeRightShift
        //vhdl<<tab<<"CONV_STD_LOGIC_VECTOR(0,"<<sizeRightShift-wE <<") & exponentDifference;" <<	endl;
        shiftVal = checked_cast<sizeRightShift>( concat(zg<sizeRightShift-wE>() , exponentDifference ) );
    }

    // compute EffSub as (signA xor signB) at cycle 1
    //vhdl<<tab<<declare("EffSub") << " <= newX("<<wE+wF<<") xor newY("<<wE+wF<<");"<<endl;
    auto EffSub = get_bit<wE+wF>(newX) ^ get_bit<wE+wF>(newY);
    if(DEBUG){
        std::cerr<<"  EffSub = "<<EffSub<<"\n";
    }

    // compute the close/far path selection signal at cycle1
    // the close path is considered only when (signA!=signB) and |exponentDifference|<=1
    //vhdl<<tab<<declare("selectClosePath") << " <= EffSub when exponentDifference("<<wE-1<<" downto "<<1<<") = ("<<wE-1<<" downto "<<1<<" => '0') else '0';"<<endl;
    auto selectClosePath = select(
        get_bits<wE-1,1>(exponentDifference)==zg<wE-1>(),
        EffSub,
        zg<1>()
    );
    
    //int DEBUG_close = DEBUG && selectClosePath.to_bool();
    int DEBUG_far = DEBUG && !selectClosePath.to_bool();


    // sdExnXY is a concatenation of the exception bits of X and Y, after swap, so exnX > exnY
    /*vhdl<<tab<<declare("sdExnXY",4) << " <= newX("<<wE+wF+2<<" downto "<<wE+wF+1<<") "
         << "& newY("<<wE+wF+2<<" downto "<<wE+wF+1<<");"<<endl;*/
    auto sdExnXY = concat( get_bits<wE+wF+2,wE+wF+1>(newX), get_bits<wE+wF+2,wE+wF+1>(newY) );
    //vhdl<<tab<<declare("pipeSignY") << " <= newY("<<wE+wF<<");"<<endl;
    auto pipeSignY = get_bit<wE+wF>(newY);

    fw_uint<wE+1+wF+1> resultBeforeRoundClose;
    fw_uint<1> roundClose;
    fw_uint<1> resultCloseIsZero;
    fw_uint<1> resSign;
    
    if(1){
        //=========================================================================|
        //                            close path                                   |
        //=========================================================================|

        //vhdl<< endl << "-- Close Path --" << endl;

        // build the fraction signals
        // padding: [sign bit][inplicit "1"][fracX][guard bit]
        //vhdl<<tab<<declare("fracXClose1",wF+3) << " <= \"01\" & newX("<<wF-1<<" downto "<<0<<") & '0';"<<endl;
        auto fracXClose1 = concat( cg<2>(0b01), take_lsbs<wF>(newX), zg<1>() );

        // the close path is considered when the |exponentDifference|<=1, so
        // the alignment of fracY is of at most 1 position
        /*vhdl<<tab<<"with exponentDifference(0) select"<<endl;
        vhdl<<tab<<declare("fracYClose1",wF+3) << " <=  \"01\" & newY("<<wF-1<<" downto "<<0<<") & '0' when '0',"<<endl;
        vhdl<<tab<<"               \"001\" & newY("<<wF-1<<" downto "<<0<<")       when others;"<<endl;*/
        auto fracYClose1 = select(
            take_lsb(exponentDifference),
            concat( cg<3>(0b001), take_lsbs<wF>(newY) ),
            concat( cg<2>(0b01), take_lsbs<wF>(newY), zg<1>() )
        );

        // substract the fraction signals for the close path;

        // instanciate the box that computes X-Y and Y-X. Note that it could take its inputs before the swap (TODO ?)
        /*REPORT(DETAILED, "Building close path dual mantissa subtraction box");
        dualSubClose = new 	IntDualSub(target, wF + 3, 0);
        dualSubClose->changeName(getName()+"_DualSubClose");
        oplist.push_back(dualSubClose);

        inPortMap  (dualSubClose, "X", "fracXClose1");
        inPortMap  (dualSubClose, "Y", "fracYClose1");
        outPortMap (dualSubClose, "RxMy","fracRClosexMy");
        outPortMap (dualSubClose, "RyMx","fracRCloseyMx");
        vhdl << instance(dualSubClose, "DualSubO");*/
        
        /* With opType=0, this does:
            RxMy <= X + not(Y) + '1'  == X - Y
            RyMx <= not(X) + Y + '1'  == Y - X
        */
        auto fracRClosexMy = fracXClose1 - fracYClose1;
        auto fracRCloseyMx = fracYClose1 - fracXClose1;
        
        //vhdl<<tab<< declare("fracSignClose") << " <= fracRClosexMy("<<wF+2<<");"<<endl;
        auto fracSignClose = get_bit<wF+2>(fracRClosexMy);
        //vhdl<<tab<< declare("fracRClose1",wF+2) << " <= fracRClosexMy("<<wF+1<<" downto 0) when fracSignClose='0' else fracRCloseyMx("<<wF+1<<" downto 0);"<<endl;
        fw_uint<wF+2> fracRClose1 = select( ~fracSignClose, take_lsbs<wF+2>(fracRClosexMy), take_lsbs<wF+2>(fracRCloseyMx));

        //TODO check the test if significand is all zero is useful.
        //vhdl<< tab << declare("resSign") << " <= '0' when selectClosePath='1' and fracRClose1 = ("<<wF+1<<" downto 0 => '0') else"<<endl;
        // vhdl<< tab << "          newX("<<wE+wF<<") xor (selectClosePath and "
        //     << "fracSignClose);"<<endl;
        resSign = select(
            selectClosePath && (fracRClose1 == zg<wF+2>()),
            zg<1>(),
            get_bit<wE+wF>(newX) ^ (selectClosePath & fracSignClose)
        );
        
        // LZC + Shifting. The number of leading zeros are returned together with the shifted input
        /*
        REPORT(DETAILED, "Building close path LZC + shifter");
        lzocs = new LZOCShifterSticky(target, wF+2, wF+2, intlog2(wF+2), false, 0);

        lzocs->changeName(getName()+"_LZCShifter");
        oplist.push_back(lzocs);

        inPortMap  (lzocs, "I", "fracRClose1");
        outPortMap (lzocs, "Count","nZerosNew");
        outPortMap (lzocs, "O","shiftedFrac");
        vhdl << instance(lzocs, "LZC_component");
        */
        
        /*
        // LZOCShifterSticky
        //   wIn = wF+2
        //   wOut = wF+2
        //   wCount = intlog2(wF+2)
        //   computeSticky = false
        //   countType = 0  -> "spec" (rather than "gen")
        */
        const int lzocCountWidth=intlog2<wF+2>::value;

        fw_uint<lzocCountWidth> nZerosNew;
        fw_uint<wF+2> shiftedFrac;
        LZOCShifter<wF+2>(shiftedFrac, nZerosNew, fracRClose1);


        // NORMALIZATION

        // shiftedFrac(0) is the round bit, shiftedFrac(1) is the parity bit,
        // shiftedFrac(wF) is the leading one, to be discarded
        // the rounding bit is computed:
        //vhdl<<tab<< declare("roundClose0") << " <= shiftedFrac(0) and shiftedFrac(1);"<<endl;
        auto roundClose0 = get_bit<0>(shiftedFrac) & get_bit<1>(shiftedFrac);
        // Is the result zero?
        /*vhdl<<tab<< declare("resultCloseIsZero0") << " <= '1' when nZerosNew"
                << " = CONV_STD_LOGIC_VECTOR(" << (1<< lzocs->getCountWidth())-1 // Should be wF+2 but this is a bug of LZOCShifterSticky: for all zeroes it returns this value
                << ", " << lzocs->getCountWidth()
             << ") else '0';" << endl;*/
        auto resultCloseIsZero0 = nZerosNew == og<lzocCountWidth>();

        // add two bits in order to absorb exceptions:
        // the second 0 will become a 1 in case of overflow,
        // the first 0 will become a 1 in case of underflow (negative biased exponent)
        /*vhdl<<tab<< declare("exponentResultClose",wE+2) << " <= (\"00\" & "
             << "newX("<<wE+wF-1<<" downto "<<wF<<")) "
             <<"- (CONV_STD_LOGIC_VECTOR(0,"<<wE-lzocs->getCountWidth()+2<<") & nZerosNew);"
             <<endl;*/
        auto exponentResultClose = zpad_hi<2>(get_bits<wE+wF-1,wF>(newX)) - zpad_hi<wE-lzocCountWidth+2>(nZerosNew);


        // concatenate exponent with fractional part before rounding so the possible carry propagation automatically increments the exponent
        //vhdl<<tab<<declare("resultBeforeRoundClose",wE+1 + wF+1) << " <= exponentResultClose("<<wE+1<<" downto 0) & shiftedFrac("<<wF<<" downto 1);"<<endl;
        resultBeforeRoundClose = concat( take_lsbs<wE+2>(exponentResultClose) , get_bits<wF,1>(shiftedFrac) );
        //vhdl<<tab<< declare("roundClose") << " <= roundClose0;"<<endl;
        roundClose = roundClose0;
        //vhdl<<tab<< declare("resultCloseIsZero") << " <= resultCloseIsZero0;"<<endl;
        resultCloseIsZero = resultCloseIsZero0;
    }

    
    fw_uint<wE+1+wF+1> resultBeforeRoundFar;
    fw_uint<1> roundFar;
    
    if(1){
        //=========================================================================|
        //                              far path                                   |
        //=========================================================================|


        //vhdl<< endl << "-- Far Path --" << endl;
        
        //add implicit 1 for frac1.
        //vhdl<<tab<< declare("fracNewY",wF+1) << " <= '1' & newY("<<wF-1<<" downto 0);"<<endl;
        auto fracNewY = opad_hi<1>(take_lsbs<wF>(newY));
        
        if(DEBUG_far){
            std::cerr<<"  Far:  fracNewY = "<<fracNewY<<"\n";
        }

        // shift right the significand of new Y with as many positions as the exponent difference suggests (alignment)
        /*REPORT(DETAILED, "Building far path right shifter");
        rightShifter = new Shifter(target,wF+1,wF+3, Shifter::Right);
        rightShifter->changeName(getName()+"_RightShifter");
        oplist.push_back(rightShifter);
        inPortMap  (rightShifter, "X", "fracNewY");
        inPortMap  (rightShifter, "S", "shiftVal");
        outPortMap (rightShifter, "R","shiftedFracY");
        vhdl << instance(rightShifter, "RightShifterComponent");
        */
        
        // The shifter parameters are:
        //   wIn = wF+1
        //   maxShift = wF+3
        // which means;
        //   wOut = wIn+maxShift = wF+wF+4
        auto preFracNewY=zpad_lo<wF+3>(fracNewY);
        fw_uint<2*wF+4> shiftedFracY = preFracNewY >> shiftVal.to_int();
        
        if(DEBUG_far){
            std::cerr<<"  Far:  shiftedFracY = "<<shiftedFracY<<"\n";
        }

        // compute sticky bit as the or of the shifted out bits during the alignment //
        //vhdl<<tab<< declare("sticky") << " <= '0' when (shiftedFracY("<<wF<<" downto 0)=CONV_STD_LOGIC_VECTOR(0,"<<wF<<")) else '1';"<<endl;
        // TODO : Bug in widths?
        // Had to extend zero constant by one bit
        auto sticky = take_lsbs<wF+1>(shiftedFracY) != zg<wF+1>();
        
        if(DEBUG_far){
            std::cerr<<"  Far:  sticky = "<<sticky<<"\n";
        }

        //pad fraction of Y [sign][shifted frac having inplicit 1][guard bits]
        //vhdl<<tab<< declare("fracYfar", wF+4) << " <= \"0\" & shiftedFracY("<<2*wF+3<<" downto "<<wF+1<<");"<<endl;
        auto fracYfar = zpad_hi<1>( get_bits<2*wF+3,wF+1>(shiftedFracY) );
        
        if(DEBUG_far){
            std::cerr<<"  Far:  fracYfar = "<<fracYfar<<"\n";
        }

        // depending on the signs of the operands, perform addition or substraction
        // the result will be: a + (b xor operation) + operation, where operation=0=addition and operation=1=substraction
        // the operation selector is the xor between the signs of the operands
        // perform xor
        //vhdl<<tab<< declare("EffSubVector", wF+4) << " <= ("<<wF+3<<" downto 0 => EffSub);"<<endl;
        auto EffSubVector = select(EffSub, og<wF+4>(), zg<wF+4>());
        //vhdl<<tab<<declare("fracYfarXorOp", wF+4) << " <= fracYfar xor EffSubVector;"<<endl;
        auto fracYfarXorOp = fracYfar ^ EffSubVector;
        //pad fraction of X [sign][inplicit 1][fracX][guard bits]
        //vhdl<<tab<< declare("fracXfar", wF+4) << " <= \"01\" & (newX("<<wF-1<<" downto 0)) & \"00\";"<<endl;
        auto fracXfar = concat( cg<2>(0b01), take_lsbs<wF>(newX), cg<2>(0b00) );
        //vhdl<<tab<< declare("cInAddFar") << " <= EffSub and not sticky;"<< endl;
        auto cInAddFar = EffSub & ~ sticky;
        
        if(DEBUG_far){
            std::cerr<<"  Far:  EffSubVector = "<<EffSubVector<<"\n";
            std::cerr<<"  Far:  fracYfarXorOp = "<<fracYfarXorOp<<"\n";
            std::cerr<<"  Far:  fracXfar = "<<fracXfar<<"\n";
            std::cerr<<"  Far:  cInAddFar = "<<cInAddFar<<"\n";
        }

        // perform carry in addition
        /* REPORT(DETAILED, "Building far path adder");
        fracAddFar = new IntAdder(target,wF+4);
        fracAddFar->changeName(getName()+"_fracAddFar");
        oplist.push_back(fracAddFar);
        inPortMap  (fracAddFar, "X", "fracXfar");
        inPortMap  (fracAddFar, "Y", "fracYfarXorOp");
        inPortMap  (fracAddFar, "Cin", "cInAddFar");
        outPortMap (fracAddFar, "R","fracResultfar0");
        vhdl << instance(fracAddFar, "fracAdderFar");
        */
        auto fracResultfar0 = add_with_cin(fracXfar, fracYfarXorOp, cInAddFar);
        
        if(DEBUG_far){
            std::cerr<<"  Far:  fracResultfar0 = "<<fracResultfar0<<"\n";
        }

        //vhdl<< tab << "-- 2-bit normalisation" <<endl;
        //vhdl<< tab << declare("fracResultFarNormStage", wF+4) << " <= fracResultfar0;"<<endl;
        auto fracResultFarNormStage = fracResultfar0;

        // NORMALIZATION
        // The leading one may be at position wF+3, wF+2 or wF+1
        //
        //vhdl<<tab<< declare("fracLeadingBits", 2) << " <= fracResultFarNormStage("<<wF+3<<" downto "<<wF+2<<") ;" << endl;
        auto fracLeadingBits = get_bits<wF+3,wF+2>(fracResultFarNormStage);
        
        if(DEBUG_far){
            std::cerr<<"  Far:  fracLeadingBits = "<<fracLeadingBits<<"\n";
        }

        /*vhdl<<tab<< declare("fracResultFar1",wF) << " <=" << endl ;
        vhdl<<tab<<tab<< "     fracResultFarNormStage("<<wF+0<<" downto 1)  when fracLeadingBits = \"00\" "<<endl;
        vhdl<<tab<<tab<< "else fracResultFarNormStage("<<wF+1<<" downto 2)  when fracLeadingBits = \"01\" "<<endl;
        vhdl<<tab<<tab<< "else fracResultFarNormStage("<<wF+2<<" downto 3);"<<endl;*/
        auto fracResultFar1 = select(
            fracLeadingBits == cg<2>(0b00),
                get_bits<wF+0,1>(fracResultFarNormStage),
            fracLeadingBits == cg<2>(0b01),
                get_bits<wF+1,2>(fracResultFarNormStage),
            // else
                get_bits<wF+2,3>(fracResultFarNormStage)
        );
        
        if(DEBUG_far){
            std::cerr<<"  Far:  fracResultFar1 = "<<fracResultFar1<<"\n";
        }

        /*vhdl<<tab<< declare("fracResultRoundBit") << " <=" << endl ;
        vhdl<<tab<<tab<< "     fracResultFarNormStage(0) 	 when fracLeadingBits = \"00\" "<<endl;
        vhdl<<tab<<tab<< "else fracResultFarNormStage(1)    when fracLeadingBits = \"01\" "<<endl;
        vhdl<<tab<<tab<< "else fracResultFarNormStage(2) ;"<<endl;*/
        auto fracResultRoundBit = select(
            fracLeadingBits == cg<2>(0b00),
                get_bit<0>(fracResultFarNormStage),
            fracLeadingBits == cg<2>(0b01),
                get_bit<1>(fracResultFarNormStage),
            // else
                get_bit<2>(fracResultFarNormStage)
        );
        
        if(DEBUG_far){
            std::cerr<<"  Far:  fracResultRoundBit = "<<fracResultRoundBit<<"\n";
        }

        /*vhdl<<tab<< declare("fracResultStickyBit") << " <=" << endl ;
        vhdl<<tab<<tab<< "     sticky 	 when fracLeadingBits = \"00\" "<<endl;
        vhdl<<tab<<tab<< "else fracResultFarNormStage(0) or  sticky   when fracLeadingBits = \"01\" "<<endl;
        vhdl<<tab<<tab<< "else fracResultFarNormStage(1) or fracResultFarNormStage(0) or sticky;"<<endl;*/
        auto fracResultStickyBit = select(
            fracLeadingBits == cg<2>(0b00),
                sticky,
            fracLeadingBits == cg<2>(0b01),
                sticky | get_bit<0>(fracResultFarNormStage),
            // else
                sticky | get_bit<0>(fracResultFarNormStage) | get_bit<1>(fracResultFarNormStage)
        );
        if(DEBUG_far){
            std::cerr<<"  Far:  fracResultStickyBit = "<<fracResultStickyBit<<"\n";
        }
        
        
        // round bit
        //vhdl<<tab<< declare("roundFar1") <<" <= fracResultRoundBit and (fracResultStickyBit or fracResultFar1(0));"<<endl;
        auto roundFar1 = fracResultRoundBit & (fracResultStickyBit | get_bit<0>(fracResultFar1));
        if(DEBUG_far){
            std::cerr<<"  Far:  roundFar1 = "<<roundFar1<<"\n";
        }

        //select operation mode. This depends on wether or not the exponent must be adjusted after normalization
        /*vhdl<<tab<<declare("expOperationSel",2) << " <= \"11\" when fracLeadingBits = \"00\" -- add -1 to exponent"<<endl;
        vhdl<<tab<<"            else   \"00\" when fracLeadingBits = \"01\" -- add 0 "<<endl;
        vhdl<<tab<<"            else   \"01\";                              -- add 1"<<endl;*/
        auto expOperationSel = select(
            fracLeadingBits == 0b00,
                cg<2>(0b11),
            fracLeadingBits == 0b01,
                cg<2>(0b00),
            // else
                cg<2>(0b01)
        );
        if(DEBUG_far){
            std::cerr<<"  Far:  expOperationSel = "<<expOperationSel<<"\n";
        }

        //the second operand depends on the operation selector
        //vhdl<<tab<<declare("exponentUpdate",wE+2) << " <= ("<<wE+1<<" downto 1 => expOperationSel(1)) & expOperationSel(0);"<<endl;
        fw_uint<wE+2> exponentUpdate = concat( 
            copybit<wE+1>(get_bit<1>(expOperationSel)),
            get_bit<0>(expOperationSel)
        );
        if(DEBUG_far){
            std::cerr<<"  Far:  exponentUpdate = "<<exponentUpdate<<"\n";
        }

        // the result exponent before normalization and rounding is = to the exponent of the first operand //
        //vhdl<<tab<<declare("exponentResultfar0",wE+2) << "<=\"00\" & (newX("<<wF+wE-1<<" downto "<<wF<<"));"<<endl;
        fw_uint<wE+2> exponentResultfar0 = zpad_hi<2>( get_bits<wF+wE-1,wF>( newX ) );
        if(DEBUG_far){
            std::cerr<<"  Far:  exponentResultfar0 = "<<exponentResultfar0<<"\n";
        }

        //vhdl<<tab<<declare("exponentResultFar1",wE+2) << " <= exponentResultfar0 + exponentUpdate;" << endl;
        auto exponentResultFar1 = exponentResultfar0 + exponentUpdate;
        if(DEBUG_far){
            std::cerr<<"  Far:  exponentResultFar1 = "<<exponentResultFar1<<"\n";
        }

        // End of normalization stage
        /*vhdl<<tab<<declare("resultBeforeRoundFar",wE+1 + wF+1) << " <= "
             << "exponentResultFar1 & fracResultFar1;" << endl;*/
        resultBeforeRoundFar = concat(exponentResultFar1,fracResultFar1);
        //vhdl<<tab<< declare("roundFar") << " <= roundFar1;" << endl;
        roundFar = roundFar1;
        if(DEBUG_far){
            std::cerr<<"  Far:  resultBeforeRoundFar = "<<resultBeforeRoundFar<<"\n";
            std::cerr<<"  Far:  roundFar1 = "<<roundFar1<<"\n";
        }
    }


    //=========================================================================|
    //                              Synchronization                            |
    //=========================================================================|
    //vhdl<<endl<<"-- Synchronization of both paths --"<<endl;

    //synchronize the close signal
    //vhdl<<tab<<declare("syncClose") << " <= selectClosePath;"<<endl;
    auto syncClose = selectClosePath;
    
    if(DEBUG){
        std::cerr<<"  syncClose = "<<syncClose<<"\n";
    }

    /*// select between the results of the close or far path as the result of the operation
    vhdl<<tab<< "with syncClose select"<<endl;
    vhdl<<tab<< declare("resultBeforeRound",wE+1 + wF+1) << " <= resultBeforeRoundClose when '1',"<<endl;
    vhdl<<tab<< "                     resultBeforeRoundFar   when others;"<<endl;*/
    fw_uint<wE+1+wF+1> resultBeforeRound = select(syncClose,
        resultBeforeRoundClose,
        resultBeforeRoundFar
    );
    if(DEBUG_far){
        std::cerr<<"  resultBeforeRound = "<<resultBeforeRound<<"\n";
    }
    /*vhdl<<tab<< "with syncClose select"<<endl;
    vhdl<<tab<< declare("round") << " <= roundClose when '1',"<<endl;
    vhdl<<tab<< "         roundFar   when others;"<<endl;*/
    auto round = select(syncClose,
        roundClose,
        roundFar
    );
    if(DEBUG_far){
        std::cerr<<"  round = "<<round<<"\n";
    }

    //vhdl<<tab<< declare("zeroFromClose") << " <= syncClose and resultCloseIsZero;" <<endl;
    auto zeroFromClose = syncClose & resultCloseIsZero;
    if(DEBUG_far){
        std::cerr<<"  zeroFromClose = "<<zeroFromClose<<"\n";
    }

    //vhdl<< endl << "-- Rounding --" << endl;

    //REPORT(DETAILED, "Building final round adder");
    // finalRoundAdd will add the mantissa concatenated with exponent, two bits reserved for possible under/overflow
    /*finalRoundAdd = new IntAdder(target, wE + wF + 2, fraInputDelays);
    finalRoundAdd->changeName(getName()+"_finalRoundAdd");
    oplist.push_back(finalRoundAdd);

    ostringstream zero;
    zero<<"("<<1+wE+wF<<" downto 0 => '0') ";
    inPortMap   (finalRoundAdd, "X", "resultBeforeRound");
    inPortMapCst(finalRoundAdd, "Y", zero.str() );
    inPortMap   (finalRoundAdd, "Cin", "round");
    outPortMap  (finalRoundAdd, "R","resultRounded");
    vhdl << instance(finalRoundAdd, "finalRoundAdder");*/
    
    fw_uint<2+wE+wF> resultRounded = add_cin(resultBeforeRound, round);
    if(DEBUG_far){
        std::cerr<<"  resultRounded = "<<resultRounded<<"\n";
    }

    // We neglect the delay of the rest
    //vhdl<<tab<<declare("syncEffSub") << " <= EffSub;"<<endl;
    auto syncEffSub = EffSub;

    //X
    //vhdl<<tab<<declare("syncX",3+wE+wF) << " <= newX;"<<endl;
    auto syncX = newX;

    //signY
    //vhdl<<tab<<declare("syncSignY") << " <= pipeSignY;"<<endl;
    auto syncSignY = pipeSignY;

    // resSign comes from closer
    //vhdl<<tab<<declare("syncResSign") << " <= resSign;"<<endl;
    auto syncResSign = resSign;

    // compute the exception bits of the result considering the possible underflow and overflow
    //vhdl<<tab<< declare("UnderflowOverflow",2) << " <= resultRounded"<<range( wE+1+wF, wE+wF)<<";"<<endl;
    fw_uint<2> UnderflowOverflow = get_bits<wE+1+wF,wE+wF>(resultRounded);
    if(DEBUG_far){
        std::cerr<<"  UnderflowOverflow = "<<UnderflowOverflow<<"\n";
    }

    /*vhdl<<tab<< "with UnderflowOverflow select"<<endl;
    vhdl<<tab<< declare("resultNoExn",wE+wF+3) << "("<<wE+wF+2<<" downto "<<wE+wF+1<<") <=   (not zeroFromClose) & \"0\" when \"01\", -- overflow"<<endl;
    vhdl<<tab<< "                              \"00\" when \"10\" | \"11\",  -- underflow"<<endl;
    vhdl<<tab<< "                              \"0\" &  not zeroFromClose  when others; -- normal "<<endl;*/
    auto resultNoExnHi = select(
        UnderflowOverflow==0b01,
            concat(~zeroFromClose,zg<1>()),
        (UnderflowOverflow==0b10) | (UnderflowOverflow==0b11),
            cg<2>(0b00),
        // else
            zpad_hi<1>(~zeroFromClose)
    );
    if(DEBUG_far){
        std::cerr<<"  resultNoExnHi = "<<resultNoExnHi<<"\n";
    }

    //vhdl<<tab<< "resultNoExn("<<wE+wF<<" downto 0) <= syncResSign & resultRounded("<<wE+wF-1<<" downto 0);"<<endl;
    fw_uint<wE+wF+1> resultNoExnLo = concat(syncResSign, take_lsbs<wE+wF>(resultRounded));
    if(DEBUG_far){
        std::cerr<<"  resultNoExnLo = "<<resultNoExnLo<<"\n";
    }
    
    fw_uint<wE+wF+3> resultNoExn = concat(resultNoExnHi, resultNoExnLo);
    if(DEBUG_far){
        std::cerr<<"  resultNoExn = "<<resultNoExn<<"\n";
    }

    //vhdl<<tab<< declare("syncExnXY", 4) << " <= sdExnXY;"<<endl;
    auto syncExnXY = sdExnXY;
    if(DEBUG_far){
        std::cerr<<"  syncExnXY = "<<syncExnXY<<"\n";
    }
    
    /*vhdl<<tab<< "-- Exception bits of the result" << endl;
    vhdl<<tab<< "with syncExnXY select -- remember that ExnX > ExnY "<<endl;
    vhdl<<tab<<tab<< declare("exnR",2) <<" <= resultNoExn("<<wE+wF+2<<" downto "<<wE+wF+1<<") when \"0101\","<<endl;
    vhdl<<tab<<tab<< "        \"1\" & syncEffSub          when \"1010\","<<endl;
    vhdl<<tab<<tab<< "        \"11\"                      when \"1110\","<<endl;
    vhdl<<tab<<tab<< "        syncExnXY(3 downto 2)     when others;"<<endl;*/
    if(DEBUG_far){
        std::cerr<<"  get_bits<wE+wF+2,wE+wF+1>(resultNoExn) = "<<get_bits<wE+wF+2,wE+wF+1>(resultNoExn)<<"\n";
        std::cerr<<"  resultNoExn>>(wE+wF-1) = "<<(resultNoExn>>(wE+wF-1))<<"\n";
        std::cerr<<"  resultNoExn = "<<(resultNoExn)<<"\n";
    }
    auto exnR = select(
        syncExnXY==0b0101,
            get_bits<wE+wF+2,wE+wF+1>(resultNoExn),
        syncExnXY==0b1010,
            opad_hi<1>(syncEffSub),
        syncExnXY==0b1110,
            cg<2>(0b11),
        // else
            get_bits<3,2>(syncExnXY)
    );
    if(DEBUG_far){
        std::cerr<<"  exnR = "<<exnR<<"\n";
    }
    
    //vhdl<<tab<< "-- Sign bit of the result" << endl;
    /*vhdl<<tab<< "with syncExnXY select"<<endl;
    vhdl<<tab<<tab<<declare("sgnR") << " <= resultNoExn("<<wE+wF<<")         when \"0101\","<<endl;
    vhdl<<tab<< "           syncX("<<wE+wF<<") and syncSignY when \"0000\","<<endl;
    vhdl<<tab<< "           syncX("<<wE+wF<<")               when others;"<<endl;*/
    auto sgnR = select(
        syncExnXY==0b0101,
            get_bit<wE+wF>(resultNoExn),
        syncExnXY==0b0000,
            get_bit<wE+wF>(syncX) & syncSignY,
        // else
            get_bit<wE+wF>(syncX)
    );
    if(DEBUG_far){
        std::cerr<<"  sgnR = "<<sgnR<<"\n";
    }

    /*vhdl<<tab<< "-- Exponent and significand of the result" << endl;
    vhdl<<tab<< "with syncExnXY select  "<<endl;
    vhdl<<tab<<tab<< declare("expsigR", wE+wF) << " <= resultNoExn("<<wE+wF-1<<" downto 0)   when \"0101\" ,"<<endl;
    vhdl<<tab<<tab<< "           syncX("<<wE+wF-1<<" downto  0)        when others; -- 0100, or at least one NaN or one infty "<<endl;*/
    auto expsigR = select(
        syncExnXY==0b0101,
            take_lsbs<wE+wF>(resultNoExn),
        // else
            take_lsbs<wE+wF>(syncX)
    );
    if(DEBUG_far){
        std::cerr<<"  expsigR = "<<expsigR<<"\n";
    }

    // assign result
    //vhdl<<tab<< "R <= exnR & sgnR & expsigR;"<<endl;
    auto R= concat(exnR,sgnR,expsigR);

    return fp_flopoco<wER,wFR>(R);
}

template<int wER,int wFR, int wEX,int wFX,int wEY,int wFY>
THLS_INLINE fp_flopoco<wER,wFR> add_fast(const fp_flopoco<wEX,wFX> &xPre, const fp_flopoco<wEY,wFY> &yPre, int DEBUG=0)
{
    return add_dual<wER,wFR,wEX,wFX,wEY,wFY>(xPre, yPre, DEBUG);
};



}; // thls

#endif
