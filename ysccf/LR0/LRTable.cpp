#include "LRTable.h"

#include "../utils/nameTable.h"

LRTable::LRTable()
{

}

ostreamType &LRTable::toStream(ostreamType &os, const nameTable &symbolTable, const nameTable &tokenTable) const
{
    for( size_t i = 0; i < size(); ++i ){
        os<<"-------"<<i<<"-------"<<std::endl << "action: ";
        for( const auto& [token,action] : actions[i]){
            const auto& [dest,actType] = action;
            os<<tokenTable.ref(token)
              <<"("
              << signs[(size_t)actType]
              << dest
              <<") ";
        }
        os<<std::endl << "goto: ";
        for( const auto& [producer,dest] : gotos[i]){
            os<<symbolTable.ref(producer)
              <<"("
              << dest
              <<") ";
        }
        os<<std::endl;
    }
    return os;
}

ostreamType &operator<<(ostreamType &os, const LRTable &lrtbl)
{
    nameTable emptytable;
    return lrtbl.toStream(os,emptytable,emptytable);
}
