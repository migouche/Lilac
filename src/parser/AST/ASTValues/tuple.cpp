//
// Created by migouche on 9/24/2023.
//

#include <iostream>
#include "parser/AST/ASTValues/tuple.h"

Tuple::Tuple(std::list<std::shared_ptr<ASTNode>> elements) {
    children = std::move(elements);
}

void Tuple::print() const {
    std::cout << " [ ";
    for(const auto& c: children)
    {
        c->print();
        std::cout << ", ";
    }
    std::cout << " ] ";
}