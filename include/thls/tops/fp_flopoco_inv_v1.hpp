/*
    Based on:

      Floating Point Divider for FloPoCo


      This file is part of the FloPoCo project
      developed by the Arenaire team at Ecole Normale Superieure de Lyon

      Authors: Jeremie Detrey, Florent de Dinechin

      Initial software.
      Copyright © ENS-Lyon, INRIA, CNRS, UCBL,
      2008-2010.
      All rights reserved.

 */

#ifndef thls_fp_flopoco_inv_v1_hpp
#define thls_fp_flopoco_inv_v1_hpp

#include "thls/tops/fp_flopoco.hpp"
#include "thls/tops/fp_promote.hpp"

namespace thls
{

template<int wER,int wFR, int wEY,int wFY>
THLS_INLINE fp_flopoco<wER,wFR> inv_v1(const fp_flopoco<wEY,wFY> &y, int DEBUG)
{
    THLS_STATIC_ASSERT(wEY==wER, "Result exp must match promotion of args.");
    THLS_STATIC_ASSERT(wFY==wFR, "Result frac must match promotion of args.");

    static const int wE=wEY;
    static const int wF=wFY;

    auto X=concat(cg<4>(0100),og<wE-1>(),zg<wF>());
    auto Y=y.bits;


    // -------- Parameter set up -----------------
    static const int nDigit = (wF+6) >> 1;

    // vhdl << tab << declare("fX",wF+1) << " <= \"1\" & X(" << wF-1 << " downto 0);" << endl;
    auto fX = opad_hi<1>(take_lsbs<wF>(X));
    //vhdl << tab << declare("fY",wF+1) << " <= \"1\" & Y(" << wF-1 << " downto 0);" << endl;
    auto fY = opad_hi<1>(take_lsbs<wF>(Y));

    //vhdl << tab << "-- exponent difference, sign and exception combination computed early, to have less bits to pipeline" << endl;

    //vhdl << tab << declare("expR0", wE+2) << " <= (\"00\" & X(" << wE+wF-1 << " downto " << wF << ")) - (\"00\" & Y(" << wE+wF-1 << " downto " << wF<< "));" << endl;
    auto expR0 = zpad_hi<2>(get_bits<wE+wF-1,wF>(X)) - zpad_hi<2>(get_bits<wE+wF-1,wF>(Y));
    if(DEBUG){
        std::cerr<<"  expR0 = "<<expR0<<"\n";
    }
    //vhdl << tab << declare("sR") << " <= X(" << wE+wF << ") xor Y(" << wE+wF<< ");" << endl;
    auto sR = get_bit<wE+wF>(X) ^ get_bit<wE+wF>(Y);
    //vhdl << tab << "-- early exception handling " <<endl;
    //vhdl << tab << declare("exnXY",4) << " <= X(" << wE+wF+2 << " downto " << wE+wF+1  << ") & Y(" << wE+wF+2 << " downto " << wE+wF+1 << ");" <<endl;
    auto exnXY = concat(get_bits<wE+wF+2,wE+wF+1>(X) , get_bits<wE+wF+2,wE+wF+1>(Y) );
    if(DEBUG){
        std::cerr<<"  exnXY = "<<exnXY<<"\n";
    }
    /*vhdl << tab << "with exnXY select" <<endl;
    vhdl << tab << tab << declare("exnR0", 2) << " <= " << endl;
    vhdl << tab << tab << tab << "\"01\"  when \"0101\",                   -- normal" <<endl;
    vhdl << tab << tab << tab << "\"00\"  when \"0001\" | \"0010\" | \"0110\", -- zero" <<endl;
    vhdl << tab << tab << tab << "\"10\"  when \"0100\" | \"1000\" | \"1001\", -- overflow" <<endl;
    vhdl << tab << tab << tab << "\"11\"  when others;                   -- NaN" <<endl;*/
    auto exnR0 = select(
        exnXY==0b0101,                                      cg<2>(0b01), // normal
        exnXY==0b0001 || exnXY==0b0010 || exnXY==0b0110,    cg<2>(0b00), // zero
        exnXY==0b0100 || exnXY==0b1000 || exnXY==0b1001,    cg<2>(0b10), // overflow
        /* else */                                          cg<2>(0b11)  // NaN
    );
    if(DEBUG){
        std::cerr<<"  exnR0 = "<<exnR0<<"\n";
    }

    //vhdl << tab << " -- compute 3Y" << endl;
    //vhdl << tab << declare("fYTimes3",wF+3) << " <= (\"00\" & fY) + (\"0\" & fY & \"0\");" << endl; // TODO an IntAdder here
    auto fYTimes3 = zpad_hi<2>(fY) + concat(zg1, fY, zg1);


    //////////////////////////////////////////////////////////////////////////////
    // Division loop
    //
    // Range: i=nDigit-1 .. 1
    //
    // Inputs:
    //   w_i        : 3+wF
    // Calculated:
    //   sel_i      : 5
    //   q_i        : 3
    //   qiTimesD_i : 4+wF
    //   wipad_i    : 4+wF
    //   wim1full_im1: 4+wF
    //   wim1_im1   : wF+3

    fw_uint<3+wF>   w[nDigit];
    fw_uint<5>      sel[nDigit];
    fw_uint<3>      q[nDigit];
    fw_uint<4+wF>   qTimesD[nDigit];
    fw_uint<4+wF>   wpad[nDigit];
    fw_uint<4+wF>   wfull[nDigit];

    //ostringstream wInit;
    //wInit << "w"<<nDigit-1;
    //vhdl << tab << declare(wInit.str(), wF+3) <<" <=  \"00\" & fX;" << endl;
    w[nDigit-1] = zpad_hi<2>(fX);

    l1 : for(int i=nDigit-1; i>=1; i--) {
#pragma HLS UNROLL
        /*
        wi << "w" << i;
        qi << "q" << i;
        wim1 << "w" << i-1;
        seli << "sel" << i;
        qiTimesD << "q" << i << "D";
        wipad << "w" << i << "pad";
        wim1full << "w" << i-1 << "full";
        */

        //vhdl << tab << declare(seli.str(),5) << " <= " << wi.str() << range( wF+2, wF-1)<<" & fY"<<of(wF-1)<<";" << endl;
        sel[i] = concat( get_bits<wF+2,wF-1>(w[i]), get_bit<wF-1>(fY) );
        /*vhdl << tab << "with " << seli.str() << " select" << endl;
        vhdl << tab << declare(qi.str(),3) << " <= " << endl;
        vhdl << tab << tab << "\"001\" when \"00010\" | \"00011\"," << endl;
        vhdl << tab << tab << "\"010\" when \"00100\" | \"00101\" | \"00111\"," << endl;
        vhdl << tab << tab << "\"011\" when \"00110\" | \"01000\" | \"01001\" | \"01010\" | \"01011\" | \"01101\" | \"01111\"," << endl;
        vhdl << tab << tab << "\"101\" when \"11000\" | \"10110\" | \"10111\" | \"10100\" | \"10101\" | \"10011\" | \"10001\"," << endl;
        vhdl << tab << tab << "\"110\" when \"11010\" | \"11011\" | \"11001\"," << endl;
        vhdl << tab << tab << "\"111\" when \"11100\" | \"11101\"," << endl;
        vhdl << tab << tab << "\"000\" when others;" << endl;*/
        if(0){
			static const lut<3,5> q_i_lut([](int i) -> int {
				switch(i){
					case 0b00010: case 0b00011:
						return 0b001;
					case 0b00100: case 0b00101: case 0b00111:
						return 0b010;
					case 0b00110: case 0b01000: case 0b01001: case 0b01010: case 0b01011: case 0b01101: case 0b01111:
						return 0b011;
					case 0b11000: case 0b10110: case 0b10111: case 0b10100: case 0b10101: case 0b10011: case 0b10001:
						return 0b101;
					case 0b11010: case 0b11011: case 0b11001:
						return 0b110;
					case 0b11100: case 0b11101:
						return 0b111;
					default:
						return 0b000;
				}
			});
			q[i]=q_i_lut(sel[i]);
        }else{
        	int qi_tmp;
        	switch(sel[i].to_int()){
				case 0b00010: case 0b00011:
					qi_tmp=0b001; break;
				case 0b00100: case 0b00101: case 0b00111:
					qi_tmp=0b010; break;
				case 0b00110: case 0b01000: case 0b01001: case 0b01010: case 0b01011: case 0b01101: case 0b01111:
					qi_tmp=0b011; break;
				case 0b11000: case 0b10110: case 0b10111: case 0b10100: case 0b10101: case 0b10011: case 0b10001:
					qi_tmp=0b101; break;
				case 0b11010: case 0b11011: case 0b11001:
					qi_tmp=0b110; break;
				case 0b11100: case 0b11101:
					qi_tmp=0b111; break;
				default:
					qi_tmp=0b000;
			}
        	q[i]=fw_uint<3>(qi_tmp);
        }


        /*vhdl << tab << "with " << qi.str() << " select" << endl;
        vhdl << tab << tab << declare(qiTimesD.str(),wF+4) << " <= "<< endl ;
        vhdl << tab << tab << tab << "\"000\" & fY            when \"001\" | \"111\"," << endl;
        vhdl << tab << tab << tab << "\"00\" & fY & \"0\"     when \"010\" | \"110\"," << endl;
        vhdl << tab << tab << tab << "\"0\" & fYTimes3             when \"011\" | \"101\"," << endl;
        vhdl << tab << tab << tab << "(" << wF+3 << " downto 0 => '0') when others;" << endl;*/
        qTimesD[i] = select(
            q[i]==0b001 || q[i]==0b111,   zpad_hi<3>(fY),
            q[i]==0b010 || q[i]==0b110,   concat(zg2,fY,zg1),
            q[i]==0b011 || q[i]==0b101,   zpad_hi<1>(fYTimes3),
            /* else */                    zg<wF+4>()
        );

        //vhdl << tab << declare(wipad.str(), wF+4) << " <= " << wi.str() << " & \"0\";" << endl;
        wpad[i] = zpad_lo<1>(w[i]);
        /*vhdl << tab << "with " << qi.str() << "(2) select" << endl;
        vhdl << tab << declare(wim1full.str(), wF+4) << "<= " << wipad.str() << " - " << qiTimesD.str() << " when '0'," << endl;
        vhdl << tab << "      " << wipad.str() << " + " << qiTimesD.str() << " when others;" << endl;*/
        if(1){
			wfull[i-1] = select(
				get_bit<2>(q[i])==0b0,   wpad[i] - qTimesD[i],
				/* else */               wpad[i] + qTimesD[i]
			);
        }else{
        	auto neg=get_bit<2>(q[i])==0b0;
        	wfull[i-1] = add_with_cin(wpad[i] , select(neg,~qTimesD[i],qTimesD[i]), neg);

        }

        //vhdl << tab << declare(wim1.str(),wF+3) << " <= " << wim1full.str()<<range(wF+1,0)<<" & \"0\";" << endl;
        w[i-1] = zpad_lo<1>( get_bits<wF+1,0>(wfull[i-1]) );
    }

    /*vhdl << tab << declare("q0",3) << "(2 downto 0) <= \"000\" when  w0 = (" << wF+2 << " downto 0 => '0')" << endl;
    vhdl << tab << "             else w0(" << wF+2 << ") & \"10\";" << endl;*/
    q[0] = select(
        get_bits<wF+2,0>(w[0])==zg<wF+3>(), cg<3>(0b000),
        /* else */                          concat( get_bit<wF+2>(w[0]), cg<2>(0b10) )
    );

    //
    // We've calculated:
    //   w[nDigits-1]..w[0]
    //   q[nDigits-1]..q[0]
    //
    ////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////
    // Now calculating:
    //   qP[nDigit-1]..qP[0]
    //   qM[nDigit-1]..qM[0]

    fw_uint<2> qP[nDigit];
    fw_uint<2> qM[nDigit];

    for(int i=nDigit-1; i>=1; i--) {
#pragma HLS UNROLL
        /*ostringstream qi, qPi, qMi;
        qi << "q" << i;
        qPi << "qP" << i;
        qMi << "qM" << i;*/
        //vhdl << tab << declare(qPi.str(), 2) <<" <=      " << qi.str() << "(1 downto 0);" << endl;
        qP[i] = take_lsbs<2>(q[i]);
        //vhdl << tab << declare(qMi.str(), 2)<<" <=      " << qi.str() << "(2) & \"0\";" << endl;
        qM[i] = zpad_lo<1>(take_msb(q[i]));
    }

    //vhdl << tab << declare("qP0", 2) << " <= q0(1 downto 0);" << endl;
    qP[0] = take_lsbs<2>(q[0]);
    //vhdl << tab << declare("qM0", 2) << " <= q0(2)  & \"0\";" << endl;
    qM[0] = zpad_lo<1>(take_msb(q[0]));

    // Done:
    //   qP[nDigit-1]..qP[0]
    //   qM[nDigit-1]..qM[0]
    //
    ////////////////////////////////////////////////////////

    /*vhdl << tab << declare("qP", 2*nDigit) << " <= qP" << nDigit-1;
    for (i=nDigit-2; i>=0; i--)
        vhdl << " & qP" << i;
    vhdl << ";" << endl;*/
    fw_uint<2*nDigit> qPcat = concat_array<nDigit-1,0>(qP);

    /*vhdl << tab << declare("qM", 2*nDigit) << " <= qM" << nDigit-1 << "(0)";
    for (i=nDigit-2; i>=0; i--)
        vhdl << " & qM" << i;
    vhdl << " & \"0\";" << endl;*/
    fw_uint<2*nDigit> qMcat = concat(
        get_bit<0>(qM[nDigit-1]),
        concat_array<nDigit-2,0>(qM),
        zg1
    );

    if(DEBUG){
        std::cerr<<"  qPcat = "<<qPcat<<"\n";
        std::cerr<<"  qMcat = "<<qMcat<<"\n";
    }


    // TODO an IntAdder here
    //vhdl << tab << declare("fR0", 2*nDigit) << " <= qP - qM;" << endl;
    auto fR0 = qPcat - qMcat;
    if(DEBUG){
        std::cerr<<"  fR0 = "<<fR0<<"\n";
    }

    /*vhdl << tab << declare("fR", wF+4) << " <= ";
    if (1 == (wF & 1) ) // odd wF
        vhdl << "fR0(" << 2*nDigit-1 << " downto 1);  -- odd wF" << endl;
    else
        vhdl << "fR0(" << 2*nDigit-1 << " downto 3)  & (fR0(2) or fR0(1));  -- even wF, fixing the round bit" << endl;*/
    fw_uint<4+wF> fR;
    if( 1==(wF&1) ){
        fR=checked_cast<4+wF>( get_bits<2*nDigit-1,1>(fR0) ); // odd wF
    }else{
        fR=checked_cast<4+wF>( concat( get_bits<2*nDigit-1,3>(fR0) , get_bit<2>(fR0)|get_bit<1>(fR0) ) );
    }
    if(DEBUG){
        std::cerr<<"  fR = "<<fR<<"\n";
    }


    //vhdl << tab << "-- normalisation" << endl;
    /*vhdl << tab << "with fR(" << wF+3 << ") select" << endl;
    vhdl << tab << tab << declare("fRn1", wF+2) << " <= fR(" << wF+2 << " downto 2) & (fR(1) or fR(0)) when '1'," << endl;
    vhdl << tab << tab << "        fR(" << wF+1 << " downto 0)                    when others;" << endl;*/
    auto fRn1 = select(
        get_bit<wF+3>(fR),      concat( get_bits<wF+2,2>(fR) , get_bit<1>(fR)|get_bit<0>(fR) ),
        /* else */              get_bits<wF+1,0>(fR)
    );
    if(DEBUG){
        std::cerr<<"  fRn1 = "<<fRn1<<"\n";
    }

    /*vhdl << tab << declare("expR1", wE+2) << " <= expR0"
          << " + (\"000\" & (" << wE-2 << " downto 1 => '1') & fR(" << wF+3 << ")); -- add back bias" << endl;*/
    fw_uint<wE+2> expR1 = expR0 + concat( zg3 , og<wE-2>() , get_bit<wF+3>(fR) );
    if(DEBUG){
        std::cerr<<"  expR1 = "<<expR1<<"\n";
    }


    //vhdl << tab << declare("round") << " <= fRn1(1) and (fRn1(2) or fRn1(0)); -- fRn1(0) is the sticky bit" << endl;
    auto round = get_bit<1>(fRn1) & (get_bit<2>(fRn1) | get_bit<0>(fRn1));
    if(DEBUG){
        std::cerr<<"  round = "<<round<<"\n";
    }

    //vhdl << tab << "-- final rounding" <<endl;
    /*vhdl << tab <<  declare("expfrac", wE+wF+2) << " <= "
         << "expR1 & fRn1(" << wF+1 << " downto 2) ;" << endl;*/
    auto expfrac = concat(expR1 , get_bits<wF+1,2>(fRn1) );
    if(DEBUG){
        std::cerr<<"  expfrac = "<<expfrac<<"\n";
    }

    /*vhdl << tab << declare("expfracR", wE+wF+2) << " <= "
         << "expfrac + ((" << wE+wF+1 << " downto 1 => '0') & round);" << endl;*/
    auto expfracR = expfrac + concat( zg<wE+wF+1>() , round );
    /*vhdl << tab <<  declare("exnR", 2) << " <=      \"00\"  when expfracR(" << wE+wF+1 << ") = '1'   -- underflow" <<endl;
    vhdl << tab << "        else \"10\"  when  expfracR(" << wE+wF+1 << " downto " << wE+wF << ") =  \"01\" -- overflow" <<endl;
    vhdl << tab << "        else \"01\";      -- 00, normal case" <<endl;*/
    auto exnR = select(
        get_bit<wE+wF+1>(expfracR)==0b1,            cg<2>(0b00),    // underflow
        get_bits<wE+wF+1,wE+wF>(expfracR)==0b01,    cg<2>(0b10),    // overflow
        /* else */                                  cg<2>(0b01)     // normal
    );
    if(DEBUG){
        std::cerr<<"  exnR = "<<exnR<<"\n";
    }



    /*vhdl << tab << "with exnR0 select" <<endl;
    vhdl << tab << tab << declare("exnRfinal", 2) << " <= " <<endl;
    vhdl << tab << tab << tab << "exnR   when \"01\", -- normal" <<endl;
    vhdl << tab << tab << tab << "exnR0  when others;" <<endl; */
    auto exnRfinal = select(
        exnR0==0b01,    exnR,   // normal
        /* else */      exnR0
    );
    if(DEBUG){
        std::cerr<<"  exnRfinal = "<<exnRfinal<<"\n";
    }

    /*vhdl << tab << "R <= exnRfinal & sR & "
         << "expfracR(" << wE+wF-1 << " downto 0);" <<endl;*/
    auto R = concat(exnRfinal, sR , get_bits<wE+wF-1,0>(expfracR));

    return fp_flopoco<wER,wFR>(R);
}

}; // thls

#endif
