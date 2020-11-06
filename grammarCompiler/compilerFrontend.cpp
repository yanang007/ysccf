#include "compilerFrontend.h"

#include "../base/errorReport.h"


compilerFrontend::compilerFrontend()
    : lexerCustomSteps(buildTokonizationPipe()),
    parserCustomSteps(buildParsingPipe())
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
    pGrammarParser = new LR0Grammar(LR0Grammar::analyze(grammar()));
}

lexer::tokenStreamStorage compilerFrontend::tokenize(stringType)
{
    return lexer::tokenStreamStorage();
}

pSyntaxTree compilerFrontend::parse(stringType tokenCoro)
{
    return function_pipe(
        std::ref(lexerCustomSteps), 
        std::ref(parserCustomSteps),
        [](pSyntaxTreeStream synStream) { 
            pSyntaxTree ret;
            for (auto p : synStream) {
                ret = p;
            } 
            return ret;
        }
    ) (std::move(tokenCoro));
}

void compilerFrontend::setTokenToBeIgnored(lexer::tokenID id)
{
    predefinedIgnoreAndCopyStep.ignored.insert(id);
}

std::pair<lexer::tokenID, compilerFrontend::declareState> compilerFrontend::declareNewToken(const stringType& name, const stringType& str, bool escaped)
{
    if (!name.empty() && !str.empty()) {
        lexer::tokenID id;

        try {
            id = tokenTable().at(name);
        }
        catch (std::out_of_range e) {
            id = lexer().newToken(str, escaped);
            tokenTable().insert(name, id);
            return std::make_pair(id, dsSuccess);
        }
        return std::make_pair(id, dsRedefined);
    }
    return std::make_pair(nodeNotExist, dsUndefined);
}

std::pair<nodeType, compilerFrontend::declareState> compilerFrontend::declareNewSymbol(const stringType& name)
{
    if (!name.empty()) {
        nodeType id;

        try {
            id = symbolTable().at(name);
        }
        catch (std::out_of_range e) {
            id = grammar().newSymbol();
            symbolTable().insert(name, id);
            return std::make_pair(id, dsSuccess);
        }
        return std::make_pair(id, dsRedefined);
    }
    return std::make_pair(nodeNotExist, dsUndefined);
}