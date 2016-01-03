#ifndef thls_fp_flopoco_hpp
#define thls_fp_flopoco_hpp

#include <thls/tops/fw_uint.hpp>

#ifndef THLS_SYNTHESIS
#include <mpfr.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#endif

#include <limits>

namespace thls
{

template<int ExpBits,int FracBits>
struct fp_flopoco
{
    enum{ exp_bits = ExpBits };
    enum{ frac_bits = FracBits };

    THLS_CONSTEXPR fp_flopoco()
        : bits()
    {}

    THLS_CONSTEXPR fp_flopoco(const fw_uint<3+ExpBits+FracBits> &_bits)
        : bits(_bits)
    {}

#ifndef THLS_SYNTHESIS
    // If allowUnderOrOverflow is on, then exponents out of range
    // will be flushed to zero or infinity.
    // Number of bits in number must _always_ match FracBits
    fp_flopoco(mpfr_t x, bool allowUnderOrOverflow=false);
        
    
    void get_exponent(int &e) const;
    void get_fraction(mpfr_t &dst) const;
 
    // The destination must have the same fractional width
    void get(mpfr_t dst) const;

    // Allows for rounding while extracting
    void get(mpfr_t dst, mpfr_rnd_t mode) const;
#endif


    fw_uint<3+ExpBits+FracBits> bits;

    fw_uint<2> get_flags() const
    { return get_bits<ExpBits+FracBits+2,ExpBits+FracBits+1>(bits); }

    fw_uint<1> get_sign() const
    { return get_bit<ExpBits+FracBits>(bits); }

    fw_uint<ExpBits> get_exp_bits() const
    { return get_bits<ExpBits+FracBits-1,FracBits>(bits); }

    fw_uint<FracBits> get_frac_bits() const
    { return get_bits<FracBits-1,0>(bits); }
    
    //! Gets the concatenation of the exp(high) and frac(lo) bits
    fw_uint<ExpBits+FracBits> get_exp_frac_bits() const
    { return get_bits<ExpBits+FracBits-1,0>(bits); }
    
    //! Gets the concatenation of the flags(high), exp(mid) and frac(lo) bits
    /*! This is useful for getting a total ordering on numbers of the same
        sign, but you need to be careful about the multiple zeros and multiple
        infinities.
    */
    fw_uint<2+ExpBits+FracBits> get_flags_exp_frac_bits() const
    { return concat(get_flags(),get_exp_frac_bits()); }
    
    /*! Returns a code such that there is a total ordering on classes (NaN,-Inf,-Norm,Zero,+Norm,+Inf) */
    fw_uint<3> get_equality_class() const
    {
        auto sf=get_bits<ExpBits+FracBits+2,ExpBits+FracBits>(bits);
        return select(
            (sf==0b110) || (sf==0b111),
                cg<3>(0), // NaN
            (sf==0b101),
                cg<3>(1), // -Inf
            (sf==0b011),
                cg<3>(2), // -Norm
            (sf==0b001) || (sf==0b000),
                cg<3>(3), // Zero
            (sf==0b010),
                cg<3>(4), // +Norm
            // (sf==0b100),
                cg<3>(5)  // +Inf
        );
    }

    fw_uint<1> is_zero() const
    { return get_flags()==fw_uint<2>(0b00); }

    fw_uint<1> is_normal() const
    { return get_flags()==fw_uint<2>(0b01); }

    fw_uint<1> is_inf() const
    { return get_flags()==fw_uint<2>(0b10); }

    fw_uint<1> is_nan() const
    { return get_flags()==fw_uint<2>(0b11); }

    fw_uint<1> is_positive() const
    { return get_sign()==fw_uint<1>(0b0); }

    fw_uint<1> is_negative() const
    { return get_sign()==fw_uint<1>(0b1); }


    fw_uint<1> is_pos_normal() const
    { return is_positive() && is_normal(); }

    fw_uint<1> is_neg_normal() const
    { return is_negative() && is_normal(); }

    fw_uint<1> is_pos_zero() const
    { return is_positive() && is_zero(); }

    fw_uint<1> is_neg_zero() const
    { return is_negative() && is_zero(); }

    fw_uint<1> is_pos_inf() const
    { return is_positive() && is_inf(); }

    fw_uint<1> is_neg_inf() const
    { return is_negative() && is_inf(); }


    #ifndef THLS_SYNTHESIS
    double to_double_approx() const;
    std::string str() const;
    #endif

