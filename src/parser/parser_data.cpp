//
// Created by migouche on 01-Nov-24.
//

#include "parser/parser_data.h"

#include <iostream>

ParserData::ParserData() :  context(std::make_unique<llvm::LLVMContext>()),
                            module(std::make_unique<llvm::Module>("jit", *context)),
                            builder(std::make_unique<llvm::IRBuilder<>>(*context))
{
    std::cout << "Context initialized: " << (context != nullptr) << std::endl;
    std::cout << "Module initialized: " << (module != nullptr) << std::endl;
    std::cout << "Builder initialized: " << (builder != nullptr) << std::endl;
}