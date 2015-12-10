#ifndef thls_bit_heap_hpp
#define thls_bit_heap_hpp

#include "fw_uint.hpp"

#ifndef THLS_SYNTHESIS
#include <iostream>
#include <mpfr.h>
#endif

namespace thls{

template<int H,class LSBs>
struct bit_heap_link
{
    static const int height = H;
    static const int max_height = thls_ctMax(H,LSBs::max_height);
    static const int width = LSBs::width+1;

    typedef LSBs lsbs_t;

    fw_uint<H> bits;
    LSBs lsbs;

    THLS_INLINE bit_heap_link(const fw_uint<H> &_msbs, const LSBs &_lsbs)
        : bits(_msbs)
        , lsbs(_lsbs)
    {}
};

template<int H>
struct bit_heap_root
{
    static const int height = H;
    static const int max_height = H;
    static const int width = 1;

    fw_uint<H> bits;

    THLS_INLINE bit_heap_root(const fw_uint<H> &_msbs)
        : bits(_msbs)
    {}
};

namespace detail{

    template<int W>
    struct uint_to_bit_heap
    {
        typedef bit_heap_link<1,typename uint_to_bit_heap<W-1>::heap_t> heap_t;

        THLS_INLINE static heap_t build(const fw_uint<W> &x)
        {
            return heap_t(
                take_msb(x),
                uint_to_bit_heap<W-1>::build(drop_msb(x))
            );
        }
    };

    template<>
    struct uint_to_bit_heap<1>
    {
        typedef bit_heap_root<1> heap_t;

        THLS_INLINE static heap_t build(const fw_uint<1> &x)
        {
            return heap_t(x);
        }
    };

};

template<int W>
THLS_INLINE  typename detail::uint_to_bit_heap<W>::heap_t fw_uint_to_bit_heap(const fw_uint<W> &x)
{
    return detail::uint_to_bit_heap<W>::build(x);
}

#ifndef THLS_SYNTHESIS
namespace detail
{
    template<class T>
    struct bit_heap_to_mpz;

    template<int H, class T>
    struct bit_heap_to_mpz<bit_heap_link<H,T> >
    {
        THLS_INLINE static void build(mpz_t dst, const bit_heap_link<H,T> &x)
        {
            //std::cerr<<x.bits<<"\n";
            auto tmp=x.bits;
            for(int i=0;i<H;i++){
                //std::cerr<<"  lsb = "<<take_lsb(tmp)<<"\n";
                if(take_lsb(tmp)){
                    mpz_add_ui(dst, dst, 1);
                }
                if(H>1){ // Avoid x>>1 when x is one bit
                    tmp= tmp>>1;
                }
                //mpfr_fprintf(stderr, "  dst=%Zd\n", dst);
            }
            mpz_mul_2exp(dst, dst, 1);
            bit_heap_to_mpz<T>::build(dst, x.lsbs);
        }
    };

    template<int H>
    struct bit_heap_to_mpz<bit_heap_root<H> >
    {
        THLS_INLINE static void build(mpz_t dst, const bit_heap_root<H> &x)
        {
            //std::cerr<<x.bits<<"\n";
            auto tmp=x.bits;
            for(int i=0;i<H;i++){
                if(take_lsb(tmp)){
                    mpz_add_ui(dst, dst, 1);
                }
                if(H>1){ // Avoid x>>1 when x is one bit
                    tmp=tmp>>1;
                }
            }
        }
    };
};

template<class T>
void bit_heap_to_mpz(mpz_t r, const T &heap)
{
    mpz_set_ui(r, 0);
    detail::bit_heap_to_mpz<T>::build(r, heap);

    // Bit heaps produce results without carry.
    // The output width is the same as the input.
    mpz_tdiv_r_2exp(r, r, T::width);
}

namespace detail
{
    template<int H>
    void write_heights(std::ostream &dst, const bit_heap_root<H> &)
    {
        if(H>9){
            dst<<"^";
        }else{
            dst<<H;
        }
    }

    template<int H,class T>
    void write_heights(std::ostream &dst, const bit_heap_link<H,T> &x)
    {
        if(H>9){
            dst<<"^";
        }else{
            dst<<H;
        }
        write_heights(dst, x.lsbs);
    }
};

template<int H,class T>
std::ostream &operator<<(std::ostream &dst, const bit_heap_link<H,T> &x)
{
    typedef bit_heap_link<H,T> this_t;

    dst<<"BH[W="<<this_t::width<<",MH="<<this_t::max_height<<",bits=0b";

    mpz_t tmp;
    mpz_init(tmp);
    bit_heap_to_mpz(tmp, x);
    char *ss=mpz_get_str (NULL, 2, tmp);
    dst<<ss;
    free(ss);
    mpz_clear(tmp);

    dst<<",H=";
    detail::write_heights(dst,x);
    dst<<"]";

    return dst;
}
#endif

namespace detail
{

