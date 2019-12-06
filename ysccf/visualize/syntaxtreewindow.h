#ifndef SYNTAXTREEWINDOW_H
#define SYNTAXTREEWINDOW_H

#include "../base/base.h"
#ifdef _USE_QT

#include <QMainWindow>
#include <QStandardItemModel>
#include <QTreeView>
#include <QGridLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QCheckBox>

#include "../LR0/LR0Grammar.h"
#include "../lexer/tLexer.h"
#include "../utils/nameTable.h"
#include "grammarCompiler.h"

class syntaxTreeWindow : public QMainWindow
{
    using _thisType = syntaxTreeWindow;
    Q_OBJECT
public:
    syntaxTreeWindow(QWidget *parent = nullptr);
    ~syntaxTreeWindow();


    void initUI();
    void connectSignals();
    void initGrammarCompiler();

    void applySyntaxTreeToView(pcSyntaxTree tree,
                               const nameTable& symbolTable,
                               const nameTable& tokenTable,
                               int shouldOpt);
    void applySyntaxTreeToViewImpl(QStandardItem * pItem, pcSyntaxTree tree,
                                   const nameTable& symbolTable,
                                   const nameTable& tokenTable);
    void applySyntaxTreeToViewLinearImpl(QStandardItem * pItem, pcSyntaxTree tree,
                                         const nameTable& symbolTable,
                                         const nameTable& tokenTable);
    void applyDecoration(const lexer::tokenStream &stream, QTextEdit*);
public slots:
    void toParseAGrammar();
    void toParseByTheGrammar();
    void toConstructGrammar();
    void toSetDefaultGrammar();
    void toSetSimpleGrammar();
    void toApplySynTree(int shouldOptimize);

protected:
    QStandardItemModel *pModel;// = new QStandardItemModel(ui->treeView);
    QTreeView *pTreeView;// = new QStandardItemModel(ui->treeView);

    QGridLayout *pMainLayout;

    QTextEdit *pGrammarRaw;
    QTextEdit *pSrcRaw;

    QPushButton *pBtnDefaultGrammar;
    QPushButton *pBtnSimpleGrammar;
    QPushButton *pBtnConstruct;
    QCheckBox *pChkChainOpt;
    QPushButton *pBtnShowMinDFA;

    grammarCompiler gm;

/*
    qLexer lexer;
    qLexer::tokenID
        _kwNull,_kwToken,
        _beforeVn,_afterVn,
        _stringConst,_comment,
        _deducer,_delimiter,_or,_space,
        _identifier;

    LR0Grammar* pGrammarParser;
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
    LR0Grammar* pCustomParser;
    qLexer customLexer;
    nameTable customTokenTable;
    nameTable customSymbolTable;
*/
    LR0Grammar* pCustomParser = nullptr;
    bijection<tLexer::tokenID,QColor> tokenColorTable;

    tLexer::tokenStream grammarTokens;
    tLexer::tokenStream srcTokens;

    pSyntaxTree lastParsed;
    bool isLastParsedAGrammar = false;

    void setBackColorToCursor(QTextCursor selectionCursor, QColor color);
    QColor randomColor() const;
    short randomIn(short minn,short maxx) const { return rand()%(maxx-minn) + minn; }
    void setToolTipToCursor(QTextCursor selectionCursor, QString tip);
    void analyzeSyntaxTree(pSyntaxTree);

    void toExpandAll();
};

bool operator<(const QColor& lhv,const QColor& rhv);

#endif // SYNTAXTREEWINDOW_H

#endif //_USE_QT
