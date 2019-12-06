#include <string>

#include "nameTable.h"


template class bijection<stringType,nodeType>;

nameTable::nameTable()
{

}

nameTable::~nameTable()
{

}

nodeType nameTable::ref(nameTable::nameType n) const
{
    try {
        return at(n);
    } catch (std::out_of_range e) {
        return nodeNotExist;
    }
}

nameTable::nameType nameTable::ref(nodeType n) const
{
    try {
        return at(n);
    } catch (std::out_of_range e) {
        return std::to_wstring(n);
    }
}

