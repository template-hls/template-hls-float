


template<
    int fracBits,
    int expBits,
    int minExp,
    int maxExp,
    bool hasInfinity,
    bool hasZero,
    bool hasNaN,
    bool hasDenormal
>
class fp_traits
{
    static const int min_exponent = minExp;
    static const int max_exponent = maxExp;
    
    static const bool has_infinity = hasInfinity;
    static const bool has_zero = hasZero;
    static const bool has_nan = hasNaN;
    static const bool has_sign = hasSign;
    
    static const bool has_denormal = hasDenormal;
    
    static const int radix = 2;
    static const int digits = fracDigits;
};

enum flags{
    Zero,
    Normal,
    Infinity,
    NaN
};

template<class TTraits>
class fp_num
{
    typedef TTraits traits;
    
    fw_uint<traits::exp_bits> exponent;
    fw_uint<traits::frac_bits> fraction;
    fw_uint<1> sign;
    fw_uint<2> flags;
    
    bool isZero() const
    { return traits::hasZero ? flags==Zero : false; }
    
    bool isPositive() const
    { return traits::hasZero || traits:: ? flags==Zero : false; }
};


fp_num mul_denorm(fp_num a, fp_num b)
{
    auto sig = a.getSignificand() * b.getSignificand()
    
    
}


