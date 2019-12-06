#include "regGrammarTest.h"

#include <iostream>

#include "../FA/FAs.h"
#include "../FA/determinize.h"
#include "../FA/minimize.h"
#include "../FA/compile.h"
#include "../lexer/lexer.h"
#include "../utils/nameTable.h"

void regGrammarTest()
{
    NFA nfa = compile(L"(a|b)*(aa|bb)(a|b)*");
    std::wcout<<nfa;
    auto dfa = determinize(nfa);
    std::wcout<<dfa;
    auto mindfa = minimize(dfa);
    std::wcout<<mindfa;
    std::wcout<<mindfa.match(L"aac") << " ";
    std::wcout<<mindfa.match(L"abababababaa") << " ";
    std::wcout<<mindfa.match(L"aaaaaaabbbbbb") << " ";
    std::wcout<<mindfa.match(L"ab") << " ";
    std::wcout<<mindfa.match(L"1") << " ";
    std::wcout<<mindfa.match(L"1+2+2") << " ";
    std::wcout<<L"ε";

    lexer lex;
    nameTable tokenTable;
    auto operatorAdd = lex.newToken(L"\\+");
    auto operatorMinus = lex.newToken(L"-");
    auto operatorMul = lex.newToken(L"\\*");
    auto operatorDiv = lex.newToken(L"/");
    auto operatorEquals = lex.newToken(L"=");
    auto operatorLP = lex.newToken(LR"(\()");
    auto operatorRP = lex.newToken(LR"(\))");
    auto oprandIdentifier = lex.newToken(LR"((\l|\L|_)\w*)");
    auto oprandConst = lex.newToken(LR"((\d+.?\d*)|(\d*.\d+))");
    auto space = lex.newToken(L"\\s*");

    tokenTable.insert(L"add",operatorAdd);
    tokenTable.insert(L"minus",operatorMinus);
    tokenTable.insert(L"mul",operatorMul);
    tokenTable.insert(L"div",operatorDiv);
    tokenTable.insert(L"equals",operatorEquals);
    tokenTable.insert(L"lp",operatorLP);
    tokenTable.insert(L"rp",operatorRP);
    tokenTable.insert(L"identifier",oprandIdentifier);
    tokenTable.insert(L"const",oprandConst);
    tokenTable.insert(L"space",space);
    tokenTable.insert(L"fin",lexer::fin);


    nameTable tokenTableChs;
    tokenTableChs.insert(L"加",operatorAdd);
    tokenTableChs.insert(L"减",operatorMinus);
    tokenTableChs.insert(L"乘",operatorMul);
    tokenTableChs.insert(L"除",operatorDiv);
    tokenTableChs.insert(L"等于",operatorEquals);
    tokenTableChs.insert(L"左括号",operatorLP);
    tokenTableChs.insert(L"右括号",operatorRP);
    tokenTableChs.insert(L"标识符",oprandIdentifier);
    tokenTableChs.insert(L"数字",oprandConst);
    tokenTableChs.insert(L"空白符",space);
    tokenTableChs.insert(L"文段终结",lexer::fin);

    stringType temp = L" x = (1+22)+3.5(x+16+.3++xx";
    std::wcout<<temp<<std::endl;
    auto ret = lex.tokenize(temp);
    ret.toStream(std::wcout,tokenTable);
    std::wcout<<std::endl;
    ret.toStream(std::wcout,tokenTableChs);
    lex.toStream(std::wcout,tokenTable);
}

/*
%token add "\+"
%token minus "-"
%token mul "\*"
%token div "/"
%token equals "="
%token lp "\("
%token rp "\)"
%token identifier "(\l|\L|_)\w*"
%token const "(\d+.?\d*)|(\d*.\d+)"
%token space "\s*"
<G>::=null;


%token add "\+"
%token minus "-"
%token mul "\*"
%token div "/"
%token equals "="
%token lp "\("
%token rp "\)"
%token identifier "[a-zA-Z_]\w*"
%token const "(\d+\.?\d*)|(\d*\.\d+)"
%token space "\s*"
<G>::=null;

*/
