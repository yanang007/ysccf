
#include "../FA/compile.h"
#include "../FA/determinize.h"
#include "../FA/minimize.h"

#include "../base/errorReport.h"
#include "lexer.h"

lexer::lexer()
{

}

lexer::~lexer()
{

}

lexer::tokenID lexer::newToken(const stringType& expr,bool plain)
{

    //eliminate redundent ^
    //QString _expr = QString::fromStdWString(newExpr);
    auto newExpr = expr;
    if( plain ){
        const static stringType escaper(LR"([.$^{[(|)*+?\])");
        for(auto ch : escaper){
            auto sch = stringType(1,ch);
            replaced(newExpr,sch,stringType(L"\\")+sch);
        }
    }

    NFA nfa = compile(newExpr);
    //vizFA(R"(D:\libraries\Graphviz\2.38\bin\dot)","pdf","nfa",nfa);
    //std::wcout<<nfa;
    auto dfa = determinize(nfa);
    //vizFA(R"(D:\libraries\Graphviz\2.38\bin\dot)","pdf","dfa",_dfa);
    auto mindfa = minimize(dfa);
    std::wcout<<mindfa;
    auto ret = size();
    _tokens.push_back(std::move(mindfa));
    _tokensRaw.push_back(replaced(newExpr,stringType(L"\""),stringType(L"\\\"")));

    return ret;
}

lexer::tokenStreamStorage lexer::tokenize(stringType text) const
{
    tokenStreamStorage tokenStreamStorage;
    if ( empty() ){
        return tokenStreamStorage;
    }
    auto iter = text.begin();
    auto ender = text.end();

    size_t matchedSize = 0;
    size_t bestMatchTokenId = -1;
    size_t bestMatchSize;
    size_t tokenId;

    lexUnitInfo infoPack;
    infoPack = {L"",1,1};

    while ( iter < ender ) {
        tokenId = 0;
        matchedSize = 0;
        bestMatchTokenId = -1;
        bestMatchSize = 0;
        for( const auto& token : _tokens ){
            matchedSize = token.match(iter,ender);
            if( matchedSize > bestMatchSize ){
                bestMatchTokenId = tokenId;
                bestMatchSize = matchedSize;
            }
            ++tokenId;
        }
        if( bestMatchTokenId == -1 ){
            errorReport(L"lexer::tokenize",
                        L"unrecognized token at pos " + std::to_wstring(iter - text.begin()),
                        10);
            goto nruter;
        }
        infoPack = { stringType(iter,iter+bestMatchSize),
                     bestMatchTokenId,
                     size_t(iter - text.begin())
                   };

        tokenStreamStorage.push_back({infoPack.id,infoPack});
        iter += bestMatchSize;
    }

    infoPack = { stringType(),
                 lexer::fin,
                 size_t(iter - text.begin())
               };

    tokenStreamStorage.push_back({infoPack.id,infoPack});
nruter:
    return tokenStreamStorage;
}
