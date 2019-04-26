#ifndef fw_uint_on_ac_uint_hpp
#define fw_uint_on_ac_uint_hpp

#ifndef fw_uint_hpp
#error "Include fw_uint.hpp, rather than this file."
#endif

#include <type_traits>
#include <string.h>
#include "ac_int.h"

#ifndef THLS_SYNTHESIS
#include <gmp.h>
#include <stdexcept>
#endif

namespace thls
{

namespace detail
{
    struct init_from_bits
    {};
}

template<int WW>
using ac_uint = ac_int<WW,false>;

template<int W>
struct fw_uint
{

    static const int width=W;

    static const int SafeW= (width<=0) ? 1 : width;

    typedef ac_uint<SafeW> bits_t;

    static const ac_uint<SafeW+1> overflow()
    {
        return ac_uint<SafeW+1>(1) << ac_uint<thls_ctLog2Ceil(SafeW+1)>(SafeW);
    }

    bits_t bits;

    THLS_INLINE_STRONG THLS_CONSTEXPR fw_uint()
    	: bits(0)
    {
        assert(W>=0);
    }

    THLS_INLINE_STRONG fw_uint(const fw_uint &x)
    	: bits(x.bits)
    {
        assert(W>=0);
    }

    THLS_INLINE_STRONG fw_uint(const bits_t &bits, detail::init_from_bits)
    	: bits(bits)
    {}

    THLS_INLINE_STRONG static fw_uint from_bits(const bits_t &x)
    {
        return fw_uint(x,detail::init_from_bits());
    }


    // Construct 1-bit vector from boolean
    template<int WW=W>
    THLS_INLINE_STRONG fw_uint(bool b, typename std::enable_if<WW==1>::type * = nullptr )
        : bits(b)
    {
        assert(W>=0);
    }
    
    THLS_INLINE_STRONG explicit fw_uint(int v)
        : bits(v)
    {
        assert(W>=0);
        assert(v>=0); // must be non-negative
        assert(uint64_t(v) < overflow()); // Must be in range
    }

    THLS_INLINE_STRONG explicit fw_uint(uint32_t v)
        : bits(v)
    {
        assert(W>=0);
        assert(v < overflow()); // Must be in range
    }

    THLS_INLINE_STRONG explicit fw_uint(uint64_t v)
        : bits(v)
    {
        assert(W>=0);
        assert(v < overflow()); // Must be in range
    }

    THLS_INLINE static fw_uint from_uint64(uint64_t x)
	{
    	return fw_uint(x);
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
                throw std::runtime_error("Unexpected trailing characters in binary string.");
            }
        }else if(!strncmp("0x",value, 2)){
            const char *read=value+2;
            // TODO: Actual range checking, as ac_int will just truncate MSBs
            bits.bit_fill_hex(read);
        }else{
            throw "Not supported. Only hex and binary supported at the moment.";
        }
    }
#endif
    THLS_INLINE_STRONG explicit fw_uint(const bits_t &x)
        : bits(x)
    {
        assert(W>=0);
    }

    /*THLS_INLINE_STRONG explicit fw_uint(const ap_range_ref<SafeW,false> &x)
        : bits(x)
    {
        assert(W>=0);
    }
    */

#ifndef THLS_SYNTHESIS
    explicit fw_uint(mpz_t x)
    {
        assert(W>=0);

        if(mpz_sizeinbase(x,2)>W){
            throw std::runtime_error("mpz is too large.");
        }
        mpz_t tmp, tmp2;
        mpz_init_set(tmp, x);
        mpz_init(tmp2);
        if(sizeof(unsigned int)*8 <= W){
            bits=mpz_get_ui(x);
        }else{
            ac_uint<((SafeW+31)/32)*32> acc=0;
            int offset=0;
            while(0!=mpz_cmp_si(tmp, 0)){
                mpz_tdiv_r_2exp(tmp2, tmp, 32);
                ac_uint<32> curr=mpz_get_ui(tmp2);
                acc.set_slc(offset, curr);
                mpz_tdiv_q_2exp(tmp, tmp, 32);
                offset+=32;
            }
            bits=acc;
        }
        mpz_clear(tmp);
        mpz_clear(tmp2);
    }
