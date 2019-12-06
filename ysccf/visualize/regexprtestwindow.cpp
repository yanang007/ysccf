#include "../base/base.h"
#ifdef _USE_QT

#include <QGridLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QString>
#include <QDesktopServices>
#include <QDebug>

#include "regexprtestwindow.h"

#include "../FA/FAs.h"
#include "../FA/compile.h"
#include "../FA/determinize.h"
#include "../FA/minimize.h"
#include "../visualize/FAViz.hpp"

regExprTestWindow::regExprTestWindow(QWidget *parent) : QMainWindow(parent)
{
    initUI();
    connectSignals();
    toResetRegExpr();
    toGenerate();
}

regExprTestWindow::~regExprTestWindow()
{

}

void regExprTestWindow::initUI()
{
    pMainLayout = new QGridLayout();

    pRegLine = new QLineEdit();
    pText = new QTextEdit();
    pText->setAcceptRichText(false);

    pBtnDefaultReg = new QPushButton(u8"默认正规式");
    pBtnGenerate = new QPushButton(u8"生成自动机");
    pBtnShowNFA = new QPushButton(u8"查看NFA");
    pBtnShowDFA = new QPushButton(u8"查看DFA");
    pBtnShowMinDFA = new QPushButton(u8"查看minDFA");

    pMainLayout->addWidget(pRegLine,0,0,1,3);
    pMainLayout->addWidget(pText,1,0,3,3);
    pMainLayout->addWidget(pBtnGenerate,0,3,1,1);
    pMainLayout->addWidget(pBtnDefaultReg,0,4,1,1);
    pMainLayout->addWidget(pBtnShowNFA,1,3,1,2);
    pMainLayout->addWidget(pBtnShowDFA,2,3,1,2);
    pMainLayout->addWidget(pBtnShowMinDFA,3,3,1,2);

    setCentralWidget(new QWidget());
    centralWidget()->setLayout(pMainLayout);
}

void regExprTestWindow::connectSignals()
{
    connect(pBtnDefaultReg,&QPushButton::clicked,
            this,&_thisType::toResetRegExpr);
    connect(pBtnGenerate,&QPushButton::clicked,
            this,&_thisType::toGenerate);
    connect(pBtnShowNFA,&QPushButton::clicked,
            this,&_thisType::toShowNFA);
    connect(pBtnShowDFA,&QPushButton::clicked,
            this,&_thisType::toShowDFA);
    connect(pBtnShowMinDFA,&QPushButton::clicked,
            this,&_thisType::toShowMinDFA);

    connect(pText,&QTextEdit::textChanged,
            this,&_thisType::onTextChanged);

}

QVector<QTextCursor> regExprTestWindow::fullTextFA(QTextCursor cursur)
{
    QVector<QTextCursor> ret;
    nodeType node = nodeNotExist;
    bool isMatched = false;
    int reserved_pos = 0;
    int lastMatchedPos = 0;
    charType path;
    ushort temp;
    while( true ){
        if(cursur.atEnd() || node == nodeNotExist){
            //cursur.clearSelection();
            if( isMatched ){
                cursur.setPosition(
                            lastMatchedPos,
                            QTextCursor::KeepAnchor);
                ret.append(cursur);
                isMatched = false;
                node = nodeNotExist;

                cursur.setPosition(lastMatchedPos);
            }
            else{
                cursur.setPosition(reserved_pos);
            }
            if( cursur.atEnd() ){
                break;
            }
            node = dfa.start();
            reserved_pos = cursur.position() + 1;
        }
        temp = cursur.document()->
                characterAt(cursur.position())
                .unicode();
        cursur.movePosition(
                    QTextCursor::NextCharacter,
                    QTextCursor::KeepAnchor);
        //if( temp < 256 ){
            //is ascii
            path = temp;
            //std::cout<<path;
            node = dfa.walk(node,path);
        /*}
        else{
            qDebug()<<"non ascii\n";
            node = nodeNotExist;
        }*/
        if( node != nodeNotExist ){
            bool _matched = dfa.isTerminal(node);
            if( _matched ){
                isMatched = _matched;
                lastMatchedPos = cursur.position();
            }
        }
    }

    return ret;
}

void regExprTestWindow::onTextChanged()
{
    static bool isMerging = false;
    if( !isMerging ){
        isMerging = true;
    }
    else{
        return;
    }

    QTextCursor cursor(pText->document());
    auto lists = fullTextFA(cursor);

    cursor.select(QTextCursor::Document);
    setColorToCursor(cursor,Qt::black);

    for(auto& selectionCursor : lists){
        setColorToCursor(selectionCursor,Qt::red);
    }

    isMerging = false;
}

void regExprTestWindow::toResetRegExpr()
{
    pRegLine->setText(defaultRegExpr);
}

void regExprTestWindow::toGenerate()
{
    NFA nfa = compile(pRegLine->text().toStdWString());
    vizFA(R"(D:\libraries\Graphviz\2.38\bin\dot)","pdf","nfa",nfa);
    std::wcout<<nfa;
    auto _dfa = determinize(nfa);
    vizFA(R"(D:\libraries\Graphviz\2.38\bin\dot)","pdf","dfa",_dfa);
    std::wcout<<_dfa;
    auto mindfa = minimize(_dfa);
    vizFA(R"(D:\libraries\Graphviz\2.38\bin\dot)","pdf","mindfa",mindfa);

    //R"(.\graphviz\dot)" dist

    dfa = std::move(mindfa);
    onTextChanged();
}

void regExprTestWindow::toShowNFA()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(nfaVizFilename));
}

void regExprTestWindow::toShowDFA()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(dfaVizFilename));
}

void regExprTestWindow::toShowMinDFA()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(mindfaVizFilename));
}

void regExprTestWindow::setColorToCursor(QTextCursor selectionCursor, QColor color)
{
    QTextCharFormat plainFormat(selectionCursor.charFormat());
    QTextCharFormat colorFormat = plainFormat;
    colorFormat.setForeground(color);
    selectionCursor.mergeCharFormat(colorFormat);
}

#endif //_USE_QT
