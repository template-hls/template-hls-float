
class carry_select_add_span3
{
    static fw_uint<W> go(const fw_uint<A> &a, const fw_uint<B> &b)
    {
        static const int W_L=W/3;
        static const int W_M=(W-W_L)/2;
        static const int W_H=W-W_L-W_M;
        
        fw_uint<W_H> hi_0=zpad_hi<1>(get_bits<W-1,W_M>(a))+zpad_hi<1>(get_bits<W-1,W_M>(b))+0;
        fw_uint<W_H> hi_1=zpad_hi<1>(get_bits<W-1,W_M>(a))+zpad_hi<1>(get_bits<W-1,W_M>(b))+1;
        
        fw_uint<W_M> mid_0=zpad_hi<1>(get_bits<W_M-1,W_L>(a))+zpad_hi<1>(get_bits<W_M-1,W_L>(b))+0;
        fw_uint<W_M> mid_1=zpad_hi<1>(get_bits<W_M-1,W_L>(a))+zpad_hi<1>(get_bits<W_M-1,W_L>(b))+1;
        
        fw_uint<W_L+1> lo=zpad_hi<1>(get_bits<W_L-1,0>(a))+zpad_hi<1>(get_bits<W_L-1,0>(b));
        
        auto sel_mid=get_bit<W_L-1>(lo);
        auto sel_hi=select(sel_mid, get_bit<W_M-1>(mid_1), get_bit<W_M-1>(mid_0));
        
        return concat(
            select(get_bit<W_LO>(lo), hi_1, hi_0),
            get_bits<W_LO-1,0>(lo)
        );
    }
};

class carry_select_add_span2
{
    static fw_uint<W> go(const fw_uint<A> &a, const fw_uint<B> &b)
    {
        static const int W_LO=W/2;
        static const int W_HI=W-W_LO;
        
        fw_uint<W_HI> hi_0=get_bits<W-1,W_LO>(a)+get_bits<W-1,W_LO>(b)+0;
        fw_uint<W_HI> hi_1=get_bits<W-1,W_LO>(a)+get_bits<W-1,W_LO>(b)+1;
        
        fw_uint<L2+1> lo=zpad_hi<1>(get_bits<W_LO-1,0>(a)) + zpad_hi<1>(get_bits<W_LO-1,0>(b));
        
        return concat( select(get_bit<W_LO>(lo), hi_1, hi_0), get_bits<W_LO-1,0>(lo) );
    }
};

fw_uint<W> carry_select_add(
    const fw_uint<W> &a,
    const fw_uint<W> &b,
){
    
}
