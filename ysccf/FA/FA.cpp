#include <vector>
#include <map>
#include <set>
#include <string>
#include <iterator>
#include <ostream>
#include <algorithm>

#include "FA.h"
#include "../utils/utils.hpp"

template <typename TT,hiddenDetect IH>
const typename FA<TT,IH>::pathsType
    FA<TT,IH>::voidPathsType = FA::pathsType();

template <typename TT,hiddenDetect IH>
const typename FA<TT,IH>::followersType
    FA<TT,IH>::voidFollowersType = FA::followersType();

template <typename TT,hiddenDetect IH>
std::string nodeString(const FA<TT,IH>& nfa, nodeType node) //deprecated
{
    /*
    std::string str = std::to_string(node);
    auto terms = nfa.terminals();
    if(node == nfa.start())
    {
        str = "[" + str + "]";
    }
    else if(std::find(terms.begin(),terms.end(),node) != terms.end())
    {
        str = "(" + str + ")";
    }
    else{
        str = " " + str + " ";
    }
    return str;
    */
}

inline
stringType pathString(charType path)
{
    if ( path == eps ) {
        return L"ε";
    }
    else {
        return stringType(1,path);
    }
}

template <typename TT,hiddenDetect IH>
ostreamType& operator<<(ostreamType& os, const FA<TT,IH>& nfa)
{
    size_t i = 0;
    for(auto &node : nfa.nodes())
    {
        for(auto &road : node)
        {
            os<<i<<" -"<<pathString(road.first)<<"-> ";
            for( auto& dest : road.second){
                os<<dest<<" ";
            }
            os<<std::endl;
        }
        ++i;
    }
    os<<"start: "<<nfa.start()<<std::endl;
    os<<"terminals: ";
    for(auto &term : nfa.terminals())
    {
        os<<term<<" ";
    }
    os<<std::endl;
    return os;
}

template <typename TT,hiddenDetect IH>
FA<TT,IH>::FA()
{
    _start = 0;
}

template <typename TT,hiddenDetect IH>
FA<TT,IH>::~FA()
{

}

template<typename TT, hiddenDetect IH>
const nodesetType FA<TT,IH>::overviewAsSet() const
{
    nodesetType ret;
    std::generate_n(
        std::inserter(ret,ret.begin()),
        size(),
        [i = 0]() mutable { return i++; }
    );
    return ret;
}

template<typename TT, hiddenDetect IH>
const nodevecType FA<TT,IH>::overviewAsVector() const
{
    nodevecType ret;
    std::generate_n(
        std::inserter(ret,ret.begin()),
        size(),
        [i = 0]() mutable { return i++; }
    );
    return ret;
}

template<typename TT, hiddenDetect IH>
const charsetType FA<TT,IH>::launchers() const
{
    //not verified
    charsetType ret;
    auto mapping = paths(start());

    std::transform(
        mapping.begin(),
        mapping.end(),
        std::inserter(ret,ret.begin()),
        [](typename decltype(mapping)::value_type& kv){
            return kv.first;
        }
    );
    return ret;
}


template <typename TT,hiddenDetect IH>
size_t FA<TT,IH>::addNode()
{
    auto ret = size();
    graph.push_back(pathsType());
    return ret;
}

template <typename TT,hiddenDetect IH>
void FA<TT,IH>::addPath(size_t from, size_t to, charType path)
{
    if ( to >= size() ){
        return; /// todo: exception handle
    }
    addPathAnyway(from,to,path);
}

template <typename TT,hiddenDetect IH>
void FA<TT,IH>::addPathAnyway(size_t from, size_t to, charType path)
{
    graph[from][path].push_back(to);
    if( !IH(path) )  //is hidden character?  etc. null char epsilon
        _chars.insert(path);
}

template <typename TT,hiddenDetect IH>
bool FA<TT,IH>::removePath(size_t from, size_t to, charType path)
{
    try {
        auto& folks = graph[from].at(path);

        if ( folks.empty() )
        {
            return false;
        }
        auto it = std::find(folks.begin(),folks.end(),to);
        if(it != folks.end()){
            folks.erase(it);
            if(folks.empty()){
                graph[from].erase(path);
            }
            return true;
        }
        else{
            return false;
        }
    } catch (std::out_of_range e) {
        return false;
    }
}

template <typename TT,hiddenDetect IH>
bool FA<TT,IH>::swapNode(nodeType n1, nodeType n2)
{
    if( n1 > size() || n2 > size() ){
        return false;
    }
    for(auto &node : graph) {
        for(auto &road : node) {
            for( auto& dest : road.second){
                if( dest == n2 ){
                    dest = n1;
                }
                else if( dest == n1 ){
                    dest = n2;
                }
            }
        }
    }
    std::swap(graph[n1],graph[n2]);
    return true;
}

template <typename TT,hiddenDetect IH>
sizeVecType FA<TT,IH>::countIngoingEdges(const nodevecType &nodes) const
{
    int i = 0;
    sizeVecType ret(nodes.size(),0);
    for(auto &node : graph) {
        for(auto &road : node) {
            for( auto& dest : road.second){
                ufindAllExec(nodes,dest,
                        [&ret,&nodes](decltype(end(nodes)) iter){
                            ret[iter - begin(nodes)]++;
                        }
                );
            }
        }
    }

    return ret;
}

template <typename TT,hiddenDetect IH>
sizeVecType FA<TT,IH>::countOutgoingEdges(const nodevecType &nodes) const
{
    sizeVecType ret;
    ret.reserve(nodes.size());
    for( const auto& node : nodes ){
        ret.push_back(paths(node).size());
    }
    return ret;
}

template <typename TT,hiddenDetect IH>
std::pair<sizeVecType,sizeVecType>
    FA<TT,IH>::countInOutgoingEdges(const nodevecType &nodes) const
{
    return std::make_pair(countIngoingEdges(nodes),countOutgoingEdges(nodes));
}

template <typename TT,hiddenDetect IH>
bool FA<TT,IH>::isTerminal(nodeType node) const
{
    return contains(terminals(),node);
}

template <typename TT,hiddenDetect IH>
void FA<TT,IH>::markStart(size_t node)
{
    _start = node;
}

template <typename TT,hiddenDetect IH>
void FA<TT,IH>::markTerminal(size_t node)
{
    _terminals.insert(node);
}

template <typename TT,hiddenDetect IH>
bool FA<TT,IH>::removeTerminal(size_t node)
{
    return _terminals.erase(node) > 0;
}

template <typename TT,hiddenDetect IH>
const typename FA<TT,IH>::pathsType& FA<TT,IH>::paths(size_t node) const
{
    try {
        return graph.at(node);
    } catch (std::out_of_range e) {
        return voidPathsType;
    }
}

template <typename TT,hiddenDetect IH>
const typename FA<TT,IH>::followersType& FA<TT,IH>::followers(size_t node, charType path) const
{
    try {
        return paths(node).at(path);
    } catch (std::out_of_range e) {
        return voidFollowersType;
    }
}
