#include <set>
#include <list>

#include "grammarAlgos.h"
#include "../utils/containerUtil.hpp"

std::pair<
    std::map<nodeType, std::set<lexer::tokenID> >,
    std::map<nodeType, std::set<lexer::tokenID> > >
    firstNfollow(const grammar & grmr)
{
    std::map<nodeType, std::set<lexer::tokenID>> first,follow;
    std::list<std::list<production::producedType>> tree;

    return std::pair(first,follow);
}

///todo : in progress
std::set<nodeType> nullableVnSet(const grammar & grmr)
{
    std::set<nodeType> nullables;
    std::set<std::reference_wrapper<production>> rProds;
    for( const auto& [producer,prods] : enumerate(grmr.deductions())){
        for( const auto& prod : prods){

            auto iter = prod.begin(),
                 ender = prod.end();
            while ( iter != ender &&
                    iter->second == production::itSymbol) {
                if( !contains(nullables,iter->first) ){
                    break;
                }
                ++iter;
            }

            if( iter == ender ){
                nullables.insert(producer);
            }
        }
    }

    return nullables;
}
