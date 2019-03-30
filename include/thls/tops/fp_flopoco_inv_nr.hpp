#ifndef thls_fp_flopoco_inv_nr_hpp
#define thls_fp_flopoco_inv_nr_hpp

#include "thls/tops/fp_flopoco.hpp"
#include "thls/tops/fp_promote.hpp"

namespace thls
{




    template<int diff>
    struct round_frac_impl;

    template<>
    struct round_frac_impl<0>
            {
        template<int wFR, int wFX>
        static void go(fw_uint<wFR> &res, const fw_uint<wFX> &x, fw_uint<1> &overflow)
        {
            static_assert(wFR>=wFX, "Assumption");
            overflow=fw_uint<1>(0);
            res=zpad_lo<wFR-wFX>(x);
        }
    };

    template<>
    struct round_frac_impl<1>
    {
        template<int wFR, int wFX>
        static void go(fw_uint<wFR> &res, const fw_uint<wFX> &x, fw_uint<1> &overflow)
        {
            static_assert(wFR+1==wFX, "Assumption");
            auto lsb=get_bit<1>(x);
            auto guard=get_bit<0>(x);
            auto extres=add_cin(zpad_hi<1>(drop_lsb(x)), lsb&guard);
            overflow=take_msb(extres);
            res=drop_msb(extres);
        }
    };

    template<>
    struct round_frac_impl<2>
    {
        template<int wFR, int wFX>
        static void go(fw_uint<wFR> &res, const fw_uint<wFX> &x, fw_uint<1> &overflow)
        {
            static_assert(wFR+2<=wFX, "Assumption");
            const int D=wFX-wFR;
            auto happy=take_msbs<wFR>(x);
            auto lsb=get_bit<D>(x);
            auto guard=get_bit<D-1>(x);
            auto sticky=take_lsbs<D-2>(x)!=zg<D-2>();
            auto extres=add_cin(zpad_hi<1>(happy), lsb.to_bool() ? guard : guard&sticky  );
            overflow=take_msb(extres);
            res=drop_msb(extres);
        }
    };

    template<int wFR, int wFX>
    fw_uint<wFR> round_frac(const fw_uint<wFX> &x, fw_uint<1> &overflow)
    {
        const int C = wFR>=wFX ? 0 : wFR+1==wFX ? 1 : 2;
        typedef round_frac_impl<C> impl;
        fw_uint<wFR> res;
        impl::go(res,x,overflow);
        return res;
    }

    template<int _MSB,int _LSB>
    class fw_fix
    {
    public:
        static const int LSB=_LSB;
        static const int MSB=_MSB;
            static const int W=MSB-LSB+1;


            double to_double() const
            {
                return ldexp(bits.to_uint64(),LSB);
            }


        explicit fw_fix(const fw_uint<W> &_init)
                : bits(_init)
        {}

        explicit fw_fix(uint64_t init)
                : bits(fw_uint<W>::from_bits(init))
        {}

        fw_uint<W> bits;
    };

    template<int MSB1,int LSB1, int MSB2,int LSB2>
    fw_fix<MSB1+MSB2+1,LSB1+LSB2> operator*(const fw_fix<MSB1,LSB1> &x, const fw_fix<MSB2,LSB2> &y)
    { return fw_fix<MSB1+MSB2+1,LSB1+LSB2>(x.bits*y.bits); }

    template<int MSB,int LSB>
    fw_fix<MSB,LSB> operator-(const fw_fix<MSB,LSB> &x, const fw_fix<MSB,LSB> &y)
    { return fw_fix<MSB,LSB>(x.bits+y.bits); }

    template<int MSB0,int LSB0,int MSB1,int LSB1>
    fw_fix<thls_ctMax(MSB0,MSB1),thls_ctMin(LSB0,LSB1)> sub_match(const fw_fix<MSB0,LSB0> &x, const fw_fix<MSB1,LSB1> &y) {
        const int MSBR = thls_ctMax(MSB0, MSB1);
        const int LSBR = thls_ctMin(LSB0, LSB1);
        return fw_fix<MSBR, LSBR>(zpad_hi<MSBR - MSB0>(zpad_lo<LSB0 - LSBR>(x.bits)) -
                                  zpad_hi<MSBR - MSB1>(zpad_lo<LSB1 - LSBR>(y.bits)));
    }


    template<int PLACES,int MSB,int LSB>
    fw_fix<MSB+PLACES,LSB+PLACES> ldexp_const(const fw_fix<MSB,LSB> &x)
    {
        return fw_fix<MSB+PLACES,LSB+PLACES>(x.bits);
    }

