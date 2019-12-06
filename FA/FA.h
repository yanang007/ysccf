#ifndef FA_H
#define FA_H

#include <vector>
#include <map>
#include <set>

#include "../base/base.h"

/*
 * FA为一个有穷自动机的类型容器
 * 模板参数terminalType为末端容器类型，容纳某节点经过某条路径后的后继节点
 * 若使用变长容器包装则可以容纳多个节点，会成为 NFA
 * 若使用单元素容器作为末端类型则代表终点确定，会成为 DFA
 */
typedef bool hiddenDetect(char);
template <typename terminalType,hiddenDetect isHidden>
class FA
{
public:
    typedef terminalType followersType;
    typedef std::map<charType,followersType> pathsType;
    typedef std::vector<pathsType> graphType;

    static const pathsType voidPathsType;
    static const followersType voidFollowersType;

public:
   FA();
   ~FA();

   nodeType addNode(); //新增节点，返回节点标号，从0开始d=1递增
   void addPath(nodeType from, nodeType to,charType path); //新增路径
   void addPathAnyway(size_t from, size_t to, charType path); // 强制增加路径，不考虑终点是否存在
   bool removePath(nodeType from, nodeType to,charType path); //移除路径,返回是否删除成功

   bool swapNode(nodeType n1,nodeType n2); //将两个节点的所有属性（入边出边）交换

   sizeVecType countIngoingEdges(const nodevecType& nodes) const; //统计对应节点的入边数
   sizeVecType countOutgoingEdges(const nodevecType& nodes) const; //统计对应节点的出边数
   std::pair<sizeVecType,sizeVecType>
        countInOutgoingEdges(const nodevecType& nodes) const;
        //统计对应节点的入边（保有于first）与出边数（保有于second）
   bool isTerminal(nodeType node) const;

   void markStart(nodeType node); //标记节点为起点，若多次调用后者会覆盖之前设置的起点
   void markTerminal(nodeType node); //标记节点为终点，可有多个终点
   bool removeTerminal(nodeType node); //取消节点的终点标记，返回是否取消成功

   const nodeType& start() const { return _start; } //返回当前FA中的起始节点
   size_t size() const { return nodes().size(); } //返回当前FA中的起始节点
   bool empty() const { return size() == 0; } //返回当前FA是否为空

   const charsetType& charset() const { return _chars; } //返回当前FA中的所有可能字符（不包含空字符eps）
   const nodesetType& terminals() const { return _terminals; } //返回当前FA中的所有终止节点集合
   const nodesetType starts() const { return nodesetType({start()}); } //返回当前FA中的起始节点集合（大小必定为1）（start封装便捷函数）
   const nodesetType overviewAsSet() const;
   const nodevecType overviewAsVector() const;
   const charsetType launchers() const;

   const graphType& nodes() const{ return graph; } //返回当前FA中的节点集合，即图容器本身
   const pathsType& paths(size_t node) const; //返回当前节点的可行路径集合，即路径字符到终点容器的映射
   const followersType& followers(size_t node,charType path) const; //返回当前节点的指定路径的后继节点集合

protected:
   graphType graph;
   charsetType _chars;
   size_t _start;
   nodesetType _terminals;
};

template <typename terminalType,hiddenDetect isHidden>
ostreamType& operator<<(ostreamType& os, const FA<terminalType,isHidden>& nfa);

#endif // FA_H
