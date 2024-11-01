//
// Created by migouche on 22/09/23.
//

#include "AST/ASTValues/literal.h"
#include <llvm/IR/Constants.h>

#include <utility>
#include <iostream>
#include "parser/parser_data.h"

Literal::Literal(Token token): token(std::move(token)) {}

void Literal::print() const {std::cout << token.get_value(); }

llvm::Value *Literal::codegen(const std::shared_ptr<ParserData>& parser_data) {
    //TODO: WE WILL ONLY USE INTS FOR NOW
    return llvm::ConstantInt::get(*parser_data->context, llvm::APInt(32, 4));
}
