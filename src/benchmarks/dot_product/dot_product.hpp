

template<class TTraits>
class dot_impl
{
    typedef typename TTraits::result_type result_type;
    typedef typename TTraits::left_type left_type;
    typedef typename TTraits::right_type right_type;

    template<unsigned N>
    result_type dot_product_seq(const left_type left[N], const right_type right[N])
    {
        result_type result;
        for(unsigned i=0; i<N; i++){
            result=TTraits::add_mult(result, left[i], right[i]);
        }
        return result;
    }

    
    result_type dot_product_par4(const left_type left[4], const right_type right[4])
    {
        return TTraits::dot2(left[0],right[0],  left[1], right[1])
            + TTraits::dot2(left[2],right[2],  left[3], right[3]);
    }

};