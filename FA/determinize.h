#ifndef DETERMINIZE_H
#define DETERMINIZE_H

#include "FAs.h"
DFA determinize(const NFA &nfa, std::vector<std::vector<nodesetType>>* pMidResult = nullptr);

#endif //DETERMINIZE_H
