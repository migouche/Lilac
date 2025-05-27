//
// Created by migouche on 4/11/24.
//
#include "compiler/llvm-primitives/llvmops.h"

std::map<std::string, llvm::Function*> LLVMOps::primitive_functions;

using BinaryOpMethod = llvm::Value* (llvm::IRBuilder<llvm::ConstantFolder, llvm::IRBuilderDefaultInserter>::*)(llvm::Value*, llvm::Value*, const llvm::Twine&);

llvm::Function* create_op(const std::unique_ptr<ParserData> &parser_data, const std::string &name, BinaryOpMethod op) {
    llvm::Type* i32 = llvm::Type::getInt32Ty(*parser_data->context);
    llvm::FunctionType* ft = llvm::FunctionType::get(i32, {i32, i32}, false);

    llvm::Function* func = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, name, *parser_data->module);
    //func->setCallingConv(llvm::CallingConv::Fast);

    const auto args = func->args();
    auto it = args.begin();
    llvm::Value* a = (it++);
    llvm::Value* b = (it);

    llvm::BasicBlock* block = llvm::BasicBlock::Create(*parser_data->context, "entry", func);
    parser_data->builder->SetInsertPoint(block);

    llvm::Value* result = (parser_data->builder.get()->*op)(a, b, "result");

    parser_data->builder->CreateRet(result);
    return func;
}


void LLVMOps::init(const std::unique_ptr<ParserData> &parser_data) {


    LLVMOps::primitive_functions["add"] = create_op(parser_data, "sum",
                                                    reinterpret_cast<BinaryOpMethod>(&llvm::IRBuilder<>::CreateAdd));

    LLVMOps::primitive_functions["sub"] = create_op(parser_data, "sub",
                                                    reinterpret_cast<BinaryOpMethod>(&llvm::IRBuilder<>::CreateSub));

    LLVMOps::primitive_functions["div"] = create_op(parser_data, "div",
                                                    reinterpret_cast<BinaryOpMethod>(&llvm::IRBuilder<>::CreateSDiv));
}

llvm::Function * LLVMOps::find_function(const std::string &name) {
    return primitive_functions[name];
}

