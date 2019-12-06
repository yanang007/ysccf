#include "syntaxTree.h"

#include <stack>

#include "../utils/invokeful.hpp"
#include "../utils/nameTable.h"

ostreamType &syntaxTree::toStream(ostreamType &os, const nameTable &symbolTable, const nameTable &tokenTable) const
{
    return toStreamImpl(os,symbolTable,tokenTable,0);
}

ostreamType &syntaxTree::toStreamImpl(ostreamType &os, const nameTable &symbolTable, const nameTable &tokenTable, size_t depth) const
{
    auto visitors = overloaded(
        [&os,&tokenTable](const lexer::tokenUnit* ptoken){
            os<<production::sBeforeToken
              <<tokenTable.ref(ptoken->id)
              <<production::sAfterToken
              <<"\""
              <<ptoken->info.raw
              <<"\"("
              <<ptoken->info.pos
              <<")";
        },
        [&os,&symbolTable]([[maybe_unused]]producerType producer){
            os<<production::sBeforeSymbol
              <<symbolTable.ref(producer)
              <<production::sAfterSymbol;
        }
    );

    os<<L"── ";
    std::visit(visitors,pData);
    os<<std::endl;

    size_t tag = 0;

    std::for_each(
        children().begin(),
        children().end(),
        [tag=0,&depth,&os,&symbolTable,&tokenTable,this]
        /*traversal*/ (const auto& child) mutable {
            for( size_t i = 0; i<depth; ++i ){
                os<<L" │";
            }
            ++tag;
            if( tag == children().size() ){
                os<<L" └";
            }
            else{
                os<<L" ├";
            }
            child->toStreamImpl(os,symbolTable,tokenTable,depth+1);
        }
    );

    return os;
}

/*
void chainSyntaxVisit(pcSyntaxTree pRoot, std::function<int (pcSyntaxTree,nodeType)> func)
{
    nodeType chainGuide;
    nodeType producer = nodeNotExist;
    auto producerTell = overloaded(
        [](const lexer::tokenUnit *ptoken){
            return;
        },
        [&producer](producerType proder){
            producer = proder;
        }
    );

    std::visit(producerTell,pRoot->data());
    if( producer != nodeNotExist ){
        chainGuide = producer;
    }
    else{
        return;
    }

    auto iter = pRoot->children().rbegin();
    std::stack<std::pair<pcSyntaxTree,decltype(iter)>> traversalStk;

    pSyntaxTree pNext;
    while (pRoot != nullptr) {
        pNext.reset();
        auto endmark = pRoot->children().rend();
        for( iter =  pRoot->children().rbegin();
             iter < endmark;
             ++iter){
        //for( auto pChild : pRoot->children() ){
            auto pChild = *iter;
            producer = nodeNotExist;
            std::visit(producerTell,pChild->data());

            if( producer == chainGuide ){
                pNext = pChild;
                traversalStk.push(std::make_pair(pRoot,iter+1));
                break;
            }
            else{
                if(func(pChild,producer)){
                    return;
                }
            }
        }
        pRoot = pNext;
    }

    while ( !traversalStk.empty() ) {
        std::tie(pRoot,iter) = toppop(traversalStk);
        auto endmark = pRoot->children().rend();
        for( ;
             iter < endmark;
             ++iter){
            auto pChild = *iter;
            producer = nodeNotExist;
            std::visit(producerTell,pChild->data());

            if( producer == chainGuide ){
                pNext = pChild;
                traversalStk.push(std::make_pair(pRoot,iter+1));
                break;
            }
            else{
                if(func(pChild,producer)){
                    return;
                }
            }
        }
    }
}
*/

void chainSyntaxVisit(pcSyntaxTree pRoot, std::function<int (pcSyntaxTree,nodeType)> func)
{
    nodeType chainGuide;
    nodeType producer = nodeNotExist;
    auto producerTell = overloaded(
        [](const lexer::tokenUnit *ptoken){
            return;
        },
        [&producer](producerType proder){
            producer = proder;
        }
    );

    std::visit(producerTell,pRoot->data());
    if( producer != nodeNotExist ){
        chainGuide = producer;
    }
    else{
        return;
    }

    auto iter = pRoot->children().rbegin();
    auto endmark = pRoot->children().rend();
    std::stack<std::pair<pcSyntaxTree,decltype(iter)>> traversalStk;
    traversalStk.push(std::pair(pRoot,iter));

    while ( !traversalStk.empty() ) {
        std::tie(pRoot,iter) = toppop(traversalStk);
        endmark = pRoot->children().rend();
        for( ;
             iter < endmark;
             ++iter){
            auto pChild = *iter;
            producer = nodeNotExist;
            std::visit(producerTell,pChild->data());

            if( producer == chainGuide ){
                traversalStk.push(std::make_pair(pRoot,iter+1));
                traversalStk.push(std::make_pair(pChild,pChild->children().rbegin()));
                break;
            }
            else{
                if(func(pChild,producer)){
                    return;
                }
            }
        }
    }
}

