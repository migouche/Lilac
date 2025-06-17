//
// Created by migouche on 01-Nov-24.
//

#include "parser/parser_data.h"
#include "compiler/llvm-primitives/llvmops.h"

#include <iostream>
#include <llvm/IR/Module.h>

ParserData::ParserData() :  context(std::make_unique<llvm::LLVMContext>()),
                            builder(std::make_unique<llvm::IRBuilder<>>(*context)),
                            module(std::make_unique<llvm::Module>("jit", *context))
{
    primitives = {
            {"int", llvm::Type::getInt32Ty(*context)},
            {"float", llvm::Type::getFloatTy(*context)},
            {"double", llvm::Type::getDoubleTy(*context)},
            {"void", llvm::Type::getVoidTy(*context)},
            {"bool", llvm::Type::getInt1Ty(*context)}
    };

    blocks = {};
    named_values.emplace_back();
    named_types.emplace_back();
}



const llvm::Function * ParserData::get_function(const std::string &name) {
    return LLVMOps::find_function(name);
}

void ParserData::enter_scope() {
    named_values.emplace_back();
    named_types.emplace_back();
}

void ParserData::exit_scope() {
    named_values.pop_back();
    named_types.pop_back();
}

void ParserData::add_value(const std::string &name, llvm::Value *value, llvm::Type *type, bool global) {
    named_values.back()[name] = value;
    if (global)
        named_types.front()[name] = type; // global types are stored in the first scope
    else
        named_types.back()[name] = type;
}

llvm::Value* ParserData::get_value(const std::string &name) const {
    for (auto scope: named_values){
        if(scope.contains(name))
            return scope[name];
    }
    return nullptr;
}

llvm::Type* ParserData::get_type(const std::string &name) const
{
    for (auto scope: named_types){
        if(scope.contains(name))
            return scope[name];
    }
    return nullptr;
}

llvm::Type *ParserData::get_primitive(const std::string &name) {
    return primitives[name];
}

std::string ParserData::add_block(llvm::Function* func)
{
    std::string block_name = "block_" + std::to_string(block_counter++);
    blocks[block_name] = func;
    return block_name;
}

llvm::Function* ParserData::get_block(const std::string& name)
{
    return blocks[name];
}