    template<class TA,class TB>
    struct merge_equal_width_bit_heaps
    {
        static_assert(TA::width==TB::width, "Widths must already be equal length.");

        typedef bit_heap_link<TA::height+TB::height, typename merge_equal_width_bit_heaps<typename TA::lsbs_t,typename TB::lsbs_t>::heap_t> heap_t;

        THLS_INLINE static heap_t build(const TA &a, const TB &b)
        {
            return heap_t(
                concat(a.bits,b.bits),
                merge_equal_width_bit_heaps<typename TA::lsbs_t,typename TB::lsbs_t>::build(a.lsbs,b.lsbs)
            );
        }
    };

    template<int WA,int WB>
    struct merge_equal_width_bit_heaps<bit_heap_root<WA>,bit_heap_root<WB> >
    {
        typedef bit_heap_root<WA+WB> heap_t;

        THLS_INLINE static heap_t build(const bit_heap_root<WA> &a, const bit_heap_root<WB> &b)
        {
            return heap_t(
                concat(a.bits,b.bits)
            );
        }
    };


    template<int Todo, class TA>
    struct pad_bit_heap_impl
    {
        static_assert(Todo>=0, "Can't extend to smaller width");

        typedef bit_heap_link<0,typename pad_bit_heap_impl<Todo-1,TA>::heap_t > heap_t;

        THLS_INLINE static const heap_t build(const TA &a)
        { return heap_t( zg<0>(), pad_bit_heap_impl<Todo-1,TA>::build(a) ); }
    };

    template<class TA>
    struct pad_bit_heap_impl<0,TA>
    {
        typedef TA heap_t;

        THLS_INLINE static const TA &build(const TA &a)
        { return a; }
    };


    template<int places>
    struct bit_heap_zeros
    {
        typedef bit_heap_link<0,typename bit_heap_zeros<places-1>::heap_t> heap_t;

        THLS_INLINE static heap_t build()
        {
            return heap_t(zg<0>(),bit_heap_zeros<places-1>::build());
        }
    };

    template<>
    struct bit_heap_zeros<1>
    {
        typedef bit_heap_root<0> heap_t;

        THLS_INLINE static heap_t build()
        {
            return heap_t(zg<0>());
        }
    };


    template<int places, class T>
    struct bit_heap_shift_left_impl;

    template<int places, int H, class T>
    struct bit_heap_shift_left_impl<places,bit_heap_link<H,T> >
    {
        typedef bit_heap_shift_left_impl<places,T> next_t;

        typedef bit_heap_link<H,typename next_t::heap_t> heap_t;

        THLS_INLINE static heap_t build(const bit_heap_link<H,T> &x)
        {
            return heap_t(x.bits,next_t::build(x.lsbs));
        }
    };

    template<int H, class T>
    struct bit_heap_shift_left_impl<0,bit_heap_link<H,T> >
    {
        typedef bit_heap_link<H,T> heap_t;

        THLS_INLINE static heap_t build(const bit_heap_link<H,T> &x)
        { return x; }
    };

    template<int places, int H>
    struct bit_heap_shift_left_impl<places,bit_heap_root<H> >
    {
        typedef bit_heap_link<H,typename bit_heap_zeros<places>::heap_t> heap_t;

        THLS_INLINE static heap_t build(const bit_heap_root<H> &x)
        {
            return heap_t(x.bits,bit_heap_zeros<places>::build());
        }
    };

    template<int H>
    struct bit_heap_shift_left_impl<0,bit_heap_root<H> >
    {
        typedef bit_heap_root<H> heap_t;

        THLS_INLINE static heap_t build(const bit_heap_root<H> &x)
        { return x; }
    };


    template<class TA,class TB>
    struct merge_bit_heaps_impl
    {
        static const int W=thls_ctMax(TA::width,TB::width);

        typedef typename pad_bit_heap_impl<W-TA::width,TA>::heap_t TA_pad;
        typedef typename pad_bit_heap_impl<W-TB::width,TB>::heap_t TB_pad;

        typedef typename merge_equal_width_bit_heaps<TA_pad,TB_pad>::heap_t heap_t;

