#ifndef TONFA_H
#define TONFA_H

#include <stack>

#include "FAs.h"


enum char_type{
    ctNoChar,
    ctChar,
    ctOperator,
    ctEscaped
};

bool isOperator(charType a);
int opPriority(charType c);
charType opConv(charType a);

nodePairType convertToMinimalGraph(charType ch,NFA& nfa);

nodePairVecType requireParam(
        size_t n,
        std::stack<nodePairType>& resultStack );

bool doCalc(
        charType op,
        charType& incentive,
        std::pair<charType,char_type> lookback,
        std::pair<charType,char_type> lookahead,
        std::stack<charType>& operatorStack,
        std::stack<nodePairType>& resultStack,
        NFA& nfa );

void pushOperator(
        charType op,
        std::stack<char>& operatorStack,
        std::stack<nodePairType>& resultStack,
        NFA& nfa  );

NFA compile(const stringType& expr);
DFA directCompile(const stringType& str);

#endif //TONFA_H
