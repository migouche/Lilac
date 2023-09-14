//
// Created by migouche on 9/10/2023.
//

#ifndef LILAC_AST_H
#define LILAC_AST_H

#include <memory>
#include "parser/AST/functiondeclaration.h"

class ASTTree
{
private:
    std::list<std::shared_ptr<ASTNode>> children = {};
public:
    void add_child(const std::shared_ptr<ASTNode>&);
    void print() const;
};

#endif //LILAC_AST_H
