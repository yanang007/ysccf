#include "LR0Grammar.h"
#include "LR0Item.h"

#include <vector>
#include <stack>
#include <variant>

#include "../base/errorReport.h"
#include "../utils/invokeful.hpp"
#include "../grammar/production.h"
#include "../utils/nameTable.h"
#include "../utils/containerUtil.hpp"

LR0Grammar LR0Grammar::analyze(
        const grammar& _gramr,
        statesVecType* pStatesVec,
        transferMapVecType* pTransferMapVec)
{
    LR0Grammar retSyn;

    retSyn.grammar::operator=(std::move(_gramr));
    //extend grammar
    grammar& egramr = retSyn;
    nodeType Sq = egramr.newSymbol();

    production eprod;
    eprod.appendSymbol(egramr.start());
    auto launcherProductionID = egramr.addProduction(Sq,eprod);

    /*
    auto _prods = egramr.productions(Sq);
    if(_prods.empty()){
        errorReport(L"LR0Grammar::analyze()",L"failed to extend grammar",0);
        //std::cout<<"LR0Grammar::analyze : failed to extend grammar";
        return retSyn;
    }
    auto startPoint = _prods.front();
    //LR0Item item0(Sq,startPoint);*/
    LR0Item item0(egramr,launcherProductionID);

    LR0ItemSet currentItems;
    currentItems.insert(item0);

    std::map<
        production::producedType,
        LR0ItemSet
    > newItemsTransfers;

    std::map<
        production::producedType,
        nodeType
    > newItemsTransfers_compiled;

    std::vector<LR0ItemSet> stateVec;
    std::vector<decltype(newItemsTransfers_compiled)> transferMapVec;

    stateVec.push_back(closure(currentItems,egramr));

    size_t i = 0;
    while ( i < stateVec.size() ) {
        currentItems = stateVec[i];
        newItemsTransfers.clear();
        newItemsTransfers_compiled.clear();

        for( const auto& item :  currentItems){
            //newStatesSet.push_back(_eps_closure_move(states,nfa,ch));
            if(!item.atEnd())
                newItemsTransfers[item.get()].insert(item.advance());
        }

        for( const auto& [path,itemSetCore] : newItemsTransfers ){
            auto itemSet = closure(itemSetCore,egramr);
            auto ipos = ufind(stateVec,itemSet);
            nodeType newItemsetId;
            if( ipos == stateVec.end() ){ // not recognized
                newItemsetId = stateVec.size();
                stateVec.push_back(itemSet);
            }
            else{
                newItemsetId = ipos - stateVec.begin();
            }
            newItemsTransfers_compiled[path] = newItemsetId;
        }
        transferMapVec.push_back(newItemsTransfers_compiled);

        ++i;
    }

    retSyn.fillTable(stateVec,transferMapVec,launcherProductionID);

    if( pStatesVec != nullptr ){
        *pStatesVec = std::move(stateVec);
    }
    if( pTransferMapVec != nullptr ){
        *pTransferMapVec = std::move(transferMapVec);
    }

    return retSyn;
}

ostreamType &LR0Grammar::printStateVec(ostreamType &os,
        const statesVecType &statesVec,
        const nameTable &symbolTable,
        const nameTable &tokenTable)
{
    int sd = 0;
    for( const auto& state : statesVec ){
        std::wcout<<"state:" <<sd << std::endl;
        for( const auto& invalidDotProd : state ){
            const auto [producer,p] = this->getHandle(invalidDotProd.production());
            const auto& prod = this->productionAt(invalidDotProd.production());
            LR0Item dotProd = LR0Item(
                    producer,
                    prod.end()-(invalidDotProd.getEnd()-invalidDotProd.getIter()),
                    prod.end(),
                    invalidDotProd.production());
            const auto [node,type] = dotProd.get();
            int count = 0;
            os << "<"
               << symbolTable.ref(producer)
               << ">"
               << LR0Grammar::sDeductionSymbol;

            for(const auto &component : prod){
                if( count == dotProd.getIter() - prod.begin() )
                    os <<L"·";

                switch (component.second) {
                case prod.itSymbol:
                    os << " "
                       << production::sBeforeSymbol
                       << symbolTable.ref(component.first)
                       << production::sAfterSymbol << " ";
                    break;
                case prod.itToken:
                    os << " ("
                       << tokenTable.ref(component.first)
                       << ") ";
                    break;
                default:
                    os << " !!!"
                       << "fatal error"
                       << "!!! ";
                    break;
                }
                ++count;
            }
            if( count == dotProd.getIter() - prod.begin() )
                os<<L"·";
            os<<std::endl;
        }
        ++sd;
    }
    return os;
}

