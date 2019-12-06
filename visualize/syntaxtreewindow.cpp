#include "../base/base.h"
#ifdef _USE_QT

#include <QGridLayout>
#include <QTextEdit>
#include <QPushButton>

#include <sstream>

#include "syntaxtreewindow.h"
#include "../utils/invokeful.hpp"

syntaxTreeWindow::syntaxTreeWindow(QWidget *parent)
    :QMainWindow(parent)
{
    initUI();
    connectSignals();
    initGrammarCompiler();
    toSetDefaultGrammar();
    toConstructGrammar();
}

syntaxTreeWindow::~syntaxTreeWindow()
{
    if  (pCustomParser!= nullptr )
        delete pCustomParser;
}

void syntaxTreeWindow::initUI()
{
    pMainLayout = new QGridLayout();

    pGrammarRaw = new QTextEdit();
    pGrammarRaw->setAcceptRichText(false);
    pSrcRaw = new QTextEdit();
    pSrcRaw->setAcceptRichText(false);

    pTreeView = new QTreeView();
    pModel = new QStandardItemModel(pTreeView);
    pTreeView->setModel(pModel);
    pModel->setHorizontalHeaderLabels(
                QStringList()<<QStringLiteral("V*Tree")
                            <<QStringLiteral("VtInfo")
                            <<QStringLiteral("Type")
                );

    pBtnDefaultGrammar = new QPushButton(u8"编译器文法");
    pBtnSimpleGrammar = new QPushButton(u8"题目文法");
    pBtnConstruct = new QPushButton(u8"构建文法");
    pChkChainOpt = new QCheckBox(u8"链式递归显示优化");
    pChkChainOpt->setCheckState(Qt::Checked);

    pMainLayout->addWidget(pGrammarRaw,0,0,7,4);
    pMainLayout->addWidget(pSrcRaw,0,4,7,4);
    pMainLayout->addWidget(pTreeView,0,8,5,4);
    pMainLayout->addWidget(pBtnDefaultGrammar,5,8,1,2);
    pMainLayout->addWidget(pBtnSimpleGrammar,5,10,1,2);
    pMainLayout->addWidget(pBtnConstruct,6,8,1,2);
    pMainLayout->addWidget(pChkChainOpt,6,10,1,2);

    /*
    pMainLayout->addWidget(pBtnGenerate,0,3,1,1);
    pMainLayout->addWidget(pBtnDefaultReg,0,4,1,1);
    pMainLayout->addWidget(pBtnShowNFA,1,3,1,2);
    pMainLayout->addWidget(pBtnShowDFA,2,3,1,2);
    pMainLayout->addWidget(pBtnShowMinDFA,3,3,1,2);
    */

    setCentralWidget(new QWidget());
    centralWidget()->setLayout(pMainLayout);
}

void syntaxTreeWindow::connectSignals()
{
    connect(pGrammarRaw,&QTextEdit::textChanged,
            this,&_thisType::toParseAGrammar);
    connect(pSrcRaw,&QTextEdit::textChanged,
            this,&_thisType::toParseByTheGrammar);
    connect(pBtnDefaultGrammar,&QPushButton::clicked,
            this,&_thisType::toSetDefaultGrammar);
    connect(pBtnSimpleGrammar,&QPushButton::clicked,
            this,&_thisType::toSetSimpleGrammar);
    connect(pBtnConstruct,&QPushButton::clicked,
            this,&_thisType::toConstructGrammar);
    connect(pChkChainOpt,&QCheckBox::stateChanged,
            this,&_thisType::toApplySynTree);
}

void syntaxTreeWindow::initGrammarCompiler()
{
    for ( const auto& [id,name] : gm.compilerTokenTable().yTox()){
        tokenColorTable.insert(id,randomColor());
    }
}

void syntaxTreeWindow::applySyntaxTreeToView(pcSyntaxTree tree, const nameTable &symbolTable, const nameTable &tokenTable, int shouldOpt)
{
    //pModel->clear();
    pModel->removeRows(0,pModel->rowCount());
    if( shouldOpt > 0 )
        applySyntaxTreeToViewLinearImpl(pModel->invisibleRootItem(),tree,symbolTable,tokenTable);
    else
        applySyntaxTreeToViewImpl(pModel->invisibleRootItem(),tree,symbolTable,tokenTable);
    toExpandAll();
}


