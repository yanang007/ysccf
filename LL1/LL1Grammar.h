#ifndef LL1GRAMMAR_H
#define LL1GRAMMAR_H

#include "../grammar/grammar.h"

class LL1Grammar : protected grammar
{
public:
    static LL1Grammar analyze(const grammar&);

private:
    LL1Grammar();
    ~LL1Grammar();
};

#endif // LL1GRAMMAR_H
