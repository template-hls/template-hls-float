#ifndef thls_fp_flopoco_fp2fix_hpp
#define thls_fp_flopoco_fp2fix_hpp

#include "thls/tops/fp_flopoco.hpp"
#include "thls/tops/fp_promote.hpp"

#include <gmpxx.h>


namespace thls
{

template<bool Signed, int MSBO, int LSBO, int wEI, int wFI, bool trunc_p>
THLS_INLINE fw_uint<MSBO-LSBO+1> fp2fix(const fp_flopoco<wEI,wFI> &I, int DEBUG=0)
{
    const int MSB=MSBO;
    const int LSB=LSBO;
        
    static_assert( MSB >= LSB, " FP2Fix: Input constraint LSB <= MSB not met.");
    static_assert( (-LSB <= wFI), "FP2Fix: Input constraint -LSB <= wFI not met.");
    static_assert( LSB<0, "FP2Fix: Input constraint LSB<0 not met.");

    
    const int wFO = MSB - LSB + 1;
    //mpz_class maxExpWE = mpz_class(1)<<(wEI-1);
    static_assert( wEI <= 24, "Not dealing with massive exponents.");
    const int maxExpWE = 1<<(wEI-1);
    //mpz_class minExpWE = 1 - maxExpWE;
    const int minExpWE = 1 - maxExpWE;

    //int eMax = static_cast<int>(maxExpWE.get_si()) - 1;
    const int eMax = maxExpWE - 1;
    /*int wFO0;
    if(eMax+1 < MSB + 1)
        wFO0 = eMax + 1 - LSB;
    else
        wFO0 = MSB + 1 - LSB;*/
    const int wFO0 = eMax+1 < MSB+1 ? eMax+1-LSB : MSB+1-LSB;

    /*if (( maxExpWE < MSB ) || ( minExpWE > LSB)){
        cerr << " The exponent is too small for full coverage. Try increasing the exponent !"<<endl;
        exit (EXIT_FAILURE);
    }*/
    static_assert( ! (( maxExpWE < MSB ) || ( minExpWE > LSB)), " The exponent is too small for full coverage. Try increasing the exponent !");
    
      
      
    const int absMSB = MSB>=0?MSB:-MSB;
    const int absLSB = LSB>=0?LSB:-LSB;
    //name<<"FP2Fix_" << wEI << "_" << wFI << (LSB<0?"M":"") << "_" << absLSB << "_" << (MSB<0?"M":"") << absMSB <<"_"<< (Signed?"S":"US") << "_" << (trunc_p==1?"T":"NT");
    //setNameWithFreqAndUID(name.str());

    // setCopyrightString("Fabrizio Ferrandi (2012)");

    /* Set up the IO signals */

    //addFPInput ("I", wEI,wFI);
    //addOutput ("O", MSB-LSB+1);

    /*	VHDL code description	*/
    //vhdl << tab << declare("eA0",wEI) << " <= I" << range(wEI+wFI-1,wFI) << ";"<<endl;
    fw_uint<wEI> eA0 = get_bits<wEI+wFI-1, wFI>(I.bits);
    //vhdl << tab << declare("fA0",wFI+1) << " <= \"1\" & I" << range(wFI-1, 0)<<";"<<endl;
    fw_uint<wFI+1> fA0 = concat( og1, get_bits<wFI-1,0>(I.bits)); 
    //mpz_class bias;
    //bias = eMax - 1;
    const int lbias = eMax - 1;
    static_assert(lbias>0, "Bias must be positive.");
    //vhdl << tab << declare("bias",wEI) << " <= not conv_std_logic_vector(" << bias << ", "<< wEI<<");"<<endl;
    auto bias = ~ fw_uint<wEI>(lbias);

    /*Exponent_difference = new IntAdder(target, wEI);
    Exponent_difference->changeName(getName()+"Exponent_difference");
    addSubComponent(Exponent_difference);
    inPortMap  (Exponent_difference, "X", "bias");
    inPortMap  (Exponent_difference, "Y", "eA0");
    inPortMapCst(Exponent_difference, "Cin", "'1'");
    outPortMap (Exponent_difference, "R","eA1");
    vhdl << instance(Exponent_difference, "Exponent_difference");*/
    auto eA1 = bias - eA0 + fw_uint<wEI>(1);
      
    // setCycleFromSignal("eA1");
    // setCriticalPath(Exponent_difference->getOutputDelay("R"));

      
    //manageCriticalPath(target->localWireDelay() + target->lutDelay());
    //const int wShiftIn = intlog2(wFO0+2);
    const int wShiftIn = (int)ceil(log(wFO0+2));
    fw_uint<wShiftIn> shiftedby;
    if(wShiftIn < wEI){
        //vhdl << tab << declare("shiftedby", wShiftIn) <<  " <= eA1" << range(wShiftIn-1, 0)                 << " when eA1" << of(wEI-1) << " = '0' else " << rangeAssign(wShiftIn-1,0,"'0'") << ";"<<endl;
        shiftedby= select( ~get_bit<wEI-1>(eA1), get_bits<wShiftIn-1, 0>(eA1) , zg<wShiftIn>() );
    }else{
        //vhdl << tab << declare("shiftedby", wShiftIn) <<  " <= " << rangeAssign(wShiftIn-wEI,0,"'0'") << " & eA1 when eA1" << of(wEI-1) << " = '0' else " << rangeAssign(wShiftIn-1,0,"'0'") << ";"<<endl;
        shiftedby= select( ~get_bit<wEI-1>(eA1), concat(zg<wShiftIn-wEI+1>(),eA1) , zg<wShiftIn>() ); 
    }
     #error "Here"
    //FXP shifter mappings
    /*FXP_shifter = new Shifter(target, wFI+1, wFO0+2, Shifter::Left);
    addSubComponent(FXP_shifter);

    inPortMap (FXP_shifter, "X", "fA0");
    inPortMap (FXP_shifter, "S", "shiftedby");
    outPortMap (FXP_shifter, "R", "fA1");
    vhdl << instance(FXP_shifter, "FXP_shifter");

    syncCycleFromSignal("fA1");
    setCriticalPath(FXP_shifter->getOutputDelay("R"));*/
    
    fw_uint<wFO0+2> fA1 = fA0 << shiftedby.to_int();

    fw_uint<wFO> fA4;
    fw_uint<wFO+1> fA3;
    if(trunc_p)
    {
        if(!Signed)
        {
            //vhdl << tab << declare("fA4",wFO) <<  "<= fA1" << range(wFO0+wFI+LSB, wFI+1+LSB)<< ";"<<endl;
            fA4 = get_bits<wFO0+wFI+LSB, wFI+1+LSB>(fA1);
        }
        else
        {
            //vhdl << tab << declare("fA2",wFO) <<  "<= fA1" << range(wFO0+wFI+LSB, wFI+1+LSB)<< ";"<<endl;
            fw_uint<wFO> fA2 = get_bits<wFO0+wFI+LSB, wFI+1+LSB>(fA1);
            //manageCriticalPath(target->localWireDelay() + target->adderDelay(wFO));
            //vhdl << tab << declare("fA4",wFO) <<  "<= fA2 when I" << of(wEI+wFI) <<" = '0' else -signed(fA2);" <<endl;
            fA4 = select( I[wEI+wFI], fA2, (~fA2)+1 );
            assert(0); // What does -signed(.) do in this context?
        }
    }
    else
    {
        //vhdl << tab << declare("fA2a",wFO+1) <<  "<= '0' & fA1" << range(wFO0+wFI+LSB, wFI+1+LSB)<< ";"<<endl;
        fw_uint<wFO+1> fA2a = concat( zg1, get_bits(fA1, wFO0+wFI+LSB, wFI+1+LSB) );
        fw_uint<1> round;
        //IntAdder* MantSum;
        if(!Signed)
        {
            //manageCriticalPath(target->localWireDelay() + target->lutDelay());
            //vhdl << tab << declare("notallzero") << " <= '0' when fA1" << range(wFI+LSB-1, 0) << " = " << rangeAssign(wFI+LSB-1, 0,"'0'") << " else '1';"<<endl;
            fw_uint<1> notallzero = ~ (get_bits<wFI+LSB-1, 0>(fA1) == zg<wFI+LSB>());
            //vhdl << tab << declare("round") << " <= fA1" << of(wFI+LSB) << " and notallzero ;"<<endl;
            round = fA1[wFI+LSB] & notallzero;
        }
        else
        {
            //manageCriticalPath(target->localWireDelay() + target->lutDelay());
            //vhdl << tab << declare("notallzero") << " <= '0' when fA1" << range(wFI+LSB-1, 0) << " = " << rangeAssign(wFI+LSB-1, 0,"'0'") << " else '1';"<<endl;
            fw_uint<1> notallzero = ~ ( get_bits<wFI+LSB-1, 0>(fA1) == zg<wFI+LSB>() );
            //vhdl << tab << declare("round") << " <= (fA1" << of(wFI+LSB) << " and I" << of(wEI+wFI) << ") or (fA1" << of(wFI+LSB) << " and notallzero and not I" << of(wEI+wFI) << ");"<<endl;
            round = ( fA1[wFI+LSB] & I[wEI+wFI] ) | (fA1[wFI+LSB] & notallzero & ~ I[wEI+wFI]);
        }   
        //vhdl << tab << declare("fA2b",wFO+1) <<  "<= '0' & " << rangeAssign(wFO-1,1,"'0'") << " & round;"<<endl;
        fw_uint<wFO+1> fA2b = concat( zg<wFO>(), round);
        /*MantSum = new IntAdder(target, wFO+1);
        MantSum->changeName(getName()+"MantSum");
        addSubComponent(MantSum);
        inPortMap  (MantSum, "X", "fA2a");
        inPortMap  (MantSum, "Y", "fA2b");
        inPortMapCst(MantSum, "Cin", "'0'");
        outPortMap (MantSum, "R","fA3");
        vhdl << instance(MantSum, "MantSum");
        setCycleFromSignal("fA3");
        setCriticalPath(MantSum->getOutputDelay("R"));*/
        fA3 = fA2a + fA2b; 
        if(!Signed)
        {
            //vhdl << tab << declare("fA4",wFO) <<  "<= fA3" << range(wFO-1, 0)<< ";"<<endl;
            fA4 = get_bits<wFO-1,0>(fA4);
        }
        else
        {
            //manageCriticalPath(target->localWireDelay() + target->adderDelay(wFO+1));
            //vhdl << tab << declare("fA3b",wFO+1) <<  "<= -signed(fA3);" <<endl;
            fw_uint<wFO+1> fA3b = (~fA3)+1;
            //manageCriticalPath(target->localWireDelay() + target->lutDelay());
            //vhdl << tab << declare("fA4",wFO) <<  "<= fA3" << range(wFO-1, 0) << " when I" << of(wEI+wFI) <<" = '0' else fA3b" << range(wFO-1, 0) << ";" <<endl;
            fA4 = select( ~get_bit<wEI+wFI>(I), take<wFO>(fA3), take<wFO>(fA3b) );
        }
    }

    fw_uint<1> overF10;
    if (eMax > MSB)
    {
        //vhdl << tab << declare("overFl0") << "<= '1' when I" << range(wEI+wFI-1,wFI) << " > conv_std_logic_vector("<< eMax+MSB << "," << wEI << ") else I" << of(wEI+wFI+2)<<";"<<endl;
        overF10 = select( get_bits<wEI+wFI-1,wFI>(I) > fw_uint<wEI>(eMax+MSB), og1, get_bit<wEI+wFI+2>(I) );
    }
    else
    {
        //vhdl << tab << declare("overFl0") << "<= I" << of(wEI+wFI+2)<<";"<<endl;
        overF10 = get_bit<wEI+wFI+2>(I);
    }
    
    fw_uint<1> overF11;
    if(trunc_p)
    {
        if(!Signed){
            //vhdl << tab << declare("overFl1") << " <= fA1" << of(wFO0+wFI+1+LSB) << ";"<<endl;
            overF11 = get_bit<wFO0+wFI+1+LSB>(fA1);
        }else{
            //manageCriticalPath(target->localWireDelay() + target->lutDelay());
            //vhdl << tab << declare("notZeroTest") << " <= '1' when fA4 /= conv_std_logic_vector(0," << wFO <<")"<< " else '0';"<<endl;
            fw_uint<1> notZeroTest = fA4 != zg<wFO>();
            //vhdl << tab << declare("overFl1") << " <= (fA4" << of(wFO-1) << " xor I" << of(wEI+wFI) << ") and notZeroTest;"<<endl;
            overF11 = (get_bit<wFO-1>(fA4) ^ get_bit<wEI+wFI>(I)) & notZeroTest;
        }
    }
    else
    {
        //vhdl << tab << declare("overFl1") << " <= fA3" << of(wFO) << ";"<<endl;
        overF11 = get_bit<wFO>(fA3);
    }

    // manageCriticalPath(target->localWireDelay() + target->lutDelay());
    //vhdl << tab << declare("eTest") << " <= (overFl0 or overFl1);" << endl;
    fw_uint<1> eTest = overF10 | overF11;

    //manageCriticalPath(target->localWireDelay() + target->lutDelay());
    //vhdl << tab << "O <= fA4 when eTest = '0' else" << endl;
    //vhdl << tab << tab << "I" << of(wEI+wFI) << " & (" << wFO-2 << " downto 0 => not I" << of(wEI+wFI) << ");"<<endl;
    return select( eTest==0, fA4,
        concat( get_bit<wEI+wFI>(I),
            select( get_bit<wEI+wFI>(I),
                concat( og1 , zg<wFO-1>()),
                concat( zg1 , og<wFO-1>())
            )
        )
    );
};


template<bool Signed, int MSBO, int LSBO, int wEI, int wFI, bool trunc_p>
THLS_INLINE fw_uint<MSBO-LSBO+1> ref_fp2fix(const fp_flopoco<wEI,wFI> &I, int DEBUG=0)
{
    static_assert(MSBO>=LSBO, "Sanity check.");
    
    static_assert(!trunc_p, "Not tested yet.");
    static_assert(!Signed, "Not tested yet.");
    
          /* Get I/O values */
      mpfr_t i;
      mpfr_init2(i, 1+wFI);
      I.to_mpfr(i);
      //std::cerr << "FP " << printMPFR(i, 100) << std::endl;
      mpz_class svO;
      
      mpfr_t cst, tmp2;
      mpfr_init2(cst, 10000); //init to infinite prec
      mpfr_init2(tmp2, 10000); //init to infinite prec
      mpfr_set_ui(cst, 2 , GMP_RNDN);
      mpfr_set_si(tmp2, -LSBO , GMP_RNDN);
      mpfr_pow(cst, cst, tmp2, GMP_RNDN);
      mpfr_mul(i, i, cst, GMP_RNDN);

      if(trunc_p)
         mpfr_get_z(svO.get_mpz_t(), i, GMP_RNDZ);
      else
         mpfr_get_z(svO.get_mpz_t(), i, GMP_RNDN);
      
      if (Signed)
      {
        mpz_class tmpCMP = (mpz_class(1)  << (MSBO-LSBO))-1;
        if (svO > tmpCMP){ //negative number 
            mpz_class tmpSUB = (mpz_class(1) << (MSBO-LSBO+1));
            svO = svO - tmpSUB;
        }
      }
      //std::cerr << "FIX " << svO << std::endl;
      //tc->addExpectedOutput("O", svO);
      fw_uint<MSBO-LSBO+1> res(svO);
      
      // clean-up
      mpfr_clears(i,cst, tmp2, NULL);
      
      return res;
}

            
}; // thls

#endif
