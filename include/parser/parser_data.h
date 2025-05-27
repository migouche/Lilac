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
    void add_value(const std::string& name, llvm::Value* value);

     static const llvm::Function * get_function(const std::string& name);
    llvm::Type* get_primitive(const std::string& name);
private:
    std::vector<std::map<std::string, llvm::Value*>> named_values; // will be used as a stack (push and pop back)
    std::map<std::string, llvm::Type*> primitives;
};



#endif //LILAC_PARSER_DATA_H
