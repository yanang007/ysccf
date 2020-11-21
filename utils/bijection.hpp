#ifndef BIJECTION_H
#define BIJECTION_H

#include <map>

template <typename _Tx,typename _Ty>
class bijection
{
public:
    using leftMap = std::map<_Tx,_Ty>;
    using rightMap = std::map<_Ty,_Tx>;

public:
    bijection(){}
    ~bijection(){}

    void insert(const _Tx& x, const _Ty& y){
        left[x] = y;
        right[y] = x;
    }

    _Tx& at(_Ty n) { return right.at(n); }
    const _Tx& at(_Ty n) const { return right.at(n); }
    _Ty& at(_Tx n)  { return left.at(n); }
    const _Ty& at(_Tx n) const { return left.at(n); }

    // 此处假定x与y类别不一致，因此可以写作重载简化调用
    std::enable_if_t<
        !std::is_same_v<_Tx, _Ty>,
        typename leftMap::const_iterator>
        find(_Tx n) const { return left.find(n); }
    std::enable_if_t<
        !std::is_same_v<_Tx, _Ty>, 
        typename rightMap::const_iterator>
        find(_Ty n) const { return right.find(n); }

    std::enable_if_t<!std::is_same_v<_Tx, _Ty>, bool>
        isEnd(typename leftMap::const_iterator& iter) { return left.end() == iter; }
    std::enable_if_t<!std::is_same_v<_Tx, _Ty>, bool>
        isEnd(typename rightMap::const_iterator& iter) { return right.end() == iter; }

    std::enable_if_t<!std::is_same_v<_Tx, _Ty>, _Tx&>
        operator[](_Ty n) { return right[n]; }
    std::enable_if_t<!std::is_same_v<_Tx, _Ty>, _Ty&>
        operator[](_Tx n) { return left[n]; }

    size_t size() const { return right.size(); } //返回当前grammar中的起始节点
    bool empty() const { return size() == 0; } //返回当前grammar是否为空

    void clear() { left.clear();right.clear(); }

public:
    leftMap left;
    rightMap right;
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
