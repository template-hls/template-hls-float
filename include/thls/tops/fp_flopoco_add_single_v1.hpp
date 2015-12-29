#ifndef thls_fp_flopoco_add_v1_hpp
#define thls_fp_flopoco_add_v1_hpp

#include "thls/tops/fp_flopoco.hpp"
#include "thls/tops/fp_promote.hpp"

namespace thls
{

template<int N>
struct intlog2
{
    enum { value = 1 + intlog2<N/2>::value };
};

template<>
struct intlog2<0>
{
    enum{ value = 0 };
};

/*
template<int maxPlaces, int WD, int WC>
void LZOCShifter(fw_uint<WD> &out, fw_uint<WC> &count, const fw_uint<WD> &x)
{
    static_assert(maxPlaces <= (1<<WC), "Not enough bits in count.");

    fw_uint<WC> nZerosNew=zg<WC>();
    fw_uint<WD> shifted = x;

    if( (x==0).to_bool() ){
        nZerosNew=og<WC>();
    }else{
        for(int i=0; i<WD-1; i++){
            if( (get_bit<WD-1>(shifted)==1).to_bool() )
                break;
            shifted=shifted<<1;
            nZerosNew=nZerosNew+1;
        }
    }

    count=nZerosNew;
    out=shifted;
}
*/


/*
    00000 -> 00000, 7
    00001 -> 10000, 4
    0001x -> 1x000, 3
    001xx -> 1xx00, 2
    01xxx -> 1xxx0, 1
    1xxxx -> 1xxxx, 0
    
    0000 -> 0000, 7
    0001 -> 1000, 3
    001x -> 1x00, 2
    01xx -> 1xx0, 1
    1xxx -> 1xxx, 0
    
    000 -> 000, 3
    001 -> 100, 2
    01x -> 1x0, 1
    1xx -> 1xx, 0
    
    
    WC==1:
    Pre:  00000000 | 01xxx000 | 1xxx0000
    Post: 00000000 | 1xxx0000
    
    WC==2:
    Pre:  00000000 | (1xxx0000 | 001xxx00) | (01xxx000 | 1xxx0000)
    Post: 00000000 | 01xxx000 | 1xxx0000
*/

template<int WD, int WC>
class LZOCShifterImpl
{
    static void stage(fw_uint<WD> &out, fw_uint<1> &bit, const fw_uint<WD> &x)
    {
        assert( WD >= (1<<(WC-1)) );
        
        static const int SHIFT=(1<<(WC-1));
        fw_uint<SHIFT> hi=get_bits<WD-1,WD-SHIFT>(x);
        
        if(SHIFT==WD){
            // We are just a zero detector
            bit= (hi==zg<SHIFT>());
            out= x;
        }else{
            #ifndef NDEBUG
            static const int BASE= WD-2*SHIFT ? 0 : WD-2*SHIFT;
            auto lo=get_bits<WD-SHIFT-1,BASE>(x);
            assert( ((x==zg<WD>()) || (hi!=zg<SHIFT>()) || (lo!=zg<lo.width>())).to_bool() );
            #endif
            
            bit= (hi==zg<SHIFT>());
            out=select(bit, (x<<SHIFT), x);
        }
    }
    
public:
    static void go(fw_uint<WD> &out, fw_uint<WC> &count, const fw_uint<WD> &x)
    {
        // Deal with top 2**(WC-1) bits
        fw_uint<1> hiCount;
        fw_uint<WD> mid;
        stage(mid, hiCount, x);
        
        // Then remaining bits.
        fw_uint<WC-1> loCount;
        LZOCShifterImpl<WD,WC-1>::go(out, loCount, mid);

        count=concat(hiCount,loCount);
    }
};

template<int WD>
class LZOCShifterImpl<WD,0>
{
public:
    static void go(fw_uint<WD> &out, fw_uint<0> &count, const fw_uint<WD> &x)
    {
        out=x;
    }
};

template<int maxPlaces, int WD, int WC>
void LZOCShifter(fw_uint<WD> &out, fw_uint<WC> &count, const fw_uint<WD> &x)
{
    return LZOCShifterImpl<WD,WC>::go(out,count,x);
}

template<int wER,int wFR, int wEX,int wFX,int wEY,int wFY>
THLS_INLINE fp_flopoco<wER,wFR> add_single(const fp_flopoco<wEX,wFX> &xPre, const fp_flopoco<wEY,wFY> &yPre, int DEBUG=0)
{
    
    #if 0
    //parameter set up. For now all wEX=wEY=wER and the same holds for fractions
    THLS_STATIC_ASSERT(wEX==wEY && wEY==wER, "All exponent widths must be the same.");
    THLS_STATIC_ASSERT(wFX==wFY && wFY==wFR, "All fraction widths must be the same.");
    // DT10 - Could just expand to the largest exponent? TODO
    const int wF = wFX;
    const int wE = wEX;
    
    const fp_flopoco<wE,wF> &x=xPre;
    const fp_flopoco<wE,wF> &y=yPre;
    
    #else
    // Allow wFX!=wFY and wER!=wFR, but require that wER=max(wEX,wEY) and wFR=max(wFX,wFY)
    
    const int wF = thls_ctMax(wFX,wFY);
    const int wE = thls_ctMax(wEX,wEY);
    
    fp_flopoco<wE,wF> x;
    fp_flopoco<wE,wF> y;
    promote(x, xPre);
    promote(y, yPre);
    
    THLS_STATIC_ASSERT(wE==wER, "Result exp must match promotion of args.");
    THLS_STATIC_ASSERT(wF==wFR, "Result frac must match promotion of args.");
    
    #endif

    // Copyright : This is heavily based on copyright work of Bogdan Pasca and Florent de Dinechin (2010)

    if(DEBUG){
        std::cerr<<"  x = "<<x.str()<<"\n";
        std::cerr<<"  y = "<<y.str()<<"\n";
    }

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

    //vhdl << tab << declare("excExpFracX",2+wE+wF) << " <= X"<<range(wE+wF+2, wE+wF+1) << " & X"<<range(wE+wF-1, 0)<<";"<<endl;
    fw_uint<2+wE+wF> excExpFracX = concat( get_bits<wE+wF+2, wE+wF+1>(X) , get_bits<wE+wF-1,0>(X) );
    //vhdl << tab << declare("excExpFracY",2+wE+wF) << " <= Y"<<range(wE+wF+2, wE+wF+1) << " & Y"<<range(wE+wF-1, 0)<<";"<<endl;
    fw_uint<2+wE+wF> excExpFracY = concat( get_bits<wE+wF+2, wE+wF+1>(Y) , get_bits<wE+wF-1, 0>(Y) );

    //vhdl<< tab << declare("eXmeY",wE+1) << " <= (\"0\" & X"<<range(wE+wF-1,wF)<<") - (\"0\" & Y"<<range(wE+wF-1,wF)<<");"<<endl;
    fw_uint<wE+1> eXmeY = zpad_hi<1>(get_bits<wE+wF-1, wF>(X)) - zpad_hi<1>(get_bits< wE+wF-1, wF>(Y));
    //vhdl<< tab << declare("eYmeX",wE+1) << " <= (\"0\" & Y"<<range(wE+wF-1,wF)<<") - (\"0\" & X"<<range(wE+wF-1,wF)<<");"<<endl;
    fw_uint<wE+1> eYmeX = zpad_hi<1>(get_bits<wE+wF-1, wF>(Y)) - zpad_hi<1>(get_bits< wE+wF-1, wF>(X));

    if(DEBUG){
        std::cerr<<"  eXmeY = "<<eXmeY<<"\n";
        std::cerr<<"  eYmeX = "<<eYmeX<<"\n";
    }

    // NOTE : FloPoCo used logic here that swapped to an adder component
    // for wf>=30. Let's hope (heh) that the HLS tool does something
    // sensible for us.
    // TODO - Does it do anything sensible for large exponent?
    fw_uint<1> swap=excExpFracX < excExpFracY;

    // depending on the value of swap, assign the corresponding values to the newX and newY signals
    // vhdl<<tab<<declare("newX",wE+wF+3) << " <= X     when swap = '0' else Y;"<<endl;
    auto newX = select(swap==0, X, Y);
    // vhdl<<tab<<declare("newY",wE+wF+3) << " <= Y     when swap = '0' else X;"<<endl;
    auto newY = select(swap==0, Y, X);
    //break down the signals
    //vhdl << tab << declare("expX",wE) << "<= newX"<<range(wE+wF-1,wF)<<";"<<endl;
    auto expX = get_bits< wE+wF-1, wF>(newX);
    //vhdl << tab << declare("excX",2)  << "<= newX"<<range(wE+wF+2,wE+wF+1)<<";"<<endl;
    auto excX = get_bits< wE+wF+2, wE+wF+1>(newX);
    //vhdl << tab << declare("excY",2)  << "<= newY"<<range(wE+wF+2,wE+wF+1)<<";"<<endl;
    auto excY = get_bits< wE+wF+2, wE+wF+1>(newY);
    //vhdl << tab << declare("signX")   << "<= newX"<<of(wE+wF)<<";"<<endl;
    auto signX = get_bit<wE+wF>(newX);
    //vhdl << tab << declare("signY")   << "<= newY"<<of(wE+wF)<<";"<<endl;
    auto signY = get_bit<wE+wF>(newY);
    //vhdl << tab << declare("EffSub") << " <= signX xor signY;"<<endl;
    auto EffSub = signX ^ signY;
    //vhdl << tab << declare("sXsYExnXY",6) << " <= signX & signY & excX & excY;"<<endl;
    fw_uint<6> sXsYExnXY = concat(signX, signY, excX, excY);
    //vhdl << tab << declare("sdExnXY",4) << " <= excX & excY;"<<endl;
    //auto sdExnXY = concat(excX, excY);
    //vhdl << tab << declare("fracY",wF+1) << " <= "<< zg(wF+1)<<" when excY=\"00\" else ('1' & newY("<<wF-1<<" downto 0));"<<endl;
    auto fracY = select(excY==0b00, zg<wF+1>(), opad_hi<1>(get_bits<wF-1,0>(newY)));

    if(DEBUG){
        std::cerr<<"  fracY = "<<fracY<<"\n";
    }

    //exception bits: need to be updated but for not FIXME
    //vhdl <<tab<<"with sXsYExnXY select "<<endl;
    //vhdl <<tab<<declare("excRt",2) << " <= \"00\" when \"000000\"|\"010000\"|\"100000\"|\"110000\","<<endl
    //<<tab<<tab<<"\"01\" when \"000101\"|\"010101\"|\"100101\"|\"110101\"|\"000100\"|\"010100\"|\"100100\"|\"110100\"|\"000001\"|\"010001\"|\"100001\"|\"110001\","<<endl
    //<<tab<<tab<<"\"10\" when \"111010\"|\"001010\"|\"001000\"|\"011000\"|\"101000\"|\"111000\"|\"000010\"|\"010010\"|\"100010\"|\"110010\"|\"001001\"|\"011001\"|\"101001\"|\"111001\"|\"000110\"|\"010110\"|\"100110\"|\"110110\", "<<endl
    //<<tab<<tab<<"\"11\" when others;"<<endl;

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

    //vhdl <<tab<<declare("signR") << "<= '0' when (sXsYExnXY=\"100000\" or sXsYExnXY=\"010000\") else signX;"<<endl;
    auto signR = select(sXsYExnXY==0b100000 || sXsYExnXY==0b010000, zg<1>(), signX);
    if(DEBUG){
        std::cerr<<"  signR = "<<signR<<"\n";
    }


    //vhdl<<tab<<declare("expDiff",wE+1) << " <= eXmeY when swap = '0' else eYmeX;"<<endl;
    auto expDiff = select(swap==0, eXmeY, eYmeX);

    if(DEBUG){
        std::cerr<<"  expDiff = "<<expDiff<<"\n";
    }

    //vhdl<<tab<<declare("shiftedOut") << " <= '1' when (expDiff >= "<<wF+2<<") else '0';"<<endl;
    auto shiftedOut = expDiff >= (wF+2);

    //shiftVal=the number of positions that fracY must be shifted to the right

    //		cout << "********" << wE << " " <<  sizeRightShift  <<endl;



    fw_uint<sizeRightShift> shiftVal;

    if (wE>sizeRightShift) {
        //vhdl<<tab<<declare("shiftVal",sizeRightShift) << " <= expDiff("<< sizeRightShift-1<<" downto 0)"
        //<< " when shiftedOut='0' else CONV_STD_LOGIC_VECTOR("<<wFX+3<<","<<sizeRightShift<<") ;" << endl;
        shiftVal = checked_cast<sizeRightShift>(  select(shiftedOut==0, get_bits< sizeRightShift-1, 0>(expDiff), fw_uint<sizeRightShift>(wF+3)) );
    }
    else if (wE==sizeRightShift) {
        //vhdl<<tab<<declare("shiftVal", sizeRightShift) << " <= expDiff" << range(sizeRightShift-1,0) << ";" << endl ;
        shiftVal = checked_cast<sizeRightShift>( get_bits<sizeRightShift-1,0>(expDiff) );
    }
    else 	{ //  wE< sizeRightShift
        //vhdl<<tab<<declare("shiftVal",sizeRightShift) << " <= CONV_STD_LOGIC_VECTOR(0,"<<sizeRightShift-wE <<") & expDiff;" <<	endl;
        shiftVal = checked_cast<sizeRightShift>( concat( zg<sizeRightShift-wE>(), expDiff) );

    }
    if(DEBUG){
        std::cerr<<"  shiftVal = "<<shiftVal<<"\n";
    }


    // shift right the significand of new Y with as many positions as the exponent difference suggests (alignment)
    // Building far path right shifter
    /*
    rightShifter = new Shifter(target,wF+1,wF+3, Shifter::Right, inDelayMap("X",getCriticalPath()));
    rightShifter->changeName(getName()+"_RightShifter");
    addSubComponent(rightShifter);
    inPortMap  (rightShifter, "X", "fracY");
    inPortMap  (rightShifter, "S", "shiftVal");
    outPortMap (rightShifter, "R","shiftedFracY");
    vhdl << instance(rightShifter, "RightShifterComponent");
    syncCycleFromSignal("shiftedFracY");
    setCriticalPath(rightShifter->getOutputDelay("R"));
    nextCycle();         ////
    setCriticalPath(0.0);////
    double cpshiftedFracY = getCriticalPath();
    //sticky compuation in parallel with addition, no need for manageCriticalPath
    //FIXME: compute inside shifter;
    //compute sticky bit as the or of the shifted out bits during the alignment //
    manageCriticalPath(target->localWireDelay() + target->eqConstComparatorDelay(wF+1));
    vhdl<<tab<< declare("sticky") << " <= '0' when (shiftedFracY("<<wF<<" downto 0)=CONV_STD_LOGIC_VECTOR(0,"<<wF<<")) else '1';"<<endl;
    double cpsticky = getCriticalPath();
    */


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
    //vhdl<<tab<< declare("fracYfar", wF+4)      << " <= \"0\" & shiftedFracY("<<2*wF+3<<" downto "<<wF+1<<");"<<endl;
    auto fracYfar = zpad_hi<1>(get_bits<2*wF+3,wF+1>(shiftedFracY));
    //vhdl<<tab<< declare("EffSubVector", wF+4) << " <= ("<<wF+3<<" downto 0 => EffSub);"<<endl;
    auto EffSubVector = select(EffSub, og<wF+4>(), zg<wF+4>());
    //vhdl<<tab<< declare("fracYfarXorOp", wF+4) << " <= fracYfar xor EffSubVector;"<<endl;
    fw_uint<wF+4> fracYfarXorOp = fracYfar ^ EffSubVector;
    //pad fraction of X [overflow][inplicit 1][fracX][guard bits]
    //vhdl<<tab<< declare("fracXfar", wF+4)      << " <= \"01\" & (newX("<<wF-1<<" downto 0)) & \"00\";"<<endl;
    fw_uint<wF+4> fracXfar = concat( fw_uint<2>(0b01), get_bits<wF-1,0>(newX), zg<2>() );

    //vhdl<<tab<< declare("cInAddFar")           << " <= EffSub and not sticky;"<< endl;//TODO understand why
    auto cInAddFar = EffSub & ~sticky;

    if(DEBUG){
        std::cerr<<"  fracYfar = "<<fracYfar<<"\n";
        std::cerr<<"  fracYfarXorOp = "<<fracYfarXorOp<<"\n";
        std::cerr<<"  fracXfar = "<<fracXfar<<"\n";
        std::cerr<<"  cInAddFar = "<<cInAddFar<<"\n";
    }

    //result is always positive.
    //fracAddFar = new IntAdder(target,wF+4, inDelayMap("X", getCriticalPath()));
    //addSubComponent(fracAddFar);
    //inPortMap  (fracAddFar, "X", "fracXfar");
    //inPortMap  (fracAddFar, "Y", "fracYfarXorOp");
    //inPortMap  (fracAddFar, "Cin", "cInAddFar");
    //outPortMap (fracAddFar, "R","fracAddResult");
    //vhdl << instance(fracAddFar, "fracAdder");

    fw_uint<wF+4> fracAddResult = add_with_cin(fracXfar,fracYfarXorOp,cInAddFar);
    if(DEBUG){
        std::cerr<<"  fracAddResult = "<<fracAddResult<<"\n";
    }

    //shift in place
    //vhdl << tab << declare("fracGRS",wF+5) << "<= fracAddResult & sticky; "<<endl;
    fw_uint<wF+5> fracGRS = concat(fracAddResult, sticky);

    //incremented exponent.
    //vhdl << tab << declare("extendedExpInc",wE+2) << "<= (\"00\" & expX) + '1';"<<endl;
    fw_uint<wE+2> extendedExpInc = zpad_hi<2>(expX)+1;
    if(DEBUG){
        std::cerr<<"  extendedExpInc = "<<extendedExpInc<<"\n";
    }



    //lzocs = new LZOCShifterSticky(target, wF+5, wF+5, intlog2(wF+5), false, 0, inDelayMap("I",getCriticalPath()));
    //addSubComponent(lzocs);
    //inPortMap  (lzocs, "I", "fracGRS");
    //outPortMap (lzocs, "Count","nZerosNew");
    //outPortMap (lzocs, "O","shiftedFrac");
    //vhdl << instance(lzocs, "LZC_component");

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
/*		manageCriticalPath(target->localWireDelay() + target->adderDelay(wE+2));*/
// 	vhdl << tab << declare("expPart",wE+2) << " <= (" << zg(wE+2-lzocs->getCountWidth(),0) <<" & nZerosNew) - 1;"<<endl;
    //update exponent

    //vhdl << tab << declare("updatedExp",wE+2) << " <= extendedExpInc - (" << zg(wE+2-lzocs->getCountWidth(),0) <<" & nZerosNew);"<<endl;
    auto updatedExp = extendedExpInc - zpad_hi<wE+2-lzocCountWidth>(nZerosNew);
    //vhdl << tab << declare("eqdiffsign")<< " <= '1' when nZerosNew="<<og(lzocs->getCountWidth(),0)<<" else '0';"<<endl;
    auto eqdiffsign = nZerosNew==og<lzocCountWidth>();

    if(DEBUG){
        std::cerr<<"  updatedExp = "<<updatedExp<<"\n";
        std::cerr<<"  eqdiffsign = "<<eqdiffsign<<"\n";
    }

    //concatenate exponent with fraction to absorb the possible carry out
    //vhdl<<tab<<declare("expFrac",wE+2+wF+1)<<"<= updatedExp & shiftedFrac"<<range(wF+3,3)<<";"<<endl;
    auto expFrac = concat(updatedExp,get_bits<wF+3,3>(shiftedFrac));


    // 		//at least in parallel with previous 2 statements
    //vhdl<<tab<<declare("stk")<<"<= shiftedFrac"<<of(1)<<" or shiftedFrac"<<of(0)<<";"<<endl;
    auto stk = get_bit<1>(shiftedFrac) | get_bit<0>(shiftedFrac);
    //vhdl<<tab<<declare("rnd")<<"<= shiftedFrac"<<of(2)<<";"<<endl;
    auto rnd = get_bit<2>(shiftedFrac);
    //vhdl<<tab<<declare("grd")<<"<= shiftedFrac"<<of(3)<<";"<<endl;
    auto grd = get_bit<3>(shiftedFrac);
    //vhdl<<tab<<declare("lsb")<<"<= shiftedFrac"<<of(4)<<";"<<endl;
    auto lsb = get_bit<4>(shiftedFrac);

    if(DEBUG){
        std::cerr<<"  stk = "<<stk<<"\n";
        std::cerr<<"  rnd = "<<rnd<<"\n";
        std::cerr<<"  grd = "<<grd<<"\n";
        std::cerr<<"  lsb = "<<lsb<<"\n";
    }

    //decide what to add to the guard bit
    //vhdl<<tab<<declare("addToRoundBit")<<"<= '0' when (lsb='0' and grd='1' and rnd='0' and stk='0')  else '1';"<<endl;
    auto addToRoundBit = select(lsb==0 && grd==1 && rnd==0 && stk==0, zg<1>(), og<1>());
    //round

    //IntAdder *ra = new IntAdder(target, wE+2+wF+1, inDelayMap("X", getCriticalPath() ) );
    //addSubComponent(ra);

    //inPortMap(ra,"X", "expFrac");
    //inPortMapCst(ra, "Y", zg(wE+2+wF+1,0) );
    //inPortMap( ra, "Cin", "addToRoundBit");
    //outPortMap( ra, "R", "RoundedExpFrac");
    //vhdl << instance(ra, "roundingAdder");

    auto RoundedExpFrac = add_cin(expFrac, addToRoundBit);
    if(DEBUG){
        std::cerr<<"  addToRoundBit = "<<addToRoundBit<<"\n";
        std::cerr<<"  expFrac = "<<expFrac<<"\n";
        std::cerr<<"  RoundedExpFrac = "<<RoundedExpFrac<<"\n";
    }

// 		vhdl<<tab<<declare("RoundedExpFrac",wE+2+wF+1)<<"<= expFrac + addToRoundBit;"<<endl;

    //possible update to exception bits
    //vhdl << tab << declare("upExc",2)<<" <= RoundedExpFrac"<<range(wE+wF+2,wE+wF+1)<<";"<<endl;
    auto upExc = get_bits<wE+wF+2,wE+wF+1>(RoundedExpFrac);

    //vhdl << tab << declare("fracR",wF)<<" <= RoundedExpFrac"<<range(wF,1)<<";"<<endl;
    auto fracR = get_bits<wF,1>(RoundedExpFrac);
    //vhdl << tab << declare("expR",wE) <<" <= RoundedExpFrac"<<range(wF+wE,wF+1)<<";"<<endl;
    auto expR = get_bits<wF+wE,wF+1>(RoundedExpFrac);



    //vhdl << tab << declare("exExpExc",4) << " <= upExc & excRt;"<<endl;
    auto exExpExc = concat(upExc, excRt);

    //vhdl << tab << "with (exExpExc) select "<<endl;
    //vhdl << tab << declare("excRt2",2) << "<= \"00\" when \"0000\"|\"0100\"|\"1000\"|\"1100\"|\"1001\"|\"1101\","<<endl
    //<<tab<<tab<<"\"01\" when \"0001\","<<endl
    //<<tab<<tab<<"\"10\" when \"0010\"|\"0110\"|\"1010\"|\"1110\"|\"0101\","<<endl
    //<<tab<<tab<<"\"11\" when others;"<<endl;
    static const lut<2,4> excRt2_lut([](int i){
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
    auto excRt2 = excRt2_lut(exExpExc);

    if(DEBUG){
        std::cerr<<"  exExpExc = "<<exExpExc<<"\n";
        std::cerr<<"  excRt2 = "<<excRt2<<"\n";
    }


    //vhdl<<tab<<declare("excR",2) << " <= \"00\" when (eqdiffsign='1' and EffSub='1') else excRt2;"<<endl;
    //auto excR = select(eqdiffsign==1 && EffSub==1, zg<2>(), excRt2);
    // TODO
    // TODO - Is there a bug in FloPoCo or here? Current behaviour suggests that (+infy)+(-infty) = 0
    // TODO
    // TODO - I think it should be NaN
    // TODO
    auto excR = select(eqdiffsign==1 && EffSub==1 && excRt2==0b01, zg<2>(), excRt2);

    // IEEE standard says in 6.3: if exact sum is zero, it should be +zero in RN
    //vhdl<<tab<<declare("signR2") << " <= '0' when (eqdiffsign='1' and EffSub='1') else signR;"<<endl;
    auto signR2 = select(eqdiffsign==1 && EffSub==1 && excR==0b00, zg<1>(), signR);

    // assign result
    //vhdl<<tab<< declare("computedR",wE+wF+3) << " <= excR & signR2 & expR & fracR;"<<endl;
    auto computedR = concat(excR,signR2,expR,fracR);
    //vhdl << tab << "R <= computedR;"<<endl;
    R=computedR;

    return fp_flopoco<wER,wFR>(R);
}

template<int wER,int wFR, int wEX,int wFX,int wEY,int wFY>
THLS_INLINE fp_flopoco<wER,wFR> add_small(const fp_flopoco<wEX,wFX> &xPre, const fp_flopoco<wEY,wFY> &yPre, int DEBUG=0)
{
    return add_single<wER,wFR,wEX,wFX,wEY,wFY>(xPre, yPre, DEBUG);
};


}; // thls

#endif
