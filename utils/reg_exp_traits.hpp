#ifndef REG_EXP_TRAITS
#define REG_EXP_TRAITS

#include <algorithm>
#include <type_traits>

template<
    typename Conv,
    typename InT,
    typename OutT
>
concept ValueConverter = requires() {
    { Conv::convert(InT()) } -> std::convertible_to<OutT>;
};

template <typename SrcT, typename DstT>
struct direct_converter
{
    constexpr static DstT convert(SrcT s)
    {
        return DstT(s);
    }
};

template<typename SrcType, typename DetType, typename Conv, SrcType... Values>
using converted = std::integer_sequence<
    DetType,
    Conv::convert(Values)...
>;

template<typename T>
using default_byte_conv = direct_converter<char, T>;

#include "./metautils/qsort.hpp"
#include "./metautils/bsearch.hpp"

template<
    typename CharT,
    typename ByteConv = default_byte_conv<CharT>
>
requires ValueConverter<ByteConv, char, CharT>
struct escape_helper
{
    // 假设编译器使用大端序，如果系统不是大端序则会报错
    static_assert(u'*' == '*');

    using regex_metas = converted<
        char,
        CharT, 
        ByteConv,
        '^', '$',
        '{', '}', '[', ']', '(', ')',
        '|', '*', '+', '?',
        '.', '\\'
    >;

    using regex_metas_set = clind::metautils::bset<regex_metas>;
};

// 转义掉涉及的控制字符，作为单纯的原始字符串
template<
    typename ByteConv, 
    typename FwdIter, 
    typename EndIter, 
    typename OutIter, 
    typename CharT = typename std::iterator_traits<FwdIter>::value_type>
void regex_escape(FwdIter&& iter, EndIter&& endIter, OutIter&& outIter, char p = 0)
{
    using helper = escape_helper<CharT, ByteConv>;
    std::for_each(
        std::forward<FwdIter>(iter),
        std::forward<EndIter>(endIter),
        [&outIter](const auto c) {
            if (helper::regex_metas_set::contains(c)) {
                *outIter++ = '\\';
            }
            *outIter++ = c;
        }
    );
}

// 转义掉涉及的控制字符，作为单纯的原始字符串
template<
    typename FwdIter, 
    typename EndIter, 
    typename OutIter, 
    typename CharT = typename std::iterator_traits<FwdIter>::value_type>
void regex_escape(FwdIter&& iter, EndIter&& endIter, OutIter&& outIter)
{
    escape<default_byte_conv<CharT>, FwdIter, EndIter, OutIter, CharT>(
        std::forward<FwdIter>(iter),
        std::forward<EndIter>(endIter),
        std::forward<OutIter>(outIter)
    );
}

// 是用于regex本身的特性，类比AllocatorTraits
// regex_traits是regex需要用到的字符特性
// 可能需要考虑分配器？
template<
    typename RegExT
>
struct reg_exp_traits
{};

#include <regex>
template<
    class CharT,
    class Traits
>
struct reg_exp_traits<std::basic_regex<CharT, Traits>>
{
    using value_type = CharT;
    using string_type = typename Traits::string_type;

};





#endif // REG_EXP_TRAITS
