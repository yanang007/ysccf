#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <map>

#include "../base/base.h"
#include "production.h"
#include "grammarTypes.h"
#include "../utils/invokeful.hpp"

class grammar
{
public:
    inline
    static const stringType sDeductionSymbol = L" ::= ";
    inline
    static const stringType sProductionDelimiter = L"| ";

    using productionID = size_t;
    using rulesType = std::vector<production>;
    using rulesVecType = std::vector<rulesType>;
    using ruleIndicesType = std::vector<productionHandle>;
    using ruleIdsType = std::map<productionHandle,productionID>;
    using deductionType = std::pair<symbolID,production>;
    using deductionsType = std::pair<symbolID,rulesType>;

    static
    ostreamType& printDeduction(
            ostreamType& os,
            symbolID producer,
            const production& production ,
            const nameTable& symbolTable,
            const nameTable& tokenTable);


public:
    grammar();
    ~grammar();

    symbolID newSymbol();
    //引入一个新非终结符Vn
    productionID addProduction(producerType, production);
    productionID addProduction(deductionType);
    production& addProduction(symbolID, productionID* p = nullptr);

    const production& productionAt(productionID id) const { return productionAt(getHandle(id)); }
    const production& productionAt(productionHandle hnd) const { auto&& [prder,prd] = hnd; return _deductions[prder][prd]; }
    productionHandle getHandle(productionID id) const { return _index[id]; }
    productionID getID(const productionHandle& hdn) const { return _iIndex.at(hdn); }
    productionID getID(producerType producer,size_t productionPos) const { return _iIndex.at(productionHandle{producer,productionPos}); }

    symbolID start() const { return _start; }
    void markStart(symbolID n) { _start = n; }

    size_t symbolSize() const { return _deductions.size(); }
    size_t symbolEmpty() const { return symbolSize() == 0; }
    size_t size() const { return _index.size(); } //返回当前grammar中的产生式数目
    bool empty() const { return size() == 0; } //返回当前grammar是否为空

    const rulesVecType& deductions() const { return _deductions; }
    const rulesType& productions(symbolID n) const { return _deductions[n]; }
    const ruleIndicesType& index() const { return _index; }

    ostreamType& toStream(ostreamType& os, const nameTable& symbolTable, const nameTable& tokenTable) const;


    void clear(){
        _deductions.clear();
        _index.clear();
        _iIndex.clear();
        markStart(0);
    }

protected:
    production& productionAt(productionID id) { 
        return deconstify(constify(this)->productionAt(getHandle(id)));
    }

    rulesVecType _deductions;
    ruleIndicesType _index;
    ruleIdsType _iIndex;
    symbolID _start;
};

ostreamType& operator<<(ostreamType& os, const grammar& syn);

/*inline
productionHandle::productionHandle(producerType producer, size_t productionPos)
    :producer(producer),productionPos(productionPos)
{

}*/

inline
bool productionHandle::operator==(const productionHandle& rhv) const{
    return producer == rhv.producer &&
            productionPos == rhv.productionPos;
}

inline
bool productionHandle::operator<(const productionHandle& rhv) const{
    if(producer < rhv.producer){
        return true;
    }
    else if(producer == rhv.producer){
        return productionPos < rhv.productionPos;
    }
    else{
        return false;
    }
}


#endif // GRAMMAR_H
