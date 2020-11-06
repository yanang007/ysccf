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


qLexer::tokenStreamStorage qLexer::tokenize(stringType text) const
{
    //CaretAtOffset
    tokenStreamStorage tokenStream;
    if (empty()) {
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
    infoPack = { L"",1,1 };

    while (pos < ender) {
        tokenId = 0;
        matchedSize = 0;
        bestMatchTokenId = -1;
        bestMatchSize = 0;
        for (const auto& token : _tokens) {
            auto matchedPos = token.indexIn(qText, pos, QRegExp::CaretAtOffset);
            if (matchedPos != pos) {
                //do nothing
            }
            else {
                matchedSize = token.matchedLength();
                if (matchedSize > bestMatchSize) {
                    bestMatchTokenId = tokenId;
                    bestMatchSize = matchedSize;
                }
            }
            ++tokenId;
        }
        if (bestMatchTokenId == -1) {
            errorReport(L"lexer::tokenize",
                stringType(L"unrecognized token at pos ")
                + std::to_wstring(pos)
                + L"\n"
                + qText.mid(pos - 10).toStdWString(),
                ec_Lexer_UnknownToken);
            goto nruter;
        }
        infoPack = { qText.mid(pos,bestMatchSize).toStdWString(),
                     bestMatchTokenId,
                     (size_t)pos
        };

        tokenStream.push_back({ infoPack.id,infoPack });
        pos += bestMatchSize;
    }
    infoPack = { stringType(),
                 lexer::fin,
                 (size_t)pos
    };

    tokenStream.push_back({ infoPack.id,infoPack });
    //tokenStreamStorage.push_back({infoPack.id,infoPack});
nruter:
    return tokenStream;
}


qLexer::tokenStream qLexer::tokenizeCoro(stringType text) const
{
    //CaretAtOffset
    if ( empty() ){
        co_return;
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
    tokenUnit unit;

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
                        ec_Lexer_UnknownToken);
            goto nruter;
        }
        infoPack = { qText.mid(pos,bestMatchSize).toStdWString(),
                     bestMatchTokenId,
                     (size_t)pos
                   };

        unit = { infoPack.id,infoPack };
        co_yield std::ref(unit);
        //tokenStreamStorage.push_back({infoPack.id,infoPack});
        pos += bestMatchSize;
    }
    infoPack = { stringType(),
                 lexer::fin,
                 (size_t)pos
               };

    unit = { infoPack.id,infoPack };
    co_yield std::ref(unit);
    //tokenStreamStorage.push_back({infoPack.id,infoPack});
nruter:
    co_return;
}

#endif //_USE_QT
