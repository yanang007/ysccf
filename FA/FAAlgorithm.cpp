#include "FAAlgorithm.h"

size_t _find_belong(nodeType node, const std::vector<nodesetType> &statesSet)
{
    size_t i = notFound;
    if( node != nodeNotExist ){
        i = 0;
        for ( const auto& state : statesSet )
        {
            //if(ufind(state,node) != state.end()){
            if( contains(state,node) ){
                return i;
            }
            ++i;
        }
        i = notFound;
    }
    return i;
}

nodevecType _move_determined(nodevecType state, const DFA &dfa, charType road, nodeSelector selector)
{
    nodevecType vec;
    vec.reserve(state.size());
    for( auto node : state )
    {
        auto dest = dfa.walk(node,road);
        if ( !selector(dest) )
            dest = nodeNotExist;
        vec.push_back(dest);
    }
    return vec;
}
