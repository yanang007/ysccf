#ifndef LR0ITEM_H
#define LR0ITEM_H

#include "../grammar/production.h"
#include "../grammar/grammar.h"

class grammar;
class LR0Item;
using LR0ItemSet = std::set<LR0Item>;

LR0ItemSet closure(const LR0ItemSet&, const grammar&);
LR0ItemSet go(const LR0ItemSet&,production::producedType);

class LR0Item
{
public:
    LR0Item(nodeType producer,
            production::const_iterator iter,
            production::const_iterator endmark,
            const grammar::productionID prod)
        :_producer(producer),iter(iter),endmark(endmark),prod(prod)
    {}
    LR0Item(const grammar& grmr,
            grammar::productionID id)
        :LR0Item(grmr.getHandle(id),id,grmr.productionAt(id))
    {}
    //~LR0Item(){}

    bool atEnd() const { return iter == endmark; }
    auto producer() const { return _producer; }
    auto production() const { return prod; }
    inline auto get() const;

    inline auto advance() const;
    inline auto getIter() const { return iter; }
    inline auto getEnd() const { return endmark; }
    void reRef(const grammar&);

    inline bool operator<(const LR0Item& rhv) const;
    inline bool operator==(const LR0Item& rhv) const;

protected:
    LR0Item(productionHandle hnd,
            grammar::productionID id,
            const class production& prod)
        :LR0Item(hnd.producer,prod.begin(),prod.end(),id)
    {}

    nodeType _producer;
    production::const_iterator iter;
    production::const_iterator endmark;
    grammar::productionID prod;
};

inline
auto LR0Item::get() const{
    if(atEnd()){
        return production::reducedEnd;
    }
    else{
        return *iter;
    }
}

inline
auto LR0Item::advance() const{
    auto ret = *this;
    if(atEnd()){
        //do nothing
    }
    else{
        ++ret.iter;
    }
    return ret;
}

inline
bool LR0Item::operator< (const LR0Item &rhv) const
{
    bool ret;
    if( this->producer() < rhv.producer() ){
        ret = true;
    }
    else if( this->producer() == rhv.producer() ){
        if( this->prod < rhv.prod ){
            ret = true;
        }
        else if( this->prod == rhv.prod ){
            ret = this->iter < rhv.iter;
        }
        else{
            ret = false;
        }
    }
    else{
        ret = false;
    }
    return ret;
}

bool LR0Item::operator==(const LR0Item &rhv) const
{
    return this->producer() == rhv.producer() &&
            this->prod == rhv.prod &&
            this->iter == rhv.iter;
}


#endif // LR0ITEM_H
