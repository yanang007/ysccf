#include "minimize.h"

nodesetType scanUnreachable(const DFA &dfa)
{
    /*
    auto available = dfa.overviewAsVector();
    nodeToSizeMapType mapping;
    for ( size_t i = 0; i<available.size(); ++i){
        mapping[available[i]] = i;
    }*/
    nodevecType available;
    nodeToSizeMapType mapping;
    fillNodeMappings(dfa,available,mapping);

    std::vector<nodevecType> dests;

    for ( auto ch : dfa.charset() ){
        dests.push_back(_move_determined(available,dfa,ch));
    } // unreachable route's destiny would be "notExist"

    size_t routes = dests.size();

    auto reachable = dfa.overviewAsSet();
    nodesetType lastReachable;

    do {
        lastReachable = std::move(reachable);
        reachable.clear();
        for( const auto& node : lastReachable ){
            for ( size_t route = 0; route< routes; ++route ){
                reachable.insert(dests[route][mapping[node]]);
            }
        }
    }
    while ( lastReachable != reachable );

    reachable = _union(reachable,dfa.starts());
    return _difference(dfa.overviewAsSet(), reachable);
}

nodesetType scanUseless(const DFA &dfa)
{
    bool isNewTerminalRelatedNodeFound;

    nodevecType available;
    nodeToSizeMapType mapping;
    fillNodeMappings(dfa,available,mapping);

    std::vector<bool>
            termRelTagged(dfa.size(),false);
    //abbr. terminalRelatedTagged

    auto terminalUnion = dfa.terminals();
    for ( const auto& terminal : terminalUnion){
        termRelTagged[mapping[terminal]] = true;
    }

    do {
        isNewTerminalRelatedNodeFound = false;
        for ( const auto& node : available){
            if ( termRelTagged[mapping[node]] ) {
                continue;
            }
            for( const auto& route : dfa.paths(node) )
            {
                // DFA assures that
                // "notExist" will be erased
                // in a legally generated graph
                if ( termRelTagged[mapping[route.second]] )
                {
                    terminalUnion.insert(node);
                    termRelTagged[mapping[node]] = true;
                    isNewTerminalRelatedNodeFound = true;
                }
            }
        }
    }
    while ( isNewTerminalRelatedNodeFound );

    return _difference(dfa.overviewAsSet(), terminalUnion);
}

DFA minimize(const DFA &dfa, std::vector<nodesetType>* pMidResult)
{
    if(dfa.empty()){
        return dfa;
    }

    auto ich = dfa.charset().begin();
    if(ich == dfa.charset().end()){
        std::wcout<<L"minimize() :imcomplete dfa - charset is empty\n";
        return dfa;
    }
    //size_t size = dfa.size();
    bool finished = true;

    auto overview = dfa.overviewAsSet(),
            terminals = dfa.terminals();

    if(terminals.empty()){
        //error incomplete dfa
        std::wcout<<L"minimize() :imcomplete dfa - no terminals\n";
        return dfa;
    }
    auto redundantStates = scanRedundantStates(dfa);

    nodevecType available;
    nodeToSizeMapType mapping;
    fillNodeMappings(dfa,available,mapping);

    std::vector<bool>
            isRedundant(available.size(),false);

    for ( const auto& redundantState : redundantStates){
        isRedundant[mapping[redundantState]] = true;
    }

    std::vector<nodesetType>
            statesSet({ _difference(overview,terminals), terminals }),
            newStatesSet,
            temp;

    for ( auto& states : statesSet)
    {
        states = _difference(states,redundantStates);
    }

    while (true)
    {
        for (const auto& state : statesSet)
        {
            size_t amount = statesSet.size() + 1; // existing states plus deadend
            temp.assign(amount,nodesetType());
            /*
            temp.clear();
            temp.reserve(amount);
            std::generate_n(std::back_inserter(temp), amount, [](){return nodesetType();});
            */
            for (const auto& node : state){
                auto follower = dfa.walk(node,*ich);
                follower = filterNode(
                            follower,
                            [&isRedundant,&mapping](nodeType n){ return !isRedundant[mapping[n]]; }
                );
                if ( isRedundant[mapping[follower]] ){
                    follower = nodeNotExist;
                }
                auto belonging = _find_belong(dfa.walk(node,*ich),statesSet);
                if (belonging != notFound)
                    temp[belonging].insert(node);
                else
                    temp.back().insert(node);
            }
            for (const auto& group : temp){
                if ( !group.empty() )
                    newStatesSet.push_back(group);
            }
        }

        ++ich;
        if (!newStatesSet.empty() &&
                statesSet != newStatesSet)
            finished = false;
        if (ich == dfa.charset().end())
        {
            ich = dfa.charset().begin();
            if (finished)
                break;
            finished = true;
        }
        statesSet = newStatesSet;
        newStatesSet.clear();
        //printContainer(statesSet);
    }

    std::sort(statesSet.begin(),statesSet.end());
    //printContainer(statesSet);

    nodevecType newDFATags;
    DFA mindfa;
    std::generate_n(std::back_inserter(newDFATags), statesSet.size(), [&mindfa](){return mindfa.addNode();});
    auto itag = newDFATags.begin();

    for ( const auto& stateUnion : statesSet ){
        const auto& behalfNode = front(stateUnion);
        const auto& tag = *itag;

        if ( !_intersect(stateUnion,terminals).empty() ){
            mindfa.markTerminal(tag);
        }

        for( const auto& route : dfa.paths(behalfNode) )
        {
            nodeType follower = route.second;
            follower = filterNode(
                        follower,
                        [&isRedundant,&mapping](nodeType n){ return !isRedundant[mapping[n]]; }
            );
            auto belonging = _find_belong(route.second, statesSet);
            if(belonging == notFound) {
                continue;
            }

            mindfa.addPath(tag,newDFATags[belonging],route.first);
        }
        ++itag;
    }

    auto start = dfa.start();
    auto belonging = _find_belong(start, statesSet);
    mindfa.markStart(newDFATags[belonging]);

    if( pMidResult != nullptr ){
        *pMidResult =std::move(statesSet);
    }

    return mindfa;
}
