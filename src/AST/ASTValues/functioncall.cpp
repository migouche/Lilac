//
// Created by migouche on 20/09/23.
//

#include <iostream>
#include <utility>
#include "parser/parser_data.h"
#include "AST/ASTValues/functioncall.h"

void FunctionCall::print() const {
    std::cout << name << " ( ";
    for(const auto& arg: arguments)
    {
        arg->print();
        std::cout << " , ";
    }
    std::cout << ") ";
}

FunctionCall::FunctionCall(std::string  name, std::list<std::shared_ptr<ASTValue>> arguments):
    name(std::move(name)), arguments(std::move(arguments))
{
}

llvm::Value *FunctionCall::codegen() {
    llvm::Function *callee = parser_data::module->getFunction(name);
    if(!callee) {
        std::cerr << "Unknown reference to: " << name << std::endl;
        return nullptr;
    }
    if(callee->arg_size() != arguments.size())
    {
        std::cerr << "Incorrect number of arguments, expected " << callee->arg_size()
        << ", got " << arguments.size() << std::endl;
    }

    std::vector<llvm::Value*> args_v;
    for(const auto& arg: arguments)
    {
        args_v.push_back(arg->codegen());
        if(!args_v.back())
            return nullptr;
    }
    return parser_data::builder->CreateCall(callee, args_v, "calltmp");
}
