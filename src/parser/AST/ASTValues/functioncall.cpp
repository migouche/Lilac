//
// Created by migouche on 20/09/23.
//

#include <iostream>
#include <utility>
#include "parser/AST/ASTValues/functioncall.h"

void FunctionCall::print() const {
    std::cout << name << " ( ";
    for(const auto& arg: children)
    {
        arg->print();
        std::cout << " , ";
    }
    std::cout << ") ";
}

FunctionCall::FunctionCall(std::string  name, std::list<std::shared_ptr<ASTNode>> arguments):
    name(std::move(name))
{
    children = std::move(arguments);
}
