#ifndef SYNTAXTREE_H
#define SYNTAXTREE_H

#include <memory>
#include <vector>
#include <variant>
#include <functional>

#include "../lexer/lexer.h"
#include "../grammar/grammar.h"

class syntaxTree;
using pSyntaxTree = std::shared_ptr<syntaxTree>;
using pcSyntaxTree = std::shared_ptr<const syntaxTree>;

class syntaxTree
{
public:
    using uniSym = std::variant<
                        const lexer::tokenUnit*,
                        producerType>;

public:
    syntaxTree(uniSym pData)
        :pData(pData)
    {}

    const auto& children() const { return _children; }
    void appendChild(pSyntaxTree ptr) { _children.push_back(ptr); }


    ostreamType& toStream(ostreamType& os, const nameTable& symbolTable, const nameTable& tokenTable) const;
    ostreamType& toStreamImpl(ostreamType& os, const nameTable& symbolTable, const nameTable& tokenTable,size_t depth) const;

    const uniSym& data() const { return pData; }

protected:
    std::vector<pSyntaxTree> _children;
    uniSym pData;
};

void chainSyntaxVisit(pcSyntaxTree, std::function<int(pcSyntaxTree, nodeType)>);
//breaks if _Func returns true
//node set to nodeNotExist means pTree is a token(or null)

#endif // SYNTAXTREE_H
