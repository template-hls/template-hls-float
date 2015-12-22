#ifndef fw_uint_on_masked_uint_hpp
#define fw_uint_on_masked_uint_hpp

#ifndef fw_uint_hpp
#error "Include fw_uint.hpp, rather than this file."
#endif

#include <cstdint>
#include <climits>
#include <cstdlib>
#include <stdlib.h>
#include <string.h>

#ifndef THLS_SYNTHESIS
#include <string>
#include <gmp.h>
#include <stdexcept>
#include <sstream>
#endif

namespace thls
{

namespace detail
{
    template<int W>
    struct bits_holder
    {
        typedef typename bits_holder< (W<=0) ? 1 : W-1 >::bits_t bits_t;
        static const int bits_w = bits_holder< (W<=0) ? 1 : W-1 >::bits_w;
    };
    
    template<>
    struct bits_holder<1>
    {
        typedef uint32_t bits_t;
        static const int bits_w = 32;
    };
    
    template<>
    struct bits_holder<33>
    {
        typedef uint64_t bits_t;
        static const int bits_w = 64;
    };
    
    // TODO : How do we detect support for 128 bit?
    template<>
    struct bits_holder<65>
    {
        typedef unsigned __int128 bits_t;
        static const int bits_w = 128;
    };
    
    template<>
    struct bits_holder<129>
    {
        // No bits_t defined for this
    };
};

template<int W>
struct fw_uint
{
    // Note that negative widths _are_ allowed, but they
    // must never be executed.

    THLS_STATIC_ASSERT(W<=128, "W must be <= 128.");

    static const int width=W;
    
    typedef typename detail::bits_holder<width>::bits_t bits_t;
    static const int bits_w = detail::bits_holder<width>::bits_w;

    static const bits_t ALL_ONES =  bits_t(0)-bits_t(1);
	static const bits_t MASK = (W <= 0) ? 0ull : (ALL_ONES >> (W < 0 ? 0 : (bits_w - W)));

    bits_t bits;

    THLS_INLINE fw_uint()
    	: bits(0)
    {
        assert(W>=0);
    }

    THLS_INLINE fw_uint(const fw_uint &x)
    	: bits(x.bits)
    {
        assert(W>=0);
    }

    THLS_INLINE void operator=(const fw_uint &o)
    {
        bits=o.bits;
    }

    THLS_INLINE explicit fw_uint(int v)
        : bits(v)
    {
        assert(W>=0);

        assert(v>=0); // must be non-negative
        assert(bits_t(v) <= MASK); // Must be in range
    }
    
    THLS_INLINE explicit fw_uint(bits_t v)
        : bits(v & MASK)
    {   
        assert(W>=0);

        assert(v>=0); // must be non-negative
        assert(v <= MASK); // Must be in range
    }
#ifndef THLS_SYNTHESIS
    explicit fw_uint(const char *value)
    {
        assert(W>=0);
        
        // Special case binary
        if(!strncmp("0b", value, 2)){
            bits=0;
            
            const char *read=value+2;
            
            int done=0;
            while(done<W){
                switch(*read){
                case 0:
                    throw std::runtime_error("Not enough binary digits in string.");
                case '_':
                    break;
                case '0':
                    bits=(bits<<1)+0;
                    done++;
                    break;
                case '1':
                    bits=(bits<<1)+1;
                    done++;
                    break;
                default:
                    throw std::runtime_error("Unexpected characterin string.");
                }
                read++;
            }
            if(*read!=0){
                throw std::runtime_error("Unexpected trailing characgers in binary string.");
            }
        }else{
            std::stringstream tmp;
            tmp<<std::string(value);
            tmp>>bits;
            assert( bits<= MASK);
            bits=bits&MASK;
        }
    }
#endif