#endif

    THLS_INLINE_STRONG fw_uint<W> operator+(const fw_uint<W> &o) const
    {
        // ac_int adds 1 to width, so must convert back
        return fw_uint(bits_t(bits+o.bits), detail::init_from_bits());
    }

    THLS_INLINE_STRONG fw_uint operator+(int b) const
    {
        return fw_uint(bits_t(bits+b), detail::init_from_bits());
    }

    THLS_INLINE_STRONG fw_uint<W> operator-(const fw_uint<W> &o) const
    {
        return fw_uint(bits_t(bits-o.bits), detail::init_from_bits());
    }

    THLS_INLINE_STRONG fw_uint operator-(int b) const
    {
        return fw_uint(bits_t(bits-b), detail::init_from_bits());
    }

    template<int O>
    THLS_INLINE_STRONG fw_uint<O+W> operator*(const fw_uint<O> &o) const
    {
    	static const int SafeRW = (O+W <= 0) ? 1 : (O+W);
        return fw_uint<O+W>(bits*o.bits, detail::init_from_bits()); // ac_int does full width product
    }

    THLS_INLINE_STRONG fw_uint<1> operator<(const fw_uint &o) const
    { return fw_uint<1>(bits < o.bits); }

    THLS_INLINE_STRONG fw_uint<1> operator<(int o) const
    {
        assert(o>=0);
        return fw_uint<1>(bits < o);
    }

    THLS_INLINE_STRONG fw_uint<1> operator<=(const fw_uint &o) const
    { return fw_uint<1>(bits <= o.bits); }

    THLS_INLINE_STRONG fw_uint<1> operator<=(int o) const
    {
        assert(o>=0);
        return fw_uint<1>(bits <= o);
    }

    THLS_INLINE_STRONG fw_uint<1> operator==(const fw_uint &o) const
    { return fw_uint<1>(bits == o.bits); }

    THLS_INLINE_STRONG fw_uint<1> operator==(int o) const
    {
        assert(o>=0);
        return fw_uint<1>(bits == o);
    }

    THLS_INLINE_STRONG fw_uint<1> operator>=(const fw_uint &o) const
    { return fw_uint<1>(bits >= o.bits); }

    THLS_INLINE_STRONG fw_uint<1> operator>=(int o) const
    {
        assert(o>=0);
        return fw_uint<1>(bits >= o);
    }

    THLS_INLINE_STRONG fw_uint<1> operator>(const fw_uint &o) const
    { return fw_uint<1>(bits > o.bits); }

    THLS_INLINE_STRONG fw_uint<1> operator>(int o) const
    {
        assert(o>=0);
        return fw_uint<1>(bits > o);
    }

    THLS_INLINE_STRONG fw_uint<1> operator!=(const fw_uint &o) const
    { return fw_uint<1>(bits != o.bits); }

    THLS_INLINE_STRONG fw_uint<1> operator!=(int o) const
    {
        assert(o>=0);
        return fw_uint<1>(bits != o);
    }

    //////////////////////////////////////////////
    // Bitwise ops.

    THLS_INLINE_STRONG fw_uint operator~() const
    { return fw_uint(~bits, detail::init_from_bits()); }

    THLS_INLINE_STRONG fw_uint operator&(const fw_uint<W> &o) const
    {
        return fw_uint(bits&o.bits, detail::init_from_bits());
    }

    THLS_INLINE_STRONG fw_uint operator&(int b) const
    {
        assert(b>=0);
        return fw_uint(bits_t(bits&b), detail::init_from_bits());
    }

    THLS_INLINE_STRONG fw_uint operator|(const fw_uint<W> &o) const
    {
        return fw_uint<W>(bits|o.bits, detail::init_from_bits());
    }

    THLS_INLINE_STRONG fw_uint operator|(int b) const
    {
        assert(b>=0);
        return fw_uint(bits_t(bits|b), detail::init_from_bits());
    }

    THLS_INLINE_STRONG fw_uint operator^(const fw_uint<W> &o) const
    {
        return fw_uint<W>(bits^o.bits, detail::init_from_bits());
    }

    THLS_INLINE_STRONG fw_uint operator^(int b) const
    {
        assert(b>=0);
        return fw_uint(bits_t(bits^b), detail::init_from_bits());
    }


    THLS_INLINE_STRONG fw_uint operator>>(int dist) const
    {
        assert(0<=dist && dist<W);
        ac_uint<thls_ctLog2Ceil(W)> rdist(dist);
        bits_t res(bits>>rdist);
        return fw_uint(res);
    }

    THLS_INLINE_STRONG fw_uint operator<<(int dist) const
    {
        assert(0<=dist && dist<W);
        ac_uint<thls_ctLog2Ceil(W)> rdist(dist);
        bits_t res(bits<<rdist);
        return fw_uint(res);
    }

