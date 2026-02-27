//
// Created by migouche on 31-May-25.
//

#include "AST/ASTValues/astif.h"
#include "parser/parser_data.h"
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>

ASTIf::ASTIf(std::unique_ptr<ASTValue> condition, std::unique_ptr<ASTValue> then_branch,
    std::unique_ptr<ASTValue> else_branch) :
condition(std::move(condition)), then_branch(std::move(then_branch)), else_branch(std::move(else_branch)) {}


llvm::Value *ASTIf::codegen(ParserData &parser_data) {
    llvm::IRBuilder<> &builder = *parser_data.builder;
    llvm::LLVMContext &ctx = *parser_data.context;

    // Emit condition
    llvm::Value *condV = condition->codegen(parser_data);
    if (!condV) return nullptr;

    // Normalize to i1
    llvm::Value *condBool;
    if (condV->getType()->isIntegerTy(1)) {
        condBool = condV;
    } else {
        auto zero = llvm::ConstantInt::get(condV->getType(), 0);
        condBool = builder.CreateICmpNE(condV, zero, "ifcond");
    }

    // Parent function
    llvm::Function *parent = builder.GetInsertBlock()->getParent();

    // Create blocks — attach them directly to `parent`
    llvm::BasicBlock *thenBB  = llvm::BasicBlock::Create(ctx, "then", parent);
    llvm::BasicBlock *elseBB  = llvm::BasicBlock::Create(ctx, "else", parent);
    llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(ctx, "ifcont", parent);

    // Conditional branch
    builder.CreateCondBr(condBool, thenBB, elseBB);

    // Then block
    builder.SetInsertPoint(thenBB);
    llvm::Value *thenV = then_branch->codegen(parser_data);
    if (!thenV) return nullptr;
    builder.CreateBr(mergeBB);
    thenBB = builder.GetInsertBlock(); // In case codegen inserted new blocks

    // Else block
    builder.SetInsertPoint(elseBB);
    llvm::Value *elseV = else_branch->codegen(parser_data);
    if (!elseV) return nullptr;
    builder.CreateBr(mergeBB);
    elseBB = builder.GetInsertBlock();

    // Merge block
    builder.SetInsertPoint(mergeBB);

    // PHI node
    if (thenV->getType() != elseV->getType()) return nullptr;
    auto *phi = builder.CreatePHI(thenV->getType(), 2, "iftmp");
    phi->addIncoming(thenV, thenBB);
    phi->addIncoming(elseV, elseBB);
    return phi;
}

std::ostream& ASTIf::print(std::ostream& os) const {
    os << "if ";
    condition->print(os);
    os << " then ";
    then_branch->print(os);
    os << " else ";
    else_branch->print(os);
    return os;
}
