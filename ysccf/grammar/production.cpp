#include <ostream>
#include <string>

#include "production.h"
#include "../utils/nameTable.h"

production::production()
{

}

production::~production()
{

}


ostreamType& production::toStream(ostreamType &os, const nameTable& symbolTable, const nameTable &tokenTable) const
{
    if( empty() ){
        os<<"null";
    }
    for(const auto &component : *this){
        /*std::visit([&os,&symbolTable](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, nodeType>)
                        os << sBeforeSymbol << symbolTable.ref(arg) << sAfterSymbol;
                    else if constexpr (std::is_same_v<T, stringType>)
                        os << arg;
                    else
                        static_assert(false, "non-exhaustive visitor!");
                }, component);*/

        switch (component.second) {
        case itSymbol:
            os << production::sBeforeSymbol
               << symbolTable.ref(component.first)
               << production::sAfterSymbol
               << " ";
        break;
        case itToken:
            os << production::sBeforeToken
               << tokenTable.ref(component.first)
               << production::sAfterToken
               << " ";
        break;
        default:
            os << "error: failed production";
        break;
        }
        os << " ";

    }
    return os;
}


ostreamType &operator<<(ostreamType &os, const production &prod)
{
    nameTable emptytable;
    return prod.toStream(os,emptytable,emptytable);
}
