#ifndef thls_constant_mult_v1_hpp
#define thls_constant_mult_v1_hpp

#include "thls/tops/fw_uint.hpp"

#include <random>
#include <typeinfo>

namespace thls {

    template<class TImpl, unsigned WR, unsigned WI, uint64_t C>
    void test_cmult_impl_WR_WI_C() {
        static_assert(WI > 0, "Must be at least 1 bit input.");
        static_assert(C > 0, "Constant must be strictly positive.");
        static_assert(WR <= 64, "Test needs specialisation for larger result sizes.");
        static_assert(WI <= 64, "Test needs specialisation for larger input sizes.");

        const unsigned log2_max_exhaust = 10;
        const unsigned log2_exhaust_test = thls_ctMin(log2_max_exhaust, WI);
        const unsigned log2_random_test = WI == (log2_exhaust_test) ? 0 : log2_max_exhaust;

        const uint64_t M = 0xFFFFFFFFFFFFFFFFull >> (64-WR);

        auto test = [](uint64_t x) {
            uint64_t got = TImpl::template go<WR, WI, C>(fw_uint<WI>::from_uint64(x)).to_uint64();
            uint64_t ref = (x * C) & M;
            if (got != ref) {
                auto print_value=[](std::ostream &dst, const char *name, uint64_t x) -> std::ostream &
                {
                    int w=dst.width();
                    dst.width(20);
                    dst<<name<<" = "<<x;
                    dst.width(16);
                    dst<<" = 0x"<<std::hex<<x<<std::dec;
                    dst.width(w);
                    return dst;
                };

                std::stringstream acc;
                acc<<"Mismatch: TImpl="<<typeid(TImpl).name()<<", WR="<<WR<<", WI="<<WI<<"\n";
                print_value(acc, "C", C)<<"\n";
                print_value(acc, "x", x)<<"\n";
                print_value(acc, "ref", ref)<<"\n";
                print_value(acc, "got", got)<<"\n";
                throw std::runtime_error(acc.str());
            }
        };

        for (unsigned i = 0; i < (1u << log2_exhaust_test); i++) {
            test(i);
        }

        std::mt19937_64 urng(1);
        for (unsigned i = 0; i < (1u << log2_random_test); i++) {
            test(urng() >> (64 - WI));
        }
    }

    template<class TImpl, unsigned WR, unsigned WI>
    void test_cmult_impl_WR_WI() {
        // about 33 combinations
        std::cerr<<" WR="<<WR<<", WI="<<WI<<"\n";

        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 1>();
        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 2>();
        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 3>();
        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 4>();
        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 5>();
        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 6>();
        
        #ifdef THLS_CMULT_LONG_TEST
        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 7>();
        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 8>();
        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 9>();
        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 10>();
        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 11>();
        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 12>();
        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 13>();
        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 14>();
        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 15>();
        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 16>();

        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 0b101010101>();
        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 0b010101010>();

        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 0b11001100>();
        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 0b01100110>();
        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 0b00110011>();

        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 0b11101110>();
        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 0b01110111>();

        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 0b11000110>();
        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 0b01100011>();

        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 0b11111111>();

        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 0b10001>();
        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 0b100001>();
        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 0b1000001>();
        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 0b10000001>();
        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 0b100000001>();
        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 0b1000000001>();
        test_cmult_impl_WR_WI_C<TImpl, WR, WI, 0b10000000001>();
#endif
    }

    template<class TImpl, unsigned WR>
    void test_cmult_impl_WR() {
        // 11 combniations

        test_cmult_impl_WR_WI<TImpl, WR, 1>();
        test_cmult_impl_WR_WI<TImpl, WR, 2>();
        test_cmult_impl_WR_WI<TImpl, WR, 3>();
        test_cmult_impl_WR_WI<TImpl, WR, 4>();
        test_cmult_impl_WR_WI<TImpl, WR, 5>();

#ifdef THLS_CMULT_LONG_TEST
        test_cmult_impl_WR_WI<TImpl, WR, 6>();
        test_cmult_impl_WR_WI<TImpl, WR, 7>();
        test_cmult_impl_WR_WI<TImpl, WR, 8>();

        test_cmult_impl_WR_WI<TImpl, WR, 15>();
        test_cmult_impl_WR_WI<TImpl, WR, 16>();
        test_cmult_impl_WR_WI<TImpl, WR, 17>();
#endif
    }

    template<class TImpl>
    void test_cmult_impl() {
        // in total 11*11*33 = ~4000 combinations. That's a lot of code...
        test_cmult_impl_WR<TImpl, 1>();
        test_cmult_impl_WR<TImpl, 2>();
        test_cmult_impl_WR<TImpl, 3>();
        test_cmult_impl_WR<TImpl, 4>();
        test_cmult_impl_WR<TImpl, 5>();
#if THLS_CMULT_LONG_TEST
        test_cmult_impl_WR<TImpl, 6>();
        test_cmult_impl_WR<TImpl, 7>();
        test_cmult_impl_WR<TImpl, 8>();
        test_cmult_impl_WR<TImpl, 15>();
        test_cmult_impl_WR<TImpl, 16>();
        test_cmult_impl_WR<TImpl, 17>();
#endif

        // Some random 32-bit points as well
        test_cmult_impl_WR_WI_C<TImpl, 32, 32, 19937>();
        test_cmult_impl_WR_WI_C<TImpl, 32, 32, 423231>();
        test_cmult_impl_WR_WI_C<TImpl, 32, 32, 1234567890ul>();

        // Some random 64-bit points
        test_cmult_impl_WR_WI_C<TImpl, 64, 64, 19937>();
        test_cmult_impl_WR_WI_C<TImpl, 64, 64, 423231>();
        test_cmult_impl_WR_WI_C<TImpl, 64, 64, 1234567890ul>();
        test_cmult_impl_WR_WI_C<TImpl, 64, 64, 1234567801234567801ull>();
    }


