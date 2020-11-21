#ifndef GRAMMARFRONTEND_H
#define GRAMMARFRONTEND_H

#include "../lexer/tLexer.h"
#include "../grammar/grammar.h"
#include "../grammar/syntaxTree.h"
#include "../LR0/LR0Grammar.h"

#include "../utils/invokeful.hpp"

struct ignoreAndCopyHelper
{
    lexer::tokenStream operator() (lexer::tokenStream tokens)
    {
        for (lexer::tokenUnit& token : tokens) {
            storage.push_back(token);
            if (!ignored.contains(token.id)) {
                co_yield storage.back();
            }
        }
    }

    void clear()
    {
        storage.clear();
        ignored.clear();
    }

    lexer::tokenStreamGrowingStorage storage;
    std::set<lexer::tokenID> ignored;
};

class compilerFrontend
{
public:
    enum class declareState {
        dsRedefined,
        dsUndefined,
        dsSuccess,
    };

public:
    compilerFrontend();
    ~compilerFrontend();

    void generateParser();
    lexer::tokenStreamStorage tokenize(stringType);
    pSyntaxTree parse(stringType);

    const LR0Grammar& parser() const { return *pGrammarParser; }
    const grammar& grammar() const { return _grammar; }
    const tLexer& lexer() const { return _lexer; }

    LR0Grammar& parser() { return *pGrammarParser; }
    ::grammar& grammar() { return _grammar; }
    tLexer& lexer() { return _lexer; }
    nameTable& symbolTable() { return _symbolTable; }
    nameTable& tokenTable() { return _tokenTable; }

    void clear() {
        grammar().clear();
        lexer().clear();
        tokenTable().clear();
        symbolTable().clear();
    }

    const nameTable& symbolTable() const { return _symbolTable; }
    const nameTable& tokenTable() const { return _tokenTable; }

    void setTokenToBeIgnored(lexer::tokenID id);
    std::pair<lexer::tokenID, declareState> declareNewToken(const stringType& name, const stringType& str, bool escaped = false);
    std::pair<nodeType, declareState> declareNewSymbol(const stringType& name);

    void markStart(nodeType n) { grammar().markStart(n); }

    ostreamType& toLexerStream(ostreamType& os) const { return lexer().toStream(os, tokenTable()); }
    ostreamType& toGrammarStream(ostreamType& os) const { return grammar().toStream(os, symbolTable(), tokenTable()); }

    lexer::tokenStreamStorage getLastTokenized() const { 
        return lexer::tokenStreamStorage(predefinedIgnoreAndCopyStep.storage.begin(), predefinedIgnoreAndCopyStep.storage.end());
    }

protected:
    auto buildTokonizationPipe()
    {
        return
            function_pipe(
                [this](const auto& passage) { return lexer().tokenizeCoro(passage); },
                std::ref(predefinedIgnoreAndCopyStep)
            );
    }

    auto buildParsingPipe()
    {
        return
            [this](auto&& tokens) { return parser().parseCoro(std::move(tokens)); };
    }

private:
    LR0Grammar* pGrammarParser = nullptr;

    nameTable _tokenTable;
    nameTable _symbolTable;

    ::grammar _grammar;
    tLexer _lexer;

    ignoreAndCopyHelper predefinedIgnoreAndCopyStep;

public:
    using static_lexer_steps_type = std::function<lexer::tokenStream(stringType)>;
    dynamic_function_pipe<lexer::tokenStream>::type<static_lexer_steps_type> lexerCustomSteps;

    using static_parser_steps_type = std::function<pSyntaxTreeStream(lexer::tokenStream)>;
    dynamic_function_pipe<pSyntaxTreeStream>::type<static_parser_steps_type> parserCustomSteps;
};

#endif // GRAMMARFRONTEND_H
