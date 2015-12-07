#ifndef row_heap_hpp
#define row_heap_hpp

#include "fw_uint.hpp"

#include <iostream>

namespace thls
{

template<int W, int D>
struct row_heap_holder
{
    static const int width=W;
    static const int depth=D;
    
    fw_uint<W> rows[D];
};

namespace detail
{
    
    
/*
    D -> D'
    0 -> 0
    1 -> 1
    2 -> 2
    3 -> 2
    4 -> 3
    5 -> 4
    6 -> 4
    7 -> 5
    8 -> 6
    9 -> 6
   10 -> 7
   11 -> 8
   12 -> 8
     ...
*/    

template<int W, int D, int SrcIndex, int DstIndex, int Left>
struct row_heap_compressor
{
    static void compress(row_heap_holder<W,(D/3)*2+(D%3)> &dst, const row_heap_holder<W,D> &bh)
    {
        const fw_uint<W> &A=bh.rows[SrcIndex], &B=bh.rows[SrcIndex+1], &C=bh.rows[SrcIndex+2];
        fw_uint<W> sum=A^B^C;
        fw_uint<W> carry=((A&B)|(B&C)|(A&C));
        dst.rows[DstIndex]=sum;
        if(W>1){
            // For W==1 the shift left by 1 is not allowed at run-time
            dst.rows[DstIndex+1]=carry<<1; // Allow overflow
        }else{
            // Note that it is effectively zg<1>() if this branch is taken
            dst.rows[DstIndex+1]=zg<W>(); // Avoid degenerate but possibly useful case
        }
        row_heap_compressor<W,D,SrcIndex+3,DstIndex+2,Left-3>::compress(dst, bh);
    }
};

template<int W, int D, int SrcIndex, int DstIndex>
struct row_heap_compressor<W,D,SrcIndex,DstIndex,2>
{
    static void compress(row_heap_holder<W,(D/3)*2+(D%3)> &dst, const row_heap_holder<W,D> &bh)
    {
        dst.rows[DstIndex]=bh.rows[SrcIndex];
        dst.rows[DstIndex+1]=bh.rows[SrcIndex+1];
    }
};

template<int W, int D, int SrcIndex, int DstIndex>
struct row_heap_compressor<W,D,SrcIndex,DstIndex,1>
{
    static void compress(row_heap_holder<W,(D/3)*2+(D%3)> &dst, const row_heap_holder<W,D> &bh)
    {
        dst.rows[DstIndex]=bh.rows[SrcIndex];
    }
};

template<int W, int D, int SrcIndex, int DstIndex>
struct row_heap_compressor<W,D,SrcIndex,DstIndex,0>
{
    static void compress(row_heap_holder<W,(D/3)*2+(D%3)> &, const row_heap_holder<W,D> &)
    {}
};


template<int W, int D>
struct row_heap_compress_iterate
{   
    static row_heap_holder<W,2> go(const row_heap_holder<W,D> &x)
    {
        row_heap_holder<W,(D/3)*2+(D%3)> curr;
        row_heap_compressor<W,D,0,0,D>::compress(curr, x);
        return row_heap_compress_iterate<curr.width,curr.depth>::go(curr);
    }
};

template<int W>
struct row_heap_compress_iterate<W,2>
{
    static row_heap_holder<W,2> go(const row_heap_holder<W,2> &x)
    { return x; }
};

template<int W>
struct row_heap_compress_iterate<W,1>
{
    static_assert(W!=W, "Can't compress a single row.");
};

template<int W>
struct row_heap_compress_iterate<W,0>
{
    static_assert(W!=W, "Can't compress no rows.");
};


}; // detail


template<int W, int D>
row_heap_holder<W,2> compress(const row_heap_holder<W,D> &bh)
{
    return detail::row_heap_compress_iterate<W,D>::go(bh);
}

template<int W, int D>
fw_uint<W> collapse(const row_heap_holder<W,D> &bh)
{
    static_assert(D==2,"Will only collapse compressed row heaps.");
    return bh.rows[0]+bh.rows[1];
}


#ifndef THLS_SYNTHESIS
template<int W, int D>
void row_heap_to_mpz(mpz_t r, const row_heap_holder<W,D> &heap)
{
    mpz_set_ui(r, 0);
    mpz_t tmp;
    mpz_init(tmp);
    for(int i=0; i<D; i++){
        //std::cerr<<" "<<i<<" = "<<heap.rows[i]<<"\n";
        heap.rows[i].to_mpz_t(tmp);
        mpz_add(r, r, tmp);
    }
    mpz_clear(tmp);

    // Bit heaps produce results without carry.
    // The output width is the same as the input.
    mpz_tdiv_r_2exp(r, r, W);
}
#endif


template<int W>
row_heap_holder<W,1> fw_uint_to_row_heap(const fw_uint<W> &x)
{
    return row_heap_holder<W,1>({x});
}

template<int WA,int DA,int WB,int DB>
row_heap_holder<thls_ctMax(WA,WB),DA+DB> operator+(const row_heap_holder<WA,DA> &a, const row_heap_holder<WB,DB> &b)
{
    static const int W=thls_ctMax(WA,WB);
    
    row_heap_holder<W,DA+DB> res;
    for(int i=0; i<DA; i++){
        res.rows[i]=extu<W>(a.rows[i]);
    }
    for(int i=0; i<DB; i++){
        res.rows[i+DA]=extu<W>(b.rows[i]);
    }
    return res;
}

template<int WA,int DA,int WB>
row_heap_holder<thls_ctMax(WA,WB),DA+1> operator+(const row_heap_holder<WA,DA> &a, const fw_uint<WB> &b)
{
    static const int W=thls_ctMax(WA,WB);
    
    row_heap_holder<W,DA+1> res;
    for(int i=0; i<DA; i++){
        res.rows[i]=extu<W>(a.rows[i]);
    }
    res.rows[DA]=extu<W>(b);
    return res;
}

}; // thls

#endif
