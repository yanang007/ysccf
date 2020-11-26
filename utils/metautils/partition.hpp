#ifndef META_PARTITION_HPP
#define META_PARTITION_HPP 1

namespace clind {
namespace metautils {
/// <summary>
/// 序列类型的类型特征
/// sequence_traits::value_type | 序列的元素类型 
/// </summary>
/// <typeparam name="Seq">序列类型(std::integral_sequence<...>)</typeparam>
template<
    typename Seq
>
struct sequence_traits;

template<
    template<
        typename ValueT,
        ValueT... values
    >
    typename ValueSeq,
    typename ValueT,
    ValueT... values
>
struct sequence_traits<
    ValueSeq<ValueT, values...>
>
{
    using value_type = ValueT;
    static constexpr size_t size = sizeof...(values);
};

template<
    typename Seq
>
using sequence_value_type = typename sequence_traits<Seq>::value_type;

template<
    typename Seq
>
constexpr size_t sequence_size = sequence_traits<Seq>::size;


/// <summary>
/// 在序列末尾追加元素形成新的序列模板
/// push_back::result | 结果
/// </summary>
/// <typeparam name="Seq">序列模板</typeparam>
/// <param name="value">待追加元素</param>
template<
    typename Seq,
    typename sequence_traits<Seq>::value_type value
>
struct push_back;

template<
    template<
        typename ValueT,
        ValueT... values
    >
    typename ValueSeq,
    typename ValueT,
    ValueT x,
    ValueT... values
>
struct push_back<
    ValueSeq<ValueT, values...>,
    x
>
{
    using result = ValueSeq<ValueT, values..., x>;
};

template<
    typename Seq,
    typename sequence_traits<Seq>::value_type value
>
using push_back_result_t = typename push_back<Seq, value>::result;

/// <summary>
/// 删除序列首部元素
/// pop_front::result | 结果
/// </summary>
/// <typeparam name="Seq">序列模板</typeparam>
template<
    typename Seq
>
requires (sequence_size<Seq> > 0)
struct pop_front;

template<
    template<
        typename ValueT,
        ValueT... values
    >
    typename ValueSeq,
    typename ValueT,
    ValueT front, 
    ValueT... values
>
struct pop_front<
    ValueSeq<ValueT, front, values...>
>
{
    using result = ValueSeq<ValueT, values...>;
};

template<
    typename Seq
>
using pop_front_result_t = typename pop_front<Seq>::result;

/// <summary>
/// 获取序列首部元素
/// front_of::value | 结果
/// </summary>
/// <typeparam name="Seq">序列模板</typeparam>
template<
    typename Seq
>
struct front_of;

template<
    template<
        typename ValueT,
        ValueT... values
    >
    typename ValueSeq,
    typename ValueT,
    ValueT frontVal,
    ValueT... values
>
//requires (sizeof...(values) > 0)
struct front_of <
    ValueSeq<ValueT, frontVal, values...>
>
{
    static inline constexpr ValueT value = frontVal;
};

template<
    typename Seq
>
constexpr auto front_of_v = front_of<Seq>::value;

/// <summary>
/// 将序列模板按leftSize切分
/// 得到长度为leftSize的左序列和长度为size - leftSize的右序列
/// partition::left | 左序列
/// partition::right | 右序列
/// </summary>
/// <typeparam name="Seq">序列模板</typeparam>
/// <param name="leftSize">左序列长度</param>
template<
    typename Seq,
    size_t leftSize
>
struct partition {};

template<
    template<
        typename ValueT,
        ValueT... values
    >
    typename ValueSeq,
    typename ValueT,
    ValueT... values,
    size_t leftSize
>
requires (leftSize <= sizeof...(values))
struct partition<ValueSeq<ValueT, values...>, leftSize>
{
    using precedent = partition<
        ValueSeq<ValueT, values...>,
        leftSize - 1
    >;

    using left = push_back_result_t<typename precedent::left, front_of_v<typename precedent::right>>;
    using right = pop_front_result_t<typename precedent::right>;
};

template<
    template<
        typename ValueT,
        ValueT... values
    >
    typename Seq,
    typename ValueT,
    ValueT... values
>
struct partition<Seq<ValueT, values...>, 0>
{
    using left = Seq<ValueT>;
    using right = Seq<ValueT, values...>;
};

/// <summary>
/// 将序列模板按二分
/// leftSize = floor(size / 2)
/// partition::left | 左序列
/// partition::right | 右序列
/// </summary>
/// <typeparam name="Seq">序列模板</typeparam>
template<
    typename Seq
>
using binary_partition = partition<Seq, sequence_size<Seq> / 2>;

/// <summary>
/// 从pos位置分割序列，并以pos位置元素为middle单独提取出来
/// </summary>
/// <typeparam name="Seq"></typeparam>
template<
    typename Seq,
    size_t pos,
    size_t seqSize = sequence_size<Seq>
>
struct devide
{
    using precedent = partition<Seq, pos>;
    using pre_left = typename precedent::left;
    using pre_right = typename precedent::right;

    using left = pre_left;
    static constexpr auto middle = front_of_v<pre_right>;
    using right = pop_front_result_t<pre_right>;
};

template<
    typename Seq,
    size_t pos
>
struct devide<Seq, pos, 0>
{
    using precedent = partition<Seq, pos>;
    using pre_left = typename precedent::left;
    using pre_right = typename precedent::right;

    using left = pre_left;
    using right = pre_right;
};

template<
    typename Seq
>
using binary_devide = devide<Seq, sequence_size<Seq> / 2>;


/// <summary>
/// 将两个序列连接成为新序列
/// leftSize = floor(size / 2)
/// partition::result | 结果
/// </summary>
/// <typeparam name="SeqL">左序列模板</typeparam>
/// <typeparam name="SeqR">右序列模板</typeparam>
template<
    typename SeqL,
    typename SeqR
>
struct concat;

template<
    template<
        typename ValueT,
        ValueT... values
    >
    typename ValueSeq,
    typename ValueT,
    ValueT... lvals,
    ValueT... rvals
>
struct concat<
    ValueSeq<ValueT, lvals...>,
    ValueSeq<ValueT, rvals...>
>
{
    using result = ValueSeq<ValueT, lvals..., rvals...>;
};

}
}
/**/


#endif // META_PARTITION_HPP