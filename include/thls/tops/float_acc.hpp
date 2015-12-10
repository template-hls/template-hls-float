

template<int WE,int WF>
class FloatAcc
{
    // Safety space to make sure that we don't overflow. So
    // we can add up to 2^WSafety numbers before overflow is
    // possible
    static const int WSafety = 64;
    
    static const int WAcc=
        WSafety         // Make sure we don't overflow
        +1              // sign bit
        +WF             // incoming fractional bits
        +(1<<WE)-1;     // Shift introduced
    
    fw_uint<WAcc> sum, carry;
    
    template<int WF>
    fw_uint<WF+1> shift_arithmetic_right(const fw_uint<1> &dist, const fw_uint<WF> &f)
    {
        return select(x, zpad_lo<1>(f), spad_hi<1>(f));
    }
    
    template<int WF, int WD>
    fw_uint<WF+(1<<WD)-1> shift_arithmetic_right(const fw_uint<WD> &dist, const fw_uint<WF> &f)
    {
        auto prev=expand(drop_msb(x), f);
        return select(take_msb(x), zpad_lo<(1<<(WD-1))>(f), spad_hi<(1<<(WD-1))>(f));
    }
    
    template<int W>
    void leading_zero_counter(fw_uint<1> &dist, fw_uint<W> &f_res, const fw_uint<W> &f)
    {
        // If the two MSBs are the same, we should shift
        fw_uint<1> doIt=(get_bit<W-1>(f) == get_bit<W-2>(f));
        dist=doIt;
        f_res=select(doIt, f<<1, f);
    }
    
    template<int W, int WD>
    void leading_zero_counter(fw_uint<WD> &dist, fw_uint<W> &f_res, const fw_uint<W> &f)
    {
        fw_uint<1> doIt=select(take_msb(f),og<1<<(WD-1)>(),zg<1<<(WD-1)>()) == get_bits<WF-2,WF-2-(w<<(WD-1))-1>(f),
        
        fw_uint<W> tmp=select(doIt, f<<(1<<(WD-1)), f);
        
        fw_uint<WD-1> dSub;
        fw_uint<W> fSub;
        leading_zero_counter(dSub, fSub, tmp);
        
        dist=concat(doIt,dSub);
    }
    
    void add(const fw_uint<w> &reset, const fp_flopoco<WE,WF> &fp)
    {
        /////////////////////////////////////////////
        // Prep work
        
        // For biggest magnitude, return 0 (no shift), for smallest magnitude return 11...11 (max shift)
        fw_uint<WE> leadingZeros=og<WE>()-fp.get_exp_bits();
        
        // Convert to twos complement with WF+1 bits
        fw_uint<WF+1> fTwosComplement=select(fp.get_sign(), zpad_hi<1>(~fp.get_frac())+1, zpad_hi<1>(fp.get_frac()));
        
        fw_uint<WF+1+(1<<WE)> expandedTwosComp=shift_arithmetic_right(leadingZeros, fTwosComplement);
        fw_uint<WAcc> safeExpanded=spad_hi<WSafety>(expandedTwosComplement);
        
        //////////////////////////////////////////////
        // Actual accumulation, should be single cycle
        
        auto sum_n=select(reset, incoming, sum^carry^incoming);
        auto carry_n=select(reset, zg<WAcc>(), (sum&carry)|(carry&incoming)|(sum&incoming) );
        
        sum=sum_n;
        carry=carry_n<<1;
        
        ///////////////////////////////////////////////
        // Convert back to actual number and sum
        
        // Some sort of reduced critical path adder
        fw_uint<WAcc> acc=contract(sum,carry);
        
        // Because of the safety factor, the exponent might have overflowed
        // to something we can't handle. 
        static_assert(WSafety<(1<<WE), "We rely on the exponent shift being less than safety.");
        fw_uint<WD+1> accLeadingZeros;
        fw_uint<WAcc> accNormalised.
        leading_zero_counter(accLeadingZeros, accNormalised, acc);
        
        /* 0ffffffFRrrrrrrrr
           If R=0
           */
    }
};