    struct cmult_impl_v1_native {
        static constexpr const char *description =
                "Converts the constant to the smallest possible integer, then"
                "does a native multiplication.";

        template<unsigned WR, unsigned WI, uint64_t C>
        THLS_INLINE static fw_uint<WR> go(const fw_uint<WI> &x) {
            const int WC = thls_ctLog2Ceil(C);
            return resize<WR>(x * fw_uint<WC>::from_uint64(C));
        }
    };

    struct cmult_impl_v2_unrolled_full {
        static constexpr const char *description =
                "Unrolls the multiplications into repeated addition.";

        template<unsigned WR, unsigned WI, uint64_t C>
        THLS_INLINE static fw_uint<WR> go(const fw_uint<WI> &x) {
            const int WC = thls_ctLog2Ceil(C);
            const int N = thls_ctMin(WR, WC);
            fw_uint<WR> res(0);
            auto xp=resize<WR>(x); // Extend/contract as needed
            for (unsigned i = 0; i < N; i++) {
#pragma HLS UNROLL
                res = res + select(fw_uint<1>((C >> i) & 1), xp, zg<WR>());
                xp = zpad_lo<1>(drop_msb(xp));
            }
            return res;
        }
    };


    struct cmult_impl_v3_unrolled_masked {
        static constexpr const char *description =
                "Unrolls the multiplications into repeated addition, and attempts to"
                "mask out the ones that don't matter.";

        template<unsigned WR, unsigned WI, uint64_t C>
        THLS_INLINE static fw_uint<WR> go(const fw_uint<WI> &x) {
            const int WC = thls_ctLog2Ceil(C);
            const int N = thls_ctMin(WR, WC);
            fw_uint<WR> res(0);
            auto xp=resize<WR>(x); // Extend/contract as needed
            for (unsigned i = 0; i < N; i++) {
#pragma HLS UNROLL
                if ((C >> i) & 1) {
                    res = res + (xp << i);
                }
            }
            return res;
        }
    };

struct cmult_impl_v4_recursive_masked
{
    static constexpr const char *description=
"Perform repeated addition of non-zero positions, using "
"recursion to tune the widths.";

    /* We have:
     * - WR : Width of result
     * - WI : Width of input
     * - WC : Width of constant
     *
     * Only the WR of anything matter, so we can clamp things as:
     * - WI > WR : Only the least significant WR actually matter, so can immediately
     *             discard the top WI-WR bits.
     * - WC > WR : Again, only the least significant WR matter, so can truncate to WR.
     */

    template<unsigned WR, unsigned WI, uint64_t C>
    struct helper
    {
        THLS_INLINE static fw_uint<WR> go(const fw_uint<WI> &x) {
            if (C == 1) {
                return resize<WR>(x);
            } else {
                fw_uint<WR - 1> base = helper<WR - 1, WI, (C >> 1)>::go(x);
                if (C & 1) {
                    return zpad_lo<1>(base) + resize<WR>(x);
                } else {
                    return zpad_lo<1>(base);
                }
            }
        }
    };

    // Base case 1
    template<unsigned WI,uint64_t C>
    struct helper<0,WI,C>
    {
        THLS_INLINE static fw_uint<0> go(const fw_uint<WI> &x)
        { return zg<0>(); }
    };

    // Base case 2
    template<unsigned WR, unsigned WI>
    struct helper<WR,WI,0>
    {
        THLS_INLINE static fw_uint<WR> go(const fw_uint<WI> &x)
        { return zg<WR>(); }
    };

    // Combined base 1+2
    template<unsigned WI>
    struct helper<0,WI,0>
    {
        THLS_INLINE static fw_uint<0> go(const fw_uint<WI> &x)
        { return zg<0>(); }
    };

    template<unsigned WR, unsigned WI, uint64_t C>
    THLS_INLINE static fw_uint<WR> go(const fw_uint<WI> &x)
    {
        static_assert(C>0, "Constant is zero.");

        const unsigned WIa=thls_ctMin(WR,WI);
        const unsigned WC=thls_ctLog2Ceil(C);
        const unsigned WCa=thls_ctMin(WR,WC);
        const uint64_t WC_MASK=0xFFFFFFFFFFFFFFFFull>>(64-WCa);
        const uint64_t Ca = WC_MASK & C;

        if(Ca==0){
            return zg<WR>();
        }else {
            return helper<WR, WIa, Ca>::go(take_lsbs<WIa>(x));
            //return cmult_impl_v1_native::template go<WR,WIa,Ca>(take_lsbs<WIa>(x));
        }
    }
};

}; // thls

#endif
