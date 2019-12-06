#ifndef REGEXPRTESTWINDOW_H
#define REGEXPRTESTWINDOW_H

#include "../base/base.h"
#include "../FA/FAs.h"

#ifdef _USE_QT

#include <QGridLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>

#include <QMainWindow>
#include <QObject>


class regExprTestWindow : public QMainWindow
{
    typedef regExprTestWindow _thisType;
    Q_OBJECT
public:
    inline
    const static QString defaultRegExpr = "(a|b)*(aa|bb)(a|b)*";

    inline
    const static QString dfaVizFilename = "dfa.dot.pdf";
    inline
    const static QString nfaVizFilename = "nfa.dot.pdf";
    inline
    const static QString mindfaVizFilename = "mindfa.dot.pdf";

public:
    explicit regExprTestWindow(QWidget *parent = nullptr);
    ~regExprTestWindow();

    void initUI();
    void connectSignals();

    QVector<QTextCursor> fullTextFA(QTextCursor);

signals:

public slots:
    void onTextChanged();
    void toResetRegExpr();
    void toGenerate();
    void toShowNFA();
    void toShowDFA();
    void toShowMinDFA();

private:

    void setColorToCursor(QTextCursor cursor,QColor color);

    DFA dfa;
    QGridLayout *pMainLayout;

    QLineEdit *pRegLine;
    QTextEdit *pText;

    QPushButton *pBtnDefaultReg;
    QPushButton *pBtnGenerate;
    QPushButton *pBtnShowNFA;
    QPushButton *pBtnShowDFA;
    QPushButton *pBtnShowMinDFA;
};

#endif // REGEXPRTESTWINDOW_H

#endif //_USE_QT
