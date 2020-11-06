#include <string>

#include "grammar.h"
#include "../utils/nameTable.h"
#include "../FA/compile.h"
#include "../FA/determinize.h"
#include "../FA/minimize.h"

ostreamType &grammar::printDeduction(
        ostreamType &os,
        nodeType producer,
        const production &production,
        const nameTable &symbolTable,
        const nameTable &tokenTable)
{
    os << production::sBeforeSymbol
       << symbolTable.ref(producer)
       << production::sAfterSymbol
       << grammar::sDeductionSymbol;
    production.toStream(os,symbolTable,tokenTable);
    return os;
}

grammar::grammar()
    :_start(nodeNotExist)
{

}

grammar::~grammar()
{

}

nodeType grammar::newSymbol()
{
    auto ret = symbolSize();
    _deductions.push_back(rulesType());
    return ret;
}

grammar::productionID grammar::addProduction(producerType producer, production prod)
{
    productionHandle hnd{producer,productions(producer).size()};

    _deductions[producer].push_back(prod);

    auto ret = _index.size();
    _index.push_back(hnd);
    _iIndex[hnd] = ret;

    return ret;
}

grammar::productionID grammar::addProduction(grammar::deductionType ded)
{
    return addProduction(ded.first,ded.second);
}

production& grammar::addProduction(nodeType producer, productionID* pid)
{
    auto ret = addProduction(producer, production());
    if (pid != nullptr) {
        *pid = ret;
    }
    return productionAt(ret);
}

ostreamType &grammar::toStream(ostreamType &os, const nameTable &symbolTable, const nameTable &tokenTable) const
{
    nodeType n = 0;
    for( const auto& prods : deductions() ){
        os << production::sBeforeSymbol << symbolTable.ref(n) << production::sAfterSymbol
           << grammar::sDeductionSymbol;
        auto c = 0;
        for( const auto& prod : prods  ){
            prod.toStream(os,symbolTable,tokenTable);
            ++c;
            if(c!=prods.size()){
                os  << grammar::sProductionDelimiter;
            }
        }
        os << ";" << std::endl;
        ++n;
    }
    return os;
}

ostreamType &operator<<(ostreamType &os, const grammar &syn)
{
    nameTable emptytable;
    return syn.toStream(os,emptytable,emptytable);
}
