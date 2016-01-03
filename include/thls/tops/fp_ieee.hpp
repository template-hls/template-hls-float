#ifndef thls_fp_ieee_hpp
#define thls_fp_ieee_hpp

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
struct fp_ieee
{
    static const bool has_denorm = false;
    enum{ exp_bits = ExpBits };
    enum{ frac_bits = FracBits };

    THLS_CONSTEXPR fp_ieee()
        : bits()
    {}

    THLS_CONSTEXPR fp_ieee(const fw_uint<1+ExpBits+FracBits> &_bits)
        : bits(_bits)
    {}

#ifndef THLS_SYNTHESIS
    // If allowUnderOrOverflow is on, then exponents out of range
    // will be flushed to zero or infinity.
    // Number of bits in number must _always_ match FracBits
    fp_ieee(mpfr_t x, bool allowUnderOrOverflow=false);
    
    void get_exponent(int &e) const;
    void get_fraction(mpfr_t &dst) const;
 
    // The destination must have the same fractional width
    void get(mpfr_t dst) const;

    // Allows for rounding while extracting
    void get(mpfr_t dst, mpfr_rnd_t mode) const;
#endif

    fw_uint<1+ExpBits+FracBits> bits;
        
    //! Returns flags equivalent to flopoco format flags
    fw_uint<2> get_flags() const
    {
        return select(
            get_exp_bits()==zg<ExpBits>(),
                fw_uint<2>(0b00), // zero, with de-normals treated as zero
            get_exp_bits()==og<ExpBits>(),
                select(get_frac_bits()==zg<FracBits>(),
                    fw_uint<2>(0b10), // infinity
                    fw_uint<2>(0b11)  // nan
                ),
            // else
                fw_uint<2>(0b01) // normal
        );
    }

    fw_uint<1> get_sign() const
    { return get_bit<ExpBits+FracBits>(bits); }

    fw_uint<ExpBits> get_exp_bits() const
    { return get_bits<ExpBits+FracBits-1,FracBits>(bits); }

    fw_uint<FracBits> get_frac_bits() const
    { return get_bits<FracBits-1,0>(bits); }

    // This also returns true for sub-normals
    fw_uint<1> is_zero() const
    { return (get_exp_bits()==0); }

    // Note that this will return false for sub-normals
    fw_uint<1> is_normal() const
    { return (get_exp_bits()!=og<ExpBits>()) && (get_exp_bits()!=zg<ExpBits>()) ; }

    fw_uint<1> is_inf() const
    { return (get_exp_bits()==og<ExpBits>()) && (get_frac_bits()==0); }

    fw_uint<1> is_nan() const
    { return (get_exp_bits()==og<ExpBits>()) && (get_frac_bits()!=0); }

    // This allows for signed nans, should check the class first
    fw_uint<1> is_positive() const
    { return get_sign()==fw_uint<1>(0b0); }

    // This allows for signed nans, should check the class first
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
    fw_uint<1> equals(const fp_ieee &o) const
    {
        auto e_eq=get_exp_bits()==o.get_exp_bits();
        auto f_eq=get_frac_bits()==o.get_frac_bits();
        auto s_eq=get_sign()==o.get_sign();
        return select(
            get_exp_bits()==0,
                e_eq, // We consider sub-normals to equal zero
            get_exp_bits()==og<ExpBits>(),
                select(get_frac_bits()==0,
                    e_eq && f_eq && s_eq, // We are infinity, are they the same one?
                    e_eq && (o.get_frac_bits()!=0) // We are nan, are they nan?
                ),
            // else, normal
                e_eq && f_eq && s_eq
        );
    }
};

}; // thls

namespace std
{
    template<int ExpBits,int FracBits>
    class numeric_limits<thls::fp_ieee<ExpBits,FracBits> >
    {
        typedef thls::fp_ieee<ExpBits,FracBits> T;
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

        static const int min_exponent = 0-bias+1;
        static const int max_exponent = 0+bias;

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
            return T(concat(thls::zg<1>(),thls::zg<ExpBits-1>(),thls::og<1>(),thls::zg<FracBits>()));
        }

        // Not in std
        static THLS_CONSTEXPR T neg_min()
        {
            return T(concat(thls::og<1>(),thls::zg<ExpBits-1>(),thls::og<1>(),thls::zg<FracBits>()));
        }

        static THLS_CONSTEXPR T max()
        {
            return T(concat(thls::zg<1>(),thls::og<ExpBits-1>(),thls::zg<1>(),thls::og<FracBits>()));
        }
        
        // Not in std
        static THLS_CONSTEXPR T neg_max()
        {
            return T(concat(thls::og<1>(),thls::og<ExpBits-1>(),thls::zg<1>(),thls::og<FracBits>()));
        }

        static THLS_CONSTEXPR T lowest()
        { // == -max()
            return T(concat(thls::og<1>(),thls::og<ExpBits-1>(),thls::zg<1>(),thls::og<FracBits>()));
        }

