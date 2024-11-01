//
// Created by migouche on 1/4/2024.
//

#ifndef LILAC_PARSER_DATA_H
#define LILAC_PARSER_DATA_H


#include <memory>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <map>



struct ParserData{
public:
     std::unique_ptr<llvm::LLVMContext> context;
     std::unique_ptr<llvm::IRBuilder<>> builder;
     std::unique_ptr<llvm::Module> module;
     std::map<std::string, llvm::Value*> named_values;

    ParserData();

    ParserData(const ParserData&) = delete;
    const std::shared_ptr<ParserData>& operator=(const ParserData&) = delete;

    ParserData(ParserData&&) = default;
    ParserData& operator=(ParserData&&) = default;
};



#endif //LILAC_PARSER_DATA_H