    template<int LSBR,int MSB,int LSB>
    fw_fix<MSB,LSBR> round_to_lsb(const fw_fix<MSB,LSB> &x, fw_uint<1> &overflowed)
    {
        const int WR=fw_fix<MSB,LSBR>::W;
        return fw_fix<MSB,LSBR>(round_frac<WR>(x.bits, overflowed));
    }

    template<int LSBR,int MSB,int LSB>
    fw_fix<MSB,LSBR> trunc_to_lsb(const fw_fix<MSB,LSB> &x)
    {
        const int WR=fw_fix<MSB,LSBR>::W;
        return fw_fix<MSB,LSBR>(take_msbs<WR>(x.bits));
    }

    /*
    MSB=W+S
    MSB'=W'+S
    W'=MSB-S
     */

    template<int MSBR,int MSB,int LSB>
    fw_fix<MSBR,LSB> truncate_to_msb(const fw_fix<MSB,LSB> &x, fw_uint<1> &overflowed) {
        static_assert(MSBR<=MSB, "Assumption");
        overflowed = take_msbs<MSB-MSBR>(x.bits) != 0;
        return fw_fix<MSBR,LSB>(drop_msbs<MSB-MSBR>(x.bits));
    }


    template<int MSBR,int LSBR, int MSBZ,int LSBZ, int MSBY,int LSBY>
    THLS_INLINE fw_fix<MSBR,LSBR> inv_frac_newton_step(const fw_fix<MSBZ,LSBZ> &z, const fw_fix<MSBY,LSBY> &y)
    {
        // PRE: z=1/y + O(eps)
        // z' = 2*z - z*z * y;
        // POST:  z' = 1/y + O(eps^2)

        //std::cerr<<"y = "<<y.to_double()<<"\n";

        auto z2=z*z;
        //std::cerr<<"z = "<<z.to_double()<<", z*z="<<z2.to_double()<<"\n";
        auto z2y=z2*y;
        auto dz=ldexp_const<1>(z);
        //std::cerr<<"zp = "<<dz.to_double()<<", x2*y="<<z2y.to_double()<<"\n";
        auto zp=sub_match(dz,z2y);
        //std::cerr<<"zp = "<<zp.to_double()<<"\n";

        // Output should be in [0.5,1]
        fw_uint<1> overflow1;
        auto rounded=round_to_lsb<LSBR>(zp, overflow1);
        assert(!overflow1.to_bool());
        //auto rounded=trunc_to_lsb<LSBR>(zp);

        fw_uint<1> overflow2;
        auto res= truncate_to_msb<MSBR>( rounded, overflow2 );
        if(overflow2.to_bool()){
            std::cerr<<rounded.to_double()<<"\n";
        }
        assert(!overflow2.to_bool());
        return res;
    }

    template<int MSBR,int LSBR, int MSBI, int LSBI>
    THLS_INLINE fw_fix<MSBR,LSBR> inv_frac_starter_table(const fw_fix<MSBI,LSBI> &x)
    {
        static_assert(MSBI==0, "Expected index in range [1,2)");
        static_assert(MSBR==0, "Expected index in range (0.5,1]");

        const int WI=MSBI-LSBI+1;
        const int WR=MSBR-LSBR+1;

        static thls::lut<WR,WI-1> table(
                [](int i) -> fw_uint<WR>
                {
                    static_assert(MSBR-LSBR+1 <=52, "Can't do wider than double here." );

                    double a=1+ldexp(i,LSBI);
                    double b=1+ldexp(i+1,LSBI);
                    double v=(a+b)/(2*a*b);
                    //double v=1/a;
                    assert(0.5<=v && v<=1.0);
                    double vr=round(ldexp(v, -LSBR));
                    if(i==0 && (vr==(1<<(WR-1)))){
                        vr--;
                    }
                    //std::cerr<<"i="<<i<<" -> v="<<v<<", vr="<<vr<<", WR="<<WR<<"\n";
                    assert( (1<<(WR-2)) <= vr );
                    assert( vr < (1<<(WR-1)) );
                    return fw_uint<WR>::from_bits(uint64_t(vr));
                }
        );
        assert(take_msb(x.bits).to_bool()); // Should always have explicit bit
        auto index=drop_msb(x.bits);
        auto bits=table(index);
        auto res= fw_fix<MSBR,LSBR>(bits);
        //std::cerr<<"  guess="<<res.to_double()<<"\n";
        return res;
    }