    THLS_INLINE static fw_uint from_bits(const bits_t &x)
    {
        return fw_uint(x);
    }


#ifndef THLS_SYNTHESIS
    explicit fw_uint(mpz_t x)
    {
        assert(W>=0);

        if(mpz_sizeinbase(x,2)>W){
            throw std::runtime_error("mpz is too large.");
        }
        mpz_t tmp;
        mpz_init_set(tmp, x);
        if(sizeof(unsigned int)*8 <= W){
            bits=mpz_get_ui(x);
        }else{
            bits=0;
            int offset=0;
            while(offset<W){
                mpz_set(tmp,x);
                mpz_tdiv_q_2exp(tmp, tmp, offset);
                mpz_tdiv_r_2exp(tmp, tmp, 32);
                uint32_t limb=mpz_get_ui(tmp);
                bits=bits | (bits_t(limb)<<offset);
                offset+=32;
            }
        }
        mpz_clear(tmp);

        assert(bits<=MASK);
        bits=bits&MASK;
    }
#endif

    THLS_INLINE fw_uint<W> operator+(const fw_uint<W> &o) const
    {
        return fw_uint<W>( (bits+o.bits) & MASK );
    }

    THLS_INLINE fw_uint operator+(int b) const
    {
        return fw_uint( bits_t(bits+b) & MASK);
    }

    THLS_INLINE fw_uint<W> operator-(const fw_uint<W> &o) const
    {
        return fw_uint<W>( (bits-o.bits) & MASK);
    }

    THLS_INLINE fw_uint operator-(int b) const
    {
        return fw_uint( bits_t(bits-b) & MASK);
    }

    template<int O>
    THLS_INLINE fw_uint<O+W> operator*(const fw_uint<O> &o) const
    {
        typedef typename fw_uint<O+W>::bits_t res_bits_t;
        return fw_uint<O+W>(res_bits_t(bits)*res_bits_t(o.bits));
    }

    THLS_INLINE fw_uint<1> operator<(const fw_uint &o) const
    { return fw_uint<1>(bits < o.bits); }

    THLS_INLINE fw_uint<1> operator<(int o) const
    {
        assert(o>=0);
        return fw_uint<1>(bits < o);
    }

    THLS_INLINE fw_uint<1> operator<=(const fw_uint &o) const
    { return fw_uint<1>(bits <= o.bits); }

    THLS_INLINE fw_uint<1> operator<=(int o) const
    {
        assert(o>=0);
        return fw_uint<1>(bits <= bits_t(o));
    }

    THLS_INLINE fw_uint<1> operator==(const fw_uint &o) const
    { return fw_uint<1>(bits == o.bits); }

    THLS_INLINE fw_uint<1> operator==(int o) const
    {
        assert(o>=0);
        return fw_uint<1>(bits == bits_t(o));
    }

    THLS_INLINE fw_uint<1> operator>=(const fw_uint &o) const
    { return fw_uint<1>(bits >= o.bits); }

    THLS_INLINE fw_uint<1> operator>=(int o) const
    {
        assert(o>=0);
        return fw_uint<1>(bits >= (bits_t)o);
    }

    THLS_INLINE fw_uint<1> operator>(const fw_uint &o) const
    { return fw_uint<1>(bits > o.bits); }

    THLS_INLINE fw_uint<1> operator>(int o) const
    {
        assert(o>=0);
        return fw_uint<1>(bits > o);
    }

    THLS_INLINE fw_uint<1> operator!=(const fw_uint &o) const
    { return fw_uint<1>(bits != o.bits); }

    THLS_INLINE fw_uint<1> operator!=(int o) const
    {
        assert(o>=0);
        return fw_uint<1>(bits != (bits_t)o);
    }

    //////////////////////////////////////////////
    // Bitwise ops.
    // numeric_std says result width is size of the LHS

    THLS_INLINE fw_uint operator~() const
    { return fw_uint( bits ^ MASK ); }

    THLS_INLINE fw_uint operator&(const fw_uint<W> &o) const
    {
        return fw_uint<W>(bits&o.bits);
    }

    THLS_INLINE fw_uint operator&(int b) const
    {
        assert(b>=0);
        return fw_uint(bits&b);
    }

    THLS_INLINE fw_uint operator|(const fw_uint<W> &o) const
    {
        return fw_uint<W>( (bits|o.bits) & MASK );
    }

    THLS_INLINE fw_uint operator|(int b) const
    {
        assert(b>=0);
        return fw_uint( bits_t(bits|b) & MASK);
    }

    THLS_INLINE fw_uint operator^(const fw_uint<W> &o) const
    {
        return fw_uint<W>( (bits^o.bits) & MASK );
    }

