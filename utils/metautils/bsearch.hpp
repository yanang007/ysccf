#ifndef META_BSEARCH_HPP
#define META_BSEARCH_HPP 1

#include <type_traits>

#include "./partition.hpp"

namespace clind {
namespace metautils {
	
template<
    typename SeqT
>
struct bsearch
{
    using precedent = binary_devide<SeqT>;
    using value_type = sequence_value_type<SeqT>;
    using left_child = bsearch<typename precedent::left>;
    using right_child = bsearch<typename precedent::right>;

    static constexpr bool contains(const value_type& v) {
        if constexpr (sequence_size<SeqT> == 0) {
            return false;
        }
        else if constexpr (sequence_size<SeqT> == 1) {
            return precedent::middle == v;
        }
        else if constexpr (sequence_size<SeqT> > 1) {
            if (precedent::middle == v) {
                return true;
            }
            else if (precedent::middle > v) {
                return left_child::contains(v);
            }
            else {
                return right_child::contains(v);
            }
        }
    }
};
}
}



#endif // META_BSEARCH_HPP