        THLS_INLINE static heap_t build(const TA &a, const TB &b)
        {
            return merge_equal_width_bit_heaps<TA_pad,TB_pad>::build(
                detail::pad_bit_heap_impl<W-TA::width,TA>::build(a),
                detail::pad_bit_heap_impl<W-TB::width,TB>::build(b)
            );
        }
    };
};

template<int W,class T>
THLS_INLINE typename detail::pad_bit_heap_impl<W-T::width,T>::heap_t pad_bit_heap(const T &x)
{
    return detail::pad_bit_heap_impl<W-T::width,T>::build(x);
}

template<class TA, class TB>
THLS_INLINE typename detail::merge_bit_heaps_impl<TA,TB>::heap_t merge_bit_heaps(const TA &a, const TB &b)
{
    return detail::merge_bit_heaps_impl<TA,TB>::build(a,b);
}


namespace detail
{

/*
    We only do 3:2 compressors.

    If h(i) is the height at bit i, we keep compressing
    till max(h(w-1),..,h(i))<=2.

    nC(i),nS(i) = h(i)==0 : (0,0)  // do nothing
                  h(i)==1 : (0,1)  // do nothing
                  h(i)==2 : (1,1)  // Single 2:2
                  h(i)==3 : (1,1)     // single 3:2
                  h(i)==4 : (1,2)     // single 3:2, plus copy one
                  h(i)==5 : (2,2)     // single 3:2, plus single 2:2
                  h(i)==6 : (2,2)    // two 3:2
                  h(i)==7 : (2,3)     // two 3:2, plus copy one
                  h(i)==8 : (3,3)    // two 3:2, one 2:2
                  h(i)==9 : (3,3)    // three 3:2
                  h(i)==10 :(3,4)    // three 3:2, one copy
                  h(i)==11: (4,4)    // three 3:2, one 2:2
    nC(i) = (h(i)+1)/3)
    nS(i) = (h(i)+2)/3)

    h'(i) = nS(i) + nC(i-1), were nC(-1) = 0
*/


template<class A,class B>
struct ipair
{
	A first;
	B second;

	THLS_INLINE ipair(const A &a, const B &b)
		: first(a)
		, second(b)
	{}
};

template<class A,class B>
THLS_INLINE ipair<A,B> make_ipair(const A &a, const B &b)
{
	return ipair<A,B>(a,b);
}

THLS_INLINE ipair<fw_uint<1>,fw_uint<1> > compress_3_2(const fw_uint<3> &x)
{
    auto tmp=zpad_hi<1>(get_bit<0>(x))+zpad_hi<1>(get_bit<1>(x))+zpad_hi<1>(get_bit<2>(x));
    return make_ipair(
        get_bit<1>(tmp),
        get_bit<0>(tmp)
    );
}

template<int W>
struct compress_column_impl
{
    // The base case is to apply a 3:2 compressor (consuming 3 bits),
    // then pass the rest on

    typedef compress_column_impl<W-3> base_t;

    typedef fw_uint<base_t::carry_t::width+1> carry_t;
    typedef fw_uint<base_t::sum_t::width+1> sum_t;

    THLS_INLINE static ipair<carry_t,sum_t> build(const fw_uint<W> &x)
    {
        auto local=compress_3_2(take_lsbs<3>(x));
        auto next=base_t::build(drop_lsbs<3>(x));
        return make_ipair(
            concat(local.first,next.first),
            concat(local.second,next.second)
        );
    }
};

template<>
struct compress_column_impl<0>
{
    typedef fw_uint<0> carry_t;
    typedef fw_uint<0> sum_t;

    THLS_INLINE static ipair<carry_t,sum_t> build(const fw_uint<0> &)
    { return make_ipair(zg<0>(),zg<0>()); }
};

template<>
struct compress_column_impl<1>
{
    typedef fw_uint<0> carry_t;
    typedef fw_uint<1> sum_t;

    THLS_INLINE static ipair<carry_t,sum_t> build(const fw_uint<1> &x)
    { return make_ipair(zg<0>(),x); }
};

template<>
struct compress_column_impl<2>
{
    typedef fw_uint<1> carry_t;
    typedef fw_uint<1> sum_t;

    THLS_INLINE static ipair<carry_t,sum_t> build(const fw_uint<2> &x)
    { return make_ipair(take_msb(x)&take_lsb(x),take_msb(x)^take_lsb(x)); }
};


template<class T>
struct compress_step;

template<int H>
struct compress_step<bit_heap_root<H> >
{
    typedef bit_heap_root<H> this_t;

    static const int nCarry = (H+1)/3;
    static const int nSum = (H+2)/3;
    static const int nHeight = nSum;

    typedef bit_heap_root<nHeight> next_t;
    typedef fw_uint<nCarry> carry_t;

    THLS_INLINE static ipair<carry_t,next_t> build(const this_t &x)
    {
        ipair<fw_uint<nCarry>,fw_uint<nSum> > cols=compress_column_impl<H>::build(x.bits);
        return make_ipair(
            cols.first,
            bit_heap_root<nSum>(cols.second)
        );
    }
};

template<int H,class LSBs>
struct compress_step<bit_heap_link<H,LSBs> >
{
    typedef bit_heap_link<H,LSBs> this_t;

