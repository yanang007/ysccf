#include "FAs.h"

#include "FA.cpp"

template class FA<nodevecType,isEps>; //nfa
template
ostreamType& operator<< <nodevecType,isEps>(
    ostreamType& os,
    const FA<nodevecType,isEps>& nfa
    );

template class FA<DFATerminalType,alwaysFalse>; //dfa
template
ostreamType& operator<< <DFATerminalType,alwaysFalse>(
        ostreamType& os,
        const FA<DFATerminalType,alwaysFalse>& nfa
        );

DFA::DFA()
{

}

DFA::~DFA()
{

}

nodeType DFA::walk(nodeType node, charType path) const
{
    auto folk = followers(node,path);
    if ( !folk.empty() )
    {
        return folk.top();
    }
    return nodeNotExist;
}

size_t DFA::match(stringType str) const
{
    return match(str.begin(),str.end());
}

size_t DFA::match(stringType::const_iterator iter,
                  stringType::const_iterator end) const
{
    bool isMatched = false;
    size_t lastMatchedSize = 0;

    size_t currentSize = 0;
    nodeType current_node = start();

    while ( iter < end ) {
        auto c = *iter;
        current_node = walk(current_node,c);

        if( current_node == nodeNotExist ){
            break;
        }

        ++currentSize;

        if( isTerminal(current_node) ){
            isMatched = true;
            lastMatchedSize = currentSize;
        }

        ++iter;
    }

    return lastMatchedSize;
}

NFA::NFA()
{

}

NFA::~NFA()
{

}

nodePairType NFA::embrace(const DFA &dfa)
{
    return nodePairType();
}

nodePairType NFA::embrace(const NFA &nfa)
{
    return nodePairType();
}
