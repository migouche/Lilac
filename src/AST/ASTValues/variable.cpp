//
// Created by migouche on 04-Nov-24.
//
#include <iostream>
#include "AST/ASTValues/variable.h"

llvm::Value *Variable::codegen(ParserData &parser_data) {
    //return parser_data.get_value(name);
    const auto v = parser_data.get_value(name);
    if (!v) {
        std::cerr << "Variable '" << name << "' not found in symbol table." << std::endl;
        return nullptr;
    }

    if (v->getType()->isPointerTy())
    {
        auto* elemTy = parser_data.get_type(name);
        if (!elemTy) {
            std::cerr << "Variable '" << name << "' has no type." << std::endl;
            return nullptr;
        }
        return parser_data.builder->CreateLoad(elemTy, v, "load_" + name);
    }
    return v;
}

Variable::Variable(const Token& token): name(token.get_value()) {}


std::ostream& Variable::print(std::ostream& os) const {
    os << name << std::endl;
    return os;
}

