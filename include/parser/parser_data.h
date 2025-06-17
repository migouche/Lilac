//
// Created by migouche on 1/4/2024.
//

#ifndef LILAC_PARSER_DATA_H
#define LILAC_PARSER_DATA_H


#include <memory>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <map>
#include <queue>


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

    static const llvm::Function * get_function(const std::string& name);
    llvm::Type* get_primitive(const std::string& name);

    std::string add_block(llvm::Function*);
    llvm::Function* get_block(const std::string& name);
private:
    //std::vector<std::map<std::string, llvm::Value*>> named_values; // will be used as a stack (push and pop back)
    //std::vector<std::map<std::string, llvm::Type*>> named_types; // will be used as a stack (push and pop back)
    std::vector<std::map<std::string, std::tuple<llvm::Value*, llvm::Type*>>> values; // will be used as a stack (push and pop back)
    // when i get this working, i will merge values and types into one map
    std::map<std::string, llvm::Type*> primitives;
    std::map<std::string, llvm::Function*> blocks; // Will store blocks as functions

    size_t block_counter = 0; // used to generate unique block names
};


#endif //LILAC_PARSER_DATA_H
