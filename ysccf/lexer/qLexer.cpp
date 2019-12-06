#include "../base/base.h"
#ifdef _USE_QT

#include "qLexer.h"
#include "../base/errorReport.h"

qLexer::qLexer()
{

}

qLexer::tokenID qLexer::newToken(const stringType& expr,bool plain)
{
    stringType newExpr;
    if( !plain ){
        auto iter = expr.begin();
        while(iter != expr.end() &&
              *iter == L'^'){
            ++iter;
        }
        newExpr = stringType(iter,expr.end());
    }
    else{
        newExpr = expr;
    }
    //eliminate redundent ^
    QString _expr = QString::fromStdWString(newExpr);
    if( plain ){
        _expr = (QRegExp::escape(_expr));
    }

    QRegExp regex('^'+_expr);
    auto ret = size();
    _tokens.push_back(regex);
    return ret;
}

qLexer::tokenStream qLexer::tokenize(stringType text) const
{
    //CaretAtOffset
    tokenStream tokenStream;
    if ( empty() ){
        return tokenStream;
    }
    auto qText = QString::fromStdWString(text);
    auto pos = 0;
    auto ender = qText.size();

    size_t matchedSize = 0;
    size_t bestMatchTokenId = -1;
    size_t bestMatchSize;
    size_t tokenId;

    lexUnitInfo infoPack;
    infoPack = {L"",1,1};

    while ( pos < ender ) {
        tokenId = 0;
        matchedSize = 0;
        bestMatchTokenId = -1;
        bestMatchSize = 0;
        for( const auto& token : _tokens ){
            auto matchedPos = token.indexIn(qText,pos,QRegExp::CaretAtOffset);
            if( matchedPos != pos ){
                //do nothing
            }
            else{
                matchedSize = token.matchedLength();
                if( matchedSize > bestMatchSize ){
                    bestMatchTokenId = tokenId;
                    bestMatchSize = matchedSize;
                }
            }
            ++tokenId;
        }
        if( bestMatchTokenId == -1 ){
            errorReport(L"lexer::tokenize",
                        stringType(L"unrecognized token at pos ")
                        + std::to_wstring(pos)
                        + L"\n"
                        + qText.mid(pos-10).toStdWString(),
                        10);
            goto nruter;
        }
        infoPack = { qText.mid(pos,bestMatchSize).toStdWString(),
                     bestMatchTokenId,
                     (size_t)pos
                   };

        tokenStream.push_back({infoPack.id,infoPack});
        pos += bestMatchSize;
    }
    infoPack = { stringType(),
                 lexer::fin,
                 (size_t)pos
               };

    tokenStream.push_back({infoPack.id,infoPack});
nruter:
    return tokenStream;
}

#endif //_USE_QT