void syntaxTreeWindow::applySyntaxTreeToViewImpl(QStandardItem * pItem, pcSyntaxTree tree,
                                                 const nameTable& symbolTable,
                                                 const nameTable& tokenTable)
{
    QList<QStandardItem *> items;
    auto visitors = overloaded(
        [&tokenTable,&items](const lexer::tokenUnit* ptoken){
            items = QList<QStandardItem *>{
                new QStandardItem(QString::fromStdWString(tokenTable.ref(ptoken->id))),
                new QStandardItem(QString::fromStdWString(ptoken->info.raw)),
                new QStandardItem(QString::fromStdWString(tokenTable.ref(ptoken->id))),
            };
        },
        [&symbolTable,&items](producerType producer){
            items = QList<QStandardItem *>{
                new QStandardItem(QString::fromStdWString(symbolTable.ref(producer))),
                new QStandardItem(),
                new QStandardItem(QString::fromStdWString(symbolTable.ref(producer))),
            };
        }
    );

    std::visit(visitors,tree->data());
    pItem->appendRow(items);

    std::for_each(
        tree->children().rbegin(),
        tree->children().rend(),
        [&tokenTable,&symbolTable,this,&items]
        (const auto& pChild) mutable {
            applySyntaxTreeToViewImpl(items.front(),pChild,
                                      symbolTable,
                                      tokenTable);
        }
    );
}

void syntaxTreeWindow::applySyntaxTreeToViewLinearImpl(QStandardItem *pItem, pcSyntaxTree tree,
                                                       const nameTable& symbolTable,
                                                       const nameTable& tokenTable)
{
    QList<QStandardItem *> items;
    auto visitors = overloaded(
        [&tokenTable,&items](const lexer::tokenUnit* ptoken){
            items = QList<QStandardItem *>{
                new QStandardItem(QString::fromStdWString(tokenTable.ref(ptoken->id))),
                new QStandardItem(QString::fromStdWString(ptoken->info.raw)),
                new QStandardItem(QString::fromStdWString(tokenTable.ref(ptoken->id))),
            };
        },
        [&symbolTable,&items]([[maybe_unused]]producerType producer){
            items = QList<QStandardItem *>{
                new QStandardItem(QString::fromStdWString(symbolTable.ref(producer))),
                new QStandardItem(),
                new QStandardItem(QString::fromStdWString(symbolTable.ref(producer))),
            };
        }
    );

    std::visit(visitors,tree->data());
    pItem->appendRow(items);
    chainSyntaxVisit(
        tree,
        [&tokenTable,&symbolTable,this,&items](pcSyntaxTree pNode,nodeType node){
        applySyntaxTreeToViewLinearImpl(items.front(),pNode,
                                        symbolTable,
                                        tokenTable);
        return false;
    });
}

void syntaxTreeWindow::applyDecoration(const lexer::tokenStream& stream, QTextEdit * pSrc)
{
    static bool isMerging = false;
    if( !isMerging ){
        isMerging = true;
    }
    else{
        return;
    }

    QTextCursor cursor(pSrc->document());
    cursor.select(QTextCursor::Document);
    setBackColorToCursor(cursor,Qt::white);
    setToolTipToCursor(cursor,"");

    for( const auto& unit : stream ){
        if(unit.id == lexer::fin){
            break;
        }
        QTextCursor cursor(pSrc->document());
        auto _begin = unit.info.pos;
        auto _end = _begin + unit.info.raw.size();
        cursor.setPosition((int)_begin);
        cursor.setPosition((int)_end,QTextCursor::KeepAnchor);
        setBackColorToCursor(cursor,tokenColorTable.at(unit.id));
        setToolTipToCursor(cursor,QString::fromStdWString(gm.compilerTokenTable().at(unit.id)));
    }

    isMerging = false;
}


void syntaxTreeWindow::toParseAGrammar()
{
    static bool isMerging = false;
    if( !isMerging ){
        isMerging = true;
    }
    else{
        return;
    }

    auto text = pGrammarRaw->document()->toPlainText().toStdWString();
    grammarTokens = gm.tokenize(text);
    applyDecoration(grammarTokens,pGrammarRaw);
    isMerging = false;

    auto syntaxtree = gm.parse(grammarTokens);
    applySyntaxTreeToView(syntaxtree,
                          gm.compilerSymbolTable(),
                          gm.compilerTokenTable(),
                          pChkChainOpt->checkState());
    toExpandAll();

    isLastParsedAGrammar = true;
    lastParsed = syntaxtree;
}

void syntaxTreeWindow::toParseByTheGrammar()
{
    static bool isMerging = false;
    if( !isMerging ){
        isMerging = true;
    }
    else{
        return;
    }

    if( pCustomParser == nullptr ){
        isMerging = false;
        return;
    }
    auto text = pSrcRaw->document()->toPlainText().toStdWString();
    srcTokens = gm.grammarLexer().tokenize(text);

    applyDecoration(srcTokens,pSrcRaw);
    isMerging = false;

    if(pCustomParser == nullptr){
        return;
    }

    LR0Grammar::parseStepVecType steps;
    auto syntaxtree = pCustomParser->parse(srcTokens,&steps);
    pCustomParser->printParseStepVec(std::wcout,
                                     steps,
                                     srcTokens,
                                     gm.grammarSymbolTable(),
                                     gm.grammarTokenTable());
    applySyntaxTreeToView(syntaxtree,
                          gm.grammarSymbolTable(),
                          gm.grammarTokenTable(),
                          pChkChainOpt->checkState());
    toExpandAll();

    isLastParsedAGrammar = false;
    lastParsed = syntaxtree;

}