    typedef compress_step<LSBs> compress_next;

    static const int nCarry = (H+1)/3;
    static const int nSum = (H+2)/3;
    static const int nHeight = nSum + compress_next::nCarry;

    typedef bit_heap_link<nHeight, typename compress_next::next_t> next_t;
    typedef fw_uint<nCarry> carry_t;

    THLS_INLINE static ipair<carry_t,next_t> build(const this_t &x)
    {
        auto cols=compress_column_impl<H>::build(x.bits); // first is carry, second is sum
        auto next=compress_next::build(x.lsbs); // first is carry, second is bit-heap
        carry_t r_carry=cols.first;
        next_t r_next=next_t(
            concat(cols.second,next.first),
            next.second
        );
        return make_ipair(
            r_carry,
            r_next
        );
    }
};



template<bool Done,class T>
struct compress;

template<bool Done, class T>
struct compress{
    typedef typename compress_step<T>::next_t next_t;
    typedef typename compress< (next_t::max_height<=2), next_t>::final_t final_t;

    THLS_INLINE static final_t build(const T &x)
    {
        auto n=compress_step<T>::build(x);
        return compress< (next_t::max_height<=2), next_t>::build(n.second);
    }
};

template<class T>
struct compress<true,T>
{
    typedef T next_t;
    typedef T final_t;

    THLS_INLINE static const final_t &build(const T &x)
    { return x; }
};

}; // detail


template<class T>
THLS_INLINE typename detail::compress<T::max_height<=2,T>::final_t compress(const T &x)
{
    return detail::compress<T::max_height<=2,T>::build(x);
}

namespace detail
{
    template<class T>
    struct collapse;

    template<int H>
    struct collapse<bit_heap_root<H> >
    {
        //static_assert(false, "Collapse should only be used on heaps with H<=2.");
    };

    template<>
    struct collapse<bit_heap_root<0> >
    {
        THLS_INLINE static fw_uint<1> get_a(const bit_heap_root<0> &)
        { return zg<1>(); }

        THLS_INLINE static fw_uint<1> get_b(const bit_heap_root<0> &)
        { return zg<1>(); }
    };

    template<>
    struct collapse<bit_heap_root<1> >
    {
        THLS_INLINE static fw_uint<1> get_a(const bit_heap_root<1> &)
        { return zg<1>(); }

        THLS_INLINE static fw_uint<1> get_b(const bit_heap_root<1> &x)
        { return x.bits; }
    };

    template<>
    struct collapse<bit_heap_root<2> >
    {
        THLS_INLINE static fw_uint<1> get_a(const bit_heap_root<2> &x)
        { return get_bit<0>(x.bits); }

        THLS_INLINE static fw_uint<1> get_b(const bit_heap_root<2> &x)
        { return get_bit<1>(x.bits); }
    };

    template<int H,class T>
    struct collapse<bit_heap_link<H,T> >
    {
        //static_assert(false, "Collapse should only be used on heaps with H<=2.");
    };

    template<class T>
    struct collapse<bit_heap_link<0,T> >
    {
        THLS_INLINE static fw_uint<1+T::width> get_a(const bit_heap_link<0,T> &x)
        { return concat(zg<1>(), collapse<T>::get_a(x.lsbs)); }

        THLS_INLINE static fw_uint<1+T::width> get_b(const bit_heap_link<0,T> &x)
        { return concat(zg<1>(), collapse<T>::get_b(x.lsbs)); }
    };

    template<class T>
    struct collapse<bit_heap_link<1,T> >
    {
        THLS_INLINE static fw_uint<1+T::width> get_a(const bit_heap_link<1,T> &x)
        { return concat(get_bit<0>(x.bits), collapse<T>::get_a(x.lsbs)); }

        THLS_INLINE static fw_uint<1+T::width> get_b(const bit_heap_link<1,T> &x)
        { return concat(zg<1>(), collapse<T>::get_b(x.lsbs)); }
    };

    template<class T>
    struct collapse<bit_heap_link<2,T> >
    {
        THLS_INLINE static fw_uint<1+T::width> get_a(const bit_heap_link<2,T> &x)
        { return concat(get_bit<0>(x.bits), collapse<T>::get_a(x.lsbs)); }

        THLS_INLINE static fw_uint<1+T::width> get_b(const bit_heap_link<2,T> &x)
        { return concat(get_bit<1>(x.bits), collapse<T>::get_b(x.lsbs)); }
    };
};

template<class T>
THLS_INLINE fw_uint<T::width> bit_heap_collapse(const T &x)
{
    auto a=detail::collapse<T>::get_a(x);
    auto b=detail::collapse<T>::get_b(x);

    return a+b;
}

}; // thls

#endif
