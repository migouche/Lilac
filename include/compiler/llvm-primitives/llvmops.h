//
// Created by migouche on 3/11/24.
//

#ifndef LILAC_LLVMOPS_H
#define LILAC_LLVMOPS_H

#include <map>
#include <string>
#include <llvm/IR/Function.h>
#include "parser/parser_data.h"

class LLVMOps {
private:
    static std::map<std::string, llvm::Function*> primitive_functions;
public:
    static void init(const std::unique_ptr<ParserData> &parser_data);
    static llvm::Function * find_function(const std::string &name);

    //static void call_function(const std::string &name, const std::unique_ptr<ParserData> &parser_data);
};
#endif //LILAC_LLVMOPS_H
