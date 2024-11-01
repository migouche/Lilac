//
// Created by migouche on 9/10/2023.
//

#ifndef LILAC_AST_H
#define LILAC_AST_H

#include <memory>
#include "functiondeclaration.h"
#include "parser/parser_data.h"

class ASTTree
{
private:
    std::list<std::shared_ptr<FunctionDeclaration>> children = {};
public:
    void add_child(const std::shared_ptr<FunctionDeclaration>&);
    void print() const;
    ASTTree();
    explicit ASTTree(std::list<std::shared_ptr<FunctionDeclaration>>);
    void codegen(const std::shared_ptr<ParserData>&) const;
};

#endif //LILAC_AST_H
