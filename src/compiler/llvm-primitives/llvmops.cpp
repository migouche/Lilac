//
// Created by migouche on 4/11/24.
//
#include "compiler/llvm-primitives/llvmops.h"

std::map<std::string, llvm::Function*> LLVMOps::primitive_functions;

using BinaryOpMethod = llvm::Value* (llvm::IRBuilder<llvm::ConstantFolder, llvm::IRBuilderDefaultInserter>::*)(llvm::Value*, llvm::Value*, const llvm::Twine&);

llvm::Function* create_op(ParserData &parser_data, const std::string &name, BinaryOpMethod op, bool is_comparison = false) {
    llvm::Type* i32 = llvm::Type::getInt32Ty(*parser_data.context);
    llvm::Type* retType = is_comparison ? llvm::Type::getInt1Ty(*parser_data.context) : i32;
    llvm::FunctionType* ft = llvm::FunctionType::get(retType, {i32, i32}, false);

    llvm::Function* func = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, name, *parser_data.module);
    
    const auto args = func->args();
    auto it = args.begin();
    it->setName("lhs");
    llvm::Value* a = (it++);
    it->setName("rhs");
    llvm::Value* b = (it);

    llvm::BasicBlock* block = llvm::BasicBlock::Create(*parser_data.context, "entry", func);
    parser_data.builder->SetInsertPoint(block);

    llvm::Value* result = (parser_data.builder.get()->*op)(a, b, "result");

    parser_data.builder->CreateRet(result);
    return func;
}


void LLVMOps::init(ParserData &parser_data) {


    primitive_functions["add"] = create_op(parser_data, "sum",
                                                    reinterpret_cast<BinaryOpMethod>(&llvm::IRBuilder<>::CreateAdd));

    primitive_functions["sub"] = create_op(parser_data, "sub",
                                                    reinterpret_cast<BinaryOpMethod>(&llvm::IRBuilder<>::CreateSub));

    primitive_functions["div"] = create_op(parser_data, "div",
                                                    reinterpret_cast<BinaryOpMethod>(&llvm::IRBuilder<>::CreateSDiv));

    primitive_functions["mul"] = create_op(parser_data, "mul",
                                                    reinterpret_cast<BinaryOpMethod>(&llvm::IRBuilder<>::CreateMul));

    primitive_functions["eq"] = create_op(parser_data, "eq",
                                                    &llvm::IRBuilder<>::CreateICmpEQ, true);

    primitive_functions["gt"] = create_op(parser_data, "gt",
                                                    &llvm::IRBuilder<>::CreateICmpSGT, true);

    primitive_functions["lt"] = create_op(parser_data, "lt",
                                                    &llvm::IRBuilder<>::CreateICmpSLT, true);

    primitive_functions["ge"] = create_op(parser_data, "ge",
                                                    &llvm::IRBuilder<>::CreateICmpSGE, true);

    primitive_functions["le"] = create_op(parser_data, "le",
                                                    &llvm::IRBuilder<>::CreateICmpSLE, true);
}

llvm::Function * LLVMOps::find_function(const std::string &name) {
    return primitive_functions[name];
}

