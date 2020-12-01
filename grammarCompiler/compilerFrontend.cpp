#include "compilerFrontend.h"

#include "../base/errorReport.h"


compilerFrontend::compilerFrontend()
    : afterTokenizing(buildTokonizationPipe()),
    afterParsing(buildParsingPipe())
{

}

compilerFrontend::~compilerFrontend()
{
    if( pGrammarParser != nullptr )
        delete pGrammarParser;
}

void compilerFrontend::generateParser()
{
    symbolTable().insert(L"ExtendedStart", symbolTable().size());
    pGrammarParser = new LR0Grammar(LR0Grammar::analyze(this->grammar()));
}

lexer::tokenStreamStorage compilerFrontend::tokenize(stringType)
{
    return lexer::tokenStreamStorage();
}

pSyntaxTree compilerFrontend::parse(stringType tokenCoro)
{
    _storageStep.clear();
    return function_pipe(
        std::ref(afterTokenizing), 
        std::ref(afterParsing),
        [](pSyntaxTreeStream synStream) { 
            // 取出最终规约的结果文法树（如果有的话）
            pSyntaxTree ret;
            for (auto p : synStream) {
                ret = p;
            } 
            return ret;
        }
    ) (std::move(tokenCoro));
}

void compilerFrontend::ignore(lexer::tokenID id)
{
    _ignoreStep.ignored.insert(id);
}

void compilerFrontend::clearIgnores()
{
    _ignoreStep.clear();
}

std::pair<lexer::tokenID, compilerFrontend::declareState> compilerFrontend::declareNewToken(const stringType& name, const stringType& str, bool escaped)
{
    if (!name.empty() && !str.empty()) {
        lexer::tokenID id;
        if (auto iter = tokenTable().find(name); !tokenTable().isEnd(iter)) {
            id = iter->second; //tokenTable().at(name);
        }
        else {
            id = lexer().newToken(str, escaped);
            tokenTable().insert(name, id);
            return std::make_pair(id, declareState::dsSuccess);
        }
        return std::make_pair(id, declareState::dsRedefined);
    }
    return std::make_pair(nodeNotExist, declareState::dsUndefined);
}

std::pair<symbolID, compilerFrontend::declareState> compilerFrontend::declareNewSymbol(const stringType& name)
{
    if (!name.empty()) {
        nodeType id;

        if (auto iter = symbolTable().find(name); !symbolTable().isEnd(iter)) {
            id = iter->second; // symbolTable().at(name);
        }
        else {
            id = grammar().newSymbol();
            symbolTable().insert(name, id);
            return std::make_pair(id, declareState::dsSuccess);
        }

        return std::make_pair(id, declareState::dsRedefined);
    }
    return std::make_pair(nodeNotExist, declareState::dsUndefined);
}