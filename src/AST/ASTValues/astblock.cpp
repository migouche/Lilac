//
// Created by migouche on 6/6/25.
//

#include "AST/ASTValues/astblock.h"

#include <iostream>

ASTBlock::ASTBlock(std::vector<std::shared_ptr<ASTDefinition>> definitions, std::shared_ptr<ASTValue> tail_expression):
    definitions(std::move(definitions)), tail_expression(std::move(tail_expression)) {}

void ASTBlock::print() const
{
    std::cout << '\n';
    for (const auto& def : definitions) {
        def->print();
    }
    std::cout << "\n Tail: ";
    tail_expression->print();
}

llvm::Value* ASTBlock::codegen(const std::unique_ptr<ParserData>& parser_data)
{
    llvm::IRBuilder<>& builder = *parser_data->builder;
    llvm::Value* last = nullptr;

    for (const auto& def : definitions) {
        last = def->codegen(parser_data);
        if (!last) {
            std::cerr << "Error in definition: ";
            def->print();
            return nullptr;
        }
    }



    return tail_expression ? tail_expression->codegen(parser_data) : last;
    // if no tail expression, return the last definition's value
}