#ifndef THLS_SYNTHESIS
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

    void to_mpz_t(mpz_t res) const
    {
        static_assert(sizeof(unsigned long)>=4, "Must have 32-bit or bigger longs");
        mpz_set_ui(res, 0);
        int todo=W;
        int offset=0;
        mpz_t tmp;
        mpz_init(tmp);
        while(todo>0){
            uint32_t curr=(bits>>offset).to_ulong();
            mpz_set_ui(tmp, curr);
            mpz_mul_2exp(tmp, tmp, offset);
            mpz_add(res, res, tmp);
            offset+=32;
            todo-=32;
        }
        mpz_clear(tmp);
    }
#endif

    THLS_INLINE int to_int() const
    {
        return (int)bits;
    }

    THLS_INLINE uint64_t to_uint64() const
    {
        assert(W<=64);
        return bits.to_uint64();
    }

    THLS_INLINE uint32_t to_uint32() const
    {
        assert(W<=32);
        return bits.to_uint64();
    }

    THLS_INLINE_STRONG bool to_bool() const
    {
        assert(W==1);
        return bits==1;
    }
};


template<int HI,int LO,int W>
THLS_INLINE_STRONG  fw_uint<HI-LO+1> get_bits(const fw_uint<W> &x)
{
	static const int SafeRW = (HI-LO+1 <= 0) ? 1 : (HI-LO+1);
    ac_uint<SafeRW> res = x.bits.template slc<SafeRW>(LO);
    return fw_uint<HI-LO+1>(res);
}


// NOTE: There was a nice C++11 implementation of concat, but
// I dropped it for simplicity and compiler compatibility. At
// some point it could be brought back, but this will do for now.

template<int WA>
THLS_INLINE_STRONG  fw_uint<WA> concat(const fw_uint<WA> &a)
{ return a; }

namespace detail
{
    template<int WA,int WB>
    struct concat_two
    {
    	THLS_INLINE_STRONG static fw_uint<WA+WB> go(const fw_uint<WA> &a, const fw_uint<WB> &b)
        {
            assert(WA>0);
            assert(WB>0);
            ac_uint<WA+WB> res(0);
            res.set_slc(0, b.bits);
            res.set_slc(WB, a.bits);
            return fw_uint<WA+WB>(res, detail::init_from_bits());
        }
    };

    template<int WA>
    struct concat_two<WA,0>
    {
    	THLS_INLINE_STRONG static fw_uint<WA> go(const fw_uint<WA> &a, const fw_uint<0> &b)
        {
            assert(WA>0);
            return a;
        }
    };

    template<int WB>
    struct concat_two<0,WB>
    {
    	THLS_INLINE_STRONG static fw_uint<WB> go(const fw_uint<0> &a, const fw_uint<WB> &b)
        {
            assert(WB>0);
            return b;
        }
    };

    template<>
    struct concat_two<0,0>
    {
    	THLS_INLINE_STRONG static fw_uint<0> go(const fw_uint<0> &a, const fw_uint<0> &b)
        {
            return a;
        }
    };
};

template<int WA,int WB>
THLS_INLINE_STRONG  fw_uint<WA+WB> concat(const fw_uint<WA> &a, const fw_uint<WB> &b)
{
    // We have to deal with the case where W==0. Under my type system it is legal to
    // instantiate zero width (but not negative width) types.
    return detail::concat_two<WA,WB>::go(a,b);
}

template<int WA,int WB,int WC>
THLS_INLINE_STRONG  fw_uint<WA+WB+WC> concat(const fw_uint<WA> &a, const fw_uint<WB> &b, const fw_uint<WC> &c)
{
	return concat(a,concat(b,c));
}

template<int WA,int WB,int WC,int WD>
THLS_INLINE_STRONG  fw_uint<WA+WB+WC+WD> concat(const fw_uint<WA> &a, const fw_uint<WB> &b, const fw_uint<WC> &c, const fw_uint<WD> &d)
{
	return concat(concat(a,b),concat(c,d));
}

template<int WD,int WS>
THLS_INLINE_STRONG fw_uint<WD> checked_cast(const fw_uint<WS> &s)
{
    static const int SafeWD=WD<=0 ? 1 : WD;
    if(WD==WS){
        return fw_uint<WD>(ac_uint<SafeWD>(s.bits));
    }else{
        assert(0);
        return ~fw_uint<WD>(); // Poison with ones
    }
}

template<int W>
THLS_INLINE_STRONG fw_uint<2*W> square(const fw_uint<W> &o)
{
	static const int SafeRW = (W <= 0) ? 1 : (2*W);
	ac_uint<SafeRW> v=o.bits*o.bits;
    return fw_uint<2*W>(v, detail::init_from_bits());
}



}; // thls

#endif
