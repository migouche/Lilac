//
// Created by migouche on 01-Nov-24.
//

#include "parser/parser_data.h"
#include "compiler/llvm-primitives/llvmops.h"

#include <iostream>

ParserData::ParserData() :  context(std::make_unique<llvm::LLVMContext>()),
                            module(std::make_unique<llvm::Module>("jit", *context)),
                            builder(std::make_unique<llvm::IRBuilder<>>(*context))
{
    primitives = {
            {"int", llvm::Type::getInt32Ty(*context)},
            {"float", llvm::Type::getFloatTy(*context)},
            {"double", llvm::Type::getDoubleTy(*context)},
            {"void", llvm::Type::getVoidTy(*context)}
    };

}



std::shared_ptr<llvm::Function> ParserData::get_function(const std::string &name) {
    return LLVMOps::find_function(name);
}

void ParserData::enter_scope() {
    named_values.emplace_back();
}

void ParserData::exit_scope() {
    named_values.pop_back();
}

void ParserData::add_value(const std::string &name, llvm::Value *value) {
    named_values.back()[name] = value;
}

llvm::Value* ParserData::get_value(const std::string &name) {
    for (auto scope: named_values){
        if(scope.find(name) != scope.end())
            return scope[name];
    }
    return nullptr;
}

llvm::Type *ParserData::get_primitive(const std::string &name) {
    return primitives[name];
}
