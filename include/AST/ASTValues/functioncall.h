//
// Created by migouche on 20/09/23.
//

#ifndef LILAC_FUNCTIONCALL_H
#define LILAC_FUNCTIONCALL_H

#include <vector>
#include <memory>
#include "AST/astvalue.h"

class FunctionCall final : public ASTValue
{
private:
    std::string name;
    std::vector<std::unique_ptr<ASTValue>> arguments;
public:
    FunctionCall(std::string name, std::vector<std::unique_ptr<ASTValue>> arguments);

    void print() const override;
    llvm::Value * codegen(ParserData&) override;
};

#endif //LILAC_FUNCTIONCALL_H
