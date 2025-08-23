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
    // Enter a new scope for block‐local definitions
    parser_data->enter_scope();
    auto& builder = *parser_data->builder;
    llvm::Value* last = nullptr;

    // Codegen each definition
    for (const auto& def : definitions) {
        last = def->codegen(parser_data);
        if (!last) {
            llvm::errs() << "Error in definition: ";
            def->print();
            parser_data->exit_scope();
            return nullptr;
        }
    }

    // Evaluate the tail expression or return the last definition’s value
    llvm::Value* result = tail_expression
        ? tail_expression->codegen(parser_data)
        : last;

    // Exit scope and return
    parser_data->exit_scope();
    return result;
}
