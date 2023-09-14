//
// Created by migouche on 14/09/23.
//

#include <iostream>
#include "parser/AST/asttree.h"

void ASTTree::print() const {
    for (auto& child: children) {
        child->print();
        std::cout << std::endl;
    }
}

void ASTTree::add_child(const std::shared_ptr<ASTNode>& node) {
    children.push_back(node);
}