//
// Created by migouche on 9/24/2023.
//

#include <iostream>
#include "AST/ASTValues/tuple.h"

Tuple::Tuple(std::list<std::shared_ptr<ASTValue>> elements): elements(std::move(elements)) {}

void Tuple::print() const {
    std::cout << " [ ";
    for(const auto& c: elements)
    {
        c->print();
        std::cout << ", ";
    }
    std::cout << " ] ";
}

llvm::Value *Tuple::codegen(const std::shared_ptr<ParserData>&) {
    return nullptr;
}
