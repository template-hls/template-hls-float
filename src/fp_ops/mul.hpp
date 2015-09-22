#ifndef mul_hpp
#define mul_hpp

#include "ap_int.h"

#include <cmath>

constexpr int ctMax(const int a, const int b)
{ return a>b ? a : b; }

struct raw_init_t {} raw_init;

template<int W>
struct fw_uint
{
    static const int width=W;
    
    ap_uint<W> bits;
    
    fw_uint() = default;
    fw_uint(const fw_uint &x) = default;
        
    
    // Construct 1-bit vector from boolean 
    explicit fw_uint(bool b)
        : bits(b)
    {
        assert(W==1); // Should only construct from bool with 1-bit value
    }
    
    explicit fw_uint(int v)
        : bits(v)
    {
        assert(v>=0); // must be non-negative
        assert(v < (1ll<<W)); // Must be in range
    }
        
    explicit fw_uint(const ap_uint<W> &x)
        : bits(x)
    {}
        
    explicit fw_uint(const ap_range_ref<W,false> &x)
        : bits(x)
    {}
        
        
    template<int O>
    fw_uint<ctMax(W,O)> operator+(const fw_uint<O> &o) const
    {
        ap_uint<ctMax(W,O)> a(bits), b(o.bits);
        return fw_uint<ctMax(W,O)>(ap_uint<ctMax(W,O)>(a+b));
    }
    
    fw_uint operator+(int b) const
    {
        return fw_uint(ap_uint<W>(bits+b));
    }
    
    template<int O>
    fw_uint<ctMax(W,O)> operator-(const fw_uint<O> &o) const
    {
        ap_uint<ctMax(W,O)> a(bits), b(o.bits);
        return fw_uint<ctMax(W,O)>(ap_uint<ctMax(W,O)>(a-b));
    }
    
    fw_uint operator-(int b) const
    {
        return fw_uint(ap_uint<W>(bits-b));
    }
    
    template<int O>
    fw_uint<O+W> operator*(const fw_uint<O> &o) const
    { 
        ap_uint<W+O> a(bits), b(o.bits);
        return fw_uint<O+W>(a*b);
    }
    
    fw_uint<1> operator<(const fw_uint &o) const
    { return fw_uint<1>(bits < o.bits); }
    
    fw_uint<1> operator<(int o) const
    {
        assert(o>=0);
        return fw_uint<1>(bits < o);
    }
    
    fw_uint<1> operator<=(const fw_uint &o) const
    { return fw_uint<1>(bits <= o.bits); }
    
    fw_uint<1> operator<=(int o) const
    {
        assert(o>=0);
        return fw_uint<1>(bits <= o);
    }
    
    fw_uint<1> operator==(const fw_uint &o) const
    { return fw_uint<1>(bits == o.bits); }
    
    fw_uint<1> operator==(int o) const
    {
        assert(o>=0);
        return fw_uint<1>(bits == o);
    }
    
    fw_uint<1> operator>=(const fw_uint &o) const
    { return fw_uint<1>(bits >= o.bits); }
    
    fw_uint<1> operator>=(int o) const
    {
        assert(o>=0);
        return fw_uint<1>(bits >= o);
    }
    
    fw_uint<1> operator>(const fw_uint &o) const
    { return fw_uint<1>(bits > o.bits); }
    
    fw_uint<1> operator>(int o) const
    {
        assert(o>=0);
        return fw_uint<1>(bits > o);
    }
    
    fw_uint<1> operator!=(const fw_uint &o) const
    { return fw_uint<1>(bits != o.bits); }
    
    fw_uint<1> operator!=(int o) const
    {
        assert(o>=0);
        return fw_uint<1>(bits != o);
    }
    
    //////////////////////////////////////////////
    // Bitwise ops.
    // numeric_std says result width is size of the LHS
    
    fw_uint operator~() const
    { return fw_uint(ap_uint<W>(~bits)); }
    
    template<int O>
    fw_uint operator&(const fw_uint<O> &o) const
    {
        ap_uint<W> b(o.bits);
        return fw_uint<W>(ap_uint<W>(bits&b));
    }
    
    fw_uint operator&(int b) const
    {
        assert(b>=0);
        return fw_uint(ap_uint<W>(bits&b));
    }
    
    template<int O>
    fw_uint operator|(const fw_uint<O> &o) const
    {
        ap_uint<W> b(o.bits);
        return fw_uint<W>(ap_uint<W>(bits|b));
    }
    
    fw_uint operator|(int b) const
    {
        assert(b>=0);
        return fw_uint(ap_uint<W>(bits|b));
    }
    
    template<int O>
    fw_uint operator^(const fw_uint<O> &o) const
    {
        ap_uint<W> b(o.bits);
        return fw_uint<W>(ap_uint<W>(bits^b));
    }
    
