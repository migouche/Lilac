//
// Created by migouche on 4/11/24.
//
#include "compiler/llvm-primitives/llvmops.h"
#include "compiler/compilersettings.h"

std::map<std::string, std::shared_ptr<llvm::Function>> LLVMOps::primitive_functions;

using BinaryOpMethod = llvm::Value* (llvm::IRBuilder<llvm::ConstantFolder, llvm::IRBuilderDefaultInserter>::*)(llvm::Value*, llvm::Value*, const llvm::Twine&);
std::shared_ptr<llvm::Function> create_op(const std::shared_ptr<ParserData> &parser_data, const std::string &name, BinaryOpMethod op) {
    llvm::Type* i32 = llvm::Type::getInt32Ty(*parser_data->context);
    llvm::FunctionType* ft = llvm::FunctionType::get(i32, {i32, i32}, false);

    llvm::Function* func = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, name, *parser_data->module);
    func->setCallingConv(llvm::CallingConv::Fast);

    auto args = func->args();
    auto it = args.begin();
    llvm::Value* a = &(*it++);
    llvm::Value* b = &(*it);

    llvm::BasicBlock* block = llvm::BasicBlock::Create(*parser_data->context, "entry", func);
    parser_data->builder->SetInsertPoint(block);

    llvm::Value* result = (parser_data->builder.get()->*op)(a, b, "result");

    parser_data->builder->CreateRet(result);
    return std::shared_ptr<llvm::Function>(func);
}

std::shared_ptr<llvm::Function> create_add(const std::shared_ptr<ParserData> &parser_data){
    llvm::Type* i32 = llvm::Type::getInt32Ty(*parser_data->context);
    llvm::FunctionType* ft = llvm::FunctionType::get(i32, {i32, i32}, false);

    // add(int, int) -> int
    llvm::Function* add = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "__add", *parser_data->module);
    add->setCallingConv(llvm::CallingConv::Fast);

    auto args = add->args();
    auto it = args.begin();
    llvm::Value* a = &(*it++);
    llvm::Value* b = &(*it);

    llvm::BasicBlock* block = llvm::BasicBlock::Create(*parser_data->context, "entry", add);
    parser_data->builder->SetInsertPoint(block);

    llvm::Value* sum_ret = parser_data->builder->CreateAdd(a, b, "sum_ret");

    parser_data->builder->CreateRet(sum_ret);
    return std::shared_ptr<llvm::Function>(add);
}

void LLVMOps::init(const std::shared_ptr<ParserData> &parser_data) {


    LLVMOps::primitive_functions["add"] = create_op(parser_data, "sum",
                                                    reinterpret_cast<BinaryOpMethod>(&llvm::IRBuilder<>::CreateAdd));

    LLVMOps::primitive_functions["div"] = create_op(parser_data, "div",
                                                    reinterpret_cast<BinaryOpMethod>(&llvm::IRBuilder<>::CreateSDiv));
}

std::shared_ptr<llvm::Function> LLVMOps::find_function(const std::string &name) {
    return primitive_functions[name];
}