        static THLS_CONSTEXPR T epsilon()
        { // == 2^-(FracBits+1)
          // expnt == ExpBits -FracBits-1
            assert(0); // not tested
            return T(concat(thls::zg<1>(),thls::fw_uint<ExpBits>( bias - FracBits-1) ,thls::zg<FracBits>()));
        }

        static THLS_CONSTEXPR T round_error()
        { // == 0.5
            assert(0); // not tested
            return T(concat(thls::zg<1>(),thls::fw_uint<ExpBits>( bias - 1 ) ,thls::zg<FracBits>()));
        }

        static THLS_CONSTEXPR T infinity()
        {
            return T(concat(thls::zg<1>(),thls::og<ExpBits>() ,thls::zg<FracBits>()));
        }

        // NOTE: not part of the standard numeric_limits
        static THLS_CONSTEXPR T neg_infinity()
        {
            return T(concat(thls::og<1>(),thls::og<ExpBits>() ,thls::zg<FracBits>()));
        }

        static THLS_CONSTEXPR T quiet_NaN()
        {
            return T(concat(thls::zg<1>(),thls::og<ExpBits>() ,thls::og<FracBits>()));
        }

        static THLS_CONSTEXPR T signalling_NaN()
        {
            // This just returns a quiet nan (MSB of fraction is one)
            return T(concat(thls::zg<1>(),thls::og<ExpBits>() ,thls::zg<FracBits>()));
        }

        static THLS_CONSTEXPR T denorm_min()
        {
            return min();
        }

        // Not part of std
        static THLS_CONSTEXPR T pos_zero()
        {
            return T(concat(thls::zg<1>(),thls::zg<ExpBits>() ,thls::zg<FracBits>()));
        }

        // Not part of std
        static THLS_CONSTEXPR T neg_zero()
        {
            return T(concat(thls::og<1>(),thls::zg<ExpBits>() ,thls::zg<FracBits>()));
        }
        
        // Not part of std
        static THLS_CONSTEXPR T zero()
        { return pos_zero(); }
        
        // Not part of std
        static THLS_CONSTEXPR T pos_one()
        {
            return T(concat(thls::zg<1>(),thls::fw_uint<ExpBits>(bias),thls::zg<FracBits>()));
        }

        // Not part of std
        static THLS_CONSTEXPR T neg_one()
        {
            return T(concat(thls::og<1>(),thls::fw_uint<ExpBits>(bias),thls::zg<FracBits>()));
        }
        
        // Not part of std
        static THLS_CONSTEXPR T one()
        { return pos_one(); }
    };
};

