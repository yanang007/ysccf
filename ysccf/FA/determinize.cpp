#include "determinize.h"
#include "FAAlgorithm.h"
#include "../utils/utils.hpp"

DFA determinize(const NFA& nfa, std::vector<std::vector<nodesetType>>* pMidResult)
{
    std::vector<nodesetType> statesList;
    auto states = nfa.starts();
    states = _eps_closure(states,nfa);

    statesList.push_back(states);

    size_t i = 0;
    std::vector<nodesetType> newStatesSet;
    newStatesSet.reserve(nfa.charset().size()); //reserve the size ahead to avoid reallocation
    std::vector<decltype(newStatesSet)> transferSet;
    transferSet.reserve(nfa.size()); //dfa'size is estimated to be the same or less

    while(i != statesList.size())
    {
        states = statesList[i];
        newStatesSet.clear();
        for( auto ch :  nfa.charset()){
            newStatesSet.push_back(_eps_closure_move(states,nfa,ch));
        }
        for( const auto& sts : newStatesSet)
        {
            //if(!sts.empty() && std::find(statesList.begin(),statesList.end(),sts) == statesList.end())
            if(!sts.empty() && !contains(statesList,sts))
            {
                statesList.push_back(sts);
            }
        }
        transferSet.push_back(newStatesSet);
        /*
        for( auto& i : newStatesSet){
            for( auto& j : i){
                std::cout<<j<<" ";
            }
            std::cout<<";";
        }
        std::cout<<std::endl;
        for( auto& i : transferSet){
            for( auto& j : i){
                std::cout<<"(";
                for( auto& k : j){
                    std::cout<<k<<" ";
                }
                std::cout<<")";
            }
            std::cout<<";";
        }
        std::cout<<std::endl;
        printContainer(transferSet);
        std::cout<<std::endl;
        */
        ++i;
    }

    DFA dfa;

    nodevecType temp;
    auto amount = transferSet.size();
    temp.reserve(amount);
    std::generate_n(std::back_inserter(temp), amount, [&dfa](){return dfa.addNode();});

    auto itag = temp.begin();
    for( auto& node_tuple : transferSet){
        auto tag = *itag;
        auto ic = nfa.charset().begin();
        for( auto& road : node_tuple){
            auto idest = std::find(statesList.begin(),statesList.end(),road);
            if( idest != statesList.end() ){
                auto dest = std::distance(statesList.begin(),idest);
                dfa.addPath(tag,dest,*ic);
            }
            ++ic;
        }
        ++itag;
    }

    auto terminals = nfa.terminals();
    for(size_t i = 0; i < statesList.size(); ++i){
        if(!_intersect(statesList[i],terminals).empty())
        {
            dfa.markTerminal(i);
        }
    }

    if(pMidResult != nullptr){
        *pMidResult = std::move(transferSet);
    }

    return dfa;
}