void syntaxTreeWindow::toExpandAll()
{
    pTreeView->expandAll();
    pTreeView->resizeColumnToContents(0);
}

void syntaxTreeWindow::toConstructGrammar()
{
    LR0Grammar::statesVecType states;
    if(pCustomParser != nullptr)
        delete pCustomParser;
    pCustomParser = new LR0Grammar(LR0Grammar::analyze(gm.construct(lastParsed),&states));
    pCustomParser->printStateVec(
                std::wcout,
                states,
                gm.grammarSymbolTable(),
                gm.grammarTokenTable()
            );
    gm.grammarLexer().toStream(std::wcout,gm.grammarTokenTable());
    gm.lastConstruct().toStream(std::wcout,
                                gm.grammarSymbolTable(),
                                gm.grammarTokenTable());
}

void syntaxTreeWindow::toSetDefaultGrammar()
{
    std::basic_stringstream<charType> ss;
    gm.compilerLexer().
            toStream(ss,
                     gm.compilerTokenTable());
    ((grammar&)gm.getCompilerGrammar())
            .toStream(ss,
                      gm.compilerSymbolTable(),
                      gm.compilerTokenTable());
    QString def = QString::fromStdWString(ss.str());
    pSrcRaw->setText(def);
    pGrammarRaw->setText(def);
}

void syntaxTreeWindow::toSetSimpleGrammar()
{
    /*
    constexpr static char simpleGrammar[]=
R"dil(<E> ::= <T> <A>;
<A> ::= "+" <T> <A>;
<A> ::= null;
<T> ::= <F> <B>;
<B> ::= "*" <F> <B>;
<B> ::= null;
<F> ::= "(" <E> ")";
<F> ::= "i";
)dil";
*/
constexpr static char simpleGrammar[]=
R"dil( %token plus "\+"
%token mul "\*"
%token lbra "\("
%token rbra "\)"
%token i "i"
<E> ::= <T> <A>;
<A> ::= plus <T> <A>;
<A> ::= null;
<T> ::= <F> <B>;
<B> ::= mul <F> <B>;
<B> ::= null;
<F> ::= lbra <E> rbra;
<F> ::= i;
)dil";




    pGrammarRaw->setText(QString::fromLocal8Bit(simpleGrammar));
}

void syntaxTreeWindow::toApplySynTree(int shouldOptimize)
{
    if( isLastParsedAGrammar )
        applySyntaxTreeToView(lastParsed,
                              gm.compilerSymbolTable(),
                              gm.compilerTokenTable(),
                              shouldOptimize);
    else
        applySyntaxTreeToView(lastParsed,
                              gm.grammarSymbolTable(),
                              gm.grammarTokenTable(),
                              shouldOptimize);
}

void syntaxTreeWindow::setBackColorToCursor(QTextCursor selectionCursor, QColor color)
{
    QTextCharFormat plainFormat(selectionCursor.charFormat());
    QTextCharFormat colorFormat = plainFormat;
    colorFormat.setBackground(color);
    selectionCursor.mergeCharFormat(colorFormat);
}

void syntaxTreeWindow::setToolTipToCursor(QTextCursor selectionCursor,QString tip)
{
    QTextCharFormat plainFormat(selectionCursor.charFormat());
    QTextCharFormat toolTipFormat = plainFormat;
    toolTipFormat.setToolTip(tip);
    selectionCursor.mergeCharFormat(toolTipFormat);
}

void syntaxTreeWindow::analyzeSyntaxTree(pSyntaxTree pGrammarDef)
{
    if( lastParsed == nullptr ){
        return;
    }

    pSyntaxTree pStmt = nullptr;
    nodeType stmtType;
    nodeType producer;
    auto producerTell = overloaded(
        [](const lexer::tokenUnit *ptoken){
            return;
        },
        [&producer](producerType proder){
            producer = proder;
        }
    );

    chainSyntaxVisit(pGrammarDef,[](pcSyntaxTree pNode,nodeType node){
        node++;
        return false;
    });
}

QColor syntaxTreeWindow::randomColor() const
{
    auto r = randomIn(130,255);
    auto g = randomIn(130,255);
    auto b = randomIn(130,255);
    return QColor(r,g,b);
}

bool operator<(const QColor &lhv, const QColor &rhv){
    int r,g,b,a;
    lhv.getRgb(&r,&g,&b,&a);
    auto tl = std::tuple(r,g,b,a);
    rhv.getRgb(&r,&g,&b,&a);
    auto tr = std::tuple(r,g,b,a);
    return tl<tr;
}

#endif