    //! Does a floating-point specific comparison
    /*! \note This considers all nans to be equal, and considers -0 and
        +0 to be non-equal. It is intended to check
        that two numbers represent the same thing, not for use in maths. */
    fw_uint<1> equals(const fp_flopoco &o) const
    {
        return select(
            get_flags()!=o.get_flags(),
                zg<1>(),
            is_nan(),
                o.is_nan(), // Any nan equals any other nan
            get_sign()!=o.get_sign(),
                zg<1>(),
            is_normal(),
                get_exp_bits()==o.get_exp_bits() && get_frac_bits()==o.get_frac_bits(),
                og<1>()                
        );
    }
};

}; // thls

namespace std
{
    template<int ExpBits,int FracBits>
    class numeric_limits<thls::fp_flopoco<ExpBits,FracBits> >
    {
        typedef thls::fp_flopoco<ExpBits,FracBits> T;
    public:
        static const bool is_specialized = true;
        static const bool is_signed = true;
        static const bool is_integer = false;
        static const bool is_exact = false;
        static const bool has_infinity = true;
        static const bool has_quiet_NaN = true;
        static const bool has_signaling_NaN = false;
        static const bool has_denorm = std::denorm_absent;
        static const bool has_denorm_loss = true; // ?
        static const bool round_style = std::round_to_nearest;
        static const bool is_iec559 = false;
        static const bool is_bounded = true;
        static const bool is_modulo = false;
        static const int digits = FracBits+1;

        // TODO
        //static const int digits10 = (int)(FracBits * 0.30102999566398119521373889472449-0.5); // Conservative?
        //static const int max_digits10 = (int)(FracBits * 0.30102999566398119521373889472449+0.5+2); // Correct?

        static const int radix = 2;

        static const int bias = (1<<(ExpBits-1))-1;

        static const int min_exponent = 0-bias;
        static const int max_exponent = 0+bias+1;

        //one more than the smallest negative power of the radix that is a valid normalized floating-point value
        // static const int min_exponent10 = TODO;
        // one more than the largest integer power of the radix that is a valid finite floating-point value
        // static const int max_exponent10 = TODO;
        // the largest integer power of 10 that is a valid finite floating-point value

        static const bool traps = false;

        // identifies floating-point types that detect tinyness before rounding
        // static const bool tinyness_before = TODO;

        static THLS_CONSTEXPR T min()
        {
            return T(concat(thls::fw_uint<3>(0b010),thls::zg<ExpBits>(),thls::zg<FracBits>()));
        }

        // Not in std
        static THLS_CONSTEXPR T neg_min()
        {
            return T(concat(thls::fw_uint<3>(0b011),thls::zg<ExpBits>(),thls::zg<FracBits>()));
        }

        static THLS_CONSTEXPR T max()
        {
            return T(concat(thls::fw_uint<3>(0b010),thls::og<ExpBits>(),thls::og<FracBits>()));
        }
        
        // Not in std
        static THLS_CONSTEXPR T neg_max()
        {
            return T(concat(thls::fw_uint<3>(0b011),thls::og<ExpBits>(),thls::og<FracBits>()));
        }

        static THLS_CONSTEXPR T lowest()
        { // == -max()
            return T(concat(thls::fw_uint<3>(0b011),thls::og<ExpBits>(),thls::og<FracBits>()));
        }

        static THLS_CONSTEXPR T epsilon()
        { // == 2^-(FracBits+1)
          // expnt == ExpBits -FracBits-1
            assert(0); // not tested
            return T(concat(thls::fw_uint<3>(0b010),thls::fw_uint<ExpBits>( bias - FracBits-1) ,thls::zg<FracBits>()));
        }

        static THLS_CONSTEXPR T round_error()
        { // == 0.5
            assert(0); // not tested
            return T(concat(thls::fw_uint<3>(0b010),thls::fw_uint<ExpBits>( bias - 1 ) ,thls::zg<FracBits>()));
        }

        static THLS_CONSTEXPR T infinity()
        {
            return T(concat(thls::fw_uint<3>(0b100),thls::zg<ExpBits>() ,thls::zg<FracBits>()));
        }

        // NOTE: not part of the standard numeric_limits
        static THLS_CONSTEXPR T neg_infinity()
        {
            return T(concat(thls::fw_uint<3>(0b101),thls::zg<ExpBits>() ,thls::zg<FracBits>()));
        }

        static THLS_CONSTEXPR T quiet_NaN()
        {
            return T(concat(thls::fw_uint<3>(0b110),thls::zg<ExpBits>() ,thls::zg<FracBits>()));
        }

