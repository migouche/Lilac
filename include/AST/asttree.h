//
// Created by migouche on 9/10/2023.
//

#ifndef LILAC_AST_H
#define LILAC_AST_H

#include <memory>
#include <vector>
#include <variant>
#include "functiondeclaration.h"
#include "parser/parser_data.h"


class ASTTree
{
    using TopLevelDeclaration = std::variant<std::unique_ptr<FunctionDeclaration>, std::unique_ptr<ASTDefinition>>;
private:
    std::vector<TopLevelDeclaration> children = {};
public:
    void add_child(std::unique_ptr<FunctionDeclaration>);
    void add_child(std::unique_ptr<ASTDefinition>);
    void print() const;
    ASTTree();
    explicit ASTTree(std::vector<TopLevelDeclaration>);
    void codegen(ParserData&) const;
};

#endif //LILAC_AST_H
