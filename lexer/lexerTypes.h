#ifndef LEXERTYPES_H
#define LEXERTYPES_H

#include <vector>
#include <ostream>
#include <string>
#include <list>

#include "../base/base.h"
#include "../utils/nameTable.h"

#include "../utils/type_wrapper.hpp"

namespace lexerTypes {
	
using tokenID = type_wrapper<nodeType, 225>;

struct lexUnitInfo
{
    stringType raw;
    tokenID id;
    size_t pos;
    ostreamType & toStream(ostreamType &os, const nameTable &tokenTable) const
    {
        os <<L"{ "
           << tokenTable.ref(id)
           << L", "
           << pos
           << L", @\""
           << raw
           << L"\" }";
        return os;
    }
};

inline const static stringType tokenDefiner = L"%token";
inline const static tokenID fin = nodeNotExist;
struct tokenUnit{
    tokenID id;
    lexUnitInfo info;
};

class tokenStreamStorage : public std::vector<tokenUnit>
{
public:
    using base = std::vector<tokenUnit>;
    using base::base;

    ostreamType & toStream(ostreamType &os, const nameTable &tokenTable) const
    {
        for( const auto& [id,info]  : *this)
        {
            info.toStream(os,tokenTable);
        }
        return os;
    }
};

using tokenStream = stdexp::generator<std::reference_wrapper<tokenUnit>>;

using tokenStreamGrowingStorage = std::list<tokenUnit>;

}

#endif // LEXERTYPES_H