ostreamType &LR0Grammar::printParseStepVec(ostreamType& os,
        const LR0Grammar::parseStepVecType& parseStepVec,
        const lexer::tokenStream &tokens,
        const nameTable &symbolTable,
        const nameTable &tokenTable,
        size_t foreseen)
{
    static const auto processors = overloaded(
        [&os,&tokenTable](const lexer::tokenUnit* ptoken){
            if( ptoken->id != lexer::fin ){
                os<< production::sBeforeToken
                  << tokenTable.ref(ptoken->id)
                  << production::sAfterToken;
            }
            else{
                os<< '#';
            }

        },
        [&os,&symbolTable](producerType producer){
            os<< production::sBeforeSymbol
              << symbolTable.ref(producer)
              << production::sAfterSymbol;
        }
    );
    for( const auto& [stepNo,stepDetail] : enumerate(parseStepVec) ){
        os<<std::endl<<"("<<stepNo<<") "<<std::endl;
        os<<"[";
        for(const auto& stateID : stepDetail.stateStk){
            os<<" "<<stateID;
        }
        os<<"]\t [";
        for(const auto& symbol : stepDetail.symbolStk){
            std::visit(processors,symbol);
            os<<" ";
        }
        os<<"]\t [";
        for(size_t i = 0; i< foreseen; ++i){
            if(i+stepDetail.inputPos >= tokens.size())
                break;
            processors(&tokens[i+stepDetail.inputPos]);
            os<<" ";
        }
        os<<"]\t ";
        os<<"?= ";
        const auto& [dest,type] = stepDetail.action;
        switch ( type ) {
        case LRTable::ateShift:
            os<<"S"<<dest;
            break;

        case LRTable::ateReduce:
            os<<"r"<<dest<<" goto " << stepDetail._goto << " (";
            printDeduction(
                    os,
                    getHandle(dest).producer,
                    productionAt(dest),
                    symbolTable,tokenTable);
            //this->productionAt(dest).toStream(os,symbolTable,tokenTable);
            os<<")";
            break;

        case LRTable::ateAccept:
            os<<"acc "<<dest;
            break;
        }
    }

    return os;
}

LR0Grammar::LR0Grammar()
{

}

LR0Grammar::~LR0Grammar()
{

}

