#ifndef GRAMMARALGOS_H
#define GRAMMARALGOS_H

#include "./grammar.h"
#include "../lexer/lexer.h"

std::tuple<
    std::map<symbolID,std::set<lexer::tokenID>>,
    std::map<symbolID,std::set<lexer::tokenID>>
>
    firstNfollow(const grammar&);

#endif // GRAMMARALGOS_H
