#ifndef GRAMMARALGOS_H
#define GRAMMARALGOS_H

#include "./grammar.h"

std::tuple<
    std::map<nodeType,std::set<lexer::tokenID>>,
    std::map<nodeType,std::set<lexer::tokenID>>
>
    firstNfollow(const grammar&);

std::set<nodeType> nullableVnSet(const grammar&);

#endif // GRAMMARALGOS_H