    fw_uint operator^(int b) const
    {
        assert(b>=0);
        return fw_uint(bits^b);
    }
    
    
    fw_uint operator>>(int dist) const
    {
        assert(0<=dist && dist<W);
        return fw_uint(bits>>dist);
    }
    
    std::string to_string() const
    {
        std::string acc="";
        for(int i=0;i<W;i++){
            if((0==(i%8))&&(i!=0)){
                acc="_"+acc;
            }
            acc=(bits[i]?"1":"0")+acc;
        }
        return "0b"+acc;
    }
    
    int to_int() const
    {
        return (int)bits;
    }
    
    uint64_t to_uint64() const
    { return (uint64_t)bits; }
    
    bool to_bool() const
    {
        assert(W==1);
        return bits==1;
    }
    
    explicit operator bool() const
    { return to_bool(); }
};

fw_uint<1> operator||(const fw_uint<1> &a, const fw_uint<1> &b)
{ return fw_uint<1>(ap_uint<1>(a.bits||b.bits)); }

fw_uint<1> operator&&(const fw_uint<1> &a, const fw_uint<1> &b)
{ return fw_uint<1>(ap_uint<1>(a.bits&&b.bits)); }

template<int W>
std::ostream &operator<<(std::ostream &dst, const fw_uint<W> &x)
{
    dst<<x.to_string();
    return dst;
}


template<int B,int W>
fw_uint<W-B> take(const fw_uint<W> &x)
{
    static_assert(B>=0, "Cannot take negative bits.");
    static_assert(B<=W, "Cannot take more than bits in the variable.");
    return fw_uint<W-B>(x.bits); // truncate
}



template<int ExpBits,int FracBits>
struct fp_ieee
{
    fw_uint<1+ExpBits+FracBits> bits;
};


template<int B,int W>
fw_uint<1> get_bit(const fw_uint<W> &x)
{
    return fw_uint<1>(ap_uint<1>(x.bits[B]));
}

template<int HI,int LO,int W>
fw_uint<HI-LO+1> get_bits(const fw_uint<W> &x)
{
    return fw_uint<HI-LO+1>(ap_uint<HI-LO+1>(x.bits.range(HI,LO)));
}

template<int A,int B>
fw_uint<A+B> full_mul(const fw_uint<A> &a, const fw_uint<B> &b)
{
    return a*b;
}

template<int W>
fw_uint<W> zg()
{
    return fw_uint<W>(0);
}

template<int W>
fw_uint<W> og()
{
    return ~zg<W>();
}

template<int P,int W>
fw_uint<P+W> zpad_hi(const fw_uint<W> &x)
{
    return fw_uint<P+W>(ap_uint<P+W>(  (zg<P>().bits,x.bits)  ));
}

template<int P,int W>
fw_uint<P+W> opad_hi(const fw_uint<W> &x)
{
    return fw_uint<P+W>(ap_uint<P+W>(  (og<P>().bits,x.bits)  ));
}

template<int P,int W>
fw_uint<P+W> zpad_lo(const fw_uint<W> &x)
{
    return fw_uint<P+W>(ap_uint<P+W>(  (x.bits,zg<P>().bits)  ));
}

template<int P,int W>
fw_uint<P+W> opad_lo(const fw_uint<W> &x)
{
    return fw_uint<P+W>(ap_uint<P+W>(  (x.bits,og<P>().bits)  ));
}

template<class ...C>
struct concat_impl;

template<int WF, class ...CR>
struct concat_impl<fw_uint<WF>,CR...>
{
    static const int S = WF+concat_impl<CR...>::S;
};

template<int WF>
struct concat_impl<fw_uint<WF> >
{
    static const int S = WF;
};

template<>
struct concat_impl<>
{
    static const int S = 0;
};

fw_uint<0> concat()
{
    return fw_uint<0>();
}

template<class CH, class ...CR>
fw_uint<concat_impl<CH,CR...>::S> concat(const CH &h, const CR &...r)
{
    ap_uint<CH::width> a(h.bits);
    ap_uint<concat_impl<CR...>::S> b(concat(r...).bits);
    
    return fw_uint<concat_impl<CH,CR...>::S>(
        ap_uint<concat_impl<CH,CR...>::S>(
            (a,b)
        )
    );
}


template<int ExpBits,int FracBits>
struct fp_flopoco
{
    fp_flopoco(float x)
    {
        assert(FracBits==23);
        assert(ExpBits==8);
        
        if(x==0){
            bits=zpad_lo<31>(fw_uint<3>(0b000));
        }else if(std::isinf(x)){
            if(x<0){
                bits=zpad_lo<31>(fw_uint<3>(0b101));
            }else{
                bits=zpad_lo<31>(fw_uint<3>(0b100));
            }
        }else if(std::isnan(x)){
            bits=zpad_lo<31>(fw_uint<3>(0b110));
        }else{
            fw_uint<1> s(x < 0);
            x=std::abs  (x);
            
            int e;
            float f=frexp(x, &e);
            f=ldexp(f-0.5,24);
            fw_uint<23> frac((int)f);
            fw_uint<8> expnt(e+126);
            
            bits=concat(fw_uint<2>(0b01),s,expnt,frac);
        }
    }
    
