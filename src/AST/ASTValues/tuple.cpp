//
// Created by migouche on 9/24/2023.
//

#include <iostream>
#include "AST/ASTValues/tuple.h"

Tuple::Tuple(std::vector<std::unique_ptr<ASTValue>> elements): elements(std::move(elements)) {}

std::ostream& Tuple::print(std::ostream& os) const {
    os << " [ ";
    for(const auto& c: elements)
    {
        c->print(os);
        os << ", ";
    }
    os << " ] ";
    return os;
}

llvm::Value *Tuple::codegen(ParserData&) {
    return nullptr;
}
