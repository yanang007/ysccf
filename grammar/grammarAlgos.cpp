#include <set>
#include <list>

#include "grammarAlgos.h"
#include "../utils/containerUtil.hpp"


class dependencyConverter
{
public:
    dependencyConverter(
        std::map<nodeType, std::set<lexer::tokenID>>& theSet,
        const std::vector<std::set<nodeType>>& dependency)
        : theSet(theSet), dependency(dependency)
    { }

    template<typename Func>
    void applyRoots(
        const std::map<nodeType, std::set<nodeType>>& roots,
        Func&& f)
    {
        for (const auto& [root, children] : roots) {
            applyRoot(root, children, f);
        }
    }

    template<typename Func>
    void applyRoot(
        const nodeType root,
        const std::set<nodeType>& children,
        Func&& f)
    {
        visited.clear();
        for (const nodeType child : children) {
            convertDependencyImpl(root, child, f);
        }
    }

    template<typename Func>
    void convertDependencyImpl(
        nodeType root,
        nodeType current,
        Func&& f)
    {
        f(root, theSet[current]);
        visited.insert(current);
        for (const nodeType child : dependency[current]) {
            if (contains(visited, child)) {
                continue;
            }
            convertDependencyImpl(root, child, f);
        }
    }

private:
    std::map<nodeType, std::set<lexer::tokenID>>& theSet;
    const std::vector<std::set<nodeType>>& dependency;
    std::set<nodeType> visited;
};

std::tuple<
    std::map<nodeType, std::set<lexer::tokenID>>,
    std::map<nodeType, std::set<lexer::tokenID>> >
    firstNfollow(const grammar & grmr)
{
    R"kk(
%token a "a"
%token b "b"
%token c "c"
<S> ::= <A><B> | b <C>;
<A> ::= b | null;
<B> ::= a <D> | null;
<C> ::= <A> <D> | b;
<D> ::= a <S> | c;
)kk";

    std::map<nodeType, std::set<lexer::tokenID>> first,follow;

    // 存储非终结符的first集依赖关系
    std::vector<std::set<nodeType>> firstSetDependencyTree(grmr.symbolSize()); 

    // 存储终结符的直接子节点
    std::map<lexer::tokenID, std::set<nodeType>> tokenRoots; 

    // 存储可空的非终结符
    std::set<nodeType> nullableVns;

    for (auto i : range(grmr.symbolSize())) {
        for (const auto& [producer, prods] : enumerate(grmr.deductions())) {
            for (const auto& prod : prods) {

                auto iter = prod.begin(),
                    ender = prod.end();

                while (iter != ender) 
                {
                    if (iter->isSymbol()) { 
                        // 非终结符，产生依赖
                        // 右指向左
                        firstSetDependencyTree[*iter].insert(producer);

                        //若可空则继续考察右方元素
                        if (!contains(nullableVns, *iter)) {
                            break;
                        }
                    }
                    else if (iter->isToken()) {
                        // 终结符，连接到对应终结符的树根上
                        // 右指向左
                        tokenRoots[*iter].insert(producer);
                        break;
                    }
                    else {
                        break;
                    }

                    ++iter;
                }

                if (iter == ender) {
                    // 发现产生式右边元素均可空（或没有元素）
                    // 则产生式左边元素也可空
                    nullableVns.insert(producer);
                }
            }
        }
    }
    // 在这里将first集依赖树转换为first集
    // 但是先不将null元素加入first集，因为follows集要依赖first集删去null
    auto addTokenIntoSetOfX = [](nodeType root, std::set<lexer::tokenID>& firstOfX)
    {
        firstOfX.insert(root);
    };
    auto dcFirst = dependencyConverter(first, firstSetDependencyTree);
    dcFirst.applyRoots(tokenRoots, addTokenIntoSetOfX);
    // -----------------------------

    tokenRoots.clear(); // 清空终结符根节点

    // 存储非终结符的follow集依赖关系
    std::vector<std::set<nodeType>> followSetDependencyTree(grmr.symbolSize());

    // 非终结符的follow集会依赖first集（不包含null）
    // 这里存储对应first集（...）的直接子节点
    std::map<nodeType, std::set<nodeType>> firstSetRoots;

    // 文章结束符加入起点的follow集
    tokenRoots[lexer::fin].insert(grmr.start());
    for (const auto& [producer, prods] : enumerate(grmr.deductions())) {
        for (const auto& prod : prods) {

            auto iter = prod.begin(),
                ender = prod.end();

            while (iter != ender)
            {
                // 只有非终结符才需要考察右侧元素来生成依赖图
                if (iter->isSymbol()) {
                    // 对产生式右边的每个非终结符Vn
                    // 遍历其右方元素
                    auto subiter = iter;
                    while (++subiter != ender) // 初次自增与迭代时自增合并到循环条件中
                    {
                        if (subiter->isSymbol()) {
                            // 右方为非终结符
                            // Vn依赖其first集（不包含null）
                            // 右指向左
                            firstSetRoots[*subiter].insert(*iter);

                            // 若可空则继续考察右方元素
                            if (!contains(nullableVns, *subiter)) {
                                break;
                            }
                        }
                        else if (subiter->isToken()) {
                            // 右方为终结符
                            // 终结符，连接到对应终结符的树根上
                            // 右指向左
                            tokenRoots[*iter].insert(producer);
                            break;
                        }
                        else {
                            break;
                        }
                    }

                    if (subiter == ender) {
                        // 发现Vn右边元素均可空（或没有元素）
                        // Vn依赖产生式左边元素的follow集合
                        // 左指向右
                        followSetDependencyTree[producer].insert(*iter);
                    }
                }

                ++iter;
            }
        }
    }

    auto dcFollow = dependencyConverter(follow, followSetDependencyTree);
    dcFollow.applyRoots(tokenRoots, addTokenIntoSetOfX);

    auto addFirstOfXIntoFollowOfY = [&first](nodeType rootFirstSetID, std::set<lexer::tokenID>& followOfX)
    {
        auto& rootFirstSet = first[rootFirstSetID];
        followOfX.insert(rootFirstSet.begin(), rootFirstSet.end());
    };
    dcFollow.applyRoots(firstSetRoots, addFirstOfXIntoFollowOfY);

    dcFirst.applyRoot(lexer::null, nullableVns, addTokenIntoSetOfX);

    return std::forward_as_tuple(first, follow);
}
