#ifndef CLEXER_H
#define CLEXER_H

#include <regex>

#include "lexer.h"
#include "../utils/nameTable.h"

//lexer using QRegex
//to avoid lexer being to slow
//faced with large charset
class cLexer
{
public:
    using tokenID = lexer::tokenID;
    using lexUnitInfo = lexer::lexUnitInfo;
    using tokenUnit = lexer::tokenUnit;
    using tokenStream = lexer::tokenStream;
    inline const static stringType tokenDefiner = lexer::tokenDefiner;
    inline const static tokenID fin = lexer::fin;

public:
    cLexer();

    tokenID newToken(const stringType &expr, bool plain = false);
    tokenStream tokenize(stringType text) const;

    size_t size() const { return _tokens.size(); }
    bool empty() const { return size() == 0; }

    void clear() { _tokens.clear();_tokenRaws.clear(); }

    ostreamType & toStream(ostreamType &os, const nameTable &tokenTable) const
    {
        nodeType n = 0;
        for( const auto& token : _tokens ){
            os << tokenDefiner <<" "
               << tokenTable.ref(n) <<" \""
               << _tokenRaws[n] << "\"";
            os <<std::endl;
            ++n;
        }
        return os;
    }

protected:
    std::vector<std::wregex> _tokens;
    std::vector<stringType> _tokenRaws;
};

#endif // CLEXER_H
