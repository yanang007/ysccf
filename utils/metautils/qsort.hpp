#ifndef META_QSORT_HPP
#define META_QSORT_HPP 1

/*concept Partition
left
right

template<
    template<
    typename ValueT,
    ValueT l, ValueT r>
    value_less>
    concept MetaComparer
    */

#include <type_traits>

#include "./partition.hpp"


namespace clind {
namespace metautils {

template<
    typename SeqT
>
struct qsort_partition;

template<
    typename SeqT,
    typename LSeqT,
    typename RSeqT
>
struct qsort_partition_impl
{};

template<
    typename SeqT
>
struct meta_qsort
{};

template<
    typename SeqT
>
using meta_qsort_result_t = typename meta_qsort<SeqT>::result;

template<
    template<
        typename ValueT,
        ValueT... values
    >
    typename ValueSeq,
    typename ValueT,
    ValueT... values
>
struct meta_qsort<ValueSeq<ValueT, values...>>
{
    using sequence = ValueSeq<ValueT, values...>;
    using partition = qsort_partition<sequence>;
    using result =
        typename concat<
            meta_qsort_result_t<typename partition::left>,
            meta_qsort_result_t<typename partition::right>
        >::result;
};

template<
    template<
        typename ValueT,
        ValueT... values
    >
    typename ValueSeq,
    typename ValueT,
    ValueT values
>
struct meta_qsort<ValueSeq<ValueT, values>>
{
    using result = ValueSeq<ValueT, values>;
};

template<
    template<
        typename ValueT,
        ValueT... values
    >
    typename ValueSeq,
    typename ValueT,
    ValueT mid,
    ValueT x,
    ValueT... values,
    ValueT... lvals,
    ValueT... rvals
>
struct qsort_partition_impl<
    ValueSeq<ValueT, mid, x, values...>,
    ValueSeq<ValueT, lvals...>,
    ValueSeq<ValueT, rvals...>
> : public std::conditional_t <
    x < mid,
    qsort_partition_impl<
        ValueSeq<ValueT, mid, values...>,
        ValueSeq<ValueT, x, lvals...>,
        ValueSeq<ValueT, rvals...>
    >,
    qsort_partition_impl<
        ValueSeq<ValueT, mid, values...>,
        ValueSeq<ValueT, lvals...>,
        ValueSeq<ValueT, x, rvals...>
    >
>{};

template<
    template<
        typename ValueT,
        ValueT... values
    >
    typename ValueSeq,
    typename ValueT,
    ValueT mid,
    ValueT... lvals,
    ValueT... rvals
>
struct qsort_partition_impl<
    ValueSeq<ValueT, mid>,
    ValueSeq<ValueT, lvals...>,
    ValueSeq<ValueT, rvals...>
>
{
    static inline constexpr ValueT mid = mid;
    //using middle = ValueSeq<ValueT, mid>;
    using left = std::conditional_t <
        sizeof...(lvals) < sizeof...(rvals),
        ValueSeq<ValueT, mid, lvals...>,
        ValueSeq<ValueT, lvals...>
    >;

    using right = std::conditional_t <
        sizeof...(lvals) < sizeof...(rvals),
        ValueSeq<ValueT, rvals...>,
        ValueSeq<ValueT, mid, rvals...>
    >;
};


template<
    template<
        typename ValueT,
        ValueT... values
    >
    typename ValueSeq,
    typename ValueT,
    ValueT mid,
    ValueT... values
>
struct qsort_partition<ValueSeq<ValueT, mid, values...>>
    : public qsort_partition_impl<
        ValueSeq<ValueT, mid, values...>,
        ValueSeq<ValueT>,
        ValueSeq<ValueT>
    > 
{};

}
}

#endif // META_QSORT_HPP