pSyntaxTree LR0Grammar::parse(
        const lexer::tokenStream &tokens,
        parseStepVecType* pParseStepVec)
{
    auto iter = tokens.begin();
    auto ender = tokens.end();

    std::deque<LRTable::rowNoType> stateStk;
    std::deque<uniSymbol> symbolStk;
    std::deque<pSyntaxTree> treeStack;

    static_assert (isDequeLike<std::deque<LRTable::rowNoType>>::value,"" );

    stateStk.push_back(table.start());
    //launcher state is assumed to be zero
    //which is assured by LR0Grammar::analyze()
    //any derived class should do the same
    //or provide custom parser according to your definition

    LRTable::rowNoType state;
    const lexer::tokenUnit* pToken;
    const production* pProd;
    productionHandle hnd;
    LRTable::actionTypeEnum type;
    nodeType dest;
    pSyntaxTree pRoot;

    auto processors = overloaded(
        [&pRoot](const lexer::tokenUnit* ptoken){
            pRoot->appendChild(pSyntaxTree(new syntaxTree(ptoken)));
        },
        [&treeStack,&pRoot]([[maybe_unused]]producerType producer){
            pRoot->appendChild(toppop(treeStack));
        }
    );
    //<_moreRule> ::= _or <_VList> <_moreRule> | null ;
try {
    while ( iter < ender ) {
        state = stateStk.back();
        pToken = &(*iter);
        auto act = table.getAction(state,pToken->id);
        dest = act.dest;
        type = act.type;
        //auto [dest,type] = table.getAction(state,pToken->id);

        if( pParseStepVec != nullptr ){
            pParseStepVec->push_back(
                {stateStk,
                 symbolStk,
                 size_t(iter-tokens.begin()),
                 act,
                 nodeNotExist }
            );
        }

        switch ( type ) {
        case LRTable::ateShift:
            stateStk.push_back(dest);
            symbolStk.push_back(pToken);
            ++iter;

        break;

        case LRTable::ateReduce:
            pProd = &productionAt(dest);
            hnd = getHandle(dest);
            pRoot.reset(new syntaxTree(hnd.producer));
            for ( size_t i = 0; i<pProd->size(); ++i){
                stateStk.pop_back();
                auto sym = toppop(symbolStk);
                std::visit(processors,sym);
            }
            treeStack.push_back(pRoot);
            symbolStk.push_back(hnd.producer);

            state = stateStk.back();
            dest = table.getGoto(state,hnd.producer);

            if( pParseStepVec != nullptr ){
                pParseStepVec->back()._goto = dest;
            }

            stateStk.push_back(dest);

        break;

        case LRTable::ateAccept:
            ++iter;
            break;
        }
    }

    if(treeStack.empty()){
        throw std::out_of_range("LR0Grammar::parse(): invalid input!");
    }
    return treeStack.back();

} catch (std::out_of_range e) {
        errorReport(L"LR0Grammar::parse()",L"the input is not accepted!",19);
        //the input is not accepted
        pRoot.reset(new syntaxTree(nodeNotExist));
        while ( !symbolStk.empty() ){
            auto sym = toppop(symbolStk);
            std::visit(processors,sym);
        }
        while ( !treeStack.empty() ){
            pRoot->appendChild(toppop(treeStack));
        }
        treeStack.push_back(pRoot);
}
    return treeStack.back();
}

//SLR1
void LR0Grammar::fillTable(const std::vector<LR0ItemSet> &stateVec, const std::vector<std::map<production::producedType, nodeType> > &transferMapVec, productionID acceptedProduction)
{
    table.clear();
    table.resize(stateVec.size());

    for ( size_t i = 0; i < stateVec.size(); ++i )
    {
        const auto& items = stateVec[i];
        const auto& routines = transferMapVec[i];

        for ( const auto& item : items ){
            if( item.get().second == production::itReduced ){
                if( item.production() == acceptedProduction ){
                    table.setAction(
                        i,
                        lexer::fin,
                        item.production(),
                        LRTable::ateAccept
                    );
                }
                else{
                    table.setAction(
                        i,
                        lexer::allRoads,
                        item.production(),
                        LRTable::ateReduce
                    );
                }
                break;
            }
        }

        for( const auto& [produced,row] : routines ){
            if( produced.second == production::itSymbol ){
                table.setGoto(i,produced.first,row);
            }
            else if ( produced.second == production::itToken ){
                table.setAction(i,produced.first,row,LRTable::ateShift);
            }
        }
    }
}

/*
 //LR0
void LR0Grammar::fillTable(const std::vector<LR0ItemSet> &stateVec, const std::vector<std::map<production::producedType, nodeType> > &transferMapVec, productionID acceptedProduction)
{
    enum stateType{
        stLeaf, // reduceOnly
        stNonLeaf,
        stNotDetermined
    };
    table.clear();
    table.resize(stateVec.size());
    stateType type;

    for ( size_t i = 0; i < stateVec.size(); ++i )
    {
        type = stNotDetermined;
        const auto& items = stateVec[i];
        const auto& routines = transferMapVec[i];

        if( routines.empty() ){
            for ( const auto& item : items ){
                if( item.get().second == production::itReduced ){
                    type = stLeaf;
                    if( item.production() == acceptedProduction ){
                        table.setAction(
                            i,
                            lexer::fin,
                            item.production(),
                            LRTable::ateAccept
                        );
                    }
                    else{
                        table.setAction(
                            i,
                            LRTable::allRoads,
                            item.production(),
                            LRTable::ateReduce
                        );
                    }
                    break;
                }
            }
        }

        if( type == stLeaf ){
            continue;
        }

        for( const auto& [produced,row] : routines ){
            if( produced.second == production::itSymbol ){
                table.setGoto(i,produced.first,row);
            }
            else if ( produced.second == production::itToken ){
                table.setAction(i,produced.first,row,LRTable::ateShift);
            }
        }
    }
}
*/
