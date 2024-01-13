//
// Created by migouche on 1/4/2024.
//

#ifndef LILAC_PARSER_DATA_H
#define LILAC_PARSER_DATA_H


#include <memory>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <map>

namespace parser_data
{
    static std::unique_ptr<llvm::LLVMContext> context;
    static std::unique_ptr<llvm::IRBuilder<>> builder =
            std::make_unique<llvm::IRBuilder<>>(*context);
    static std::unique_ptr<llvm::Module> module;
    static std::map<std::string, llvm::Value*> named_values;
}


#endif //LILAC_PARSER_DATA_H