    fp_flopoco(const fw_uint<3+ExpBits+FracBits> &_bits)
        : bits(_bits)
    {}
    
    fw_uint<3+ExpBits+FracBits> bits;
        
    float to_float() const
    {
        assert(FracBits==23);
        assert(ExpBits==8);
        if(get_bits<FracBits+ExpBits+2,FracBits+ExpBits+1>(bits)==0b00){
            return 0;
        }else if(get_bits<FracBits+ExpBits+2,FracBits+ExpBits+1>(bits)==0b10){
            return get_bit<FracBits+ExpBits>(bits) ? -INFINITY : +INFINITY;
        }else if(get_bits<FracBits+ExpBits+2,FracBits+ExpBits+1>(bits)==0b11){
            return NAN;
        }else{
            fw_uint<23> frac=get_bits<FracBits-1,0>(bits);
            fw_uint<8> expnt=get_bits<ExpBits+FracBits-1,FracBits>(bits);
            
            float f=ldexp(frac.to_int(),-24)+0.5f;
            f=ldexp(f, expnt.to_int()-126);
            return get_bit<ExpBits+FracBits>(bits) ? -f : +f;
        }
    }
    
    #ifndef __SYNTHESIS__
    std::string str() const
    {
        std::stringstream acc;
        acc<<bits.to_string();
        fw_uint<2> flags=get_bits<ExpBits+FracBits+2,ExpBits+FracBits+1>(bits);
        if(flags==0b00){
            acc<<" zero";
        }else if(flags==0b01){
            acc<<" normal";
            
            fw_uint<ExpBits> expnt=get_bits<ExpBits+FracBits-1,FracBits>(bits);
            fw_uint<FracBits> frac=get_bits<FracBits-1,0>(bits);
            acc<<" expnt="<<expnt<<"="<<(expnt.to_int()-126);
            acc<<" frac="<<frac<<"="<<(ldexp(frac.to_int()+(1<<FracBits),-FracBits-1));
        }else if(flags==0b10){
            acc<<" infinity";
        }else{
            acc<<" nan";
        }
        return acc.str();
    }
    #endif
};


