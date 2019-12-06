﻿#ifndef TOKENSTREAM_H
#define TOKENSTREAM_H

#include <vector>
#include <ostream>
#include <string>

#include "../base/base.h"
#include "../utils/nameTable.h"

using tokenID = nodeType;
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


class tokenStream : public std::vector<tokenUnit>
{
public:
    ostreamType & toStream(ostreamType &os, const nameTable &tokenTable) const
    {
        for( const auto& [id,info]  : *this)
        {
            info.toStream(os,tokenTable);
        }
        return os;
    }
};

#endif // TOKENSTREAM_H