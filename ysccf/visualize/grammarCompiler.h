#ifndef GRAMMARCOMPILER_H
#define GRAMMARCOMPILER_H

#include "../lexer/tLexer.h"
#include "../grammar/grammar.h"
#include "../grammar/syntaxTree.h"
#include "../LR0/LR0Grammar.h"

class grammarCompiler
{
public:
    grammarCompiler();
    ~grammarCompiler();

    void clear(){
        customGrammarContainer.clear();
        customLexer.clear();
        customTokenTable.clear();
        customSymbolTable.clear();
    }

    lexer::tokenStream tokenize(stringType);
    pSyntaxTree parse(const lexer::tokenStream &);
    const grammar& construct(pSyntaxTree);
    const grammar& lastConstruct() const { return customGrammarContainer; }

    const tLexer& compilerLexer() const { return lexer; }
    const tLexer& grammarLexer() const { return customLexer; }

    const nameTable& compilerSymbolTable() const { return symbolTable; }
    const nameTable& compilerTokenTable() const { return tokenTable; }

    const nameTable& grammarSymbolTable() const { return customSymbolTable; }
    const nameTable& grammarTokenTable() const { return customTokenTable; }

    const LR0Grammar& getCompilerGrammar() const { return *pGrammarParser; }
protected:

    void analyzeParser(pcSyntaxTree tree);
    void parseStatement(pcSyntaxTree tree);
    void parseToken(pcSyntaxTree tree);
    void parseRule(pcSyntaxTree tree);
    void parseProducer(pcSyntaxTree tree);
    void parseProduced(nodeType producer,pcSyntaxTree tree);
    void parseOrProduced(nodeType producer,pcSyntaxTree tree);


    enum declareState{
        dsRedefined,
        dsUndefined,
        dsSuccess,
    };


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

    LR0Grammar* pGrammarParser;
    tLexer lexer;
    lexer::tokenID
        _kwNull,_kwToken,
        _beforeVn,_afterVn,
        _stringConst,_comment,
        _deducer,_delimiter,_or,_space,
        _identifier;

    nodeType _grammarDef,
            _statement,
            _tokenDef,
            _rule,_producer,_produced,_orProduced,
            _V,
            _VnExpr,_Vn,
            _Vt;
    nameTable tokenTable;
    nameTable symbolTable;

    grammar customGrammarContainer;
    tLexer customLexer;
    nameTable customTokenTable;
    nameTable customSymbolTable;
};

#endif // GRAMMARCOMPILER_H