        static THLS_CONSTEXPR T signalling_NaN()
        {
            // Just return zero
            return T(concat(thls::fw_uint<3>(0b000),thls::zg<ExpBits>() ,thls::zg<FracBits>()));
        }

        static THLS_CONSTEXPR T denorm_min()
        {
            return min();
        }

        // Not part of std
        static THLS_CONSTEXPR T pos_zero()
        {
            return T(concat(thls::fw_uint<3>(0b000),thls::zg<ExpBits>() ,thls::zg<FracBits>()));
        }

        // Not part of std
        static THLS_CONSTEXPR T neg_zero()
        {
            return T(concat(thls::fw_uint<3>(0b001),thls::zg<ExpBits>() ,thls::zg<FracBits>()));
        }
        
        // Not part of std
        static THLS_CONSTEXPR T zero()
        { return pos_zero(); }
        
        // Not part of std
        static THLS_CONSTEXPR T pos_one()
        {
            return T(concat(thls::fw_uint<3>(0b010),thls::fw_uint<ExpBits>(bias),thls::zg<FracBits>()));
        }

        // Not part of std
        static THLS_CONSTEXPR T neg_one()
        {
            return T(concat(thls::fw_uint<3>(0b011),thls::fw_uint<ExpBits>(bias),thls::zg<FracBits>()));
        }
        
        // Not part of std
        static THLS_CONSTEXPR T one()
        { return pos_one(); }
    };
};

