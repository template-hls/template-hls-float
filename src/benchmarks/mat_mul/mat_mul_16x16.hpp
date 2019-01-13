typedef typename traits_t::result_t result_type;
typedef typename traits_t::left_t left_type;
typedef typename traits_t::right_t right_type;

static const int N=16;

void mat_mul(
	const left_type left[N][N],
	const right_type right[N][N],
	result_type result[N][N]
){
#pragma HLS INLINE RECURSIVE

	for(unsigned r=0; r<N; r++){
		for(unsigned c=0; c<N; c++){
			result_type res;
			for(unsigned i=0; i<N; i++){
				res = traits_t::add_mul(res, left[r][i] , right[i][c]);
			}
			result[r][c]=res;
		}
	}
}
