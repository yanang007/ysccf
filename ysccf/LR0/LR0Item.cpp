#include "LR0Item.h"
#include "../grammar/grammar.h"

#include "../utils/utils.hpp"

LR0ItemSet closure(const LR0ItemSet& items, const grammar& gramr)
{
    //LR0ItemSet ret = items;

    nodesetType producers;
    nodesetType producerDiff;
    for( const auto& item :  items ){
        //producers.insert(item.producer());
        const auto& produced = item.get();
        if( produced.second == production::itSymbol ) {
            producerDiff.insert(produced.first);
        }
    }
    //producerDiff = _difference(producerDiff,producers);

    auto _items = items;
    decltype(producerDiff) newProducers;

    while(!producerDiff.empty()){
        //_items.insert(diff.begin(),diff.end());
        producers.insert(producerDiff.begin(),producerDiff.end());
        for ( const auto& producer : producerDiff ){
            //for ( const auto& production : gramr.productions(producer) ){
            for ( size_t prodPos = 0; prodPos<gramr.productions(producer).size(); ++prodPos ){
                auto itemToInsert = LR0Item(gramr,gramr.getID(producer,prodPos));

                auto produced = itemToInsert.get();
                if( produced.second == production::itSymbol ){
                    newProducers.insert(produced.first);
                }

                _items.insert(std::move(itemToInsert));
            }
        }

        producerDiff.clear();
        producerDiff = _difference(newProducers,producers);
    }
    return _items;
}

void LR0Item::reRef(const grammar & gram)
{
    auto ret = gram.productionAt(this->production());
    iter = ret.begin();
    endmark = ret.end();
}