template<int wEX,int wFX,int wEY,int wFY, int wER=wEX,int wFR=wFX>
fp_flopoco<wER,wFR> mul(const fp_flopoco<wEX,wFX> &x, const fp_flopoco<wEY,wFY> &y)
{
    auto X=x.bits;
    auto Y=y.bits;
    
    auto sign= get_bit<wEX+wFX>(X) ^ get_bit<wEY+wFY>(Y);

    /* Exponent Handling */
    fw_uint<wEX> expX=get_bits<wEX+wFX-1,wFX>(X);
    fw_uint<wEY> expY=get_bits<wEY+wFY-1,wFY>(Y);

    //Add exponents and substract bias
	fw_uint<wEX+2> expSumPreSub= zpad_hi<2>(expX) + zpad_hi<2>(expY);
    fw_uint<wEX+2> bias( (1<<(wER-1))-1 );
    
    #ifndef __SYNTHESIS__
    std::cerr<<"expSumPreSub="<<expSumPreSub<<"="<<expSumPreSub.to_int()<<", bias="<<bias<<"="<<bias.to_int()<<"\n";
    #endif

    fw_uint<wEX+2> expSum=expSumPreSub - bias;

    /* Significand Handling */
    fw_uint<1+wFX> sigX= opad_hi<1>( get_bits<wFX-1,0>(X) );
    fw_uint<1+wFY> sigY= opad_hi<1>( get_bits<wFY-1,0>(Y) );


    const int sigProdSize=wFX+1 + wFY+1;
    fw_uint<sigProdSize> sigProd=full_mul(sigX,sigY);
    
    #ifndef __SYNTHESIS__
    assert(wFX+wFY<53); // Need to go to arbitrary precision to print
    std::cerr<<"  sigX="<<sigX<<"="<<sigX.to_uint64()<<" = "<<ldexp((double)sigX.to_uint64(),-wFX-1)<<"\n";
    std::cerr<<"  sigY="<<sigY<<"="<<sigY.to_uint64()<<" = "<<ldexp((double)sigY.to_uint64(),-wFY-1)<<"\n";
    std::cerr<<"  sigProd="<<sigProd<<"="<<sigProd.to_uint64()<<" = "<<ldexp((double)sigProd.to_uint64(),-wFX-wFY-2)<<"\n";
    #endif
		
    /* Exception Handling, assumed to be faster than both exponent and significand computations */
    fw_uint<4> excSel=concat(get_bits<wEX+wFX+2,wEX+wFX+1>(X) , get_bits<wEY+wFY+2,wEY+wFY+1>(Y));
    
    #ifndef __SYNTHESIS__
    std::cerr<<"excX="<<get_bits<wEX+wFX+2,wEX+wFX+1>(X)<<", excY="<<get_bits<wEY+wFY+2,wEY+wFY+1>(Y)<<"\n";
    #endif
    
    
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
    
    #ifndef __SYNTHESIS__
    std::cerr<<"  excSel="<<excSel<<", exc="<<exc<<"\n";
    #endif
				
	fw_uint<1> norm=get_bit<sigProdSize-1>(sigProd);
    #ifndef __SYNTHESIS__
    std::cerr<<"  norm="<<norm<<"\n";
    #endif
			
    // exponent update
    fw_uint<wEX+2> expPostNorm= expSum + zpad_hi<wEX+1>(norm);
    
    #ifndef __SYNTHESIS__
    std::cerr<<"  expPostNorm="<<expPostNorm<<" = "<<expPostNorm.to_int()<<"\n";
    #endif

	//  exponent update is in parallel to the mantissa shift, so get back there
    fw_uint<3+wER+wFR> R;

    //check is rounding is needed
    if (1+wFR >= wFX+wFY+2) {  
        /* => no rounding needed - possible padding; 
           in this case correctlyRounded_ is irrelevant: result is exact  */
        fw_uint<wFR> resSig = norm ? concat(get_bits<wFX+wFY,0>(sigProd) , zg<1+wFR-(wFX+wFY+2)>())
                                   : concat(get_bits<wFX+wFY-1,0>(sigProd) , zg<1+wFR-(wFX+wFY+2)+1>());

        fw_uint<2> expPostNormB = get_bits<wER+1,wER>(expPostNorm);
        fw_uint<2> excPostNorm = (expPostNormB==0b00).to_bool() ? fw_uint<2>(0b01) :
                                 (expPostNormB==0b01).to_bool() ? fw_uint<2>(0b10) :
                                 ((expPostNormB==0b11) || (expPostNormB==0b10)).to_bool() ? fw_uint<2>(0b11) :
                                 fw_uint<2>(0b11); // Impossible last case?
        
        fw_uint<2> finalExc=(exc==0b11||exc==0b10||exc==0b00).to_bool() ? exc : excPostNorm;
        
        R = concat(finalExc,sign, get_bits<wER-1,0>(expPostNorm), resSig);
    }
    else{
        // significand normalization shift
        fw_uint<sigProdSize> sigProdExt = norm==1 ? concat(get_bits<sigProdSize-2,0>(sigProd) , zg<1>())
                                                  : concat(get_bits<sigProdSize-3,0>(sigProd) , zg<2>());

        fw_uint<2+wER+wFR> expSig = concat(expPostNorm , get_bits<sigProdSize-1,sigProdSize-wFR>(sigProdExt));
        
        fw_uint<1> sticky = get_bit<wFX+wFY+1-wFR>(sigProdExt);
        
        fw_uint<1> guard = get_bits<wFX+wFY+1-wFR-1,0>(sigProdExt) == zg<wFX+wFY+1-wFR-1+1>() ? fw_uint<1>(0) : fw_uint<1>(1);
        
        fw_uint<1> round = sticky & ( (guard & ~get_bit<wFX+wFY+1-wFR+1>(sigProdExt))
            | (get_bit<wFX+wFY + 1 - wFR+1>(sigProdExt) ));
        
        #ifndef __SYNTHESIS__
        std::cerr<<"  round="<<round<<"\n";
        #endif
    
        fw_uint<2+wER+wFR> expSigPostRound = expSig + zpad_hi<2+wER+wFR-1>(round);
        
        fw_uint<2> expSigPostRoundB=get_bits<wER+wFR+1,wER+wFR>(expSigPostRound);
        
        fw_uint<2> excPostNorm = (expSigPostRoundB==0b00).to_bool() ? fw_uint<2>(0b01) :
                                 (expSigPostRoundB==0b01).to_bool() ? fw_uint<2>(0b10) :
                                 (expSigPostRoundB==0b11||expSigPostRoundB==0b10).to_bool() ? fw_uint<2>(0b00) :
                                 fw_uint<2>(0b11);
                                 
        fw_uint<2> finalExc=(exc==0b11||exc==0b10||exc==0b00).to_bool() ? exc : excPostNorm;

        R = concat(finalExc,sign, get_bits<wER+wFR-1,0>(expSigPostRound));
    }
    
    return fp_flopoco<wER,wFR>(R);
}

#endif
