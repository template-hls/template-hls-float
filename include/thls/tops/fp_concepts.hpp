

enum FloatRangeFlags
{
    FloatRangePosInf    = 0x01,
    FloatRangePos       = 0x02,
    FloatRangeZero      = 0x04,
    FloatRangeNeg       = 0x08,
    FloatRangeNegInf    = 0x10,
    FloatRangeInf       = 0x20,
    FloatRangeNaN       = 0x40,
    FloatRangeDenormal  = 0x80,

    FloatRangeFloPoCo  = FloatRangePositive|FloatRangeZero|FloatRangeNegative|FloatRangeInf|FloatRangeNaN,
    FloatRangeIEEE     = FloatRangePositive|FloatRangeZero|FloatRangeNegative|FloatRangeInf|FloatRangeNaN|FloatRangeDenormal

}

template<int TwE,int TwF,FloatRangeFlags Tflags>
class ap_float
{
    const int wE = TwE;
    const int wF = TwF;
    const FloatRangeFlags = Tflags;
};



constexpr FloatRangeFlags neg_return_range(FloatRangeFlags flags)
{
    
}


template<int TE0,int TF0,FloatRangeFlags TR0, int TE1,int TF1,FloatRangeFlags TR1, int TE2,int TF2,FloatRangeFlags TR2>
ap_float<int TE0,int TR0,FloatRangeFlags TR0> add(
    const ap_float<TE1,TF1,TR1> &x1,
    const ap_float<TE2,TF2,TR2> &x2
);

template<int TE0,int TF0,FloatRangeFlags TR0, int TE1,int TF1,FloatRangeFlags TR1, int TE2,int TF2,FloatRangeFlags TR2>
ap_float<int TE0,int TR0,FloatRangeFlags TR0> sub(
    const ap_float<TE1,TF1,TR1> &x1,
    const ap_float<TE2,TF2,TR2> &x2
);

constexpr FloatRangeFlags neg_return_range(FloatRangeFlags flags)
{
    return  (flags&FloatRangePositive) && (flags&FloatRangeNegative) ? flags : (flags^FloatRangePositive^FloatRangeNegative);
}

template<int TE1,int TF1,FloatRangeFlags TR1>
ap_float<TE1,TF1,neg_return_range(TR1)> neg(
    const ap_float<TE1,TF1,TR1> &x1
);

template<int TE0,int TF0,FloatRangeFlags TR0, int TE1,int TF1,FloatRangeFlags TR1, int TE2,int TF2,FloatRangeFlags TR2, int TE3,int TF3,FloatRangeFlags TR3>
ap_float<int TE0,int TR0,FloatRangeFlags TR0> fma(
    const ap_float<TE1,TF1,TR1> &x1,
    const ap_float<TE2,TF2,TR2> &x2,
    const ap_float<TE3,TF3,TR3> &x3
);


template<int TE1,int TF1,FloatRangeFlags TR1>
ap_float<TE1,TF1,neg_return_range(TR1)> neg(
    const ap_float<TE1,TF1,TR1> &x1
);

template<int TE0,int TF0,FloatRangeFlags TR0, int TE1,int TF1,FloatRangeFlags TR1, int TE2,int TF2,FloatRangeFlags TR2>
ap_float<int TE0,int TR0,FloatRangeFlags TR0> mul(
    const ap_float<TE1,TF1,TR1> &x1,
    const ap_float<TE2,TF2,TR2> &x2
);

template<int TE0,int TF0,FloatRangeFlags TR0, int TE1,int TF1,FloatRangeFlags TR1, int TE2,int TF2,FloatRangeFlags TR2>
ap_float<int TE0,int TR0,FloatRangeFlags TR0> div(
    const ap_float<TE1,TF1,TR1> &x1,
    const ap_float<TE2,TF2,TR2> &x2
);

constexpr FloatRangeFlags inv_return_range(FloatRangeFlags flags)
{
    return  (flags&~(FloatRangeZero|FloatRangeInf))
        
    
}

template<int TE1,int TF1,FloatRangeFlags TR1>
ap_float<TE1,TF1,div_return_range(TR1)> inv(
    const ap_float<TE1,TF1,TR1> &x1
);


template<int TE0,int TF0,FloatRangeFlags TR0, int TE1,int TF1,FloatRangeFlags TR1>
ap_float<int TE0,int TR0,FloatRangeFlags TR0> convert(
    const ap_float<TE1,TF1,TR1> &x1
);

template<int TE0,int TF0,FloatRangeFlags TR0, int TE1,int TF1,FloatRangeFlags TR1>
ap_float<int TE0,int TR0,FloatRangeFlags TR0> promote(
    const ap_float<TE1,TF1,TR1> &x1
);


template<int TE1,int TF1,FloatRangeFlags TR1, int TE2,int TF2,FloatRangeFlags TR2>
bool less_than(
    const ap_float<TE1,TF1,TR1> &x1,
    const ap_float<TE2,TF2,TR2> &x2
);

template<int TE1,int TF1,FloatRangeFlags TR1, int TE2,int TF2,FloatRangeFlags TR2>
bool equals(
    const ap_float<TE1,TF1,TR1> &x1,
    const ap_float<TE2,TF2,TR2> &x2
);
