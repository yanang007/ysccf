#ifndef MINIMIZE_H
#define MINIMIZE_H

#include "FAs.h"
#include "../utils/utils.hpp"
#include "FAAlgorithm.h"

DFA minimize(const DFA& dfa, std::vector<nodesetType>* pMidResult = nullptr);

nodesetType scanUnreachable(const DFA& dfa);
nodesetType scanUseless(const DFA& dfa);

inline
nodesetType scanRedundantStates(const DFA& dfa)
{
    nodesetType unreachableSet = scanUnreachable(dfa);
    nodesetType uselessSet = scanUseless(dfa);
    //printContainer(uselessSet);
    return _union(unreachableSet,uselessSet);
}


#endif //MINIMIZE_H
