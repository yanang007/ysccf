#ifndef LEXER_H
#define LEXER_H

#include <vector>

#include "./tokenStream.h"

#include "../FA/FAs.h"

class lexer
{
public:
    using tokenID = nodeType;
    using lexUnitInfo = lexUnitInfo;
    inline const static stringType tokenDefiner = L"%token";

    //conflicts may occur when states amount are fairly huge
    inline const static tokenID fin = nodeNotExist;
    inline const static tokenID null = fin - 1;
    inline const static tokenID allRoads = fin - 2;
    using tokenUnit = tokenUnit;

    using tokenStreamStorage = tokenStreamStorage;
    using tokenStreamGrowingStorage = tokenStreamGrowingStorage;
    using tokenStream = tokenStream;

public:
    lexer();
    ~lexer();

    tokenID newToken(const stringType& expr,bool plain = false);
    tokenStreamStorage tokenize(stringType text) const;

    size_t size() const { return _tokens.size(); }
    bool empty() const { return size() == 0; }

    void clear() { _tokens.clear();_tokensRaw.clear(); }

    ostreamType & toStream(ostreamType &os, const nameTable &tokenTable) const
    {
        nodeType n = 0;
        for( const auto& tokenRaw : _tokensRaw ){
            os << tokenDefiner <<" "
               << tokenTable.ref(n) <<" \""
               << tokenRaw << "\"";
            os <<std::endl;
            ++n;
        }
        return os;
    }

protected:
    std::vector<DFA> _tokens;
    std::vector<stringType> _tokensRaw;
};

#endif
