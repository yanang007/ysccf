#include <iostream>
#include <stack>
#include <variant>
#include <ctime>

#include "./base/base.h"

#ifdef _REGTEST
#include "tests/regGrammarTest.h"
#if defined(_USE_QT) && defined(_USE_QWINDOW)
#include <QApplication>
#include "visualize/regexprtestwindow.h"
#endif
#endif //_REGTEST


#ifdef _SYNTAXTEST
#include "tests/grammarParserTest.h"

#if defined(_USE_QT) && defined(_USE_QWINDOW)
#include <QApplication>
#include "visualize/syntaxtreewindow.h"
#endif
#endif

#include "utils/containerUtil.hpp"

bool testContainerUtil();
#include "grammar/grammarAlgos.h"

#include <any>
#include "utils/invokeful.hpp"

int main([[maybe_unused]] int argv, [[maybe_unused]] char * args[])
{
    const static
    std::locale loc(
            std::locale::classic(),
            "en_US.UTF-8",
            std::locale::ctype );
    std::locale::global(loc);

    //1572617670
    //1572703044
    //1572703562
    //1572709101
    //1572847431
    auto seed = time(nullptr);
    std::cout<<"seed: "<< seed << std::endl;
    srand(seed);

#ifdef _SYNTAXTEST
#ifdef _USE_QWINDOW
    QApplication app(argv,args);
    syntaxTreeWindow* pMain = new syntaxTreeWindow();
    pMain->show();
    return app.exec();
#else
    grammarParserTest();
#endif
#endif


#ifdef _REGTEST
#ifdef _USE_QWINDOW
    QApplication app(argv,args);
    regExprTestWindow* pMain = new regExprTestWindow();
    pMain->show();
    return app.exec();
#else
    regGrammarTest();
#endif
#endif // _REGTEST
}

bool testContainerUtil()
{
    std::vector<int> v{3,2,1};

    auto x = enumerate(v);
    for( auto [tag,val] : enumerate(v) ){
        auto x = tag + val;
    }
    for( auto i : range(0,-10,-1) ){
        auto x = i;
    }

    for( auto i : range(0,10,1) ){
        auto x = i;
    }
    return true;
}

/*

cp1\cp1.pro 工程文件，根据目标平台可能变化
cp1\cp1.pro.user 同上
cp1\main.cpp 主函数文件
cp1\base\base.h 核心类型定义
cp1\base\errorReport.cpp 用于全局错误报告的封装
cp1\base\errorReport.h 用于全局错误报告的封装
cp1\FA\compile.cpp 编译正则文法的接口compile的声明
cp1\FA\compile.h 编译正则文法的接口compile的实现
cp1\FA\determinize.cpp 确定化NFA为DFA的接口determinize的声明
cp1\FA\determinize.h 确定化NFA为DFA的接口determinize的实现
cp1\FA\FA.cpp 自动机基类型FA的相关实现
cp1\FA\FA.h 自动机基类型FA的定义
cp1\FA\FAAlgorithm.cpp 自动机相关算法的实现
cp1\FA\FAAlgorithm.h自动机相关算法的声明
cp1\FA\FAs.cpp 自动机派生类型NFA与DFA的相关实现
cp1\FA\FAs.h自动机派生类型NFA与DFA的定义
cp1\FA\minimize.cpp 最小化DFA的接口minimize的声明
cp1\FA\minimize.h 最小化DFA的接口minimize的实现
cp1\grammar\grammar.cpp 文法的表达基类grammar的相关实现
cp1\grammar\grammar.h文法的表达基类grammar的定义
cp1\grammar\grammarAlgos.cpp文法相关算法的实现
cp1\grammar\grammarAlgos.h文法相关算法的定义
cp1\grammar\production.cpp 产生式右端的表达类型production的实现
cp1\grammar\production.h 产生式右端的表达类型production的定义
cp1\grammar\syntaxTree.cpp文法推导树的表达类型syntaxTree的实现
cp1\grammar\syntaxTree.h文法推导树的表达类型syntaxTree的定义
cp1\LALR1\LALR1Grammar.cpp LALR(1)文法的解析接口实现（暂未完成）
cp1\LALR1\LALR1Grammar.h LALR(1)文法的解析接口定义
cp1\lexer\cLexer.cpp 基于<regex>的词法分析器实现
cp1\lexer\cLexer.h基于<regex>的词法分析器定义
cp1\lexer\lexer.cpp基于我们设计的DFA的词法分析器实现
cp1\lexer\lexer.h基于我们设计的DFA的词法分析器定义
cp1\lexer\qLexer.cpp基于<QRegex>的词法分析器实现
cp1\lexer\qLexer.h基于< QRegex >的词法分析器实现
cp1\lexer\tLexer.h词法分析器的选择器头文件
cp1\LL1\LL1Grammar.cpp LL(1)文法的解析接口实现（暂未完成）
cp1\LL1\LL1Grammar.h LL(1)文法的解析接口定义
cp1\LR0\LR0Grammar.cpp LR(0)文法的解析接口实现
cp1\LR0\LR0Grammar.h LR(0)文法的解析接口定义
cp1\LR0\LR0Item.cpp LR(0)的项目元素实现
cp1\LR0\LR0Item.h LR(0)的项目元素定义
cp1\LR0\LRParsingHelper.cpp LR(0)的项目元素定义
cp1\LR0\LRParsingHelper.h LR(0)的项目元素定义
cp1\LR0\LRTable.cpp LR类型文法分析表实现
cp1\LR0\LRTable.h LR类型文法分析表定义
cp1\tests\grammarParserTest.cpp 测试器，LR(0)文法解析器的测试实现
cp1\tests\grammarParserTest.h测试器，LR(0)文法解析器的测试定义
cp1\tests\regGrammarTest.cpp测试器，词法解析器的测试函数实现
cp1\tests\regGrammarTest.h测试器，词法解析器的测试函数定义
cp1\utils\bijection.hpp工具类，双映射类型 bijection的定义与实现
cp1\utils\containerUtil.hpp工具类，容器相关工具的定义与实现
cp1\utils\nameTable.cpp工具类，名字表nameTable的实现
cp1\utils\nameTable.h工具类，名字表nameTable的定义
cp1\utils\utils.hpp一些简单的工具函数
cp1\visualize\FAViz.hpp用于将自动机可视化的定义与实现（需要第三方软件GraphViz）
cp1\visualize\grammarCompiler.cpp文法解析器grammarCompiler的实现
cp1\visualize\grammarCompiler.h文法解析器grammarCompiler的定义
cp1\visualize\regexprtestwindow.cpp正则文法测试窗口的实现
cp1\visualize\regexprtestwindow.h正则文法测试窗口的定义
cp1\visualize\syntaxtreewindow.cpp 文法解析器测试窗口的实现
cp1\visualize\syntaxtreewindow.h 文法解析器测试窗口的定义

 * */
