#ifndef LALR1GRAMMAR_H
#define LALR1GRAMMAR_H

#include "../grammar/grammar.h"

class LALR1Grammar : protected grammar
{
public:
    static LALR1Grammar analyze(const grammar&);

public:
    void tokenize();

private:
    LALR1Grammar();
    ~LALR1Grammar();
};

#endif // LALR1GRAMMAR_H