    template<int LSBR,int LSBY>
    fw_fix<0,LSBR> inv_frac_table_newton_iter1(const fw_fix<0,LSBY> &x)
    {
        const int BITS1=-LSBR;
        const int BITS0=(BITS1+1)/2;
        const int G=1;

        const int LSBG1=-BITS0-G;

        auto g=inv_frac_starter_table<0,LSBG1>(trunc_to_lsb<LSBG1>(x));
        auto r=inv_frac_newton_step<0,LSBR>(g,x);

        return r;
    }

    template<int LSBR,int LSBY>
    fw_fix<0,LSBR> inv_frac_table_newton_iter2(const fw_fix<0,LSBY> &x)
    {
        const int BITS2=-LSBR;
        // I think this is an overestimate of the precision needed, but passes tests
        const int BITS1=(BITS2+2)/2;
        const int BITS0=(BITS1+2)/2;
        const int G=1;

        const int LSBG1=-BITS0-G;
        const int LSBG2=-BITS1-G;

        auto g=inv_frac_starter_table<0,LSBG1>(trunc_to_lsb<LSBG1>(x));
        auto g2=inv_frac_newton_step<0,LSBG2>(g,x);
        auto r=inv_frac_newton_step<0,LSBR>(g2,x);

        return r;
    }

    template<int LSBR,int LSBY>
    fw_fix<0,LSBR> inv_frac_table_newton_iter3(const fw_fix<0,LSBY> &x)
    {
        const int BITS3=-LSBR;
        const int BITS2=(BITS3+1)/2;
        const int BITS1=(BITS2+1)/2;
        const int BITS0=(BITS1+1)/2;
        const int G=2;

        const int LSBG1=-BITS0-G;
        const int LSBG2=-BITS1-G;
        const int LSBG3=-BITS2-G;

        auto g=inv_frac_starter_table<0,LSBG1>(trunc_to_lsb<LSBG1>(x));
        auto g2=inv_frac_newton_step<0,LSBG2>(g,x);
        auto g3=inv_frac_newton_step<0,LSBG3>(g2,x);
        auto r=inv_frac_newton_step<0,LSBR>(g3,x);

        return r;
    }

    /* This implements a rounded table directly. Main diff to the starter table
     * is that we don't try to pick best value in the range, as only one value
     * every comes in per entry.
     */
    template<int MSBR,int LSBR, int MSBI, int LSBI>
    THLS_INLINE fw_fix<MSBR,LSBR> inv_frac_direct_table(const fw_fix<MSBI,LSBI> &x)
    {
        static_assert(MSBI==0, "Expected index in range [1,2)");
        static_assert(MSBR==0, "Expected index in range (0.5,1]");

        const int WI=MSBI-LSBI+1;
        const int WR=MSBR-LSBR+1;

        static thls::lut<WR,WI-1> table(
                [](int i) -> fw_uint<WR>
                {
                    static_assert(MSBR-LSBR+1 <=52, "Can't do wider than double here." );

                    double a=1+ldexp(i,LSBI);
                    double v=1/a;
                    assert(0.5<=v && v<=1.0);
                    double vr=round(ldexp(v, -LSBR));
                    /*if(i==0 && (vr==(1<<(WR-1)))){
                        vr--;
                    }*/
                    //std::cerr<<"i="<<i<<" -> v="<<v<<", vr="<<vr<<", WR="<<WR<<"\n";
                    //assert( (1<<(WR-2)) <= vr );
                    //assert( vr < (1<<(WR-1)) );
                    return fw_uint<WR>::from_bits(uint64_t(vr));
                }
        );
        assert(take_msb(x.bits).to_bool()); // Should always have explicit bit
        auto index=drop_msb(x.bits);
        auto bits=table(index);
        auto res= fw_fix<MSBR,LSBR>(bits);
        //std::cerr<<"  guess="<<res.to_double()<<"\n";
        return res;
    }

    template<int TIter>
    struct inv_frac_table_newton_iter_dispatch;

    template<>
    struct inv_frac_table_newton_iter_dispatch<0>
    {
        template<int LSBR,int LSBY>
        static fw_fix<0,LSBR> go(const fw_fix<0,LSBY> &x)
        { return inv_frac_direct_table<LSBR>(x); }
    };

    template<>
    struct inv_frac_table_newton_iter_dispatch<1>
    {
        template<int LSBR,int LSBY>
        static fw_fix<0,LSBR> go(const fw_fix<0,LSBY> &x)
        { return inv_frac_table_newton_iter1<LSBR>(x); }
    };

