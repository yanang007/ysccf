#ifndef FAS_H
#define FAS_H

#include <array>

#include "FA.h"
#include "../utils/utils.hpp"

template<typename _Tp>
struct SingleElementContainer; //helper for dfa's terminal type
typedef SingleElementContainer<nodeType> DFATerminalType;

extern template class FA<DFATerminalType,alwaysFalse>;
extern template
ostreamType& operator<< <DFATerminalType,alwaysFalse>(
    ostreamType& os,
    const FA<DFATerminalType,alwaysFalse>& nfa
    );

/**
 * @brief
 *
 */
class DFA : public FA<DFATerminalType,alwaysFalse>
{
public:
    DFA();
    ~DFA();

    /**
     * @brief
     *
     * @param node
     * @param path
     * @return nodeType
     */
    nodeType walk(nodeType node, charType path) const;

    /**
     * @brief
     *
     * @param stringType::const_iterator
     * @param stringType::const_iterator
     * @return size_t
     */
    size_t match(stringType c) const;

    /**
     * @brief
     *
     * @param stringType::const_iterator
     * @param stringType::const_iterator
     * @return size_t
     */
    size_t match(stringType::const_iterator,stringType::const_iterator) const;
};

//typedef FA<nodevecType,isEps> NFA;

extern template class FA<nodevecType,isEps>;
extern template
ostreamType& operator<< <nodevecType,isEps>(
    ostreamType& os,
    const FA<nodevecType,isEps>& nfa
    );

class NFA : public FA<nodevecType,isEps>
{
public:
    NFA();
    ~NFA();

    /**
     * @brief
     * embrace a certain dfa into *this* nfa
     * @param dfa the dfa to be embraced
     * @return nodePairType the front and terminal of the dfa copy in nfa
     * @warning not implemented yet
     * @deprecated is not used yet
     */
    nodePairType embrace(const DFA& dfa);
    nodePairType embrace(const NFA& dfa);
};

template<typename _Tp>
struct SingleElementContainer
{
    typedef SingleElementContainer<_Tp> _type;
    std::array<_Tp,1> container;
    bool isEmpty = true;

    operator _Tp&() { return const_cast<_Tp&>(const_cast<const _type*>(this)->operator const _Tp&()); }
    operator const _Tp&() const { return top(); }

    void push(const _Tp& val){ container[0] = val; isEmpty = false; }
    void push_back(const _Tp& val){ push(val); }
    void erase(typename decltype(container)::iterator){ isEmpty = true; }
    void erase(const _Tp&){ isEmpty = true; }

    _Tp& top() { return const_cast<_Tp&>(const_cast<const _type*>(this)->top()); }
    const _Tp& top() const { auto tmp = end(); --tmp; return *tmp; }
    _Tp& back() { return top(); }
    const _Tp& back() const { return top(); }
    _Tp& front() { return top(); }
    const _Tp& front() const { return top(); }
    bool empty() const { return isEmpty; }
    auto begin() noexcept { return container.begin(); }
    auto begin() const noexcept { return container.begin(); }
    auto end() noexcept { return empty() ? container.begin() : container.end(); }
    auto end() const noexcept { return empty() ? container.begin() : container.end(); }
};

#endif
