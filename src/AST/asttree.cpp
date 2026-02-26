//
// Created by migouche on 14/09/23.
//

#include <iostream>
#include <utility>
#include "AST/asttree.h"



void ASTTree::print() const {
    std::cout << "AST Tree:" << std::endl;
    for (const auto& child: children) {
        if (std::holds_alternative<std::unique_ptr<FunctionDeclaration>>(child)) {
            const auto& func = std::get<std::unique_ptr<FunctionDeclaration>>(child);
            func->print();
        } else if (std::holds_alternative<std::unique_ptr<ASTDefinition>>(child)) {
            const auto& def = std::get<std::unique_ptr<ASTDefinition>>(child);
            def->print();
        }
    }
    std::cout << "End of AST Tree." << std::endl;
}

ASTTree::ASTTree(std::vector<TopLevelDeclaration> l) : children(std::move(l)){}

void ASTTree::add_child(std::unique_ptr<FunctionDeclaration> node) {
    children.emplace_back(std::move(node));
}

void ASTTree::add_child(std::unique_ptr<ASTDefinition> node) {
    children.emplace_back(std::move(node));
}

ASTTree::ASTTree(): children(std::vector<TopLevelDeclaration>()) {}

void ASTTree::codegen(ParserData& data) const{
    // first pass, codegen prototypes:
    for (const auto& child: children) {
        if (std::holds_alternative<std::unique_ptr<FunctionDeclaration>>(child)) {
            const auto& func = std::get<std::unique_ptr<FunctionDeclaration>>(child);
            (void)func->prototype_codegen(data);
        }
    }

    // second pass, codegen definitions:
    for (const auto& child: children) {
        if (std::holds_alternative<std::unique_ptr<FunctionDeclaration>>(child)) {
            const auto& func = std::get<std::unique_ptr<FunctionDeclaration>>(child);
            (void)func->codegen(data);
        } else if (std::holds_alternative<std::unique_ptr<ASTDefinition>>(child)) {
            const auto& def = std::get<std::unique_ptr<ASTDefinition>>(child);
            (void)def->codegen(data);
        }
    }
}
