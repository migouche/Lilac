//
// Created by migouche on 14/09/23.
//

#include <iostream>
#include <utility>
#include "AST/asttree.h"

void ASTTree::print() const {
    for (auto& child: children) {
        child->print();
        std::cout << std::endl;
    }
}

ASTTree::ASTTree(std::list<std::shared_ptr<FunctionDeclaration>> l) : children(std::move(l)){}

void ASTTree::add_child(const std::shared_ptr<FunctionDeclaration>& node) {
    children.push_back(node);
}

ASTTree::ASTTree(): children(std::list<std::shared_ptr<FunctionDeclaration>>()) {}

void ASTTree::codegen(const std::unique_ptr<ParserData>& data) const{
    for (auto const& c: children)
        (void)c->prototype_codegen(data); // if we do this first then double-recursion is fixed

    for(auto const& c: children)
        (void)c->codegen(data); // maybe??
}