    THLS_INLINE fw_uint operator^(int b) const
    {
        assert(b>=0);
        return fw_uint( bits_t(bits^b) & MASK);
    }


    THLS_INLINE fw_uint operator>>(int dist) const
    {
        assert(0<=dist && dist<W);
        return fw_uint(bits>>dist);
    }


    THLS_INLINE fw_uint operator<<(int dist) const
    {
        assert(0<=dist && dist<W);
        return fw_uint( (bits<<dist) & MASK);
    }

#ifndef THLS_SYNTHESIS
    std::string to_string() const
    {
        std::string acc="";
        for(int i=0;i<W;i++){
            if((0==(i%8))&&(i!=0)){
                acc="_"+acc;
            }
            acc=(((bits>>i)&1)?"1":"0")+acc;
        }
        return "0b"+acc;
    }
#endif

    THLS_INLINE int to_int() const
    {
        assert(bits <= INT_MAX);
        return (int)bits;
    }

    THLS_INLINE uint64_t to_uint64() const
    {
        assert( bits == uint64_t(bits_t(bits)) );
        return uint64_t(bits);
    }

    THLS_INLINE bool to_bool() const
    {
        assert(W==1);
        return bits==1;
    }

    #ifndef THLS_SYNTHESIS
    void to_mpz_t(mpz_t res) const
    {
        mpz_t tmp;
        mpz_init(tmp);
        
        mpz_set_ui(res, 0);
        
        static_assert(sizeof(unsigned long)>=4, "Must have 32-bit or bigger longs");
        int offset=0;
        while(offset<W){
            uint32_t limb=uint32_t( (bits>>offset)&0xFFFFFFFFull );
            mpz_set_ui(tmp, limb);
            mpz_mul_2exp(tmp, tmp, offset);
            mpz_add(res, res, tmp);
            offset+=32;
        }
        
        mpz_clear(tmp);
    }

    #endif

    //explicit operator bool() const
    //{ return to_bool(); }

    //THLS_INLINE operator bool() const
    //{ return to_bool(); }
};

template<int HI,int LO,int W>
THLS_INLINE fw_uint<HI-LO+1> get_bits(const fw_uint<W> &x)
{
    typedef typename fw_uint<HI-LO+1>::bits_t bits_t;
    static const bits_t MASK=fw_uint<HI-LO+1>::MASK;
    
    assert( W>HI );
    assert( LO>=0 );
    
    static const int SAFE_LO = LO<0 ? 0 : LO;
    
    return fw_uint<HI-LO+1>( bits_t(x.bits>>SAFE_LO) & MASK );
}

template<int WA>
THLS_INLINE fw_uint<WA> concat(const fw_uint<WA> &a)
{ return a; }

template<int WA,int WB>
THLS_INLINE fw_uint<WA+WB> concat(const fw_uint<WA> &a, const fw_uint<WB> &b)
{
    // This is to protect against warnings in paths that will never
    // be executed, but will still be compiled (i.e. things that
    // would be fixed with static_if.
    static const int S=(WB < 0) ? 0 : WB;
    
    typedef typename fw_uint<WA+WB>::bits_t bits_t;

    return fw_uint<WA+WB>( (bits_t(a.bits)<<S) | bits_t(b.bits) );
}

template<int WA,int WB,int WC>
THLS_INLINE fw_uint<WA+WB+WC> concat(const fw_uint<WA> &a, const fw_uint<WB> &b, const fw_uint<WC> &c)
{ return concat(a,concat(b,c)); }

template<int WA,int WB,int WC,int WD>
THLS_INLINE fw_uint<WA+WB+WC+WD> concat(const fw_uint<WA> &a, const fw_uint<WB> &b, const fw_uint<WC> &c, const fw_uint<WD> &d)
{ return concat(concat(a,b),concat(c,d)); }


template<int WD,int WS>
THLS_INLINE fw_uint<WD> checked_cast(const fw_uint<WS> &s)
{
    if(WD==WS){
        return fw_uint<WD>(s.bits);
    }else{
        assert(0);
        return ~fw_uint<WD>(); // Poison with ones
    }
}

}; // thls

#endif
