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
    using TopLevelDeclaration = std::variant<std::shared_ptr<FunctionDeclaration>, std::shared_ptr<ASTDefinition>>;
private:
    std::list<TopLevelDeclaration> children = {};
public:
    void add_child(const std::shared_ptr<FunctionDeclaration>&);
    void add_child(const std::shared_ptr<ASTDefinition>&);
    void print() const;
    ASTTree();
    explicit ASTTree(std::list<TopLevelDeclaration>);
    void codegen(const std::unique_ptr<ParserData>&) const;
};

#endif //LILAC_AST_H