namespace thls{


#ifndef THLS_SYNTHESIS
template<int ExpBits,int FracBits>
fp_ieee<ExpBits,FracBits>::fp_ieee(mpfr_t x, bool allowUnderOrOverflow)
{
    typedef std::numeric_limits<fp_ieee> traits;

    if(mpfr_get_prec(x)!=FracBits+1){
        throw std::runtime_error("Incorrect number of bits in mpfr input.");
    }

    if(mpfr_nan_p(x)){
        bits=std::numeric_limits<fp_ieee>::quiet_NaN().bits;
    }else if(mpfr_inf_p(x)){
        bits = (mpfr_sgn(x) > 0 ? traits::infinity() : traits::neg_infinity()).bits;
    }else if(mpfr_zero_p(x)){
        // TODO : I cannot work out how we are supposed to get the sign out of
        // a zero. mpfr_sgn returns 0 if the number is +-zero
        fw_uint<1> sign(x->_mpfr_sign==-1);

        bits=concat(sign,zg<ExpBits+FracBits>());
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
        
        fw_uint<1> sign(negative);
        fw_uint<ExpBits> expnt;
        fw_uint<FracBits> frac( fracBits );
        
		mpz_clear(fracBits);

        if(e < traits::min_exponent){
             if(allowUnderOrOverflow){
                expnt=zg<ExpBits>();
                frac=zg<FracBits>();
            }else{
                throw std::runtime_error("Exponent out of range.");
            }
        }else if(e > traits::max_exponent){
            if(allowUnderOrOverflow){
                expnt=og<ExpBits>();
                frac=zg<FracBits>();
            }else{
                throw std::runtime_error("Exponent out of range.");
            }
        }else{
            expnt=fw_uint<ExpBits>( e + traits::bias );
        }
        //std::cerr<<"bits = "<<flags<<" & "<<sign<<" & "<<expnt<<" & "<<frac<<"\n";
        bits=concat(sign,expnt,frac);
    }
}

template<int ExpBits,int FracBits>
void fp_ieee<ExpBits,FracBits>::get(mpfr_t dst) const
{
    if(mpfr_get_prec(dst)!=FracBits+1){
        throw std::runtime_error("Incorrect number of bits in mpfr destination.");
    }
    get(dst, MPFR_RNDN);
}

template<int ExpBits,int FracBits>
void fp_ieee<ExpBits,FracBits>::get(mpfr_t dst, mpfr_rnd_t mode) const
{
    typedef std::numeric_limits<fp_ieee> traits;

    if(is_zero().to_bool()){
        //std::cerr<<"Zero in\n";
        mpfr_set_zero(dst, select(is_negative().to_bool() , -1 , +1));
    }else if( is_inf().to_bool() ){
        mpfr_set_inf(dst, select(is_negative().to_bool() , -1 , +1));
    }else if( is_nan().to_bool() ){
        mpfr_set_nan(dst);
    }else{
        mpz_t fracBits;
        mpz_init(fracBits);
        get_bits<FracBits-1,0>(bits).to_mpz_t(fracBits);
        mpz_setbit(fracBits, FracBits); // Add explicit bit

        int e=get_bits<FracBits+ExpBits-1,FracBits>(bits).to_int();
        e=e-FracBits-traits::bias; // Move from fraction to integer

        mpfr_set_z_2exp(dst, fracBits, e, mode);

        if(is_negative().to_bool()){
            mpfr_mul_si(dst, dst, -1, MPFR_RNDN);
        }

        mpz_clear(fracBits);
    }

}


template<int ER,int FR,int EA,int FA,int EB,int FB>
fp_ieee<ER,FR> ref_mul(const fp_ieee<EA,FA> &a, const fp_ieee<EB,FB> &b)
{
    mpfr_t ma, mb, mr;
    mpfr_init2(ma,FA+1);
    mpfr_init2(mb,FB+1);
    mpfr_init2(mr,FR+1);

    a.get(ma);
    b.get(mb);

    mpfr_mul(mr,ma,mb,MPFR_RNDN);

    //mpfr_fprintf(stderr, "mpfr : %Rg * %Rg = %Rg\n", ma, mb, mr);

    fp_ieee<ER,FR> res(mr,true);

    //std::cerr<<"   = "<<res.str()<<"\n";

    mpfr_clear(ma);
    mpfr_clear(mb);
    mpfr_clear(mr);

    return res;
}

template<int ER,int FR,int EA,int FA,int EB,int FB>
void ref_mul(fp_ieee<ER,FR> &dst, const fp_ieee<EA,FA> &a, const fp_ieee<EB,FB> &b)
{
    dst=ref_mul<ER,FR>(a,b);
}

template<int ER,int FR,int EA,int FA,int EB,int FB>
fp_ieee<ER,FR> ref_add(const fp_ieee<EA,FA> &a, const fp_ieee<EB,FB> &b)
{
    mpfr_t ma, mb, mr;
    mpfr_init2(ma,FA+1);
    mpfr_init2(mb,FB+1);
    mpfr_init2(mr,FR+1);

    a.get(ma);
    b.get(mb);

    mpfr_add(mr,ma,mb,MPFR_RNDN);
    
    //mpfr_fprintf(stderr, "mpfr : %Rg + %Rg = %Rg\n", ma, mb, mr);

    fp_ieee<ER,FR> res(mr,true);

    mpfr_clear(ma);
    mpfr_clear(mb);
    mpfr_clear(mr);

    return res;
}

template<int ER,int FR,int EA,int FA,int EB,int FB>
void ref_add(fp_ieee<ER,FR> &dst, const fp_ieee<EA,FA> &a, const fp_ieee<EB,FB> &b)
{
    dst=ref_add<ER,FR>(a,b);
}


template<int ExpBits,int FracBits>
double fp_ieee<ExpBits,FracBits>::to_double_approx() const
{
    mpfr_t tmp;
    mpfr_init2(tmp, FracBits+1);
    get(tmp);
    double r=mpfr_get_d(tmp, MPFR_RNDN);
    mpfr_clear(tmp);
    return r;
}

template<int ExpBits,int FracBits>
std::string fp_ieee<ExpBits,FracBits>::str() const
{
    std::stringstream acc;
    acc<<bits.to_string();
    if( is_zero().to_bool() ){
        acc<<select(is_negative(), " -zero" , " +zero");
    }else if( is_normal().to_bool() ){
        acc<<" normal";

        fw_uint<ExpBits> expnt=get_bits<ExpBits+FracBits-1,FracBits>(bits);
        fw_uint<FracBits> frac=get_bits<FracBits-1,0>(bits);
        acc<<" e="<<expnt<<"="<<(expnt.to_int()- (1<<(ExpBits-1))+1);
        
        acc<<" f="<<frac<<"= (approx)"<<(ldexp(frac.to_int()+ldexp(1,FracBits),-FracBits));
    }else if( is_inf().to_bool() ){
        acc<<select(is_negative(), "-inf" , "+inf");
    }else{
        acc<<" nan"; 
    }
    return acc.str();
}
#endif

}; // thls

#endif
