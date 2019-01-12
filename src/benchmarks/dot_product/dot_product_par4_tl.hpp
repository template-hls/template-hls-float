#include "dot_product.hpp"


typedef typename traits_t::result_t result_type;
typedef typename traits_t::left_t left_type;
typedef typename traits_t::right_t right_type;

result_type dot_product_par4(
		const left_type left0, const right_type right0,
		const left_type left1, const right_type right1,
		const left_type left2, const right_type right2,
		const left_type left3, const right_type right3
)
{
#pragma HLS PIPELINE
#pragma HLS INLINE RECURSIVE

    return traits_t::add(
    		traits_t::dot2(left0,right0,  left1, right1),
            traits_t::dot2(left2,right2,  left3, right3)
    );
}

