//
// Created by migouche on 6/6/25.
//

#include "AST/ASTValues/astblock.h"

#include <iostream>

ASTBlock::ASTBlock(std::vector<std::unique_ptr<ASTDefinition>> definitions, std::unique_ptr<ASTValue> tail_expression):
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



llvm::Value* ASTBlock::codegen(ParserData& parser_data)
{
    parser_data.enter_scope();
    llvm::Value* last = nullptr;
    for (const auto& def : definitions) {
        if (!def->codegen(parser_data)) {
            std::cerr << "Failed to codegen definition inside block." << std::endl;
            parser_data.exit_scope();
            return nullptr;
        }
    }
    llvm::Value* result = nullptr;
    if (tail_expression) {
        result = tail_expression->codegen(parser_data);
    }
    parser_data.exit_scope();
    return result;
}
