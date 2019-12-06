#ifndef LRTABLE_H
#define LRTABLE_H

#include <map>

#include "../base/base.h"

#include "../lexer/lexer.h"
#include "../grammar/grammar.h"

class LRTable
{
public:
    enum actionTypeEnum{
        ateShift = 0,
        ateReduce,
        ateAccept,
    };
    inline const static stringType signs[] = {
        L"S",L"r",L"acc"
    };
    inline const static lexer::tokenID allRoads = lexer::fin - 1;
    //conflicts may occur when states amount are fairly huge

    using rowNoType = nodeType;
    struct actionType{
        nodeType dest;
        actionTypeEnum type;
    };

    //using actionPair = std::pair<nodeType,actionTypeEnum>;
    using actionMap = std::map<lexer::tokenID,actionType>;
    using gotoMap = std::map<producerType,rowNoType>;

public:
    LRTable();

    void resize(size_t size){ actions.resize(size);gotos.resize(size); }
    void clear(){ actions.clear();gotos.clear(); }
    size_t size() const { return actions.size(); }

    rowNoType newRow();

    void setStart(rowNoType row) { _start = row; }
    rowNoType start() const { return _start; }

    void setGoto(rowNoType r1, producerType p,rowNoType r2){
        gotos[r1][p] = r2;
    }
    void setAction(rowNoType r1, lexer::tokenID token, nodeType target,actionTypeEnum behavior){
        /*if(token == allRoads)
            actions[r1].clear();*/
        actions[r1][token] = actionType{target,behavior};
    }
    /*
    void set(rowNoType r1, production::producedType prodcd, rowNoType r2){
        if(prodcd.second == production::itToken){
            setAction(r1,prodcd.first,)
        }
    }*/
    //nodeType can be rowNoType or productionID
    //depending on arg3 - actionTypeEnum

    const auto& getGoto(rowNoType r,producerType p) const{
        return gotos[r].at(p);
    }
    const auto& getAction(rowNoType r,lexer::tokenID t) const{
        //if( actions[r].size() == 1 ){
            try {
                return actions[r].at(t);
            } catch (std::out_of_range e) {
                return actions[r].at(allRoads);
                //return actions[r].at(t);
            }
        //}
        //return actions[r].at(t);
    }

    ostreamType& toStream(ostreamType& os, const nameTable& symbolTable, const nameTable& tokenTable) const;

protected:
    rowNoType _start = 0;
    std::vector<actionMap> actions;
    std::vector<gotoMap> gotos;
};

ostreamType& operator<<(ostreamType& os, const LRTable& lrtbl);

#endif // LRTABLE_H
