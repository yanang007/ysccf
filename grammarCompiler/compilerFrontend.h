#ifndef GRAMMARFRONTEND_H
#define GRAMMARFRONTEND_H

#include "../lexer/tLexer.h"
#include "../grammar/grammar.h"
#include "../grammar/syntaxTree.h"
#include "../LR0/LR0Grammar.h"

#include "../utils/invokeful.hpp"
#include "../LR0/LR0Grammar.h"

struct ignoreStep
{
    lexer::tokenStream operator() (lexer::tokenStream tokens)
    {
        for (lexer::tokenUnit& token : tokens) {
            if (!ignored.contains(token.id)) {
                co_yield token;
            }
        }
    }

    void clear()
    {
        ignored.clear();
    }

    std::set<lexer::tokenID> ignored;
};

struct storageStep
{
    lexer::tokenStream operator() (lexer::tokenStream tokens)
    {
        for (lexer::tokenUnit& token : tokens) {
            storage.push_back(token);
            co_yield storage.back();
        }
    }

    void clear()
    {
        storage.clear(); 
    }

    lexer::tokenStreamGrowingStorage storage;
};

/*struct finalReduceStep
{
    pSyntaxTree operator() (pSyntaxTreeStream synStream) {
        pSyntaxTree ret;
        for (auto p : synStream) {
            ret = p;
        }
        return ret;

    void clear()
    {
        storage.clear(); 
    }

    lexer::tokenStreamGrowingStorage storage;
};*/

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

    LR0Grammar* parser() { return pGrammarParser; }
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

    void ignore(lexer::tokenID id);
    void clearIgnores();
    std::pair<lexer::tokenID, declareState> declareNewToken(const stringType& name, const stringType& str, bool escaped = false);
    std::pair<symbolID, declareState> declareNewSymbol(const stringType& name);

    void markStart(symbolID n) { grammar().markStart(n); }
    bool construct(
        LR0Grammar::statesVecType* pStatesVec = nullptr,
        LR0Grammar::transferMapVecType* pTransferMapVec = nullptr
    ) {
        pGrammarParser = new LR0Grammar(LR0Grammar::analyze(this->grammar(), pStatesVec, pTransferMapVec));
        return true;
    }

    ostreamType& toLexerStream(ostreamType& os) const { return lexer().toStream(os, tokenTable()); }
    ostreamType& toGrammarStream(ostreamType& os) const { return grammar().toStream(os, symbolTable(), tokenTable()); }

    lexer::tokenStreamStorage getLastTokenized() const { 
        return lexer::tokenStreamStorage(_storageStep.storage.begin(), _storageStep.storage.end());
    }

protected:
    auto buildTokonizationPipe()
    {
        return
            function_pipe(
                [this](const auto& passage) { return lexer().tokenizeCoro(passage); },
                std::ref(_storageStep),
                std::ref(_ignoreStep)
            );
    }

    auto buildParsingPipe()
    {
        return
            [this](auto&& tokens) { return parser()->parseCoro(std::move(tokens), &steps); };
    }

private:
    LR0Grammar* pGrammarParser = nullptr;

    nameTable _tokenTable;
    nameTable _symbolTable;

    ::grammar _grammar;
    tLexer _lexer;

    storageStep _storageStep;
    ignoreStep _ignoreStep;

public:
    LR0Grammar::parseStepVecType steps;

    using static_lexer_steps_type = std::function<lexer::tokenStream(stringType)>;
    using tokenProcessingPipeBase = priority_function_pipe<lexer::tokenStream>;
    using tokenProcessor = tokenProcessingPipeBase::interfunc_t;
    tokenProcessingPipeBase::type<static_lexer_steps_type> afterTokenizing;

    using static_parser_steps_type = std::function<pSyntaxTreeStream(lexer::tokenStream)>;
    using syntaxProcessingPipeBase = priority_function_pipe<pSyntaxTreeStream>;
    using syntaxProcessor = syntaxProcessingPipeBase::interfunc_t;
    syntaxProcessingPipeBase::type<static_parser_steps_type> afterParsing;
};

#endif // GRAMMARFRONTEND_H
