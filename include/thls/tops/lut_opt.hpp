#ifndef thls_tops_lut_opt_hpp
#define thls_tops_lut_opt_hpp

#include "thls/tops/ct/ct_vec_unique.hpp"
#include "thls/tops/ct/ct_vec_permute.hpp"
#include "thls/tops/ct/ct_vec_algorithms.hpp"

#include "thls/tops/ct/ct_rng_lcg_mod_2y32.hpp"

#include "thls/tops/fw_uint.hpp"

/*	
	int n, r, t, maxk;			// rng parameters
	uint32_t s;		// Seed for generator
	vector<set<int> > taps;	// connections
	vector<int> cycle;	// cycle through bits
	vector<int> perm;	// output permutation	
	int seedTap;			// Entry point to cycle

	rng(int _n, int _r, int _t, int _maxk, uint32_t _s)
		: n(_n), r(_r), t(_t), maxk(_maxk), s(_s)
		, taps(n), cycle(n), perm(r), seedTap(0)
	{  // Construct an rng using (n,r,t,maxk,s) tuple			
		vector<int> outputs(r), len(r,0);    int bit;
		
		// 1: Create cycle through bits for seed loading
		for(int i=0;i<r;i++){ cycle[i]=perm[i]=(i+1)%r; }
		outputs=perm; // current output of each fifo
		
        # No FIFOs
		# for(int i=r;i<n;i++){ // 2: Extend bit-wide FIFOs
		# 	do{ bit=LCG(_s)%r; }while(len[bit]>=maxk) ;
		#	cycle[i]=i;       swap(cycle[i], cycle[bit]);
		#	outputs[bit]=i;    len[bit]++;
		#}
		
		for(int i=0;i<n;i++) // 3: Loading connections
			taps[i].insert(cycle[i]);
		
		for(int j=1;j<t;j++){ // 4: XOR connections
			Permute(_s, outputs);
			for(int i=0;i<r;i++){
				taps[i].insert(outputs[i]);
				if(taps[i].size()<taps[seedTap].size())
					seedTap=i;
		}}
		
		Permute(_s, perm); // 5: Output permutation
    }
*/

namespace thls
{

template<int Trem, class Rng, class Vec>
struct lut_opt_spec_make_perms
{

private:
    typedef ct::ct_vec_permute<Rng,Vec> permed;
    
    typedef lut_opt_spec_make_perms<Trem-1,typename permed::rng,typename permed::type> next;
public:
    typedef typename permed::type sources;
    typedef typename next::rng rng;

    template<int D,int I>
    struct get_source
    {
    private:
        static_assert(D>0, "Can't have negative D.");
        typedef typename next::template get_source<D-1,I> nn;
    public:
        static const int value = nn::value;
    };
    
    template<int I>
    struct get_source<0,I>
    {
        static const int value = ct::ct_vec_read<I,sources>::type::value;
    };
};

template<class Rng, class Vec>
struct lut_opt_spec_make_perms<0,Rng,Vec> {
    typedef Rng rng;
};

        // Acc should be a vector of ints with the same length as Perms has depth.
template<int I, int O, class Perms, class Acc>
struct lut_opt_spec_get_src_vec
{
private:
    typedef typename std::integral_constant<int,Perms::template get_source<O-1,I>::value>::type nv;
    typedef typename ct::ct_vec_write<O-1,nv,Acc>::type acc_n;
public:
    typedef typename lut_opt_spec_get_src_vec<I,O-1,Perms,acc_n>::type type;
};

template<int I,class Perms, class Acc>
struct lut_opt_spec_get_src_vec<I,0,Perms,Acc>
{
    typedef Acc type;
};


template<int N,int T,uint32_t S>
struct lut_opt_spec {
    static const int n = N;
    static const int t = T;
    static const int maxk = 0;
    static const int r = n;

private:
    typedef typename ct::make_quick_and_dirty<S>::type rng_t;

    typedef typename ct::create_ct_vec_rotation<1, n>::type cycle_t;

    typedef lut_opt_spec_make_perms<T, rng_t, cycle_t> perms_t;

    typedef typename ct::ct_vec_permute<typename perms_t::rng,cycle_t>::type output_t;

    typedef typename ct::create_ct_vec<T, typename std::integral_constant<int, 0>::type>::type src_init_t;

    struct make_bit {
    private:
        const fw_uint<N> &m_state;
    public:
        make_bit(const fw_uint<N> &s)
            : m_state(s)
        {}

        template<class TL, class TR>
        struct reduce_type {
            typedef fw_uint<1> type;
        };

        template<int off, class Val>
        struct map_type {
            typedef fw_uint<1> type;
        };

        THLS_INLINE  fw_uint<1> reduce(const fw_uint<1> &a, const fw_uint<1> &b) const {
            return a ^ b;
        }

        template<int off, class Val>
        THLS_INLINE fw_uint<1> map() const {
            //std::cerr<<"    map<off="<<off<<", val="<<Val::value<<">\n";
            return get_bit<Val::value>(m_state);
        }
    };

    struct cat_bit {
    private:
        const fw_uint<N> &m_state;

    public:
        cat_bit(const fw_uint<N> &s)
            : m_state(s)
        {}

        template<class TL, class TR>
        struct reduce_type {
            typedef fw_uint<TL::width+TR::width> type;
        };

        template<int off, class Val>
        struct map_type {
            typedef fw_uint<1> type;
        };

        template<int off, class Val>
        THLS_INLINE  fw_uint<1> map() const {
            typedef typename lut_opt_spec_get_src_vec<Val::value, T, perms_t, src_init_t>::type raw_srcs;

            typedef typename ct::ct_vec_unique<raw_srcs>::type srcs;

            //std::cerr<<"Map<off="<<off<<",val="<<Val::value<<">\n";
            make_bit mb(m_state);
            return ct::ct_vec_map_reduce<srcs>(mb);
        }

        template<int A,int B>
        THLS_INLINE  fw_uint<A + B> reduce(const fw_uint<A> &a, const fw_uint<B> &b) const
        {
            //std::cerr<<"Reduce<"<<A<<","<<B<<"> = concat("<<a<<","<<b<<")\n";
            return concat(a,b);
        }

    };




public:
    typedef fw_uint<n> state_t;

    static fw_uint<N> step(const fw_uint<N> &s)
    {
        cat_bit state(s);

        return ct::ct_vec_map_reduce<output_t>(state);
    }


};

}; // thls

#endif
