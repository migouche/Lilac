//
// Created by migouche on 9/10/2023.
//

#ifndef LILAC_AST_H
#define LILAC_AST_H

#include <memory>
#include "functiondeclaration.h"

class ASTTree
{
private:
    std::list<std::shared_ptr<FunctionDeclaration>> children = {};
public:
    void add_child(const std::shared_ptr<FunctionDeclaration>&);
    void print() const;
    ASTTree();
    explicit ASTTree(std::list<std::shared_ptr<FunctionDeclaration>>);
};

#endif //LILAC_AST_H
