#include "grammarParserTest.h"

#include "../grammar/grammar.h"
#include "../grammar/production.h"
#include "../lexer/tLexer.h"
#include "../LR0/LR0Grammar.h"
#include "../utils/nameTable.h"

void grammarParserTest()
{
    constexpr static charType compilerGrammarText[] =
LR"yudalaoniubi(
%token kwNull "\s*null\s*"
%token kwToken "\s*%token\s+"
%token beforeVn "\s*<\s*"
%token afterVn "\s*>\s*"
%token stringConst "\"(\\.|[^\"\\])*\""
%token comment "\s*/\*.*\*/\s*"
%token deducer "\s*::=\s*"
%token delimiter "\s*;\s*"
%token or "\s*\|\s*"
%token space "\s*"
%token identifier "[a-zA-Z_]\w*"
<grammarDef> ::= <grammarDef> <statement> | null;
<statement> ::= <tokenDef> | <rule> | comment ;
<tokenDef> ::= kwToken identifier space stringConst ;
<rule> ::= <producer> deducer <produced> <orProduced> delimiter ;
<producer> ::= <VnExpr> ;
<produced> ::= <produced> <V> | null;
<orProduced> ::= or <produced> <orProduced> | null;
<V> ::= <VnExpr> | <Vt> ;
<VnExpr> ::= beforeVn <Vn> afterVn ;
<Vn> ::= identifier ;
<Vt> ::= identifier | identifier space | kwNull | stringConst | stringConst space ;
<ExtendedStart> ::= <grammarDef> ;
)yudalaoniubi";

    constexpr static charType simpleGrammar[] =
LR"yudalaoniubi(
<E> ::= <T> <A>;
<A> ::= "+" <T> <A>;
<A> ::= null;
<T> ::= <F> <B>;
<B> ::= "*" <F> <B>;
<B> ::= null;
<F> ::= "(" <E> ")";
<F> ::= "i";
)yudalaoniubi";
//加法与乘法的无二义文法描述

    grammar grammar;
    tLexer lex;
    auto E = grammar.newSymbol(),
        T = grammar.newSymbol(),
        A = grammar.newSymbol(),
        F = grammar.newSymbol(),
        B = grammar.newSymbol();
    grammar.markStart(E); //需要显式标记起点（默认为-1（即无效节点））
    auto plus = lex.newToken(L"+", true),
        mul = lex.newToken(L"*", true),
        lbracket = lex.newToken(L"(", true),
        rbracket = lex.newToken(L")", true),
        i = lex.newToken(L"i", false);

    nameTable symbolTable, tokenTable;
    symbolTable.insert(L"E", E);
    symbolTable.insert(L"T", T);
    symbolTable.insert(L"A", A);
    symbolTable.insert(L"F", F);
    symbolTable.insert(L"B", B);
    auto ex = grammar.symbolSize();
    symbolTable.insert(L"E'", ex);

    tokenTable.insert(L"plus", plus);
    tokenTable.insert(L"mul", mul);
    tokenTable.insert(L"lbracket", lbracket);
    tokenTable.insert(L"rbracket", rbracket);
    tokenTable.insert(L"i", i);

    production prod;
    prod.symbol(T)
        .symbol(A);
    grammar.addProduction(E, prod);
    prod.clear();

    prod.token(plus)
        .symbol(T)
        .symbol(A);
    grammar.addProduction(A, prod);
    prod.clear();

    prod;
    grammar.addProduction(A, prod);
    prod.clear();

    prod.symbol(F)
        .symbol(B);
    grammar.addProduction(T, prod);
    prod.clear();

    prod.token(mul)
        .symbol(F)
        .symbol(B);
    grammar.addProduction(B, prod);
    prod.clear();

    prod;
    grammar.addProduction(B, prod);
    prod.clear();

    prod.token(lbracket)
        .symbol(E)
        .token(rbracket);
    grammar.addProduction(F, prod);
    prod.clear();

    prod.token(i);
    grammar.addProduction(F, prod);
    prod.clear();

    grammar.toStream(std::wcout, symbolTable, tokenTable);
    lex.toStream(std::wcout, tokenTable);

    LR0Grammar::statesVecType states; //存储项目集表
    auto lr0Grammar(LR0Grammar::analyze(grammar, &states));
    lr0Grammar.printStateVec(
        std::wcout,
        states,
        symbolTable,
        tokenTable
    ); //输出项目集表

    auto tokens = lex.tokenize(L"i+i*(i+i)");

    LR0Grammar::parseStepVecType steps; //存储中间步骤表
    auto synTree = lr0Grammar.parse(tokens,&steps);
    lr0Grammar.printParseStepVec(std::wcout,
        steps,
        tokens,
        symbolTable,
        tokenTable
    )<<std::endl; //利用中间步骤表输出分析过程

    synTree->toStream(std::wcout, symbolTable, tokenTable);

}
