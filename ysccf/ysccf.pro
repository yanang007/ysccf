TEMPLATE = app
CONFIG += console c++17
QMAKE_CXXFLAGS += /std:c++17

QT       += core gui sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        FA/FA.cpp \
        FA/FAAlgorithm.cpp \
        FA/FAs.cpp \
        FA/compile.cpp \
        FA/determinize.cpp \
        FA/minimize.cpp \
        LALR1/LALR1Grammar.cpp \
        LL1/LL1Grammar.cpp \
        LR0/LR0Grammar.cpp \
        LR0/LR0Item.cpp \
        LR0/LRParsingHelper.cpp \
        LR0/LRTable.cpp \
        base/errorReport.cpp \
        grammar/grammar.cpp \
        grammar/grammarAlgos.cpp \
        grammar/production.cpp \
        grammar/syntaxTree.cpp \
        lexer/cLexer.cpp \
        lexer/lexer.cpp \
        lexer/qLexer.cpp \
        lexer/tLexer.cpp \
        lexer/tokenStream.cpp \
        main.cpp \
        tests/grammarParserTest.cpp \
        tests/regGrammarTest.cpp \
        utils/nameTable.cpp \
        visualize/grammarCompiler.cpp \
        visualize/regexprtestwindow.cpp \
        visualize/syntaxtreewindow.cpp

HEADERS += \ \
    FA/FA.h \
    FA/FAAlgorithm.h \
    FA/FAs.h \
    FA/compile.h \
    FA/determinize.h \
    FA/minimize.h \
    LALR1/LALR1Grammar.h \
    LL1/LL1Grammar.h \
    LR0/LR0Grammar.h \
    LR0/LR0Item.h \
    LR0/LRParsingHelper.h \
    LR0/LRTable.h \
    base/base.h \
    base/errorReport.h \
    grammar/grammar.h \
    grammar/grammarAlgos.h \
    grammar/production.h \
    grammar/syntaxTree.h \
    lexer/cLexer.h \
    lexer/lexer.h \
    lexer/qLexer.h \
    lexer/tLexer.h \
    lexer/tokenStream.h \
    tests/grammarParserTest.h \
    tests/regGrammarTest.h \
    utils/bijection.hpp \
    utils/containerUtil.hpp \
    utils/invokeful.hpp \
    utils/nameTable.h \
    utils/utils.hpp \
    visualize/FAViz.hpp \
    visualize/grammarCompiler.h \
    visualize/regexprtestwindow.h \
    visualize/syntaxtreewindow.h
