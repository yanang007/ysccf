#ifndef GRAMMARTYPES_H
#define GRAMMARTYPES_H

#include "../base/base.h"
#include "../utils/type_wrapper.hpp"

using symbolID = type_wrapper<nodeType, 818>;
using producerType = symbolID;

struct productionHandle
{
    producerType producer;
    size_t productionPos;
    inline bool operator==(const productionHandle&) const;
    inline bool operator<(const productionHandle&) const;
};

struct producedUnitHandle : public productionHandle
{
    size_t producedPos;
};


#endif // GRAMMARTYPES_H
