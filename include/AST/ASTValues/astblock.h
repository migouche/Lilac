//
// Created by migouche on 6/6/25.
//

#ifndef ASTBLOCK_H
#define ASTBLOCK_H
#include "../astdefinition.h"
#include "AST/astvalue.h"

class ASTBlock final : public ASTValue
{ // a block is like in rust, a set of definitions and a tail expression
    public:
    ASTBlock(std::vector<std::shared_ptr<ASTDefinition>>, std::shared_ptr<ASTValue>);
    void print() const override;
    llvm::Value* codegen(const std::unique_ptr<ParserData>&) override;
    private:
    std::vector<std::shared_ptr<ASTDefinition>> definitions;
    std::shared_ptr<ASTValue> tail_expression;
};

#endif //ASTBLOCK_H
