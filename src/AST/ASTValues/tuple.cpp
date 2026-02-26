//
// Created by migouche on 9/24/2023.
//

#include <iostream>
#include "AST/ASTValues/tuple.h"

Tuple::Tuple(std::vector<std::unique_ptr<ASTValue>> elements): elements(std::move(elements)) {}

void Tuple::print() const {
    std::cout << " [ ";
    for(const auto& c: elements)
    {
        c->print();
        std::cout << ", ";
    }
    std::cout << " ] ";
}

llvm::Value *Tuple::codegen(ParserData&) {
    return nullptr;
}
