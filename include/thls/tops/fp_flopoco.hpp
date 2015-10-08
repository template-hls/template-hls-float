#ifndef thls_fp_flopoco_hpp
#define thls_fp_flopoco_hpp

#include <thls/tops/fw_uint.hpp>

#ifndef HLS_SYNTHESIS
#include <mpfr.h>
#endif

namespace thls
{

template<int ExpBits,int FracBits>
struct fp_flopoco
{
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

    // The destination must have the same fractional width
    void get(mpfr_t dst) const;

    // Allows for rounding while extracting
    void get(mpfr_t dst, mpfr_rnd_t mode) const;
#endif


    fw_uint<3+ExpBits+FracBits> bits;

    fw_uint<2> get_flags() const
    { return get_bits<ExpBits+FracBits+2,ExpBits+FracBits+1>(bits); }

    bool get_sign() const
    { return get_bit<ExpBits+FracBits>(bits); }

    fw_uint<ExpBits> get_exp_bits() const
    { return get_bits<ExpBits+FracBits-1,FracBits>(bits); }

    fw_uint<FracBits> get_frac_bits() const
    { return get_bits<FracBits-1,0>(bits); }


    bool is_zero() const
    { return get_flags()==fw_uint<2>(0b00); }

    bool is_normal() const
    { return get_flags()==fw_uint<2>(0b01); }

    bool is_inf() const
    { return get_flags()==fw_uint<2>(0b10); }

    bool is_nan() const
    { return get_flags()==fw_uint<2>(0b11); }

    bool is_positive() const
    { return get_sign()==fw_uint<1>(0b0); }

    bool is_negative() const
    { return get_sign()==fw_uint<1>(0b1); }


    bool is_pos_normal() const
    { return is_positive() && is_normal(); }

    bool is_neg_normal() const
    { return is_negative() && is_normal(); }

    bool is_pos_zero() const
    { return is_positive() && is_zero(); }

    bool is_neg_zero() const
    { return is_negative() && is_zero(); }

    bool is_pos_inf() const
    { return is_positive() && is_inf(); }

    bool is_neg_inf() const
    { return is_negative() && is_inf(); }
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
        static const int max_exponent = 0+bias-1;

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
            return T(concat(fw_uint<3>(0b010),zg<ExpBits>(),zg<FracBits>()));
        }

        // Not in std
        static THLS_CONSTEXPR T neg_min()
        {
            return T(concat(fw_uint<3>(0b011),zg<ExpBits>(),zg<FracBits>()));
        }

        static THLS_CONSTEXPR T max()
        {
            return T(concat(fw_uint<3>(0b010),og<ExpBits>(),og<FracBits>()));
        }

        static THLS_CONSTEXPR T lowest()
        { // == -max()
            return T(concat(fw_uint<3>(0b011),og<ExpBits>(),og<FracBits>()));
        }

        static THLS_CONSTEXPR T espilon()
        { // == 2^-(FracBits+1)
          // expnt == ExpBits -FracBits-1
            return T(concat(fw_uint<3>(0b010),fw_uint<ExpBits>( bias - FracBits-1) ,zg<FracBits>()));
        }

        static THLS_CONSTEXPR T round_error()
        { // == 0.5
            return T(concat(fw_uint<3>(0b010),fw_uint<ExpBits>( bias - 1 ) ,zg<FracBits>()));
        }

        static THLS_CONSTEXPR T infinity()
        {
            return T(concat(fw_uint<3>(0b100),zg<ExpBits>() ,zg<FracBits>()));
        }

        // NOTE: not part of the standard numeric_limits
        static THLS_CONSTEXPR T neg_infinity()
        {
            return T(concat(fw_uint<3>(0b101),zg<ExpBits>() ,zg<FracBits>()));
        }

        static THLS_CONSTEXPR T quiet_NaN()
        {
            return T(concat(fw_uint<3>(0b110),zg<ExpBits>() ,zg<FracBits>()));
        }

        static THLS_CONSTEXPR T signalling_NaN()
        {
            // Just return zero
            return T(concat(fw_uint<3>(0b000),zg<ExpBits>() ,zg<FracBits>()));
        }

        static THLS_CONSTEXPR T denorm_min()
        {
            return min();
        }

        // Not part of std
        static THLS_CONSTEXPR T pos_zero()
        {
            return T(concat(fw_uint<3>(0b000),zg<ExpBits>() ,zg<FracBits>()));
        }

        // Not part of std
        static THLS_CONSTEXPR T neg_zero()
        {
            return T(concat(fw_uint<3>(0b001),zg<ExpBits>() ,zg<FracBits>()));
        }
    };
};

