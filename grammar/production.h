#ifndef PRODUCTION_H
#define PRODUCTION_H

#include "../base/base.h"
#include "../lexer/lexer.h"

class nameTable;

class production
{
public:
    inline static const stringType sBeforeSymbol = L"<";
    inline static const stringType sAfterSymbol = L">";
    inline static const stringType sBeforeToken = L"";
    inline static const stringType sAfterToken = L"";

    enum idType{
        itSymbol, //非终结符
        itToken,  //终结符
        itReduced,//已归约
        itEmpty,
    };

    //using producedType = std::pair<nodeType,idType>;
    class producedType : public std::pair<nodeType,idType>{
    public:
        using base = std::pair<nodeType,idType>;
        producedType(nodeType n,idType t) : base(n,t){}
        bool isSymbol() const { return second == itSymbol; }
        bool isToken() const { return second == itToken; }
        bool isReduced() const { return second == itReduced; }
        bool isEmpty() const { return second == itEmpty; }

        operator nodeType() const
        {
            return first;
        }
    };

    using producedVecType = std::vector<producedType>;

    using iterator = typename producedVecType::iterator;
    using const_iterator = typename producedVecType::const_iterator;

    inline static const producedType reducedEnd = producedType(nodeNotExist,production::itReduced);

public:
    production();
    ~production();

    production& symbol(nodeType symbol) { return appendSymbol(symbol); }
    production& token(lexer::tokenID token) { return appendToken(token); }
    production& appendSymbol(nodeType symbol) { return appendv(symbol,itSymbol); }
    production& appendToken(lexer::tokenID token) { return appendv(token,itToken); }
    production& appendv(nodeType n,idType t){ return appendv(genV(n,t)); }
    production& appendv(producedType p) {
        _produced.push_back(p);
        return *this;
    }

    producedType genSymbol(nodeType symbol){ return genV(symbol,itSymbol); }
    producedType genToken(nodeType token){ return genV(token,itToken); }
    producedType genV(nodeType n,idType t){ return producedType(n,t); }

    auto begin() const { return _produced.begin(); }
    auto end() const { return _produced.end(); }
    auto begin() { return _produced.begin(); }
    auto end() { return _produced.end(); }

    auto rbegin() const { return _produced.rbegin(); }
    auto rend() const { return _produced.rend(); }
    auto rbegin() { return _produced.rbegin(); }
    auto rend() { return _produced.rend(); }

    auto size() const { return _produced.size(); }
    auto empty() const { return size() == 0; }

    auto back() const { return _produced.back(); }
    auto front() const { return _produced.front(); }

    void clear() { _produced.clear(); }

    ostreamType& toStream(ostreamType& os, const nameTable& symbolTable, const nameTable& tokenTable) const;

private:
    producedVecType _produced;
};


ostreamType& operator<<(ostreamType& os, const production& prod);

#endif // PRODUCTION_H
