//
// Created by migouche on 1/4/2024.
//

#ifndef LILAC_PARSER_DATA_H
#define LILAC_PARSER_DATA_H


#include <memory>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <unordered_map>
#include <queue>
#include <vector>
#include <string>

#include "AST/enumdeclaration.h"

struct ParserData{
public:
     std::unique_ptr<llvm::LLVMContext> context;
     std::unique_ptr<llvm::IRBuilder<>> builder;
     std::unique_ptr<llvm::Module> module;

    ParserData();


    void enter_scope();
    void exit_scope();
    [[nodiscard]] llvm::Value* get_value(const std::string& name) const;
    llvm::Type* get_type(const std::string& name) const;

    void add_value(const std::string& name, llvm::Value* value, llvm::Type* type, bool global);

    void register_enum(const std::string& name, const std::vector<EnumVariant>& variants);
    const std::vector<EnumVariant>* get_enum(const std::string& name) const;
    std::string get_enum_for_variant(const std::string& variant_name) const;

    static const llvm::Function * get_function(const std::string& name);
    llvm::Type* get_primitive(const std::string& name);

    std::string add_block(llvm::Function*);
    llvm::Function* get_block(const std::string& name);
private:
    std::unordered_map<std::string, std::vector<EnumVariant>> enums;
    std::unordered_map<std::string, std::string> variant_to_enum;
    std::vector<std::unordered_map<std::string, std::tuple<llvm::Value*, llvm::Type*>>> values; // will be used as a stack (push and pop back)
    // when i get this working, i will merge values and types into one map
    std::unordered_map<std::string, llvm::Type*> primitives;
    std::unordered_map<std::string, llvm::Function*> blocks; // Will store blocks as functions

    size_t block_counter = 0; // used to generate unique block names
};


#endif //LILAC_PARSER_DATA_H
