#ifndef NAMETABLE_H
#define NAMETABLE_H

#include "../base/base.h"
#include "bijection.hpp"

extern template class bijection<stringType,nodeType>;

class nameTable : public bijection<stringType,nodeType>
{
    typedef stringType nameType;
public:
    nameTable();
    ~nameTable();

    nodeType ref(nameType n) const;
    nameType ref(nodeType n) const;
};

#endif // NAMETABLE_H
