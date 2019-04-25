#ifndef thls_function_utils_hpp
#define thls_function_utils_hpp

#include <array>

namespace thls
{

template <size_t... I>
class index_sequence {};

template <size_t N, size_t ...I>
struct make_index_sequence
    : make_index_sequence<N-1, N-1,I...>
{};

template <size_t ...I>
struct make_index_sequence<0,I...>
    : index_sequence<I...>
{};



template<class TF, class TA, size_t N, size_t... I>
TA callN_impl(TF f, const std::array<TA,N> &x, index_sequence<I...>)
{
    return f( x[I]... );
}

template<class TF, class TA, size_t N>
TA callN(TF f, const std::array<TA,N> &x)
{
    return callN_impl(f, x, make_index_sequence<N>() );
}

};

#endif