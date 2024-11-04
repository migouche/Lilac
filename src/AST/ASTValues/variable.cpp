//
// Created by migouche on 04-Nov-24.
//
#include <iostream>
#include "AST/ASTValues/variable.h"

llvm::Value *Variable::codegen(const std::shared_ptr<ParserData> &parser_data) {
    return parser_data->get_value(name);
}

Variable::Variable(const Token& token): name(token.get_value()) {}


void Variable::print() const {
    std::cout << name << std::endl;

}

