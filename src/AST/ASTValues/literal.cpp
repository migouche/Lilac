//
// Created by migouche on 22/09/23.
//

#include "AST/ASTValues/literal.h"
#include <llvm/IR/Constants.h>

#include <utility>
#include <iostream>
#include "parser/parser_data.h"

Literal::Literal(const Token& token): value(token.get_value()) {}

void Literal::print() const {std::cout << value; }

llvm::Value *Literal::codegen(const std::unique_ptr<ParserData>& parser_data) {
    //TODO: WE WILL ONLY USE INTS FOR NOW
    int n = std::stoi(value);
    return llvm::ConstantInt::get(*parser_data->context, llvm::APInt(32, n));
}
