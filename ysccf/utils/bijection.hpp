#ifndef BIJECTION_H
#define BIJECTION_H

#include <map>

template <typename _Tx,typename _Ty>
class bijection
{
public:
    typedef std::map<_Tx,_Ty> xToyMap;
    typedef std::map<_Ty,_Tx> yToxMap;
public:
    bijection(){}
    ~bijection(){}

    void insert(const _Tx& x, const _Ty& y){
        _xToy[x] = y;
        _yTox[y] = x;
    }

    _Tx& at(_Ty n) { return _yTox.at(n); }
    const _Tx& at(_Ty n) const { return _yTox.at(n); }
    _Ty& at(_Tx n)  { return _xToy.at(n); }
    const _Ty& at(_Tx n) const { return _xToy.at(n); }

    _Tx& operator[](_Ty n) { return _yTox[n]; }
    _Ty& operator[](_Tx n) { return _xToy[n]; }

    const xToyMap& xToy() const { return _xToy; }
    const yToxMap& yTox() const { return _yTox; }

    size_t size() const { return _yTox.size(); } //返回当前grammar中的起始节点
    bool empty() const { return size() == 0; } //返回当前grammar是否为空

    void clear() { _xToy.clear();_yTox.clear(); }

private:
    xToyMap _xToy;
    yToxMap _yTox;
};

/*
 * typedef stringType nameType;
    typedef std::map<nameType,nodeType> nameToIdType;
    typedef std::map<nodeType,nameType> idToNameType;
public:
    nameTable();
    ~nameTable();

    void declare(const nodeType& node, const nameType& name);

    nameType& at(nodeType n) { return idToName.at(n); }
    const nameType& at(nodeType n) const { return idToName.at(n); }
    nodeType& at(nameType n)  { return nameToId.at(n); }
    const nodeType& at(nameType n) const { return nameToId.at(n); }

    nameType& operator[](nodeType n) { return idToName[n]; }
    nodeType& operator[](nameType n) { return nameToId[n]; }

    nodeType ref(nameType n) const;
    nameType ref(nodeType n) const;


    size_t size() const { return idToName.size(); } //返回当前grammar中的起始节点
    bool empty() const { return size() == 0; } //返回当前grammar是否为空

private:
    nameToIdType nameToId;
    idToNameType idToName;
 *
 */

#endif // BIJECTION_H
