#ifndef fw_uint_on_cpp_int_hpp
#define fw_uint_on_cpp_int_hpp

#error "This only partially works. Decided to move away from it as the software version."

#ifndef fw_uint_hpp
#error "Include fw_uint.hpp, rather than this file."
#endif

#include <boost/multiprecision/cpp_int.hpp>

#include <gmpxx.h>

template<int W>
struct fw_uint
{
    static const int width=(W<=0) ? 0 : W;
    
    template<unsigned WW>
    using backendv_t = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<WW, WW, boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void> >;
    
    typedef backendv_t<W> backend_t;

    backend_t bits;

    fw_uint()
    	: bits(0)
    {}

    fw_uint(const fw_uint &x)
    	: bits(x.bits)
    {}

    template<int WW=W>
    explicit fw_uint(bool b)
        : bits(b)
    {
        THLS_STATIC_ASSERT(WW==1, "Can only construct from bool with W=1.");
    }

    explicit fw_uint(int v)
        : bits(v)
    {
        assert(W>=0);
        assert(v>=0); // must be non-negative
        assert(v < (1ll<< (W<0 ? 0 : W))); // Must be in range
    }

    explicit fw_uint(uint64_t v)
        : bits(v)
    {
        assert(W>=0);
        assert(v>=0); // must be non-negative
        assert(v < (1ll<< (W<0 ? 0 : W))); // Must be in range
    }

    explicit fw_uint(const char *value)
        : bits(value)
    {
        // TODO : Really need to range-check the actual number
    }

    explicit fw_uint(const backend_t &x)
        : bits(x)
    {}
        
    static fw_uint from_bits(const backend_t &x)
    {
        return fw_uint(x);
    }
    
    void operator=(const fw_uint &o)
    {
        if(this!=&o){
            bits=o.bits;
        }
    }


#ifndef HLS_SYNTHESIS
    explicit fw_uint(mpz_class x)
    {
        if(mpz_sizeinbase(x.get_mpz_t(),2)>W){
            throw std::runtime_error("mpz is too large.");
        }
        std::string dec=x.get_str(); // LAZY
        bits=backend_t(dec.c_str());
    }
#endif

    template<int O>
    fw_uint<thls_ctMax(W,O)> operator+(const fw_uint<O> &o) const
    {
        backendv_t<thls_ctMax(W,O)> a(bits), b(o.bits);
        return fw_uint<thls_ctMax(W,O)>(a+b);
    }

    fw_uint operator+(int b) const
    {
        return fw_uint(bits+b);
    }

    template<int O>
    fw_uint<thls_ctMax(W,O)> operator-(const fw_uint<O> &o) const
    {
        backendv_t<thls_ctMax(W,O)> a(bits), b(o.bits);
        return fw_uint<thls_ctMax(W,O)>(a-b);
    }

    fw_uint operator-(int b) const
    {
        return fw_uint(bits-b);
    }

    template<int O>
    fw_uint<O+W> operator*(const fw_uint<O> &o) const
    {
        typedef backendv_t<W+O> be_t;
        be_t aa;
        multiply(aa, bits, o.bits);
        return fw_uint<O+W>::from_bits(aa);
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
    { return fw_uint(~bits); }

    template<int O>
    fw_uint operator&(const fw_uint<O> &o) const
    {
        backendv_t<W> b(o.bits);
        return fw_uint<W>(bits&b);
    }

    fw_uint operator&(int b) const
    {
        assert(b>=0);
        return fw_uint(bits&b);
    }

    template<int O>
    fw_uint operator|(const fw_uint<O> &o) const
    {
        backendv_t<W> b(o.bits);
        return fw_uint<W>(bits|b);
    }

    fw_uint operator|(int b) const
    {
        assert(b>=0);
        return fw_uint(bits|b);
    }

    template<int O>
    fw_uint operator^(const fw_uint<O> &o) const
    {
        backendv_t<W> b(o.bits);
        return fw_uint<W>(bits^b);
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
    
    fw_uint operator<<(int dist) const
    {
        assert(0<=dist && dist<W);
        return fw_uint(bits<<dist);
    }

    std::string to_string() const
    {
        std::string acc="";
        for(int i=0;i<W;i++){
            if((0==(i%8))&&(i!=0)){
                acc="_"+acc;
            }
            acc=(bit_test(bits,i)?"1":"0")+acc;
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

    #ifndef HLS_SYNTHESIS
    mpz_class to_mpz_class() const
    {
        mpz_class res;
        backend_t tmp=bits;
        
        int shift=0;
        while(tmp!=0){
            res=res+(mpz_class((uint64_t)tmp)<<shift);
            shift+=64;
            tmp=tmp>>64;
        }
        
        return res;
    }
    #endif

    //explicit operator bool() const
    //{ return to_bool(); }

    operator bool() const
    { return to_bool(); }
};



fw_uint<1> operator||(const fw_uint<1> &a, const fw_uint<1> &b)
{ return fw_uint<1>(a.bits||b.bits); }

fw_uint<1> operator&&(const fw_uint<1> &a, const fw_uint<1> &b)
{ return fw_uint<1>(a.bits&&b.bits); }

#ifndef HLS_SYNTHESIS
template<int W>
std::ostream &operator<<(std::ostream &dst, const fw_uint<W> &x)
{
    dst<<x.to_string();
    return dst;
}
#endif

template<int HI,int LO,int W>
fw_uint<HI-LO+1> get_bits(const fw_uint<W> &x)
{
    typedef typename fw_uint<W>::backend_t backend_orig_t;
    typedef typename fw_uint<HI-LO+1>::backend_t backend_new_t;
    backend_orig_t o(x.bits>>LO);
    return fw_uint<HI-LO+1>(backend_new_t(o));
}

template<int WA>
fw_uint<WA> concat(const fw_uint<WA> &a)
{ return a; }

template<int WA,int WB>
fw_uint<WA+WB> concat(const fw_uint<WA> &a, const fw_uint<WB> &b)
{
    typedef typename fw_uint<WA+WB>::backend_t backend_t;
    return fw_uint<WA+WB>((backend_t(a.bits)<<WB)+(backend_t(b.bits)) );
}

template<int WA,int WB,int WC>
fw_uint<WA+WB+WC> concat(const fw_uint<WA> &a, const fw_uint<WB> &b, const fw_uint<WC> &c)
{ return concat(a,concat(b,c)); }

template<int WA,int WB,int WC,int WD>
fw_uint<WA+WB+WC+WD> concat(const fw_uint<WA> &a, const fw_uint<WB> &b, const fw_uint<WC> &c, const fw_uint<WD> &d)
{ return concat(concat(a,b),concat(c,d)); }

template<int WD,int WS>
fw_uint<WD> checked_cast(const fw_uint<WS> &s)
{
    if(WD==WS){
        return fw_uint<WD>(s.bits);
    }else{
        assert(0);
        return ~fw_uint<WD>(); // Poison with ones
    }
}

#endif
