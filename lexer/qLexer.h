#ifndef QLEXER_H
#define QLEXER_H

#include <QRegExp>

#include "../base/base.h"
#include "lexer.h"
#include "../utils/nameTable.h"

//lexer using QRegex
//to avoid lexer being to slow
//faced with large charset
class qLexer
{
public:
    using tokenID = lexer::tokenID;
    using lexUnitInfo = lexer::lexUnitInfo;
    using tokenUnit = lexer::tokenUnit;
    using tokenStreamStorage = lexer::tokenStreamStorage;
    using tokenStream = lexer::tokenStream;
    inline const static stringType tokenDefiner = lexer::tokenDefiner;
    inline const static tokenID fin = lexer::fin;

public:
    qLexer();

    tokenID newToken(const stringType &expr, bool plain = false);
    tokenStreamStorage tokenize(stringType text) const;
    tokenStream tokenizeCoro(stringType text) const;

    size_t size() const { return _tokens.size(); }
    bool empty() const { return size() == 0; }

    void clear() { _tokens.clear(); }

    ostreamType & toStream(ostreamType &os, const nameTable &tokenTable) const
    {
        nodeType n = 0;
        for( const auto& token : _tokens ){
            os << tokenDefiner <<" "
               << tokenTable.ref(n) <<" \""
               << token.pattern().replace("\"","\\\"").toStdWString().c_str()+1 << "\"";
            os <<std::endl;
            ++n;
        }
        return os;
    }

protected:
    std::vector<QRegExp> _tokens;
};

#endif // QLEXER_H
