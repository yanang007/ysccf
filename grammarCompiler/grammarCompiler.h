#ifndef GRAMMARCOMPILER_H
#define GRAMMARCOMPILER_H

#include "../lexer/tLexer.h"
#include "../grammar/grammar.h"
#include "../grammar/syntaxTree.h"
#include "../LR0/LR0Grammar.h"
#include "./compilerFrontend.h"
#include "./attribute/attributeManager.h"

class grammarCompiler
{
public:
    grammarCompiler();
    ~grammarCompiler();

    void clear(){
        cfe.clear();
    }

    lexer::tokenStreamStorage tokenize(stringType);
    pSyntaxTree parse(const lexer::tokenStreamStorage &);
    pSyntaxTree parse(stringType);
    const grammar& construct(pSyntaxTree);
    const grammar& lastConstruct() const { return cfe.grammar(); }

    compilerFrontend& grammarFrontend() { return cfe; }

    const tLexer& compilerLexer() const { return cfeOfCC.lexer(); }
    const nameTable& compilerSymbolTable() const { return cfeOfCC.symbolTable(); }
    const nameTable& compilerTokenTable() const { return cfeOfCC.tokenTable(); }

    const tLexer& grammarLexer() const { return cfe.lexer(); }
    const nameTable& grammarSymbolTable() const { return cfe.symbolTable(); }
    const nameTable& grammarTokenTable() const { return cfe.tokenTable(); }

    const LR0Grammar& getCompilerGrammar() const { return *pGrammarParser; }

    lexer::tokenStreamStorage getLastTokenized() const { return cfeOfCC.getLastTokenized(); }

protected:
    void analyzeParser(pcSyntaxTree tree);
    void parseStatement(pcSyntaxTree tree);
    void parseToken(pcSyntaxTree tree);
    void parseRule(pcSyntaxTree tree);
    void parseProducer(pcSyntaxTree tree);
    void parseProduced(nodeType producer,pcSyntaxTree tree);
    void parseOrProduced(nodeType producer,pcSyntaxTree tree);

    using declareState = compilerFrontend::declareState;

    std::pair<nodeType,declareState> declareNewToken(const stringType &name, const stringType &str,bool escaped = false);
    std::pair<nodeType,declareState> declareNewSymbol(const stringType &name);

    enum vtType{
        vttVn,
        vttVtIndtifier,
        vttStringConst,
        vttKwNull,
        vttFailed,
    };

    std::pair<stringType,vtType> parseV(pcSyntaxTree tree);

    stringType parseVnExpr(pcSyntaxTree tree);
    stringType parseVn(pcSyntaxTree tree);

    std::pair<stringType,vtType> parseVt(pcSyntaxTree tree);
    stringType parseIdentifier(pcSyntaxTree tree);
    stringType parseKwNull(pcSyntaxTree tree);
    stringType parseStringConst(pcSyntaxTree tree);

    stringType processStringConst(const stringType& sc);


    void initGrammarCompiler();
    void initAttributes();

    LR0Grammar* pGrammarParser;

    lexer::tokenID
        _kwNull,_kwToken,
        _beforeVn,_afterVn,
        _stringConst,_comment,
        _deducer,_delimiter,_or,_space,
        _identifier,
        _beforeAttr, _afterAttr;

    nodeType _grammarDef,
            _statement,
            _tokenDef,
            _rule,_producer,_produced,_orProduced,
            _V,
            _VnExpr,_Vn,
            _Vt,
            _attribute,
            _attributedStatement;

    attributeManager attrs;
    compilerFrontend cfe;
    compilerFrontend cfeOfCC;
};

#endif // GRAMMARCOMPILER_H
