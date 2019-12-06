#ifndef LR0GRAMMAR_H
#define LR0GRAMMAR_H

#include <stack>

#include "../grammar/grammar.h"
#include "LR0Item.h"

#include "LRTable.h"
#include "../lexer/lexer.h"
#include "../grammar/syntaxTree.h"

class LR0Grammar : protected grammar
{
public:
    using statesVecType = std::vector<LR0ItemSet>;
    using transferMapVecType = std::vector<std::map<production::producedType,nodeType>>;

    using uniSymbol = syntaxTree::uniSym;
    struct parseStep{
        std::deque<LRTable::rowNoType> stateStk;
        std::deque<uniSymbol> symbolStk;
        size_t inputPos;
        LRTable::actionType action;
        LRTable::rowNoType _goto;
    };
    using parseStepVecType = std::vector<parseStep>;


    static LR0Grammar analyze(
            const grammar&,
            statesVecType* pStatesVec = nullptr,
            transferMapVecType* pTransferMaps = nullptr);

public:
    LR0Grammar(const LR0Grammar&) = default;
    LR0Grammar(LR0Grammar&&) = default;
    LR0Grammar& operator=(LR0Grammar&&) = default;
    //LR0Grammar& operator=(LR0Grammar&) = default;
    ~LR0Grammar();

    pSyntaxTree parse(const lexer::tokenStream& tokens,parseStepVecType* pParseStepVec = nullptr);
    const LRTable& getTable() const { return table; }

    ostreamType& printStateVec(
            ostreamType&,
            const statesVecType&,
            const nameTable& symbolTable,
            const nameTable& tokenTable);

    ostreamType& printParseStepVec(ostreamType&,
            const parseStepVecType&,
            const lexer::tokenStream& tokens,
            const nameTable& symbolTable,
            const nameTable& tokenTable,
            size_t foreseen = 5);


protected:
    LRTable table;
    void fillTable(const std::vector<LR0ItemSet>& stateVec,
                   const std::vector<
                             std::map<
                                 production::producedType,
                                 nodeType
                             >
                         >& transferMapVec,
                   productionID acceptedProduction);

private:
    LR0Grammar();
};

#endif // LR0GRAMMAR_H
