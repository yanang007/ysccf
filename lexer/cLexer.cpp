#include "cLexer.h"
#include "../base/errorReport.h"

cLexer::cLexer()
{

}

cLexer::tokenID cLexer::newToken(const stringType& expr,bool plain)
{
    stringType newExpr = expr;
    //eliminate redundent ^
    //QString _expr = QString::fromStdWString(newExpr);
    if( plain ){
        const static std::wregex escaper(LR"([.$^{[(|)*+?\\])");
        newExpr = std::regex_replace(newExpr,escaper,L"\\$&");
        //_expr = (QRegExp::escape(_expr));
    }

    std::wregex regex(newExpr,std::regex_constants::ECMAScript);
    auto ret = size();
    _tokens.push_back(regex);
    _tokenRaws.push_back(replaced(newExpr,stringType(L"\""),stringType(L"\\\"")));
    return ret;
}

cLexer::tokenStreamStorage cLexer::tokenize(stringType text) const
{
    //CaretAtOffset
    tokenStreamStorage tokenStreamStorage;
    if ( empty() ){
        return tokenStreamStorage;
    }
    auto qText = (text);
    auto pos = qText.cbegin();
    auto ender = qText.cend();

    size_t matchedSize = 0;
    size_t bestMatchTokenId = -1;
    size_t bestMatchSize;
    size_t tokenId;

    lexUnitInfo infoPack;
    infoPack = {L"",1,1};
    std::wsmatch matched;

    while ( pos < ender ) {
        tokenId = 0;
        matchedSize = 0;
        bestMatchTokenId = -1;
        bestMatchSize = 0;
        for( const auto& token : _tokens ){
            bool isMatched = std::regex_search(pos,ender,matched,token,std::regex_constants::match_continuous);
            if( !isMatched ){
                //do nothing
            }
            else{
                matchedSize = matched.str().length();
                if( matchedSize > bestMatchSize ){
                    bestMatchTokenId = tokenId;
                    bestMatchSize = matchedSize;
                }
            }
            ++tokenId;
        }
        if( bestMatchTokenId == -1 ){
            errorReport(L"lexer::tokenize",
                        stringType(L"unrecognized token at pos.")
                        + std::to_wstring(pos - qText.begin())
                        + L"\n",
                        ec_Lexer_UnknownToken);
            goto nruter;
        }
        infoPack = { stringType(pos,pos+bestMatchSize),
                     bestMatchTokenId,
                     (size_t)(pos-qText.begin())
                   };

        tokenStreamStorage.push_back({infoPack.id,infoPack});
        pos += bestMatchSize;
    }
    infoPack = { stringType(),
                 lexer::fin,
                 (size_t)(pos-qText.begin())
               };

    tokenStreamStorage.push_back({infoPack.id,infoPack});
nruter:
    return tokenStreamStorage;
}