namespace thls{

template<int ExpBits,int FracBits>
fp_flopoco<ExpBits,FracBits> nextup(const fp_flopoco<ExpBits,FracBits> &x)
{
    typedef std::numeric_limits<fp_flopoco<ExpBits,FracBits>> traits;

    if(x.is_pos_inf()){
        return x;
    }else if(x.is_pos_normal()){
        if(x.get_frac_bits()==og<FracBits>()){
            if(x.get_exp_bits()==og<ExpBits>()){
                return traits::infinity();
            }else{
                return concat(fw_uint<3>(0b010),x.get_exp_bits()+1,zg<FracBits>());
            }
        }else{
            return concat(fw_uint<3>(0b010),x.get_exp_bits(),x.get_frac_bits()+1);
        }
    }else if(x.is_pos_zero()){
        return traits::min();
    }else if(x.is_neg_zero()){
        return traits::pos_zero();
    }else if(x.is_neg_normal()){
        if(x.get_frac_bits()==zg<FracBits>()){
            if(x.get_exp_bits()==zg<ExpBits>()){
                return traits::neg_zero();
            }else{
                return concat(fw_uint<3>(0b011),x.get_exp_bits()-1,og<FracBits>());
            }
        }else{
            return concat(fw_uint<3>(0b011),x.get_exp_bits(),x.get_frac_bits()-1);
        }
    }else if(x.is_neg_inf()){
        return traits::lowest();
    }else{
        assert(x.is_nan());
        return x; // nan -> nan
    }
}

template<int ExpBits,int FracBits>
fp_flopoco<ExpBits,FracBits> nextdown(const fp_flopoco<ExpBits,FracBits> &x)
{
    typedef std::numeric_limits<fp_flopoco<ExpBits,FracBits>> traits;

    if(x.is_pos_inf()){
        return traits::max();
    }else if(x.is_pos_normal()){
        if(x.get_frac_bits()==zg<FracBits>()){
            if(x.get_exp_bits()==zg<ExpBits>()){
                return traits::pos_zero();
            }else{
                return concat(fw_uint<3>(0b010),x.get_exp_bits()-1,zg<FracBits>());
            }
        }else{
            return concat(fw_uint<3>(0b010),x.get_exp_bits(),x.get_frac_bits()-1);
        }
    }else if(x.is_pos_zero()){
        return traits::neg_zero();
    }else if(x.is_neg_zero()){
        return traits::neg_min();
    }else if(x.is_neg_normal()){
        if(x.get_frac_bits()==og<FracBits>()){
            if(x.get_exp_bits()==og<ExpBits>()){
                return traits::neg_infinity();
            }else{
                return concat(fw_uint<3>(0b011),x.get_exp_bits()+1,og<FracBits>());
            }
        }else{
            return concat(fw_uint<3>(0b011),x.get_exp_bits(),x.get_frac_bits()+1);
        }
    }else if(x.is_neg_inf()){
        return x;
    }else{
        assert(x.is_nan());
        return x;
    }
}



template<int ExpBits,int FracBits>
fp_flopoco<ExpBits,FracBits> nextafter(const fp_flopoco<ExpBits,FracBits> &x, double y)
{
    if(y==INFINITY){
        return nextup(x);
    }else if(y==-INFINITY){
        return nextdown(x);
    }else{
        throw std::runtime_error("arbitrary target not implemented, must be -INF or +INF.");
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
        bits=fw_uint<3+ExpBits+FracBits>();
    }else{
        mpz_class fracBits; // Fraction as integer. So in range [2^FracBits..2^(FracBits+1)) rather than [1..1-2^FracBits)
        int e=mpfr_get_z_2exp(fracBits.get_mpz_t(), x);

        // Check for explicit bit
        assert(mpz_tstbit(fracBits.get_mpz_t(), FracBits));
        // Clear the explicit bit
        mpz_clrbit(fracBits.get_mpz_t(),FracBits);

        e=e+FracBits; // Actual exponent

        //std::cerr<<" 2^"<<e<<" * (2^"<<FracBits<<" + "<<fracBits<<") / 2^("<<FracBits<<")\n";

        fw_uint<2> flags(0b01);
        fw_uint<1> sign(mpfr_sgn(x)<0);
        fw_uint<ExpBits> expnt;
        fw_uint<FracBits> frac( fracBits );

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

    if(flags==fw_uint<2>(0b00)){
        std::cerr<<"Zero in\n";
        mpfr_set_zero(dst, get_bit<ExpBits+FracBits>(bits) ? -1 : +1);
    }else if(flags==fw_uint<2>(0b10)){
        mpfr_set_inf(dst, get_bit<ExpBits+FracBits>(bits) ? -1 : +1);
    }else if(flags==fw_uint<2>(0b11)){
        mpfr_set_nan(dst);
    }else{
        mpz_class fracBits=get_bits<FracBits-1,0>(bits).to_mpz_class();
        mpz_setbit(fracBits.get_mpz_t(), FracBits); // Add explicit bit

        int e=get_bits<FracBits+ExpBits-1,FracBits>(bits).to_int();
        e=e-FracBits-traits::bias; // Move from fraction to integer

        mpfr_set_z_2exp(dst, fracBits.get_mpz_t(), e, mode);
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

    fp_flopoco<ER,FR> res(mr,true);

    mpfr_clear(ma);
    mpfr_clear(mb);
    mpfr_clear(mr);

    return res;
}

#endif


/*

#ifndef __SYNTHESIS__
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
	#endif

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

#ifndef __SYNTHESIS__
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
#endif
*/

}; // thls

#endif