namespace thls{

template<int ExpBits,int FracBits>
fp_flopoco<ExpBits,FracBits> nextup(const fp_flopoco<ExpBits,FracBits> &x)
{
    typedef std::numeric_limits<fp_flopoco<ExpBits,FracBits>> traits;
    typedef fp_flopoco<ExpBits,FracBits> fp_t;

    return select(
        x.is_pos_inf(),
            x,
        x.is_pos_normal(),
            select(x.get_frac_bits()==og<FracBits>(),
                select(x.get_exp_bits()==og<ExpBits>(),
                    traits::infinity(),
                    fp_t(concat(fw_uint<3>(0b010),x.get_exp_bits()+1,zg<FracBits>()))
                ),
                fp_t(concat(fw_uint<3>(0b010),x.get_exp_bits(),x.get_frac_bits()+1))
            ),
        x.is_pos_zero(),
            traits::min(),
        x.is_neg_zero(),
            traits::pos_zero(),
        x.is_neg_normal(),
            select(x.get_frac_bits()==zg<FracBits>(),
                select(x.get_exp_bits()==zg<ExpBits>(),
                    traits::neg_zero(),
                    fp_t(concat(fw_uint<3>(0b011),x.get_exp_bits()-1,og<FracBits>()))
                ),
                fp_t(concat(fw_uint<3>(0b011),x.get_exp_bits(),x.get_frac_bits()-1))
            ),
        x.is_neg_inf(),
            traits::lowest(),
        // else
            traits::quiet_NaN() // x==nan
    );
}

template<int ExpBits,int FracBits>
fp_flopoco<ExpBits,FracBits> nextdown(const fp_flopoco<ExpBits,FracBits> &x)
{
    typedef std::numeric_limits<fp_flopoco<ExpBits,FracBits>> traits;
    typedef fp_flopoco<ExpBits,FracBits> fp_t;

    return select(
        x.is_pos_inf(),
            traits::max(),
        x.is_pos_normal(),
            select(x.get_frac_bits()==zg<FracBits>(),
                select(x.get_exp_bits()==zg<ExpBits>(),
                    traits::pos_zero(),
                    fp_t(concat(fw_uint<3>(0b010),x.get_exp_bits()-1,zg<FracBits>()))
                ),    
                fp_t(concat(fw_uint<3>(0b010),x.get_exp_bits(),x.get_frac_bits()-1))
            ),
        x.is_pos_zero(),
            traits::neg_zero(),
        x.is_neg_zero(),
            traits::neg_min(),
        x.is_neg_normal(),
            select(x.get_frac_bits()==og<FracBits>(),
                select(x.get_exp_bits()==og<ExpBits>(),
                    traits::neg_infinity(),
                    fp_t(concat(fw_uint<3>(0b011),x.get_exp_bits()+1,og<FracBits>()))
                ),
                fp_t(concat(fw_uint<3>(0b011),x.get_exp_bits(),x.get_frac_bits()+1))
            ),
        x.is_neg_inf(),
            x,
        // else
            traits::quiet_NaN()
    );
}



template<int ExpBits,int FracBits>
fp_flopoco<ExpBits,FracBits> nextafter(const fp_flopoco<ExpBits,FracBits> &x, double y)
{
    if(y==INFINITY){
        return nextup(x);
    }else if(y==-INFINITY){
        return nextdown(x);
    }else{
        #ifndef THLS_SYNTHESIS
        throw std::runtime_error("arbitrary target not implemented, must be -INF or +INF.");
        #else
        assert(0);
        return x;
        #endif
    }

}

#ifndef THLS_SYNTHESIS
template<int ExpBits,int FracBits>
fp_flopoco<ExpBits,FracBits>::fp_flopoco(mpfr_t x, bool allowUnderOrOverflow)
{
    typedef std::numeric_limits<fp_flopoco> traits;

    if(mpfr_get_prec(x)!=FracBits+1){
        throw std::runtime_error("Incorrect number of bits in mpfr input.");
    }

    if(mpfr_nan_p(x)){
        bits=std::numeric_limits<fp_flopoco>::quiet_NaN().bits;
    }else if(mpfr_inf_p(x)){
        bits = (mpfr_sgn(x) > 0 ? traits::infinity() : traits::neg_infinity()).bits;
    }else if(mpfr_zero_p(x)){
        // TODO : I cannot work out how we are supposed to get the sign out of
        // a zero. mpfr_sgn returns 0 if the number is +-zero
        fw_uint<1> sign(x->_mpfr_sign==-1);

        bits=concat(zg<2>(),sign,zg<ExpBits+FracBits>());
    }else{
        mpz_t fracBits; // Fraction as integer. So in range [2^FracBits..2^(FracBits+1)) rather than [1..1-2^FracBits)
		mpz_init(fracBits);
        int e=mpfr_get_z_2exp(fracBits, x);

        bool negative=false;
        if(mpz_sgn(fracBits)==-1){
            negative=true;
			mpz_mul_si(fracBits, fracBits, -1);
        }

        //mpfr_fprintf(stderr, "x=%Rg, e=%d, frac=0x%Zx\n", x, e, fracBits);

        // Check for explicit bit
        assert(mpz_tstbit(fracBits, FracBits));
        // Clear the explicit bit
        mpz_clrbit(fracBits,FracBits);
        
        //mpfr_fprintf(stderr, "           frac=0x%Zx\n",fracBits);

        e=e+FracBits; // Actual exponent

        //mpfr_fprintf(stderr, "   2^%d * (2^%d + %Zd) / 2^(%d)\n", e, FracBits, fracBits, FracBits);
        
        fw_uint<2> flags(0b01);
        fw_uint<1> sign(negative);
        fw_uint<ExpBits> expnt;
        fw_uint<FracBits> frac( fracBits );
        //std::cerr<<"frac.bits = "<<frac.bits<<", mask="<<frac.MASK<<"\n";

		mpz_clear(fracBits);

        if(e < traits::min_exponent){
             if(allowUnderOrOverflow){
                flags=fw_uint<2>(0b00);
            }else{
                throw std::runtime_error("Exponent out of range.");
            }
        }else if(e > traits::max_exponent){
            if(allowUnderOrOverflow){
                flags=fw_uint<2>(0b10);
            }else{
                throw std::runtime_error("Exponent out of range.");
            }
        }else{
            expnt=fw_uint<ExpBits>( e + traits::bias );
        }
        //std::cerr<<"bits = "<<flags<<" & "<<sign<<" & "<<expnt<<" & "<<frac<<"\n";
        bits=concat(flags,sign,expnt,frac);
    }
}

template<int ExpBits,int FracBits>
void fp_flopoco<ExpBits,FracBits>::get(mpfr_t dst) const
{
    if(mpfr_get_prec(dst)!=FracBits+1){
        throw std::runtime_error("Incorrect number of bits in mpfr destination.");
    }
    get(dst, MPFR_RNDN);
}

template<int ExpBits,int FracBits>
void fp_flopoco<ExpBits,FracBits>::get(mpfr_t dst, mpfr_rnd_t mode) const
{
    typedef std::numeric_limits<fp_flopoco> traits;

    fw_uint<2> flags=get_bits<ExpBits+FracBits+2,ExpBits+FracBits+1>(bits);
    auto negative=get_bit<ExpBits+FracBits>(bits);

    if( (flags==fw_uint<2>(0b00)).to_bool()){
        //std::cerr<<"Zero in\n";
        mpfr_set_zero(dst, select(negative , -1 , +1));
    }else if( (flags==fw_uint<2>(0b10)).to_bool() ){
        mpfr_set_inf(dst, select(negative , -1 , +1));
    }else if( (flags==fw_uint<2>(0b11)).to_bool() ){
        mpfr_set_nan(dst);
    }else{
        mpz_t fracBits;
        mpz_init(fracBits);
        get_bits<FracBits-1,0>(bits).to_mpz_t(fracBits);
        mpz_setbit(fracBits, FracBits); // Add explicit bit

        int e=get_bits<FracBits+ExpBits-1,FracBits>(bits).to_int();
        e=e-FracBits-traits::bias; // Move from fraction to integer

        mpfr_set_z_2exp(dst, fracBits, e, mode);

        if(negative.to_bool()){
            mpfr_mul_si(dst, dst, -1, MPFR_RNDN);
        }

        mpz_clear(fracBits);
    }

}


template<int ER,int FR,int EA,int FA,int EB,int FB>
fp_flopoco<ER,FR> ref_mul(const fp_flopoco<EA,FA> &a, const fp_flopoco<EB,FB> &b)
{
    mpfr_t ma, mb, mr;
    mpfr_init2(ma,FA+1);
    mpfr_init2(mb,FB+1);
    mpfr_init2(mr,FR+1);

    a.get(ma);
    b.get(mb);

    mpfr_mul(mr,ma,mb,MPFR_RNDN);

    //mpfr_fprintf(stderr, "mpfr : %Rg * %Rg = %Rg\n", ma, mb, mr);

    fp_flopoco<ER,FR> res(mr,true);

    //std::cerr<<"   = "<<res.str()<<"\n";

    mpfr_clear(ma);
    mpfr_clear(mb);
    mpfr_clear(mr);

    return res;
}

template<int ER,int FR,int EA,int FA,int EB,int FB>
void ref_mul(fp_flopoco<ER,FR> &dst, const fp_flopoco<EA,FA> &a, const fp_flopoco<EB,FB> &b)
{
    dst=ref_mul<ER,FR>(a,b);
}

template<int ER,int FR,int EA,int FA,int EB,int FB>
fp_flopoco<ER,FR> ref_add(const fp_flopoco<EA,FA> &a, const fp_flopoco<EB,FB> &b)
{
    mpfr_t ma, mb, mr;
    mpfr_init2(ma,FA+1);
    mpfr_init2(mb,FB+1);
    mpfr_init2(mr,FR+1);

    a.get(ma);
    b.get(mb);

    mpfr_add(mr,ma,mb,MPFR_RNDN);
    
    //mpfr_fprintf(stderr, "mpfr : %Rg + %Rg = %Rg\n", ma, mb, mr);

    fp_flopoco<ER,FR> res(mr,true);

    mpfr_clear(ma);
    mpfr_clear(mb);
    mpfr_clear(mr);

    return res;
}

template<int ER,int FR,int EA,int FA,int EB,int FB>
void ref_add(fp_flopoco<ER,FR> &dst, const fp_flopoco<EA,FA> &a, const fp_flopoco<EB,FB> &b)
{
    dst=ref_add<ER,FR>(a,b);
}


template<int ExpBits,int FracBits>
double fp_flopoco<ExpBits,FracBits>::to_double_approx() const
{
    mpfr_t tmp;
    mpfr_init2(tmp, FracBits+1);
    get(tmp);
    double r=mpfr_get_d(tmp, MPFR_RNDN);
    mpfr_clear(tmp);
    return r;
}

template<int ExpBits,int FracBits>
std::string fp_flopoco<ExpBits,FracBits>::str() const
{
    std::stringstream acc;
    acc<<bits.to_string();
    fw_uint<2> flags=get_bits<ExpBits+FracBits+2,ExpBits+FracBits+1>(bits);
    fw_uint<1> negative=get_bit<ExpBits+FracBits>(bits);
    if( (flags==0b00).to_bool() ){
        acc<<select(negative==1, " -zero" , " +zero");
    }else if( (flags==0b01).to_bool() ){
        acc<<" normal";

        fw_uint<ExpBits> expnt=get_bits<ExpBits+FracBits-1,FracBits>(bits);
        fw_uint<FracBits> frac=get_bits<FracBits-1,0>(bits);
        acc<<" e="<<expnt<<"="<<(expnt.to_int()- (1<<(ExpBits-1))+1);
        
        acc<<" f="<<frac<<"= (approx)"<<(ldexp(frac.to_int()+ldexp(1,FracBits),-FracBits));
    }else if( (flags==0b10).to_bool() ){
        acc<<select(negative==1, "-inf" , "+inf");
    }else{
        acc<<" nan"; 
    }
    return acc.str();
}
#endif


}; // thls

#endif