    template<>
    struct inv_frac_table_newton_iter_dispatch<2>
    {
        template<int LSBR,int LSBY>
        static fw_fix<0,LSBR> go(const fw_fix<0,LSBY> &x)
        { return inv_frac_table_newton_iter2<LSBR>(x); }
    };

    template<>
    struct inv_frac_table_newton_iter_dispatch<3>
    {
        template<int LSBR,int LSBY>
        static fw_fix<0,LSBR> go(const fw_fix<0,LSBY> &x)
        { return inv_frac_table_newton_iter3<LSBR>(x); }
    };

    template<int LSBR,int LSBY>
    static fw_fix<0,LSBR> inv_frac_table_newton(const fw_fix<0,LSBY> &x)
    {
        const int iterations =
                LSBY >= -7 ? 0 : // Always go table if the input width is small
                LSBR >= -7 ? 0 : // Or if the input is small
                LSBR >= -15 ? 1 : // Single iteration
                LSBR >= -31 ? 2 : // Double iteration
                LSBR >= -63 ? 3 : // Triple iteration
                4; // Force a compiler error
        return inv_frac_table_newton_iter_dispatch<iterations>::template go<LSBR>(x);
    }


template<int wER,int wFR, int wEY,int wFY>
THLS_INLINE fp_flopoco<wER,wFR> inv_table_newton(const fp_flopoco<wEY,wFY> &y, int DEBUG)
{
    static_assert(wER==wEY, "Currently exponents must match.");

    fw_uint<2> flags=y.get_flags();
    fw_uint<1> sign=y.get_sign();
    fw_fix<0,-wFY> frac_in_fix(opad_hi<1>(y.get_frac_bits()));
    fw_fix<0,-wFR-1> frac_out_fix=inv_frac_table_newton<-wFR-1>(frac_in_fix);
    fw_uint<wFR> frac_out=drop_msbs<2>(frac_out_fix.bits);
    fw_uint<wER> exp=(og<wEY>() - fw_uint<wEY>(2)) - y.get_exp_bits();
    //if((y.get_frac_bits()==zg<wFY>()).to_bool()){
    if(take_msb(frac_out_fix.bits).to_bool()){
        exp=exp+fw_uint<wER>(1);
    }

    switch(flags.to_int()){
    case 0:
        flags=fw_uint<2>(0b10);
        break;
    case 1:
        if((y.get_exp_bits()==og<wEY>()).to_bool()){
            flags=fw_uint<2>(0b00);
        }else {
            flags = fw_uint<2>(0b01);
        }
        break;
    case 2:
        flags=fw_uint<2>(0);
        break;
    case 3:
        flags=fw_uint<2>(0b11);
        break;
    }

    return fp_flopoco<wER,wFR>(concat(
        flags,sign,exp,frac_out
    ));
}

    template<int wER,int wFR, int wEY,int wFY>
    THLS_INLINE fp_flopoco<wER,wFR> inv_direct(const fp_flopoco<wEY,wFY> &y, int DEBUG)
    {
        static_assert(wER==wEY, "Currently exponents must match.");

        fw_uint<2> flags=y.get_flags();
        fw_uint<1> sign=y.get_sign();
        fw_fix<0,-wFY> frac_in_fix(opad_hi<1>(y.get_frac_bits()));
        auto frac_out_fix=inv_frac_direct_table<0,-wFR-1,0,-wFY>(frac_in_fix);
        fw_uint<wFR> frac=take_lsbs<wFR>(frac_out_fix.bits);
        fw_uint<wER> exp=(og<wEY>() - fw_uint<wEY>(2)) - y.get_exp_bits();
        if((y.get_frac_bits()==zg<wFY>()).to_bool()){
            exp=exp+fw_uint<wER>(1);
        }

        switch(flags.to_int()){
            case 0:
                flags=fw_uint<2>(0b10);
                break;
            case 1:
                if((y.get_exp_bits()==og<wEY>()).to_bool()){
                    flags=fw_uint<2>(0b00);
                }else {
                    flags = fw_uint<2>(0b01);
                }
                break;
            case 2:
                flags=fw_uint<2>(0);
                break;
            case 3:
                flags=fw_uint<2>(0b11);
                break;
        }

        return fp_flopoco<wER,wFR>(concat(
                flags,sign,exp,frac
        ));
    }

}; // thls

#endif
