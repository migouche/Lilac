//
// Created by migouche on 01-Nov-24.
//

#include "parser/parser_data.h"
#include "compiler/llvm-primitives/llvmops.h"

#include <iostream>
#include <ranges>
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
    enter_scope(); // start with a global scope
}



const llvm::Function * ParserData::get_function(const std::string &name) {
    return LLVMOps::find_function(name);
}

void ParserData::enter_scope() {
    values.emplace_back();
}

void ParserData::exit_scope() {
    if (values.empty()) {
        std::cerr << "Error: trying to exit scope when no scopes are present." << std::endl;
        return;
    }
    values.pop_back();
}

void ParserData::add_value(const std::string &name, llvm::Value *value, llvm::Type *type, const bool global) {
    /*named_values.back()[name] = value;
    if (global)
        named_types.front()[name] = type; // global types are stored in the first scope
    else
        named_types.back()[name] = type;*/

    if (global)
        values.front()[name] = std::make_tuple(value, type); // global values are stored in the first scope
    else
        values.back()[name] = std::make_tuple(value, type);
}

void ParserData::register_enum(const std::string& name, const std::vector<EnumVariant>& variants) {
    enums[name] = variants;
    for (const auto& variant : variants) {
        variant_to_enum[variant.name] = name;
    }
}

const std::vector<EnumVariant>* ParserData::get_enum(const std::string& name) const {
    auto it = enums.find(name);
    if (it != enums.end()) {
        return &it->second;
    }
    return nullptr;
}

std::string ParserData::get_enum_for_variant(const std::string& variant_name) const {
    auto it = variant_to_enum.find(variant_name);
    if (it != variant_to_enum.end()) {
        return it->second;
    }
    return "";
}

llvm::Value* ParserData::get_value(const std::string &name) const {
    for (const auto& scope: std::ranges::reverse_view(values)){
        if(auto it = scope.find(name); it != scope.end())
            return std::get<llvm::Value*>(it->second);
    }
    return nullptr;
}

llvm::Type* ParserData::get_type(const std::string &name) const
{
    for (const auto& scope: std::ranges::reverse_view(values)){
        if(auto it = scope.find(name); it != scope.end())
            return std::get<llvm::Type*>(it->second);
    }
    return nullptr;
}

llvm::Type* ParserData::get_primitive(const std::string &name) {
    auto it = primitives.find(name);
    if (it != primitives.end()) {
        return it->second;
    }
    // Check if it's an enum
    if (enums.find(name) != enums.end()) {
        std::vector<llvm::Type*> elements = {
            llvm::Type::getInt32Ty(*context), // tag
            llvm::Type::getInt32Ty(*context)  // payload (int/float representation)
        };
        return llvm::StructType::get(*context, elements);
    }
    std::cerr << "Warning: type not found '" << name << "'" << std::endl;
    return nullptr;
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
