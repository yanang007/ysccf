#ifndef FAALGORITHM_H
#define FAALGORITHM_H

#include "../base/base.h"
#include "../FA/FAs.h"

template <typename terminalType,hiddenDetect isHidden>
nodesetType _move(
        const nodesetType& state,
        const FA<terminalType,isHidden>& fa,
        charType road )
{
    std::set<size_t> ret;
    for( auto node : state )
    {
        auto folks = fa.followers(node,road);
        ret.insert(folks.begin(),folks.end());
    }
    return ret;
}

template <typename terminalType,hiddenDetect isHidden>
inline
nodesetType _eps_closure_move(
        const nodesetType& state,
        const FA<terminalType,isHidden>& fa,
        charType road )
{
    return _eps_closure(_move(state, fa, road), fa);
}

template <typename terminalType,hiddenDetect isHidden>
nodesetType _eps_closure(
        const nodesetType& state,
        const FA<terminalType,isHidden>& fa )
{
    //modified to prevent redundent initializing of _state
    //probably wrong
    auto diff = state;
    decltype(diff) _state,newstate;
    while(!diff.empty()){
        _state.insert(diff.begin(),diff.end());
        newstate = _move(diff,fa,eps);

        diff.clear();

        std::set_difference(newstate.begin(),newstate.end(),
                            _state.begin(),_state.end(),
                            std::inserter(diff,diff.begin()));
    }
    return _state;
}

typedef bool (*nodeSelector)(nodeType);
nodevecType _move_determined(
        nodevecType state,
        const DFA& dfa,
        charType road,
        nodeSelector selector = alwaysTrue );

size_t _find_belong(nodeType node, const std::vector<nodesetType>& statesSet);

inline
void fillNodeMappings(
        const DFA& dfa,
        nodevecType& available,
        nodeToSizeMapType& mapping )
{
    available = dfa.overviewAsVector();
    for ( size_t i = 0; i<available.size(); ++i){
        mapping[available[i]] = i;
    }
}

template< typename _Fn >
inline
nodeType filterNode(nodeType node, _Fn filter)
{
    return filter(node) ? node : nodeNotExist;
}


#endif
